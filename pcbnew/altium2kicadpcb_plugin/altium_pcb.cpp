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
#include <class_dimension.h>
#include <class_drawsegment.h>
#include <class_pcb_text.h>

#include <class_edge_mod.h>
#include <class_text_mod.h>

#include <board_stackup_manager/stackup_predefined_prms.h>

#include <compoundfilereader.h>
#include <convert_basic_shapes_to_polygon.h>
#include <trigo.h>
#include <utf.h>


const CFB::COMPOUND_FILE_ENTRY* FindStream(
        const CFB::CompoundFileReader& reader, const char* streamName )
{
    const CFB::COMPOUND_FILE_ENTRY* ret = nullptr;
    reader.EnumFiles( reader.GetRootEntry(), -1,
            [&]( const CFB::COMPOUND_FILE_ENTRY* entry, const CFB::utf16string& u16dir,
                    int level ) -> void {
                if( reader.IsStream( entry ) )
                {
                    std::string name = UTF16ToUTF8( entry->name );
                    if( u16dir.length() > 0 )
                    {
                        std::string dir = UTF16ToUTF8( u16dir.c_str() );
                        if( strncmp( streamName, dir.c_str(), dir.length() ) == 0
                                && streamName[dir.length()] == '\\'
                                && strcmp( streamName + dir.length() + 1, name.c_str() ) == 0 )
                        {
                            ret = entry;
                        }
                    }
                    else
                    {
                        if( strcmp( streamName, name.c_str() ) == 0 )
                        {
                            ret = entry;
                        }
                    }
                }
            } );
    return ret;
}

ALTIUM_LAYER ALTIUM_PCB::altium_layer_from_name( const std::string& aName ) const
{
    // TODO: only initialize table once
    std::unordered_map<std::string, ALTIUM_LAYER> hash_map;
    hash_map["TOP"]    = ALTIUM_LAYER::TOP_LAYER;
    hash_map["MID1"]   = ALTIUM_LAYER::MID_LAYER_1;
    hash_map["MID2"]   = ALTIUM_LAYER::MID_LAYER_2;
    hash_map["MID3"]   = ALTIUM_LAYER::MID_LAYER_3;
    hash_map["MID4"]   = ALTIUM_LAYER::MID_LAYER_4;
    hash_map["MID5"]   = ALTIUM_LAYER::MID_LAYER_5;
    hash_map["MID6"]   = ALTIUM_LAYER::MID_LAYER_6;
    hash_map["MID7"]   = ALTIUM_LAYER::MID_LAYER_7;
    hash_map["MID8"]   = ALTIUM_LAYER::MID_LAYER_8;
    hash_map["MID9"]   = ALTIUM_LAYER::MID_LAYER_9;
    hash_map["MID10"]  = ALTIUM_LAYER::MID_LAYER_10;
    hash_map["MID11"]  = ALTIUM_LAYER::MID_LAYER_11;
    hash_map["MID12"]  = ALTIUM_LAYER::MID_LAYER_12;
    hash_map["MID13"]  = ALTIUM_LAYER::MID_LAYER_13;
    hash_map["MID14"]  = ALTIUM_LAYER::MID_LAYER_14;
    hash_map["MID15"]  = ALTIUM_LAYER::MID_LAYER_15;
    hash_map["MID16"]  = ALTIUM_LAYER::MID_LAYER_16;
    hash_map["MID17"]  = ALTIUM_LAYER::MID_LAYER_17;
    hash_map["MID18"]  = ALTIUM_LAYER::MID_LAYER_18;
    hash_map["MID19"]  = ALTIUM_LAYER::MID_LAYER_19;
    hash_map["MID20"]  = ALTIUM_LAYER::MID_LAYER_20;
    hash_map["MID21"]  = ALTIUM_LAYER::MID_LAYER_21;
    hash_map["MID22"]  = ALTIUM_LAYER::MID_LAYER_22;
    hash_map["MID23"]  = ALTIUM_LAYER::MID_LAYER_23;
    hash_map["MID24"]  = ALTIUM_LAYER::MID_LAYER_24;
    hash_map["MID25"]  = ALTIUM_LAYER::MID_LAYER_25;
    hash_map["MID26"]  = ALTIUM_LAYER::MID_LAYER_26;
    hash_map["MID27"]  = ALTIUM_LAYER::MID_LAYER_27;
    hash_map["MID28"]  = ALTIUM_LAYER::MID_LAYER_28;
    hash_map["MID29"]  = ALTIUM_LAYER::MID_LAYER_29;
    hash_map["MID30"]  = ALTIUM_LAYER::MID_LAYER_30;
    hash_map["BOTTOM"] = ALTIUM_LAYER::BOTTOM_LAYER;

    hash_map["PLANE1"]  = ALTIUM_LAYER::INTERNAL_PLANE_1;
    hash_map["PLANE2"]  = ALTIUM_LAYER::INTERNAL_PLANE_2;
    hash_map["PLANE3"]  = ALTIUM_LAYER::INTERNAL_PLANE_3;
    hash_map["PLANE4"]  = ALTIUM_LAYER::INTERNAL_PLANE_4;
    hash_map["PLANE5"]  = ALTIUM_LAYER::INTERNAL_PLANE_5;
    hash_map["PLANE6"]  = ALTIUM_LAYER::INTERNAL_PLANE_6;
    hash_map["PLANE7"]  = ALTIUM_LAYER::INTERNAL_PLANE_7;
    hash_map["PLANE8"]  = ALTIUM_LAYER::INTERNAL_PLANE_8;
    hash_map["PLANE9"]  = ALTIUM_LAYER::INTERNAL_PLANE_9;
    hash_map["PLANE10"] = ALTIUM_LAYER::INTERNAL_PLANE_10;
    hash_map["PLANE11"] = ALTIUM_LAYER::INTERNAL_PLANE_11;
    hash_map["PLANE12"] = ALTIUM_LAYER::INTERNAL_PLANE_12;
    hash_map["PLANE13"] = ALTIUM_LAYER::INTERNAL_PLANE_13;
    hash_map["PLANE14"] = ALTIUM_LAYER::INTERNAL_PLANE_14;
    hash_map["PLANE15"] = ALTIUM_LAYER::INTERNAL_PLANE_15;
    hash_map["PLANE16"] = ALTIUM_LAYER::INTERNAL_PLANE_16;

    hash_map["MECHANICAL1"]  = ALTIUM_LAYER::MECHANICAL_1;
    hash_map["MECHANICAL2"]  = ALTIUM_LAYER::MECHANICAL_2;
    hash_map["MECHANICAL3"]  = ALTIUM_LAYER::MECHANICAL_3;
    hash_map["MECHANICAL4"]  = ALTIUM_LAYER::MECHANICAL_4;
    hash_map["MECHANICAL5"]  = ALTIUM_LAYER::MECHANICAL_5;
    hash_map["MECHANICAL6"]  = ALTIUM_LAYER::MECHANICAL_6;
    hash_map["MECHANICAL7"]  = ALTIUM_LAYER::MECHANICAL_7;
    hash_map["MECHANICAL8"]  = ALTIUM_LAYER::MECHANICAL_8;
    hash_map["MECHANICAL9"]  = ALTIUM_LAYER::MECHANICAL_9;
    hash_map["MECHANICAL10"] = ALTIUM_LAYER::MECHANICAL_10;
    hash_map["MECHANICAL11"] = ALTIUM_LAYER::MECHANICAL_11;
    hash_map["MECHANICAL12"] = ALTIUM_LAYER::MECHANICAL_12;
    hash_map["MECHANICAL13"] = ALTIUM_LAYER::MECHANICAL_13;
    hash_map["MECHANICAL14"] = ALTIUM_LAYER::MECHANICAL_14;
    hash_map["MECHANICAL15"] = ALTIUM_LAYER::MECHANICAL_15;
    hash_map["MECHANICAL16"] = ALTIUM_LAYER::MECHANICAL_16;

    auto it = hash_map.find( aName );
    if( it == hash_map.end() )
    {
        return ALTIUM_LAYER::UNKNOWN;
    }
    else
    {
        return it->second;
    }
}


PCB_LAYER_ID ALTIUM_PCB::kicad_layer( ALTIUM_LAYER aAltiumLayer ) const
{
    auto override = m_layermap.find( aAltiumLayer );
    if( override != m_layermap.end() )
    {
        return override->second;
    }

    switch( aAltiumLayer )
    {
    case ALTIUM_LAYER::UNKNOWN:
        return UNDEFINED_LAYER;

    case ALTIUM_LAYER::TOP_LAYER:
        return F_Cu;
    case ALTIUM_LAYER::MID_LAYER_1:
        return In1_Cu; // TODO: stackup same as in KiCad?
    case ALTIUM_LAYER::MID_LAYER_2:
        return In2_Cu;
    case ALTIUM_LAYER::MID_LAYER_3:
        return In3_Cu;
    case ALTIUM_LAYER::MID_LAYER_4:
        return In4_Cu;
    case ALTIUM_LAYER::MID_LAYER_5:
        return In5_Cu;
    case ALTIUM_LAYER::MID_LAYER_6:
        return In6_Cu;
    case ALTIUM_LAYER::MID_LAYER_7:
        return In7_Cu;
    case ALTIUM_LAYER::MID_LAYER_8:
        return In8_Cu;
    case ALTIUM_LAYER::MID_LAYER_9:
        return In9_Cu;
    case ALTIUM_LAYER::MID_LAYER_10:
        return In10_Cu;
    case ALTIUM_LAYER::MID_LAYER_11:
        return In11_Cu;
    case ALTIUM_LAYER::MID_LAYER_12:
        return In12_Cu;
    case ALTIUM_LAYER::MID_LAYER_13:
        return In13_Cu;
    case ALTIUM_LAYER::MID_LAYER_14:
        return In14_Cu;
    case ALTIUM_LAYER::MID_LAYER_15:
        return In15_Cu;
    case ALTIUM_LAYER::MID_LAYER_16:
        return In16_Cu;
    case ALTIUM_LAYER::MID_LAYER_17:
        return In17_Cu;
    case ALTIUM_LAYER::MID_LAYER_18:
        return In18_Cu;
    case ALTIUM_LAYER::MID_LAYER_19:
        return In19_Cu;
    case ALTIUM_LAYER::MID_LAYER_20:
        return In20_Cu;
    case ALTIUM_LAYER::MID_LAYER_21:
        return In21_Cu;
    case ALTIUM_LAYER::MID_LAYER_22:
        return In22_Cu;
    case ALTIUM_LAYER::MID_LAYER_23:
        return In23_Cu;
    case ALTIUM_LAYER::MID_LAYER_24:
        return In24_Cu;
    case ALTIUM_LAYER::MID_LAYER_25:
        return In25_Cu;
    case ALTIUM_LAYER::MID_LAYER_26:
        return In26_Cu;
    case ALTIUM_LAYER::MID_LAYER_27:
        return In27_Cu;
    case ALTIUM_LAYER::MID_LAYER_28:
        return In28_Cu;
    case ALTIUM_LAYER::MID_LAYER_29:
        return In29_Cu;
    case ALTIUM_LAYER::MID_LAYER_30:
        return In30_Cu;
    case ALTIUM_LAYER::BOTTOM_LAYER:
        return B_Cu;

    case ALTIUM_LAYER::TOP_OVERLAY:
        return F_SilkS;
    case ALTIUM_LAYER::BOTTOM_OVERLAY:
        return B_SilkS;
    case ALTIUM_LAYER::TOP_PASTE:
        return F_Paste;
    case ALTIUM_LAYER::BOTTOM_PASTE:
        return B_Paste;
    case ALTIUM_LAYER::TOP_SOLDER:
        return F_Mask;
    case ALTIUM_LAYER::BOTTOM_SOLDER:
        return B_Mask;

    case ALTIUM_LAYER::INTERNAL_PLANE_1:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_2:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_3:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_4:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_5:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_6:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_7:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_8:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_9:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_10:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_11:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_12:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_13:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_14:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_15:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::INTERNAL_PLANE_16:
        return UNDEFINED_LAYER;

    case ALTIUM_LAYER::DRILL_GUIDE:
        return Dwgs_User;
    case ALTIUM_LAYER::KEEP_OUT_LAYER:
        return UNDEFINED_LAYER;

    case ALTIUM_LAYER::MECHANICAL_1:
        return Edge_Cuts;
    case ALTIUM_LAYER::MECHANICAL_2:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_3:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_4:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_5:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_6:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_7:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_8:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_9:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_10:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_11:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_12:
        return Dwgs_User;
    case ALTIUM_LAYER::MECHANICAL_13:
        return F_Fab;
    case ALTIUM_LAYER::MECHANICAL_14:
        return B_Fab;
    case ALTIUM_LAYER::MECHANICAL_15:
        return F_CrtYd;
    case ALTIUM_LAYER::MECHANICAL_16:
        return B_CrtYd;

    case ALTIUM_LAYER::DRILL_DRAWING:
        return Dwgs_User;
    case ALTIUM_LAYER::MULTI_LAYER:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::CONNECTIONS:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::BACKGROUND:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::DRC_ERROR_MARKERS:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::SELECTIONS:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::VISIBLE_GRID_1:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::VISIBLE_GRID_2:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::PAD_HOLES:
        return UNDEFINED_LAYER;
    case ALTIUM_LAYER::VIA_HOLES:
        return UNDEFINED_LAYER;

    default:
        return UNDEFINED_LAYER;
    }
}


ALTIUM_PCB::ALTIUM_PCB( BOARD* aBoard )
{
    m_board = aBoard;
}

ALTIUM_PCB::~ALTIUM_PCB()
{
}

namespace ALTIUM_DESIGNER
{
const std::string FILE_HEADER = "FileHeader";

const std::string ARCS6_DATA        = "Arcs6\\Data";
const std::string BOARD6_DATA       = "Board6\\Data";
const std::string BOARDREGIONS_DATA = "BoardRegions\\Data";
const std::string CLASSES6_DATA     = "Classes6\\Data";
const std::string COMPONENTS6_DATA  = "Components6\\Data";
const std::string DIMENSIONS6_DATA  = "Dimensions6\\Data";
const std::string FILLS6_DATA       = "Fills6\\Data";
const std::string NETS6_DATA        = "Nets6\\Data";
const std::string PADS6_DATA        = "Pads6\\Data";
const std::string POLYGONS6_DATA    = "Polygons6\\Data";
const std::string REGIONS6_DATA     = "Regions6\\Data";
const std::string RULES6_DATA       = "Rules6\\Data";
const std::string TEXTS6_DATA       = "Texts6\\Data";
const std::string TRACKS6_DATA      = "Tracks6\\Data";
const std::string VIAS6_DATA        = "Vias6\\Data";
}; // namespace ALTIUM_DESIGNER

// those directories were found by searching for equivalent files in both formats
namespace ALTIUM_CIRCUIT_STUDIO
{
const std::string FILE_HEADER = "FileHeader";

const std::string ARCS6_DATA       = "00C595EB90524FFC8C3BD9670020A2\\Data";
const std::string BOARD6_DATA      = "88857D7F1DF64F7BBB61848C965636\\Data";
// const std::string BOARDREGIONS_DATA = "TODO\\Data";
// const std::string CLASSES6_DATA     = "TODO\\Data";
const std::string COMPONENTS6_DATA = "465416896A15486999A39C643935D2\\Data";
// const std::string DIMENSIONS6_DATA  = "TODO\\Data";
const std::string FILLS6_DATA      = "4E83BDC3253747F08E9006D7F57020\\Data";
const std::string NETS6_DATA       = "D95A0DA2FE9047779A5194C127F30B\\Data";
const std::string PADS6_DATA       = "47D69BC5107A4B8DB8DAA23E39C238\\Data";
const std::string POLYGONS6_DATA   = "D7038392280E4E229B9D9B5426B295\\Data";
// const std::string REGIONS6_DATA = "TODO\\Data";
// const std::string RULES6_DATA   = "TODO\\Data";
const std::string TEXTS6_DATA      = "349ABBB211DB4F5B8AE41B1B49555A\\Data";
const std::string TRACKS6_DATA     = "530C20C225354B858B2578CAB8C08D\\Data";
const std::string VIAS6_DATA       = "CA5F5989BCDB404DA70A9D1D3D5758\\Data";
}; // namespace ALTIUM_CIRCUIT_STUDIO

// those directories were found by searching for equivalent files in both formats
namespace ALTIUM_CIRCUIT_MAKER
{
const std::string FILE_HEADER = "FileHeader";

const std::string ARCS6_DATA        = "1CEEB63FB33847F8AFC4485F64735E\\Data";
const std::string BOARD6_DATA       = "96B09F5C6CEE434FBCE0DEB3E88E70\\Data";
const std::string BOARDREGIONS_DATA = "E3A544335C30403A991912052C936F\\Data";
const std::string CLASSES6_DATA     = "4F71DD45B09143988210841EA1C28D\\Data";
const std::string COMPONENTS6_DATA  = "F9D060ACC7DD4A85BC73CB785BAC81\\Data";
const std::string DIMENSIONS6_DATA  = "068B9422DBB241258BA2DE9A6BA1A6\\Data";
const std::string FILLS6_DATA       = "6FFE038462A940E9B422EFC8F5D85E\\Data";
const std::string NETS6_DATA        = "35D7CF51BB9B4875B3A138B32D80DC\\Data";
const std::string PADS6_DATA        = "4F501041A9BC4A06BDBDAB67D3820E\\Data";
const std::string POLYGONS6_DATA    = "A1931C8B0B084A61AA45146575FDD3\\Data";
const std::string REGIONS6_DATA     = "F513A5885418472886D3EF18A09E46\\Data";
const std::string RULES6_DATA       = "C27718A40C94421388FAE5BD7785D7\\Data";
const std::string TEXTS6_DATA       = "A34BC67C2A5F408D8F377378C5C5E2\\Data";
const std::string TRACKS6_DATA      = "412A754DBB864645BF01CD6A80C358\\Data";
const std::string VIAS6_DATA        = "C87A685A0EFA4A90BEEFD666198B56\\Data";
}; // namespace ALTIUM_CIRCUIT_MAKER

void ALTIUM_PCB::ParseDesigner( const CFB::CompoundFileReader& aReader )
{
    ParseHelper( aReader, ALTIUM_DESIGNER::FILE_HEADER, [this]( auto aReader, auto fileHeader ) {
        this->ParseFileHeader( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::BOARD6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseBoard6Data( aReader, fileHeader );
    } );

    ParseHelper(
            aReader, ALTIUM_DESIGNER::COMPONENTS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseComponents6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_DESIGNER::NETS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseNets6Data( aReader, fileHeader );
    } );

    // we need the nets assigned beforehand? Or use the UUID?
    ParseHelper( aReader, ALTIUM_DESIGNER::CLASSES6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseClasses6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::RULES6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseRules6Data( aReader, fileHeader );
    } );

    ParseHelper(
            aReader, ALTIUM_DESIGNER::DIMENSIONS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseDimensions6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_DESIGNER::POLYGONS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParsePolygons6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::ARCS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseArcs6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::PADS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParsePads6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::VIAS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseVias6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::TRACKS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseTracks6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::TEXTS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseTexts6Data( aReader, fileHeader );
    } );

    ParseHelper( aReader, ALTIUM_DESIGNER::FILLS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseFills6Data( aReader, fileHeader );
    } );

    ParseHelper(
            aReader, ALTIUM_DESIGNER::BOARDREGIONS_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseBoardRegionsData( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_DESIGNER::REGIONS6_DATA, [this]( auto aReader, auto fileHeader ) {
        this->ParseRegions6Data( aReader, fileHeader );
    } );

    FinishParsingHelper();
}

void ALTIUM_PCB::ParseCircuitStudio( const CFB::CompoundFileReader& aReader )
{
    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::FILE_HEADER, [this]( auto aReader, auto fileHeader ) {
                this->ParseFileHeader( aReader, fileHeader );
            } );

    // TODO: Board Stackup seems to differ from Altium Designer
    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::BOARD6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseBoard6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_CIRCUIT_STUDIO::COMPONENTS6_DATA,
            [this]( auto aReader, auto fileHeader ) {
                this->ParseComponents6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::NETS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseNets6Data( aReader, fileHeader );
            } );

    // we need the nets assigned beforehand? Or use the UUID?
    //    ParseHelper(
    //            aReader, ALTIUM_CIRCUIT_MAKER::CLASSES6_DATA, [this]( auto aReader, auto fileHeader ) {
    //                this->ParseClasses6Data( aReader, fileHeader );
    //            } );

    //    ParseHelper( aReader, ALTIUM_CIRCUIT_STUDIO::RULES6_DATA, [this]( auto aReader, auto fileHeader ) {
    //        this->ParseRules6Data( aReader, fileHeader );
    //    } );

    //    ParseHelper( aReader, ALTIUM_CIRCUIT_STUDIO::DIMENSIONS6_DATA, [this]( auto aReader, auto fileHeader ) {
    //        this->ParseDimensions6Data( aReader, fileHeader );
    //    } );

    ParseHelper( aReader, ALTIUM_CIRCUIT_STUDIO::POLYGONS6_DATA,
            [this]( auto aReader, auto fileHeader ) {
                this->ParsePolygons6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::ARCS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseArcs6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::PADS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParsePads6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::VIAS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseVias6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::TRACKS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseTracks6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::TEXTS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseTexts6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_STUDIO::FILLS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseFills6Data( aReader, fileHeader );
            } );

    // TODO: enable
    //    ParseHelper(
    //            aReader, ALTIUM_CIRCUIT_STUDIO::BOARDREGIONS_DATA, [this]( auto aReader, auto fileHeader ) {
    //                this->ParseBoardRegionsData( aReader, fileHeader );
    //            } );
    //
    //    ParseHelper(
    //            aReader, ALTIUM_CIRCUIT_STUDIO::REGIONS6_DATA, [this]( auto aReader, auto fileHeader ) {
    //                this->ParseRegions6Data( aReader, fileHeader );
    //            } );

    FinishParsingHelper();
}

void ALTIUM_PCB::ParseCircuitMaker( const CFB::CompoundFileReader& aReader )
{
    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::FILE_HEADER, [this]( auto aReader, auto fileHeader ) {
                this->ParseFileHeader( aReader, fileHeader );
            } );

    // TODO: Board Stackup seems to differ from Altium Designer
    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::BOARD6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseBoard6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_CIRCUIT_MAKER::COMPONENTS6_DATA,
            [this]( auto aReader, auto fileHeader ) {
                this->ParseComponents6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::NETS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseNets6Data( aReader, fileHeader );
            } );

    // we need the nets assigned beforehand? Or use the UUID?
    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::CLASSES6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseClasses6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::RULES6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseRules6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_CIRCUIT_MAKER::DIMENSIONS6_DATA,
            [this]( auto aReader, auto fileHeader ) {
                this->ParseDimensions6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::POLYGONS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParsePolygons6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::ARCS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseArcs6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::PADS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParsePads6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::VIAS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseVias6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::TRACKS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseTracks6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::TEXTS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseTexts6Data( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::FILLS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseFills6Data( aReader, fileHeader );
            } );

    ParseHelper( aReader, ALTIUM_CIRCUIT_MAKER::BOARDREGIONS_DATA,
            [this]( auto aReader, auto fileHeader ) {
                this->ParseBoardRegionsData( aReader, fileHeader );
            } );

    ParseHelper(
            aReader, ALTIUM_CIRCUIT_MAKER::REGIONS6_DATA, [this]( auto aReader, auto fileHeader ) {
                this->ParseRegions6Data( aReader, fileHeader );
            } );

    FinishParsingHelper();
}

void ALTIUM_PCB::ParseHelper( const CFB::CompoundFileReader& aReader, const std::string& streamName,
        parse_function_pointer_t fp )
{
    const CFB::COMPOUND_FILE_ENTRY* file = FindStream( aReader, streamName.c_str() );
    wxASSERT_MSG( file != nullptr, "File not found: " + streamName );
    if( file != nullptr )
    {
        fp( aReader, file );
    }
}

void ALTIUM_PCB::FinishParsingHelper()
{
    // Finish Board by recalculating module boundingboxes
    for( auto& module : m_board->Modules() )
    {
        module->CalculateBoundingBox();
    }
}

MODULE* ALTIUM_PCB::GetComponent( const uint16_t id )
{
    // I asume this is a special case where a elements belongs to the board.
    if( id == std::numeric_limits<uint16_t>::max() )
    {
        MODULE* module = new MODULE( m_board );
        m_board->Add( module, ADD_MODE::APPEND );
        return module;
    }

    MODULE* module = m_components.size() > id ? m_components.at( id ) : nullptr;
    if( module == nullptr )
    {
        module = new MODULE( m_board );
        m_board->Add( module, ADD_MODE::APPEND );
        if( id >= m_components.size() )
        {
            m_components.resize( id + 1, nullptr );
        }
        m_components.insert( m_components.begin() + id, module );
    }
    return module;
}

int ALTIUM_PCB::GetNetCode( const uint16_t id )
{
    return id == std::numeric_limits<uint16_t>::max() ? NETINFO_LIST::UNCONNECTED : id + 1;
}

void ALTIUM_PCB::ParseFileHeader(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    reader.read_subrecord_length();
    std::string header = reader.read_string();
    //std::cout << "HEADER: " << header << std::endl;  // tells me: PCB 5.0 Binary File

    //reader.subrecord_skip();

    // TODO: does not seem to work all the time at the moment
    //wxASSERT(!reader.parser_error());
    //wxASSERT(reader.bytes_remaining() == 0);
}

void ALTIUM_PCB::ParseBoard6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    ABOARD6 elem( reader );

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );

    m_board->SetAuxOrigin( elem.sheetpos );
    m_board->SetGridOrigin( elem.sheetpos );

    // read layercount from stackup, because LAYERSETSCOUNT is not always correct?!
    size_t layercount = 0;
    for( size_t i                                = static_cast<size_t>( ALTIUM_LAYER::TOP_LAYER );
            i < elem.stackup.size() && i != 0; i = elem.stackup[i - 1].nextId, layercount++ )
        ;
    m_board->SetCopperLayerCount( layercount );

    BOARD_DESIGN_SETTINGS& designSettings = m_board->GetDesignSettings();
    BOARD_STACKUP&         stackup        = designSettings.GetStackupDescriptor();

    // create board stackup
    stackup.RemoveAll(); // Just to be sure
    stackup.BuildDefaultStackupList( &designSettings, layercount );

    auto it = stackup.GetList().begin();
    // find first copper layer
    for( ; it != stackup.GetList().end() && ( *it )->GetType() != BS_ITEM_TYPE_COPPER; ++it )
        ;

    auto curLayer = static_cast<int>( F_Cu );
    for( size_t i                                = static_cast<size_t>( ALTIUM_LAYER::TOP_LAYER );
            i < elem.stackup.size() && i != 0; i = elem.stackup[i - 1].nextId, layercount++ )
    {
        auto layer = elem.stackup.at( i - 1 ); // array starts with 0, but stackup with 1
        m_layermap.insert(
                { static_cast<ALTIUM_LAYER>( i ), static_cast<PCB_LAYER_ID>( curLayer++ ) } );

        wxASSERT( ( *it )->GetType() == BS_ITEM_TYPE_COPPER );
        ( *it )->SetThickness( layer.copperthick );

        if( ( *it )->GetBrdLayerId() == B_Cu )
        {
            wxASSERT( layer.nextId == 0 );
            // overwrite entry from internal -> bottom
            m_layermap[static_cast<ALTIUM_LAYER>( i )] = B_Cu;
            break;
        }

        ++it;
        wxASSERT( ( *it )->GetType() == BS_ITEM_TYPE_DIELECTRIC );
        ( *it )->SetThickness( layer.dielectricthick, 0 );
        ( *it )->SetMaterial(
                layer.dielectricmaterial.empty() ? NotSpecifiedPrm() : layer.dielectricmaterial );
        ( *it )->SetEpsilonR( layer.dielectricconst, 0 );

        ++it;
    }
}

void ALTIUM_PCB::ParseClasses6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    BOARD_DESIGN_SETTINGS& designSettings = m_board->GetDesignSettings();

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ACLASS6 elem( reader );

        if( elem.kind == ALTIUM_CLASS_KIND::NET_CLASS )
        {
            const NETCLASSPTR& netclass = std::make_shared<NETCLASS>( elem.name );
            designSettings.m_NetClasses.Add( netclass );

            for( const auto& name : elem.names )
            {
                netclass->Add(
                        name ); // TODO: it seems it can happen that we have names not attached to any net.
            }
        }
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseComponents6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    uint16_t componentId = 0;
    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ACOMPONENT6 elem( reader );

        MODULE* module = GetComponent( componentId );

        module->SetPosition( elem.position );
        module->SetOrientationDegrees( elem.rotation );
        module->SetReference( elem.sourcedesignator ); // TODO: text duplication
        module->SetLocked( elem.locked );
        module->Reference().SetVisible( elem.nameon );
        module->Value().SetVisible( elem.commenton );
        module->SetLayer( elem.layer == "TOP" ? F_Cu : B_Cu );

        componentId++;
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}


void ALTIUM_PCB::ParseDimensions6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ADIMENSION6 elem( reader );

        PCB_LAYER_ID layer = kicad_layer( altium_layer_from_name( elem.layer ) );
        if( layer == UNDEFINED_LAYER )
        {
            wxFAIL_MSG( "Ignore Dimension on layer " + elem.layer
                        + " because we do not know where to place " );
            continue;
        }

        DIMENSION* dimension = new DIMENSION( m_board );
        m_board->Add( dimension, ADD_MODE::APPEND );

        dimension->SetLayer( layer );
        dimension->SetOrigin( elem.referencePoint0, elem.textprecission );
        if( elem.referencePoint0 != elem.xy1 )
        {
            /**
             * Basically REFERENCE0POINT and REFERENCE1POINT are the two end points of the dimension.
             * XY1 is the position of the arrow above REFERENCE0POINT. those three points are not necesarily
             * in 90degree angle, but KiCad requires this to show the correct measurements.
             *
             * Therefore, we take the vector of REFERENCE0POINT -> XY1, calculate the normal, and intersect it with
             * REFERENCE1POINT pointing the same direction as REFERENCE0POINT -> XY1. This should give us a valid
             * measurement point where we can place the drawsegment.
             */
            wxPoint direction = elem.xy1 - elem.referencePoint0;
            wxPoint intersection;
            bool    hasIntersection = SegmentIntersectsSegment( elem.referencePoint0,
                    elem.referencePoint0 + VectorNorm( direction ), elem.referencePoint1,
                    elem.referencePoint1 + direction, &intersection );
            // dimension->SetEnd( intersection, elem.textprecission ); // TODO: the intersection is not correct in all cases
            dimension->SetEnd(
                    elem.referencePoint1, elem.textprecission ); // workaround until fixed

            int height = static_cast<int>( EuclideanNorm( direction ) );
            if( direction.x > 0 || direction.y < 0 )
            {
                height = -height;
            }
            dimension->SetHeight( height, elem.textprecission );
        }
        else
        {
            dimension->SetEnd( elem.referencePoint1, elem.textprecission );
        }

        dimension->SetWidth( elem.linewidth );
        // TODO: place measurement

        dimension->Text().SetThickness( elem.textlinewidth );
        dimension->Text().SetTextSize( wxSize( elem.textheight, elem.textheight ) );
        dimension->Text().SetBold( elem.textbold );
        dimension->Text().SetItalic( elem.textitalic );

        switch( elem.textunit )
        {
        case ALTIUM_UNIT::INCHES:
            dimension->SetUnits( EDA_UNITS::INCHES, false );
            break;
        case ALTIUM_UNIT::MILS:
            dimension->SetUnits( EDA_UNITS::INCHES, true );
            break;
        case ALTIUM_UNIT::MILLIMETERS:
            dimension->SetUnits( EDA_UNITS::MILLIMETRES, false );
            break;
        default:
            break;
        }

        dimension->AdjustDimensionDetails( elem.textprecission );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseNets6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    uint16_t netCode = 1; // 0 = UNCONNECTED
    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ANET6 elem( reader );

        m_board->Add( new NETINFO_ITEM( m_board, elem.name, netCode ), ADD_MODE::APPEND );

        netCode++;
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParsePolygons6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        APOLYGON6 elem( reader );

        PCB_LAYER_ID layer = kicad_layer( altium_layer_from_name( elem.layer ) );
        if( layer == UNDEFINED_LAYER )
        {
            wxFAIL_MSG( "Ignore polygon on layer " + elem.layer
                        + " because we do not know where to place " );
            continue;
        }

        ZONE_CONTAINER* zone = new ZONE_CONTAINER( m_board );
        m_board->Add( zone, ADD_MODE::APPEND );

        zone->SetNetCode( GetNetCode( elem.net ) );
        zone->SetLayer( layer );
        zone->SetPosition( elem.vertices.at( 0 ).position );
        zone->SetLocked( elem.locked );

        SHAPE_LINE_CHAIN linechain;
        for( auto& vertice : elem.vertices )
        {
            linechain.Append( vertice.position );
        }
        linechain.Append( elem.vertices.at( 0 ).position );
        linechain.SetClosed( true );

        SHAPE_POLY_SET* outline = new SHAPE_POLY_SET();
        outline->AddOutline( linechain );
        zone->SetOutline( outline );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseRules6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ARULE6 elem( reader );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseBoardRegionsData(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        AREGION6 elem( reader );

        // TODO: implement?
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseRegions6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        AREGION6 elem( reader );

        if( elem.kind == 1 ) // TODO: is kind==1 always keepout?
        {
            ZONE_CONTAINER* zone = new ZONE_CONTAINER( m_board );
            zone->SetIsKeepout( true );
            zone->SetDoNotAllowTracks( false );
            zone->SetDoNotAllowVias( false );
            zone->SetDoNotAllowCopperPour( true );
            // TODO: contours are not visible in editor?

            m_board->Add( zone, ADD_MODE::APPEND );
            // TODO: part of component

            //zone->SetNetCode( GetNetCode( elem.net ) );
            if( elem.layer == ALTIUM_LAYER::MULTI_LAYER )
            {
                zone->SetLayer( F_Cu );
                zone->SetLayerSet( LSET::AllCuMask() );
            }
            else
            {
                PCB_LAYER_ID klayer = kicad_layer( elem.layer );
                zone->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );
            }

            zone->SetPosition( elem.vertices.at( 0 ) );
            SHAPE_LINE_CHAIN linechain;
            for( auto& vertice : elem.vertices )
            {
                linechain.Append( vertice );
            }
            linechain.Append( elem.vertices.at( 0 ) );
            linechain.SetClosed( true );

            SHAPE_POLY_SET* outline = new SHAPE_POLY_SET();
            outline->AddOutline( linechain );
            zone->SetOutline( outline );
        }
        // TODO: handle other regions
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseArcs6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        AARC6 elem( reader );

        // TODO: better approach to select if item belongs to a MODULE
        DRAWSEGMENT* ds = nullptr;
        if( elem.component == std::numeric_limits<uint16_t>::max() )
        {
            ds = new DRAWSEGMENT( m_board );
            m_board->Add( ds, ADD_MODE::APPEND );
        }
        else
        {
            MODULE* module = GetComponent( elem.component );
            ds             = new EDGE_MODULE( module );
            module->Add( ds, ADD_MODE::APPEND );
        }

        ds->SetCenter( elem.center );
        ds->SetWidth( elem.width );
        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        ds->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );

        if( elem.startangle == 0. && elem.endangle == 360. )
        { // TODO: other variants to define circle?
            ds->SetShape( STROKE_T::S_CIRCLE );
            ds->SetArcStart( elem.center - wxPoint( 0, elem.radius ) );
        }
        else
        {
            ds->SetShape( STROKE_T::S_ARC );
            ds->SetAngle( -NormalizeAngleDegreesPos( elem.endangle - elem.startangle ) * 10. );

            double  startradiant = DEG2RAD( elem.startangle );
            wxPoint arcStartOffset =
                    wxPoint( static_cast<int32_t>( std::cos( startradiant ) * elem.radius ),
                            -static_cast<int32_t>( std::sin( startradiant ) * elem.radius ) );
            ds->SetArcStart( elem.center + arcStartOffset );
        }

        if( elem.component != std::numeric_limits<uint16_t>::max() )
        {
            dynamic_cast<EDGE_MODULE*>( ds )->SetLocalCoord();
        }
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParsePads6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 * 6 /* TODO: use Header section of file */ )
    {
        APAD6 elem( reader );

        // Create Pad
        MODULE* module = GetComponent( elem.component );
        D_PAD*  pad    = new D_PAD( module );
        module->Add( pad, ADD_MODE::APPEND );

        pad->SetName( elem.name );
        pad->SetNetCode( GetNetCode( elem.net ) );

        pad->SetPosition( elem.position );
        pad->SetOrientationDegrees( elem.direction );
        pad->SetLocalCoord();

        pad->SetSize( elem.topsize );

        if( elem.holesize == 0 )
        {
            wxASSERT( elem.layer != ALTIUM_LAYER::MULTI_LAYER );
            pad->SetAttribute( PAD_ATTR_T::PAD_ATTRIB_SMD );
        }
        else
        {
            wxASSERT( elem.layer
                      == ALTIUM_LAYER::
                              MULTI_LAYER ); // TODO: I assume other values are possible as well?
            pad->SetAttribute( elem.plated ? PAD_ATTR_T::PAD_ATTRIB_STANDARD :
                                             PAD_ATTR_T::PAD_ATTRIB_HOLE_NOT_PLATED );
            if( elem.sizeAndShape && elem.sizeAndShape->isslot )
            {
                pad->SetDrillShape( PAD_DRILL_SHAPE_T::PAD_DRILL_SHAPE_OBLONG );
                double normalizedSlotrotation =
                        NormalizeAngleDegreesPos( elem.sizeAndShape->slotrotation );
                if( normalizedSlotrotation == 0. || normalizedSlotrotation == 180. )
                {
                    pad->SetDrillSize( wxSize( elem.sizeAndShape->slotsize, elem.holesize ) );
                }
                else
                {
                    wxASSERT_MSG( normalizedSlotrotation == 90. || normalizedSlotrotation == 270.,
                            "slotrotation is not in 90degree angle!" );
                    pad->SetDrillSize( wxSize( elem.holesize, elem.sizeAndShape->slotsize ) );
                }
            }
            else
            {
                pad->SetDrillShape( PAD_DRILL_SHAPE_T::PAD_DRILL_SHAPE_CIRCLE );
                pad->SetDrillSize( wxSize( elem.holesize, elem.holesize ) );
            }
        }

        wxASSERT( elem.padmode == ALTIUM_PAD_MODE::SIMPLE );
        // wxASSERT( topshape == midshape == botshape );

        switch( elem.topshape )
        {
        case ALTIUM_PAD_SHAPE::RECT:
            pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_RECT );
            break;
        case ALTIUM_PAD_SHAPE::CIRCLE:
            if( elem.sizeAndShape
                    && elem.sizeAndShape->alt_shape[0] == ALTIUM_PAD_SHAPE_ALT::ROUNDRECT )
            {
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_ROUNDRECT ); // 100 = round, 0 = rectangular
                double ratio = elem.sizeAndShape->cornerradius[0] / 200.;
                pad->SetRoundRectRadiusRatio( ratio );
            }
            else if( elem.topsize.x == elem.topsize.y )
            {
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_CIRCLE );
            }
            else
            {
                pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_OVAL );
            }
            break;
        case ALTIUM_PAD_SHAPE::OCTAGONAL:
            pad->SetShape( PAD_SHAPE_T::PAD_SHAPE_CHAMFERED_RECT );
            pad->SetChamferPositions( RECT_CHAMFER_ALL );
            pad->SetChamferRectRatio( 0.25 );
            break;
        case ALTIUM_PAD_SHAPE::UNKNOWN:
        default:
            wxFAIL_MSG( "unknown shape" );
            break;
        }

        switch( elem.layer )
        {
        case ALTIUM_LAYER::TOP_LAYER:
            pad->SetLayer( F_Cu );
            pad->SetLayerSet( D_PAD::SMDMask() );
            break;
        case ALTIUM_LAYER::BOTTOM_LAYER:
            pad->SetLayer( B_Cu );
            pad->SetLayerSet( FlipLayerMask( D_PAD::SMDMask() ) );
            break;
        case ALTIUM_LAYER::MULTI_LAYER:
        default:
            pad->SetLayerSet( elem.plated ? D_PAD::StandardMask() : D_PAD::UnplatedHoleMask() );
            break;
        }

        if( elem.pastemaskexpansionmode == ALTIUM_PAD_RULE::MANUAL )
        {
            pad->SetLocalSolderPasteMargin( elem.pastemaskexpansionmanual );
        }

        if( elem.soldermaskexpansionmode == ALTIUM_PAD_RULE::MANUAL )
        {
            pad->SetLocalSolderMaskMargin( elem.soldermaskexpansionmanual );
        }

        if( elem.tenttop )
        {
            pad->SetLayerSet( pad->GetLayerSet().reset( F_Mask ) );
        }
        if( elem.tentbootom )
        {
            pad->SetLayerSet( pad->GetLayerSet().reset( B_Mask ) );
        }
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseVias6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        AVIA6 elem( reader );

        VIA* via = new VIA( m_board );
        m_board->Add( via, ADD_MODE::APPEND );

        via->SetPosition( elem.position );
        via->SetWidth( elem.diameter );
        via->SetDrill( elem.holesize );
        via->SetViaType( VIATYPE::THROUGH ); // TODO
        via->SetNetCode( GetNetCode( elem.net ) );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseTracks6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ATRACK6 elem( reader );

        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        if( klayer >= F_Cu && klayer <= B_Cu )
        {
            TRACK* track = new TRACK( m_board );
            m_board->Add( track, ADD_MODE::APPEND );

            track->SetStart( elem.start );
            track->SetEnd( elem.end );
            track->SetWidth( elem.width );
            track->SetLayer( klayer );
            track->SetNetCode( GetNetCode( elem.net ) );
        }
        else
        {
            DRAWSEGMENT* ds = nullptr;

            if( elem.component == std::numeric_limits<uint16_t>::max() )
            {
                ds = new DRAWSEGMENT( m_board );
                ds->SetShape( STROKE_T::S_SEGMENT );
                m_board->Add( ds, ADD_MODE::APPEND );

                ds->SetStart( elem.start );
                ds->SetEnd( elem.end );
            }
            else
            {
                MODULE*      module = GetComponent( elem.component );
                EDGE_MODULE* em     = new EDGE_MODULE( module, STROKE_T::S_SEGMENT );
                module->Add( em, ADD_MODE::APPEND );

                em->SetStart( elem.start );
                em->SetEnd( elem.end );
                em->SetLocalCoord();

                ds = em;
            }

            ds->SetWidth( elem.width );

            ds->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );
        }

        reader.subrecord_skip();
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseTexts6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        ATEXT6 elem( reader );

        // TODO: better approach to select if item belongs to a MODULE
        EDA_TEXT*   tx  = nullptr;
        BOARD_ITEM* itm = nullptr;
        if( elem.component == std::numeric_limits<uint16_t>::max() )
        {
            TEXTE_PCB* txp = new TEXTE_PCB( m_board );
            tx             = txp;
            itm            = txp;
            m_board->Add( txp, ADD_MODE::APPEND );
        }
        else
        {
            MODULE*       module = GetComponent( elem.component );
            TEXTE_MODULE* txm;
            if( elem.isDesignator )
            {
                txm = &module->Reference();
            }
            else if( elem.isComment )
            {
                txm = &module->Value();
            }
            else
            {
                txm = new TEXTE_MODULE( module );
                module->Add( txm, ADD_MODE::APPEND );
            }

            tx  = txm;
            itm = txm;
        }

        if( !elem.isDesignator )
        {
            if( elem.text == ".Designator" )
            {
                tx->SetText( "%R" );
            }
            else
            {
                tx->SetText( elem.text );
            }
        }

        itm->SetPosition( elem.position );
        tx->SetTextAngle( elem.rotation * 10. );
        if( elem.component != std::numeric_limits<uint16_t>::max() )
        {
            dynamic_cast<TEXTE_MODULE*>( tx )->SetLocalCoord();
        }

        PCB_LAYER_ID klayer = kicad_layer( elem.layer );
        itm->SetLayer( klayer != UNDEFINED_LAYER ? klayer : Eco1_User );

        tx->SetTextSize( wxSize( elem.height, elem.height ) ); // TODO: parse text width
        tx->SetThickness( elem.strokewidth );
        tx->SetMirrored( elem.mirrored );
        switch( elem.textposition )
        {
        case ALTIUM_TEXT_POSITION::LEFT_TOP:
        case ALTIUM_TEXT_POSITION::LEFT_CENTER:
        case ALTIUM_TEXT_POSITION::LEFT_BOTTOM:
            tx->SetHorizJustify( EDA_TEXT_HJUSTIFY_T::GR_TEXT_HJUSTIFY_LEFT );
            break;
        case ALTIUM_TEXT_POSITION::CENTER_TOP:
        case ALTIUM_TEXT_POSITION::CENTER_CENTER:
        case ALTIUM_TEXT_POSITION::CENTER_BOTTOM:
            tx->SetHorizJustify( EDA_TEXT_HJUSTIFY_T::GR_TEXT_HJUSTIFY_CENTER );
            break;
        case ALTIUM_TEXT_POSITION::RIGHT_TOP:
        case ALTIUM_TEXT_POSITION::RIGHT_CENTER:
        case ALTIUM_TEXT_POSITION::RIGHT_BOTTOM:
            tx->SetHorizJustify( EDA_TEXT_HJUSTIFY_T::GR_TEXT_HJUSTIFY_RIGHT );
            break;
        default:
            wxFAIL_MSG( "unexpected horizontal text position" );
            break;
        }

        switch( elem.textposition )
        {
        case ALTIUM_TEXT_POSITION::LEFT_TOP:
        case ALTIUM_TEXT_POSITION::CENTER_TOP:
        case ALTIUM_TEXT_POSITION::RIGHT_TOP:
            tx->SetVertJustify( EDA_TEXT_VJUSTIFY_T::GR_TEXT_VJUSTIFY_TOP );
            break;
        case ALTIUM_TEXT_POSITION::LEFT_CENTER:
        case ALTIUM_TEXT_POSITION::CENTER_CENTER:
        case ALTIUM_TEXT_POSITION::RIGHT_CENTER:
            tx->SetVertJustify( EDA_TEXT_VJUSTIFY_T::GR_TEXT_VJUSTIFY_CENTER );
            break;
        case ALTIUM_TEXT_POSITION::LEFT_BOTTOM:
        case ALTIUM_TEXT_POSITION::CENTER_BOTTOM:
        case ALTIUM_TEXT_POSITION::RIGHT_BOTTOM:
            tx->SetVertJustify( EDA_TEXT_VJUSTIFY_T::GR_TEXT_VJUSTIFY_BOTTOM );
            break;
        default:
            wxFAIL_MSG( "unexpected vertical text position" );
            break;
        }

        wxASSERT( !reader.parser_error() );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}

void ALTIUM_PCB::ParseFills6Data(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    ALTIUM_PARSER reader( aReader, aEntry );

    while( !reader.parser_error()
            && reader.bytes_remaining() >= 4 /* TODO: use Header section of file */ )
    {
        AFILL6 elem( reader );

        ZONE_CONTAINER* zone = new ZONE_CONTAINER( m_board );
        m_board->Add( zone, ADD_MODE::APPEND );

        zone->SetNetCode( GetNetCode( elem.net ) );
        zone->SetLayer( kicad_layer( elem.layer ) );
        zone->SetPosition( elem.pos1 );

        SHAPE_LINE_CHAIN linechain;
        linechain.Append( elem.pos1.x, elem.pos1.y );
        linechain.Append( elem.pos2.x, elem.pos1.y );
        linechain.Append( elem.pos2.x, elem.pos2.y );
        linechain.Append( elem.pos1.x, elem.pos2.y );
        linechain.Append( elem.pos1.x, elem.pos1.y );
        linechain.SetClosed( true );

        SHAPE_POLY_SET* outline = new SHAPE_POLY_SET();
        outline->AddOutline( linechain );
        zone->SetOutline( outline );
    }

    wxASSERT( !reader.parser_error() );
    wxASSERT( reader.bytes_remaining() == 0 );
}


ABOARD6::ABOARD6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    /*for (auto & property : properties) {
        std::cout << "  * '" << property.first << "' = '" << property.second << "'" << std::endl;
    }*/

    sheetpos  = wxPoint( ALTIUM_PARSER::property_unit( properties, "SHEETX", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "SHEETY", "0mil" ) );
    sheetsize = wxSize( ALTIUM_PARSER::property_unit( properties, "SHEETWIDTH", "0mil" ),
            ALTIUM_PARSER::property_unit( properties, "SHEETHEIGHT", "0mil" ) );

    layercount = ALTIUM_PARSER::property_int( properties, "LAYERSETSCOUNT", 1 ) + 1;

    for( size_t i = 1; i < std::numeric_limits<size_t>::max(); i++ )
    {
        const std::string layeri    = "LAYER" + std::to_string( i );
        const std::string layername = layeri + "NAME";

        auto layernameit = properties.find( layername );
        if( layernameit == properties.end() )
        {
            break; // it doesn't seem like we know beforehand how many vertices are inside a polygon
        }

        ABOARD6_LAYER_STACKUP curlayer;

        curlayer.name =
                ALTIUM_PARSER::property_string( properties, layername, "" ); // TODO: trim string
        curlayer.nextId = ALTIUM_PARSER::property_int( properties, layeri + "NEXT", 0 );
        curlayer.prevId = ALTIUM_PARSER::property_int( properties, layeri + "PREV", 0 );
        curlayer.copperthick =
                ALTIUM_PARSER::property_unit( properties, layeri + "COPTHICK", "1.4mil" );

        curlayer.dielectricconst =
                ALTIUM_PARSER::property_double( properties, layeri + "DIELCONST", 0. );
        curlayer.dielectricthick =
                ALTIUM_PARSER::property_unit( properties, layeri + "DIELHEIGHT", "60mil" );
        curlayer.dielectricmaterial =
                ALTIUM_PARSER::property_string( properties, layeri + "DIELMATERIAL", "FR-4" );

        stackup.push_back( curlayer );
    }
}

ACLASS6::ACLASS6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    name     = ALTIUM_PARSER::property_string( properties, "NAME", "" );
    uniqueid = ALTIUM_PARSER::property_string( properties, "UNIQUEID", "" );
    kind = static_cast<ALTIUM_CLASS_KIND>( ALTIUM_PARSER::property_int( properties, "KIND", -1 ) );

    for( size_t i = 0; i < std::numeric_limits<size_t>::max(); i++ )
    {
        auto mit = properties.find( "M" + std::to_string( i ) );
        if( mit == properties.end() )
        {
            break; // it doesn't seem like we know beforehand how many components are in the netclass
        }
        names.push_back( mit->second );
    }
}

ACOMPONENT6::ACOMPONENT6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    layer              = ALTIUM_PARSER::property_string( properties, "LAYER", "" );
    position           = wxPoint( ALTIUM_PARSER::property_unit( properties, "X", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "Y", "0mil" ) );
    rotation           = ALTIUM_PARSER::property_double( properties, "ROTATION", 0. );
    locked             = ALTIUM_PARSER::property_bool( properties, "LOCKED", false );
    nameon             = ALTIUM_PARSER::property_bool( properties, "NAMEON", true );
    commenton          = ALTIUM_PARSER::property_bool( properties, "COMMENTON", false );
    sourcedesignator   = ALTIUM_PARSER::property_string( properties, "SOURCEDESIGNATOR", "" );
    sourcelibreference = ALTIUM_PARSER::property_string( properties, "SOURCELIBREFERENCE", "" );
}

ADIMENSION6::ADIMENSION6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 6 );
    wxASSERT( !reader.parser_error() );

    reader.skip( 2 );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    layer          = ALTIUM_PARSER::property_string( properties, "LAYER", "" );
    linewidth      = ALTIUM_PARSER::property_unit( properties, "LINEWIDTH", "10mil" );
    textheight     = ALTIUM_PARSER::property_unit( properties, "TEXTHEIGHT", "10mil" );
    textlinewidth  = ALTIUM_PARSER::property_unit( properties, "TEXTLINEWIDTH", "6mil" );
    textprecission = ALTIUM_PARSER::property_int( properties, "TEXTPRECISION", 2 );
    textbold       = ALTIUM_PARSER::property_bool( properties, "TEXTLINEWIDTH", false );
    textitalic     = ALTIUM_PARSER::property_bool( properties, "ITALIC", false );
    referencePoint0 =
            wxPoint( ALTIUM_PARSER::property_unit( properties, "REFERENCE0POINTX", "0mil" ),
                    -ALTIUM_PARSER::property_unit( properties, "REFERENCE0POINTY", "0mil" ) );
    referencePoint1 =
            wxPoint( ALTIUM_PARSER::property_unit( properties, "REFERENCE1POINTX", "0mil" ),
                    -ALTIUM_PARSER::property_unit( properties, "REFERENCE1POINTY", "0mil" ) );
    xy1 = wxPoint( ALTIUM_PARSER::property_unit( properties, "X1", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "Y1", "0mil" ) );

    std::string dimensionunit =
            ALTIUM_PARSER::property_string( properties, "TEXTDIMENSIONUNIT", "Millimeters" );
    if( dimensionunit == "Inches" )
    {
        textunit = ALTIUM_UNIT::INCHES;
    }
    else if( dimensionunit == "Mils" )
    {
        textunit = ALTIUM_UNIT::MILS;
    }
    else if( dimensionunit == "Millimeters" )
    {
        textunit = ALTIUM_UNIT::MILLIMETERS;
    }
    else
    {
        textunit = ALTIUM_UNIT::UNKNOWN;
    }
}

ANET6::ANET6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    name = ALTIUM_PARSER::property_string( properties, "NAME", "" );
}

APOLYGON6::APOLYGON6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    layer = ALTIUM_PARSER::property_string( properties, "LAYER", "" );
    net    = ALTIUM_PARSER::property_int( properties, "NET", std::numeric_limits<uint16_t>::max() );
    locked = ALTIUM_PARSER::property_bool( properties, "LOCKED", false );

    for( size_t i = 0; i < std::numeric_limits<size_t>::max(); i++ )
    {
        const std::string vxi = "VX" + std::to_string( i );
        const std::string vyi = "VY" + std::to_string( i );

        auto vxit = properties.find( vxi );
        auto vyit = properties.find( vyi );
        if( vxit == properties.end() || vyit == properties.end() )
        {
            break; // it doesn't seem like we know beforehand how many vertices are inside a polygon
        }

        const wxPoint vertice_position =
                wxPoint( ALTIUM_PARSER::property_unit( properties, vxi, "0mil" ),
                        -ALTIUM_PARSER::property_unit( properties, vyi, "0mil" ) );

        vertices.emplace_back( vertice_position );
    }
}

ARULE6::ARULE6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    reader.skip( 2 );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    name     = ALTIUM_PARSER::property_string( properties, "NAME", "" );
    priority = ALTIUM_PARSER::property_int( properties, "PRIORITY", 1 );

    std::string rulekind = ALTIUM_PARSER::property_string( properties, "RULEKIND", "" );
    if( rulekind == "Clearance" )
    {
        kind = ALTIUM_RULE_KIND::CLEARANCE;
    }
    else if( rulekind == "DiffPairsRouting" )
    {
        kind = ALTIUM_RULE_KIND::DIFF_PAIR_ROUTINGS;
    }
    else if( rulekind == "Height" )
    {
        kind = ALTIUM_RULE_KIND::HEIGHT;
    }
    else if( rulekind == "HoleSize" )
    {
        kind = ALTIUM_RULE_KIND::HOLE_SIZE;
    }
    else if( rulekind == "HoleToHoleClearance" )
    {
        kind = ALTIUM_RULE_KIND::HOLE_TO_HOLE_CLEARANCE;
    }
    else if( rulekind == "Width" )
    {
        kind = ALTIUM_RULE_KIND::WIDTH;
    }
    else if( rulekind == "PasteMaskExpansion" )
    {
        kind = ALTIUM_RULE_KIND::PASTE_MASK_EXPANSION;
    }
    else
    {
        kind = ALTIUM_RULE_KIND::UNKNOWN;
    }
}

AARC6::AARC6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::ARC );

    // Subrecord 1
    reader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
    reader.skip( 2 );
    net = reader.read<uint16_t>();
    reader.skip( 2 );
    component = reader.read<uint16_t>();
    reader.skip( 4 );
    center     = reader.read_point();
    radius     = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );
    startangle = reader.read<double>();
    endangle   = reader.read<double>();
    width      = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

APAD6::APAD6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
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
    wxASSERT( subrecord5 >= 120 ); // TODO: exact minimum length we know?

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );

    uint8_t flags1  = reader.read<uint8_t>();
    tentbootom      = ( flags1 & 0x40 ) != 0;
    tenttop         = ( flags1 & 0x20 ) != 0;

    reader.skip( 1 );
    net = reader.read<uint16_t>();
    reader.skip( 2 );
    component = reader.read<uint16_t>();
    reader.skip( 4 );

    position = reader.read_point();
    topsize  = reader.read_size();
    midsize  = reader.read_size();
    botsize  = reader.read_size();
    holesize = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );

    topshape = static_cast<ALTIUM_PAD_SHAPE>( reader.read<uint8_t>() );
    midshape = static_cast<ALTIUM_PAD_SHAPE>( reader.read<uint8_t>() );
    botshape = static_cast<ALTIUM_PAD_SHAPE>( reader.read<uint8_t>() );

    direction = reader.read<double>();
    plated    = reader.read<uint8_t>() != 0;
    reader.skip( 1 );
    padmode = static_cast<ALTIUM_PAD_MODE>( reader.read<uint8_t>() );
    reader.skip( 23 );
    pastemaskexpansionmanual  = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
    soldermaskexpansionmanual = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
    reader.skip( 7 );
    pastemaskexpansionmode  = static_cast<ALTIUM_PAD_RULE>( reader.read<uint8_t>() );
    soldermaskexpansionmode = static_cast<ALTIUM_PAD_RULE>( reader.read<uint8_t>() );
    reader.skip( 3 );
    holerotation = reader.read<double>();
    if( subrecord5 == 120 )
    {
        tolayer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
        reader.skip( 2 );
        fromlayer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
        //reader.skip( 2 );
    }
    else if( subrecord5 == 171 )
    {
    }
    reader.subrecord_skip();

    // Subrecord 6
    size_t subrecord6 = reader.read_subrecord_length();
    if( subrecord6 == 651
            || subrecord6 == 628 ) // TODO: better detection mechanism (Altium 14 = 628)
    { // TODO: detect type from something else than the size?
        sizeAndShape = std::make_unique<APAD6_SIZE_AND_SHAPE>();

        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_size[i].x = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
        }
        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_size[i].y = -ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
        }

        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_shape[i] = static_cast<ALTIUM_PAD_SHAPE>( reader.read<uint8_t>() );
        }

        reader.skip( 1 );

        sizeAndShape->isslot       = reader.read<uint8_t>() == 0x02;
        sizeAndShape->slotsize     = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
        sizeAndShape->slotrotation = reader.read<double>();

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->holeoffset[i].x = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
        }
        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->holeoffset[i].y = ALTIUM_PARSER::kicad_unit( reader.read<int32_t>() );
        }

        reader.skip( 1 );

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->alt_shape[i] =
                    static_cast<ALTIUM_PAD_SHAPE_ALT>( reader.read<uint8_t>() );
        }

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->cornerradius[i] = reader.read<uint8_t>();
        }
    }

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

AVIA6::AVIA6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::VIA );

    // Subrecord 1
    reader.read_subrecord_length();

    reader.skip( 3 );
    net = reader.read<uint16_t>();
    reader.skip( 8 );
    position = reader.read_point();
    diameter = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );
    holesize = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

ATRACK6::ATRACK6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::TRACK );

    // Subrecord 1
    reader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
    reader.skip( 2 );
    net = reader.read<uint16_t>();
    reader.skip( 2 );
    component = reader.read<uint16_t>();
    reader.skip( 4 );
    start = reader.read_point();
    end   = reader.read_point();
    width = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

ATEXT6::ATEXT6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::TEXT );

    // Subrecord 1 - Properties
    size_t subrecord1 = reader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
    reader.skip( 6 );
    component = reader.read<uint16_t>();
    reader.skip( 4 );
    position = reader.read_point();
    height   = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );
    reader.skip( 2 );
    rotation     = reader.read<double>();
    mirrored     = reader.read<uint8_t>() != 0;
    strokewidth  = ALTIUM_PARSER::kicad_unit( reader.read<uint32_t>() );
    isComment    = reader.read<uint8_t>() != 0;
    isDesignator = reader.read<uint8_t>() != 0;
    if( subrecord1 > 230 )
    {
        reader.skip( 90 );
        textposition = static_cast<ALTIUM_TEXT_POSITION>( reader.read<uint8_t>() );
    }
    else
    {
        /**
         * In Altium 14 (subrecord1 == 230) only left bottom is valid?
         * https://gitlab.com/kicad/code/kicad/merge_requests/60#note_274913397
         */
        textposition = ALTIUM_TEXT_POSITION::LEFT_BOTTOM;
    }


    reader.subrecord_skip();

    // Subrecord 2 - String
    reader.read_subrecord_length();

    text = reader.read_string(); // TODO: what about strings with length > 255?

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

AFILL6::AFILL6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::FILL );

    // Subrecord 1
    reader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
    reader.skip( 2 );
    net = reader.read<uint16_t>();
    reader.skip( 8 );
    pos1     = reader.read_point();
    pos2     = reader.read_point();
    rotation = reader.read<double>();

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}

AREGION6::AREGION6( ALTIUM_PARSER& reader )
{
    wxASSERT( reader.bytes_remaining() > 4 );
    wxASSERT( !reader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( reader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::REGION );

    // Subrecord 1
    reader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( reader.read<uint8_t>() );
    reader.skip( 2 );
    net = reader.read<uint16_t>();
    reader.skip( 2 );
    component = reader.read<uint16_t>();
    reader.skip( 9 );

    std::map<std::string, std::string> properties = reader.read_properties();
    wxASSERT( !properties.empty() );

    kind          = ALTIUM_PARSER::property_int( properties, "KIND", 0 );
    iskeepout     = ALTIUM_PARSER::property_bool( properties, "KEEPOUT", false );
    isboardcutout = ALTIUM_PARSER::property_bool( properties, "ISBOARDCUTOUT", false );

    uint32_t num_vertices = reader.read<uint32_t>();

    for( uint32_t i = 0; i < num_vertices; i++ )
    {
        // no idea why, but for regions the coordinates are stored as double and not as int32_t
        double x = reader.read<double>();
        double y = -reader.read<double>();

        vertices.emplace_back( x * 2.54, y * 2.54 );
    }

    reader.subrecord_skip();

    wxASSERT( !reader.parser_error() );
}
