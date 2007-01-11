/* 
 * Proview   $Id: ge_gtk.h,v 1.2 2007-01-05 10:36:25 claes Exp $
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

#ifndef ge_gtk_h
#define ge_gtk_h

#ifndef ge_h
#include "ge.h"
#endif

/* ge_gtk.h -- Simple graphic editor */
class CoWowRecall;
class CoWowEntryGtk;

class GeGtk : public Ge {
  GtkWidget	*parent_wid;
  GtkWidget	*grow_widget;
  GtkWidget	*form_widget;
  GtkWidget	*main_pane;
  GtkWidget	*palette_pane;
  GtkWidget	*colorpalette_widget;
  GtkWidget	*colpal_main_widget;
  GtkWidget	*plant_widget;
  GtkWidget	*subpalette_widget;
  GtkWidget	*subgraphs_widget;
  GtkWidget	*toplevel;
  GtkWidget	*india_widget;
  GtkWidget	*india_label;
  GtkWidget	*india_text;
  GtkWidget	*cursor_position;
  GtkWidget	*msg_label;
  GtkWidget	*cmd_prompt;
  GtkWidget	*cmd_input;
  GtkWidget	*graph_form;
  GtkWidget	*confirm_widget;
  GtkWidget	*confirm_label;
  GtkWidget	*yesnodia_widget;
  GtkWidget	*yesnodia_label;
  GtkWidget	*grid_on_w;
  GtkWidget	*grid_size_w;
  GtkWidget	*grid_size_10_w;
  GtkWidget	*grid_size_05_w;
  GtkWidget	*grid_size_02_w;
  GtkWidget	*grid_size_01_w;
  GtkWidget	*show_grid_w;
  GtkWidget	*view_plant_w;
  GdkAtom	graph_atom;
  CoWowRecall	*text_recall;
  CoWowRecall	*name_recall;
  CoWowRecall	*value_recall;
  CoWowRecall	*cmd_recall;
  CoWowEntryGtk *recall_entry;

 public:
  GeGtk( void *parent_ctx, GtkWidget *parent_widget,
      ldh_tSesContext ldhses, int exit_when_close,
      char *graph_name);
  ~GeGtk();

  virtual void set_title( char *title);
  virtual void open_input_dialog( char *text, char *title,
				  char *init_text,
				  void (*india_ok_cb)( Ge *, char *));
  virtual void message( char severity, char *message);
  virtual void status_msg( char *pos_str);
  virtual void open_yesnodia( char *text, char *title, 
			      void (*yes_cb)( Ge *), void (*no_cb)( Ge *));
  virtual void set_prompt( char *prompt);
  virtual void subgraphs_new();
  virtual void update();
  virtual int get_plant_select( char *name);
  virtual void create_list( char *title, char *texts,
			    void (action_cb)( void *, char *), void *ctx);

  static void valchanged_cmd_input( GtkWidget *w, gpointer data);
  static void change_text_cb( void *ge_ctx, void *text_object, char *text);
  static void change_name_cb( void *ge_ctx, void *text_object, char *text);
  static void change_value_cb( void *ge_ctx, void *value_object, char *text);
  static void confirm_cb( void *ge_ctx, void *confirm_object, char *text);

  static void create_cursor_position( GtkWidget *w, gpointer gectx);
  static void create_msg_label( GtkWidget *w, gpointer gectx);
  static void create_cmd_prompt( GtkWidget *w, gpointer gectx);
  static void create_cmd_input( GtkWidget *w, gpointer gectx);
  static void create_graph_form( GtkWidget *w, gpointer gectx);
  static void create_main_pane( GtkWidget *w, gpointer gectx);
  static void create_palette_pane( GtkWidget *w, gpointer gectx);
  static void create_widget_cb( GtkWidget *w, gpointer gectx);
  static void create_india_label( GtkWidget *w, gpointer gectx);
  static void create_india_text( GtkWidget *w, gpointer gectx);

  static void activate_change_text( GtkWidget *w, gpointer gectx);
  static void activate_change_name( GtkWidget *w, gpointer gectx);
  static void activate_preview_start( GtkWidget *w, gpointer gectx);
  static void activate_preview_stop( GtkWidget *w, gpointer gectx);
  static void activate_cut( GtkWidget *w, gpointer gectx);
  static void activate_copy( GtkWidget *w, gpointer gectx);
  static void activate_rotate( GtkWidget *w, gpointer gectx);
  static void activate_rotate90( GtkWidget *w, gpointer gectx);
  static void activate_flip_vert( GtkWidget *w, gpointer gectx);
  static void activate_flip_horiz( GtkWidget *w, gpointer gectx);
  static void activate_pop( GtkWidget *w, gpointer gectx);
  static void activate_push( GtkWidget *w, gpointer gectx);
  static void activate_edit_polyline( GtkWidget *w, gpointer gectx);
  static void activate_scale_equal( GtkWidget *w, gpointer gectx);
  static void activate_move_horizontal( GtkWidget *w, gpointer gectx);
  static void activate_move_vertical( GtkWidget *w, gpointer gectx);
  static void activate_move_reset( GtkWidget *w, gpointer gectx);
  static void activate_align_horiz_up( GtkWidget *w, gpointer gectx);
  static void activate_align_horiz_down( GtkWidget *w, gpointer gectx);
  static void activate_align_horiz_center( GtkWidget *w, gpointer gectx);
  static void activate_align_vert_left( GtkWidget *w, gpointer gectx);
  static void activate_align_vert_right( GtkWidget *w, gpointer gectx);
  static void activate_align_vert_center( GtkWidget *w, gpointer gectx);
  static void activate_equid_vert_up( GtkWidget *w, gpointer gectx);
  static void activate_equid_vert_down( GtkWidget *w, gpointer gectx);
  static void activate_equid_vert_center( GtkWidget *w, gpointer gectx);
  static void activate_equid_horiz_left( GtkWidget *w, gpointer gectx);
  static void activate_equid_horiz_right( GtkWidget *w, gpointer gectx);
  static void activate_equid_horiz_center( GtkWidget *w, gpointer gectx);
  static void activate_select_cons( GtkWidget *w, gpointer gectx);
  static void activate_select_objects( GtkWidget *w, gpointer gectx);
  static void activate_group( GtkWidget *w, gpointer gectx);
  static void activate_ungroup( GtkWidget *w, gpointer gectx);
  static void activate_connect( GtkWidget *w, gpointer gectx);
  static void activate_connectsecond( GtkWidget *w, gpointer gectx);
  static void activate_objectattributes( GtkWidget *w, gpointer gectx);
  static void activate_show_grid( GtkWidget *w, gpointer gectx);
  static void activate_paste( GtkWidget *w, gpointer gectx);
  static void activate_command( GtkWidget *w, gpointer gectx);
  static void activate_exit( GtkWidget *w, gpointer gectx);
  static void activate_print( GtkWidget *w, gpointer gectx);
  static void activate_new( GtkWidget *w, gpointer gectx);
  static void activate_save( GtkWidget *w, gpointer gectx);
  static void activate_save_as( GtkWidget *w, gpointer gectx);
  static void activate_export_javabean( GtkWidget *w, gpointer gectx);
  static void activate_export_javabean_as( GtkWidget *w, gpointer gectx);
  static void activate_export_gejava( GtkWidget *w, gpointer gectx);
  static void activate_export_gejava_as( GtkWidget *w, gpointer gectx);
  static void activate_export_java( GtkWidget *w, gpointer gectx);
  static void activate_export_java_as( GtkWidget *w, gpointer gectx);
  static void activate_generate_web( GtkWidget *w, gpointer gectx);
  static void activate_creanextpage( GtkWidget *w, gpointer gectx);
  static void activate_nextpage( GtkWidget *w, gpointer gectx);
  static void activate_prevpage( GtkWidget *w, gpointer gectx);
  static void activate_graph_attr( GtkWidget *w, gpointer gectx);
  static void activate_open( GtkWidget *w, gpointer gectx);
  static void activate_subgraphs( GtkWidget *w, gpointer gectx);
  static void activate_rect( GtkWidget *w, gpointer gectx);
  static void activate_rectrounded( GtkWidget *w, gpointer gectx);
  static void activate_line( GtkWidget *w, gpointer gectx);
  static void activate_polyline( GtkWidget *w, gpointer gectx);
  static void activate_circle( GtkWidget *w, gpointer gectx);
  static void activate_text( GtkWidget *w, gpointer gectx);
  static void activate_annot( GtkWidget *w, gpointer gectx);
  static void activate_conpoint( GtkWidget *w, gpointer gectx);
  static void activate_fill( GtkWidget *w, gpointer gectx);
  static void activate_border( GtkWidget *w, gpointer gectx);
  static void activate_shadow( GtkWidget *w, gpointer gectx);
  static void activate_incr_lightness( GtkWidget *w, gpointer gectx);
  static void activate_decr_lightness( GtkWidget *w, gpointer gectx);
  static void activate_incr_intensity( GtkWidget *w, gpointer gectx);
  static void activate_decr_intensity( GtkWidget *w, gpointer gectx);
  static void activate_incr_shift( GtkWidget *w, gpointer gectx);
  static void activate_decr_shift( GtkWidget *w, gpointer gectx);
  static void activate_scale( GtkWidget *w, gpointer gectx);
  static void activate_grid( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_1( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_2( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_3( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_4( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_5( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_6( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_7( GtkWidget *w, gpointer gectx);
  static void activate_linewidth_8( GtkWidget *w, gpointer gectx);
  static void activate_linetype1( GtkWidget *w, gpointer gectx);
  static void activate_linetype2( GtkWidget *w, gpointer gectx);
  static void activate_linetype3( GtkWidget *w, gpointer gectx);
  static void activate_linetype4( GtkWidget *w, gpointer gectx);
  static void activate_linetype5( GtkWidget *w, gpointer gectx);
  static void activate_linetype6( GtkWidget *w, gpointer gectx);
  static void activate_linetype7( GtkWidget *w, gpointer gectx);
  static void activate_gridsize_4( GtkWidget *w, gpointer gectx);
  static void activate_gridsize_3( GtkWidget *w, gpointer gectx);
  static void activate_gridsize_2( GtkWidget *w, gpointer gectx);
  static void activate_gridsize_1( GtkWidget *w, gpointer gectx);
  static void activate_textsize_0( GtkWidget *w, gpointer gectx);
  static void activate_textsize_1( GtkWidget *w, gpointer gectx);
  static void activate_textsize_2( GtkWidget *w, gpointer gectx);
  static void activate_textsize_3( GtkWidget *w, gpointer gectx);
  static void activate_textsize_4( GtkWidget *w, gpointer gectx);
  static void activate_textsize_5( GtkWidget *w, gpointer gectx);
  static void activate_textbold( GtkWidget *w, gpointer gectx);
  static void activate_zoom_in( GtkWidget *w, gpointer gectx);
  static void activate_zoom_out( GtkWidget *w, gpointer gectx);
  static void activate_zoom_reset( GtkWidget *w, gpointer gectx);
  static void activate_view_plant( GtkWidget *w, gpointer gectx);
  static void activate_concorner_right( GtkWidget *w, gpointer gectx);
  static void activate_concorner_rounded( GtkWidget *w, gpointer gectx);
  static void activate_round_amount_1( GtkWidget *w, gpointer gectx);
  static void activate_round_amount_2( GtkWidget *w, gpointer gectx);
  static void activate_round_amount_3( GtkWidget *w, gpointer gectx);
  static void activate_round_amount_4( GtkWidget *w, gpointer gectx);
  static void activate_round_amount_5( GtkWidget *w, gpointer gectx);
  static void activate_contype_straight( GtkWidget *w, gpointer gectx);
  static void activate_contype_routed( GtkWidget *w, gpointer gectx);
  static void activate_contype_stronearr( GtkWidget *w, gpointer gectx);
  static void activate_contype_stepdiv( GtkWidget *w, gpointer gectx);
  static void activate_contype_stepconv( GtkWidget *w, gpointer gectx);
  static void activate_contype_transdiv( GtkWidget *w, gpointer gectx);
  static void activate_contype_transconv( GtkWidget *w, gpointer gectx);
  static void activate_condir_center( GtkWidget *w, gpointer gectx);
  static void activate_condir_left( GtkWidget *w, gpointer gectx);
  static void activate_condir_right( GtkWidget *w, gpointer gectx);
  static void activate_condir_up( GtkWidget *w, gpointer gectx);
  static void activate_condir_down( GtkWidget *w, gpointer gectx);
  static void activate_background_color( GtkWidget *w, gpointer gectx);
  static void activate_help( GtkWidget *w, gpointer gectx);
  static void activate_india_ok( GtkWidget *w, gpointer gectx);
  static void activate_india_cancel( GtkWidget *w, gpointer gectx);
  static void activate_yesnodia_yes( GtkWidget *w, gpointer gectx);
  static void activate_yesnodia_no( GtkWidget *w, gpointer gectx);
  static void activate_yesnodia_cancel( GtkWidget *w, gpointer gectx);
  static void activate_confirm_ok( GtkWidget *w, gpointer gectx);
  static void activate_confirm_cancel( GtkWidget *w, gpointer gectx);
  static gboolean ge_action_inputfocus( GtkWidget *w, GdkEvent *event, gpointer data);
};

#endif