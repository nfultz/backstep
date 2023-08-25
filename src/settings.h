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
#include "configuration.h"

extern BSBooleanConfigItem *BS_FRAME_SHOW;
extern BSStringConfigItem *BS_FRAME_BORDER;
extern BSBooleanConfigItem *BS_ICONS_XRENDER;
extern BSBooleanConfigItem *BS_ICONS_STICKY;
extern BSBooleanConfigItem *BS_FRAME_COLOR_CUSTOM;
extern BSIntegerConfigItem *BS_FRAME_COLOR_RED;
extern BSIntegerConfigItem *BS_FRAME_COLOR_GREEN;
extern BSIntegerConfigItem *BS_FRAME_COLOR_BLUE;
extern BSBooleanConfigItem *BS_ICONS_SAME_SIZE;
extern BSIntegerConfigItem *BS_ICONS_SAME_SIZE_SIZE;
extern BSIntegerConfigItem *BS_FRAME_COMMON_SIZE;

void bs_settings_init();
