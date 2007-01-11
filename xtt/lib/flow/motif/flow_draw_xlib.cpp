/* 
 * Proview   $Id: flow_draw_xlib.cpp,v 1.1 2007-01-04 07:57:00 claes Exp $
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

#include "flow_std.h"

#include <stdio.h>
#include <stdlib.h>


#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Mrm/MrmPublic.h>
#include <Xm/Text.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/cursorfont.h>
#define XK_MISCELLANY
#include <X11/keysymdef.h>

#include "flow.h"
#include "flow_ctx.h"
#include "flow_browctx.h"
#include "flow_draw_xlib.h"
#include "flow_widget_motif.h"
#include "flow_browwidget_motif.h"
#include "flow_msg.h"
#include "pwr.h"
#include "co_dcli.h"

#if defined IMLIB
# if defined OS_LYNX
#  define __NO_INCLUDE_WARN__ 1
# endif
# include <X11/extensions/shape.h>
# include <Imlib.h>
# if defined OS_LYNX
#  undef __NO_INCLUDE_WARN__
# endif
#else
typedef void *ImlibData;
typedef void *ImlibImage;
#endif

#define DRAW_PRESS_PIX 9

typedef struct {
	Widget		w;
	int		x;
	int		y;
	int		width;
	int		height;
	FlowArrayElem	*node;
	int		number;
        FlowCtx 	*ctx;
} draw_sAnnotData;

typedef struct {
	FlowCtx 	*ctx;
	void 		(*callback_func)( FlowCtx *ctx);
	XtIntervalId	timer_id;
} draw_sTimerCb;

typedef struct {
	Pixmap pixmap[DRAW_PIXMAP_SIZE];
	} draw_sPixmap;

static char	font_names[draw_eFont__][DRAW_FONT_SIZE][80] = { {	
	"-*-Helvetica-Bold-R-Normal--8-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--10-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--12-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--14-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--14-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Bold-R-Normal--24-*-*-*-P-*-ISO8859-1"
	},{ 
	"-*-Helvetica-Medium-R-Normal--8-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--10-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--12-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--14-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--14-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--18-*-*-*-P-*-ISO8859-1", 
	"-*-Helvetica-Medium-R-Normal--24-*-*-*-P-*-ISO8859-1"
	} };


static  XEvent	last_event;

static int flow_allocate_color( FlowDrawXLib *draw_ctx, char *named_color);
static void event_timer( FlowCtx *ctx, int time_ms);
static void cancel_event_timer(FlowCtx *ctx);
static void event_timer_cb( FlowCtx *ctx);
static void draw_input_cb( Widget w, XtPointer client_data, XtPointer call_data);


static int flow_create_cursor( FlowDrawXLib *draw_ctx)
{
  /* Create some cursors */

  /* Cross cursor */
  draw_ctx->cursors[draw_eCursor_CrossHair] = 
		XCreateFontCursor( draw_ctx->display, XC_crosshair);

  draw_ctx->cursors[draw_eCursor_DiamondCross] = 
		XCreateFontCursor( draw_ctx->display, XC_diamond_cross);

  draw_ctx->cursors[draw_eCursor_Hand] = 
		XCreateFontCursor( draw_ctx->display, XC_hand2);

  return 1;
}

static int draw_free_gc( FlowDrawXLib *draw_ctx)
{
  int i, j;

  for ( i = 1; i < draw_eCursor__ ; i++)
    XFreeCursor( draw_ctx->display, draw_ctx->cursors[i]);

  XFreeGC( draw_ctx->display, draw_ctx->gc_yellow);
  XFreeGC( draw_ctx->display, draw_ctx->gc_green);
  XFreeGC( draw_ctx->display, draw_ctx->gc_inverse);
  for ( i = 0; i < flow_eDrawType__; i++)
  {
    for ( j = 0; j < DRAW_TYPE_SIZE; j++)
      XFreeGC( draw_ctx->display, draw_ctx->gcs[i][j]);
  }  

  for ( i = 0; i < draw_eFont__; i++)
  {
    for ( j = 0; j < DRAW_FONT_SIZE; j++)
    {
//      XUnloadFont( draw_ctx->display, draw_ctx->font[i][j]);
      XFreeFont( draw_ctx->display, draw_ctx->font_struct[i][j]);
    }
  }
  return 1;
}

static int flow_create_gc( FlowDrawXLib *draw_ctx, Window window)
{
  Font 			font;
  XGCValues 		xgcv;
  int			i;

  /* Inverse gc */
  xgcv.background = XBlackPixelOfScreen(draw_ctx->screen);
  xgcv.foreground = draw_ctx->background;
  draw_ctx->gc_inverse = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);

  /* Yellow gc */
  xgcv.foreground = flow_allocate_color( draw_ctx, "yellow");
  xgcv.background = draw_ctx->background;
  draw_ctx->gc_yellow = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);

  /* Green gc */
  xgcv.foreground = flow_allocate_color( draw_ctx, "green");
  xgcv.background = draw_ctx->background;
  draw_ctx->gc_green = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);

  /* Black line gc */
  xgcv.foreground = XBlackPixelOfScreen(draw_ctx->screen);
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;

    draw_ctx->gcs[flow_eDrawType_Line][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground | GCLineWidth, &xgcv);
  }

  /* Erase line gc */
  xgcv.foreground = draw_ctx->background;
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;

    draw_ctx->gcs[flow_eDrawType_LineErase][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground | GCLineWidth, &xgcv);
  }

  /* Red line gc */
  xgcv.foreground = flow_allocate_color( draw_ctx, "red");
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;

    draw_ctx->gcs[flow_eDrawType_LineRed][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground | GCLineWidth, &xgcv);
  }

  /* Gray line gc */
  xgcv.foreground = flow_allocate_color( draw_ctx, "gray");
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;

    draw_ctx->gcs[flow_eDrawType_LineGray][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground | GCLineWidth, &xgcv);
  }

  /* Dashed line gc */
  xgcv.foreground = XBlackPixelOfScreen(draw_ctx->screen);
  xgcv.background = draw_ctx->background;
  xgcv.line_style = LineOnOffDash;
  xgcv.dash_offset = 0;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;
    xgcv.dashes = 7 + i;

    draw_ctx->gcs[flow_eDrawType_LineDashed][i] = XCreateGC( draw_ctx->display, 
	window, 
	GCForeground | GCBackground | GCLineWidth| GCLineStyle | GCDashOffset |
	GCDashList, &xgcv);
  }

  /* Red dashed line gc */
  xgcv.foreground = flow_allocate_color( draw_ctx, "red");
  xgcv.background = draw_ctx->background;
  xgcv.line_style = LineOnOffDash;
  xgcv.dash_offset = 0;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    xgcv.line_width = i + 1;
    xgcv.dashes = 7 + i;

    draw_ctx->gcs[flow_eDrawType_LineDashedRed][i] = XCreateGC( draw_ctx->display, 
	window, 
	GCForeground | GCBackground | GCLineWidth| GCLineStyle | GCDashOffset |
	GCDashList, &xgcv);
  }

  /* Text */
  xgcv.foreground = XBlackPixelOfScreen(draw_ctx->screen);
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    draw_ctx->gcs[flow_eDrawType_TextHelvetica][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);
  }

  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    draw_ctx->gcs[flow_eDrawType_TextHelveticaBold][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);
  }

  xgcv.foreground = draw_ctx->background;
  xgcv.background = draw_ctx->background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    draw_ctx->gcs[flow_eDrawType_TextHelveticaErase][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);
  }

  for ( i = 0; i < DRAW_TYPE_SIZE; i++)
  {
    draw_ctx->gcs[flow_eDrawType_TextHelveticaEraseBold][i] = XCreateGC( draw_ctx->display, 
	window, GCForeground | GCBackground, &xgcv);
  }

  for ( i = 0; i < DRAW_FONT_SIZE; i++)
  {
    font = XLoadFont( draw_ctx->display, 
	font_names[draw_eFont_HelveticaBold][i]);
    XSetFont( draw_ctx->display, 
	draw_ctx->gcs[flow_eDrawType_TextHelveticaBold][i], font);
    XSetFont( draw_ctx->display, 
	draw_ctx->gcs[flow_eDrawType_TextHelveticaEraseBold][i], font);
    draw_ctx->font[draw_eFont_HelveticaBold][i] = font;
    draw_ctx->font_struct[draw_eFont_HelveticaBold][i] =
		XQueryFont( draw_ctx->display, font);
  }
  for ( i = 0; i < DRAW_FONT_SIZE; i++)
  {
    font = XLoadFont( draw_ctx->display, font_names[draw_eFont_Helvetica][i]);
    XSetFont( draw_ctx->display, 
	draw_ctx->gcs[flow_eDrawType_TextHelvetica][i], font);
    XSetFont( draw_ctx->display, 
	draw_ctx->gcs[flow_eDrawType_TextHelveticaErase][i], font);
    draw_ctx->font[draw_eFont_Helvetica][i] = font;
    draw_ctx->font_struct[draw_eFont_Helvetica][i] =
		XQueryFont( draw_ctx->display, font);
  }

  return 1;
}


static int flow_allocate_color( FlowDrawXLib *draw_ctx, char *named_color)
{
  XColor exact_color, screen_color;
  Screen *screen;
  Visual *visual;

  screen = draw_ctx->screen;
  visual = XDefaultVisualOfScreen( screen);
  if ( visual->c_class == TrueColor ||
       visual->c_class == PseudoColor ||
       visual->c_class == DirectColor ||
       visual->c_class == StaticColor)
  {
    if ( XAllocNamedColor( draw_ctx->display, 
		XDefaultColormapOfScreen( draw_ctx->screen),
	    	named_color, &screen_color, &exact_color))
      return screen_color.pixel;
    else
      printf( "** Color not allocated !\n");
    return XBlackPixelOfScreen( draw_ctx->screen);
  }
  else
  {
    return XBlackPixelOfScreen( draw_ctx->screen);
  }
}

FlowDrawXLib::~FlowDrawXLib()
{
  basectx->set_nodraw();
  delete basectx;
  draw_free_gc( this);
}

int FlowDrawXLib::create_secondary_ctx( 
	FlowCtx *flow_ctx,
	void **secondary_flow_ctx,
        int (*init_proc)(FlowCtx *ctx, void *client_data),
	void  *client_data, 
	flow_eCtxType type
)
{
  FlowCtx	*ctx;

  if ( type == flow_eCtxType_Brow)
    ctx = (FlowCtx *) new BrowCtx("Claes context", 20);
  else
    ctx = new FlowCtx("Claes context", 20);
  
  ctx->fdraw = this;
  ctx->set_nodraw();
  ctx->trace_connect_func = flow_ctx->trace_connect_func;
  ctx->trace_disconnect_func = flow_ctx->trace_disconnect_func;
  ctx->trace_scan_func = flow_ctx->trace_scan_func;
  ctx->trace_started = flow_ctx->trace_started;
  ctx->zoom_factor = flow_ctx->zoom_factor;
  if ( type == flow_eCtxType_Brow) {
    ((BrowCtx *)ctx)->scroll_callback = ((BrowCtx *)flow_ctx)->scroll_callback;
    ((BrowCtx *)ctx)->scroll_data = ((BrowCtx *)flow_ctx)->scroll_data;
  }

  init_proc( ctx, client_data);
  *secondary_flow_ctx = (void *) ctx;
  return 1;
}

int FlowDrawXLib::change_ctx( 
	FlowCtx *from_ctx,
	FlowCtx *to_ctx
)
{
  if ( from_ctx->type() == flow_eCtxType_Brow)
    ((BrowWidget)toplevel)->brow.brow_ctx = to_ctx;
  else
    ((FlowWidget)toplevel)->flow.flow_ctx = to_ctx;

  to_ctx->window_width = from_ctx->window_width;
  to_ctx->window_height = from_ctx->window_height;
  from_ctx->set_nodraw();
  to_ctx->reset_nodraw();
  if ( from_ctx->type() == flow_eCtxType_Brow) {
    to_ctx->zoom_factor = from_ctx->zoom_factor;
    ((BrowCtx *)to_ctx)->configure(0);
  }
  else
    to_ctx->redraw();
  return 1;
}

void FlowDrawXLib::delete_secondary_ctx( FlowCtx *ctx)
{
  ctx->set_nodraw();
  delete ctx;
}

FlowDrawXLib::FlowDrawXLib( 
	Widget x_toplevel, 
	void **flow_ctx,
        int (*init_proc)(Widget w, FlowCtx *ctx, void *client_data),
	void  *client_data, 
	flow_eCtxType type) :
    toplevel(x_toplevel), nav_shell(0), 
    nav_toplevel(0), app_ctx(0), display(0),
    window(0), nav_window(0), screen(0), background(0), timer_id(0)
{
  int		i;
  Arg 		args[20];

  memset( gcs, 0, sizeof(gcs));
  // memset( font_struct, 0, sizeof(font_struct));
  memset( font, 0, sizeof(font));
  memset( cursors, 0, sizeof(cursors));

  if ( type == flow_eCtxType_Brow)
    basectx = (FlowCtx *) new BrowCtx("Claes context", 20);
  else
    basectx = new FlowCtx("Claes context", 20);
  *flow_ctx = (void *) basectx;
  
  basectx->fdraw = this;
  toplevel = toplevel;

  display = XtDisplay( toplevel);
  window = XtWindow( toplevel);
  screen = XtScreen( toplevel);

/*
  i = 0;
  XtSetArg(args[i],XmNbackground, XWhitePixelOfScreen(screen)); i++;
  XtSetValues( toplevel ,args,i);
*/
  i = 0;
  XtSetArg(args[i],XmNbackground, &background); i++;
  XtGetValues( toplevel ,args,i);

  flow_create_gc( this, window);

  flow_create_cursor( this);

#if defined IMLIB
  imlib = Imlib_init( display);
#endif

  init_proc( toplevel, basectx, client_data);
}


int FlowDrawXLib::init_nav( Widget nav_widget, void *flow_ctx)
{
  nav_toplevel = nav_widget;
  nav_window = XtWindow( nav_toplevel);

//  flow_create_gc( this, nav_window);

  ((FlowCtx *)flow_ctx)->no_nav = 0;
  return 1;
}


int FlowDrawXLib::event_handler( FlowCtx *ctx, XEvent event)
{
  static int	button_pressed = 0;
  static int	button_clicked = 0;
  static int	button_clicked_and_pressed = 0;
  static int 	button1_pressed = 0;
  static int 	button2_pressed = 0;
  static int 	button3_pressed = 0;
  static int	last_press_x = 0;
  static int	last_press_y = 0;
  int           sts = 1;

  if ( event.xany.window == window) {
    switch ( event.type) {
    case KeyPress : {
      KeySym keysym;
      Modifiers mod;

      XtTranslateKeycode( display, event.xkey.keycode, 0, &mod,
		&keysym);
      keysym &= 0xFFFF;
      switch ( keysym) {
      case XK_Return:
	sts = ctx->event_handler( flow_eEvent_Key_Return, 0, 0, 0, 0);
//            printf( "-- Return key event\n");
	break;
      case XK_Up:
	if ( event.xkey.state & ShiftMask)
	  sts = ctx->event_handler( flow_eEvent_Key_ShiftUp, 0, 0, 0, 0);
	else
	  sts = ctx->event_handler( flow_eEvent_Key_Up, 0, 0, 0, 0);
	break;
      case XK_Down:
	if ( event.xkey.state & ShiftMask)
	  sts = ctx->event_handler( flow_eEvent_Key_ShiftDown, 0, 0, 0, 0);
	else
	  sts = ctx->event_handler( flow_eEvent_Key_Down, 0, 0, 0, 0);
	break;
      case XK_Right:
	if ( event.xkey.state & ShiftMask)
	  sts = ctx->event_handler( flow_eEvent_Key_ShiftRight, 0, 0, 0, 0);
	else
	  sts = ctx->event_handler( flow_eEvent_Key_Right, 0, 0, 0, 0);
	break;
      case XK_Left:
	if ( event.xkey.state & ShiftMask)
	  sts = ctx->event_handler( flow_eEvent_Key_ShiftLeft, 0, 0, 0, 0);
	else
	  sts = ctx->event_handler( flow_eEvent_Key_Left, 0, 0, 0, 0);
	break;
      case XK_Page_Up:
      case 0xFF41:
	sts = ctx->event_handler( flow_eEvent_Key_PageUp, 0, 0, 0, 0);
	break;
      case XK_Page_Down:
      case 0xFF42:
	sts = ctx->event_handler( flow_eEvent_Key_PageDown, 0, 0, 0, 0);
	break;
      case XK_BackSpace:
	sts = ctx->event_handler( flow_eEvent_Key_BackSpace, 0, 0, 0, 0);
	break;
      case XK_KP_F1:
	sts = ctx->event_handler( flow_eEvent_Key_PF1, 0, 0, 0, 0);
	break;
      case XK_KP_F2:
	sts = ctx->event_handler( flow_eEvent_Key_PF2, 0, 0, 0, 0);
	break;
      case XK_KP_F3:
	sts = ctx->event_handler( flow_eEvent_Key_PF3, 0, 0, 0, 0);
	break;
      case XK_KP_F4:
	sts = ctx->event_handler( flow_eEvent_Key_PF4, 0, 0, 0, 0);
	break;
      case XK_Tab:
	sts = ctx->event_handler( flow_eEvent_Key_Tab, 0, 0, 0, 0);
	break;
      default:
	;
      }
      break;
    }
    case ButtonPress : 

      switch ( event.xbutton.button) {
      case Button3:
	ctx->event_handler( flow_eEvent_MB3Down, event.xbutton.x, event.xbutton.y, 0, 0);
	if ( click_sensitivity & flow_mSensitivity_MB3Press && 
	     !(click_sensitivity & flow_mSensitivity_MB3DoubleClick) && 
	     !(click_sensitivity & flow_mSensitivity_MB3Click)) {
	  ctx->event_handler( flow_eEvent_MB3Press, 
				  event.xbutton.x, event.xbutton.y, 0, 0);
	  click_sensitivity = 0;
	  return 1;
	}
	break;
      default:
	;
      }
      /* Detect press or click event */
      if ( button_clicked) {
	/* Wait for release */
	button_clicked_and_pressed = 1;
	cancel_event_timer( ctx);
	button_clicked = 0;
	memcpy( &last_event, &event, sizeof(event));
	button_pressed = event.xbutton.button;
	last_press_x = event.xbutton.x;
	last_press_y = event.xbutton.y;
	event_timer(ctx, 200);
	return 1;
      }
      if ( !button_pressed ) {
	memcpy( &last_event, &event, sizeof(event));
	button_pressed = event.xbutton.button;
	last_press_x = event.xbutton.x;
	last_press_y = event.xbutton.y;
	event_timer(ctx, 200);
	return 1;
      }
      else {    
	/* Press event, callback from timer */
	button_pressed = 0;
	button_clicked_and_pressed = 0;
	switch ( event.xbutton.button) {
	case Button1:
	  button1_pressed = 1;
	  if ( (event.xbutton.state & ShiftMask) && 
	       !(event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( !(event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( (event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else
	    ctx->event_handler( flow_eEvent_MB1Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	case Button2:
	  button2_pressed = 1;
	  if ( (event.xbutton.state & ShiftMask) && 
	       !(event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( !(event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( (event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else
	    sts = ctx->event_handler( flow_eEvent_MB2Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	case Button3:
	  button3_pressed = 1;
#if 0
	  if ( (event.xbutton.state & ShiftMask) && 
	       !(event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB3PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( !(event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB3PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if (  (event.xbutton.state & ShiftMask) && 
		     (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB3PressShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else
#endif
	    sts = ctx->event_handler( flow_eEvent_MB3Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	}
      }
      break;

    case ButtonRelease:
      button1_pressed = 0;
      button2_pressed = 0;
      button3_pressed = 0;

      if ( ! button_pressed ) {
	if ( button_clicked) {
	  /* Button click, callback from timer */
	  button_clicked = 0;
	  switch ( event.xbutton.button) {
	  case Button1:
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1ClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1ClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if (  (event.xbutton.state & ShiftMask) && 
		       (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1ClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
	      sts = ctx->event_handler( flow_eEvent_MB1Click, event.xbutton.x, event.xbutton.y, 0, 0);
	    break;
	  case Button2:
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2ClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2ClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if (  (event.xbutton.state & ShiftMask) && 
		       (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2ClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
	      sts = ctx->event_handler( flow_eEvent_MB2Click, event.xbutton.x, event.xbutton.y, 0, 0);
	    break;
	  case Button3:
#if 0
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3ClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3ClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if (  (event.xbutton.state & ShiftMask) && 
		       (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3ClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
#endif
	      sts = ctx->event_handler( flow_eEvent_MB3Click, event.xbutton.x, event.xbutton.y, 0, 0);
	    break;
	  }
	}
	else {
	  /* Button release */
	  sts = ctx->event_handler( flow_eEvent_ButtonRelease, event.xbutton.x, event.xbutton.y, 0, 0);
	}        
      }
      else {
	/* Button click */
	cancel_event_timer( ctx);
	if ( ! button_clicked_and_pressed) {
	  /* wait for button double click */
	  memcpy( &last_event, &event, sizeof(event));
	  button_clicked = 1;
	  event_timer( ctx, 200);
	  button_pressed = 0;
	  return 1;
	}
	else {
	  /* Button double click */ 
	  cancel_event_timer( ctx);
	  button_clicked = 0;
	  button_pressed = 0;
	  button_clicked_and_pressed = 0;
	  switch ( event.xbutton.button) {
	  case Button1:
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1DoubleClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1DoubleClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if (  (event.xbutton.state & ShiftMask) && 
		       (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB1DoubleClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
	      sts = ctx->event_handler( flow_eEvent_MB1DoubleClick, event.xbutton.x, event.xbutton.y, 0, 0);
	    break;
	  case Button2:
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2DoubleClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2DoubleClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if (  (event.xbutton.state & ShiftMask) && 
		       (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB2DoubleClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
	      sts = ctx->event_handler( flow_eEvent_MB2DoubleClick, event.xbutton.x, event.xbutton.y, 0, 0);
	    break;
	  case Button3:
#if 0
	    if ( (event.xbutton.state & ShiftMask) && 
		 !(event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3DoubleClickShift, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( !(event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3DoubleClickCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else if ( (event.xbutton.state & ShiftMask) && 
		      (event.xbutton.state & ControlMask))
	      sts = ctx->event_handler( flow_eEvent_MB3DoubleClickShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	    else
	      sts = ctx->event_handler( flow_eEvent_MB3DoubleClick, event.xbutton.x, event.xbutton.y, 0, 0);
#endif
	    break;
	  }
	}
      }
      break;
    case Expose:
      sts = ctx->event_handler( flow_eEvent_Exposure, event.xexpose.x, 
		event.xexpose.y, event.xexpose.width, event.xexpose.height);
      break;
    case VisibilityNotify : 
      switch ( event.xvisibility.state) {
      case VisibilityUnobscured:
	sts = ctx->event_handler( flow_eEvent_VisibilityUnobscured, 0, 0, 0, 0);
	break;
      default: 
	sts = ctx->event_handler( flow_eEvent_VisibilityObscured, 0, 0, 0, 0);
	break;
      }
      break;        
    case MotionNotify:
      if ( button3_pressed) {
	button3_pressed = 0;
	button_pressed = 0;
	button_clicked_and_pressed = 0;
      }

      if ( button_pressed && 
	   (abs( event.xbutton.x - last_press_x) > DRAW_PRESS_PIX ||
	    abs( event.xbutton.y - last_press_y) > DRAW_PRESS_PIX)) {

	event.xbutton.x = last_press_x;
	event.xbutton.y = last_press_y;

	/* Button press */
	cancel_event_timer( ctx);
	switch ( button_pressed) {
	case Button1:
	  button1_pressed = 1;
	  if ( (event.xbutton.state & ShiftMask) && 
	       !(event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( !(event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( (event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB1PressShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else
	    ctx->event_handler( flow_eEvent_MB1Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	case Button2:
	  button2_pressed = 1;
	  if ( (event.xbutton.state & ShiftMask) && 
	       !(event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if ( !(event.xbutton.state & ShiftMask) && 
		    (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShift, event.xbutton.x, event.xbutton.y, 0, 0);
	  else if (  (event.xbutton.state & ShiftMask) && 
		     (event.xbutton.state & ControlMask))
	    sts = ctx->event_handler( flow_eEvent_MB2PressShiftCtrl, event.xbutton.x, event.xbutton.y, 0, 0);
	  else
	    sts = ctx->event_handler( flow_eEvent_MB2Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	case Button3:
	  button3_pressed = 1;
	  sts = ctx->event_handler( flow_eEvent_MB3Press, event.xbutton.x, event.xbutton.y, 0, 0);
	  break;
	}
	button_pressed = 0;
	button_clicked_and_pressed = 0;
	
      }
      if ( button1_pressed || button2_pressed || button3_pressed)
	sts = ctx->event_handler( flow_eEvent_ButtonMotion, event.xbutton.x, event.xbutton.y, 0, 0);
      else
	sts = ctx->event_handler( flow_eEvent_CursorMotion, event.xbutton.x, event.xbutton.y, 0, 0);
      break;
    case EnterNotify:
      sts = ctx->event_handler( flow_eEvent_Enter, event.xbutton.x, event.xbutton.y, 0, 0);
      break;          
    case LeaveNotify:
      sts = ctx->event_handler( flow_eEvent_Leave, event.xbutton.x, event.xbutton.y, 0, 0);
      break;          
    case MapNotify:
      sts = ctx->event_handler( flow_eEvent_Map, 0, 0, 0, 0);
      break;          
    case UnmapNotify:
      sts = ctx->event_handler( flow_eEvent_Unmap, 0, 0, 0, 0);
      break;          
    case FocusIn:
      break;          
    default:
      break;
    }
  }
  else if ( event.xany.window == nav_window) {
    switch ( event.type) {
    case ButtonPress : 
      switch ( event.xbutton.button) {
      case Button1:
	button1_pressed = 1;
	sts = ctx->event_handler_nav( flow_eEvent_MB1Press, event.xbutton.x, event.xbutton.y);
	break;
      case Button2:
	button2_pressed = 1;
	sts = ctx->event_handler_nav( flow_eEvent_MB2Press, event.xbutton.x, event.xbutton.y);
	break;
      case Button3:
	button3_pressed = 1;
	break;
      }
      break;          
    case ButtonRelease : 
      switch ( event.xbutton.button) {
      case Button1:
	button1_pressed = 0;
	break;
      case Button2:
	button2_pressed = 0;
	break;
      case Button3:
	button3_pressed = 0;
	break;
      }
      sts = ctx->event_handler_nav( flow_eEvent_ButtonRelease, event.xbutton.x, event.xbutton.y);
      break;          
    case Expose:
      sts = ctx->event_handler_nav( flow_eEvent_Exposure, event.xbutton.x, event.xbutton.y);
      break; 
    case MotionNotify:
      if ( button1_pressed || button2_pressed || button3_pressed)
	sts = ctx->event_handler_nav( flow_eEvent_ButtonMotion, event.xbutton.x, event.xbutton.y);
      else
	sts = ctx->event_handler_nav( flow_eEvent_CursorMotion, event.xbutton.x, event.xbutton.y);
      break;          
    }
  }
  if ( sts != FLOW__DESTROYED)
    XFlush( display);
  return 1;
}

void FlowDrawXLib::enable_event( FlowCtx *ctx, flow_eEvent event, 
		flow_eEventType event_type, 
		int (*event_cb)(FlowCtx *ctx, flow_tEvent event))
{
  ctx->enable_event( event, event_type, event_cb);
}

int FlowDrawXLib::rect( FlowCtx *ctx, int x, int y, int width, int height, 
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawRectangle( display, window, 
	gcs[gc_type+highlight][idx], 
	x, y, width, height);
  return 1;
}

int FlowDrawXLib::rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
	int idx)
{
  if ( ctx->nodraw) return 1;

  XDrawRectangle( display, window, 
	gcs[flow_eDrawType_LineErase][idx], 
	x, y, width, height);
  return 1;
}

int FlowDrawXLib::nav_rect( FlowCtx *ctx, int x, int y, int width, int height,
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawRectangle( display, nav_window,
	gcs[gc_type+highlight][idx], 
	x, y, width, height);
  return 1;
}

int FlowDrawXLib::nav_rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
	int idx)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XDrawRectangle( display, nav_window,
	gcs[flow_eDrawType_LineErase][idx], 
	x, y, width, height);
  return 1;
}

int FlowDrawXLib::arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	int x3, int y3,
	flow_eDrawType gc_type, int idx, int highlight)
{
  XPoint p[4] = {{x1,y1},{x2,y2},{x3,y3},{x1,y1}};
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XFillPolygon( display, window,
	gcs[gc_type+highlight][idx], p, 4, Complex, CoordModeOrigin);
  return 1;
}

int FlowDrawXLib::arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	int x3, int y3,
	int idx)
{
  XPoint p[4] = {{x1,y1},{x2,y2},{x3,y3},{x1,y1}};
  if ( ctx->nodraw) return 1;

  XFillPolygon( display, window,
	gcs[flow_eDrawType_LineErase][idx],
	p, 4, Complex, CoordModeOrigin);
  return 1;
}

int FlowDrawXLib::nav_arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	int x3, int y3,
	flow_eDrawType gc_type, int idx, int highlight)
{
  XPoint p[4] = {{x1,y1},{x2,y2},{x3,y3},{x1,y1}};
  if ( ctx->no_nav || ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XFillPolygon( display, nav_window,
	gcs[gc_type+highlight][idx], p, 4, Complex, CoordModeOrigin);
  return 1;
}

int FlowDrawXLib::nav_arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	int x3, int y3,
	int idx)
{
  XPoint p[4] = {{x1,y1},{x2,y2},{x3,y3},{x1,y1}};
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XFillPolygon( display, nav_window,
	gcs[flow_eDrawType_LineErase][idx],
	p, 4, Complex, CoordModeOrigin);
  return 1;
}

int FlowDrawXLib::arc( FlowCtx *ctx, int x, int y, int width, int height, 
	int angel1, int angel2,
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawArc( display, window,
	gcs[gc_type+highlight][idx], 
	x, y, width, height, angel1*64, angel2*64);
  return 1;
}

int FlowDrawXLib::arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
	int angel1, int angel2,
	int idx)
{
  if ( ctx->nodraw) return 1;

  XDrawArc( display, window,
	gcs[flow_eDrawType_LineErase][idx], 
	x, y, width, height, angel1*64, angel2*64);
  return 1;
}

int FlowDrawXLib::nav_arc( FlowCtx *ctx, int x, int y, int width, int height,
	int angel1, int angel2,
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawArc( display, nav_window,
	gcs[gc_type+highlight][idx], 
	x, y, width, height, angel1*64, angel2*64);
  return 1;
}

int FlowDrawXLib::nav_arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
	int angel1, int angel2,
	int idx)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XDrawArc( display, nav_window,
	gcs[flow_eDrawType_LineErase][idx], 
	x, y, width, height, angel1*64, angel2*64);
  return 1;
}

int FlowDrawXLib::line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawLine( display, window,
	gcs[gc_type+highlight][idx], 
	x1, y1, x2, y2);
  return 1;
}

int FlowDrawXLib::line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	int idx)
{
  if ( ctx->nodraw) return 1;

  XDrawLine( display, window,
	gcs[flow_eDrawType_LineErase][idx], 
	x1, y1, x2, y2);
  return 1;
}

int FlowDrawXLib::nav_line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	flow_eDrawType gc_type, int idx, int highlight)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_LineGray && highlight)
    gc_type = flow_eDrawType_Line;

  XDrawLine( display, nav_window,
	gcs[gc_type+highlight][idx], 
	x1, y1, x2, y2);
  return 1;
}

int FlowDrawXLib::nav_line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	int idx)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XDrawLine( display, nav_window,
	gcs[flow_eDrawType_LineErase][idx], 
	x1, y1, x2, y2);
  return 1;
}

int FlowDrawXLib::text( FlowCtx *ctx, int x, int y, char *text, int len,
	flow_eDrawType gc_type, int idx, int highlight, int line)
{
  if ( ctx->nodraw) return 1;

  XDrawString( display, window,
	gcs[gc_type][idx], 
	x, y, text, len);
  return 1;
}

int FlowDrawXLib::text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
	flow_eDrawType gc_type, int idx, int line)
{
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_TextHelvetica)
    gc_type = flow_eDrawType_TextHelveticaErase;
  else if ( gc_type == flow_eDrawType_TextHelveticaBold)
    gc_type = flow_eDrawType_TextHelveticaEraseBold;
  XDrawString( display, window,
	gcs[gc_type][idx], 
	x, y, text, len);
  return 1;
}

int FlowDrawXLib::nav_text( FlowCtx *ctx, int x, int y, char *text, int len,
	flow_eDrawType gc_type, int idx, int highlight, int line)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XDrawString( display, nav_window,
	gcs[gc_type][idx], 
	x, y, text, len);
  return 1;
}

int FlowDrawXLib::nav_text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
	flow_eDrawType gc_type, int idx, int line)
{
  if ( ctx->no_nav || ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_TextHelvetica)
    gc_type = flow_eDrawType_TextHelveticaErase;
  else if ( gc_type == flow_eDrawType_TextHelveticaBold)
    gc_type = flow_eDrawType_TextHelveticaEraseBold;
  XDrawString( display, nav_window,
	gcs[gc_type][idx], 
	x, y, text, len);
  return 1;
}

int FlowDrawXLib::pixmaps_create( FlowCtx *ctx, flow_sPixmapData *pixmap_data,
	void **pixmaps)
{
  draw_sPixmap *pms;
  flow_sPixmapDataElem *prev_pdata, *pdata = (flow_sPixmapDataElem *)pixmap_data;
  int	i;

  pms = (draw_sPixmap *) calloc( 1, sizeof( *pms));
  for ( i = 0; i < DRAW_PIXMAP_SIZE; i++) {
    if ( i == 0 || 
         (i > 0 && pdata->bits != prev_pdata->bits))
      pms->pixmap[i] = XCreateBitmapFromData( display,
	window, pdata->bits, pdata->width,
	pdata->height);
    else
      pms->pixmap[i] = pms->pixmap[i-1];
    prev_pdata = pdata;
    pdata++;
  }
  *pixmaps = (void *) pms;
  return 1;
}

void FlowDrawXLib::pixmaps_delete( FlowCtx *ctx, void *pixmaps)
{
  draw_sPixmap *pms;
  int i;

  pms = (draw_sPixmap *) pixmaps;
  for ( i = 0; i < DRAW_PIXMAP_SIZE; i++) {
    if ( i == 0 || 
         (i > 0 && pms->pixmap[i] != pms->pixmap[i-1]))
      XFreePixmap( display, pms->pixmap[i]);
  }
  free ( pixmaps);
}

int FlowDrawXLib::pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line)
{
  draw_sPixmap *pms;
  flow_sPixmapDataElem *pdata = (flow_sPixmapDataElem *)pixmap_data + idx;

  if ( ctx->nodraw) return 1;
  
  pms = (draw_sPixmap *) pixmaps;

  XCopyPlane( display, pms->pixmap[idx], window,
	gcs[gc_type][idx], 
	0, 0, pdata->width, pdata->height, x, y, 1);
  return 1;
}

int FlowDrawXLib::pixmap_inverse( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	void *pixmaps, flow_eDrawType gc_type, int idx, int line)
{
  draw_sPixmap *pms;
  flow_sPixmapDataElem *pdata = (flow_sPixmapDataElem *)pixmap_data + idx;

  if ( ctx->nodraw) return 1;
  
  pms = (draw_sPixmap *) pixmaps;

  XCopyPlane( display, pms->pixmap[idx], window,
	gc_inverse,
	0, 0, pdata->width, pdata->height, x, y, 1);
  return 1;
}

int FlowDrawXLib::pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	void *pixmaps, flow_eDrawType gc_type, int idx, int line)
{
  flow_sPixmapDataElem *pdata = (flow_sPixmapDataElem *)pixmap_data + idx;
  if ( ctx->nodraw) return 1;

  XClearArea( display, window, 
	x, y, pdata->width, pdata->height, False);
  return 1;
}

int FlowDrawXLib::nav_pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line)
{
  draw_sPixmap *pms;
  flow_sPixmapDataElem *pdata = (flow_sPixmapDataElem *)pixmap_data + idx;

  if ( ctx->no_nav || ctx->nodraw) return 1;
  
  pms = (draw_sPixmap *) pixmaps;

  XCopyPlane( display, pms->pixmap[idx], nav_window,
	gcs[gc_type][idx], 
	0, 0, pdata->width, pdata->height, x, y, 1);
  return 1;
}

int FlowDrawXLib::nav_pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	void *pixmaps, flow_eDrawType gc_type, int idx, int line)
{
  flow_sPixmapDataElem *pdata = (flow_sPixmapDataElem *)pixmap_data + idx;
  if ( ctx->no_nav || ctx->nodraw) return 1;

  XClearArea( display, nav_window, 
	x, y, pdata->width, pdata->height, False);
  return 1;
}

int FlowDrawXLib::fill_rect( FlowCtx *ctx, int x, int y, int w, int h, 
	flow_eDrawType gc_type)
{
  XPoint p[5] = {{x,y},{x+w,y},{x+w,y+h},{x,y+h},{x,y}};
  if ( ctx->nodraw) return 1;

  if ( gc_type == flow_eDrawType_Green)
    XFillPolygon( display, window,
	gc_green, p, 5, Convex, CoordModeOrigin);
  else if ( gc_type == flow_eDrawType_Yellow)
    XFillPolygon( display, window,
	gc_yellow, p, 5, Convex, CoordModeOrigin);
  else
    XFillPolygon( display, window,
	gcs[gc_type][0], p, 5, Convex, CoordModeOrigin);
  return 1;
}

int FlowDrawXLib::image( FlowCtx *ctx, int x, int y, int width, int height,
			 flow_tImImage im, flow_tPixmap pixmap, flow_tPixmap clip_mask)
{
  if ( ctx->nodraw) return 1;

  if ( width == 0 || height == 0)
    return 1;
  
  if ( clip_mask)
    set_image_clip_mask( ctx, clip_mask, x, y);

  XCopyArea( display, (Pixmap)pixmap, window,
	gcs[flow_eDrawType_Line][0], 
	0, 0, width, height, x, y);

  if ( clip_mask)
    reset_image_clip_mask( ctx);
  return 1;
}

void FlowDrawXLib::clear( FlowCtx *ctx)
{
  if ( ctx->nodraw) return;

  XClearWindow( display, window);
}

void FlowDrawXLib::nav_clear( FlowCtx *ctx)
{
  if ( ctx->no_nav || ctx->nodraw) return;

  XClearWindow( display, nav_window);
}


void FlowDrawXLib::get_window_size( FlowCtx *ctx, int *width, int *height)
{
  XWindowAttributes attr;

  XGetWindowAttributes( display, window, &attr);
  *width = attr.width;
  *height = attr.height;
}

void FlowDrawXLib::get_nav_window_size( FlowCtx *ctx, int *width, int *height)
{
  XWindowAttributes attr;

  if ( ctx->no_nav) return;

  XGetWindowAttributes( display, nav_window, &attr);
  *width = attr.width;
  *height = attr.height;
}

void FlowDrawXLib::set_nav_window_size( FlowCtx *ctx, int width, int height)
{
  XWindowChanges	xwc;

  if ( ctx->no_nav) return;

  xwc.width = width;
  xwc.height = height;

  XConfigureWindow( display, nav_window, CWWidth | CWHeight,
	&xwc);
}

void draw_timer_cb( draw_sTimerCb *timer_cb)
{
  FlowDrawXLib *draw_ctx = (FlowDrawXLib *) timer_cb->ctx->fdraw;

  (timer_cb->callback_func)(timer_cb->ctx);
  XFlush( draw_ctx->display);
  free( timer_cb);
}

static void event_timer_cb( FlowCtx *ctx)
{
  ((FlowDrawXLib *)ctx->fdraw)->event_handler( ctx, last_event);
}

static void cancel_event_timer( FlowCtx *ctx)
{
  FlowDrawXLib *draw_ctx  = (FlowDrawXLib *) ctx->fdraw;
  if ( draw_ctx->timer_id) {
    XtRemoveTimeOut( draw_ctx->timer_id);
    draw_ctx->timer_id = 0;
  }
}

static void event_timer( FlowCtx *ctx, int time_ms)
{
  FlowDrawXLib *draw_ctx = (FlowDrawXLib *) ctx->fdraw;
  draw_ctx->timer_id = XtAppAddTimeOut(
	XtWidgetToApplicationContext(draw_ctx->toplevel) ,time_ms,
	(XtTimerCallbackProc)event_timer_cb, ctx);

}

void FlowDrawXLib::set_timer( FlowCtx *ctx, int time_ms,
  	void (*callback_func)( FlowCtx *ctx), void **id)
{
  draw_sTimerCb *timer_cb;

  timer_cb = (draw_sTimerCb *) calloc( 1, sizeof(draw_sTimerCb));
  timer_cb->ctx = ctx;
  timer_cb->callback_func = callback_func;

  timer_cb->timer_id = XtAppAddTimeOut(
	XtWidgetToApplicationContext(toplevel) ,time_ms,
	(XtTimerCallbackProc)draw_timer_cb, timer_cb);

  *id = (void *)timer_cb;
}

void FlowDrawXLib::cancel_timer( FlowCtx *ctx, void *id)
{
  XtRemoveTimeOut( ((draw_sTimerCb *)id)->timer_id);

  free( (char *) id);
}

void FlowDrawXLib::set_cursor( FlowCtx *ctx, draw_eCursor cursor)
{
  if ( cursor == draw_eCursor_Normal)
    XUndefineCursor( display, window);
  else
    XDefineCursor( display, window, 
		cursors[cursor]);
  XFlush( display);
}

void FlowDrawXLib::set_nav_cursor( FlowCtx *ctx, draw_eCursor cursor)
{

  if ( ctx->no_nav) return;

  if ( cursor == draw_eCursor_Normal)
    XUndefineCursor( display, nav_window);
  else
    XDefineCursor( display, nav_window, 
		cursors[cursor]);
  XFlush( display);
}

int FlowDrawXLib::get_text_extent( FlowCtx *ctx, char *text, int len,
	flow_eDrawType gc_type, int idx,
	int *width, int *height)
{
  int	direction, ascent, descent;
  XCharStruct char_struct;
  draw_eFont font_idx;

  switch( gc_type) {
    case flow_eDrawType_TextHelvetica:
    case flow_eDrawType_TextHelveticaErase:
      font_idx = draw_eFont_Helvetica;
      break;
    case flow_eDrawType_TextHelveticaBold:
    case flow_eDrawType_TextHelveticaEraseBold:
      font_idx = draw_eFont_HelveticaBold;
      break;
    default:
      *width = 0;
      *height = 0;
      return 0;
  }
  
  XTextExtents( font_struct[font_idx][idx], text, len,
	&direction, &ascent, &descent, &char_struct);
  *height = font_struct[font_idx][idx]->ascent + 
  	font_struct[font_idx][idx]->descent;
  *width = char_struct.width;
  return 1;
}

void FlowDrawXLib::copy_area( FlowCtx *ctx, int x, int y)
{
  GC gc;
  if ( ctx->nodraw) return;

  gc = gcs[flow_eDrawType_Line][3];
  if ( x >= 0 && y >= 0) {
    XCopyArea( display, window, window, gc, 
	0, 0, ctx->window_width-x, ctx->window_height-y, x, y);
    if ( x)
      XClearArea( display, window, 0, 0, 
		x, ctx->window_height, 0);
    if ( y)
      XClearArea( display, window, x, 0, 
		ctx->window_width, y, 0);
  }
  else if ( x <= 0 && y <= 0) {
    XCopyArea( display, window, window, gc,
	-x, -y, ctx->window_width+x, ctx->window_height+y, 0, 0);
    if ( x)
      XClearArea( display, window, 
	ctx->window_width+x, 0, ctx->window_width, ctx->window_height, 0);
    if ( y)
      XClearArea( display, window, 
	0, ctx->window_height+y, ctx->window_width+x, ctx->window_height, 0);
  }
  else if ( x <= 0 && y >= 0) {
    XCopyArea( display, window, window, gc,
	-x, 0, ctx->window_width+x, ctx->window_height-y, 0, y);
    if ( x)
      XClearArea( display, window, 
	ctx->window_width+x, 0, ctx->window_width, ctx->window_height, 0);
    if ( y)
      XClearArea( display, window, 
	0, 0, ctx->window_width+x, y, 0);
  }
  else {
    XCopyArea( display, window, window, gc,
	0, -y, ctx->window_width-x, ctx->window_height+y, x, 0);
    if ( x)
      XClearArea( display, window, 
	0, 0, x, ctx->window_height, 0);
    if ( y)
      XClearArea( display, window, 
	x, ctx->window_height+y, ctx->window_width, ctx->window_height, 0);
  }
}

void FlowDrawXLib::clear_area( FlowCtx *ctx, int ll_x, int ur_x, int ll_y, int ur_y)
{
  if ( ctx->nodraw) return;

  XClearArea( display, window, ll_x, ll_y, ur_x - ll_x, 
		ur_y - ll_y, 0);
}

int FlowDrawXLib::create_input( FlowCtx *ctx, int x, int y, char *text, int len,
	int idx, int width, int height, void *node, int number, void **data)
{
  XmFontList fontlist;
  XmFontListEntry entry;
  draw_sAnnotData *annot_data; 
  if ( ctx->nodraw) return 1;

  annot_data = (draw_sAnnotData *) calloc(1, sizeof( draw_sAnnotData));
  entry = XmFontListEntryCreate("tag", XmFONT_IS_FONT, 
		font_struct[0][idx]);
  fontlist = XmFontListAppendEntry( NULL, entry);

  annot_data->w = XtVaCreateWidget( "input", xmTextWidgetClass, toplevel,
	XmNvalue, text, XmNx, x, XmNy, y - height, XmNfontList, fontlist,
	XmNwidth, width + 20, XmNheight, height + 20, NULL);
  XtAddCallback( annot_data->w, XmNactivateCallback, draw_input_cb, annot_data);
  XtManageChild( annot_data->w);
  annot_data->node = (FlowArrayElem *)node;
  annot_data->number = number;
  annot_data->x = x;
  annot_data->y = y;
  annot_data->width = width;
  annot_data->height = height;
  annot_data->ctx = ctx;
  *data = (void *) annot_data;
  XtFree( (char *)entry);
  XmFontListFree( fontlist);
  return 1;
}

int FlowDrawXLib::close_input( FlowCtx *ctx, void *data)
{
  draw_sAnnotData *annot_data = (draw_sAnnotData *)data;
  XtDestroyWidget( annot_data->w);
  free( (char *) annot_data);
  return 1;
}

int FlowDrawXLib::get_input( FlowCtx *ctx, void *data, char **text)
{
  draw_sAnnotData *annot_data = (draw_sAnnotData *)data;
  char *t;

  t = XmTextGetString( annot_data->w);

  *text = (char *) malloc( strlen(t) + 1);
  strcpy( *text, t);
  XtFree( t);
  return 1;
}

static void draw_input_cb( Widget w, XtPointer client_data, XtPointer call_data)
{
  draw_sAnnotData *data = (draw_sAnnotData *)client_data;
  char *text;

  /* Get input value */
  text = XmTextGetString( w);

  /* Call backcall function */
  data->ctx->annotation_input_cb( data->node, data->number, text);
  XtFree( text);
}

void FlowDrawXLib::move_input( FlowCtx *ctx, void *data, int x, int y,
	flow_ePosition pos_type)
{
  draw_sAnnotData *annot_data = (draw_sAnnotData *)data;

  if ( pos_type == flow_ePosition_Relative) {
    if ( !x && !y)
      return;

    annot_data->x += x;
    annot_data->y += y;
  }
  else {
    if ( x == annot_data->x && y == annot_data->y)
      return;

    annot_data->x = x;
    annot_data->y = y;
  }
  XtMoveWidget( annot_data->w, annot_data->x, annot_data->y - annot_data->height);
#if 0
  XtSetArg(args[i],XmNx, text_x + x); i++;
  XtSetArg(args[i],XmNy, text_y + y); i++;
  XtSetValues( annot_data->w, args, i);
#endif
}

void FlowDrawXLib::set_inputfocus( FlowCtx *ctx)
{
  XSetInputFocus( display, window, 
		RevertToParent, CurrentTime);
}

void FlowDrawXLib::set_click_sensitivity( FlowCtx *ctx, int value)
{
  click_sensitivity = value;
}

void FlowDrawXLib::set_image_clip_mask( FlowCtx *ctx, flow_tPixmap pixmap, int x, int y)
{
  XSetClipMask( display, gcs[flow_eDrawType_Line][0], 
		(Pixmap) pixmap);
  XSetClipOrigin( display, gcs[flow_eDrawType_Line][0], x, y);
}

void FlowDrawXLib::reset_image_clip_mask( FlowCtx *ctx)
{
  XSetClipMask( display, gcs[flow_eDrawType_Line][0], 
		None);
  XSetClipOrigin( display, gcs[flow_eDrawType_Line][0], 0, 0);
}

void FlowDrawXLib::set_white_background( FlowCtx *ctx)
{
  XGCValues 		xgcv;
  int			i;
  Arg 			args[5];

  background = XWhitePixelOfScreen(screen);

  // Change erase gcs
  xgcv.foreground =   background;
  xgcv.background =   background;
  for ( i = 0; i < DRAW_TYPE_SIZE; i++) {
    XChangeGC( display, gcs[flow_eDrawType_LineErase][i],
	GCForeground | GCBackground, &xgcv);
    XChangeGC( display, gcs[flow_eDrawType_TextHelveticaErase][i],
	GCForeground | GCBackground, &xgcv);
    XChangeGC( display, gcs[flow_eDrawType_TextHelveticaEraseBold][i],
      GCForeground | GCBackground, &xgcv);
  }
  i = 0;
  XtSetArg(args[i],XmNbackground, background); i++;
  XtSetValues( toplevel, args, i);
  if ( !ctx->no_nav)
    XtSetValues( nav_toplevel, args, i);

}

int FlowDrawXLib::image_get_width( flow_tImImage image)
{
  return ((ImlibImage *)image)->rgb_width;
}

int FlowDrawXLib::image_get_height( flow_tImImage image)
{
  return ((ImlibImage *)image)->rgb_height;
}

void FlowDrawXLib::image_scale( float scale, flow_tImImage *im, flow_tPixmap *im_pixmap,
				flow_tPixmap *im_mask)
{
  int current_width = int( ((ImlibImage *)*im)->rgb_width * scale);
  int current_height = int( ((ImlibImage *)*im)->rgb_height * scale);

  Imlib_render( (ImlibData *)imlib, (ImlibImage *)*im, current_width, current_height);

  if ( *im_pixmap)
    Imlib_free_pixmap( (ImlibData *)imlib, (Pixmap)*im_pixmap);
  *im_pixmap = (flow_tPixmap) Imlib_move_image( (ImlibData *)imlib, (ImlibImage *)*im);
  *im_mask = (flow_tPixmap) Imlib_move_mask( (ImlibData *)imlib, (ImlibImage *)*im);
}

int FlowDrawXLib::image_load( char *imagefile, float scale, float nav_scale,
			      flow_tImImage *orig_im, flow_tImImage *im, 
			      flow_tPixmap *im_pixmap, flow_tPixmap *im_mask,
			      flow_tPixmap *im_nav_pixmap, flow_tPixmap *im_nav_mask)
{
  pwr_tFileName filename;
  bool found = false;

  strcpy( filename, imagefile);

  // Look for file in $pwrp_exe, $pwr_doc/en_us/orm and $pwr_exe
  for ( int i = 0; i < 3; i++) {
    switch( i) {
    case 0: strcpy( filename, "$pwrp_exe/");  break;
    case 1: strcpy( filename, "$pwr_doc/en_us/orm/");  break;
    case 2: strcpy( filename, "$pwr_exe/");  break;
    }
    strcat( filename, imagefile);
    dcli_translate_filename( filename, filename);
    FILE *fp = fopen( filename, "r");
    if ( !fp)
      continue;
    fclose( fp);
    found = true;
    break;
  }
  if ( !found)
    return 0;

#if defined IMLIB
  if ( *im_pixmap) {
    Imlib_free_pixmap( (ImlibData *)imlib, (Pixmap)*im_pixmap);
    *im_pixmap = 0;
  }
  if ( *im_nav_pixmap) {
    Imlib_free_pixmap( (ImlibData *)imlib, (Pixmap)*im_nav_pixmap);  
    *im_nav_pixmap = 0;
  }
  if ( *im)
    Imlib_destroy_image( (ImlibData *)imlib, (ImlibImage *)*im);

  *orig_im = Imlib_load_image( (ImlibData *)imlib, filename);
  if ( !*orig_im) 
    return 0;
  // Make a copy
  *im = Imlib_clone_image( (ImlibData *)imlib, (ImlibImage *)*orig_im);

  int current_width = int( ((ImlibImage *)*im)->rgb_width * scale);
  int current_height = int( ((ImlibImage *)*im)->rgb_height * scale);
  int current_nav_width = int( ((ImlibImage *)*im)->rgb_width * nav_scale);
  int current_nav_height = int( ((ImlibImage *)*im)->rgb_height * nav_scale);

  Imlib_render( (ImlibData *)imlib, (ImlibImage *)*im, current_width, current_height);
  *im_pixmap = (flow_tPixmap) Imlib_move_image( (ImlibData *)imlib, (ImlibImage *)*im);
  *im_mask = (flow_tPixmap) Imlib_move_mask( (ImlibData *)imlib, (ImlibImage *)*im);

  Imlib_render( (ImlibData *)imlib, (ImlibImage *)*im, current_nav_width, current_nav_height);
  *im_nav_pixmap = (flow_tPixmap) Imlib_move_image( (ImlibData *)imlib, (ImlibImage *)*im);
  *im_nav_mask = (flow_tPixmap) Imlib_move_mask( (ImlibData *)imlib, (ImlibImage *)*im);
#endif
  return 1;
}


#if 0
void draw_set_widget_inputfocus( Widget w)
{
  XSetInputFocus( XtDisplay(w), XtWindow(w), 
		RevertToParent, CurrentTime);
}
#endif
