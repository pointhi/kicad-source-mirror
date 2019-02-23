#include "autoroute_tool.h"
#include "pcb_actions.h"
#include "selection_tool.h"
#include "tool/tool_manager.h"
#include "class_module.h"

#include "autorouter/c-pcb/router.h"

TOOL_ACTION PCB_ACTIONS::routeAll(
        "pcbnew.Autoroute.RouteAll",
        AS_GLOBAL, MD_CTRL + MD_SHIFT + int( 'Q' ),
        _( "Autoroute Board" ), _( "Autoroute Board" ) );

TOOL_ACTION PCB_ACTIONS::routeTrack(
        "pcbnew.Autoroute.RouteTrack",
        AS_GLOBAL, MD_CTRL + MD_SHIFT + int( 'W' ),
        _( "Autoroute Track" ), _( "Autoroute Track to next pad" ) );


/*
 * AUTOROUTE_TOOL implementation
 */

AUTOROUTE_TOOL::AUTOROUTE_TOOL() :
        PCB_TOOL( "pcbnew.Autoroute" )
        , m_frame( nullptr )
        , m_board( nullptr )
{

}


AUTOROUTE_TOOL::~AUTOROUTE_TOOL()
{}


void AUTOROUTE_TOOL::Reset( RESET_REASON aReason )
{
    m_board = getModel<BOARD>();
    m_frame = getEditFrame<PCB_EDIT_FRAME>();
}

#include "selection_tool.h"

bool AUTOROUTE_TOOL::Init()
{
    /*auto activeToolCondition = [ this ] ( const SELECTION& aSel ) {
        return ( m_frame->GetToolId() != ID_NO_TOOL_SELECTED );
    };*/

    auto inactiveStateCondition = [ this ] ( const SELECTION& aSel ) {
        return ( m_frame->GetToolId() == ID_NO_TOOL_SELECTED && aSel.Size() == 0 );
    };

    SELECTION_TOOL* selTool = m_toolMgr->GetTool<SELECTION_TOOL>();

    if( selTool )
    {
        auto& toolMenu = selTool->GetToolMenu();
        auto& menu = toolMenu.GetMenu();

        menu.AddSeparator( inactiveStateCondition );

        // add our own tool's action
        menu.AddItem( PCB_ACTIONS::routeAll, inactiveStateCondition );
        // add the PCB_EDITOR_CONTROL's zone unfill all action
        menu.AddItem( PCB_ACTIONS::routeTrack, inactiveStateCondition );
    }

    return true;
}

#include <iostream>

//generate range of routing vectors
nodes gen_vectors(int vec_range, int x_range, int y_range)
{
    auto yield = nodes{};
    for (auto y = y_range; y >= -y_range; --y)
    {
        for (auto x = x_range; x >= -x_range; --x)
        {
            auto p = point_2d{double(x), double(y)};
            if (length_2d(p) > 0.1 && length_2d(p) <= double(vec_range))
            {
                yield.push_back(node{x, y, 0});
            }
        }
    }
    return yield;
}

//read, (width height depth)
pcb::dims dimensions()
{
    auto t = pcb::dims{};
    t.m_width = 100.0; // TODO: in grid points?
    t.m_height = 100.0;
    t.m_depth = 2.0;
    return t;
}

int AUTOROUTE_TOOL::routeAll( const TOOL_EVENT& aEvent ) {
    std::cerr << "routeALL" << std::endl;

    //create flooding and backtracking vectors
    auto flood_range = 2;
    auto flood_range_x_even_layer = 1;
    auto flood_range_y_odd_layer = 1;
    auto path_range = flood_range + 0;
    auto path_range_x_even_layer = flood_range_x_even_layer + 0;
    auto path_range_y_odd_layer = flood_range_y_odd_layer + 0;

    auto routing_flood_vectorss = nodess{
            gen_vectors(flood_range, flood_range_x_even_layer, flood_range),
            gen_vectors(flood_range, flood_range, flood_range_y_odd_layer)};

    auto routing_path_vectorss = nodess{
            gen_vectors(path_range, path_range_x_even_layer, path_range),
            gen_vectors(path_range, path_range, path_range_y_odd_layer)};

    //create pcb object and populate with tracks from input
    pcb::dims dims;
    dims.m_width = m_board->GetBoundingBox().GetWidth() / IU_PER_MM;
    dims.m_height = m_board->GetBoundingBox().GetHeight() / IU_PER_MM;
    dims.m_depth = 2.0;

    int x_offset = m_board->GetBoundingBox().GetLeft();
    int y_offset = m_board->GetBoundingBox().GetTop();

    auto current_pcb = pcb(dims, routing_flood_vectorss, routing_path_vectorss, 1, 0, 1, 8);

    for(auto it = m_board->BeginNets(); it != m_board->EndNets(); it++) {
        track track;
        track.m_id = std::to_string(it->GetNet());
        track.m_track_radius = it->GetTrackWidth() / IU_PER_MM / 2.;
        track.m_via_radius = it->GetViaSize() / IU_PER_MM / 2.;
        track.m_gap = it->GetClearance(nullptr) / IU_PER_MM / 2.;

        for( MODULE* module = m_board->m_Modules; module; module = module->Next() ) {
            for( D_PAD* dpad : module->Pads() ) {
                if((*it) == dpad->GetNet()) {
                    // TODO: detect actual CU-Layers used
                    for (int layer = 0; layer < 2; layer++) {
                        pad p;
                        p.m_radius = dpad->GetSize().x / IU_PER_MM / 2.; // TODO: improve pad shapes
                        p.m_gap = it->GetClearance(nullptr) / IU_PER_MM / 2.;
                        p.m_pos = {(dpad->GetPosition().x - x_offset) / IU_PER_MM,
                                   (dpad->GetPosition().y - y_offset) / IU_PER_MM, (double)layer};
                        track.m_pads.push_back(p);
                    }
                }
            }
        }

        //track.m_paths = read_paths(in);

        current_pcb.add_track(track);
    }

    //run number of samples of solution and pick best one
    current_pcb.print_pcb();
    auto best_cost = 1000000000;
    auto best_pcb = current_pcb;
    for (auto i = 0; i < 1; ++i)
    {
        if (!current_pcb.route(30.0)) // seconds
        {
            current_pcb.increase_quantization();
            continue;
        }
        auto cost = current_pcb.cost();
        if (cost <= best_cost)
        {
            best_cost = cost;
            best_pcb = current_pcb;
            best_pcb.print_stats();
        }
    }
    best_pcb.print_netlist();
    best_pcb.print_stats();

    return 0;
}

int AUTOROUTE_TOOL::routeTrack( const TOOL_EVENT& aEvent ) {
    std::cerr << "routeTrack" << std::endl;
    return 0;
}

void AUTOROUTE_TOOL::setTransitions()
{
    Go( &AUTOROUTE_TOOL::routeAll, PCB_ACTIONS::routeAll.MakeEvent() );
    Go( &AUTOROUTE_TOOL::routeTrack, PCB_ACTIONS::routeTrack.MakeEvent() );
}