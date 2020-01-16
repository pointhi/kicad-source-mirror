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

#include <vector>
#include <zconf.h>
#include <layers_id_colors_and_visibility.h>


struct ALTIUM_RECORD
{
    enum
    {
        ARC                 = 1,
        PAD                 = 2,
        VIA                 = 3,
        TRACK               = 4,
        TEXT                = 5,
        FILL                = 6,
        REGION              = 11,
        MODEL               = 12
    };
};


struct ALTIUM_PAD_SHAPE
{
    enum
    {
        UNKNOWN             = 0,
        CIRCLE              = 1,
        RECT                = 2,
        OVAL                = 3
    };
};


struct ALTIUM_PAD_MODE
{
    enum
    {
        SIMPLE              = 0,
        TOP_MIDDLE_BOTTOM   = 1,
        FULL_STACK          = 2
    };
};


struct ALTIUM_LAYER
{
    enum
    {
        TOP_LAYER           = 1,
        MID_LAYER_1         = 2,
        MID_LAYER_2         = 3,
        MID_LAYER_3         = 4,
        MID_LAYER_4         = 5,
        MID_LAYER_5         = 6,
        MID_LAYER_6         = 7,
        MID_LAYER_7         = 8,
        MID_LAYER_8         = 9,
        MID_LAYER_9         = 10,
        MID_LAYER_10        = 11,
        MID_LAYER_11        = 12,
        MID_LAYER_12        = 13,
        MID_LAYER_13        = 14,
        MID_LAYER_14        = 15,
        MID_LAYER_15        = 16,
        MID_LAYER_16        = 17,
        MID_LAYER_17        = 18,
        MID_LAYER_18        = 19,
        MID_LAYER_19        = 20,
        MID_LAYER_20        = 21,
        MID_LAYER_21        = 22,
        MID_LAYER_22        = 23,
        MID_LAYER_23        = 24,
        MID_LAYER_24        = 25,
        MID_LAYER_25        = 26,
        MID_LAYER_26        = 27,
        MID_LAYER_27        = 28,
        MID_LAYER_28        = 29,
        MID_LAYER_29        = 30,
        MID_LAYER_30        = 31,
        BOTTOM_LAYER        = 32,

        TOP_OVERLAY         = 33,
        BOTTOM_OVERLAY      = 34,
        TOP_PASTE           = 35,
        BOTTOM_PASTE        = 36,
        TOP_SOLDER          = 37,
        BOTTOM_SOLDER       = 38,

        INTERNAL_PLANE_1    = 39,
        INTERNAL_PLANE_2    = 40,
        INTERNAL_PLANE_3    = 41,
        INTERNAL_PLANE_4    = 42,
        INTERNAL_PLANE_5    = 43,
        INTERNAL_PLANE_6    = 44,
        INTERNAL_PLANE_7    = 45,
        INTERNAL_PLANE_8    = 46,
        INTERNAL_PLANE_9    = 47,
        INTERNAL_PLANE_10   = 48,
        INTERNAL_PLANE_11   = 49,
        INTERNAL_PLANE_12   = 50,
        INTERNAL_PLANE_13   = 51,
        INTERNAL_PLANE_14   = 52,
        INTERNAL_PLANE_15   = 53,
        INTERNAL_PLANE_16   = 54,

        DRILL_GUIDE         = 55,
        KEEP_OUT_LAYER      = 56,

        MECHANICAL_1        = 57,
        MECHANICAL_2        = 58,
        MECHANICAL_3        = 59,
        MECHANICAL_4        = 60,
        MECHANICAL_5        = 61,
        MECHANICAL_6        = 62,
        MECHANICAL_7        = 63,
        MECHANICAL_8        = 64,
        MECHANICAL_9        = 65,
        MECHANICAL_10       = 66,
        MECHANICAL_11       = 67,
        MECHANICAL_12       = 68,
        MECHANICAL_13       = 69,
        MECHANICAL_14       = 70,
        MECHANICAL_15       = 71,
        MECHANICAL_16       = 72,

        DRILL_DRAWING       = 73,
        MULTI_LAYER         = 74,
        CONNECTIONS         = 75,
        BACKGROUND          = 76,
        DRC_ERROR_MARKERS   = 77,
        SELECTIONS          = 78,
        VISIBLE_GRID_1      = 79,
        VISIBLE_GRID_2      = 80,
        PAD_HOLES           = 81,
        VIA_HOLES           = 82,
    };
};


class BOARD;
class MODULE;

namespace CFB {
    class CompoundFileReader;
    struct COMPOUND_FILE_ENTRY;
}


class ALTIUM_PCB
{
public:
    explicit ALTIUM_PCB( BOARD* aBoard );
    ~ALTIUM_PCB();

    void Parse( const CFB::CompoundFileReader& aReader );

private:
    PCB_LAYER_ID kicad_layer( int aAltiumLayer ) const;

    MODULE* GetComponent( const u_int16_t id);
    int GetNetCode( const u_int16_t id);

    void ParseFileHeader( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Text Format
    void ParseBoard6Data(const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseComponents6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseNets6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Binary Format
    void ParseArcs6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParsePads6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseVias6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseTracks6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseTexts6Data( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    BOARD*                  m_board;
    std::vector<MODULE*>    m_components;
};


#endif //ALTIUM_PCB_H
