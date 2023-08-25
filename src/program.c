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
#include <stdlib.h>
#include <gtk/gtk.h>
#include "program.h"

static gboolean no_daemon = FALSE;
static gchar *client_id = NULL;
static gchar *prog_name = NULL;
static gboolean version = FALSE;

void 
bs_program_parse_args(gint *argc, gchar ***argv)
{
    static gchar *description = "Show hidden windows as icons on the desktop";
    static GOptionEntry entries[] = {
        { "client", 'c', 0, G_OPTION_ARG_STRING, &client_id,
          "Client id for session management", "ID" },
        { "no-daemon", 'n', 0, G_OPTION_ARG_NONE, &no_daemon,
          "Do not run as a daemon", NULL },
        { "version", 'v', 0, G_OPTION_ARG_NONE, &version, 
          "output version information and exit", NULL },
        { NULL }
    };
    GOptionContext *context;
    GError *error = NULL;
    context = g_option_context_new(description);
    g_option_context_add_main_entries(context, entries, NULL);
    g_option_context_add_group(context, gtk_get_option_group(TRUE));
    g_option_context_parse(context, argc, argv, &error);
    if (error) {
        fprintf(stderr, "%s: %s\n", (*argv)[0], error->message);
        g_error_free(error);
        exit(EXIT_FAILURE);
    }

    if (version) {
        fprintf(stdout, "%s %s\n", PACKAGE, VERSION);
        fprintf(stdout, "Copyright (C) 2005 Christopher Carroll\n");
        fprintf(stdout, "This is free software; see the source for copying "
                        "conditions.  There is NO\nwarranty; not even for "
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR "
                        "PURPOSE.\n");
        exit(EXIT_SUCCESS);
    }

    if (*argc != 1) {
        fprintf(stderr, "Usage: %s [OPTION...] %s\n", (*argv)[0], description);
        exit(EXIT_FAILURE);
    }

    prog_name = g_strdup((*argv)[0]);
}


gboolean
bs_program_no_daemon()
{
    return no_daemon;
}

const gchar *
bs_program_client_id()
{
    return client_id;
}


const gchar *
bs_program_name()
{
    return prog_name;
}
