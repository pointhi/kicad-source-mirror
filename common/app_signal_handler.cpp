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

#include <app_signal_handler.h>

#include <csignal>

#include <execinfo.h>
#include <stdio.h>
#include <time.h>

#define BACKTRACE_ARRAY_SIZE 64
#define TMP_BUFFER_SIZE 64


void APP_SIGNAL_HANDLER::RegisterSignalHandler( int sig, SigHandler handler )
{
    signal( sig, handler );
}

void APP_SIGNAL_HANDLER::BacktraceSignalHandler( int sig )
{
    APP_SIGNAL_HANDLER::WriteBacktraceReport( STDERR_FILENO, sig );

    time_t timer;
    struct tm* tm_info;

    time( &timer );
    tm_info = localtime( &timer );

    char tmp_buffer[TMP_BUFFER_SIZE];

    // Write crashdump into the current working directory
    strftime( tmp_buffer, TMP_BUFFER_SIZE, "kicad%Y%m%d_%H%M%S.crash", tm_info );
    dprintf( STDERR_FILENO, "\nwrite crashdump to: \"%s\"\n", tmp_buffer );
    int fd = open( tmp_buffer, O_CREAT|O_WRONLY|O_APPEND, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH );
    if( fd >= 0 )
    {
        dprintf( fd, "=== KiCad Crashdump ===\n" );

        strftime( tmp_buffer, TMP_BUFFER_SIZE, "%Y-%m-%d %H:%M:%S", tm_info );
        dprintf( fd, "Time: %s\n", tmp_buffer );

        APP_SIGNAL_HANDLER::WriteBacktraceReport( fd, sig );

        close( fd );
    }
    else
    {
        dprintf( STDERR_FILENO, "[unable to write report to disk]\n" );
    }


    quick_exit( EXIT_FAILURE );
}


void APP_SIGNAL_HANDLER::WriteBacktraceReport( int fd, int sig, int skip )
{
    dprintf( fd, "Error: signal %d (%s)\n", sig, strsignal( sig ) );

    dprintf( fd, "Backtrace:\n" );

    void *array[BACKTRACE_ARRAY_SIZE];
    size_t size;

    size = backtrace( array, BACKTRACE_ARRAY_SIZE );

    backtrace_symbols_fd( array+skip, size-skip, fd );

    if( size == BACKTRACE_ARRAY_SIZE )
    {
        dprintf( fd, "[truncated]\n" );
    }
}
