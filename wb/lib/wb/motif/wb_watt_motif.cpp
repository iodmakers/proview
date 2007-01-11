/* 
 * Proview   $Id: wb_watt_motif.cpp,v 1.1 2007-01-04 07:29:02 claes Exp $
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

/* wb_watt.cpp -- Display object attributes */

#if defined OS_VMS && defined __ALPHA
# pragma message disable (NOSIMPINT,EXTROUENCUNNOBJ)
#endif

#if defined OS_VMS && !defined __ALPHA
# pragma message disable (LONGEXTERN)
#endif

#include "glow_std.h"

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/Text.h>
#include <Mrm/MrmPublic.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "co_cdh.h"
#include "co_dcli.h"
#include "co_time.h"
#include "flow_x.h"
#include "wb_watt_msg.h"
#include "co_mrm_util.h"

#include "flow.h"
#include "flow_browctx.h"
#include "flow_browapi.h"
#include "wb_watt_motif.h"
#include "wb_wattnav_motif.h"
#include "wb_wtt.h"
#include "wb_wnav.h"
#include "co_xhelp.h"

// Static member elements
char WAttMotif::value_recall[30][160];

void WAttMotif::message( char severity, char *message)
{
  Arg 		args[2];
  XmString	cstr;

  cstr=XmStringCreateLtoR( message, "ISO8859-1");
  XtSetArg(args[0],XmNlabelString, cstr);
  XtSetArg(args[1],XmNheight, 20);
  XtSetValues( msg_label, args, 2);
  XmStringFree( cstr);
}

void WAttMotif::set_prompt( char *prompt)
{
  Arg 		args[3];
  XmString	cstr;

  cstr=XmStringCreateLtoR( prompt, "ISO8859-1");
  XtSetArg(args[0],XmNlabelString, cstr);
  XtSetArg(args[1],XmNwidth, 50);
  XtSetArg(args[2],XmNheight, 30);
  XtSetValues( cmd_prompt, args, 3);
  XmStringFree( cstr);
}

void WAttMotif::change_value( int set_focus)
{
  int		sts;
  Widget	text_w;
  int		multiline;
  char		*value;
  Arg 		args[5];
  int		input_size;

  if ( input_open) {
    XtUnmanageChild( cmd_input);
    set_prompt( "");
    input_open = 0;
    return;
  }

  sts = ((WAttNav *)wattnav)->check_attr( &multiline, &input_node, input_name,
		&value, &input_size);
  if ( EVEN(sts)) {
    if ( sts == WATT__NOATTRSEL)
      message( 'E', "No attribute is selected");
    else
      message( 'E', wnav_get_message( sts));
    return;
  }

  if ( multiline) {
    text_w = cmd_scrolledinput;
    XtManageChild( text_w);
    XtManageChild( cmd_scrolled_ok);
    XtManageChild( cmd_scrolled_ca);

    // XtSetArg(args[0], XmNpaneMaximum, 300);
    // XtSetValues( wattnav_form, args, 1);

    XtSetArg(args[0], XmNmaxLength, input_size-1);
    XtSetValues( text_w, args, 1);
    if ( value) {
      XmTextSetString( text_w, value);
//    XmTextSetInsertionPosition( text_w, strlen(value));
    }
    else
      XmTextSetString( text_w, "");

    input_multiline = 1;
  }
  else {
    text_w = cmd_input;
    XtManageChild( text_w);
    XtSetArg(args[0],XmNmaxLength, input_size-1);
    XtSetValues( text_w, args, 1);
    if ( value) {
      XmTextSetString( text_w, value);
      XmTextSetInsertionPosition( text_w, strlen(value));
      XmTextSetSelection( text_w, 0, strlen(value), CurrentTime);
    }
    else
      XmTextSetString( text_w, "");

    input_multiline = 0;
  }

  message( ' ', "");
  if ( set_focus)
    flow_SetInputFocus( text_w);
  set_prompt( "value >");
  input_open = 1;
}

//
//  Callbackfunctions from menu entries
//
void WAttMotif::activate_change_value( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->change_value(1);
}

void WAttMotif::activate_close_changeval( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->change_value_close();
}

void WAttMotif::activate_exit( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  if ( watt->close_cb)
    (watt->close_cb)( watt);
  else
    delete watt;
}

void WAttMotif::activate_help( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  CoXHelp::dhelp( "objecteditor_refman", 0, navh_eHelpFile_Other, 
		  "$pwr_lang/man_dg.dat", true);
}

void WAttMotif::create_msg_label( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->msg_label = w;
}
void WAttMotif::create_cmd_prompt( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->cmd_prompt = w;
}
void WAttMotif::create_cmd_input( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  Arg args[2];

  XtSetArg    (args[0], XmNuserData, watt);
  XtSetValues (w, args, 1);

  mrm_TextInit( w, (XtActionProc) valchanged_cmd_input, mrm_eUtility_WAtt);
  watt->cmd_input = w;
}
void WAttMotif::create_cmd_scrolledinput( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->cmd_scrolledinput = w;
}
void WAttMotif::create_cmd_scrolled_ok( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->cmd_scrolled_ok = w;
}
void WAttMotif::create_cmd_scrolled_ca( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->cmd_scrolled_ca = w;
}
void WAttMotif::create_wattnav_form( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  watt->wattnav_form = w;
}

static void watt_enable_set_focus( WAttMotif *watt)
{
  watt->set_focus_disabled--;
}

static void watt_disable_set_focus( WAttMotif *watt, int time)
{
  watt->set_focus_disabled++;
  watt->focus_timerid = XtAppAddTimeOut(
	XtWidgetToApplicationContext( watt->toplevel), time,
	(XtTimerCallbackProc)watt_enable_set_focus, watt);
}

void WAttMotif::action_inputfocus( Widget w, XmAnyCallbackStruct *data)
{
  Arg args[1];
  WAttMotif *watt;

  XtSetArg    (args[0], XmNuserData, &watt);
  XtGetValues (w, args, 1);

  if ( !watt)
    return;

  if ( mrm_IsIconicState(w))
    return;

  if ( watt->set_focus_disabled)
    return;

  if ( flow_IsManaged( watt->cmd_scrolledinput))
    flow_SetInputFocus( watt->cmd_scrolledinput);
  else if ( flow_IsManaged( watt->cmd_input))
    flow_SetInputFocus( watt->cmd_input);
  else if ( watt->wattnav)
    ((WAttNav *)watt->wattnav)->set_inputfocus();

  watt_disable_set_focus( watt, 400);

}

void WAttMotif::valchanged_cmd_input( Widget w, XEvent *event)
{
  WAtt 	*watt;
  int 	sts;
  char 	*text;
  Arg 	args[2];

  XtSetArg(args[0], XmNuserData, &watt);
  XtGetValues(w, args, 1);

  sts = mrm_TextInput( w, event, (char *)value_recall, sizeof(value_recall[0]),
	sizeof( value_recall)/sizeof(value_recall[0]),
	&((WAttMotif *)watt)->value_current_recall);
  if ( sts)
  {
    text = XmTextGetString( w);
    if ( watt->input_open) {
      sts = ((WAttNav *)watt->wattnav)->set_attr_value( watt->input_node, 
		watt->input_name, text);
      XtUnmanageChild( w);
      watt->set_prompt( "");
      watt->input_open = 0;
      if ( watt->redraw_cb)
        (watt->redraw_cb)( watt);

      ((WAttNav *)watt->wattnav)->set_inputfocus();
    }
  }
}

void WAttMotif::change_value_close()
{
  char *text;
  int sts;

  text = XmTextGetString( cmd_scrolledinput);
  if ( input_open) {
    if ( input_multiline) {
      sts = ((WAttNav *)wattnav)->set_attr_value( input_node,
		input_name, text);
      XtUnmanageChild( cmd_scrolledinput);
      XtUnmanageChild( cmd_scrolled_ok);
      XtUnmanageChild( cmd_scrolled_ca);
      set_prompt( "");
      input_open = 0;

      ((WAttNav *)wattnav)->redraw();
      ((WAttNav *)wattnav)->set_inputfocus();
    }
    else {
      text = XmTextGetString( cmd_input);

      sts = ((WAttNav *)wattnav)->set_attr_value( input_node, 
		input_name, text);
      XtUnmanageChild( cmd_input);
      set_prompt( "");
      input_open = 0;
      if ( redraw_cb)
        (redraw_cb)( this);

      ((WAttNav *)wattnav)->set_inputfocus();
    }
  }
}

void WAttMotif::activate_cmd_input( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  char *text;
  int sts;

  text = XmTextGetString( w);
  if ( watt->input_open) {
    sts = ((WAttNav *)watt->wattnav)->set_attr_value( watt->input_node, 
		watt->input_name, text);
    XtUnmanageChild( w);
    watt->set_prompt( "");
    watt->input_open = 0;
    if ( watt->redraw_cb)
      (watt->redraw_cb)( watt);

    ((WAttNav *)watt->wattnav)->set_inputfocus();
  }
}

void WAttMotif::activate_cmd_scrolled_ok( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{
  char *text;
  unsigned char *s;
  int sts;


  text = XmTextGetString( watt->cmd_scrolledinput);
  // Replace ctrl characters with space
  for ( s = (unsigned char *) text; *s; s++) {
    if ( *s < ' ' && *s != 10 && *s != 13)
      *s = ' ';
  }
  if ( watt->input_open) {
    sts = ((WAttNav *)watt->wattnav)->set_attr_value( watt->input_node,
		watt->input_name, text);
    XtUnmanageChild( watt->cmd_scrolledinput);
    XtUnmanageChild( watt->cmd_scrolled_ok);
    XtUnmanageChild( watt->cmd_scrolled_ca);
    watt->set_prompt( "");
    watt->input_open = 0;

    ((WAttNav *)watt->wattnav)->redraw();
    ((WAttNav *)watt->wattnav)->set_inputfocus();
  }
}

void WAttMotif::activate_cmd_scrolled_ca( Widget w, WAttMotif *watt, XmAnyCallbackStruct *data)
{

  if ( watt->input_open) {
    XtUnmanageChild( watt->cmd_scrolledinput);
    XtUnmanageChild( watt->cmd_scrolled_ok);
    XtUnmanageChild( watt->cmd_scrolled_ca);
    watt->set_prompt( "");
    watt->input_open = 0;
    ((WAttNav *)watt->wattnav)->set_inputfocus();
  }
}

int WAttMotif::open_changevalue( char *name)
{
  int sts;

  sts = ((WAttNav*)wattnav)->select_by_name( name);
  if ( EVEN(sts)) return sts;

  change_value(0);
  return WATT__SUCCESS;
}

void WAttMotif::pop()
{
  flow_UnmapWidget( parent_wid);
  flow_MapWidget( parent_wid);
}

WAttMotif::~WAttMotif()
{
  if ( set_focus_disabled)
    XtRemoveTimeOut( focus_timerid);

  delete (WAttNav *)wattnav;
  XtDestroyWidget( parent_wid);
}

WAttMotif::WAttMotif( 
	Widget 		wa_parent_wid,
	void 		*wa_parent_ctx, 
	ldh_tSesContext wa_ldhses, 
	pwr_sAttrRef 	wa_aref,
	int 		wa_editmode,
	int 		wa_advanced_user,
	int		wa_display_objectname) :
  WAtt(wa_parent_ctx,wa_ldhses,wa_aref,wa_editmode,wa_advanced_user,
       wa_display_objectname), parent_wid(wa_parent_wid), 
  set_focus_disabled(0), value_current_recall(0)
{
  char		uid_filename[120] = {"pwr_exe:wb_watt.uid"};
  char		*uid_filename_p = uid_filename;
  Arg 		args[20];
  pwr_tStatus	sts;
  char 		title[80];
  char		*title_p;
  int		size;
  int		i;
  MrmHierarchy s_DRMh;
  MrmType dclass;
  char		name[] = "Proview/R Navigator";

  static char translations[] =
    "<FocusIn>: watt_inputfocus()\n";
  static XtTranslations compiled_translations = NULL;

  static XtActionsRec actions[] =
  {
    {"watt_inputfocus",      (XtActionProc) action_inputfocus}
  };

  static MrmRegisterArg	reglist[] = {
        { "watt_ctx", 0 },
	{"watt_activate_exit",(caddr_t)activate_exit },
	{"watt_activate_change_value",(caddr_t)activate_change_value },
	{"watt_activate_close_changeval",(caddr_t)activate_close_changeval },
	{"watt_activate_help",(caddr_t)activate_help },
	{"watt_create_msg_label",(caddr_t)create_msg_label },
	{"watt_create_cmd_prompt",(caddr_t)create_cmd_prompt },
	{"watt_create_cmd_input",(caddr_t)create_cmd_input },
	{"watt_create_cmd_scrolledinput",(caddr_t)create_cmd_scrolledinput },
	{"watt_create_cmd_scrolled_ok",(caddr_t)create_cmd_scrolled_ok },
	{"watt_create_cmd_scrolled_ca",(caddr_t)create_cmd_scrolled_ca },
	{"watt_create_wattnav_form",(caddr_t)create_wattnav_form },
	{"watt_activate_cmd_scrolledinput",(caddr_t)activate_cmd_input },
	{"watt_activate_cmd_scrolled_ok",(caddr_t)activate_cmd_scrolled_ok },
	{"watt_activate_cmd_scrolled_ca",(caddr_t)activate_cmd_scrolled_ca }
	};

  static int	reglist_num = (sizeof reglist / sizeof reglist[0]);

  // for ( i = 0; i < int(sizeof(value_recall)/sizeof(value_recall[0])); i++)
  //  value_recall[i][0] = 0;

  dcli_translate_filename( uid_filename, uid_filename);

  // Compose a title
  sts = ldh_AttrRefToName( ldhses, &aref, cdh_mName_path | cdh_mName_object | cdh_mName_attribute, 
			   &title_p, &size);
  strncpy( title, title_p, sizeof(title));

  // Create object context
//  attrctx->close_cb = close_cb;
//  attrctx->redraw_cb = redraw_cb;

  // Motif
  MrmInitialize();

  reglist[0].value = (caddr_t) this;

  // Save the context structure in the widget
  i = 0;
  XtSetArg (args[i], XmNuserData, (unsigned int) this);i++;
  XtSetArg( args[i], XmNdeleteResponse, XmDO_NOTHING);i++;

  sts = MrmOpenHierarchy( 1, &uid_filename_p, NULL, &s_DRMh);
  if (sts != MrmSUCCESS) printf("can't open %s\n", uid_filename);

  MrmRegisterNames(reglist, reglist_num);

  parent_wid = XtCreatePopupShell( title, 
		topLevelShellWidgetClass, parent_wid, args, i);

  sts = MrmFetchWidgetOverride( s_DRMh, "watt_window", parent_wid,
				name, args, 1, &toplevel, &dclass);
  if (sts != MrmSUCCESS)  printf("can't fetch %s\n", name);

  MrmCloseHierarchy(s_DRMh);


  if (compiled_translations == NULL) 
    XtAppAddActions( XtWidgetToApplicationContext(toplevel), 
		     actions, XtNumber(actions));
 
  if (compiled_translations == NULL) 
    compiled_translations = XtParseTranslationTable(translations);
  XtOverrideTranslations( toplevel, compiled_translations);

  i = 0;
  XtSetArg(args[i],XmNwidth,420);i++;
  XtSetArg(args[i],XmNheight,600);i++;
  XtSetValues( toplevel ,args,i);
    
  XtManageChild( toplevel);
  XtUnmanageChild( cmd_input);
  XtUnmanageChild( cmd_scrolledinput);
  XtUnmanageChild( cmd_scrolled_ok);
  XtUnmanageChild( cmd_scrolled_ca);
  
  if ( ((WUtility *)parent_ctx)->utype == wb_eUtility_WNav)
    parent_ctx = ((WNav *)parent_ctx)->parent_ctx;
  utility = ((WUtility *)parent_ctx)->utype;
  
  wattnav = new WAttNavMotif( (void *)this, wattnav_form, "Plant",
			      ldhses, aref, wa_editmode, wa_advanced_user,
			      wa_display_objectname, utility, &brow_widget, &sts);
  ((WAttNav *)wattnav)->message_cb = &WAtt::message_cb;
  ((WAttNav *)wattnav)->change_value_cb = &WAtt::change_value_cb;
  
  XtPopup( parent_wid, XtGrabNone);

  // Connect the window manager close-button to exit
  flow_AddCloseVMProtocolCb( parent_wid, 
			     (XtCallbackProc)activate_exit, this);

  if ( utility == wb_eUtility_Wtt) {
    ((Wtt *)parent_ctx)->register_utility( (void *) this,
					   wb_eUtility_AttributeEditor);
  }
}










