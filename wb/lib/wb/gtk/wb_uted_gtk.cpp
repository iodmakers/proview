/* 
 * Proview   $Id: wb_uted_gtk.cpp,v 1.1 2007-01-04 07:29:02 claes Exp $
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

/* wb_uted_gtk.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>

#include <gtk/gtk.h>

#include "flow_ctx.h"
#include "co_time.h"
#include "co_msg.h"
#include "co_dcli.h"
#include "co_xhelp.h"
#include "co_wow_gtk.h"
#include "wb_ldh.h"
#include "wb_foe.h"
#include "wb_foe_msg.h"
#include "wb_vldh_msg.h"
#include "wb_cmd_msg.h"
#include "wb_vldh.h"
#include "wb_uted_gtk.h"

#define	BEEP	    putchar( '\7' );

#define UTED_INDEX_NOCOMMAND -1
#define UTED_TEXT_NOCOMMAND "No command selected"

void WUtedGtk::clock_cursor()
{
  if ( !cursor)
    cursor = gdk_cursor_new_for_display( gtk_widget_get_display( toplevel),
					       GDK_WATCH);

  gdk_window_set_cursor( toplevel->window, cursor);
  gdk_display_flush( gtk_widget_get_display( toplevel));
}

void WUtedGtk::reset_cursor()
{
  gdk_window_set_cursor( toplevel->window, NULL);
}

void WUtedGtk::remove_command_window()
{
}


void WUtedGtk::activate_command( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;
  char		label[40];

  uted->message( "");

  for ( int j = 0; ((WUtedGtk *)uted)->widgets.optmenubuttons[j]; j++) {
    if ( w == ((WUtedGtk *)uted)->widgets.optmenubuttons[j]) {
      strncpy( label, gtk_label_get_label( 
               GTK_LABEL(((WUtedGtk *)uted)->widgets.optmenubuttonslabel[j])), 
	       sizeof(label));
      uted->configure_quals( label);
    }
  }
}

void WUtedGtk::activate_helputils( GtkWidget *w, gpointer data)
{
  CoXHelp::dhelp( "utilities_refman", 0, navh_eHelpFile_Other, "$pwr_lang/man_dg.dat", true);
}

void WUtedGtk::activate_helppwr_plc( GtkWidget *w, gpointer data)
{
  CoXHelp::dhelp( "help command", 0, navh_eHelpFile_Other, "$pwr_lang/man_dg.dat", true);
}

void WUtedGtk::activate_batch( GtkWidget *w, gpointer data)
{
}

void WUtedGtk::activate_currsess( GtkWidget *w, gpointer data)
{
}

//
//	Callback from the menu.
//
void WUtedGtk::activate_quit( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  delete uted;
}

void WUtedGtk::activate_ok( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->clock_cursor();
  uted->execute(0);
  uted->reset_cursor();
}

void WUtedGtk::activate_cancel( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  delete uted;
}

void WUtedGtk::activate_show_cmd( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->clock_cursor();
  uted->execute( 1);
  uted->reset_cursor();
}

void WUtedGtk::activate_cmd_wind( GtkWidget *w, gpointer data)
{
}

void WUtedGtk::commandchanged( GtkWidget *w, gpointer data)
{
}

void WUtedGtk::activate_present1( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[0] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present2( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[1] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present3( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[2] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present4( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[3] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present5( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[4] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present6( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[5] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present7( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[6] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present8( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[7] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present9( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[8] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}
void WUtedGtk::activate_present10( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  uted->message( "");
  uted->present_sts[9] = (int) gtk_toggle_button_get_active( GTK_TOGGLE_BUTTON(w));
}

//
//   Execute the yes procedure for the questionbox.
//
void WUtedGtk::qbox_yes_cb( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  g_object_set( ((WUtedGtk *)uted)->widgets.questionbox, "visible", FALSE, NULL);

  if ( uted->questionbox_yes != NULL)
    (uted->questionbox_yes) ( uted);
}

//
//   Execute the no procedure for the questionbox.
//
void WUtedGtk::qbox_no_cb( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  g_object_set( ((WUtedGtk *)uted)->widgets.questionbox, "visible", FALSE, NULL);

  if ( uted->questionbox_no != NULL)
    (uted->questionbox_no) ( uted);
}

//
//   Unmanage the questionbox.
//
void WUtedGtk::qbox_cancel_cb( GtkWidget *w, gpointer data)
{
  WUted *uted = (WUted *)data;

  if ( uted->questionbox_cancel != NULL)
    (uted->questionbox_cancel) ( uted);
  g_object_set( ((WUtedGtk *)uted)->widgets.questionbox, "visible", FALSE, NULL);
}

static gint delete_event( GtkWidget *w, GdkEvent *event, gpointer uted)
{
  WUtedGtk::activate_quit(w, uted);

  return TRUE;
}

static void destroy_event( GtkWidget *w, gpointer data)
{
}

//
//	Create a new ute window
//
WUtedGtk::WUtedGtk( void	       	*wu_parent_ctx,
		    GtkWidget		*wu_parent_wid,
		    char	       	*wu_name,
		    char	       	*wu_iconname,
		    ldh_tWBContext	wu_ldhwb,
		    ldh_tSesContext	wu_ldhses,
		    int	       	wu_editmode,
		    void 	       	(*wu_quit_cb)(void *),
		    pwr_tStatus     	*status) :
  WUted(wu_parent_ctx,wu_name,wu_iconname,wu_ldhwb,wu_ldhses,wu_editmode,wu_quit_cb,
	status), parent_wid(wu_parent_wid), cursor(0)
{
  const int	window_width = 800;
  const int    	window_height = 400;

  memset( &widgets, 0, sizeof(widgets));

  toplevel = (GtkWidget *) g_object_new( GTK_TYPE_WINDOW, 
			   "default-height", window_height,
			   "default-width", window_width,
			   NULL);

  g_signal_connect( toplevel, "delete_event", G_CALLBACK(delete_event), this);
  g_signal_connect( toplevel, "destroy", G_CALLBACK(destroy_event), this);

  CoWowGtk::SetWindowIcon( toplevel);

  GtkAccelGroup *accel_g = (GtkAccelGroup *) g_object_new(GTK_TYPE_ACCEL_GROUP, NULL);
  gtk_window_add_accel_group(GTK_WINDOW(toplevel), accel_g);

  GtkMenuBar *menu_bar = (GtkMenuBar *) g_object_new(GTK_TYPE_MENU_BAR, NULL);

  // File Entry
  GtkWidget *file_close = gtk_image_menu_item_new_from_stock(GTK_STOCK_CLOSE, accel_g);
  g_signal_connect(file_close, "activate", G_CALLBACK(activate_quit), this);

  GtkMenu *file_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), file_close);

  GtkWidget *file = gtk_menu_item_new_with_mnemonic("_File");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), file);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(file), GTK_WIDGET(file_menu));

  // View Entry
  GtkWidget *view_show_cmd = gtk_menu_item_new_with_mnemonic( "_Show Current Command");
  g_signal_connect( view_show_cmd, "activate", 
		    G_CALLBACK(activate_show_cmd), this);

  GtkMenu *view_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), view_show_cmd);

  GtkWidget *view = gtk_menu_item_new_with_mnemonic("_View");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), view);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(view), GTK_WIDGET(view_menu));

  // Commands entry
  GtkMenu *commands_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  GtkMenu *current_menu = commands_menu;
  
  int j = 0;
  int in_submenu = 0;
  uted_sCommand *next_cmd;
  for ( uted_sCommand *command_ptr = commands; command_ptr->command[0]; command_ptr++) {

    // Build submenu if first command verb is equal to next
    char cmd_verb[5][80];
    int num;
    char next_cmd_verb[5][80];
    int next_num;
    num = dcli_parse( command_ptr->command, " ", "", (char *)cmd_verb, 
		      sizeof( cmd_verb) / sizeof( cmd_verb[0]), sizeof( cmd_verb[0]), 0);

    
    next_cmd = command_ptr + 1;
    int close_submenu = 0;
    if ( next_cmd->command[0]) {
      next_num = dcli_parse( next_cmd->command, " ", "", (char *)next_cmd_verb, 
			     sizeof( next_cmd_verb) / sizeof( next_cmd_verb[0]), 
			     sizeof( next_cmd_verb[0]), 0);

      if ( in_submenu) {
	// Check if time to close menu
	if ( strcmp( cmd_verb[0], next_cmd_verb[0]) != 0) {
	  // Close submenu
	  close_submenu = 1;
	}
      }
      else if ( strcmp( cmd_verb[0], next_cmd_verb[0]) == 0) {
	// Open submenu
	GtkWidget *submenu_item = gtk_menu_item_new_with_label( cmd_verb[0]);
	GtkMenu *submenu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(submenu_item),
				  GTK_WIDGET(submenu));
	gtk_menu_shell_append(GTK_MENU_SHELL(commands_menu), submenu_item);
	current_menu = submenu;
	in_submenu = 1;
      }
    }
    else if ( in_submenu) {
      // Last command
      close_submenu = 1;
    }


    widgets.optmenubuttons[j] = gtk_menu_item_new();
    widgets.optmenubuttonslabel[j] = gtk_label_new( command_ptr->command);
    gtk_misc_set_alignment( GTK_MISC(widgets.optmenubuttonslabel[j]), 0.0, 0.5);
    gtk_container_add( GTK_CONTAINER(widgets.optmenubuttons[j]), 
		       widgets.optmenubuttonslabel[j]);
    
    g_signal_connect( widgets.optmenubuttons[j], "activate", 
		      G_CALLBACK(activate_command), this);
    gtk_menu_shell_append(GTK_MENU_SHELL(current_menu), widgets.optmenubuttons[j]);

    if ( close_submenu) {
      current_menu = commands_menu;
      in_submenu = 0;
    }

    j++;
  }

  GtkWidget *commands = gtk_menu_item_new_with_mnemonic("_Commands");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), commands);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(commands), GTK_WIDGET(commands_menu));

  // Menu Help
  GtkWidget *help_utils = gtk_image_menu_item_new_with_mnemonic("_On Window");
  gtk_image_menu_item_set_image( GTK_IMAGE_MENU_ITEM(help_utils), 
				 gtk_image_new_from_stock( "gtk-help", GTK_ICON_SIZE_MENU));
  g_signal_connect(help_utils, "activate", G_CALLBACK(activate_helputils), this);
  gtk_widget_add_accelerator( help_utils, "activate", accel_g,
			      'h', GDK_CONTROL_MASK, GTK_ACCEL_VISIBLE);

  GtkWidget *help_cmd = gtk_menu_item_new_with_mnemonic( "On _Commands");
  g_signal_connect( help_cmd, "activate", 
		    G_CALLBACK(activate_helppwr_plc), this);

  GtkMenu *help_menu = (GtkMenu *) g_object_new( GTK_TYPE_MENU, NULL);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_utils);
  gtk_menu_shell_append(GTK_MENU_SHELL(help_menu), help_cmd);

  GtkWidget *help = gtk_menu_item_new_with_mnemonic("_Help");
  gtk_menu_shell_append(GTK_MENU_SHELL(menu_bar), help);
  gtk_menu_item_set_submenu(GTK_MENU_ITEM(help), GTK_WIDGET(help_menu));

  // Qualifier labels, entries and checkboxes
  GtkWidget *qual_box_qual = gtk_vbox_new( FALSE, 0);
  GtkWidget *qual_box_value = gtk_vbox_new( FALSE, 0);
  for ( j = 0; j < 10; j++) {
    widgets.qualifier[j] = gtk_label_new( "Qual");
    gtk_widget_set_size_request( widgets.qualifier[j], 120, 20);
    gtk_misc_set_alignment( GTK_MISC(widgets.qualifier[j]), 0.0, 0.5);
    gtk_box_pack_start( GTK_BOX(qual_box_qual), widgets.qualifier[j], FALSE, FALSE, 5);
    GtkWidget *box = gtk_hbox_new( FALSE, 0);
    widgets.value[j] = gtk_entry_new();
    gtk_widget_set_size_request( widgets.value[j], -1, 20);
    gtk_box_pack_start( GTK_BOX(box), widgets.value[j], TRUE, TRUE, 0);
    widgets.present[j] = gtk_check_button_new();
    gtk_widget_set_size_request( widgets.present[j], -1, 20);
    gtk_box_pack_start( GTK_BOX(box), widgets.present[j], FALSE, FALSE, 0);
    gtk_box_pack_start( GTK_BOX(qual_box_value), box, FALSE, FALSE, 5);
  }

  g_signal_connect( widgets.present[0], "toggled", G_CALLBACK(activate_present1), this);
  g_signal_connect( widgets.present[1], "toggled", G_CALLBACK(activate_present2), this);
  g_signal_connect( widgets.present[2], "toggled", G_CALLBACK(activate_present3), this);
  g_signal_connect( widgets.present[3], "toggled", G_CALLBACK(activate_present4), this);
  g_signal_connect( widgets.present[4], "toggled", G_CALLBACK(activate_present5), this);
  g_signal_connect( widgets.present[5], "toggled", G_CALLBACK(activate_present6), this);
  g_signal_connect( widgets.present[6], "toggled", G_CALLBACK(activate_present7), this);
  g_signal_connect( widgets.present[7], "toggled", G_CALLBACK(activate_present8), this);
  g_signal_connect( widgets.present[8], "toggled", G_CALLBACK(activate_present9), this);
  g_signal_connect( widgets.present[9], "toggled", G_CALLBACK(activate_present10), this);

  // Command label
  GtkWidget *cmdlabel = gtk_label_new( "Command");
  widgets.commandlabel = gtk_label_new( "No command selected");
  gtk_misc_set_alignment( GTK_MISC(widgets.commandlabel), 0.0, 0.5);
  gtk_widget_set_size_request( widgets.commandlabel, 140, -1);

  GtkWidget *vbox_command = gtk_hbox_new( FALSE, 0);
  gtk_box_pack_start( GTK_BOX(vbox_command), cmdlabel, FALSE, FALSE, 10);
  gtk_box_pack_start( GTK_BOX(vbox_command), widgets.commandlabel, FALSE, FALSE, 10);

  // Apply button
  GtkWidget *applybutton = gtk_button_new_with_label( "Apply");
  gtk_widget_set_size_request( applybutton, 70, 25);
  g_signal_connect( applybutton, "clicked", 
  		    G_CALLBACK(activate_ok), this);

  GtkWidget *left_box = gtk_vbox_new( FALSE, 0);
  gtk_box_pack_start( GTK_BOX(left_box), vbox_command, FALSE, FALSE, 10);

  GtkWidget *button_box = gtk_hbox_new( FALSE, 0);
  gtk_box_pack_start( GTK_BOX(button_box), applybutton, FALSE, FALSE, 50);

  GtkWidget *qual_box = gtk_hbox_new( FALSE, 0);
  gtk_box_pack_start( GTK_BOX(qual_box), left_box, FALSE, FALSE, 10);
  gtk_box_pack_start( GTK_BOX(qual_box), gtk_vseparator_new(), FALSE, FALSE, 0);
  gtk_box_pack_start( GTK_BOX(qual_box), qual_box_qual, FALSE, FALSE, 10);
  gtk_box_pack_start( GTK_BOX(qual_box), qual_box_value, TRUE, TRUE, 10);


  // Statusbar
  GtkWidget *statusbar = gtk_hbox_new( FALSE, 0);
  widgets.label = gtk_label_new( "");
  gtk_box_pack_start( GTK_BOX(statusbar), widgets.label, FALSE, FALSE, 20);


  GtkWidget *vbox = gtk_vbox_new( FALSE, 0);
  gtk_box_pack_start( GTK_BOX(vbox), GTK_WIDGET(menu_bar), FALSE, FALSE, 0);
  gtk_box_pack_start( GTK_BOX(vbox), qual_box, TRUE, TRUE, 0);
  gtk_box_pack_start( GTK_BOX(vbox), gtk_hseparator_new(), FALSE, FALSE, 3);
  gtk_box_pack_start( GTK_BOX(vbox), button_box, FALSE, FALSE, 3);
  gtk_box_pack_start( GTK_BOX(vbox), statusbar, FALSE, FALSE, 3);

  gtk_container_add( GTK_CONTAINER(toplevel), vbox);

  gtk_widget_show_all( toplevel);

  batch_sts = UTED_BATCH_CURRSESS;
  reset_qual();
  enable_entries( wu_editmode);

#if 0
  Arg		args[20];
  pwr_tStatus	sts;
  int i;
  MrmHierarchy	s_MrmH;
  MrmType	dclass;

  /* DRM database hierarchy related variables */
  static Pixmap icon = 0;
  Dimension	icon_w;
  Dimension	icon_h;

  static Atom	MwmDeleteWind = 0;
  char		uid_filename[200] = {"pwr_exe:wb_uted.uid"};
  char		*uid_filename_p = uid_filename;

  static MrmRegisterArg	reglist[] = {
    /* First the context variable */
    { "uted_ctx", 0 },

    /* Callbacks for the controlled foe widget */
    {"uted_create_adb",(caddr_t)create_adb},
    {"uted_create_quit",(caddr_t)create_quit},
    {"uted_activate_quit",(caddr_t)activate_quit},
    {"uted_create_commandlabel",(caddr_t)create_commandlabel},
    {"uted_create_batchoptmenu",(caddr_t)create_batchoptmenu},
    {"uted_create_batch",(caddr_t)create_batch},
    {"uted_create_currsess",(caddr_t)create_currsess},
    {"uted_create_file_entry",(caddr_t)create_file_entry},
    {"uted_create_label",(caddr_t)create_label},
    {"uted_create_adb",(caddr_t)create_adb},
    {"uted_activate_command",(caddr_t)activate_command},
    {"uted_create_command",(caddr_t)create_command},
    {"uted_activate_batch",(caddr_t)activate_batch},
    {"uted_activate_currsess",(caddr_t)activate_currsess},
    {"uted_activate_ok",(caddr_t)activate_ok},
    {"uted_activate_cancel",(caddr_t)activate_cancel},
    {"uted_activate_show_cmd",(caddr_t)activate_show_cmd},
    {"uted_create_commandwind_button",(caddr_t)create_commandwind_button},
    {"uted_activate_cmd_wind",(caddr_t)activate_cmd_wind},
    {"uted_create_cmd_wind",(caddr_t)create_cmd_wind},
    {"uted_create_timelabel",(caddr_t)create_timelabel},
    {"uted_create_timevalue",(caddr_t)create_timevalue},
    {"uted_create_qualifier1",(caddr_t)create_qualifier1},
    {"uted_create_value1",(caddr_t)create_value1},
    {"uted_activate_present1",(caddr_t)activate_present1},
    {"uted_create_present1",(caddr_t)create_present1},
    {"uted_create_qualifier2",(caddr_t)create_qualifier2},
    {"uted_create_value2",(caddr_t)create_value2},
    {"uted_activate_present2",(caddr_t)activate_present2},
    {"uted_create_present2",(caddr_t)create_present2},
    {"uted_create_qualifier3",(caddr_t)create_qualifier3},
    {"uted_create_value3",(caddr_t)create_value3},
    {"uted_activate_present3",(caddr_t)activate_present3},
    {"uted_create_present3",(caddr_t)create_present3},
    {"uted_create_qualifier4",(caddr_t)create_qualifier4},
    {"uted_create_value4",(caddr_t)create_value4},
    {"uted_activate_present4",(caddr_t)activate_present4},
    {"uted_create_present4",(caddr_t)create_present4},
    {"uted_create_qualifier5",(caddr_t)create_qualifier5},
    {"uted_create_value5",(caddr_t)create_value5},
    {"uted_activate_present5",(caddr_t)activate_present5},
    {"uted_create_present5",(caddr_t)create_present5},
    {"uted_create_qualifier6",(caddr_t)create_qualifier6},
    {"uted_create_value6",(caddr_t)create_value6},
    {"uted_activate_present6",(caddr_t)activate_present6},
    {"uted_create_present6",(caddr_t)create_present6},
    {"uted_create_qualifier7",(caddr_t)create_qualifier7},
    {"uted_create_value7",(caddr_t)create_value7},
    {"uted_activate_present7",(caddr_t)activate_present7},
    {"uted_create_present7",(caddr_t)create_present7},
    {"uted_create_qualifier8",(caddr_t)create_qualifier8},
    {"uted_create_value8",(caddr_t)create_value8},
    {"uted_activate_present8",(caddr_t)activate_present8},
    {"uted_create_present8",(caddr_t)create_present8},
    {"uted_create_qualifier9",(caddr_t)create_qualifier9},
    {"uted_create_value9",(caddr_t)create_value9},
    {"uted_activate_present9",(caddr_t)activate_present9},
    {"uted_create_present9",(caddr_t)create_present9},
    {"uted_create_qualifier10",(caddr_t)create_qualifier10},
    {"uted_create_value10",(caddr_t)create_value10},
    {"uted_activate_present10",(caddr_t)activate_present10},
    {"uted_create_present10",(caddr_t)create_present10},
    {"uted_activate_helputils",(caddr_t)activate_helputils},
    {"uted_activate_helppwr_plc",(caddr_t)activate_helppwr_plc},
    {"uted_commandchanged",(caddr_t)commandchanged},
    {"uted_qbox_cr",(caddr_t)qbox_cr},
    {"uted_qbox_yes_cb",(caddr_t)qbox_yes_cb},
    {"uted_qbox_no_cb",(caddr_t)qbox_no_cb},
    {"uted_qbox_cancel_cb",(caddr_t)qbox_cancel_cb}
  };

  static int	reglist_num = (sizeof reglist / sizeof reglist[0]);

  /*
   * Now start the module creation
   */

  sts = dcli_translate_filename( uid_filename, uid_filename);
  if ( EVEN(sts)) {
    printf( "** pwr_exe is not defined\n");
    exit(0);
  }

  /* set initialization values in context */

  /* Save the context structure in the widget */
  XtSetArg (args[0], XmNuserData, (unsigned int) this);

  /*
   * Create a new widget
   * Open the UID files (the output of the UIL compiler) in the hierarchy
   * Register the items DRM needs to bind for us.
   * Create a new widget
   * Close the hierarchy
   * Compile the additional button translations and augment and add actions
   */ 
  sts = MrmOpenHierarchy( 1, &uid_filename_p, NULL, &s_MrmH);
  if (sts != MrmSUCCESS) printf("can't open hierarchy\n");

  reglist[0].value = (caddr_t) this;

  MrmRegisterNames(reglist, reglist_num);

  if (icon == 0)
    sts = MrmFetchBitmapLiteral(s_MrmH,"icon", 
	      XtScreen(parent_wid), XtDisplay(parent_wid),
	      &icon, &icon_w, &icon_h);

  i=0;
  XtSetArg(args[i],XmNiconName, wu_iconname);  i++;
  XtSetArg(args[i],XmNiconPixmap, icon);  i++;
  XtSetArg(args[i],XmNtitle, name);  i++;
  
  /* SG 11.02.91 Save the id of the top in the context */ 
  parent_wid = XtCreatePopupShell("utilities", 
			       topLevelShellGtkWidgetClass, parent_wid, args, i);

  /* the positioning of a top level can only be define after the creation
      of the widget . So i do it know: 
     SG 24.02.91 use the parameters received x and y 
  */
  i=0;
  XtSetArg(args[i],XmNx,100);i++;
  XtSetArg(args[i],XmNy,100);i++;
  XtSetArg(args[i],XtNallowShellResize,TRUE), i++;
  XtSetValues( parent_wid, args, i);

  /* now that we have a top level we can get the main window */
  sts = MrmFetchGtkWidgetOverride(s_MrmH, "uted_window", parent_wid ,
		name, args, 1,
		&widgets.uted_window, &dclass);
  if (sts != MrmSUCCESS) printf("can't fetch utedit widget\n");

  XtManageChild( widgets.uted_window);

  /* SG 09.02.91 a top level should always be realized ! */
  XtPopup( parent_wid, XtGrabNone );

  /* Ask MRM to fetch the question box */
  i=0;
  if (MrmFetchGtkWidgetOverride(s_MrmH,
			"uted_qbox", 
	     	        widgets.uted_window,
			0,
			args , i,
			&widgets.questionbox, &dclass) != MrmSUCCESS) {
    printf("can't fetch uted_qbox widget\n");
  }

  MrmCloseHierarchy(s_MrmH);

  reset_qual();

  batch_sts = UTED_BATCH_CURRSESS;
  XtSetArg(args[0],XmNsensitive, 0);
  XtSetValues( widgets.timelabel,args,1);
  XtUnmanageChild( widgets.timevalue);
  XtUnmanageChild( widgets.command_window);
  enable_entries( wu_editmode);


  if (MwmDeleteWind == 0)
     MwmDeleteWind = XInternAtom(XtDisplay(parent_wid), "WM_DELETE_WINDOW", FALSE);

  if (MwmDeleteWind != 0)
  {
    XtVaSetValues( parent_wid, XmNdeleteResponse, XmDO_NOTHING, NULL);
    XmAddWMProtocolCallback( parent_wid, MwmDeleteWind, 
       (XtCallbackProc) activate_quit, this);
  }
#endif
  *status = FOE__SUCCESS;
}

//
//	Destroys a ute instance.
//      Destroys the widget and frees allocated memory for the 
//	context.
//
WUtedGtk::~WUtedGtk()
{
  if ( quit_cb)
    (quit_cb)( parent_ctx); 

  /* Destroy the widget */
  gtk_widget_destroy( toplevel);
}

//
// Get value from textentry with specified index
//
void WUtedGtk::get_value( int idx, char *str, int len)
{
  char *value;

  value = gtk_editable_get_chars( GTK_EDITABLE(widgets.value[idx]), 0, -1);
  strncpy( str, value, len);
  g_free( value);
}

//
//	 Hide all qualifiers.
//
void WUtedGtk::reset_qual()
{
  int		i;

  for( i = 0; i < UTED_QUALS; i++) {
    g_object_set( widgets.qualifier[i], "visible", FALSE, NULL);
    g_object_set( widgets.value[i], "visible", FALSE, NULL);
    g_object_set( widgets.present[i], "visible", FALSE, NULL);
    present_sts[i] = 0;
    gtk_toggle_button_set_active( GTK_TOGGLE_BUTTON( widgets.present[i]), FALSE);
    gtk_editable_delete_text( GTK_EDITABLE(widgets.value[i]), 0, -1);
  }

  gtk_label_set_label( GTK_LABEL(widgets.commandlabel), UTED_TEXT_NOCOMMAND);

  current_index = UTED_INDEX_NOCOMMAND;
}

//
//	Displays a message in the ute window.
//	Changes the label of a label widget which id is stored in the context.
//
void WUtedGtk::message( char *new_label) 
{
  gtk_label_set_label( GTK_LABEL(widgets.label), new_label);
}

void WUtedGtk::set_command_window( char *cmd)
{
}

void WUtedGtk::configure_quals( char *label)
{
  int		i;
  int		index;
  uted_sQual	*qual_ptr;

  /* Reset the picture */
  reset_qual();

  get_command_index( label, &index);
  current_index = index;

  /* Check the qualifiers and view the corresponding widgets */
  i = 0;
  qual_ptr = &commands[index].qualifier[0];
  while ( qual_ptr->qual[0] != 0) {
    if ( qual_ptr->type == UTED_QUAL_DEFQUAL) {
      gtk_label_set_label( GTK_LABEL(widgets.qualifier[i]),
		     "Select an object in the Navigator");
      g_object_set( widgets.qualifier[i], "visible", TRUE, NULL);
    }
    else {
      gtk_label_set_label( GTK_LABEL(widgets.qualifier[i]), qual_ptr->qual);
      g_object_set( widgets.qualifier[i], "visible", TRUE, NULL);
      if ( qual_ptr->present)
	g_object_set( widgets.present[i], "visible", TRUE, NULL);
      if ( qual_ptr->value)
	g_object_set( widgets.value[i], "visible", TRUE, NULL);
    }
    qual_ptr++;
    i++;
  }
  gtk_label_set_label( GTK_LABEL(widgets.commandlabel), label);
}

void WUtedGtk::enable_entries( int enable)
{
  int 	j;
  uted_sCommand	*command_ptr;

  if ( enable)
    mode = UTED_MODE_EDIT;
  else
    mode = UTED_MODE_VIEW;

  command_ptr = commands;
  j = 0;
  while ( command_ptr->command[0] != 0) {
    if ( !command_ptr->view_sensitivity ) {
      gtk_widget_set_sensitive( widgets.optmenubuttons[j], TRUE);
    }
    j++;
    command_ptr++;
  }

  /* If view and current command is not sensitiv, change to first 
     command */
  if ( !enable) {
    if ( current_index != UTED_INDEX_NOCOMMAND &&
	 !commands[ current_index].view_sensitivity) {
      reset_qual();
    }
  }  
}


/*************************************************************************
*
* Name:		void uted_questionbox()
*
* Type		void
*
* Type		Parameter	IOGF	Description
* uted_ctx	ute		I	ute context.
* char 		*question_title I	title of the question box.
* char 		*question_text	I	question to be displayed.
* voidp         yes_procedure   I       procedure called when yes is activated
* voidp         no_procedure   	I       procedure called when no is activated
*
* Description:
* 	Displays a question box.
*	When the yes-button in the box is pressed the yes_procedure is
*	called. When the no-button is pressed the no_procedure is called.
*	yes_procedure and no_procedure can be NULL.
*	Declaration of the yes and no procedures:
*		void yes_or_no_answer( uted_ctx ute)
*
**************************************************************************/

void WUtedGtk::questionbox( char *question_title,
			 char	  *question_text,
			 void	  (* yes_procedure) (WUted *),
			 void	  (* no_procedure) (WUted *),
			 void	  (* cancel_procedure) (WUted *), 
			 pwr_tBoolean cancel) 
{
#if 0
  Arg		args[5];
  int 		i;
  XmString	cstr;
  XmString	cstr2;
  XmString	help_label;
  GtkWidget		help_button;
  GtkWidget		yes_button;

  cstr = XmStringCreateLtoR( question_text, "ISO8859-1");
  cstr2 = XmStringCreateLtoR( question_title, "ISO8859-1");
  help_button = XmMessageBoxGetChild(widgets.questionbox, 
				     XmDIALOG_HELP_BUTTON);
  yes_button = XmMessageBoxGetChild( widgets.questionbox, 
				     XmDIALOG_OK_BUTTON);
  
  if (cancel)
    help_label = XmStringCreateLtoR("Cancel", "ISO8859-1");
  else
    help_label = XmStringCreateLtoR("Help", "ISO8859-1");

  i=0;
  XtSetArg(args[i], XmNhelpLabelString, help_label); i++;
  XtSetArg(args[i], XmNmessageString, cstr); i++;
  XtSetArg (args[i], XmNdialogTitle, cstr2); i++;
  XtSetValues( widgets.questionbox, args,i);                    
  
  if (cancel)
    XtVaSetValues(help_button, XmNsensitive, 1, NULL);
  else
    XtVaSetValues(help_button, XmNsensitive, 0, NULL);

  XtManageChild( widgets.questionbox);

  XmStringFree( cstr);
  XmStringFree( cstr2);
  XmStringFree(help_label);

  /* Store the yes and no functions in the context */
  questionbox_yes = yes_procedure;        
  questionbox_no = no_procedure;
  questionbox_cancel = cancel_procedure;

  XmProcessTraversal(yes_button, XmTRAVERSE_CURRENT);
#endif
}

//
//	Raises window
//
void WUtedGtk::raise_window()
{
  gtk_window_present( GTK_WINDOW(toplevel));
}