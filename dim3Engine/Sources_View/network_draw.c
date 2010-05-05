/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: HUD Network Routines

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit �Created with dim3 Technology� is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "objects.h"
#include "remotes.h"
#include "consoles.h"
#include "interfaces.h"
#include "effects.h"
#include "video.h"
#include "sounds.h"
#include "timing.h"

extern map_type				map;
extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

extern void chat_time_out(void);

/* =======================================================

      Draw Score Pieces
      
======================================================= */

int network_score_get_list_high(int nscore,int *fnt_sz)
{
	int			high,yadd;
	
		// default height or scale to fit
		
	high=gl_text_get_char_height(hud.font.text_size_large);
	*fnt_sz=hud.font.text_size_large;

	if (nscore!=0) {
		yadd=((hud.scale_y/10)*6)/nscore;
		if (yadd<high) {
			*fnt_sz=(hud.font.text_size_large*yadd)/high;
			high=yadd;
		}
	}
	
	return(high);
}

void network_score_single_name_draw(char *name,int score,int lx,int rx,int ty,int by,d3col *col,int fnt_sz,bool header,bool winner)
{
	int			x,y,k;
	char		txt[256];
	float		alpha,f_flash;
	d3col		col2;
	
		// background
		
	col2.r=col->r*0.7f;
	col2.g=col->g*0.7f;
	col2.b=col->b*0.7f;
	
	alpha=(server.state==gs_score_limit)?1.0f:0.4f;

	view_draw_next_vertex_object_2D_color_poly(lx,ty,col,rx,ty,col,rx,by,&col2,lx,by,&col2,alpha);
	
		// text
		
	if (header) {
		col2.r=col2.g=col2.b=1.0f;
	}
	else {
		col2.r=col2.g=col2.b=0.0f;
	}
	
	sprintf(txt,"%d",score);
		
	gl_text_start(font_hud_index,fnt_sz);
	gl_text_draw((lx+5),(by+1),name,tx_left,FALSE,&col2,1.0f);
	gl_text_draw((rx-5),(by+1),txt,tx_right,FALSE,&col2,1.0f);
	gl_text_end();
	
		// winner status
			
	if (!winner) return;
	
	lx=(lx+5)+gl_text_get_string_width(font_hud_index,fnt_sz,name);
	rx=(rx-5)-gl_text_get_string_width(font_hud_index,fnt_sz,txt);
	
	k=game_time_get_raw()%2000;
	if (k>=1000) k=2000-k;
	f_flash=0.5f*((float)k/1000.0f);
	
	col2.r=f_flash;
	col2.g=f_flash;
	col2.b=f_flash;
	
	x=(lx+rx)>>1;
	y=(ty+by)>>1;
	
	gl_text_start(font_hud_index,((fnt_sz*2)/3));
	gl_text_draw(x,y,"Winner!",tx_center,TRUE,&col2,1.0f);
	gl_text_end();

}

/* =======================================================

      Draw Score
      
======================================================= */

int network_score_players_draw(bool use_teams)
{
	int				n,lx,rx,y,yadd,high,nscore,fnt_sz;
	short			sort_idx[max_object];
	d3col			col;
	obj_type		*obj;

		// sort player scores

	nscore=0;
	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if ((obj->type_idx==object_type_player) || (obj->type_idx==object_type_remote) || (obj->type_idx==object_type_bot_multiplayer)) {
			sort_idx[obj->score.place-1]=n;
			if (obj->score.place>nscore) nscore=obj->score.place;
		}

		obj++;
	}
	
		// sizes

	lx=hud.scale_x>>2;
	rx=lx+(hud.scale_x>>1);
	
	yadd=network_score_get_list_high(nscore,&fnt_sz);
	high=(yadd+3)*nscore;
	
	y=(hud.scale_y-high)>>1;
	
		// player boxes
	
	for (n=0;n!=nscore;n++) {
		obj=&server.objs[sort_idx[n]];
		object_get_tint(obj,&col);
		network_score_single_name_draw(obj->name,obj->score.score,lx,rx,y,(y+yadd),&col,fnt_sz,FALSE,((server.state==gs_score_limit)&&(n==0)));
		y+=(yadd+3);
	}
	
	return(sort_idx[0]);
}

void network_score_teams_draw_single_team(int team_idx,char *team_name,int team_score,d3col *team_col,int lx,int rx,int y,int yadd,int fnt_sz,bool winner)
{
	int				n,nscore;
	short			sort_idx[max_object];
	obj_type		*obj;
	
	network_score_single_name_draw(team_name,team_score,lx,rx,y,(y+yadd),team_col,fnt_sz,TRUE,winner);
	
		// clear scores
		
	for (n=0;n!=server.count.obj;n++) {
		sort_idx[n]=-1;
	}
	
		// sort team scores
		// we need to use -1 because scores on team
		// won't completely fill the list

	nscore=0;

	for (n=0;n!=server.count.obj;n++) {
		obj=&server.objs[n];
		if ((obj->type_idx!=object_type_player) && (obj->type_idx!=object_type_remote) && (obj->type_idx!=object_type_bot_multiplayer)) continue;
		if (obj->team_idx!=team_idx) continue;
		
		sort_idx[obj->score.place-1]=n;
		if (obj->score.place>nscore) nscore=obj->score.place;
	}
	
		// draw the scores
		
	for (n=0;n!=nscore;n++) {
		if (sort_idx[n]!=-1) {
			y+=(yadd+3);
			obj=&server.objs[sort_idx[n]];
			network_score_single_name_draw(obj->name,obj->score.score,lx,rx,y,(y+yadd),team_col,fnt_sz,FALSE,FALSE);
		}
	}
}

int network_score_teams_draw(void)
{
	int				n,lx,rx,y,xadd,xsz,yadd,fnt_sz,
					red_score,blue_score,red_count,blue_count,max_count;
	bool			winner;
	d3col			col;
	obj_type		*obj;

		// add up team scores

	red_score=blue_score=0;
	red_count=blue_count=0;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if ((obj->type_idx!=object_type_player) && (obj->type_idx!=object_type_remote) && (obj->type_idx!=object_type_bot_multiplayer)) {
			obj++;
			continue;
		}
			
		if (obj->team_idx==net_team_red) {
			red_score+=obj->score.score;
			red_count++;
		}
		else {
			blue_score+=obj->score.score;
			blue_count++;
		}

		obj++;
	}
	
	max_count=red_count;
	if (blue_count>max_count) max_count=blue_count;
	
		// box sizes
		
	xadd=hud.scale_x/10;
	xsz=(hud.scale_x-(xadd*3))>>1;
		
	yadd=network_score_get_list_high((max_count+1),&fnt_sz);
	y=(hud.scale_y-((yadd+3)*(max_count+1)))>>1;
	
		// red box
		
	lx=xadd;
	rx=lx+xsz;

	col.r=1.0f;
	col.g=col.b=0.0f;
	
	winner=((server.state==gs_score_limit) && (red_score>blue_score));
	network_score_teams_draw_single_team(net_team_red,"Red Team",red_score,&col,lx,rx,y,yadd,fnt_sz,winner);

		// blue box
		
	lx=rx+xadd;
	rx=lx+xsz;

	col.b=1.0f;
	col.r=col.g=0.0f;
	
	winner=((server.state==gs_score_limit) && (blue_score>red_score));
	network_score_teams_draw_single_team(net_team_blue,"Blue Team",blue_score,&col,lx,rx,y,yadd,fnt_sz,winner);
	
		// return winning team
		
	if (red_score>=blue_score) return(net_team_red);
	
	return(net_team_blue);
}

void network_score_draw(void)
{
	int				y,win_idx;
	char			str[256];
	bool			use_teams;
	d3col			col;
	obj_type		*player_obj;
	
		// never draw if menu is on
		
	if ((view.menu.active) || (view.menu.fade_in) || (view.menu.fade_out)) return;

		// always draw if a dedicated host,
		// the game is at it's score limit,
		// the player is dead or score display
		// is on
		
	if (net_setup.mode!=net_mode_host_dedicated) {
		if (server.state!=gs_score_limit) {
			if (!hud.score.on) {
				player_obj=object_find_uid(server.player_obj_uid);
				if (player_obj->status.health!=0) return;
			}
		}
	}
	
		// calculate place
		
	object_score_recalc_place();
	
		// draw player and team scores
		// if this game is team type

	use_teams=hud.net_game.games[net_setup.game_idx].use_teams;

	if (!use_teams) {
		win_idx=network_score_players_draw(use_teams);
	}
	else {
		win_idx=network_score_teams_draw();
	}
	
		// map message
		
	y=gl_text_get_char_height(hud.font.text_size_large);
		
	col.r=col.g=col.b=1.0f;

	sprintf(str,"%s at %s",hud.net_game.games[net_setup.game_idx].name,map.info.name);
		
	gl_text_start(font_hud_index,hud.font.text_size_large);
	gl_text_draw((hud.scale_x>>1),y,str,tx_center,TRUE,&col,1.0f);
	gl_text_end();
	
		// resuming messages
		
	str[0]=0x0;
		
	if (server.state==gs_score_limit) {
		sprintf(str,"Resuming in %d seconds...",score_limit_get_resume_time());
	}
	else {
		player_obj=object_find_uid(server.player_obj_uid);
		if (player_obj->status.health<=0) {
			sprintf(str,"Respawning in %d seconds...",object_get_respawn_time(player_obj));
		
		}
	}
	
	if (str[0]!=0x0) {
		gl_text_start(font_hud_index,hud.font.text_size_medium);
		gl_text_draw((hud.scale_x>>1),(y+y),str,tx_center,TRUE,&col,1.0f);
		gl_text_end();
	}
}

/* =======================================================

      HUD Network Chats
      
======================================================= */

void network_chat_draw(void)
{
	int					n,x,y,yadd,ntop;
	char				txt[chat_str_len+name_str_len+4];
	hud_chat_line_type	*line;
	d3col				col;

		// dedicated hosts don't chat

	if (net_setup.mode==net_mode_host_dedicated) return;

		// draw position

	yadd=gl_text_get_char_height(hud.font.text_size_small)+1;

	x=hud.chat.x;
	y=hud.chat.y;

		// draw text

	gl_text_start(font_hud_index,hud.font.text_size_small);

		// currently typing?

	if (hud.chat.type_on) {
		col.r=col.g=col.b=1.0f;
		gl_text_draw(x,y,hud.chat.type_str,tx_right,FALSE,&col,1.0f);
		y-=yadd;
	}

		// run chat time outs
		
	chat_time_out();
	
		// history lines

	ntop=0;
	if ((hud.chat.type_on) && (hud.chat.nline>=max_chat_lines)) ntop=1;		// push top line off when typing if over max
	
	for (n=(hud.chat.nline-1);n>=ntop;n--) {
		line=&hud.chat.lines[n];
		sprintf(txt,"%s [%s]",line->str,line->name);
		gl_text_draw(x,y,txt,tx_right,FALSE,&line->col,1.0f);
		y-=yadd;
	}

	gl_text_end();
}

/* =======================================================

      Draw Network HUD
      
======================================================= */

void network_draw(void)
{
	if (net_setup.mode==net_mode_none) return;
	
		// set up view
		
	gl_2D_view_interface();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the score and chat
		
	network_score_draw();
	network_chat_draw();
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}
