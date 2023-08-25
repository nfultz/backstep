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
#ifndef ERROR_H
#define ERROR_H

#define BACKSTEP_ERROR bs_error_quark()

typedef enum {
    BS_ERROR_WIN_PROP_NOEXIST,
    BS_ERROR_WIN_PROP_WRONG_TYPE,
    BS_ERROR_WIN_PROP_WRONG_FMT,
    BS_ERROR_X_ERROR
} BSError;

#endif /* ERROR_H */
