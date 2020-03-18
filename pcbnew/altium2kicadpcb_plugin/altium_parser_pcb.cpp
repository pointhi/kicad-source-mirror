/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2020 Thomas Pointhuber <thomas.pointhuber@gmx.at>
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

#include <map>
#include <unordered_map>

#include <math/util.h>

#include "altium_parser.h"
#include "altium_parser_pcb.h"


ALTIUM_LAYER altium_layer_from_name( const wxString& aName )
{
    static const std::unordered_map<std::string, ALTIUM_LAYER> hash_map = {
        { "TOP", ALTIUM_LAYER::TOP_LAYER },
        { "MID1", ALTIUM_LAYER::MID_LAYER_1 },
        { "MID2", ALTIUM_LAYER::MID_LAYER_2 },
        { "MID3", ALTIUM_LAYER::MID_LAYER_3 },
        { "MID4", ALTIUM_LAYER::MID_LAYER_4 },
        { "MID5", ALTIUM_LAYER::MID_LAYER_5 },
        { "MID6", ALTIUM_LAYER::MID_LAYER_6 },
        { "MID7", ALTIUM_LAYER::MID_LAYER_7 },
        { "MID8", ALTIUM_LAYER::MID_LAYER_8 },
        { "MID9", ALTIUM_LAYER::MID_LAYER_9 },
        { "MID10", ALTIUM_LAYER::MID_LAYER_10 },
        { "MID11", ALTIUM_LAYER::MID_LAYER_11 },
        { "MID12", ALTIUM_LAYER::MID_LAYER_12 },
        { "MID13", ALTIUM_LAYER::MID_LAYER_13 },
        { "MID14", ALTIUM_LAYER::MID_LAYER_14 },
        { "MID15", ALTIUM_LAYER::MID_LAYER_15 },
        { "MID16", ALTIUM_LAYER::MID_LAYER_16 },
        { "MID17", ALTIUM_LAYER::MID_LAYER_17 },
        { "MID18", ALTIUM_LAYER::MID_LAYER_18 },
        { "MID19", ALTIUM_LAYER::MID_LAYER_19 },
        { "MID20", ALTIUM_LAYER::MID_LAYER_20 },
        { "MID21", ALTIUM_LAYER::MID_LAYER_21 },
        { "MID22", ALTIUM_LAYER::MID_LAYER_22 },
        { "MID23", ALTIUM_LAYER::MID_LAYER_23 },
        { "MID24", ALTIUM_LAYER::MID_LAYER_24 },
        { "MID25", ALTIUM_LAYER::MID_LAYER_25 },
        { "MID26", ALTIUM_LAYER::MID_LAYER_26 },
        { "MID27", ALTIUM_LAYER::MID_LAYER_27 },
        { "MID28", ALTIUM_LAYER::MID_LAYER_28 },
        { "MID29", ALTIUM_LAYER::MID_LAYER_29 },
        { "MID30", ALTIUM_LAYER::MID_LAYER_30 },
        { "BOTTOM", ALTIUM_LAYER::BOTTOM_LAYER },

        { "PLANE1", ALTIUM_LAYER::INTERNAL_PLANE_1 },
        { "PLANE2", ALTIUM_LAYER::INTERNAL_PLANE_2 },
        { "PLANE3", ALTIUM_LAYER::INTERNAL_PLANE_3 },
        { "PLANE4", ALTIUM_LAYER::INTERNAL_PLANE_4 },
        { "PLANE5", ALTIUM_LAYER::INTERNAL_PLANE_5 },
        { "PLANE6", ALTIUM_LAYER::INTERNAL_PLANE_6 },
        { "PLANE7", ALTIUM_LAYER::INTERNAL_PLANE_7 },
        { "PLANE8", ALTIUM_LAYER::INTERNAL_PLANE_8 },
        { "PLANE9", ALTIUM_LAYER::INTERNAL_PLANE_9 },
        { "PLANE10", ALTIUM_LAYER::INTERNAL_PLANE_10 },
        { "PLANE11", ALTIUM_LAYER::INTERNAL_PLANE_11 },
        { "PLANE12", ALTIUM_LAYER::INTERNAL_PLANE_12 },
        { "PLANE13", ALTIUM_LAYER::INTERNAL_PLANE_13 },
        { "PLANE14", ALTIUM_LAYER::INTERNAL_PLANE_14 },
        { "PLANE15", ALTIUM_LAYER::INTERNAL_PLANE_15 },
        { "PLANE16", ALTIUM_LAYER::INTERNAL_PLANE_16 },

        { "MECHANICAL1", ALTIUM_LAYER::MECHANICAL_1 },
        { "MECHANICAL2", ALTIUM_LAYER::MECHANICAL_2 },
        { "MECHANICAL3", ALTIUM_LAYER::MECHANICAL_3 },
        { "MECHANICAL4", ALTIUM_LAYER::MECHANICAL_4 },
        { "MECHANICAL5", ALTIUM_LAYER::MECHANICAL_5 },
        { "MECHANICAL6", ALTIUM_LAYER::MECHANICAL_6 },
        { "MECHANICAL7", ALTIUM_LAYER::MECHANICAL_7 },
        { "MECHANICAL8", ALTIUM_LAYER::MECHANICAL_8 },
        { "MECHANICAL9", ALTIUM_LAYER::MECHANICAL_9 },
        { "MECHANICAL10", ALTIUM_LAYER::MECHANICAL_10 },
        { "MECHANICAL11", ALTIUM_LAYER::MECHANICAL_11 },
        { "MECHANICAL12", ALTIUM_LAYER::MECHANICAL_12 },
        { "MECHANICAL13", ALTIUM_LAYER::MECHANICAL_13 },
        { "MECHANICAL14", ALTIUM_LAYER::MECHANICAL_14 },
        { "MECHANICAL15", ALTIUM_LAYER::MECHANICAL_15 },
        { "MECHANICAL16", ALTIUM_LAYER::MECHANICAL_16 },
    };

    auto it = hash_map.find( std::string( aName.c_str() ) );
    if( it == hash_map.end() )
    {
        return ALTIUM_LAYER::UNKNOWN;
    }
    else
    {
        return it->second;
    }
}

void altium_parse_polygons(
        std::map<wxString, wxString>& aProperties, std::vector<ALTIUM_VERTICE>& aVertices )
{
    for( size_t i = 0; i < std::numeric_limits<size_t>::max(); i++ )
    {
        const wxString si = std::to_string( i );

        const wxString vxi = "VX" + si;
        const wxString vyi = "VY" + si;

        if( aProperties.find( vxi ) == aProperties.end()
                || aProperties.find( vyi ) == aProperties.end() )
        {
            break; // it doesn't seem like we know beforehand how many vertices are inside a polygon
        }

        const bool    isRound = ALTIUM_PARSER::property_int( aProperties, "KIND" + si, 0 ) != 0;
        const int32_t radius  = ALTIUM_PARSER::property_unit( aProperties, "R" + si, "0mil" );
        const double  sa      = ALTIUM_PARSER::property_double( aProperties, "SA" + si, 0. );
        const double  ea      = ALTIUM_PARSER::property_double( aProperties, "EA" + si, 0. );
        const wxPoint vp      = wxPoint( ALTIUM_PARSER::property_unit( aProperties, vxi, "0mil" ),
                -ALTIUM_PARSER::property_unit( aProperties, vyi, "0mil" ) );
        const wxPoint cp = wxPoint( ALTIUM_PARSER::property_unit( aProperties, "CX" + si, "0mil" ),
                -ALTIUM_PARSER::property_unit( aProperties, "CY" + si, "0mil" ) );

        aVertices.emplace_back( isRound, radius, sa, ea, vp, cp );
    }
}

ABOARD6::ABOARD6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    std::map<wxString, wxString> properties = aReader.read_properties();
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
        const wxString layeri    = "LAYER" + std::to_string( i );
        const wxString layername = layeri + "NAME";

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

    altium_parse_polygons( properties, board_vertices );
}

ACLASS6::ACLASS6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    std::map<wxString, wxString> properties = aReader.read_properties();
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

ACOMPONENT6::ACOMPONENT6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    layer     = altium_layer_from_name( ALTIUM_PARSER::property_string( properties, "LAYER", "" ) );
    position  = wxPoint( ALTIUM_PARSER::property_unit( properties, "X", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "Y", "0mil" ) );
    rotation  = ALTIUM_PARSER::property_double( properties, "ROTATION", 0. );
    locked    = ALTIUM_PARSER::property_bool( properties, "LOCKED", false );
    nameon    = ALTIUM_PARSER::property_bool( properties, "NAMEON", true );
    commenton = ALTIUM_PARSER::property_bool( properties, "COMMENTON", false );
    sourcedesignator   = ALTIUM_PARSER::property_string( properties, "SOURCEDESIGNATOR", "" );
    sourcelibreference = ALTIUM_PARSER::property_string( properties, "SOURCELIBREFERENCE", "" );
}

ADIMENSION6::ADIMENSION6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 6 );
    wxASSERT( !aReader.parser_error() );

    aReader.skip( 2 );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    layer = altium_layer_from_name( ALTIUM_PARSER::property_string( properties, "LAYER", "" ) );
    kind  = static_cast<ALTIUM_DIMENSION_KIND>(
            ALTIUM_PARSER::property_int( properties, "DIMENSIONKIND", 0 ) );

    textformat = ALTIUM_PARSER::property_string( properties, "TEXTFORMAT", "" );

    height = ALTIUM_PARSER::property_unit( properties, "HEIGHT", "0mil" );
    angle  = ALTIUM_PARSER::property_double( properties, "ANGLE", 0. );

    linewidth      = ALTIUM_PARSER::property_unit( properties, "LINEWIDTH", "10mil" );
    textheight     = ALTIUM_PARSER::property_unit( properties, "TEXTHEIGHT", "10mil" );
    textlinewidth  = ALTIUM_PARSER::property_unit( properties, "TEXTLINEWIDTH", "6mil" );
    textprecission = ALTIUM_PARSER::property_int( properties, "TEXTPRECISION", 2 );
    textbold       = ALTIUM_PARSER::property_bool( properties, "TEXTLINEWIDTH", false );
    textitalic     = ALTIUM_PARSER::property_bool( properties, "ITALIC", false );

    arrowsize = ALTIUM_PARSER::property_unit( properties, "ARROWSIZE", "60mil" );

    xy1 = wxPoint( ALTIUM_PARSER::property_unit( properties, "X1", "0mil" ),
            -ALTIUM_PARSER::property_unit( properties, "Y1", "0mil" ) );

    int refcount = ALTIUM_PARSER::property_int( properties, "REFERENCES_COUNT", 0 );
    for( int i = 0; i < refcount; i++ )
    {
        const std::string refi = "REFERENCE" + std::to_string( i );
        referencePoint.emplace_back(
                ALTIUM_PARSER::property_unit( properties, refi + "POINTX", "0mil" ),
                -ALTIUM_PARSER::property_unit( properties, refi + "POINTY", "0mil" ) );
    }

    for( size_t i = 1; i < std::numeric_limits<size_t>::max(); i++ )
    {
        const std::string texti  = "TEXT" + std::to_string( i );
        const std::string textix = texti + "X";
        const std::string textiy = texti + "Y";

        if( properties.find( textix ) == properties.end()
                || properties.find( textiy ) == properties.end() )
        {
            break; // it doesn't seem like we know beforehand how many vertices are inside a polygon
        }

        textPoint.emplace_back( ALTIUM_PARSER::property_unit( properties, textix, "0mil" ),
                -ALTIUM_PARSER::property_unit( properties, textiy, "0mil" ) );
    }

    wxString dimensionunit =
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
    else if( dimensionunit == "Centimeters" )
    {
        textunit = ALTIUM_UNIT::CENTIMETER;
    }
    else
    {
        textunit = ALTIUM_UNIT::UNKNOWN;
    }
}

ANET6::ANET6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    name = ALTIUM_PARSER::property_string( properties, "NAME", "" );
}

APOLYGON6::APOLYGON6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    layer  = altium_layer_from_name( ALTIUM_PARSER::property_string( properties, "LAYER", "" ) );
    net    = ALTIUM_PARSER::property_int( properties, "NET", std::numeric_limits<uint16_t>::max() );
    locked = ALTIUM_PARSER::property_bool( properties, "LOCKED", false );

    // TODO: kind

    gridsize      = ALTIUM_PARSER::property_unit( properties, "GRIDSIZE", "0mil" );
    trackwidth    = ALTIUM_PARSER::property_unit( properties, "TRACKWIDTH", "0mil" );
    minprimlength = ALTIUM_PARSER::property_unit( properties, "MINPRIMLENGTH", "0mil" );
    useoctagons   = ALTIUM_PARSER::property_bool( properties, "USEOCTAGONS", false );

    wxString hatchstyleraw = ALTIUM_PARSER::property_string( properties, "HATCHSTYLE", "" );
    if( hatchstyleraw == "Solid" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::SOLID;
    }
    else if( hatchstyleraw == "45Degree" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::DEGREE_45;
    }
    else if( hatchstyleraw == "90Degree" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::DEGREE_90;
    }
    else if( hatchstyleraw == "Horizontal" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::HORIZONTAL;
    }
    else if( hatchstyleraw == "Vertical" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::VERTICAL;
    }
    else if( hatchstyleraw == "None" )
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::NONE;
    }
    else
    {
        hatchstyle = ALTIUM_POLYGON_HATCHSTYLE::UNKNOWN;
    }

    altium_parse_polygons( properties, vertices );
}

ARULE6::ARULE6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    aReader.skip( 2 );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    name     = ALTIUM_PARSER::property_string( properties, "NAME", "" );
    priority = ALTIUM_PARSER::property_int( properties, "PRIORITY", 1 );

    scope1expr = ALTIUM_PARSER::property_string( properties, "SCOPE1EXPRESSION", "" );
    scope2expr = ALTIUM_PARSER::property_string( properties, "SCOPE2EXPRESSION", "" );

    wxString rulekind = ALTIUM_PARSER::property_string( properties, "RULEKIND", "" );
    if( rulekind == "Clearance" )
    {
        kind         = ALTIUM_RULE_KIND::CLEARANCE;
        clearanceGap = ALTIUM_PARSER::property_unit( properties, "GAP", "10mil" );
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
    else if( rulekind == "PlaneClearance" )
    {
        kind                    = ALTIUM_RULE_KIND::PLANE_CLEARANCE;
        planeclearanceClearance = ALTIUM_PARSER::property_unit( properties, "CLEARANCE", "10mil" );
    }
    else if( rulekind == "PolygonConnect" )
    {
        kind = ALTIUM_RULE_KIND::POLYGON_CONNECT;
        polygonconnectAirgapwidth =
                ALTIUM_PARSER::property_unit( properties, "AIRGAPWIDTH", "10mil" );
        polygonconnectReliefconductorwidth =
                ALTIUM_PARSER::property_unit( properties, "RELIEFCONDUCTORWIDTH", "10mil" );
        polygonconnectReliefentries = ALTIUM_PARSER::property_int( properties, "RELIEFENTRIES", 4 );
    }
    else
    {
        kind = ALTIUM_RULE_KIND::UNKNOWN;
    }
}

AARC6::AARC6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::ARC );

    // Subrecord 1
    aReader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );

    uint8_t flags1 = aReader.read<uint8_t>();
    is_locked      = ( flags1 & 0x04 ) == 0;

    uint8_t flags2 = aReader.read<uint8_t>();
    is_keepout     = flags2 == 2;

    net = aReader.read<uint16_t>();
    aReader.skip( 2 );
    component = aReader.read<uint16_t>();
    aReader.skip( 4 );
    center     = aReader.read_point();
    radius     = aReader.read_unit();
    startangle = aReader.read<double>();
    endangle   = aReader.read<double>();
    width      = aReader.read_unit();

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

APAD6::APAD6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::PAD );

    // Subrecord 1
    size_t subrecord1 = aReader.read_subrecord_length();
    wxASSERT( subrecord1 > 0 );
    name = aReader.read_string();
    wxASSERT( aReader.subrecord_remaining() == 0 );
    aReader.subrecord_skip();

    // Subrecord 2
    aReader.read_subrecord_length();
    aReader.subrecord_skip();

    // Subrecord 3
    aReader.read_subrecord_length();
    aReader.subrecord_skip();

    // Subrecord 4
    aReader.read_subrecord_length();
    aReader.subrecord_skip();

    // Subrecord 5
    size_t subrecord5 = aReader.read_subrecord_length();
    wxASSERT( subrecord5 >= 120 ); // TODO: exact minimum length we know?

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );

    uint8_t flags1  = aReader.read<uint8_t>();
    is_test_fab_top = ( flags1 & 0x80 ) != 0;
    is_tent_bottom  = ( flags1 & 0x40 ) != 0;
    is_tent_top     = ( flags1 & 0x20 ) != 0;
    is_locked       = ( flags1 & 0x04 ) == 0;

    uint8_t flags2     = aReader.read<uint8_t>();
    is_test_fab_bottom = ( flags2 & 0x01 ) != 0;

    net = aReader.read<uint16_t>();
    aReader.skip( 2 );
    component = aReader.read<uint16_t>();
    aReader.skip( 4 );

    position = aReader.read_point();
    topsize  = aReader.read_size();
    midsize  = aReader.read_size();
    botsize  = aReader.read_size();
    holesize = aReader.read_unit();

    topshape = static_cast<ALTIUM_PAD_SHAPE>( aReader.read<uint8_t>() );
    midshape = static_cast<ALTIUM_PAD_SHAPE>( aReader.read<uint8_t>() );
    botshape = static_cast<ALTIUM_PAD_SHAPE>( aReader.read<uint8_t>() );

    direction = aReader.read<double>();
    plated    = aReader.read<uint8_t>() != 0;
    aReader.skip( 1 );
    padmode = static_cast<ALTIUM_PAD_MODE>( aReader.read<uint8_t>() );
    aReader.skip( 23 );
    pastemaskexpansionmanual  = aReader.read_unit();
    soldermaskexpansionmanual = aReader.read_unit();
    aReader.skip( 7 );
    pastemaskexpansionmode  = static_cast<ALTIUM_PAD_RULE>( aReader.read<uint8_t>() );
    soldermaskexpansionmode = static_cast<ALTIUM_PAD_RULE>( aReader.read<uint8_t>() );
    aReader.skip( 3 );
    holerotation = aReader.read<double>();
    if( subrecord5 == 120 )
    {
        tolayer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );
        aReader.skip( 2 );
        fromlayer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );
        //aReader.skip( 2 );
    }
    else if( subrecord5 == 171 )
    {
    }
    aReader.subrecord_skip();

    // Subrecord 6
    size_t subrecord6 = aReader.read_subrecord_length();
    if( subrecord6 == 651
            || subrecord6 == 628 ) // TODO: better detection mechanism (Altium 14 = 628)
    {                              // TODO: detect type from something else than the size?
        sizeAndShape = std::make_unique<APAD6_SIZE_AND_SHAPE>();

        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_size[i].x = aReader.read_unit_x();
        }
        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_size[i].y = aReader.read_unit_y();
        }

        for( int i = 0; i < 29; i++ )
        {
            sizeAndShape->inner_shape[i] = static_cast<ALTIUM_PAD_SHAPE>( aReader.read<uint8_t>() );
        }

        aReader.skip( 1 );

        sizeAndShape->isslot       = aReader.read<uint8_t>() == 0x02;
        sizeAndShape->slotsize     = aReader.read_unit();
        sizeAndShape->slotrotation = aReader.read<double>();

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->holeoffset[i].x = aReader.read_unit_x();
        }
        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->holeoffset[i].y = aReader.read_unit_y();
        }

        aReader.skip( 1 );

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->alt_shape[i] =
                    static_cast<ALTIUM_PAD_SHAPE_ALT>( aReader.read<uint8_t>() );
        }

        for( int i = 0; i < 32; i++ )
        {
            sizeAndShape->cornerradius[i] = aReader.read<uint8_t>();
        }
    }

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

AVIA6::AVIA6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::VIA );

    // Subrecord 1
    aReader.read_subrecord_length();

    aReader.skip( 1 );

    uint8_t flags1  = aReader.read<uint8_t>();
    is_test_fab_top = ( flags1 & 0x80 ) != 0;
    is_tent_bottom  = ( flags1 & 0x40 ) != 0;
    is_tent_top     = ( flags1 & 0x20 ) != 0;
    is_locked       = ( flags1 & 0x04 ) == 0;

    uint8_t flags2     = aReader.read<uint8_t>();
    is_test_fab_bottom = ( flags2 & 0x01 ) != 0;

    net = aReader.read<uint16_t>();
    aReader.skip( 8 );
    position = aReader.read_point();
    diameter = aReader.read_unit();
    holesize = aReader.read_unit();

    layer_start = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );
    layer_end   = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );
    aReader.skip( 43 );
    viamode = static_cast<ALTIUM_PAD_MODE>( aReader.read<uint8_t>() );

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

ATRACK6::ATRACK6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::TRACK );

    // Subrecord 1
    aReader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );

    uint8_t flags1 = aReader.read<uint8_t>();
    is_locked      = ( flags1 & 0x04 ) == 0;
    // is_polygon_outline = ( flags1 & 0x02 ) != 0;  // TODO: only matches on outline?

    uint8_t flags2 = aReader.read<uint8_t>();
    is_keepout     = flags2 == 2;

    net = aReader.read<uint16_t>();
    aReader.skip( 2 );
    component = aReader.read<uint16_t>();
    aReader.skip( 4 );
    start = aReader.read_point();
    end   = aReader.read_point();
    width = aReader.read_unit();

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

ATEXT6::ATEXT6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::TEXT );

    // Subrecord 1 - Properties
    size_t subrecord1 = aReader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );
    aReader.skip( 6 );
    component = aReader.read<uint16_t>();
    aReader.skip( 4 );
    position = aReader.read_point();
    height   = aReader.read_unit();
    aReader.skip( 2 );
    rotation     = aReader.read<double>();
    mirrored     = aReader.read<uint8_t>() != 0;
    strokewidth  = aReader.read_unit();
    isComment    = aReader.read<uint8_t>() != 0;
    isDesignator = aReader.read<uint8_t>() != 0;
    aReader.skip( 90 );
    textposition = static_cast<ALTIUM_TEXT_POSITION>( aReader.read<uint8_t>() );
    /**
     * In Altium 14 (subrecord1 == 230) only left bottom is valid? I think there is a bit missing.
     * https://gitlab.com/kicad/code/kicad/merge_requests/60#note_274913397
     */
    if( subrecord1 <= 230 )
    {
        textposition = ALTIUM_TEXT_POSITION::LEFT_BOTTOM;
    }
    aReader.skip( 27 );
    isTruetype = aReader.read<uint8_t>() != 0;

    aReader.subrecord_skip();

    // Subrecord 2 - String
    aReader.read_subrecord_length();

    text = aReader.read_string(); // TODO: what about strings with length > 255?

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

AFILL6::AFILL6( ALTIUM_PARSER& aReader )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::FILL );

    // Subrecord 1
    aReader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );

    uint8_t flags1 = aReader.read<uint8_t>();
    is_locked      = ( flags1 & 0x04 ) == 0;

    uint8_t flags2 = aReader.read<uint8_t>();
    is_keepout     = flags2 == 2;

    net = aReader.read<uint16_t>();
    aReader.skip( 2 );
    component = aReader.read<uint16_t>();
    aReader.skip( 4 );
    pos1     = aReader.read_point();
    pos2     = aReader.read_point();
    rotation = aReader.read<double>();

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}

AREGION6::AREGION6( ALTIUM_PARSER& aReader, bool aExtendedVertices )
{
    wxASSERT( aReader.bytes_remaining() > 4 );
    wxASSERT( !aReader.parser_error() );

    ALTIUM_RECORD recordtype = static_cast<ALTIUM_RECORD>( aReader.read<uint8_t>() );
    wxASSERT( recordtype == ALTIUM_RECORD::REGION );

    // Subrecord 1
    aReader.read_subrecord_length();

    layer = static_cast<ALTIUM_LAYER>( aReader.read<uint8_t>() );

    uint8_t flags1 = aReader.read<uint8_t>();
    is_locked      = ( flags1 & 0x04 ) == 0;

    uint8_t flags2 = aReader.read<uint8_t>();
    is_keepout     = flags2 == 2;

    net = aReader.read<uint16_t>();
    aReader.skip( 2 );
    component = aReader.read<uint16_t>();
    aReader.skip( 9 );

    std::map<wxString, wxString> properties = aReader.read_properties();
    wxASSERT( !properties.empty() );

    int  pkind     = ALTIUM_PARSER::property_int( properties, "KIND", 0 );
    bool is_cutout = ALTIUM_PARSER::property_bool( properties, "ISBOARDCUTOUT", false );

    is_shapebased = ALTIUM_PARSER::property_bool( properties, "ISSHAPEBASED", false );

    switch( pkind )
    {
    case 0:
        if( is_cutout )
        {
            kind = ALTIUM_REGION_KIND::BOARD_CUTOUT;
        }
        else
        {
            kind = ALTIUM_REGION_KIND::COPPER;
        }
        break;
    case 1:
        kind = ALTIUM_REGION_KIND::POLYGON_CUTOUT;
        break;
    case 4:
        kind = ALTIUM_REGION_KIND::CAVITY_DEFINITION;
        break;
    default:
        kind = ALTIUM_REGION_KIND::UNKNOWN;
        break;
    }

    uint32_t num_vertices = aReader.read<uint32_t>();

    for( uint32_t i = 0; i < num_vertices; i++ )
    {
        if( aExtendedVertices )
        {
            bool    isRound  = aReader.read<uint8_t>() != 0;
            wxPoint position = aReader.read_point();
            wxPoint center   = aReader.read_point();
            int32_t radius   = aReader.read_unit();
            double  angle1   = aReader.read<double>();
            double  angle2   = aReader.read<double>();
            vertices.emplace_back( isRound, radius, angle1, angle2, position, center );
        }
        else
        {
            // no idea why, but for some regions the coordinates are stored as double and not as int32_t
            double x = aReader.read<double>();
            double y = -aReader.read<double>();
            vertices.emplace_back( wxPoint( KiROUND( x ), KiROUND( y ) ) );
        }
    }

    aReader.subrecord_skip();

    wxASSERT( !aReader.parser_error() );
}
