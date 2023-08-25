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
#include <glib.h>
#include <stdio.h>
#include "configuration.h"
#include "callbacktable.h"

#define BS_GROUP_NAME "global"
#define BS_DRAW_FRAMES "draw_frames"
#define BS_DRAW_FRAMES_DEFAULT TRUE

static GKeyFile *key_file = NULL;
static gchar *key_file_path = NULL;
static BSCallbackTable *callbacks;

static void reload();

static gboolean get_int_value(const gchar *group, BSIntegerConfigItem *ci,
                              gint *value);

static gboolean get_string_value(const gchar *group, BSStringConfigItem *ci,
                                 gchar **value);
static gboolean get_boolean_value(const gchar *group, BSBooleanConfigItem *ci,
                                  gboolean *value);
static void store();

void
bs_configuration_init()
{
    key_file_path = g_strconcat(g_get_home_dir(), "/.backstep", NULL);
    callbacks = bs_callback_table_new();
    reload();
}


static void
reload()
{
    gchar *conf_file;
    GError *error = NULL;

    if (key_file)
        g_key_file_free(key_file);
            
    key_file = g_key_file_new();

    if (g_file_test(key_file_path, G_FILE_TEST_EXISTS))
        if (!g_key_file_load_from_file(key_file, key_file_path, 
                                       G_KEY_FILE_KEEP_COMMENTS | 
                                       G_KEY_FILE_KEEP_TRANSLATIONS, 
                                       &error))
            g_critical(error->message);
}


gchar *
bs_configuration_get_string_value(BSStringConfigItem *ci)
{
    gchar *result;

    get_string_value(BS_GROUP_NAME, ci, &result);
    return result;
}


gint
bs_configuration_get_int_value(BSIntegerConfigItem *ci)
{
    gint result;

    get_int_value(BS_GROUP_NAME, ci, &result);
    return result;
}


gboolean
bs_configuration_get_boolean_value(BSBooleanConfigItem *ci)
{
    gboolean result;

    get_boolean_value(BS_GROUP_NAME, ci, &result);
    return result;
}


static gboolean
get_string_value(const gchar *group, BSStringConfigItem *ci, gchar **ret_value)
{
    GError *error = NULL;
    gboolean found = FALSE;
    const gchar *result;

    result = g_key_file_get_string(key_file, group, ci->name, &error);
    if (error) {
        if (error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND
         || error->code == G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
            result = g_strdup(ci->default_value);
        else
            g_critical("Cannot read config value for %s:%s: %s",
                       group, ci->name, error->message);
        g_error_free(error);
    } else
        found = TRUE;


    if (ci->allowed) {
        gint z = 0;
        gboolean found = FALSE;
        while (ci->allowed[z])
            if (strcmp(result, ci->allowed[z++]) == 0) {
                found = TRUE;
                break;
            }
        if (!found)
            g_critical("Config value %s is not allowed for %s:%s",
                       result, group, ci->name);
    }

    *ret_value = (gchar *)result;
    return found;
}


static gboolean
get_int_value(const gchar *group, BSIntegerConfigItem *ci, gint *ret_value)
{
    GError *error = NULL;
    gboolean found = FALSE;
    gint result;

    result = g_key_file_get_integer(key_file, group, ci->name, &error);
    if (error) {
        if (error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND ||
            error->code == G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
            result = ci->default_value;
        else
            g_critical("Cannot read config value for %s:%s: %s",
                       group, ci->name, error->message);
        g_error_free(error);
    } else
        found = TRUE;

    if ((ci->min != -1 && result < ci->min)
     || (ci->max != -1 && result >= ci->max))
        g_critical("Config value for %s:%s is out of range", group, ci->name);

    *ret_value = result;
    return found;
}


static gboolean
get_boolean_value(const gchar *group, BSBooleanConfigItem *ci, 
                  gboolean *ret_value)
{
    GError *error = NULL;
    gboolean found = FALSE;
    gboolean result;

    result = g_key_file_get_boolean(key_file, group, ci->name, &error);
    if (error) {
        if (error->code == G_KEY_FILE_ERROR_KEY_NOT_FOUND ||
            error->code == G_KEY_FILE_ERROR_GROUP_NOT_FOUND)
            result = ci->default_value;
        else
            g_critical("Cannot read config value for %s:%s: %s",
                       group, ci->name, error->message);
        g_error_free(error);
    } else
        found = TRUE;

    *ret_value = result;
    return found;
}



void
bs_configuration_set_value(const gchar *key, const gchar *value)
{
    g_assert(key != NULL);
    g_assert(value != NULL);
    g_key_file_set_string(key_file, BS_GROUP_NAME, key, value);
    store();
    bs_callback_table_call(callbacks, key);
}


gint 
bs_configuration_add_callback(const gchar *key, void (*callback)(void *),
                              void *data)
{
    return bs_callback_table_add(callbacks, key, callback, data);
}


static void
store()
{
    gchar **groups, *group;
    gint i = 0;
    FILE *out;

    out = fopen(key_file_path, "w");
    if (!out) {
        g_warning("Could not open configuration file for writing");
        return;
    }

    groups = g_key_file_get_groups(key_file, NULL);

    while ((group = groups[i++])) {
        gchar **keys, *key;
        gint j = 0;

        fprintf(out, "[%s]\n", group);
        keys = g_key_file_get_keys(key_file, group, NULL, NULL);

        while ((key = keys[j++]))
            fprintf(out, "%s=%s\n", key, g_key_file_get_string(key_file, group,
                                                               key, NULL));
    }
    fclose(out);
}


void
bs_configuration_remove_callback(gint id)
{
    bs_callback_table_remove(callbacks, id);
}
