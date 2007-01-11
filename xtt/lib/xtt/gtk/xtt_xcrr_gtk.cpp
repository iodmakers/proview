/* 
 * Proview   $Id: xtt_xcrr_gtk.cpp,v 1.1 2007-01-04 08:29:32 claes Exp $
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
 */

/* xtt_xcrr_gtk.cpp -- Display object crossreferences */

#include "glow_std.h"

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>

#include "co_cdh.h"
#include "co_dcli.h"
#include "co_time.h"
#include "co_wow_gtk.h"
#include "rt_xnav_msg.h"
#include "flow.h"
#include "flow_browctx.h"
#include "flow_browapi.h"
#include "xtt_xcrr_gtk.h"
#include "xtt_xattnav_gtk.h"
#include "co_lng.h"
#include "xtt_xnav.h"
#include "rt_xatt_msg.h"


void XCrrGtk::activate_exit(GtkWidget *w, gpointer data)
{
  XCrr *xcrr = (XCrr *)data;

  if ( xcrr->close_cb)
    (xcrr->close_cb)( xcrr->parent_ctx, (void *)xcrr);
  else
    delete xcrr;
}

void XCrrGtk::activate_openplc(GtkWidget *w, gpointer data)
{
  XCrr *xcrr = (XCrr *)data;

  xcrr->xcrrnav->start_trace();
}

void XCrrGtk::activate_help(GtkWidget *w, gpointer data)
{
  // Not yet implemented
}

gboolean XCrrGtk::action_inputfocus( GtkWidget *w, GdkEvent *event, gpointer data)
{
  XCrrGtk *xcrr = (XCrrGtk *)data;

  if ( xcrr->focustimer.disabled()) {
    return TRUE;
  }
  if ( xcrr->xcrrnav)
    xcrr->xcrrnav->set_inputfocus();
  xcrr->focustimer.disable( 400);

  return FALSE;
}

void XCrrGtk::pop()
{
  gtk_window_present( GTK_WINDOW(toplevel));
}

XCrrGtk::~XCrrGtk()
{
  delete xcrrnav;
  gtk_widget_destroy( toplevel);
}

static gint delete_event( GtkWidget *w, GdkEvent *event, gpointer data)
{
  XCrrGtk *xcrr = (XCrrGtk *)data;

  if ( xcrr->close_cb)
    (xcrr->close_cb)( xcrr->parent_ctx, (void *)xcrr);
  else
    delete xcrr;
  
  return FALSE;
}

static void destroy_event( GtkWidget *w, gpointer data)
{
}

XCrrGtk::XCrrGtk( 
	GtkWidget      	*xa_parent_wid,
	void 		*xa_parent_ctx, 
	pwr_sAttrRef 	*xa_objar,
	int 		xa_advanced_user,
        int             *xa_sts) :
  XCrr( xa_parent_ctx, xa_objar, xa_advanced_user, xa_sts),
  parent_wid(xa_parent_wid)
{
  int sts;
  pwr_tAName   	title;

  *xa_sts = gdh_AttrrefToName( &objar, title, sizeof(title), cdh_mNName);
  if ( EVEN(*xa_sts)) return;

  toplevel = (GtkWidget *) g_object_new( GTK_TYPE_WINDOW, 
			   "default-height", 420,
			   "default-width", 300,
			   "title", title,
			   NULL);

  g_signal_connect( toplevel, "delete_event", G_CALLBACK(delete_event), this);
  g_signal_connect( toplevel, "destroy", G_CALLBACK(destroy_event), this);
  g_signal_connect( toplevel, "focus-in-event", G_CALLBACK(action_inputfocus), this);

  CoWowGtk::SetWindowIcon( toplevel);

  GtkWidget *vbox = gtk_vbox_new( FALSE, 0);

  // Menu
  // Accelerators
  GtkAccelGroup *accel_g = (GtkAccelGroup *) g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(toplevel), accel_g);

  GtkMenuBar *menu_bar = (GtkMenuBar *) g_object_new(GTK_TYPE_MENU_BAR, NULL);

  // File entry
  GtkWidget *file_close = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, accel_g);
  g_signal_connect(file_close, "activate", G_CALLBACK(activate_exit), this);

  GtkMenu *file_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_close);

  GtkWidget *file = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_File"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), GTK_WIDGET(file_menu));

  // Functions entry
  GtkWidget *functions_open_plc = gtk_menu_item_new_with_mnemonic( CoWowGtk::translate_utf8("Open _Program"));
  g_signal_connect( functions_open_plc, "activate", 
		    G_CALLBACK(activate_openplc), this);
  gtk_widget_add_accelerator( functions_open_plc, "activate", accel_g,
  			      'l', GdkModifierType(GDK_CONTROL_MASK), 
  			      GTK_ACCEL_VISIBLE);


  GtkMenu *func_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(func_menu), functions_open_plc);

  GtkWidget *functions = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_Functions"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), functions);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(functions), GTK_WIDGET(func_menu));

  // Help entry
  GtkWidget *help_help = gtk_image_menu_item_new_from_stock(GTK_STOCK_HELP, accel_g);
  g_signal_connect(help_help, "activate", G_CALLBACK(activate_help), this);

  GtkMenu *help_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_help);

  GtkWidget *help = gtk_menu_item_new_with_mnemonic(CoWowGtk::translate_utf8("_Help"));
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), GTK_WIDGET(help_menu));

  xcrrnav = new XAttNavGtk( (void *)this, vbox, xattnav_eType_CrossRef,
		"Plant", &objar, xa_advanced_user, &brow_widget, &sts);
  xcrrnav->popup_menu_cb = &xcrr_popup_menu_cb;
  xcrrnav->start_trace_cb = &xcrr_start_trace_cb;
  xcrrnav->close_cb = &xcrr_close_cb;

  gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(menu_bar), FALSE, FALSE, 0);
  gtk_box_pack_end( GTK_BOX(vbox), GTK_WIDGET(brow_widget), TRUE, TRUE, 0);
  
  gtk_container_add( GTK_CONTAINER(toplevel), vbox);
  gtk_widget_show_all( toplevel);

  *xa_sts = XATT__SUCCESS;
}

