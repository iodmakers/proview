/* 
 * Proview   $Id: flow_draw_gtk.h,v 1.1 2007-01-04 07:56:44 claes Exp $
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

#ifndef flow_draw_gtk_h
#define flow_draw_gtk_h

#include <stdlib.h>

#include <libgnome.h>
#include <libgnomeui/libgnomeui.h>
#include <gtk/gtkprivate.h>
#include "flow_draw.h"

class FlowDrawGtk : public FlowDraw {
 public:
  GtkWidget  	*toplevel;
  GtkWidget	*nav_shell;
  GtkWidget	*nav_toplevel; 
  GdkDisplay	*display;
  GdkWindow	*window;
  GdkWindow	*nav_window;
  GdkScreen	*screen;
  GdkGC		*gc;
  GdkGC		*gc_erase;
  GdkGC		*gc_inverse;
  GdkGC		*gc_yellow;
  GdkGC		*gc_green;
  GdkGC		*gcs[flow_eDrawType__][DRAW_TYPE_SIZE];
  // XFontStruct	*font_struct[draw_eFont__][DRAW_FONT_SIZE];
  GdkFont 	*font[draw_eFont__][DRAW_FONT_SIZE];
  GdkColormap 	*colormap;
  GdkCursor	*cursors[draw_eCursor__];
  GdkColor 	background;
  GdkColor 	foreground;
  guint		timer_id;

  FlowDrawGtk( GtkWidget *toplevel, 
		void **flow_ctx,
		int (*init_proc)(GtkWidget *w, FlowCtx *ctx, void *client_data),
		void  *client_data, 
		flow_eCtxType type);
  ~FlowDrawGtk();
  int init_nav( GtkWidget *nav_widget, void *flow_ctx);
  int event_handler( FlowCtx *ctx, GdkEvent event);
  void	enable_event( FlowCtx *ctx, flow_eEvent event, 
		flow_eEventType event_type, 
		int (*event_cb)(FlowCtx *ctx, flow_tEvent event));
  void clear( FlowCtx *ctx);
  void nav_clear( FlowCtx *ctx);

  void get_window_size( FlowCtx *ctx, int *width, int *height);
  void get_nav_window_size( FlowCtx *ctx, int *width, int *height);
  void set_nav_window_size( FlowCtx *ctx, int width, int height);

  int rect( FlowCtx *ctx, int x, int y, int width, int height, 
	    flow_eDrawType gc_type, int idx, int highlight);
  int rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
		  int idx);
  int nav_rect( FlowCtx *ctx, int x, int y, int width, int height,
		flow_eDrawType gc_type, int idx, int highlight);
  int nav_rect_erase( FlowCtx *ctx, int x, int y, int width, int height,
		      int idx);
  int arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
	     int x3, int y3,
	     flow_eDrawType gc_type, int idx, int highlight);
  int arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		   int x3, int y3,
		   int idx);
  int nav_arrow( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		 int x3, int y3,
		 flow_eDrawType gc_type, int idx, int highlight);
  int nav_arrow_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2, 
		       int x3, int y3,
		       int idx);
  int arc( FlowCtx *ctx, int x, int y, int width, int height, 
	   int angel1, int angel2,
	   flow_eDrawType gc_type, int idx, int highlight);
  int arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
		 int angel1, int angel2,
		 int idx);
  int nav_arc( FlowCtx *ctx, int x, int y, int width, int height,
	       int angel1, int angel2,
	       flow_eDrawType gc_type, int idx, int highlight);
  int nav_arc_erase( FlowCtx *ctx, int x, int y, int width, int height,
		     int angel1, int angel2,
		     int idx);
  int line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
	    flow_eDrawType gc_type, int idx, int highlight);
  int line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		  int idx);
  int nav_line( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		flow_eDrawType gc_type, int idx, int highlight);
  int nav_line_erase( FlowCtx *ctx, int x1, int y1, int x2, int y2,
		      int idx);
  int text( FlowCtx *ctx, int x, int y, char *text, int len,
	    flow_eDrawType gc_type, int idx, int highlight, int line);
  int text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
		  flow_eDrawType gc_type, int idx, int line);
  int nav_text( FlowCtx *ctx, int x, int y, char *text, int len,
		flow_eDrawType gc_type, int idx, int highlight, int line);
  int nav_text_erase( FlowCtx *ctx, int x, int y, char *text, int len,
		      flow_eDrawType gc_type, int idx, int line);
  int fill_rect( FlowCtx *ctx, int x, int y, int width, int height, 
		 flow_eDrawType gc_type);
  int image( FlowCtx *ctx, int x, int y, int width, int height,
	     flow_tImImage image, flow_tPixmap pixmap, flow_tPixmap clip_mask);
  int pixmaps_create( FlowCtx *ctx, flow_sPixmapData *pixmap_data,
		      void **pixmaps);
  void pixmaps_delete( FlowCtx *ctx, void *pixmaps);
  int pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
	      void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line);
  int pixmap_inverse( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		      void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  int pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		    void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  int nav_pixmap( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
		  void *pixmaps, flow_eDrawType gc_type, int idx, int highlight, int line);
  int nav_pixmap_erase( FlowCtx *ctx, int x, int y, flow_sPixmapData *pixmap_data,
			void *pixmaps, flow_eDrawType gc_type, int idx, int line);
  
  void set_timer( FlowCtx *ctx, int time_ms,
		       void (*callback_func)( FlowCtx *ctx), void **id);
  void cancel_timer( FlowCtx *ctx, void *id);
  void set_cursor( FlowCtx *ctx, draw_eCursor cursor);
  void set_nav_cursor( FlowCtx *ctx, draw_eCursor cursor);
  int get_text_extent( FlowCtx *ctx, char *text, int len,
			    flow_eDrawType gc_type, int idx,
			    int *width, int *height);
  void copy_area( FlowCtx *ctx, int x, int y);
  void clear_area( FlowCtx *ctx, int ll_x, int ur_x, int ll_y, int ur_y);
  int create_input( FlowCtx *ctx, int x, int y, char *text, int len,
		    int idx, int width, int height, void *node, int number, void **data)
    {return 1;}
  int close_input( FlowCtx *ctx, void *data) {return 1;}
  int get_input( FlowCtx *ctx, void *data, char **text) {return 1;}
  void move_input( FlowCtx *ctx, void *data, int x, int y,
		    flow_ePosition pos_type) {}
  void delete_secondary_ctx( FlowCtx *ctx);
  int create_secondary_ctx( 
			   FlowCtx *flow_ctx,
			   void **secondary_flow_ctx,
			   int (*init_proc)(FlowCtx *, void *),
			   void  *client_data, 
			   flow_eCtxType type);
  int change_ctx( 
		 FlowCtx *from_ctx,
		 FlowCtx *to_ctx);
  void set_inputfocus( FlowCtx *ctx);
  void set_click_sensitivity( FlowCtx *ctx, int value);
  void set_image_clip_mask( FlowCtx *ctx, flow_tPixmap pixmap, int x, int y);
  void reset_image_clip_mask( FlowCtx *ctx);
  void set_white_background( FlowCtx *ctx);
  int get_font_idx( int gc_type);
  int image_get_width( flow_tImImage image);
  int image_get_height( flow_tImImage image);
  void image_scale( float scale, flow_tImImage orig_im, flow_tImImage *im, 
		    flow_tPixmap *im_pixmap,
		    flow_tPixmap *im_mask);
  int image_load( char *imagefile, float scale, float nav_scale,
		  flow_tImImage *orig_im, flow_tImImage *im, 
		  flow_tPixmap *im_pixmap, flow_tPixmap *im_mask,
		  flow_tPixmap *im_nav_pixmap, flow_tPixmap *im_nav_mask);
};

#endif