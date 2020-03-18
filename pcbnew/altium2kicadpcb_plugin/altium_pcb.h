/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2019-2020 Thomas Pointhuber <thomas.pointhuber@gmx.at>
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

#include <altium_parser_pcb.h>

class BOARD;
class MODULE;
class ZONE_CONTAINER;


namespace CFB
{
class CompoundFileReader;
struct COMPOUND_FILE_ENTRY;
} // namespace CFB


// type declaration required for a helper method
class ALTIUM_PCB;
typedef std::function<void( const CFB::CompoundFileReader&, const CFB::COMPOUND_FILE_ENTRY* )>
        PARSE_FUNCTION_POINTER_fp;


class ALTIUM_PCB
{
public:
    explicit ALTIUM_PCB( BOARD* aBoard );
    ~ALTIUM_PCB();

    void ParseDesigner( const CFB::CompoundFileReader& aReader );
    void ParseCircuitStudio( const CFB::CompoundFileReader& aReader );
    void ParseCircuitMaker( const CFB::CompoundFileReader& aReader );

private:
    PCB_LAYER_ID kicad_layer( ALTIUM_LAYER aAltiumLayer ) const;

    int     GetNetCode( uint16_t aId );
    ARULE6* GetRule( ALTIUM_RULE_KIND aKind, const wxString& aName );
    ARULE6* GetRuleDefault( ALTIUM_RULE_KIND aKind );

    void ParseHelper( const CFB::CompoundFileReader& aReader, const wxString& aStreamName,
            PARSE_FUNCTION_POINTER_fp aFP );
    void FinishParsingHelper();

    void ParseFileHeader(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Text Format
    void ParseBoard6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseClasses6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseComponents6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseDimensions6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseNets6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParsePolygons6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseRules6Data(
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
    void ParseBoardRegionsData(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseShapeBasedRegions6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    void ParseRegions6Data(
            const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );

    // Helper Functions
    void HelperParseDimensions6Linear( const ADIMENSION6& aElem );
    void HelperParseDimensions6Leader( const ADIMENSION6& aElem );
    void HelperParseDimensions6Datum( const ADIMENSION6& aElem );
    void HelperParseDimensions6Center( const ADIMENSION6& aElem );

    void HelperCreateBoardOutline( const std::vector<ALTIUM_VERTICE>& aVertices );

    BOARD*                               m_board;
    std::vector<MODULE*>                 m_components;
    std::map<ALTIUM_LAYER, PCB_LAYER_ID> m_layermap; // used to correctly map copper layers
    std::map<ALTIUM_RULE_KIND, std::vector<ARULE6>> m_rules;

    std::map<ALTIUM_LAYER, ZONE_CONTAINER*> m_outer_plane;
};


#endif //ALTIUM_PCB_H
