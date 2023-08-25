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
#include "configurator.h"
#include "groups.h"
#include "trace.h"
#include "glade_interface.h"

static GtkWidget *menu;
static GtkWidget *config_win = NULL;
static BSManagedWindow *current_win = NULL;

static void menu_init();
static void do_config(gpointer widget, gpointer data);
static void do_group(gpointer widget, gpointer data);
static gint count_g_list(GList *list);
static GtkWidget *new_group_menu_item(gint group, GSList *radio_group);
static void set_group(gpointer widget, gpointer data);
static GList *get_group_menu_items();
static void remove_group_item(gint index);

void
bs_configurator_init()
{
    menu_init();
}


static void
menu_init()
{
    GtkWidget *config_item, *group_item, *group_submenu;

    menu = gtk_menu_new();
    gtk_widget_show(menu);

    config_item = gtk_menu_item_new_with_label("Backstep Preferences...");
    gtk_widget_show(config_item);
    g_signal_connect(G_OBJECT(config_item), "activate", 
                     G_CALLBACK(do_config), NULL);

    group_item = gtk_menu_item_new_with_label("Grouping");
    group_submenu = gtk_menu_new();
    gtk_widget_show(group_item);
    gtk_widget_show(group_submenu);
    gtk_menu_item_set_submenu(GTK_MENU_ITEM(group_item), group_submenu);
    g_signal_connect(G_OBJECT(group_item), "activate", 
                     G_CALLBACK(do_group), NULL);

    gtk_menu_shell_append(GTK_MENU_SHELL(menu), config_item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), group_item);
}


void
bs_configurator_show_menu(guint32 time, BSManagedWindow *bsmwin)
{
    current_win = bsmwin;
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, 0, time);
}


static void
do_config(gpointer widget, gpointer data)
{
    if (config_win) {
        gtk_window_present(GTK_WINDOW(config_win));
        return;
    }

    config_win = create_PreferencesWindow();
    gtk_widget_show(config_win);
    current_win = NULL;
}


void 
bs_configurator_window_closed(gpointer data)
{
    config_win = NULL;
}


static void
do_group(gpointer widget, gpointer data)
{
    gint new_group_id, current_group_id, count, z;
    GList *items, *item;
    GSList *radio_group;

    new_group_id = bs_groups_next_new_group();
    current_group_id = current_win->group;
    
    items = get_group_menu_items();
    count = count_g_list(items);
    
    /* set the radio group to use.  if the list is empty, add an item and use
     * its group */
    if (count == 0) {
        GtkWidget *item;
        item = new_group_menu_item(0, NULL);
        radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
        count++;
    } else {
        GtkWidget *item = (GtkWidget *)items->data;
        radio_group = gtk_radio_menu_item_get_group(GTK_RADIO_MENU_ITEM(item));
    }

    for (z = count - 1; z > new_group_id; z--) {
        remove_group_item(z);
        count--;
    }

    for (z = count; z <= new_group_id; z++) {
        new_group_menu_item(z, radio_group);
    }

    items = get_group_menu_items();
    item = g_list_nth_data(items, current_win->group);
    gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(item), TRUE);
}


static gint 
count_g_list(GList *list)
{
    gint z = 0;
    while (list) {
        z++;
        list = list->next;
    }
    return z;
}


static GtkWidget *
new_group_menu_item(gint group, GSList *radio_group)
{
    GList *list;
    GtkWidget *parent_item, *item, *submenu;

    list = gtk_container_get_children(GTK_CONTAINER(menu));
    parent_item = (GtkWidget *)g_list_nth_data(list, 1);
    submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(parent_item));

    if (group == 0) {
        item = gtk_radio_menu_item_new_with_label(radio_group, "No Group");
    } else {
        gchar name[16];
        sprintf(name, "Group %d", group);
        item = gtk_radio_menu_item_new_with_label(radio_group, name);
    }
    gtk_widget_show(item);
    gtk_menu_shell_append(GTK_MENU_SHELL(submenu), item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(set_group), NULL);
    return item;
}


static void
set_group(gpointer widget, gpointer data)
{
    GList *items;
    gint z = 0;

    items = get_group_menu_items();
    while (items) {
        GtkWidget *item = (GtkWidget *)items->data;
        if (gtk_check_menu_item_get_active(GTK_CHECK_MENU_ITEM(item))) {
            bs_managed_window_set_group(current_win, z);
            return;
        }
        z++;
        items = items->next;
    }
}


static GList *
get_group_menu_items()
{
    GList *list;
    GtkWidget *parent_item, *submenu;

    list = gtk_container_get_children(GTK_CONTAINER(menu));
    parent_item = (GtkWidget *)g_list_nth_data(list, 1);
    submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(parent_item));
    return gtk_container_get_children(GTK_CONTAINER(submenu));
}


static void
remove_group_item(gint index)
{
    GList *list, *items;
    GtkWidget *parent_item, *submenu, *group_item;

    list = gtk_container_get_children(GTK_CONTAINER(menu));
    parent_item = (GtkWidget *)g_list_nth_data(list, 1);
    submenu = gtk_menu_item_get_submenu(GTK_MENU_ITEM(parent_item));
    items = gtk_container_get_children(GTK_CONTAINER(submenu));
    group_item = (GtkWidget *)g_list_nth_data(items, index);
    gtk_container_remove(GTK_CONTAINER(submenu), group_item);
}
