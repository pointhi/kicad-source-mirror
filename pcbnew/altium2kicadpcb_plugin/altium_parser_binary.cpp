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

#include "altium_parser_binary.h"

#include <compoundfilereader.h>
#include <utf.h>


ALTIUM_PARSER_BINARY::ALTIUM_PARSER_BINARY(const CFB::CompoundFileReader &aReader,
                                           const CFB::COMPOUND_FILE_ENTRY *aEntry) {
    subrecord_end = nullptr;
    if (aEntry->size > std::numeric_limits<size_t>::max())
    {
        std::cerr << "error: stream too large" << std::endl;
        pos = nullptr;

        size = 0;
        error = true;
        return;
    }

    size = static_cast<size_t>(aEntry->size);
    error = false;
    content.reset(new char[size]);
    pos = content.get();

    // read file into buffer
    aReader.ReadFile(aEntry, 0, content.get(), size);
}

ALTIUM_PARSER_BINARY::~ALTIUM_PARSER_BINARY() {

}

std::map<std::string, std::string> ALTIUM_PARSER_BINARY::read_properties() {
    std::map<std::string, std::string> kv;

    u_int32_t length = read<u_int32_t>();
    if ( length > bytes_remaining() || pos[length-1] != '\0' ) {
        error = true;
        return kv;
    }

    std::string str = std::string(pos, length-1);
    pos += length;

    std::size_t token_end = 0;
    while(token_end < str.size() && token_end != std::string::npos) {
        std::size_t token_start = str.find('|', token_end);
        std::size_t token_equal = str.find('=', token_start);
        token_end = str.find('|', token_equal);

        std::string key = str.substr(token_start+1, token_equal-token_start-1);
        std::string value = str.substr(token_equal+1, token_end-token_equal-1);
        kv.insert({key, value});
    }

    return kv;
}
