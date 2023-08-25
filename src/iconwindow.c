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
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/xpm.h>
#include <X11/extensions/shape.h>
#include "iconwindow.h"
#include "configuration.h"
#include "configurator.h"
#include "settings.h"
#include "colors.h"
#include "atoms.h"
#include "trace.h"



/*
 * Frames can have a large or a small border size.  There is a set of masks
 * corresponding to each border size used for rounding the corners.
 *
 * Frames can also have no border. Additionally, a mask can be applied over the
 * child area of the frame (non-norder area).  Therefore is is possible to have
 * a completely invisible frame.
 */
static gint large_border = 6;
static gint small_border = 2;

/*
 * These masks are meant to be subtracted from the visible area of the frame.
 */
static Pixmap large_topleft_mask = None;
static Pixmap large_topright_mask = None;
static Pixmap large_bottomright_mask = None;
static Pixmap large_bottomleft_mask = None;

static Pixmap small_topleft_mask = None;
static Pixmap small_topright_mask = None;
static Pixmap small_bottomright_mask = None;
static Pixmap small_bottomleft_mask = None;

static gchar *large_topleft_xpm[] = {
    "6 6 2 1",
    " 	c None",
    ".	c #000000",
    "....  ",
    "..    ",
    ".     ",
    ".     ",
    "      ",
    "      "
};

static gchar *small_topleft_xpm[] = {
    "2 2 2 1",
    "   c None",
    ".  c #000000",
    "..",
    ". "
};

static gchar *large_topright_xpm[] = {
    "6 6 2 1",
    " 	c None",
    ".	c #000000",
    "  ....",
    "    ..",
    "     .",
    "     .",
    "      ",
    "      "
};

static gchar *small_topright_xpm[] = {
    "2 2 2 1",
    "   c None",
    ".  c #000000",
    "..",
    " ."
};

static gchar *large_bottomright_xpm[] = {
    "6 6 2 1",
    " 	c None",
    ".	c #000000",
    "      ",
    "      ",
    "     .",
    "     .",
    "    ..",
    "  ...."
};

static gchar *small_bottomright_xpm[] = {
    "2 2 2 1",
    "   c None",
    ".  c #000000",
    " .",
    ".."
};

static gchar *large_bottomleft_xpm[] = {
    "6 6 2 1",
    " 	c None",
    ".	c #000000",
    "      ",
    "      ",
    ".     ",
    ".     ",
    "..    ",
    "....  "
};

static gchar *small_bottomleft_xpm[] = {
    "2 2 2 1",
    "   c None",
    ".  c #000000",
    ". ",
    ".."
};


/*
 * We have to keep track of the state of the mouse so we know differentiate 
 * between releases that complete mouse clicks and releases that complete mouse
 * moves
 */
#define MOUSE_UP     0
#define MOUSE_DOWN   1
#define MOUSE_MOVING 2


#define BORDER_NONE   0
#define BORDER_SMALL  1
#define BORDER_LARGE  2


static void update(gpointer data);
static gint get_border_for_name(const gchar *name);
static gint get_border_width(gint border);
static unsigned long setup_window_attributes(XSetWindowAttributes *attrs);
static unsigned long get_bg_color();
static void shape_window(Window window, gint width, gint height, gint border);
static GdkFilterReturn event_filter(GdkXEvent *xevent, GdkEvent *gdkevent,
                                    gpointer data);
static void set_hints(Window window, gboolean sticky, gint desktop);

static void frame_border_cb(void *data);
static void frame_show_cb(void *data);
static void frame_color_cb(void *data);
static void redraw(BSIconWindow *iw);
static void destroy_windows(BSIconWindow *iw);
static void unmask_window(BSIconWindow *iw, gint width, gint height, 
                          gint border);

static void send_desktop_message(BSIconWindow *iw, gint desktop);
static void render_pixmap(BSIconWindow *iw, gint x, gint y, gint w, gint h);


/*
 * Called once to create all the pixmap masks
 */
static void create_masks();


/*
 * Called by create_masks() to create an individual mask
 */
static void create_mask(gchar *data[], Pixmap *mask);


BSIconWindow *
bs_icon_window_new(BSWindowPixmap *src)
{
    BSIconWindow *iw;

    iw = (BSIconWindow *)g_malloc(sizeof(BSIconWindow));
    iw->src = src;
    iw->window = None;
    iw->gdkwindow = NULL;
    iw->dst_window = None;
    iw->dst_gdkwindow = NULL;
    iw->has_position = FALSE;
    iw->activated_callback = NULL;
    iw->moved_callback = NULL;
    iw->rightclick_action = NULL;
    iw->rightclick_data = NULL;
    iw->sticky = FALSE;
    iw->border = BORDER_NONE;
    iw->mouse_state = MOUSE_UP;

    bs_window_pixmap_set_changed_callback(src, update, iw);

    trace("checking if src pixmap is ready");
    if (bs_window_pixmap_is_ready(src)) {
        trace("yes");
        update(iw);
    } else {
        trace("no");
    }

    iw->frame_border_callback_id = 
        bs_configuration_add_callback(BS_FRAME_BORDER->name, frame_border_cb, 
                                      iw);
    iw->frame_show_callback_id = 
        bs_configuration_add_callback(BS_FRAME_SHOW->name, frame_show_cb, iw);
    iw->frame_color_callback_id =
        bs_configuration_add_callback(BS_FRAME_COLOR_CUSTOM->name, 
                                      frame_color_cb, iw);
    return iw;
}


static void
destroy_windows(BSIconWindow *iw)
{
    if (iw->dst_gdkwindow) {
        g_object_unref(iw->dst_gdkwindow);
        iw->dst_gdkwindow = NULL;
        XDestroyWindow(GDK_DISPLAY(), iw->dst_window);
        iw->dst_window = None;
    }

    if (iw->gdkwindow) {
        g_object_unref(iw->gdkwindow);
        iw->gdkwindow = NULL;
        XDestroyWindow(GDK_DISPLAY(), iw->window);
        iw->window = None;
    }
}


void
bs_icon_window_free(BSIconWindow *iw)
{
    destroy_windows(iw);
    if (iw->activated_callback)
        bs_callback_free(iw->activated_callback);
    if (iw->moved_callback)
        bs_callback_free(iw->moved_callback);
    bs_configuration_remove_callback(iw->frame_border_callback_id);
    bs_configuration_remove_callback(iw->frame_show_callback_id);
    bs_configuration_remove_callback(iw->frame_color_callback_id);
    g_free(iw);
}

static void 
update(gpointer data)
{
    gint width, height;
    BSIconWindow *iw;
    
    g_assert(data != NULL);

    iw = (BSIconWindow *)data;

    if (bs_window_pixmap_use_common_frame_size(iw->src)) {
        width = bs_configuration_get_int_value(BS_FRAME_COMMON_SIZE);
        height = width;
    } else
        bs_window_pixmap_get_size(iw->src, &width, &height);
    g_assert(width > 0 && height > 0);

    if (iw->window == None) {
        gchar *border_name;
        gint border_width;
        XSetWindowAttributes attrs;
        unsigned long attrs_mask;
        GdkBitmap *mask;

        if (large_topleft_mask == None)
            create_masks();


        border_name = bs_configuration_get_string_value(BS_FRAME_BORDER);
        iw->border = get_border_for_name(border_name);
        g_free(border_name);
        g_assert(iw->border != -1);
        border_width = get_border_width(iw->border);
        
        attrs_mask = setup_window_attributes(&attrs);

        iw->window = XCreateWindow(GDK_DISPLAY(),
                                   GDK_ROOT_WINDOW(),
                                   0,
                                   0,
                                   width + (2 * border_width),
                                   height + (2 * border_width),
                                   0,
                                   CopyFromParent,
                                   InputOutput,
                                   CopyFromParent,
                                   attrs_mask,
                                   &attrs);
        iw->gdkwindow = gdk_window_foreign_new(iw->window);

        iw->dst_window = XCreateSimpleWindow(GDK_DISPLAY(),
                                             iw->window, 
                                             border_width,
                                             border_width,
                                             width,
                                             height,
                                             0,
                                             0,
                                             0);
        iw->dst_gdkwindow = gdk_window_foreign_new(iw->dst_window);

        mask = bs_window_pixmap_get_mask(iw->src);
        if (mask) {
            gboolean show_frame;
            gint pm_width, pm_height;
            bs_window_pixmap_get_size(iw->src, &pm_width, &pm_height);
            show_frame = bs_configuration_get_boolean_value(BS_FRAME_SHOW);

            if (!show_frame) {

                gdk_window_shape_combine_mask(
                    iw->gdkwindow, mask, 
                    border_width + ((width - pm_width) / 2),
                    border_width + ((height - pm_height) / 2)
                );

                if (iw->border != BORDER_NONE) {
                    XRectangle rects[4];

                    rects[0].x = 0;
                    rects[0].y = 0;
                    rects[0].width = width + (2 * border_width);
                    rects[0].height = border_width;
                    rects[1].x = 0;
                    rects[1].y = 0;
                    rects[1].width = border_width;
                    rects[1].height = height + (2 * border_width);
                    rects[2].x = width + border_width;
                    rects[2].y = 0;
                    rects[2].width = border_width;
                    rects[2].height = height + (2 * border_width);
                    rects[3].x = 0;
                    rects[3].y = height + border_width;
                    rects[3].width = width + (2 * border_width);
                    rects[3].height = border_width;

                    XShapeCombineRectangles(GDK_DISPLAY(), iw->window,
                                            ShapeBounding, 0, 0, rects, 4, 
                                            ShapeUnion, 1);
                }
            }
            gdk_window_shape_combine_mask(
                iw->dst_gdkwindow, mask,
                ((width - pm_width) / 2),
                ((height - pm_height) / 2)
            );
        }
        shape_window(iw->window, width, height, iw->border);
        XMapWindow(GDK_DISPLAY(), iw->dst_window);
        set_hints(iw->window, iw->sticky, -1);

        gdk_window_set_events(iw->gdkwindow, GDK_BUTTON_PRESS_MASK |
                                             GDK_BUTTON_RELEASE_MASK |
                                             GDK_BUTTON1_MOTION_MASK);
        gdk_window_set_events(iw->dst_gdkwindow, GDK_EXPOSURE_MASK);
        gdk_window_add_filter(iw->gdkwindow, event_filter, iw);
        gdk_window_add_filter(iw->dst_gdkwindow, event_filter, iw);

    } else {
        /* respond to src resize */
    }
}


static gint 
get_border_for_name(const gchar *name)
{
    g_assert(name != NULL);
    if (strcmp(name, "large") == 0)
        return BORDER_LARGE;
    if (strcmp(name, "small") == 0)
        return BORDER_SMALL;
    if (strcmp(name, "none") == 0)
        return BORDER_NONE;
    return -1;
}


static gint 
get_border_width(gint border)
{
    switch (border) {
        case BORDER_NONE:
            return 0;
        case BORDER_SMALL:
            return small_border;
        case BORDER_LARGE:
            return large_border;
    }
    return 0;
}


static unsigned long
setup_window_attributes(XSetWindowAttributes *attrs)
{
    attrs->background_pixmap = None;
    attrs->background_pixel = get_bg_color();
    attrs->border_pixmap = None;
    attrs->border_pixel = None;
    attrs->event_mask = ButtonPressMask | ButtonReleaseMask | Button1MotionMask;
    attrs->override_redirect = False;
    return CWBackPixmap | CWBackPixel | CWBorderPixmap | CWBorderPixel |
           CWOverrideRedirect | CWEventMask;
}


static unsigned long
get_bg_color()
{
    return bs_colors_get_frame_color();
}


void
bs_icon_window_set_sticky(BSIconWindow *iw, gboolean sticky)
{
    iw->sticky = sticky;
}


void
bs_icon_window_show(BSIconWindow *iw, gint desktop)
{
    gint x, y;
    trace("mapping window %ld", iw->window);
    if (!iw->has_position)
        iw->has_position = TRUE;

    set_hints(iw->window, iw->sticky, desktop);
    XMapWindow(GDK_DISPLAY(), iw->window);
    send_desktop_message(iw, desktop);
}


void
bs_icon_window_hide(BSIconWindow *iw)
{
    /*XEvent e;
    Atom state[15];
    int i = 0;*/
    trace("unmapping icon %ld", iw->window);
/*
	e.xclient.type = ClientMessage;
	e.xclient.message_type = NET_WM_STATE;
	e.xclient.display = GDK_DISPLAY();
	e.xclient.window = iw->window;
	e.xclient.format = 32;
	e.xclient.data.l[3] = 0l;
	e.xclient.data.l[4] = 0l;
    e.xclient.data.l[0] = 1;
    e.xclient.data.l[1] = NET_WM_STATE_HIDDEN;
    e.xclient.data.l[2] = 0l;

    XSendEvent(GDK_DISPLAY(), GDK_ROOT_WINDOW(), False, SubstructureRedirectMask | SubstructureNotifyMask, &e);

*/
    XUnmapWindow(GDK_DISPLAY(), iw->window);
 /*   
    if (iw->sticky)
        state[i++] = NET_WM_STATE_STICKY;

    state[i++] = NET_WM_STATE_SKIP_TASKBAR;
    state[i++] = NET_WM_STATE_SKIP_PAGER;
    state[i++] = NET_WM_STATE_HIDDEN;

    XChangeProperty(GDK_DISPLAY(), iw->window, NET_WM_STATE, XA_ATOM, 32,
                    PropModeReplace, (unsigned char *)state, i);*/
}


void
bs_icon_window_set_activated_callback(BSIconWindow *iw,
                                      void(*callback)(gpointer data),
                                      gpointer data)
{
    g_assert(iw != NULL);
    g_assert(callback != NULL);
    if (iw->activated_callback != NULL)
        g_warning("Assigning callback over old callback");

    iw->activated_callback = bs_callback_new(callback, data);
}


static void 
shape_window(Window window, gint width, gint height, gint border)
{
    gint border_width;

    g_assert(window != None);

    border_width = get_border_width(border);

    /*
     * Round the corners if we have a border
     */
    if (border != BORDER_NONE) {
        gboolean large = (border == BORDER_LARGE);

        XShapeCombineMask(GDK_DISPLAY(),
                          window,
                          ShapeBounding,
                          0,
                          0,
                          large ? large_topleft_mask : small_topleft_mask,
                          ShapeSubtract);

        XShapeCombineMask(GDK_DISPLAY(),
                          window,
                          ShapeBounding,
                          border_width + width,
                          0,
                          large ? large_topright_mask : small_topright_mask,
                          ShapeSubtract);

        XShapeCombineMask(GDK_DISPLAY(),
                          window,
                          ShapeBounding,
                          0,
                          border_width + height,
                          large ? large_bottomleft_mask : small_bottomleft_mask,
                          ShapeSubtract);

        XShapeCombineMask(GDK_DISPLAY(),
                          window,
                          ShapeBounding,
                          border_width + width,
                          border_width + height,
                          large ? large_bottomright_mask :
                                  small_bottomright_mask,
                          ShapeSubtract);
    }
}


static void 
create_masks()
{
    create_mask(large_topleft_xpm, &large_topleft_mask);
    create_mask(large_topright_xpm, &large_topright_mask);
    create_mask(large_bottomright_xpm, &large_bottomright_mask);
    create_mask(large_bottomleft_xpm, &large_bottomleft_mask);
    create_mask(small_topleft_xpm, &small_topleft_mask);
    create_mask(small_topright_xpm, &small_topright_mask);
    create_mask(small_bottomright_xpm, &small_bottomright_mask);
    create_mask(small_bottomleft_xpm, &small_bottomleft_mask);
}


static void 
create_mask(gchar *data[], Pixmap *mask)
{
    Pixmap tmp_pm;

    if (XpmCreatePixmapFromData(GDK_DISPLAY(), GDK_ROOT_WINDOW(), 
                                data, &tmp_pm, mask, NULL) == XpmSuccess)
        XFreePixmap(GDK_DISPLAY(), tmp_pm);
    else
        g_critical("Could not create pixmap from data");
}


static void
set_hints(Window window, gboolean sticky, gint desktop)
{
    Atom types[1];
    Atom states[4];
    gint desktops[1];
    
    types[0] = NET_WM_WINDOW_TYPE_DOCK;
    states[0] = NET_WM_STATE_SKIP_TASKBAR;
    states[1] = NET_WM_STATE_SKIP_PAGER;
    states[2] = NET_WM_STATE_BELOW;
    states[3] = NET_WM_STATE_STICKY;

    XChangeProperty(GDK_DISPLAY(), window, NET_WM_WINDOW_TYPE, XA_ATOM, 32,
                    PropModeReplace, (guchar *)types, 1);
    XChangeProperty(GDK_DISPLAY(), window, NET_WM_STATE, XA_ATOM, 32,
                    PropModeReplace, (guchar *)states, sticky ? 4 : 3);

    if (!sticky && desktop != -1) {
        desktops[0] = desktop;
        XChangeProperty(GDK_DISPLAY(), window, NET_WM_DESKTOP, XA_CARDINAL, 32,
                        PropModeReplace, (guchar *)desktops, 1);
    }
}


static GdkFilterReturn
event_filter(GdkXEvent *xevent, GdkEvent *gdkevent, gpointer data)
{
    XEvent *event;
    BSIconWindow *iw;
    XMotionEvent *motion_event;
    XButtonEvent *button_event;
    XExposeEvent *expose_event;
    static gint x_origin = 0, y_origin = 0;
    gint x, y, dx, dy;

    event = (XEvent *)xevent;
    iw = (BSIconWindow *)data;

    switch (event->type) {
        case  MotionNotify:
            motion_event = (XMotionEvent *)event;
            iw->mouse_state = MOUSE_MOVING;
            dx = motion_event->x_root - x_origin;
            dy = motion_event->y_root - y_origin;
            gdk_window_get_origin(iw->gdkwindow, &x, &y);
            gdk_window_move(iw->gdkwindow, x + dx, y + dy);
            x_origin = motion_event->x_root;
            y_origin = motion_event->y_root;
            break;
        case ButtonPress:
            button_event = (XButtonEvent *)event;
            if (button_event->button == 1) {
                iw->mouse_state = MOUSE_DOWN;
                x_origin = button_event->x_root;
                y_origin = button_event->y_root;
            } else if (button_event->button == 3) {
                if (iw->rightclick_action) {
                    iw->rightclick_action(button_event->time,
                                          iw->rightclick_data);
                }
            }
            break;
        case ButtonRelease:
            button_event = (XButtonEvent *)event;
            if (button_event->button == 1) {
                if (iw->mouse_state == MOUSE_DOWN) {
                    if (iw->activated_callback)
                        bs_callback_call(iw->activated_callback);
                } else if (iw->mouse_state == MOUSE_MOVING) {
                    if (iw->moved_callback)
                        bs_callback_call(iw->moved_callback);
                }
                iw->mouse_state = MOUSE_UP;
            }
            break;
        case Expose:
            expose_event = (XExposeEvent *)event;
            render_pixmap(iw, expose_event->x, expose_event->y,
                          expose_event->width, expose_event->height);
            /*XCopyArea(GDK_DISPLAY(),
                      GDK_PIXMAP_XID(bs_window_pixmap_get_pixmap(iw->src)),
                      iw->dst_window,
                      DefaultGC(GDK_DISPLAY(), 
                                DefaultScreen(GDK_DISPLAY())),
                      expose_event->x,
                      expose_event->y,
                      expose_event->width,
                      expose_event->height,
                      expose_event->x,
                      expose_event->y);*/
            break;
    }
    return GDK_FILTER_CONTINUE;
}


gboolean
bs_icon_window_is_positioned(BSIconWindow *iw)
{
    return iw->has_position;
}


void
bs_icon_window_show_at(BSIconWindow *iw, gint desktop, gint x, gint y)
{
    trace("mapping window %ld to %d, %d", iw->window, x, y);
    XMoveWindow(GDK_DISPLAY(), iw->window, x, y);
    iw->has_position = TRUE;

    set_hints(iw->window, iw->sticky, desktop);
    XMapWindow(GDK_DISPLAY(), iw->window);
    send_desktop_message(iw, desktop);
}


void
bs_icon_window_get_size(BSIconWindow *iw, gint *width, gint *height)
{
    gdk_window_get_geometry(iw->gdkwindow, NULL, NULL, width, height, NULL);
}    

    
static void 
frame_border_cb(void *data)
{
    BSIconWindow *iw = (BSIconWindow *)data;
    if (bs_window_pixmap_is_ready(iw->src))
        redraw(iw);
}


static void 
frame_show_cb(void *data)
{
    BSIconWindow *iw = (BSIconWindow *)data;
    if (bs_window_pixmap_is_ready(iw->src))
        redraw(iw);
}


static void
redraw(BSIconWindow *iw)
{
    GdkBitmap *mask;
    gint old_height, old_width, old_border_width;
    gint width, height, full_width, full_height, border_width;
    gint pm_width, pm_height;
    gchar *border_name;
    GdkRectangle rect;
    XSetWindowAttributes attrs;

    g_assert(iw != NULL);

    gdk_window_get_size(iw->dst_gdkwindow, &old_width, &old_height);
    old_border_width = get_border_width(iw->border);

    unmask_window(iw, old_width - (2 * old_border_width), 
                  old_height - (2 * old_border_width), old_border_width);

    border_name = bs_configuration_get_string_value(BS_FRAME_BORDER);
    iw->border = get_border_for_name(border_name);
    g_free(border_name);
    g_assert(iw->border != -1);

    if (bs_window_pixmap_use_common_frame_size(iw->src)) {
        width = bs_configuration_get_int_value(BS_FRAME_COMMON_SIZE);
        height = width;
    } else
        bs_window_pixmap_get_size(iw->src, &width, &height);

    g_assert(width > 0 && height > 0);
    border_width = get_border_width(iw->border);
    full_width = width + (2 * border_width);
    full_height = height + (2 * border_width);

    mask = bs_window_pixmap_get_mask(iw->src);
    bs_window_pixmap_get_size(iw->src, &pm_width, &pm_height);
    gdk_window_shape_combine_mask(
        iw->gdkwindow, mask, 
        border_width + ((width - pm_width) / 2),
        border_width + ((height - pm_height) / 2)
    );

    g_assert(large_topleft_mask != None);


    XResizeWindow(GDK_DISPLAY(), iw->window, full_width, full_height);
    XMoveWindow(GDK_DISPLAY(), iw->dst_window, border_width, border_width);

    attrs.background_pixel = get_bg_color();
    XChangeWindowAttributes(GDK_DISPLAY(), iw->window, 0 | CWBackPixel, &attrs);

    unmask_window(iw, width, height, border_width);

    if (mask) {
        gboolean show_frame;

        show_frame = bs_configuration_get_boolean_value(BS_FRAME_SHOW);
        if (!show_frame) {

            gdk_window_shape_combine_mask(
                iw->gdkwindow, mask, 
                border_width + ((width - pm_width) / 2),
                border_width + ((height - pm_height) / 2)
            );

            if (iw->border != BORDER_NONE) {
                XRectangle rects[4];

                rects[0].x = 0;
                rects[0].y = 0;
                rects[0].width = width + (2 * border_width);
                rects[0].height = border_width;
                rects[1].x = 0;
                rects[1].y = 0;
                rects[1].width = border_width;
                rects[1].height = height + (2 * border_width);
                rects[2].x = width + border_width;
                rects[2].y = 0;
                rects[2].width = border_width;
                rects[2].height = height + (2 * border_width);
                rects[3].x = 0;
                rects[3].y = height + border_width;
                rects[3].width = width + (2 * border_width);
                rects[3].height = border_width;

                XShapeCombineRectangles(GDK_DISPLAY(), iw->window,
                        ShapeBounding, 0, 0, rects, 4, 
                        ShapeUnion, 1);
            }
        }

        gdk_window_shape_combine_mask(
            iw->dst_gdkwindow, mask,
            ((width - pm_width) / 2),
            ((height - pm_height) / 2)
        );
    }
    shape_window(iw->window, width, height, iw->border);
    rect.x = 0;
    rect.y = 0;
    rect.width = width + (2 * border_width);
    rect.height = height + (2 * border_width);

    /* force the window to update immediates (need to figure out the right 
     * way to do this */
    XResizeWindow(GDK_DISPLAY(), iw->window, full_width + 1, full_height);
    XResizeWindow(GDK_DISPLAY(), iw->window, full_width, full_height);
    XResizeWindow(GDK_DISPLAY(), iw->dst_window, width + 1, height);
    XResizeWindow(GDK_DISPLAY(), iw->dst_window, width, height);
}


static void
unmask_window(BSIconWindow *iw, gint width, gint height, gint border_width)
{
    XRectangle rects[1];

    rects[0].x = 0;
    rects[0].y = 0;
    rects[0].width = width + (2 * border_width);
    rects[0].height = height + (2 * border_width);

    XShapeCombineRectangles(GDK_DISPLAY(), iw->window,
            ShapeBounding, 0, 0, rects, 1, 
            ShapeUnion, 1);

    rects[0].width = width;
    rects[0].height = height;

    XShapeCombineRectangles(GDK_DISPLAY(), iw->dst_window,
            ShapeBounding, 0, 0, rects, 1, 
            ShapeUnion, 1);
}


static void 
frame_color_cb(void *data)
{
    BSIconWindow *iw = (BSIconWindow *)data;
    if (bs_window_pixmap_is_ready(iw->src))
        redraw(iw);
}


static void
send_desktop_message(BSIconWindow *iw, gint desktop)
{
    XEvent xev;
    xev.xclient.type = ClientMessage;
    xev.xclient.serial = 0;
    xev.xclient.send_event = True;
    xev.xclient.window = iw->window;
    xev.xclient.display = GDK_DISPLAY();
    xev.xclient.message_type = NET_WM_DESKTOP;
    xev.xclient.format = 32;
    xev.xclient.data.l[0] = desktop;
    xev.xclient.data.l[1] = 0;
    xev.xclient.data.l[2] = 0;
    xev.xclient.data.l[3] = 0;
    xev.xclient.data.l[4] = 0;

    XSendEvent(GDK_DISPLAY(), GDK_ROOT_WINDOW(), False,
               SubstructureRedirectMask | SubstructureNotifyMask, &xev);
}


void
bs_icon_window_set_rightclick_action(BSIconWindow *iw, 
                                     void (*cb)(guint32, gpointer),
                                     gpointer data)
{
    iw->rightclick_action = cb;
    iw->rightclick_data = data;
}


void
bs_icon_window_set_moved_callback(BSIconWindow *iw, void (*cb)(void *),
                                  void *data)
{
    BSCallback *bscb = bs_callback_new(cb, data);
    iw->moved_callback = bscb;
}


void
bs_icon_window_get_gemoetry(BSIconWindow *iw, gint *x, gint *y,
                            gint *w, gint *h)
{
    bs_icon_window_get_size(iw, w, h);
    gdk_window_get_root_origin(iw->gdkwindow, x, y);
}


static void 
render_pixmap(BSIconWindow *iw, gint x, gint y, gint width, gint height)
{
    GdkRectangle xpose, pm, isect;
    gint pm_x, pm_y, pm_width, pm_height;
    gint win_width, win_height;
    gint src_x, src_y;

    xpose.x = x;
    xpose.y = y;
    xpose.width = width;
    xpose.height = height;

    bs_window_pixmap_get_size(iw->src, &pm.width, &pm.height);
    gdk_window_get_size(iw->dst_gdkwindow, &win_width, &win_height);

    pm.x = 0 - ((pm.width - win_width) / 2);
    pm.y = 0 - ((pm.height - win_height) / 2);

    gdk_rectangle_intersect(&xpose, &pm, &isect);

    src_x = isect.x + ((pm.width - win_width) / 2);
    src_y = isect.y + ((pm.width - win_width) / 2);

    XCopyArea(GDK_DISPLAY(),
              GDK_PIXMAP_XID(bs_window_pixmap_get_pixmap(iw->src)),
              iw->dst_window,
              DefaultGC(GDK_DISPLAY(), DefaultScreen(GDK_DISPLAY())),
              src_x, src_y,
              isect.width, isect.height,
              isect.x, isect.y);
}
