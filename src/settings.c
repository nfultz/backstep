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
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include "settings.h"

static BSBooleanConfigItem _BS_ICONS_XRENDER = {
    "icons.xrender",
    FALSE
};

static BSBooleanConfigItem _BS_ICONS_STICKY = {
    "icons.sticky",
    FALSE
};

static BSBooleanConfigItem _BS_FRAME_SHOW = { 
    "frame.show",
    TRUE
};

static BSStringConfigItem _BS_FRAME_BORDER = { 
    "frame.border",
   "large",
   { "large", "small", "none", NULL }
};


static BSBooleanConfigItem _BS_FRAME_COLOR_CUSTOM = { 
    "frame.color.custom",
    FALSE
};

static BSIntegerConfigItem _BS_FRAME_COLOR_RED = {
    "frame.color.red",
    0, 0, 65536,
};

static BSIntegerConfigItem _BS_FRAME_COLOR_BLUE = {
    "frame.color.blue",
    0, 0, 65536,
};

static BSIntegerConfigItem _BS_FRAME_COLOR_GREEN = {
    "frame.color.green",
    0, 0, 65536,
};

static BSBooleanConfigItem _BS_ICONS_SAME_SIZE = {
    "icons.same_size",
    FALSE
};

static BSIntegerConfigItem _BS_ICONS_SAME_SIZE_SIZE = {
    "icons.same_size.size",
    48, 1, -1,
};

static BSIntegerConfigItem _BS_FRAME_COMMON_SIZE = {
    "frame.common_size",
    48, 1, -1,
};

BSBooleanConfigItem *BS_ICONS_XRENDER;
BSBooleanConfigItem *BS_FRAME_SHOW;
BSStringConfigItem *BS_FRAME_BORDER;
BSBooleanConfigItem *BS_ICONS_STICKY;
BSBooleanConfigItem *BS_FRAME_COLOR_CUSTOM;
BSIntegerConfigItem *BS_FRAME_COLOR_RED;
BSIntegerConfigItem *BS_FRAME_COLOR_BLUE;
BSIntegerConfigItem *BS_FRAME_COLOR_GREEN;
BSBooleanConfigItem *BS_ICONS_SAME_SIZE;
BSIntegerConfigItem *BS_ICONS_SAME_SIZE_SIZE;
BSIntegerConfigItem *BS_FRAME_COMMON_SIZE;

void
bs_settings_init()
{
    BS_FRAME_SHOW = &_BS_FRAME_SHOW;
    BS_FRAME_BORDER = &_BS_FRAME_BORDER;
    BS_ICONS_STICKY = &_BS_ICONS_STICKY;
    BS_ICONS_XRENDER = &_BS_ICONS_XRENDER;
    BS_FRAME_COLOR_CUSTOM = &_BS_FRAME_COLOR_CUSTOM;
    BS_FRAME_COLOR_RED = &_BS_FRAME_COLOR_RED;
    BS_FRAME_COLOR_BLUE = &_BS_FRAME_COLOR_BLUE;
    BS_FRAME_COLOR_GREEN = &_BS_FRAME_COLOR_GREEN;
    BS_ICONS_SAME_SIZE = &_BS_ICONS_SAME_SIZE;
    BS_ICONS_SAME_SIZE_SIZE = &_BS_ICONS_SAME_SIZE_SIZE;
    BS_FRAME_COMMON_SIZE = &_BS_FRAME_COMMON_SIZE;
}
