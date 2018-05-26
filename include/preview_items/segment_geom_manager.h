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

#ifndef PREVIEW_ITEMS_SEGMENT_GEOMETRY_MANAGER_H
#define PREVIEW_ITEMS_SEGMENT_GEOMETRY_MANAGER_H

#include <preview_items/multistep_geom_manager.h>

#include <common.h>

namespace KIGFX {
namespace PREVIEW {


/**
 * Class ARC_GEOM_MANAGER
 *
 * A class to manage the construction of a circular arc though
 * sequential setting of critical points: centre, arc start
 * and arc end. The manager is driven by setting cursor points, which
 * update the geometry, and optionally advance the manager state.
 *
 * Interfaces are provided to return both arc geometry (can be used
 * to set up real arcs on PCBs, for example) as well as important
 * control points for informational overlays.
 */
class SEGMENT_GEOM_MANAGER: public MULTISTEP_GEOM_MANAGER
{
public:
    SEGMENT_GEOM_MANAGER(int aShape);

    enum SEGMENT_STEPS
    {
        SET_ORIGIN = 0,     ///> Waiting to lock in origin point
        SET_END,          ///> Waiting to lock in the end point
        COMPLETE
    };

    int getMaxStep() const override
    {
        return COMPLETE;
    }

    /**
     * Get the current step the mananger is on (useful when drawing
     * something depends on the current state)
     */
    SEGMENT_STEPS GetStep() const
    {
        return static_cast<SEGMENT_STEPS>( getStep() );
    }

    bool acceptPoint( const VECTOR2I& aPt ) override;

    ///> Set angle snapping (for the next point)
    void SetAngleSnap( bool aSnap )
    {
        m_angleSnap = aSnap;
    }

    /*
     * Geometry query interface - used by clients of the manager
     */

    ///> Get the shape of the segment, @see STROKE_T
    int GetShape() const
    {
        return m_shape;
    }

    ///> Get the start point of the drawsegment (valid when state > SET_ORIGIN)
    VECTOR2I GetOrigin() const
    {
        return m_origin;
    }

    ///> Get the coordinates of the end
    VECTOR2I GetEnd() const
    {
        return m_end;
    }

    ///> Get the radius of the segment
    double GetDistance() const;

private:

    /*
     * Type of Segment
     */
    const int m_shape;

    /*
     * Point acceptor functions
     */

    ///> Set the centre point of the drawsegment
    bool setOrigin( const VECTOR2I& aOrigin );

    ///> Set a end point
    bool setEnd( const VECTOR2I& aCursor );

    /*
     * Segment geometry
     */
    VECTOR2I m_origin;
    VECTOR2I m_end;

    /*
     * construction parameters
     */
    bool m_angleSnap = false;
};
}       // PREVIEW
}       // KIGFX

#endif  // PREVIEW_ITEMS_SEGMENT_GEOMETRY_MANAGER_H
