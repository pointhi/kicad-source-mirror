/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2018 Kicad Developers, see AUTHORS.txt for contributors.
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

#include <preview_items/segment_geom_manager.h>

#include <class_board_item.h>

using namespace KIGFX::PREVIEW;


SEGMENT_GEOM_MANAGER::SEGMENT_GEOM_MANAGER( int aShape ) :
        m_shape( aShape )
{
}

bool SEGMENT_GEOM_MANAGER::acceptPoint( const VECTOR2I& aPt )
{
    switch( getStep() )
    {
    case SET_ORIGIN:
        return setOrigin( aPt );
    case SET_END:
        return setEnd( aPt );
    case COMPLETE:
        break;
    }

    return false;
}


double SEGMENT_GEOM_MANAGER::GetDistance() const
{
    const auto radVec = m_end - m_origin;

    if( m_shape == S_SEGMENT && m_angleSnap ) {
        const double minDistance = std::min( std::abs( radVec.x ), std::abs( radVec.y ) );
        const double maxDistance = std::max( std::abs( radVec.x ), std::abs( radVec.y ) );

        const double lin45Distance = VECTOR2D( minDistance, minDistance ).EuclideanNorm();

        return std::max( lin45Distance, maxDistance - minDistance );
    } else {
        return radVec.EuclideanNorm();
    }
}


bool SEGMENT_GEOM_MANAGER::setOrigin( const VECTOR2I& aOrigin )
{
    m_origin = aOrigin;
    m_end = m_origin;

    return true;
}


bool SEGMENT_GEOM_MANAGER::setEnd( const VECTOR2I& aEnd )
{
    m_end = aEnd;

    return m_origin != m_end;
}
