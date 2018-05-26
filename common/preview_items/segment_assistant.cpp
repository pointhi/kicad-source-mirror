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

#include <preview_items/segment_assistant.h>

#include <preview_items/preview_utils.h>
#include <gal/graphics_abstraction_layer.h>
#include <view/view.h>
#include <pcb_painter.h>

#include <class_board_item.h>
#include <common.h>
#include <base_units.h>

using namespace KIGFX::PREVIEW;

SEGMENT_ASSISTANT::SEGMENT_ASSISTANT( const SEGMENT_GEOM_MANAGER& aManager, EDA_UNITS_T aUnits ) :
    EDA_ITEM( NOT_USED ),
    m_constructMan( aManager ),
    m_units( aUnits )
{
}


const BOX2I SEGMENT_ASSISTANT::ViewBBox() const
{
    BOX2I tmp;

    // no bounding box when no graphic shown
    if( m_constructMan.IsReset() )
        return tmp;

    // just enclose the whle circular area
    auto origin = m_constructMan.GetOrigin();
    auto end = m_constructMan.GetEnd();

    switch( m_constructMan.GetShape() ) {
        case S_SEGMENT:
            tmp.SetOrigin( origin );
            tmp.SetEnd( end );
            break;

        case S_CIRCLE:
        default:
            tmp.SetOrigin( origin + end );
            tmp.SetEnd( origin - end );
            break;
    }
    tmp.Normalize();
    return tmp;
}


static void drawLine( KIGFX::VIEW *aView, const VECTOR2I& aStart, const VECTOR2I& aEnd )
{
    auto gal = aView->GetGAL();
    auto rs = static_cast<KIGFX::PCB_RENDER_SETTINGS*>( aView->GetPainter()->GetSettings() );

    gal->SetStrokeColor( rs->GetLayerColor( LAYER_AUX_ITEMS ) );

    gal->DrawLine( aStart, aEnd );
}


void SEGMENT_ASSISTANT::ViewDraw( int aLayer, KIGFX::VIEW* aView ) const
{
    auto& gal = *aView->GetGAL();

    // not in a position to draw anything
    if( m_constructMan.IsReset() )
        return;

    gal.SetLineWidth( 1.0 );
    gal.SetIsStroke( true );
    gal.SetIsFill( true );

    gal.ResetTextAttributes();

    // constant text size on screen
    SetConstantGlyphHeight( gal, 12.0 );

    const auto origin = m_constructMan.GetOrigin();
    const auto end = m_constructMan.GetEnd();

    std::vector<wxString> cursorStrings;

    switch( m_constructMan.GetShape() ) {
        case S_SEGMENT:
            cursorStrings.push_back( DimensionLabel( "d", m_constructMan.GetDistance(), m_units ) );
            break;

        case S_CIRCLE:
            drawLine( aView, origin, end );

            cursorStrings.push_back( DimensionLabel( "r", m_constructMan.GetDistance(), m_units ) );
            break;

        default:
            return; // not supported shape

    }

    // FIXME: spaces choke OpenGL lp:1668455
    for( auto& str : cursorStrings )
    {
        str.erase( std::remove( str.begin(), str.end(), ' ' ),
                str.end() );
    }

    // place the text next to cursor, on opposite side from drawing
    DrawTextNextToCursor( aView, end, origin - end, cursorStrings );
}
