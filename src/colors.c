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
#include <gtk/gtk.h>
#include "colors.h"
#include "configuration.h"
#include "settings.h"

static gulong frame_color = 0l;


static void update();
static void color_change_cb(void *data);
static gulong get_theme_color();
static gulong get_user_color();


void
bs_colors_init()
{
    update();
    bs_configuration_add_callback(BS_FRAME_COLOR_CUSTOM->name, color_change_cb,
                                  NULL);
}


gulong
bs_colors_get_frame_color()
{
    return frame_color;
}


static void
update()
{
    if (bs_configuration_get_boolean_value(BS_FRAME_COLOR_CUSTOM)) {
        frame_color = get_user_color();
    } else {
        frame_color = get_theme_color();
    }
}


static void 
color_change_cb(void *data)
{
    update();
}


static gulong
get_theme_color()
{
    gulong color = 0;

    /* temporary hack until I figure out how to do this correctly */
    if (!color) {
        GtkStyle *style;
        GtkWidget *win;
        win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
        gtk_widget_realize(win);
        style = gtk_rc_get_style(win);
        color = style->mid[3].pixel;
        gtk_widget_destroy(win);
    }
    return color;
}


static gulong
get_user_color()
{
    GdkColor color;

    color.red = bs_configuration_get_int_value(BS_FRAME_COLOR_RED);
    color.blue = bs_configuration_get_int_value(BS_FRAME_COLOR_BLUE);
    color.green = bs_configuration_get_int_value(BS_FRAME_COLOR_GREEN);

    gdk_colormap_alloc_color(gdk_colormap_get_system(), &color, FALSE, TRUE);
    return color.pixel;
}


GdkColor *
bs_colors_get_custom_rgb()
{
    GdkColor *color;

    color = (GdkColor *)g_malloc(sizeof(GdkColor));
    color->red = bs_configuration_get_int_value(BS_FRAME_COLOR_RED);
    color->blue = bs_configuration_get_int_value(BS_FRAME_COLOR_BLUE);
    color->green = bs_configuration_get_int_value(BS_FRAME_COLOR_GREEN);

    return color;
}
