/* 
 * Proview   $Id: wb_uted_motif.cpp,v 1.1 2007-01-04 07:29:02 claes Exp $
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

/* wb_uted_motif.cpp */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>

#include "wb_login.h"

#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Protocols.h>
#include <X11/cursorfont.h>
#include <Xm/ToggleB.h>
#include <Xm/Text.h>
#include <X11/Intrinsic.h>
#include <Xm/MessageB.h>

#include "flow_ctx.h"
#include "co_time.h"
#include "co_msg.h"
#include "co_dcli.h"
#include "co_xhelp.h"
#include "wb_ldh.h"
#include "wb_foe.h"
#include "wb_foe_msg.h"
#include "wb_vldh_msg.h"
#include "wb_cmd_msg.h"
#include "wb_vldh.h"
#include "wb_uted_motif.h"
#include "wb_wtt.h"

#define	BEEP	    putchar( '\7' );

#define UTED_INDEX_NOCOMMAND -1
#define UTED_TEXT_NOCOMMAND "No command selected"

void WUtedMotif::clock_cursor()
{
  if( cursor == 0)
     cursor = XCreateFontCursor( XtDisplay(widgets.uted_window), XC_watch);
  XDefineCursor( XtDisplay( widgets.uted_window), 
		 XtWindow( widgets.uted_window), cursor);
  XFlush(XtDisplay( widgets.uted_window));
}

void WUtedMotif::reset_cursor()
{
  XUndefineCursor(XtDisplay(widgets.uted_window), XtWindow(widgets.uted_window)); 
}

void WUtedMotif::remove_command_window()
{
  int height, command_height;

  if (XtIsManaged( widgets.command_window)) {
    XtVaGetValues( widgets.uted_window, XmNheight, &height, NULL);      
    XtVaGetValues( widgets.command_window, XmNheight, &command_height, NULL);
    height -= command_height;
    XtVaSetValues( widgets.uted_window, XmNheight, height, NULL);

    XtUnmanageChild( widgets.command_window);
    XmToggleButtonSetState( widgets.commandwind_button, 0, 0);    
  }
}



void WUtedMotif::activate_command( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  Arg 		arg[1];
  char		label[40];
  XmString 	latinstr;

  uted->message( "");

  /* Get the label on this pushbutton and identify the index in
     the command table */
  XtSetArg(arg[0], XmNlabelString, &latinstr);
  XtGetValues( w, arg, 1);
  WUtedMotif::GetCSText( latinstr, label);

  uted->configure_quals( label);
}

void WUtedMotif::create_command( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  Arg 		arg[1];
  char		label[40];
  XmString 	latinstr;
  int		index;

  /* Get the label on this pushbutton and identify the index in
     the command table */
  XtSetArg(arg[0], XmNlabelString, &latinstr);
  XtGetValues( w, arg, 1);
  ((WUtedMotif *)uted)->GetCSText( latinstr, label);
  
  uted->get_command_index( label, &index);
  ((WUtedMotif *)uted)->widgets.optmenubuttons[index] = w;
}

void WUtedMotif::activate_helputils( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  CoXHelp::dhelp( "utilities_refman", 0, navh_eHelpFile_Other, "$pwr_lang/man_dg.dat", true);
}

void WUtedMotif::activate_helppwr_plc( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  CoXHelp::dhelp( "help command", 0, navh_eHelpFile_Other, "$pwr_lang/man_dg.dat", true);
}

void WUtedMotif::activate_batch( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  Arg		args[2];

  uted->message( "");
  uted->batch_sts = UTED_BATCH_BATCH;
  XmTextSetString( ((WUtedMotif *)uted)->widgets.timevalue, "");
  XtSetArg(args[0],XmNsensitive, 1);
  XtSetValues( ((WUtedMotif *)uted)->widgets.timelabel,args,1);
  XtManageChild( ((WUtedMotif *)uted)->widgets.timevalue);
}

void WUtedMotif::activate_currsess( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  Arg		args[2];

  uted->message( "");
  uted->batch_sts = UTED_BATCH_CURRSESS;
  XtUnmanageChild( ((WUtedMotif *)uted)->widgets.timevalue);
  XtSetArg(args[0],XmNsensitive, 0);
  XtSetValues( ((WUtedMotif *)uted)->widgets.timelabel,args,1);
}

//
//	Callback from the menu.
//
void WUtedMotif::activate_quit( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{

  delete uted;
}

void WUtedMotif::activate_ok( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{

  if ( data->reason != 10) return;

  uted->message( "");
  uted->clock_cursor();
  uted->execute(0);
  uted->reset_cursor();
}

void WUtedMotif::activate_cancel( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  delete uted;
}

void WUtedMotif::activate_show_cmd( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  uted->clock_cursor();
  uted->execute( 1);
  uted->reset_cursor();
}

void WUtedMotif::activate_cmd_wind( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  int		height;
  int		command_height;
  Arg		arg[20];

  if ( data->set > 1) return;

  if ( data->set == 1 ) {
    /* Increase the height of the ute window */
    XtSetArg(arg[0], XmNheight, &height);
    XtGetValues( ((WUtedMotif *)uted)->widgets.uted_window, arg, 1);
    
    XtSetArg(arg[0], XmNheight, &command_height);
    XtGetValues( ((WUtedMotif *)uted)->widgets.command_window, arg, 1);
    height += command_height;
    
    XtSetArg(arg[0], XmNheight, height);
    XtSetValues( ((WUtedMotif *)uted)->widgets.uted_window, arg, 1);
    
    XtManageChild(((WUtedMotif *)uted)->widgets.command_window);
  }
  else {
    uted->remove_command_window();
  }
}

void WUtedMotif::commandchanged( Widget w, WUted *uted, XmCommandCallbackStruct *data)
{
  static char 	str[256];
  pwr_tStatus	sts;
  pwr_tStatus	display_sts;
  int		commandfile;

  uted->message( "");
  GetCSText( data->value, str);

  display_sts = 1;
  commandfile = 0;

  dcli_remove_blank( str, str);

  sts = ((Wtt *)uted->parent_ctx)->wnav->command( str);
  if ( EVEN(sts)) {
    uted->message_error( sts);
    BEEP;
  }
  else {
    uted->message( "Done");
    BEEP;
  }
}

void WUtedMotif::activate_present1( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[0] = data->set;
}
void WUtedMotif::activate_present2( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[1] = data->set;
}
void WUtedMotif::activate_present3( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[2] = data->set;
}
void WUtedMotif::activate_present4( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[3] = data->set;
}
void WUtedMotif::activate_present5( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[4] = data->set;
}
void WUtedMotif::activate_present6( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[5] = data->set;
}
void WUtedMotif::activate_present7( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[6] = data->set;
}
void WUtedMotif::activate_present8( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[7] = data->set;
}
void WUtedMotif::activate_present9( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[8] = data->set;
}
void WUtedMotif::activate_present10( Widget w, WUted *uted, XmToggleButtonCallbackStruct *data)
{
  uted->message( "");
  uted->present_sts[9] = data->set;
}

void WUtedMotif::create_label( Widget w, WUted *uted, XmAnyCallbackStruct *data) 
{
  ((WUtedMotif *)uted)->widgets.label = w;
}

void WUtedMotif::create_adb( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.adb = w;
}

void WUtedMotif::create_file_entry( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.file_entry = w;
}

void WUtedMotif::create_quit( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.quit = w;
}

void WUtedMotif::create_commandwind_button( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.commandwind_button = w;
}

void WUtedMotif::create_batchoptmenu( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.batchoptmenu = w;
}

void WUtedMotif::create_commandlabel( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.commandlabel = w;
}

void WUtedMotif::create_batch( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.batch = w;
}

void WUtedMotif::create_currsess( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.currsess = w;
}

void WUtedMotif::create_cmd_wind( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.command_window = w;
}

void WUtedMotif::create_timelabel( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.timelabel = w;
}

void WUtedMotif::create_timevalue( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.timevalue = w;
}

void WUtedMotif::create_qualifier1( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[0] = w;
}

void WUtedMotif::create_value1( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[0] = w;
}

void WUtedMotif::create_present1( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[0] = w;
}

void WUtedMotif::create_qualifier2( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[1] = w;
}

void WUtedMotif::create_value2( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[1] = w;
}

void WUtedMotif::create_present2( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[1] = w;
}

void WUtedMotif::create_qualifier3( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[2] = w;
}

void WUtedMotif::create_value3( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[2] = w;
}

void WUtedMotif::create_present3( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[2] = w;
}

void WUtedMotif::create_qualifier4( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{ 
  ((WUtedMotif *)uted)->widgets.qualifier[3] = w;
}

void WUtedMotif::create_value4( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[3] = w;
}

void WUtedMotif::create_present4( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[3] = w;
}

void WUtedMotif::create_qualifier5( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[4] = w;
}

void WUtedMotif::create_value5( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[4] = w;
}

void WUtedMotif::create_present5( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[4] = w;
}

void WUtedMotif::create_qualifier6( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[5] = w;
}

void WUtedMotif::create_value6( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[5] = w;
}

void WUtedMotif::create_present6( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[5] = w;
}

void WUtedMotif::create_qualifier7( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[6] = w;
}

void WUtedMotif::create_value7( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[6] = w;
}

void WUtedMotif::create_present7( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[6] = w;
}

void WUtedMotif::create_qualifier8( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[7] = w;
}

void WUtedMotif::create_value8( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[7] = w;
}

void WUtedMotif::create_present8( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[7] = w;
}

void WUtedMotif::create_qualifier9( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[8] = w;
}

void WUtedMotif::create_value9( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[8] = w;
}

void WUtedMotif::create_present9( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[8] = w;
}

void WUtedMotif::create_qualifier10( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.qualifier[9] = w;
}

void WUtedMotif::create_value10( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.value[9] = w;
}

void WUtedMotif::create_present10( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.present[9] = w;
}

void WUtedMotif::qbox_cr( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  ((WUtedMotif *)uted)->widgets.questionbox = w;
}

//
//   Execute the yes procedure for the questionbox.
//
void WUtedMotif::qbox_yes_cb( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  XtUnmanageChild( ((WUtedMotif *)uted)->widgets.questionbox);

  if ( uted->questionbox_yes != NULL)
    (uted->questionbox_yes) ( uted);
}

//
//   Execute the no procedure for the questionbox.
//
void WUtedMotif::qbox_no_cb( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  XtUnmanageChild( ((WUtedMotif *)uted)->widgets.questionbox);

  if ( uted->questionbox_no != NULL)
    (uted->questionbox_no) ( uted);
}

//
//   Unmanage the questionbox.
//
void WUtedMotif::qbox_cancel_cb( Widget w, WUted *uted, XmAnyCallbackStruct *data)
{
  if ( uted->questionbox_cancel != NULL)
    (uted->questionbox_cancel) ( uted);
  XtUnmanageChild( ((WUtedMotif *)uted)->widgets.questionbox);
}

//
//	Create a new ute window
//
WUtedMotif::WUtedMotif( void	       	*wu_parent_ctx,
			Widget		wu_parent_wid,
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
			       topLevelShellWidgetClass, parent_wid, args, i);

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
  sts = MrmFetchWidgetOverride(s_MrmH, "uted_window", parent_wid ,
		name, args, 1,
		&widgets.uted_window, &dclass);
  if (sts != MrmSUCCESS) printf("can't fetch utedit widget\n");

  XtManageChild( widgets.uted_window);

  /* SG 09.02.91 a top level should always be realized ! */
  XtPopup( parent_wid, XtGrabNone );

  /* Ask MRM to fetch the question box */
  i=0;
  if (MrmFetchWidgetOverride(s_MrmH,
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
  *status = FOE__SUCCESS;
}

//
//	Destroys a ute instance.
//      Destroys the widget and frees allocated memory for the 
//	context.
//
WUtedMotif::~WUtedMotif()
{
  if ( quit_cb)
    (quit_cb)( parent_ctx); 

  /* Destroy the widget */
  XtDestroyWidget( parent_wid);
}

//
// Get value from textentry with specified index
//
void WUtedMotif::get_value( int idx, char *str, int len)
{
  char *value;

  value = XmTextGetString( widgets.value[idx]);
  strncpy( str, value, len);
  XtFree( value);
}

//
//	 Hide all qualifiers.
//
void WUtedMotif::reset_qual()
{
  int		i;
  XmString 	cstr;

  for( i = 0; i < UTED_QUALS; i++) {
    XtUnmanageChild( widgets.qualifier[i]);
    XtUnmanageChild( widgets.value[i]);
    XtUnmanageChild( widgets.present[i]);
    present_sts[i] = 0;
    XmToggleButtonSetState( widgets.present[i], 0, 0);
    XmTextSetString( widgets.value[i], "");
  }

  cstr = XmStringCreateSimple( UTED_TEXT_NOCOMMAND);
  XtVaSetValues( widgets.commandlabel, 
		 XmNlabelString, cstr,
		 NULL);
  XmStringFree( cstr);

  current_index = UTED_INDEX_NOCOMMAND;
}

void WUtedMotif::GetCSText( XmString ar_value, char *t_buffer)
{
  char			*first_seg;
  XmStringContext		context;
  XmStringCharSet 		charset;
  XmStringDirection 		dir_r_to_l;
  Boolean 			sep;

  if (ar_value == 0) {
    strcpy(t_buffer,"");
    return;
  }
    
    
  XmStringInitContext( &context, ar_value);
  
  if ( XmStringGetNextSegment( context, &first_seg, &charset, &dir_r_to_l,
			       &sep) != TRUE ) {
    *first_seg = 0;
  }
  else {
    strcpy(t_buffer,first_seg);
    XtFree(first_seg);
  }
  XmStringFreeContext (context);
}

//
//	Displays a message in the ute window.
//	Changes the label of a label widget which id is stored in the context.
//
void WUtedMotif::message( char *new_label) 
{
  Arg		args[2];
  XmString	cstr;

  XtSetArg(args[0], XmNlabelString,
	   cstr=XmStringCreateLtoR(new_label , "ISO8859-1"));
  XtSetValues( widgets.label, args,1);
  XmStringFree( cstr);
}

void WUtedMotif::set_command_window( char *cmd)
{
  XmString	cstr;
  Arg		arg[2];

  cstr=XmStringCreateLtoR( cmd, "ISO8859-1");
  XtSetArg(arg[0], XmNcommand, cstr);
  XtSetValues( widgets.command_window, arg, 1);
  XmStringFree( cstr);
}

void WUtedMotif::configure_quals( char *label)
{
  Arg 		arg[1];
  Arg 		args[1];
  int		i;
  int		index;
  uted_sQual	*qual_ptr;
  XmString 	latinstr;

  /* Reset the picture */
  reset_qual();


  get_command_index( label, &index);
  current_index = index;

  /* Make the batch buttons sensitive or not */
  if ( commands[index].batch_sensitivity & 1) {
    XtSetArg(args[0],XmNsensitive, 1);
    XtSetValues( widgets.currsess,args,1);
  }
  else {
    XtSetArg(args[0],XmNsensitive, 0);
    XtSetValues( widgets.currsess,args,1);
    XtSetArg(args[0],XmNmenuHistory, widgets.batch);
    XtSetValues( widgets.batchoptmenu,args,1);
    batch_sts = UTED_BATCH_BATCH;
  }
  if ( commands[index].batch_sensitivity & 2) {
    XtSetArg(args[0],XmNsensitive, 1);
    XtSetValues( widgets.batch,args,1);
  }
  else {
    XtSetArg(args[0],XmNsensitive, 0);
    XtSetValues( widgets.batch,args,1);
    XtSetArg(args[0],XmNmenuHistory, widgets.currsess);
    XtSetValues( widgets.batchoptmenu,args,1);
    batch_sts = UTED_BATCH_CURRSESS;
  }
	
  /* Check the qualifiers and view the corresponding widgets */
  i = 0;
  qual_ptr = &commands[index].qualifier[0];
  while ( qual_ptr->qual[0] != 0) {
    if ( qual_ptr->type == UTED_QUAL_DEFQUAL) {
      latinstr = XmStringCreateLtoR( 
		     "Select an object in the Navigator", "ISO8859-1");
      XtSetArg( arg[0], XmNlabelString, latinstr);
      XtSetValues( widgets.qualifier[i], arg, 1);
      XmStringFree( latinstr);
      XtManageChild( widgets.qualifier[i]);
    }
    else {
      latinstr = XmStringCreateLtoR( qual_ptr->qual, "ISO8859-1");
      XtSetArg( arg[0], XmNlabelString, latinstr);
      XtSetValues( widgets.qualifier[i], arg, 1);
      XmStringFree( latinstr);
      XtManageChild( widgets.qualifier[i]);
      if ( qual_ptr->present)
	XtManageChild( widgets.present[i]);
      if ( qual_ptr->value)
	XtManageChild( widgets.value[i]);
    }
    qual_ptr++;
    i++;
  }
  latinstr = XmStringCreateSimple( label);
  XtVaSetValues( widgets.commandlabel, 
		 XmNlabelString, latinstr,
		 NULL);
  XmStringFree( latinstr);
}

void WUtedMotif::enable_entries( int enable)
{
  Arg	args[20];
  int 	j;
  uted_sCommand	*command_ptr;

  if ( enable)
    mode = UTED_MODE_EDIT;
  else
    mode = UTED_MODE_VIEW;

  XtSetArg(args[0],XmNsensitive, enable );
  XtSetValues( widgets.commandwind_button,args,1);

  command_ptr = commands;
  j = 0;
  while ( command_ptr->command[0] != 0) {
    if ( !command_ptr->view_sensitivity ) {
      XtSetValues( widgets.optmenubuttons[j],args,1);
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

void WUtedMotif::questionbox( char *question_title,
			 char	  *question_text,
			 void	  (* yes_procedure) (WUted *),
			 void	  (* no_procedure) (WUted *),
			 void	  (* cancel_procedure) (WUted *), 
			 pwr_tBoolean cancel) 
{
  Arg		args[5];
  int 		i;
  XmString	cstr;
  XmString	cstr2;
  XmString	help_label;
  Widget		help_button;
  Widget		yes_button;

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

}

//
//	Raises window
//
void WUtedMotif::raise_window()
{
  Widget shell;

  shell = XtParent( widgets.uted_window);
  while (!XtIsShell(shell))
    shell = XtParent(shell);
  
  XMapRaised(XtDisplay(shell), XtWindow(shell));
}