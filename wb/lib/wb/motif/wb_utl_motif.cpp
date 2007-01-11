/* 
 * Proview   $Id: wb_utl_motif.cpp,v 1.1 2007-01-04 07:29:02 claes Exp $
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

/* wb_utl.c
   Utilitys for getting information about the plcprogram structure.  */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "pwr.h"

#include <Xm/Xm.h>
#include <Mrm/MrmPublic.h>

#include "flow_ctx.h"
#include "co_cdh.h"
#include "co_dcli.h"
#include "wb_ldh.h"
#include "wb_foe_msg.h"
#include "wb_foe_motif.h"
#include "wb_utl_motif.h"
#include "wb_uilutil.h"


int wb_utl_motif::utl_foe_new( char *name, pwr_tOid plcpgm,
			       ldh_tWBContext ldhwbctx, ldh_tSesContext ldhsesctx,
			       WFoe **foe, int map_window, ldh_eAccess access)
{
  pwr_tStatus sts = 1;

  *foe = WFoe::get( plcpgm);
  if ( !*foe)
    *foe = new WFoeMotif( 0, widget, name, plcpgm, ldhwbctx, ldhsesctx,
			map_window, access, &sts);
  return sts;
}

int wb_utl_motif::utl_foe_new_local( WFoe *foe, char *name, pwr_tOid plcpgm, 
				     ldh_tWBContext ldhwbctx, ldh_tSesContext ldhsesctx, 
				     vldh_t_node nodeobject, unsigned long windowindex, 
				     unsigned long new_window, WFoe **return_foe, 
				     int map_window, ldh_eAccess access, 
				     foe_eFuncAccess function_access)
{
  pwr_tStatus sts = 1;

  *return_foe = WFoe::get( plcpgm);
  if ( !*return_foe)
    *return_foe = new WFoeMotif( foe, ((WFoeMotif *)foe)->widgets.foe_window,
				 name, plcpgm, ldhwbctx, ldhsesctx,
				 nodeobject, windowindex, new_window,
				 map_window, access, function_access, &sts);
  return sts;
}
 

/*************************************************************************
*
* Name:		utl_create_mainwindow()
*
* Type		int
*
* Type		Parameter	IOGF	Description
*
* Description: 	
*
**************************************************************************/

int wb_utl_motif::create_mainwindow( int argc, char **argv)
{
  Widget toplevel;
  Widget mainwindow;
  char uid_filename[200] = "pwr_exe:wb.uid";
  char *uid_filename_p = uid_filename;
  Arg args[20];
  int sts;

  MrmInitialize();

  sts = dcli_translate_filename( uid_filename, uid_filename);
  if ( EVEN(sts))
  {
    printf( "** pwr_exe is not defined\n");
    exit(0);
  }

  toplevel = XtInitialize ("AutoPrint", "svn", NULL, 0, &argc, argv);
  XtSetArg (args[0], XtNallowShellResize, TRUE);
  XtSetValues (toplevel, args, 1);

  uilutil_fetch( &uid_filename_p, 1, 0, 0,
		toplevel, "mainwindow", "svn_svn", 0, 0,
		&mainwindow, NULL );

  XtSetArg    (args[0], XmNheight, 500);
  XtSetValues (mainwindow, args, 1);
  XtManageChild(mainwindow);

  widget = mainwindow;

  return FOE__SUCCESS;
}

int wb_utl_motif::destroy_mainwindow()
{
  XtDestroyWidget(XtParent(widget));
  return FOE__SUCCESS;
}
