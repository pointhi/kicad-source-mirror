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

#include <iostream>

#include "altium_parser.h"

#include <compoundfilereader.h>
#include <sstream>
#include <utf.h>


ALTIUM_PARSER::ALTIUM_PARSER(
        const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry )
{
    subrecord_end = nullptr;
    if( aEntry->size > std::numeric_limits<size_t>::max() )
    {
        std::cerr << "error: stream too large" << std::endl;
        pos = nullptr;

        size  = 0;
        error = true;
        return;
    }

    size  = static_cast<size_t>( aEntry->size );
    error = false;
    content.reset( new char[size] );
    pos = content.get();

    // read file into buffer
    aReader.ReadFile( aEntry, 0, content.get(), size );
}

ALTIUM_PARSER::~ALTIUM_PARSER()
{
}

std::map<std::string, std::string> ALTIUM_PARSER::read_properties()
{
    std::map<std::string, std::string> kv;

    uint32_t length = read<uint32_t>();
    if( length > bytes_remaining() || pos[length - 1] != '\0' )
    {
        error = true;
        return kv;
    }

    std::string str = std::string( pos, length - 1 );
    pos += length;

    std::size_t token_end = 0;
    while( token_end < str.size() && token_end != std::string::npos )
    {
        std::size_t token_start = str.find( '|', token_end );
        std::size_t token_equal = str.find( '=', token_start );
        token_end               = str.find( '|', token_equal );

        std::string key   = str.substr( token_start + 1, token_equal - token_start - 1 );
        std::string value = str.substr( token_equal + 1, token_end - token_equal - 1 );
        kv.insert( { key, value } );
    }

    return kv;
}

int ALTIUM_PARSER::property_int(
        const std::map<std::string, std::string>& properties, const std::string& key, int def )
{
    try
    {
        const std::string& value = properties.at( key );

        return std::stoi( value );
    }
    catch( const std::out_of_range& oor )
    {
        return def;
    }
}

double ALTIUM_PARSER::property_double(
        const std::map<std::string, std::string>& properties, const std::string& key, double def )
{
    try
    {
        const std::string& value = properties.at( key );

        // Locale independent str -> double conversation
        std::istringstream istr( value );
        istr.imbue( std::locale( "C" ) );

        double doubleValue;
        istr >> doubleValue;

        return doubleValue;
    }
    catch( const std::out_of_range& oor )
    {
        return def;
    }
}

bool ALTIUM_PARSER::property_bool(
        const std::map<std::string, std::string>& properties, const std::string& key, bool def )
{
    try
    {
        const std::string& value = properties.at( key );

        return value == "TRUE";
    }
    catch( const std::out_of_range& oor )
    {
        return def;
    }
}

int32_t ALTIUM_PARSER::property_unit( const std::map<std::string, std::string>& properties,
        const std::string& key, const std::string& def )
{
    const std::string& value = property_string( properties, key, def );

    bool        positive      = value.at( 0 ) != '-';
    std::size_t decimal_point = value.find( '.' );
    std::size_t value_end     = value.find_first_not_of( "0123456789." );

    std::string before_decimal_str   = value.substr( positive ? 0 : 1, decimal_point );
    int         before_decimal       = std::stoi( before_decimal_str );
    int         after_decimal        = 0;
    int         after_decimal_digits = 0;
    if( decimal_point != std::string::npos )
    {
        if( value_end != std::string::npos )
        {
            after_decimal_digits = value_end - ( decimal_point + 1 );
        }
        else
        {
            after_decimal_digits = value.size() - ( decimal_point + 1 ); // TODO: correct?
        }
        std::string after_decimal_str = value.substr( decimal_point + 1, after_decimal_digits );
        after_decimal                 = std::stoi( after_decimal_str );
    }

    if( value.length() > 3 && value.compare( value.length() - 3, 3, "mil" ) == 0 )
    {
        // ensure after_decimal is formatted to base 1000
        int after_decimal_1000;
        if( after_decimal_digits <= 4 )
        {
            after_decimal_1000 = after_decimal * std::pow( 10, 4 - after_decimal_digits );
        }
        else
        {
            after_decimal_1000 = after_decimal / std::pow( 10, after_decimal_digits - 4 );
        }

        long mils = ( positive ? 1 : -1 ) * ( before_decimal * 10000 + after_decimal_1000 );
        return kicad_unit( mils );
    }

    wxFAIL_MSG( value + " does not end with mils" );
    return 0;
}

std::string ALTIUM_PARSER::property_string( const std::map<std::string, std::string>& properties,
        const std::string& key, std::string def )
{
    try
    {
        return properties.at( key );
    }
    catch( const std::out_of_range& oor )
    {
        return def;
    }
}