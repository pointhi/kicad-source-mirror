/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright (C) 2017 KiCad Developers, see CHANGELOG.txt for contributors.
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

#ifndef APP_SIGNAL_HANDLER_H_
#define APP_SIGNAL_HANDLER_H_

#include <wx/app.h>

/**
 * Class APP_SIGNAL_HANDLER
 *
 * Contains functions for registering signal handlers to run against
 * a wxApp instance.
 *
 * There can only be one of these per program, as the signal handler
 * callbacks have to be static. You can still register for signals
 * elsewhere if you do not want to handle them here.
 */
class APP_SIGNAL_HANDLER
{
public:
    /*!
     * Function signature of a signal handler
     */
    typedef void ( * SigHandler )( int );

    /*!
     * Function registerSignalHandler
     *
     * Register a signal handler for a set of signals
     *
     * @param sig the signal number to registr against
     * @param handler the handler to register for that signal
     */
    static void RegisterSignalHandler( int sig, SigHandler handler );

    /*!
     * Signal handler that print and saves a backtrace and then exits
     */
    static void BacktraceSignalHandler( int sig );

private:

    static void WriteBacktraceReport( int fd, int sig, int skip=2 );
};

#endif
