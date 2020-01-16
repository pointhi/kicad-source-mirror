/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019 Thomas Pointhuber <thomas.pointhuber@gmx.at>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#include "altium_pcb.h"
#include "altium_parser.h"

#include <class_board.h>
#include <class_drawsegment.h>
#include <class_pcb_text.h>

#include <class_edge_mod.h>
#include <class_text_mod.h>

#include <compoundfilereader.h>
#include <utf.h>


const CFB::COMPOUND_FILE_ENTRY* FindStream(const CFB::CompoundFileReader& reader, const char* streamName)
{
    const CFB::COMPOUND_FILE_ENTRY* ret = nullptr;
    reader.EnumFiles(reader.GetRootEntry(), -1,
                     [&](const CFB::COMPOUND_FILE_ENTRY* entry, const CFB::utf16string& u16dir, int level)->void
                     {
                         if (reader.IsStream(entry))
                         {
                             std::string name = UTF16ToUTF8(entry->name);
                             if (u16dir.length() > 0)
                             {
                                 std::string dir = UTF16ToUTF8(u16dir.c_str());
                                 if (strncmp(streamName, dir.c_str(), dir.length()) == 0 &&
                                     streamName[dir.length()] == '\\' &&
                                     strcmp(streamName + dir.length() + 1, name.c_str()) == 0)
                                 {
                                     ret = entry;
                                 }
                             }
                             else
                             {
                                 if (strcmp(streamName, name.c_str()) == 0)
                                 {
                                     ret = entry;
                                 }
                             }
                         }
                     });
    return ret;
}


PCB_LAYER_ID ALTIUM_PCB::kicad_layer( int aAltiumLayer ) const {
    switch( aAltiumLayer )
    {
        case ALTIUM_LAYER::TOP_LAYER:           return F_Cu;
        case ALTIUM_LAYER::MID_LAYER_1:         return In1_Cu; // TODO: stackup same as in KiCad?
        case ALTIUM_LAYER::MID_LAYER_2:         return In2_Cu;
        case ALTIUM_LAYER::MID_LAYER_3:         return In3_Cu;
        case ALTIUM_LAYER::MID_LAYER_4:         return In4_Cu;
        case ALTIUM_LAYER::MID_LAYER_5:         return In5_Cu;
        case ALTIUM_LAYER::MID_LAYER_6:         return In6_Cu;
        case ALTIUM_LAYER::MID_LAYER_7:         return In7_Cu;
        case ALTIUM_LAYER::MID_LAYER_8:         return In8_Cu;
        case ALTIUM_LAYER::MID_LAYER_9:         return In9_Cu;
        case ALTIUM_LAYER::MID_LAYER_10:        return In10_Cu;
        case ALTIUM_LAYER::MID_LAYER_11:        return In11_Cu;
        case ALTIUM_LAYER::MID_LAYER_12:        return In12_Cu;
        case ALTIUM_LAYER::MID_LAYER_13:        return In13_Cu;
        case ALTIUM_LAYER::MID_LAYER_14:        return In14_Cu;
        case ALTIUM_LAYER::MID_LAYER_15:        return In15_Cu;
        case ALTIUM_LAYER::MID_LAYER_16:        return In16_Cu;
        case ALTIUM_LAYER::MID_LAYER_17:        return In17_Cu;
        case ALTIUM_LAYER::MID_LAYER_18:        return In18_Cu;
        case ALTIUM_LAYER::MID_LAYER_19:        return In19_Cu;
        case ALTIUM_LAYER::MID_LAYER_20:        return In20_Cu;
        case ALTIUM_LAYER::MID_LAYER_21:        return In21_Cu;
        case ALTIUM_LAYER::MID_LAYER_22:        return In22_Cu;
        case ALTIUM_LAYER::MID_LAYER_23:        return In23_Cu;
        case ALTIUM_LAYER::MID_LAYER_24:        return In24_Cu;
        case ALTIUM_LAYER::MID_LAYER_25:        return In25_Cu;
        case ALTIUM_LAYER::MID_LAYER_26:        return In26_Cu;
        case ALTIUM_LAYER::MID_LAYER_27:        return In27_Cu;
        case ALTIUM_LAYER::MID_LAYER_28:        return In28_Cu;
        case ALTIUM_LAYER::MID_LAYER_29:        return In29_Cu;
        case ALTIUM_LAYER::MID_LAYER_30:        return In30_Cu;
        case ALTIUM_LAYER::BOTTOM_LAYER:        return B_Cu;

        case ALTIUM_LAYER::TOP_OVERLAY:         return F_SilkS;
        case ALTIUM_LAYER::BOTTOM_OVERLAY:      return B_SilkS;
        case ALTIUM_LAYER::TOP_PASTE:           return F_Paste;
        case ALTIUM_LAYER::BOTTOM_PASTE:        return B_Paste;
        case ALTIUM_LAYER::TOP_SOLDER:          return F_Mask;
        case ALTIUM_LAYER::BOTTOM_SOLDER:       return B_Mask;

        case ALTIUM_LAYER::INTERNAL_PLANE_1:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_2:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_3:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_4:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_5:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_6:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_7:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_8:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_9:    return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_10:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_11:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_12:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_13:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_14:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_15:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::INTERNAL_PLANE_16:   return UNDEFINED_LAYER;

        case ALTIUM_LAYER::DRILL_GUIDE:         return Dwgs_User;
        case ALTIUM_LAYER::KEEP_OUT_LAYER:      return UNDEFINED_LAYER;

        case ALTIUM_LAYER::MECHANICAL_1:        return Edge_Cuts;
        case ALTIUM_LAYER::MECHANICAL_2:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_3:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_4:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_5:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_6:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_7:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_8:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_9:        return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_10:       return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_11:       return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_12:       return Dwgs_User;
        case ALTIUM_LAYER::MECHANICAL_13:       return F_Fab;
        case ALTIUM_LAYER::MECHANICAL_14:       return B_Fab;
        case ALTIUM_LAYER::MECHANICAL_15:       return F_CrtYd;
        case ALTIUM_LAYER::MECHANICAL_16:       return B_CrtYd;

        case ALTIUM_LAYER::DRILL_DRAWING:       return Dwgs_User;
        case ALTIUM_LAYER::MULTI_LAYER:         return UNDEFINED_LAYER;
        case ALTIUM_LAYER::CONNECTIONS:         return UNDEFINED_LAYER;
        case ALTIUM_LAYER::BACKGROUND:          return UNDEFINED_LAYER;
        case ALTIUM_LAYER::DRC_ERROR_MARKERS:   return UNDEFINED_LAYER;
        case ALTIUM_LAYER::SELECTIONS:          return UNDEFINED_LAYER;
        case ALTIUM_LAYER::VISIBLE_GRID_1:      return UNDEFINED_LAYER;
        case ALTIUM_LAYER::VISIBLE_GRID_2:      return UNDEFINED_LAYER;
        case ALTIUM_LAYER::PAD_HOLES:           return UNDEFINED_LAYER;
        case ALTIUM_LAYER::VIA_HOLES:           return UNDEFINED_LAYER;

        default:                                return UNDEFINED_LAYER;
    }
}


ALTIUM_PCB::ALTIUM_PCB(BOARD *aBoard) {
    m_board = aBoard;
}

ALTIUM_PCB::~ALTIUM_PCB() {

}

void ALTIUM_PCB::Parse( const CFB::CompoundFileReader& aReader ) {
    // Parse file header
    const CFB::COMPOUND_FILE_ENTRY* fileHeader = FindStream(aReader, "FileHeader");
    wxASSERT( fileHeader != nullptr );
    if (fileHeader != nullptr)
    {
        ParseFileHeader(aReader, fileHeader);
    }

    // Parse board data
    const CFB::COMPOUND_FILE_ENTRY* board = FindStream(aReader, "Board6\\Data");
    wxASSERT( board != nullptr );
    if (board != nullptr)
    {
        ParseBoard6Data(aReader, board);
    }

    // Parse component data
    const CFB::COMPOUND_FILE_ENTRY* components = FindStream(aReader, "Components6\\Data");
    wxASSERT( components != nullptr );
    if (components != nullptr)
    {
        ParseComponents6Data(aReader, components);
    }

    // Parse nets data
    const CFB::COMPOUND_FILE_ENTRY* nets = FindStream(aReader, "Nets6\\Data");
    wxASSERT( nets != nullptr );
    if (nets != nullptr)
    {
        ParseNets6Data(aReader, nets);
    }

    // Parse arcs
    const CFB::COMPOUND_FILE_ENTRY* arcs6 = FindStream(aReader, "Arcs6\\Data");
    wxASSERT( arcs6 != nullptr );
    if (arcs6 != nullptr)
    {
        ParseArcs6Data(aReader, arcs6);
    }

    // Parse pads
    const CFB::COMPOUND_FILE_ENTRY* pads6 = FindStream(aReader, "Pads6\\Data");
    wxASSERT( pads6 != nullptr );
    if (pads6 != nullptr)
    {
        ParsePads6Data(aReader, pads6);
    }

    // Parse vias
    const CFB::COMPOUND_FILE_ENTRY* vias6 = FindStream(aReader, "Vias6\\Data");
    wxASSERT( vias6 != nullptr );
    if (vias6 != nullptr)
    {
        ParseVias6Data(aReader, vias6);
    }

    // Parse tracks
    const CFB::COMPOUND_FILE_ENTRY* tracks6 = FindStream(aReader, "Tracks6\\Data");
    wxASSERT( tracks6 != nullptr );
    if (tracks6 != nullptr)
    {
        ParseTracks6Data(aReader, tracks6);
    }

    // Parse texts
    const CFB::COMPOUND_FILE_ENTRY* texts6 = FindStream(aReader, "Texts6\\Data");
    wxASSERT( texts6 != nullptr );
    if (texts6 != nullptr)
    {
        ParseTexts6Data(aReader, texts6);
    }
}

MODULE* ALTIUM_PCB::GetComponent( const u_int16_t id ) {
    // I asume this is a special case where a elements belongs to the board.
    if( id == std::numeric_limits<u_int16_t>::max() ) {
        MODULE* module = new MODULE( m_board );
        m_board->Add(module);
        return module;
    }

    MODULE *module = m_components.size() > id ? m_components.at( id ) : nullptr;
    if ( module == nullptr )
    {
        module = new MODULE(m_board );
        m_board->Add(module);
        if (id >= m_components.size()) {
            m_components.resize(id + 1, nullptr);
        }
        m_components.insert(m_components.begin() + id, module );
    }
    return module;
}

int ALTIUM_PCB::GetNetCode( const u_int16_t id ) {
    return id == std::numeric_limits<u_int16_t>::max() ?
         NETINFO_LIST::UNCONNECTED :
         id + 1;
}

void ALTIUM_PCB::ParseFileHeader( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry);

    reader.read_subrecord_length();
    std::string header = reader.read_string();
    //std::cout << "HEADER: " << header << std::endl;  // tells me: PCB 5.0 Binary File

    //reader.subrecord_skip();

    // TODO: does not seem to work all the time at the moment
    //wxASSERT(!reader.parser_error());
    //wxASSERT(reader.bytes_remaining() == 0);
}

void ALTIUM_PCB::ParseBoard6Data(const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    ABOARD6 elem( reader );

    wxASSERT(!reader.parser_error());
    wxASSERT(reader.bytes_remaining() == 0);

    m_board->SetCopperLayerCount( elem.layercount );
}

void ALTIUM_PCB::ParseComponents6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    u_int16_t componentId = 0;
    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        ACOMPONENT6 elem( reader );

        MODULE* module = GetComponent(componentId);

        module->SetPosition( elem.position );
        module->SetOrientationDegrees( elem.rotation );
        module->SetReference( elem.sourcedesignator ); // TODO: text duplication
        module->SetLocked( elem.locked );
        module->SetLayer( elem.layer == "TOP" ? F_Cu : B_Cu );

        componentId++;
    }

    wxASSERT(!reader.parser_error());
    wxASSERT(reader.bytes_remaining() == 0);
}

void ALTIUM_PCB::ParseNets6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    u_int16_t netCode = 1; // 0 = UNCONNECTED
    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        ANET6 elem( reader );

        m_board->Add( new NETINFO_ITEM( m_board, elem.name, netCode ) );

        netCode++;
    }

    wxASSERT(!reader.parser_error());
    wxASSERT(reader.bytes_remaining() == 0);
}

void ALTIUM_PCB::ParseArcs6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        AARC6 elem( reader );

        // TODO: better approach to select if item belongs to a MODULE
        DRAWSEGMENT* ds = nullptr;
        if (elem.component == std::numeric_limits<u_int16_t>::max()) {
            ds = new DRAWSEGMENT( m_board );
            m_board->Add( ds );
        } else {
            MODULE* module = GetComponent( elem.component );
            ds = new EDGE_MODULE( module );
            module->Add( ds );
        }

        ds->SetCenter( elem.center );
        ds->SetWidth( elem.width );
        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        ds->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );

        if ( elem.startangle == 0. && elem.endangle == 360. ) {  // TODO: other variants to define circle?
            ds->SetShape( STROKE_T::S_CIRCLE );
            ds->SetArcStart( elem.center - wxPoint( 0, elem.radius ));
        } else {
            ds->SetShape( STROKE_T::S_ARC );

            double angle = elem.endangle - elem.startangle;
            ds->SetAngle( - angle * 10. );

            double startradiant = elem.startangle * M_PI / 180;
            wxPoint arcStartOffset = wxPoint(
                    static_cast<int32_t>(std::cos(startradiant) * elem.radius),
                    -static_cast<int32_t>(std::sin(startradiant) * elem.radius) );
            ds->SetArcStart( elem.center + arcStartOffset);
        }
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParsePads6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error() && reader.bytes_remaining() >= 4*6 /* TODO: use Header section of file */ ) {
        APAD6 elem(reader );

        // Create Pad
        MODULE *module = GetComponent( elem.component );
        D_PAD *pad = new D_PAD( module );
        module->Add( pad );

        pad->SetName( elem.name );
        pad->SetNetCode( GetNetCode( elem.net ) );
        pad->SetPosition( elem.position );
        pad->SetSize( elem.topsize );
        pad->SetOrientationDegrees( elem.direction );
        if ( elem.holesize == 0 ) {
            wxASSERT( elem.layer != ALTIUM_LAYER::MULTI_LAYER );
            pad->SetAttribute( PAD_ATTR_T::PAD_ATTRIB_SMD );
        } else {
            wxASSERT( elem.layer == ALTIUM_LAYER::MULTI_LAYER );  // TODO: I assume other values are possible as well?
            pad->SetAttribute( elem.plated ? PAD_ATTR_T::PAD_ATTRIB_STANDARD : PAD_ATTR_T::PAD_ATTRIB_HOLE_NOT_PLATED );
            pad->SetDrillSize( wxSize( elem.holesize, elem.holesize ) );
        }

        wxASSERT( elem.padmode == ALTIUM_PAD_MODE::SIMPLE );
        // wxASSERT( topshape == midshape == botshape );
        switch ( elem.topshape ) {
            case ALTIUM_PAD_SHAPE::RECT:
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_RECT );
                break;
            case ALTIUM_PAD_SHAPE::CIRCLE:
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_CIRCLE );
                break;
            case ALTIUM_PAD_SHAPE::OVAL:
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_OVAL );
                break;
            case ALTIUM_PAD_SHAPE::UNKNOWN:
            default:
                wxFAIL_MSG("unknown shape");
                break;
        }

        switch ( elem.layer ) {
            case ALTIUM_LAYER::TOP_LAYER:
                pad->SetLayer( F_Cu );
                pad->SetLayerSet( LSET( 3, F_Cu, F_Paste, F_Mask ) );
                break;
            case ALTIUM_LAYER::BOTTOM_LAYER:
                pad->SetLayer( B_Cu );
                pad->SetLayerSet( LSET( 3, B_Cu, B_Paste, B_Mask ) );
                break;
            case ALTIUM_LAYER::MULTI_LAYER:
            default:
                pad->SetLayerSet( LSET::AllCuMask() );
                pad->SetLayerSet( pad->GetLayerSet().set( B_Mask).set( F_Mask ) ); // Solder Mask
                break;
        }
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseVias6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        AVIA6 elem(reader );

        VIA *via = new VIA( m_board );
        m_board->Add( via );

        via->SetPosition( elem.position );
        via->SetWidth( elem.diameter );
        via->SetDrill( elem.holesize );
        via->SetViaType( VIATYPE::THROUGH ); // TODO
        via->SetNetCode( GetNetCode( elem.net ) );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseTracks6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        ATRACK6 elem(reader );

        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        if( klayer >= F_Cu && klayer <= B_Cu )
        {
            TRACK* track = new TRACK( m_board );
            m_board->Add( track );

            track->SetStart( elem.start );
            track->SetEnd( elem.end );
            track->SetWidth( elem.width );
            track->SetLayer( klayer );
            track->SetNetCode( GetNetCode( elem.net ) );
        }
        else
        {
            DRAWSEGMENT* ds = nullptr;

            if ( elem.component == std::numeric_limits<u_int16_t>::max() ) {
                ds = new DRAWSEGMENT( m_board );
                ds->SetShape( STROKE_T::S_SEGMENT );
                m_board->Add( ds );
            } else {
                MODULE* module = GetComponent( elem.component );
                ds = new EDGE_MODULE( module, STROKE_T::S_SEGMENT );
                module->Add( ds );
            }

            ds->SetStart( elem.start );
            ds->SetEnd( elem.end );
            ds->SetWidth( elem.width );

            ds->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );
        }

        reader.subrecord_skip();
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseTexts6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry ) {
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error() && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ ) {
        ATEXT6 elem(reader );

        // TODO: better approach to select if item belongs to a MODULE
        EDA_TEXT* tx = nullptr;
        BOARD_ITEM* itm = nullptr;
        if (elem.component == std::numeric_limits<u_int16_t>::max()) {
            TEXTE_PCB* txp = new TEXTE_PCB(m_board );
            tx = txp;
            itm = txp;
            m_board->Add( txp );
        } else {
            MODULE* module = GetComponent( elem.component );
            TEXTE_MODULE* txm = new TEXTE_MODULE( module );
            tx = txm;
            itm = txm;
            module->Add( txm );
        }

        itm->SetPosition( elem.position );
        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        itm->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );

        tx->SetTextHeight( elem.height );
        tx->SetTextAngle ( elem.rotation * 10. );
        tx->SetText( elem.text );
        tx->SetHorizJustify( EDA_TEXT_HJUSTIFY_T::GR_TEXT_HJUSTIFY_LEFT ); // TODO: what byte

        wxASSERT(!reader.parser_error());
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

ABOARD6::ABOARD6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    /*for (auto & property : properties) {
        std::cout << "  * '" << property.first << "' = '" << property.second << "'" << std::endl;
    }*/

    layercount = ALTIUM_PARSER::property_int( properties, "LAYERSETSCOUNT", 2 );
}

ACOMPONENT6::ACOMPONENT6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    layer               = ALTIUM_PARSER::property_string( properties, "LAYER", "" );
    position            = wxPoint(
            ALTIUM_PARSER::property_unit( properties, "X", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "Y", "0mil" ));
    rotation            = ALTIUM_PARSER::property_double( properties, "ROTATION", 0. );
    locked              = ALTIUM_PARSER::property_bool( properties, "LOCKED", false );
    sourcedesignator    = ALTIUM_PARSER::property_string( properties, "SOURCEDESIGNATOR", "" );
    sourcelibreference  = ALTIUM_PARSER::property_string( properties, "SOURCELIBREFERENCE", "" );
}

ANET6::ANET6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    name = ALTIUM_PARSER::property_string( properties, "NAME", "" );
}

AARC6::AARC6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    u_int8_t recordtype = reader.read<u_int8_t>();
    wxASSERT( recordtype == ALTIUM_RECORD::ARC );

    // Subrecord 1
    reader.read_subrecord_length();

    layer       = reader.read<u_int8_t>();
    reader.skip( 2 );
    net         = reader.read<u_int16_t>();
    reader.skip( 2 );
    component   = reader.read<u_int16_t>();
    reader.skip(4);
    center      = reader.read_point();
    radius      = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );
    startangle  = reader.read<double>();
    endangle    = reader.read<double>();
    width       = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );

    reader.subrecord_skip();

    wxASSERT(!reader.parser_error());
}

APAD6::APAD6(ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    u_int8_t recordtype = reader.read<u_int8_t>();
    wxASSERT( recordtype == ALTIUM_RECORD::PAD );

    // Subrecord 1
    size_t subrecord1 = reader.read_subrecord_length();
    wxASSERT( subrecord1 > 0 );
    name = reader.read_string();
    wxASSERT( reader.subrecord_remaining() == 0 );
    reader.subrecord_skip();

    // Subrecord 2
    reader.read_subrecord_length();
    reader.subrecord_skip();

    // Subrecord 3
    reader.read_subrecord_length();
    reader.subrecord_skip();

    // Subrecord 4
    reader.read_subrecord_length();
    reader.subrecord_skip();

    // Subrecord 5
    size_t subrecord5 = reader.read_subrecord_length();
    wxASSERT( subrecord5 >= 120 );  // TODO: exact minimum length we know?

    layer       = reader.read<u_int8_t>();
    reader.skip( 2 );
    net         = reader.read<u_int16_t>();
    reader.skip( 2 );
    component   = reader.read<u_int16_t>();
    reader.skip( 4 );

    position    = reader.read_point();
    topsize     = reader.read_size();
    midsize     = reader.read_size();
    botsize     = reader.read_size();

    holesize    = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );
    topshape    = reader.read<u_int8_t>();
    midshape    = reader.read<u_int8_t>();
    botshape    = reader.read<u_int8_t>();

    direction   = reader.read<double>();
    plated      = reader.read<u_int8_t>() != 0;
    reader.skip( 1 );
    padmode     = reader.read<u_int8_t>();
    reader.skip( 38 );
    pastemaskexpansionmode  = reader.read<u_int8_t>();
    soldermaskexpansion     = reader.read<u_int8_t>();
    reader.skip( 3 );
    holerotation = reader.read<double>();
    if ( subrecord5 == 120 ) {
        tolayer     = reader.read<u_int8_t>();
        reader.skip( 2 );
        fromlayer   = reader.read<u_int8_t>();
        //reader.skip( 2 );
    } else if ( subrecord5 == 171 ) {

    }
    reader.subrecord_skip();

    // Subrecord 6
    reader.read_subrecord_length();
    reader.subrecord_skip();

    wxASSERT(!reader.parser_error());
}

AVIA6::AVIA6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    u_int8_t recordtype = reader.read<u_int8_t>();
    wxASSERT( recordtype == ALTIUM_RECORD::VIA );

    // Subrecord 1
    reader.read_subrecord_length();

    reader.skip( 3 );
    net         = reader.read<u_int16_t>();
    reader.skip( 8 );
    position    = reader.read_point();
    diameter    = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );
    holesize    = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );

    reader.subrecord_skip();

    wxASSERT(!reader.parser_error());
}

ATRACK6::ATRACK6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    u_int8_t recordtype = reader.read<u_int8_t>();
    wxASSERT( recordtype == ALTIUM_RECORD::TRACK );

    // Subrecord 1
    reader.read_subrecord_length();

    layer       = reader.read<u_int8_t>();
    reader.skip( 2 );
    net         = reader.read<u_int16_t>();
    reader.skip( 2 );
    component   = reader.read<u_int16_t>();
    reader.skip( 4 );
    start       = reader.read_point();
    end         = reader.read_point();
    width       = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );

    reader.subrecord_skip();

    wxASSERT(!reader.parser_error());
}

ATEXT6::ATEXT6( ALTIUM_PARSER &reader ) {
    wxASSERT(reader.bytes_remaining() > 4);
    wxASSERT(!reader.parser_error());

    u_int8_t recordtype = reader.read<u_int8_t>();
    wxASSERT( recordtype == ALTIUM_RECORD::TEXT );

    // Subrecord 1 - Properties
    reader.read_subrecord_length();

    layer       = reader.read<u_int8_t>();
    reader.skip(6);
    component   = reader.read<u_int16_t>();
    reader.skip(4);
    position    = reader.read_point();
    height      = ALTIUM_PARSER::kicad_unit( reader.read<u_int32_t>() );
    reader.skip(2);
    rotation    = reader.read<double>();

    reader.subrecord_skip();

    // Subrecord 2 - String
    reader.read_subrecord_length();

    text        = reader.read_string(); // TODO: what about strings with length > 255?

    reader.subrecord_skip();

    wxASSERT(!reader.parser_error());
}