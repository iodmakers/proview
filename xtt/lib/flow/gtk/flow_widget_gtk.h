/* 
 * Proview   $Id: flow_widget_gtk.h,v 1.1 2007-01-04 07:56:44 claes Exp $
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

#ifndef flow_widget_gtk_h
#define flow_widget_gtk_h

#include <stdlib.h>
#include "glow_std.h"

#include "flow.h"
#include "flow_ctx.h"
#include "flow_ctx.h"

#include <gtk/gtk.h>
#include <gtk/gtkprivate.h>

#if defined __cplusplus
extern "C" {
#endif

#define FLOWWIDGETGTK_TYPE (flowwidgetgtk_get_type())
#define FLOWWIDGETGTK(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), FLOWWIDGETGTK_TYPE, FlowWidgetGtk))
#define FLOWWIDGETGTK_CLASS(obj) (G_TYPE_CHECK_CLASS_CAST((obj), FLOWWIDGETGTK, FlowWidgetGtkClass))
#define IS_FLOWWIDGETGTK(obj) (G_TYPE_CHECK_INSTANCE_TYPE((obj), FLOWWIDGETGTK_TYPE))
#define IS_FLOWWIDGETGTK_CLASS(obj) (G_TYPE_CHECK_CLASS_TYPE((obj), FLOWWIDGETGTK_TYPE))
#define FLOWWIDGETGTK_GET_CLASS (G_TYPE_INSTANCE_GET_CLASS((obj), FLOWWIDGETGTK, FlowWidgetGtkClass))

GType flowwidgetgtk_get_type(void);

GtkWidget *flowwidgetgtk_new(
        int (*init_proc)(FlowCtx *ctx, void *client_data),
	void *client_data);
GtkWidget *scrolledflowwidgetgtk_new(
        int (*init_proc)(FlowCtx *ctx, void *client_data),
	void *client_data, GtkWidget **flowwidget);

GtkWidget *flownavwidgetgtk_new( GtkWidget *main_flow);

void flowwidgetgtk_get_ctx( GtkWidget *w, void **ctx);
void flowwidgetgtk_modify_ctx( GtkWidget *w, void *ctx);

#if defined __cplusplus
}
#endif
#endif