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
/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "glade_callbacks.h"
#include "glade_interface.h"
#include "support.h"
#include "configuration.h"
#include "settings.h"
#include "colors.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_PreferencesWindow (void)
{
  GtkWidget *PreferencesWindow;
  GtkWidget *MainBox;
  GtkWidget *MainControlsBox;
  GtkWidget *DrawFramesCheckbox;
  GtkWidget *BorderBox;
  GtkWidget *LargeBorderRadioButton;
  GSList *LargeBorderRadioButton_group = NULL;
  GtkWidget *SmallBorderRadioButton;
  GtkWidget *NoBorderRadioButton;
  GtkWidget *ColorBox;
  GtkWidget *ThemeColorRadioButton;
  GSList *ThemeColorRadioButton_group = NULL;
  GtkWidget *CustomColorBox;
  GtkWidget *SelectColorRadioButton;
  GtkWidget *CustomColorButton;
  GtkWidget *ButtonBox;
  GtkWidget *CloseButton;

  gchar *border_name;
  gboolean custom_color;
  GdkColor *color;

  PreferencesWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (PreferencesWindow), _("Backstep Preferences"));

  MainBox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (MainBox);
  gtk_container_add (GTK_CONTAINER (PreferencesWindow), MainBox);

  MainControlsBox = gtk_vbox_new (FALSE, 15);
  gtk_widget_show (MainControlsBox);
  gtk_box_pack_start (GTK_BOX (MainBox), MainControlsBox, TRUE, TRUE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (MainControlsBox), 15);

  DrawFramesCheckbox = gtk_check_button_new_with_mnemonic (_("Draw frames"));
  gtk_widget_show (DrawFramesCheckbox);
  gtk_box_pack_start (GTK_BOX (MainControlsBox), DrawFramesCheckbox, FALSE, FALSE, 1);

  BorderBox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (BorderBox);
  gtk_box_pack_start (GTK_BOX (MainControlsBox), BorderBox, FALSE, FALSE, 0);

  LargeBorderRadioButton = gtk_radio_button_new_with_mnemonic (NULL, _("Large border"));
  gtk_widget_show (LargeBorderRadioButton);
  gtk_box_pack_start (GTK_BOX (BorderBox), LargeBorderRadioButton, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (LargeBorderRadioButton), LargeBorderRadioButton_group);
  LargeBorderRadioButton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (LargeBorderRadioButton));

  SmallBorderRadioButton = gtk_radio_button_new_with_mnemonic (NULL, _("Small border"));
  gtk_widget_show (SmallBorderRadioButton);
  gtk_box_pack_start (GTK_BOX (BorderBox), SmallBorderRadioButton, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (SmallBorderRadioButton), LargeBorderRadioButton_group);
  LargeBorderRadioButton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (SmallBorderRadioButton));

  NoBorderRadioButton = gtk_radio_button_new_with_mnemonic (NULL, _("No border"));
  gtk_widget_show (NoBorderRadioButton);
  gtk_box_pack_start (GTK_BOX (BorderBox), NoBorderRadioButton, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (NoBorderRadioButton), LargeBorderRadioButton_group);
  LargeBorderRadioButton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (NoBorderRadioButton));

  ColorBox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (ColorBox);
  gtk_box_pack_start (GTK_BOX (MainControlsBox), ColorBox, TRUE, TRUE, 0);

  ThemeColorRadioButton = gtk_radio_button_new_with_mnemonic (NULL, _("Get frame color from current theme"));
  gtk_widget_show (ThemeColorRadioButton);
  gtk_box_pack_start (GTK_BOX (ColorBox), ThemeColorRadioButton, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (ThemeColorRadioButton), ThemeColorRadioButton_group);
  ThemeColorRadioButton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (ThemeColorRadioButton));

  CustomColorBox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (CustomColorBox);
  gtk_box_pack_start (GTK_BOX (ColorBox), CustomColorBox, TRUE, TRUE, 0);

  SelectColorRadioButton = gtk_radio_button_new_with_mnemonic (NULL, _("Select custom frame color"));
  gtk_widget_show (SelectColorRadioButton);
  gtk_box_pack_start (GTK_BOX (CustomColorBox), SelectColorRadioButton, FALSE, FALSE, 0);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (SelectColorRadioButton), ThemeColorRadioButton_group);
  ThemeColorRadioButton_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (SelectColorRadioButton));

  CustomColorButton = gtk_color_button_new ();
  gtk_widget_show (CustomColorButton);
  gtk_box_pack_start (GTK_BOX (CustomColorBox), CustomColorButton, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (CustomColorButton), 5);

  ButtonBox = gtk_hbutton_box_new ();
  gtk_widget_show (ButtonBox);
  gtk_box_pack_start (GTK_BOX (MainBox), ButtonBox, FALSE, FALSE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (ButtonBox), GTK_BUTTONBOX_END);

  CloseButton = gtk_button_new_with_mnemonic (_("Close"));
  gtk_widget_show (CloseButton);
  gtk_container_add (GTK_CONTAINER (ButtonBox), CloseButton);
  GTK_WIDGET_SET_FLAGS (CloseButton, GTK_CAN_DEFAULT);


  gtk_toggle_button_set_active(
          GTK_TOGGLE_BUTTON(DrawFramesCheckbox),
          bs_configuration_get_boolean_value(BS_FRAME_SHOW));

  border_name = bs_configuration_get_string_value(BS_FRAME_BORDER);
  if (g_str_equal(border_name, "large"))
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(LargeBorderRadioButton),
                                   TRUE);
  else if (g_str_equal(border_name, "small"))
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(SmallBorderRadioButton),
                                   TRUE);
  else if (g_str_equal(border_name, "none"))
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(NoBorderRadioButton), 
                                   TRUE);


  custom_color = bs_configuration_get_boolean_value(BS_FRAME_COLOR_CUSTOM);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(SelectColorRadioButton),
                               custom_color);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ThemeColorRadioButton),
                               !custom_color);
        
    color = bs_colors_get_custom_rgb();
    gtk_color_button_set_color(GTK_COLOR_BUTTON(CustomColorButton), color);
    g_free(color);


  g_signal_connect ((gpointer) PreferencesWindow, "delete_event",
                    G_CALLBACK (on_PreferencesWindow_delete_event),
                    NULL);
  g_signal_connect ((gpointer) PreferencesWindow, "destroy_event",
                    G_CALLBACK (on_PreferencesWindow_destroy_event),
                    NULL);
  g_signal_connect ((gpointer) DrawFramesCheckbox, "toggled",
                    G_CALLBACK (on_draw_frames_checkbox_toggled),
                    NULL);
  g_signal_connect ((gpointer) LargeBorderRadioButton, "clicked",
                    G_CALLBACK (on_large_border_radio_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) SmallBorderRadioButton, "clicked",
                    G_CALLBACK (on_small_border_radio_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) NoBorderRadioButton, "clicked",
                    G_CALLBACK (on_no_border_radio_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) ThemeColorRadioButton, "clicked",
                    G_CALLBACK (on_theme_color_radio_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) SelectColorRadioButton, "clicked",
                    G_CALLBACK (on_select_color_radio_button_clicked),
                    NULL);
  g_signal_connect ((gpointer) CloseButton, "clicked",
                    G_CALLBACK (on_close_button_clicked),
                    NULL);

  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (PreferencesWindow, PreferencesWindow, "PreferencesWindow");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, MainBox, "MainBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, MainControlsBox, "MainControlsBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, DrawFramesCheckbox, "DrawFramesCheckbox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, BorderBox, "BorderBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, LargeBorderRadioButton, "LargeBorderRadioButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, SmallBorderRadioButton, "SmallBorderRadioButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, NoBorderRadioButton, "NoBorderRadioButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, ColorBox, "ColorBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, ThemeColorRadioButton, "ThemeColorRadioButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, CustomColorBox, "CustomColorBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, SelectColorRadioButton, "SelectColorRadioButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, CustomColorButton, "CustomColorButton");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, ButtonBox, "ButtonBox");
  GLADE_HOOKUP_OBJECT (PreferencesWindow, CloseButton, "CloseButton");

  return PreferencesWindow;
}

