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


void
on_draw_frames_checkbox_toggled        (GtkToggleButton *togglebutton,
                                        gpointer         user_data);

void
on_large_border_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_small_border_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_no_border_radio_button_clicked      (GtkButton       *button,
                                        gpointer         user_data);

void
on_theme_color_radio_button_clicked    (GtkButton       *button,
                                        gpointer         user_data);

void
on_select_color_radio_button_clicked   (GtkButton       *button,
                                        gpointer         user_data);

void
on_close_button_clicked                (GtkButton       *button,
                                        gpointer         user_data);

gboolean
on_PreferencesWindow_delete_event      (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);

gboolean
on_PreferencesWindow_destroy_event     (GtkWidget       *widget,
                                        GdkEvent        *event,
                                        gpointer         user_data);
