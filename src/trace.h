/*
 * Copyright (C) 2005 Christopher Carroll <christopher@salbot.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
#ifndef BS_TRACE_H
#define BS_TRACE_H

#ifdef TRACE
#define trace(fmt, args...)                                          \
{                                                                    \
    fprintf(stderr, "[%s:%d] %s(): ", __FILE__, __LINE__, __func__); \
    fprintf(stderr, fmt, ##args);                                    \
    fprintf(stderr, "\n");                                           \
}
#else
#define trace(fmt, args...) { for(;0;); }
#endif

#endif /* BS_TRACE_H */
