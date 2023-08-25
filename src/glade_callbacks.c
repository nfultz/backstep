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

#include <gtk/gtk.h>

#include "glade_callbacks.h"
#include "glade_interface.h"
#include "support.h"
#include "settings.h"
#include "configurator.h"
#include "configuration.h"


void
on_draw_frames_checkbox_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
    if (gtk_toggle_button_get_active(togglebutton))
        bs_configuration_set_value(BS_FRAME_SHOW->name, "true");
    else
        bs_configuration_set_value(BS_FRAME_SHOW->name, "false");
}


void
on_large_border_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
    bs_configuration_set_value(BS_FRAME_BORDER->name, "large");
}


void
on_small_border_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{
    bs_configuration_set_value(BS_FRAME_BORDER->name, "small");
}


void
on_no_border_radio_button_clicked      (GtkButton       *button,
                                        gpointer         user_data)
{
    bs_configuration_set_value(BS_FRAME_BORDER->name, "none");
}


void
on_theme_color_radio_button_clicked    (GtkButton       *button,
                                        gpointer         user_data)
{

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
        bs_configuration_set_value(BS_FRAME_COLOR_CUSTOM->name, "false");
    }
}


void
on_select_color_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data)
{

    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button))) {
        GdkColor color;
        GtkWidget *widget;
        char buf[6];

        widget = lookup_widget(GTK_WIDGET(button), "CustomColorButton");
        gtk_color_button_get_color(GTK_COLOR_BUTTON(widget), &color);
        sprintf(buf, "%d", color.red);
        bs_configuration_set_value(BS_FRAME_COLOR_RED->name, buf);
        sprintf(buf, "%d", color.blue);
        bs_configuration_set_value(BS_FRAME_COLOR_BLUE->name, buf);
        sprintf(buf, "%d", color.green);
        bs_configuration_set_value(BS_FRAME_COLOR_GREEN->name, buf);

        bs_configuration_set_value(BS_FRAME_COLOR_CUSTOM->name, "true");
    }
}


void
on_close_button_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    GtkWidget *window;
    window = lookup_widget(GTK_WIDGET(button), "PreferencesWindow");
    gtk_widget_destroy(window);
    bs_configurator_window_closed();
}


gboolean
on_PreferencesWindow_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
    bs_configurator_window_closed();
    return FALSE;
}


gboolean
on_PreferencesWindow_destroy_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{

  return FALSE;
}

