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

#ifndef ALTIUM_PARSER_H
#define ALTIUM_PARSER_H

#include <map>
#include <memory>

#include <wx/gdicmn.h>


namespace CFB
{
class CompoundFileReader;
struct COMPOUND_FILE_ENTRY;
} // namespace CFB

// Helper method to find file inside compound file
const CFB::COMPOUND_FILE_ENTRY* FindStream(
        const CFB::CompoundFileReader& aReader, const char* aStreamName );


class ALTIUM_PARSER
{
public:
    ALTIUM_PARSER( const CFB::CompoundFileReader& aReader, const CFB::COMPOUND_FILE_ENTRY* aEntry );
    ~ALTIUM_PARSER() = default;

    template <typename Type>
    Type read()
    {
        if( bytes_remaining() >= sizeof( Type ) )
        {
            Type val = *(Type*) ( pos );
            pos += sizeof( Type );
            return val;
        }
        else
        {
            error = true;
            return 0;
        }
    }

    wxString read_string()
    {
        uint8_t len = read<uint8_t>();
        if( bytes_remaining() >= len )
        {

            //altium uses LATIN1/ISO 8859-1, convert it
            wxString val = wxString( pos, wxConvISO8859_1, len );
            pos += len;
            return val;
        }
        else
        {
            error = true;
            return wxString( "" );
        }
    }

    int32_t read_unit()
    {
        return kicad_unit( read<int32_t>() );
    }

    int32_t read_unit_x()
    {
        return read_unit();
    }

    int32_t read_unit_y()
    {
        return -read_unit();
    }

    wxPoint read_point()
    {
        int32_t x = read_unit_x();
        int32_t y = read_unit_y();
        return { x, y };
    }

    wxSize read_size()
    {
        int32_t x = read_unit();
        int32_t y = read_unit();
        return { x, y };
    }

    size_t read_subrecord_length()
    {
        uint32_t length  = read<uint32_t>();
        subrecord_end    = pos + length;
        return length;
    }

    std::map<wxString, wxString> read_properties();

    static int32_t kicad_unit( const int32_t aValue )
    {
        return ( ( (int64_t) aValue ) * 254L ) / 100;
    }

    static int property_int(
            const std::map<wxString, wxString>& aProperties, const wxString& aKey, int aDefault );

    static double property_double( const std::map<wxString, wxString>& aProperties,
            const wxString& aKey, double aDefault );

    static bool property_bool(
            const std::map<wxString, wxString>& aProperties, const wxString& aKey, bool aDefault );

    static int32_t property_unit( const std::map<wxString, wxString>& aProperties,
            const wxString& aKey, const wxString& aDefault );

    static wxString property_string( const std::map<wxString, wxString>& aProperties,
            const wxString& aKey, wxString aDefault );

    void skip( size_t aLength )
    {
        if( bytes_remaining() >= aLength )
        {
            pos += aLength;
        }
        else
        {
            error = true;
        }
    }

    void subrecord_skip()
    {
        if( subrecord_end == nullptr || subrecord_end < pos )
        {
            error = true;
        }
        else
        {
            pos = subrecord_end;
        }
    };

    size_t bytes_remaining() const
    {
        return pos == nullptr ? 0 : size - ( pos - content.get() );
    }

    size_t subrecord_remaining() const
    {
        return pos == nullptr || subrecord_end == nullptr || subrecord_end <= pos ?
                       0 :
                       subrecord_end - pos;
    };

    bool parser_error()
    {
        return error;
    }

private:
    std::unique_ptr<char[]> content;
    size_t                  size;

    char* pos;           // current read pointer
    char* subrecord_end; // pointer which points to next subrecord start
    bool  error;
};


#endif //ALTIUM_PARSER_H
