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

extern server_type			server;
extern view_type			view;
extern hud_type				hud;
extern setup_type			setup;
extern network_setup_type	net_setup;
extern render_info_type		render_info;

extern float				team_color_tint[net_team_count][3];
extern char					setup_team_color_list[][32];

extern void chat_time_out(int tick);

/* =======================================================

      Draw Score Pieces
      
======================================================= */

void network_score_single_box_draw(int lx,int rx,int y,int yadd,d3col *team_col)
{
	int			x,x2,y2;
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glColor4f(team_col->r,team_col->g,team_col->b,0.4f);
	
	x=lx;
	x2=rx;
	y2=y;
	y=y2-yadd;
	
	glBegin(GL_QUADS);
	glVertex2i((x-2),(y-1));
	glVertex2i((x2+2),(y-1));
	glVertex2i((x2+2),(y2+1));
	glVertex2i((x-2),(y2+1));
	glEnd();
}

void network_score_single_name_draw(char *name,int score,int lx,int rx,int y)
{
	char		txt[256];
	d3col		col;
	
	col.r=col.g=col.b=0.0f;
	sprintf(txt,"%d",score);
		
	gl_text_start(FALSE);
	gl_text_draw(lx,(y+1),name,tx_left,FALSE,&col,0.75f);
	gl_text_draw(rx,(y+1),txt,tx_right,FALSE,&col,0.75f);
	gl_text_end();
}

/* =======================================================

      Draw Score
      
======================================================= */

void network_score_players_draw(void)
{
	int				n,k,lx,rx,y,y2,yadd,high,nscore,idx,sz;
	short			s_score,
					sort_idx[max_object],sort_score[max_object];
	d3col			col;
	obj_type		*obj;

		// sort player scores

	nscore=0;
	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if ((obj->player) || (obj->remote.on)) {

			s_score=(short)obj->score.score;
			
			idx=nscore;

			for (k=0;k!=nscore;k++) {
				if (s_score>sort_score[k]) {
					idx=k;
					break;
				}
			}

			if (idx<nscore) {
				sz=sizeof(short)*(nscore-idx);
				memmove(&sort_idx[idx+1],&sort_idx[idx],sz);
				memmove(&sort_score[idx+1],&sort_score[idx],sz);
			}

			sort_idx[idx]=(short)n;
			sort_score[idx]=s_score;
			nscore++;

		}

		obj++;
	}

		// sizes

	lx=hud.scale_x>>4;
	rx=(hud.scale_x>>1)-(hud.scale_x>>5);
		
	yadd=gl_text_get_char_height(FALSE);
	high=(yadd+3)*nscore;
	y=((hud.scale_y-high)>>1)+(yadd+3);

		// header

	col.r=col.g=col.b=1.0f;
	gl_text_start(FALSE);
	gl_text_draw(lx,(y-(yadd+3)),"Players",tx_left,FALSE,&col,1.0f);
	gl_text_end();
	
		// player boxes
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	y2=y;

	for (n=0;n!=nscore;n++) {

		obj=&server.objs[sort_idx[n]];
	
		if (obj->remote.on) {
			remote_get_ui_color(obj,&col);
			network_score_single_box_draw(lx,rx,y2,yadd,&col);
		}
		else {
			player_get_ui_color(&col);
			network_score_single_box_draw(lx,rx,y2,yadd,&col);
		}

		y2+=(yadd+3);
	}

		// player scores
		
	for (n=0;n!=nscore;n++) {

		obj=&server.objs[sort_idx[n]];
		network_score_single_name_draw(obj->name,obj->score.score,lx,rx,y);

		y+=(yadd+3);
	}
}

void network_score_teams_draw(void)
{
	int				n,k,lx,rx,y,y2,yadd,high,nscore,nplayer,idx,sz;
	short			s_score,team_score[net_team_count],
					sort_idx[net_team_count],sort_score[net_team_count];
	bool			team_on[net_team_count];
	d3col			col;
	obj_type		*obj;

		// clear team scores

	for (n=0;n!=net_team_count;n++) {
		team_on[n]=FALSE;
		team_score[n]=0;
	}

		// add up team scores

	nplayer=0;

	obj=server.objs;

	for (n=0;n!=server.count.obj;n++) {

		if ((obj->player) || (obj->remote.on)) {
			team_on[obj->team_idx]=TRUE;
			team_score[obj->team_idx]+=(short)obj->score.score;
			nplayer++;
		}

		obj++;
	}

		// sort team scores

	nscore=0;

	for (n=0;n!=net_team_count;n++) {

		if (!team_on[n]) continue;

		s_score=team_score[n];
			
		idx=nscore;

		for (k=0;k!=nscore;k++) {
			if (s_score>sort_score[k]) {
				idx=k;
				break;
			}
		}

		if (idx<nscore) {
			sz=sizeof(short)*(nscore-idx);
			memmove(&sort_idx[idx+1],&sort_idx[idx],sz);
			memmove(&sort_score[idx+1],&sort_score[idx],sz);
		}

		sort_idx[idx]=(short)n;
		sort_score[idx]=s_score;
		nscore++;
	}

		// sizes

	lx=(hud.scale_x>>1)+(hud.scale_x>>5);
	rx=hud.scale_x-(hud.scale_x>>4);
		
	yadd=gl_text_get_char_height(FALSE);
	high=(yadd+3)*nplayer;
	y=((hud.scale_y-high)>>1)+(yadd+3);

		// header

	col.r=col.g=col.b=1.0f;
	gl_text_start(FALSE);
	gl_text_draw(rx,(y-(yadd+3)),"Teams",tx_right,FALSE,&col,1.0f);
	gl_text_end();
	
		// team boxes
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	y2=y;
	
	for (n=0;n!=nscore;n++) {

		idx=sort_idx[n];

		col.r=team_color_tint[idx][0];
		col.g=team_color_tint[idx][1];
		col.b=team_color_tint[idx][2];

		network_score_single_box_draw(lx,rx,y2,yadd,&col);

		y2+=(yadd+3);
	}

		// team scores
		
	for (n=0;n!=nscore;n++) {

		idx=sort_idx[n];
		network_score_single_name_draw(setup_team_color_list[idx],(int)sort_score[n],lx,rx,y);

		y+=(yadd+3);
	}
}

void network_score_draw(void)
{
	obj_type		*player_obj;

		// only draw if player is dead or
		// score set to show
		
	if (!hud.score.on) {
		player_obj=object_find_uid(server.player_obj_uid);
		if (player_obj->status.health!=0) return;
	}
	
		// draw player and team scores

	network_score_players_draw();
	network_score_teams_draw();
}

/* =======================================================

      HUD Network Chats
      
======================================================= */

void network_chat_draw(int tick)
{
	int					n,x,y,yadd,ntop;
	char				txt[chat_str_len+name_str_len+4];
	hud_chat_line_type	*line;
	d3col			col;

		// draw position

	yadd=gl_text_get_char_height(TRUE)+1;

	x=hud.chat.x;
	y=hud.chat.y;

		// draw text

	gl_text_start(TRUE);

		// currently typing?

	if (hud.chat.type_on) {
		col.r=col.g=col.b=1.0f;
		gl_text_draw(x,y,hud.chat.type_str,tx_right,FALSE,&col,1.0f);
		y-=yadd;
	}

		// run chat time outs
		
	chat_time_out(tick);
	
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

void network_draw(int tick)
{
	if (!net_setup.client.joined) return;
	
		// set up view
		
	gl_setup_viewport(console_y_offset());
	gl_2D_view_interface();
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glDisable(GL_DEPTH_TEST);
	
		// draw the score and chat
		
	network_score_draw();
	network_chat_draw(tick);
	
		// reset any color changes
		
	glColor4f(0.0f,0.0f,0.0f,1.0f);
}

