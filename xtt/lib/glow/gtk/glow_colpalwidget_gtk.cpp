/* 
 * Proview   $Id: glow_colpalwidget_gtk.cpp,v 1.1 2007-01-04 08:07:43 claes Exp $
 * Copyright (C) 2005 SSAB Oxel�sund AB.
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as 
 * published by the Free Software Foundation, either version 2 of 
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with the program, if not, write to the Free Software 
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 **/

#include <stdlib.h>
#include "glow_std.h"

#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>
#include "glow_colpalwidget_gtk.h"
#include "glow.h"
#include "glow_ctx.h"
#include "glow_colpalctx.h"
#include "glow_draw.h"
#include "glow_draw_gtk.h"

typedef struct _ColPalWidgetGtk		ColPalWidgetGtk;
typedef struct _ColPalWidgetGtkClass	ColPalWidgetGtkClass;

typedef struct {
	GtkWidget  	*colpal;
	GtkWidget	*form;
	GtkWidget	*scroll_h;
	GtkWidget	*scroll_v;
	int		scroll_h_managed;
	int		scroll_v_managed;
	} colpalwidget_sScroll;

struct _ColPalWidgetGtk {
  GtkDrawingArea parent;

  /* Private */
  void		*colpal_ctx;
  void 		*draw_ctx;
  int		(*init_proc)(GlowCtx *ctx, void *clien_data);
  int		is_navigator;
  void		*client_data;
  GtkWidget	*main_colpal_widget;
  GtkWidget    	*scroll_h;
  GtkWidget    	*scroll_v;
  GtkWidget    	*form;
  int		scroll_h_ignore;
  int		scroll_v_ignore;
};

struct _ColPalWidgetGtkClass {
  GtkDrawingAreaClass parent_class;
};

G_DEFINE_TYPE( ColPalWidgetGtk, colpalwidgetgtk, GTK_TYPE_DRAWING_AREA);

static void scroll_callback( glow_sScroll *data)
{
  colpalwidget_sScroll *scroll_data;

  scroll_data = (colpalwidget_sScroll *) data->scroll_data;

  if ( data->total_width <= data->window_width) {
    if ( data->offset_x == 0)
      data->total_width = data->window_width;
    if ( scroll_data->scroll_h_managed) {
      // Remove horizontal scrollbar
    }
  }
  else {
    if ( !scroll_data->scroll_h_managed) {
      // Insert horizontal scrollbar
    }
  }

  if ( data->total_height <= data->window_height) {
    if ( data->offset_y == 0)
      data->total_height = data->window_height;
    if ( scroll_data->scroll_v_managed) {
      // Remove vertical scrollbar
    }
  }
  else {
    if ( !scroll_data->scroll_v_managed) {
      // Insert vertical scrollbar
    }
  }
  if ( data->offset_x < 0) {
    data->total_width += -data->offset_x;
    data->offset_x = 0;
  }
  if ( data->offset_y < 0) {
    data->total_height += -data->offset_y;
    data->offset_y = 0;
  }
  if ( data->total_height < data->window_height + data->offset_y)
    data->total_height = data->window_height + data->offset_y;
  if ( data->total_width < data->window_width + data->offset_x)
    data->total_width = data->window_width + data->offset_x;
  if ( data->window_width < 1)
    data->window_width = 1;
  if ( data->window_height < 1)
    data->window_height = 1;

  if ( scroll_data->scroll_h_managed) {
    ((ColPalWidgetGtk *)scroll_data->colpal)->scroll_h_ignore = 1;
    g_object_set( ((GtkScrollbar *)scroll_data->scroll_h)->range.adjustment,
		 "upper", (gdouble)data->total_width,
		 "page-size", (gdouble)data->window_width,
		 "value", (gdouble)data->offset_x,
		 NULL);
    gtk_adjustment_changed( 
        ((GtkScrollbar *)scroll_data->scroll_h)->range.adjustment);
  }

  if ( scroll_data->scroll_v_managed) {
    ((ColPalWidgetGtk *)scroll_data->colpal)->scroll_v_ignore = 1;
    g_object_set( ((GtkScrollbar *)scroll_data->scroll_v)->range.adjustment,
		 "upper", (gdouble)data->total_height,
		 "page-size", (gdouble)data->window_height,
		 "value", (gdouble)data->offset_y,
		 NULL);
    gtk_adjustment_changed( 
        ((GtkScrollbar *)scroll_data->scroll_v)->range.adjustment);
  }
}

static void scroll_h_action( 	GtkWidget      	*w,
				gpointer 	data)
{
  ColPalWidgetGtk *colpalw = (ColPalWidgetGtk *)data;
  if ( colpalw->scroll_h_ignore) {
    colpalw->scroll_h_ignore = 0;
    return;
  }

  printf( "Horizontal scroll callback\n");

  ColPalCtx *ctx = (ColPalCtx *) colpalw->colpal_ctx;
  gdouble value;
  g_object_get( w,
		"value", &value,
		NULL);
  glow_scroll_horizontal( ctx, int(value), 0);

}

static void scroll_v_action( 	GtkWidget 	*w,
				gpointer 	data)
{
  ColPalWidgetGtk *colpalw = (ColPalWidgetGtk *)data;

  if ( colpalw->scroll_v_ignore) {
    colpalw->scroll_v_ignore = 0;
    return;
  }
    
  printf( "Vertical scroll callback\n");

  ColPalCtx *ctx = (ColPalCtx *) colpalw->colpal_ctx;
  gdouble value;
  g_object_get( w,
		"value", &value,
		NULL);
  glow_scroll_vertical( ctx, int(value), 0);
}

static int colpal_init_proc( GtkWidget *w, GlowCtx *fctx, void *client_data)
{
  colpalwidget_sScroll *scroll_data;
  ColPalCtx	*ctx;

  ctx = (ColPalCtx *) ((ColPalWidgetGtk *) w)->colpal_ctx;

  if ( ((ColPalWidgetGtk *) w)->scroll_h) {
    scroll_data = (colpalwidget_sScroll *) malloc( sizeof( colpalwidget_sScroll));
    scroll_data->colpal = w;
    scroll_data->scroll_h = ((ColPalWidgetGtk *) w)->scroll_h;
    scroll_data->scroll_v = ((ColPalWidgetGtk *) w)->scroll_v;
    scroll_data->form = ((ColPalWidgetGtk *) w)->form;
    scroll_data->scroll_h_managed = 1;
    scroll_data->scroll_v_managed = 1;

    ctx->register_scroll_callback( (void *) scroll_data, scroll_callback);
  }

  ctx->configure();

  if ( ((ColPalWidgetGtk *) w)->init_proc)
    return (((ColPalWidgetGtk *) w)->init_proc)( ctx, client_data);
  else
    return 1;
}

static gboolean colpalwidgetgtk_expose( GtkWidget *glow, GdkEventExpose *event)
{
  ((GlowDrawGtk *)((ColPalCtx *)((ColPalWidgetGtk *)glow)->colpal_ctx)->gdraw)->event_handler( 
					*(GdkEvent *)event);
  return TRUE;
}

static gboolean colpalwidgetgtk_event( GtkWidget *glow, GdkEvent *event)
{
  if ( event->type == GDK_MOTION_NOTIFY) {
    GdkEvent *next = gdk_event_peek();
    if ( next && next->type == GDK_MOTION_NOTIFY) {
      gdk_event_free( next);
      return TRUE;
    }
    else if ( next)
      gdk_event_free( next);
  }

  ((GlowDrawGtk *)((ColPalCtx *)((ColPalWidgetGtk *)glow)->colpal_ctx)->gdraw)->event_handler( *event);
  return TRUE;
}

static void colpalwidgetgtk_realize( GtkWidget *widget)
{
  GdkWindowAttr attr;
  gint attr_mask;
  ColPalWidgetGtk *colpal;

  g_return_if_fail (widget != NULL);
  g_return_if_fail (IS_COLPALWIDGETGTK( widget));

  GTK_WIDGET_SET_FLAGS( widget, GTK_REALIZED);
  colpal = COLPALWIDGETGTK( widget);

  attr.x = widget->allocation.x;
  attr.y = widget->allocation.y;
  attr.width = widget->allocation.width;
  attr.height = widget->allocation.height;
  attr.wclass = GDK_INPUT_OUTPUT;
  attr.window_type = GDK_WINDOW_CHILD;
  attr.event_mask = gtk_widget_get_events( widget) |
    GDK_EXPOSURE_MASK | 
    GDK_BUTTON_PRESS_MASK | 
    GDK_BUTTON_RELEASE_MASK | 
    GDK_KEY_PRESS_MASK |
    GDK_POINTER_MOTION_MASK |
    GDK_BUTTON_MOTION_MASK |
    GDK_ENTER_NOTIFY_MASK |
    GDK_LEAVE_NOTIFY_MASK;
  attr.visual = gtk_widget_get_visual( widget);
  attr.colormap = gtk_widget_get_colormap( widget);

  attr_mask = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;
  widget->window = gdk_window_new( widget->parent->window, &attr, attr_mask);
  widget->style = gtk_style_attach( widget->style, widget->window);
  gdk_window_set_user_data( widget->window, widget);
  gtk_style_set_background( widget->style, widget->window, GTK_STATE_ACTIVE);

  GTK_WIDGET_SET_FLAGS( widget, GTK_CAN_FOCUS);

  if ( colpal->is_navigator) {
    if ( !colpal->colpal_ctx) {
      ColPalWidgetGtk *main_colpal = (ColPalWidgetGtk *) colpal->main_colpal_widget;

      colpal->colpal_ctx = main_colpal->colpal_ctx;
      colpal->draw_ctx = main_colpal->draw_ctx;
      ((GlowDrawGtk *)colpal->draw_ctx)->init_nav( widget);
    }
  }
  else {
    if ( !colpal->colpal_ctx) {
      colpal->draw_ctx = new GlowDrawGtk( widget, 
					&colpal->colpal_ctx, 
					colpal_init_proc, 
					colpal->client_data,
					glow_eCtxType_ColPal);
    }
  }

}

static void colpalwidgetgtk_class_init( ColPalWidgetGtkClass *klass)
{
  GtkWidgetClass *widget_class;
  widget_class = GTK_WIDGET_CLASS( klass);
  widget_class->realize = colpalwidgetgtk_realize;
  widget_class->expose_event = colpalwidgetgtk_expose;
  widget_class->event = colpalwidgetgtk_event;
}

static void colpalwidgetgtk_init( ColPalWidgetGtk *glow)
{
}

GtkWidget *colpalwidgetgtk_new(
        int (*init_proc)(GlowCtx *ctx, void *client_data),
	void *client_data)
{
  ColPalWidgetGtk *w;
  w =  (ColPalWidgetGtk *) g_object_new( COLPALWIDGETGTK_TYPE, NULL);
  w->init_proc = init_proc;
  w->colpal_ctx = 0;
  w->is_navigator = 0;
  w->client_data = client_data;
  w->scroll_h = 0;
  w->scroll_v = 0;
  return (GtkWidget *) w;  
}

GtkWidget *scrolledcolpalwidgetgtk_new(
        int (*init_proc)(GlowCtx *ctx, void *client_data),
	void *client_data, GtkWidget **colpalwidget)
{
  ColPalWidgetGtk *w;

  GtkWidget *form = gtk_scrolled_window_new( NULL, NULL);

  w =  (ColPalWidgetGtk *) g_object_new( COLPALWIDGETGTK_TYPE, NULL);
  w->init_proc = init_proc;
  w->colpal_ctx = 0;
  w->is_navigator = 0;
  w->client_data = client_data;
  w->scroll_h = gtk_scrolled_window_get_hscrollbar( GTK_SCROLLED_WINDOW(form));
  w->scroll_v = gtk_scrolled_window_get_vscrollbar( GTK_SCROLLED_WINDOW(form));
  w->scroll_h_ignore = 0;
  w->scroll_v_ignore = 0;
  w->form = form;
  *colpalwidget = GTK_WIDGET( w);

  g_signal_connect( ((GtkScrollbar *)w->scroll_h)->range.adjustment, 
		    "value-changed", G_CALLBACK(scroll_h_action), w);
  g_signal_connect( ((GtkScrollbar *)w->scroll_v)->range.adjustment, 
		    "value-changed", G_CALLBACK(scroll_v_action), w);

  GtkWidget *viewport = gtk_viewport_new( NULL, NULL);
  gtk_container_add( GTK_CONTAINER(viewport), GTK_WIDGET(w));
  gtk_container_add( GTK_CONTAINER(form), GTK_WIDGET(viewport));

  return (GtkWidget *) form;  
}

GtkWidget *colpalnavwidgetgtk_new( GtkWidget *main_colpal)
{
  ColPalWidgetGtk *w;
  w =  (ColPalWidgetGtk *) g_object_new( COLPALWIDGETGTK_TYPE, NULL);
  w->init_proc = 0;
  w->colpal_ctx = 0;
  w->is_navigator = 1;
  w->main_colpal_widget = main_colpal;
  w->client_data = 0;
  w->scroll_h = 0;
  w->scroll_v = 0;
  w->scroll_h_ignore = 0;
  w->scroll_v_ignore = 0;
  return (GtkWidget *) w;  
}

#if 0
GType colpalwidgetgtk_get_type(void)
{
  static GType colpalwidgetgtk_type = 0;

  if ( !colpalwidgetgtk_type) {
    static const GTypeInfo colpalwidgetgtk_info = {
      sizeof(ColPalWidgetGtkClass), NULL, NULL, (GClassInitFunc)colpalwidgetgtk_class_init,
      NULL, NULL, sizeof(ColPalWidgetGtk), 1, NULL, NULL};
    
    colpalwidgetgtk_type = g_type_register_static( G_TYPE_OBJECT, "ColPalWidgetGtk", &colpalwidgetgtk_info, 
					   (GTypeFlags)0);  
  }
  return colpalwidgetgtk_type;
}
#endif