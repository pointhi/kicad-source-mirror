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

#ifndef ALTIUM_PCB_H
#define ALTIUM_PCB_H

#include <functional>
#include <layers_id_colors_and_visibility.h>
#include <vector>
#include <zconf.h>


enum class ALTIUM_RECORD
{
    ARC    = 1,
    PAD    = 2,
    VIA    = 3,
    TRACK  = 4,
    TEXT   = 5,
    FILL   = 6,
    REGION = 11,
    MODEL  = 12
};

enum class ALTIUM_PAD_SHAPE
{
    UNKNOWN   = 0,
    CIRCLE    = 1,
    RECT      = 2,
    OCTAGONAL = 3
};

enum class ALTIUM_PAD_MODE
{
    SIMPLE            = 0,
    TOP_MIDDLE_BOTTOM = 1,
    FULL_STACK        = 2
};

enum class ALTIUM_TEXT_POSITION
{
    LEFT_TOP      = 1,
    LEFT_CENTER   = 2,
    LEFT_BOTTOM   = 3,
    CENTER_TOP    = 4,
    CENTER_CENTER = 5,
    CENTER_BOTTOM = 6,
    RIGHT_TOP     = 7,
    RIGHT_CENTER  = 8,
    RIGHT_BOTTOM  = 9
};


enum class ALTIUM_LAYER
{
    UNKNOWN = 0,

    TOP_LAYER    = 1,
    MID_LAYER_1  = 2,
    MID_LAYER_2  = 3,
    MID_LAYER_3  = 4,
    MID_LAYER_4  = 5,
    MID_LAYER_5  = 6,
    MID_LAYER_6  = 7,
    MID_LAYER_7  = 8,
    MID_LAYER_8  = 9,
    MID_LAYER_9  = 10,
    MID_LAYER_10 = 11,
    MID_LAYER_11 = 12,
    MID_LAYER_12 = 13,
    MID_LAYER_13 = 14,
    MID_LAYER_14 = 15,
    MID_LAYER_15 = 16,
    MID_LAYER_16 = 17,
    MID_LAYER_17 = 18,
    MID_LAYER_18 = 19,
    MID_LAYER_19 = 20,
    MID_LAYER_20 = 21,
    MID_LAYER_21 = 22,
    MID_LAYER_22 = 23,
    MID_LAYER_23 = 24,
    MID_LAYER_24 = 25,
    MID_LAYER_25 = 26,
    MID_LAYER_26 = 27,
    MID_LAYER_27 = 28,
    MID_LAYER_28 = 29,
    MID_LAYER_29 = 30,
    MID_LAYER_30 = 31,
    BOTTOM_LAYER = 32,

    TOP_OVERLAY    = 33,
    BOTTOM_OVERLAY = 34,
    TOP_PASTE      = 35,
    BOTTOM_PASTE   = 36,
    TOP_SOLDER     = 37,
    BOTTOM_SOLDER  = 38,

    INTERNAL_PLANE_1  = 39,
    INTERNAL_PLANE_2  = 40,
    INTERNAL_PLANE_3  = 41,
    INTERNAL_PLANE_4  = 42,
    INTERNAL_PLANE_5  = 43,
    INTERNAL_PLANE_6  = 44,
    INTERNAL_PLANE_7  = 45,
    INTERNAL_PLANE_8  = 46,
    INTERNAL_PLANE_9  = 47,
    INTERNAL_PLANE_10 = 48,
    INTERNAL_PLANE_11 = 49,
    INTERNAL_PLANE_12 = 50,
    INTERNAL_PLANE_13 = 51,
    INTERNAL_PLANE_14 = 52,
    INTERNAL_PLANE_15 = 53,
    INTERNAL_PLANE_16 = 54,

    DRILL_GUIDE    = 55,
    KEEP_OUT_LAYER = 56,

    MECHANICAL_1  = 57,
    MECHANICAL_2  = 58,
    MECHANICAL_3  = 59,
    MECHANICAL_4  = 60,
    MECHANICAL_5  = 61,
    MECHANICAL_6  = 62,
    MECHANICAL_7  = 63,
    MECHANICAL_8  = 64,
    MECHANICAL_9  = 65,
    MECHANICAL_10 = 66,
    MECHANICAL_11 = 67,
    MECHANICAL_12 = 68,
    MECHANICAL_13 = 69,
    MECHANICAL_14 = 70,
    MECHANICAL_15 = 71,
    MECHANICAL_16 = 72,

    DRILL_DRAWING     = 73,
    MULTI_LAYER       = 74,
    CONNECTIONS       = 75,
    BACKGROUND        = 76,
    DRC_ERROR_MARKERS = 77,
    SELECTIONS        = 78,
    VISIBLE_GRID_1    = 79,
    VISIBLE_GRID_2    = 80,
    PAD_HOLES         = 81,
    VIA_HOLES         = 82,
};

class ALTIUM_PARSER;

struct ABOARD6
{
    wxPoint sheetpos;
    wxSize  sheetsize;

    int layercount;

    explicit ABOARD6( ALTIUM_PARSER& reader );
};

struct ACOMPONENT6
{
    std::string layer;
    wxPoint     position;
    double      rotation;
    bool        locked;
    bool        nameon;
    bool        commenton;
    std::string sourcedesignator;
    std::string sourcelibreference;

    explicit ACOMPONENT6( ALTIUM_PARSER& reader );
};

struct ANET6
{
    std::string name;

    explicit ANET6( ALTIUM_PARSER& reader );
};

struct APOLYGON6_VERTICE
{
    const wxPoint position;

    explicit APOLYGON6_VERTICE( const wxPoint position ) : position( position )
    {
    }
};

struct APOLYGON6
{
    std::string layer;
    u_int16_t   net;
    bool        locked;

    std::vector<APOLYGON6_VERTICE> vertices;

    explicit APOLYGON6( ALTIUM_PARSER& reader );
};

struct AARC6
{
    ALTIUM_LAYER layer;
    u_int16_t net;
    u_int16_t component;

    wxPoint   center;
    u_int32_t radius;
    double    startangle;
    double    endangle;
    u_int32_t width;

    explicit AARC6( ALTIUM_PARSER& reader );
};

struct APAD6_SIZE_AND_SHAPE
{
    wxSize   inner_size[29];
    ALTIUM_PAD_SHAPE inner_shape[29];
    wxPoint  holeoffset[32];
    u_int8_t cornerradius[32];
};

struct APAD6
{
    std::string name;

    ALTIUM_LAYER layer;
    u_int16_t net;
    u_int16_t component;

    wxPoint position;
    wxSize  topsize;
    wxSize  midsize;
    wxSize  botsize;
    u_int32_t holesize;

    ALTIUM_PAD_SHAPE topshape;
    ALTIUM_PAD_SHAPE midshape;
    ALTIUM_PAD_SHAPE botshape;

    ALTIUM_PAD_MODE padmode;

    double   direction;
    bool     plated;
    u_int8_t pastemaskexpansionmode;
    u_int8_t soldermaskexpansion;
    double   holerotation;

    ALTIUM_LAYER tolayer;
    ALTIUM_LAYER fromlayer;

    std::unique_ptr<APAD6_SIZE_AND_SHAPE> sizeAndShape;

    explicit APAD6( ALTIUM_PARSER& reader );
};

struct AVIA6
{
    u_int16_t net;

    wxPoint   position;
    u_int32_t diameter;
    u_int32_t holesize;

    explicit AVIA6( ALTIUM_PARSER& reader );
};

struct ATRACK6
{
    ALTIUM_LAYER layer;
    u_int16_t net;
    u_int16_t component;

    wxPoint   start;
    wxPoint   end;
    u_int32_t width;

    explicit ATRACK6( ALTIUM_PARSER& reader );
};

struct ATEXT6
{
    ALTIUM_LAYER layer;
    u_int16_t component;

    wxPoint   position;
    u_int32_t height;
    double    rotation;
    u_int32_t strokewidth;
    ALTIUM_TEXT_POSITION textposition;
    bool      mirrored;

    bool isComment;
    bool isDesignator;

    std::string text;

    explicit ATEXT6( ALTIUM_PARSER& reader );
};

struct AFILL6
{
    ALTIUM_LAYER layer;
    u_int16_t net;

    wxPoint pos1;
    wxPoint pos2;
    double  rotation;

    explicit AFILL6( ALTIUM_PARSER& reader );
};

class BOARD;
class MODULE;


namespace CFB
{
class CompoundFileReader;
struct COMPOUND_FILE_ENTRY;
} // namespace CFB


// type declaration required for a helper method
class ALTIUM_PCB;
typedef std::function<void(
        const ALTIUM_PCB&, const CFB::CompoundFileReader&, const CFB::COMPOUND_FILE_ENTRY* )>
        parse_function_pointer_t;


class ALTIUM_PCB
{
public:
    explicit ALTIUM_PCB( BOARD* aBoard );
    ~ALTIUM_PCB();

    void ParseDesigner( const CFB::CompoundFileReader& aReader );
    void ParseCircuitStudio( const CFB::CompoundFileReader& aReader );
    void ParseCircuitMaker( const CFB::CompoundFileReader& aReader );

private:
    ALTIUM_LAYER altium_layer_from_name( const std::string& aName ) const;
    PCB_LAYER_ID kicad_layer( ALTIUM_LAYER aAltiumLayer ) const;

    MODULE* GetComponent( const u_int16_t id );
    int     GetNetCode( const u_int16_t id );

    void ParseHelper( const CFB::CompoundFileReader& aReader, const std::string& streamName,
            parse_function_pointer_t fp );
    void FinishParsingHelper();

    void ParseFileHeader(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Text Format
    void ParseBoard6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseComponents6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseNets6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParsePolygons6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Binary Format
    void ParseArcs6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParsePads6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseVias6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseTracks6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseTexts6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseFills6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );


    BOARD*               m_board;
    std::vector<MODULE*> m_components;
};


#endif //ALTIUM_PCB_H
