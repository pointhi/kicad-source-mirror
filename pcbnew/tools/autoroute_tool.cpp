#include "autoroute_tool.h"
#include "pcb_actions.h"
#include "selection_tool.h"
#include "tool/tool_manager.h"

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
{

}


AUTOROUTE_TOOL::~AUTOROUTE_TOOL()
{}


void AUTOROUTE_TOOL::Reset( RESET_REASON aReason )
{
}

#include "selection_tool.h"

bool AUTOROUTE_TOOL::Init()
{
    m_frame = getEditFrame<PCB_EDIT_FRAME>();

    auto activeToolCondition = [ this ] ( const SELECTION& aSel ) {
        return ( m_frame->GetToolId() != ID_NO_TOOL_SELECTED );
    };

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

int AUTOROUTE_TOOL::routeAll( const TOOL_EVENT& aEvent ) {
    std::cout << "routeALL" << std::endl;
    return 0;
}

int AUTOROUTE_TOOL::routeTrack( const TOOL_EVENT& aEvent ) {
    std::cout << "routeTrack" << std::endl;
    return 0;
}

void AUTOROUTE_TOOL::setTransitions()
{
    Go( &AUTOROUTE_TOOL::routeAll, PCB_ACTIONS::routeAll.MakeEvent() );
    Go( &AUTOROUTE_TOOL::routeTrack, PCB_ACTIONS::routeTrack.MakeEvent() );
}