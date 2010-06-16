/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Game Save/Load

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit ÒCreated with dim3 TechnologyÓ is given on a single
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
#include "scripts.h"
#include "interfaces.h"
#include "models.h"
#include "inputs.h"
#include "timing.h"

typedef struct		{
						int					tick;
						char				version[16],map_name[name_str_len];
					} file_save_header;					

extern map_type			map;
extern hud_type			hud;
extern camera_type		camera;
extern view_type		view;
extern server_type		server;
extern js_type			js;
extern setup_type		setup;

unsigned long			game_file_sz,game_file_pos;
char					game_file_last_save_name[256];
unsigned char			*game_file_data;

extern void view_capture_draw(char *path);
extern void group_moves_synch_with_load(void);
extern void view_draw_fade_cancel(void);
extern void view_game_reset_timing(void);
extern void view_clear_fps(void);
extern void rain_reset(void);

/* =======================================================

      Initialize and Last Saves
      
======================================================= */

void game_file_initialize(void)
{
	game_file_last_save_name[0]=0x0;
}

/* =======================================================

      Chunks
      
======================================================= */

bool game_file_add_chunk(void *data,int count,int sz)
{
	int				nxt_file_sz;
	unsigned char	*nptr;
	
	sz*=count;
	nxt_file_sz=game_file_sz+(sz+(sizeof(int)*2));

	nptr=realloc(game_file_data,nxt_file_sz);
	if (nptr==NULL) return(FALSE);
	
	game_file_data=nptr;

	memmove((game_file_data+game_file_sz),&count,sizeof(int));
	game_file_sz+=sizeof(int);

	memmove((game_file_data+game_file_sz),&sz,sizeof(int));
	game_file_sz+=sizeof(int);
	
	memmove((game_file_data+game_file_sz),data,sz);
	game_file_sz+=sz;
	
	return(TRUE);
}

char* game_file_get_chunk(void *data)
{
	int			count,sz;
	
	memmove(&count,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);

	memmove(&sz,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);
	
	memmove(data,(game_file_data+game_file_pos),sz);
	game_file_pos+=sz;
	
	return(data);
}

unsigned char* game_file_replace_chunk(void)
{
	int				count,sz;
	unsigned char	*data;
	
	memmove(&count,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);

	memmove(&sz,(game_file_data+game_file_pos),sizeof(int));
	game_file_pos+=sizeof(int);
	
	data=malloc(sz);
	if (data==NULL) return(NULL);
	
	memmove(data,(game_file_data+game_file_pos),sz);
	game_file_pos+=sz;
	
	return(data);
}

/* =======================================================

      Files
      
======================================================= */

bool game_file_compress_save(char *path,char *err_str)
{
	unsigned long   compress_sz;
	ptr				compress_data;
	FILE			*file;
	
		// compress it

	compress_data=zip_compress(game_file_data,game_file_sz,&compress_sz,err_str);
	if (compress_data==NULL) return(FALSE);
	
		// save file
		
	file=fopen(path,"wb");
	if (file==NULL) {
		strcpy(err_str,"Could not create file");
		free(compress_data);
		return(FALSE);
	}
		
	fwrite(&game_file_sz,1,sizeof(unsigned long),file);
	fwrite(&compress_sz,1,sizeof(unsigned long),file);
	fwrite(compress_data,1,compress_sz,file);
	fclose(file);
	
#ifndef D3_OS_WINDOWS
	truncate(path,((sizeof(unsigned long)*2)+compress_sz));
#endif

	free(compress_data);
	
	return(TRUE);
}

bool game_file_expand_load(char *path,char *err_str)
{
	unsigned long   file_sz,compress_sz;
	ptr				compress_data;
	FILE			*file;
	struct stat		sb;
	
		// get file size

	if (stat(path,&sb)!=0) {
		strcpy(err_str,"File does not exist");
		return(FALSE);
	}
	file_sz=sb.st_size;
	
	compress_data=malloc(file_sz);
	if (compress_data==NULL) {
		strcpy(err_str,"Out of memory");
		return(FALSE);
	}
	
		// get compressed data
		
	file=fopen(path,"rb");
	if (file==NULL) {
		strcpy(err_str,"Could not open file");
		free(compress_data);
		return(FALSE);
	}
	
	fread(&game_file_sz,1,sizeof(unsigned long),file);
	fread(&compress_sz,1,sizeof(unsigned long),file);
	fread(compress_data,1,compress_sz,file);
	fclose(file);
	
		// decompress file

	game_file_data=zip_expand(compress_data,compress_sz,&game_file_sz,err_str);
	if (game_file_data==NULL) {
		free(compress_data);
		return(FALSE);
	}
	
	free(compress_data);
	
	return(TRUE);
}

/* =======================================================

      Get Save File Path
      
======================================================= */

void game_file_create_name(int tick,char *file_name)
{
	int					sec;
	char				elapse_str[32],time_str[32];
	struct tm			*tm;
	time_t				curtime;

	curtime=time(NULL);
	tm=localtime(&curtime);
	sprintf(time_str,"%.4d%.2d%.2d%.2d%.2d",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min);
		
	sec=(game_time_get()-map.start_game_tick)/1000;
	if (sec>(99*60)) sec=99*60;
	sprintf(elapse_str,"%.2d%.2d",(sec/60),(sec%60));
		
	sprintf(file_name,"%s_%s_%s",time_str,elapse_str,map.info.name);
}

/* =======================================================

      Save Game
      
======================================================= */

bool game_file_save(char *err_str)
{
	int					n,k,t,count,tick;
	char				path[1024],file_name[256];
	bool				ok;
	file_save_header	head;
	obj_type			*obj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	
	progress_initialize("Saving",NULL);
	progress_draw(5);
	
		// get saved data file names
		
	tick=game_time_get();

	game_file_create_name(tick,file_name);
	
		// save screen
		
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"png");
	view_capture_draw(path);
	
		// start chunks
		
	game_file_sz=0;
	game_file_data=malloc(32);

		// header

	head.tick=tick;
	strcpy(head.version,dim3_version);
	strcpy(head.map_name,map.info.name);
		
	game_file_add_chunk(&head,1,sizeof(file_save_header));
	
		// send scripts save event
		// to backup globals
		
	progress_draw(10);
	
	if (!script_state_save()) {
		free(game_file_data);
		progress_shutdown();
		return(FALSE);
	}
	
		// view & server state
		
	progress_draw(20);
		
	game_file_add_chunk(&view.time,1,sizeof(view_time_type));
	game_file_add_chunk(&camera,1,sizeof(camera_type));
	
	game_file_add_chunk(&server.time,1,sizeof(server_time_type));
	game_file_add_chunk(&server.player_obj_idx,1,sizeof(int));
	game_file_add_chunk(&server.skill,1,sizeof(int));
	
		// objects, weapons, and projectile setups

	progress_draw(30);

	count=object_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(obj,1,sizeof(obj_type));

		count=object_count_weapons(obj);
		game_file_add_chunk(&count,1,sizeof(int));

		for (k=0;k!=max_weap_list;n++) {
			weap=obj->weap_list.weaps[k];
			if (weap==NULL) continue;

			game_file_add_chunk(&k,1,sizeof(int));
			game_file_add_chunk(weap,1,sizeof(weapon_type));

			count=weapon_count_projectile_setups(weap);
			game_file_add_chunk(&count,1,sizeof(int));

			for (t=0;t!=max_proj_setup_list;t++) {
				proj_setup=weap->proj_setup_list.proj_setups[t];
				if (proj_setup==NULL) continue;

				game_file_add_chunk(&t,1,sizeof(int));
				game_file_add_chunk(proj_setup,1,sizeof(proj_setup_type));
			}
		}
	}

		// projectiles, effects and decals

	progress_draw(40);

	count=projectile_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_proj_list;n++) {
		proj=server.proj_list.projs[n];
		if (proj==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(proj,1,sizeof(proj_type));
	}

	count=effect_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_effect_list;n++) {
		effect=server.effect_list.effects[n];
		if (effect==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(effect,1,sizeof(effect_type));
	}

	count=decal_count_list();
	game_file_add_chunk(&count,1,sizeof(int));

	for (n=0;n!=max_decal_list;n++) {
		decal=server.decal_list.decals[n];
		if (decal==NULL) continue;

		game_file_add_chunk(&n,1,sizeof(int));
		game_file_add_chunk(decal,1,sizeof(decal_type));
	}

		// supergumba -- maybe we can do away with the size/count in add_chunk?

	
	
	
	progress_draw(50);
	
	game_file_add_chunk(hud.bitmaps,hud.count.bitmap,sizeof(hud_bitmap_type));
	game_file_add_chunk(hud.texts,hud.count.text,sizeof(hud_text_type));
	game_file_add_chunk(hud.bars,hud.count.bar,sizeof(hud_bar_type));
	game_file_add_chunk(&hud.radar,1,sizeof(hud_radar_type));
	
		// map changes
		
	progress_draw(60);

	game_file_add_chunk(&map.ambient,1,sizeof(map_ambient_type));					
	game_file_add_chunk(&map.fog,1,sizeof(map_fog_type));

	progress_draw(70);

	game_file_add_chunk(map.groups,1,sizeof(group_type)*map.ngroup);
	game_file_add_chunk(map.movements,1,sizeof(movement_type)*map.nmovement);
	
		// script timers and globals
		
	progress_draw(80);

	
//	game_file_add_chunk(&js.count,1,sizeof(script_count_type));
		
//	game_file_add_chunk(js.timers,js.count.timer,sizeof(timer_type));
//	game_file_add_chunk(js.globals,js.count.global,sizeof(global_type));

		// compress and save
		
	progress_draw(90);
		
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",file_name,"sav");
	ok=game_file_compress_save(path,err_str);
	
	progress_draw(100);
	
	free(game_file_data);
	
		// remember last map
		
	strcpy(game_file_last_save_name,strrchr(path,'/'));
	
		// finished
		
	progress_shutdown();

		// misc initialization

	view_clear_fps();
	rain_reset();
    
    return(ok);
}

/* =======================================================

      Load Game
      
======================================================= */

bool game_file_load(char *file_name,char *err_str)
{
	bool				ok;
	char				*c,path[1024],fname[256];
	file_save_header	head;
	
		// load and expand
		
	strcpy(fname,file_name);
	c=strrchr(fname,'.');
	if (c!=NULL) *c=0x0;			// remove any extensions
	
	file_paths_documents(&setup.file_path_setup,path,"Saved Games",fname,"sav");
	if (!game_file_expand_load(path,err_str)) return(FALSE);
	
	game_file_pos=0;

		// if game isn't running, then start
		
	if (!server.game_open) {

		scripts_lock_events();
		ok=game_start(skill_medium,err_str);
		scripts_unlock_events();
		
		if (!ok) {
			free(game_file_data);
			return(FALSE);
		}
	}

		// get header

	game_file_get_chunk(&head);

		// check version
		
	if (strcmp(head.version,dim3_version)!=0) {
		sprintf(err_str,"This saved game file is from a different version of dim3");
		free(game_file_data);
		return(FALSE);
	}
		
		// reload map

	if ((!server.map_open) || (strcmp(head.map_name,map.info.name)!=0)) {		// need to load a map?
	
		if (server.map_open) map_end();
		
		strcpy(map.info.name,head.map_name);
		map.info.player_start_name[0]=0x0;

		scripts_lock_events();
		ok=map_start(TRUE,err_str);
		scripts_unlock_events();
		
		if (!ok) {
			free(game_file_data);
			return(FALSE);
		}
	}

		// start progress

	progress_initialize("Loading",NULL);

		// view and server objects
		
	progress_draw(10);
					
	game_file_get_chunk(&view.time);
	game_file_get_chunk(&view.fps);
	game_file_get_chunk(&camera);
	
	game_file_get_chunk(&server.time);
	game_file_get_chunk(&server.player_obj_idx);
	game_file_get_chunk(&server.skill);
	
//	game_file_get_chunk(&server.uid);
	
	progress_draw(20);

// supergumba -- needs to be different

	// --> object_dispose_single
//	free(server.objs);
//	free(server.weapons);
//	free(server.proj_setups);
	
	progress_draw(30);

	// supergumba -- needs to be different, plus memory in obj->draw, etc	

//	server.objs=(obj_type*)game_file_replace_chunk();
//	server.weapons=(weapon_type*)game_file_replace_chunk();
//	server.proj_setups=(proj_setup_type*)game_file_replace_chunk();

//	if ((server.objs==NULL) || (server.weapons==NULL) || (server.proj_setups==NULL)) {
//		free(game_file_data);
//		return(FALSE);
//	}

	progress_draw(40);

//	game_file_get_chunk(server.projs);
//	game_file_get_chunk(server.effects);
//	game_file_get_chunk(server.decals);

	progress_draw(50);

	game_file_get_chunk(hud.bitmaps);
	game_file_get_chunk(hud.texts);
	game_file_get_chunk(hud.bars);
	game_file_get_chunk(&hud.radar);
	
		// map changes
		
	progress_draw(60);

	game_file_get_chunk(&map.ambient);					
	game_file_get_chunk(&map.rain);					
	game_file_get_chunk(&map.background);					
	game_file_get_chunk(&map.sky);
	game_file_get_chunk(&map.fog);

	progress_draw(70);
	
	map_group_dispose_unit_list(&map);			// need to destroy and rebuild unit lists
	game_file_get_chunk(map.groups);
	map_group_create_unit_list(&map);

	game_file_get_chunk(map.movements);

	group_moves_synch_with_load();
	
		// script objects
		
	progress_draw(80);

//	game_file_get_chunk(&js.count);
	
//	game_file_get_chunk(js.timers);
//	game_file_get_chunk(js.globals);

		// reset model

	progress_draw(90);

	models_reset();
	
		// fix the script state
		// and reset indexes on timers
		
	progress_draw(95);
	
	if (!script_state_load()) {
		free(game_file_data);
		progress_shutdown();
		return(FALSE);
	}

	timers_fix_script_indexes();

		// finish
		
	progress_draw(100);

	free(game_file_data);
	progress_shutdown();

		// fix some necessary functions

	map.rain.reset=TRUE;
	view_draw_fade_cancel();
		
		 // fix all the timing
		 // and state informaton
		 
	input_clear_mouse();

	game_time_reset(head.tick);
	view_game_reset_timing();
  
    return(TRUE);
}

bool game_file_reload_ok(void)
{
	char			*c,name[256];
	
		// is there a last save?
		
	if (game_file_last_save_name[0]==0x0) return(FALSE);
	
		// find the map name for this save
		
	strcpy(name,(game_file_last_save_name+19));
	
	c=strchr(name,'.');			// remove the .sav
	if (c!=NULL) *c=0x0;

	return(strcmp(name,map.info.name)==0);
}

bool game_file_reload(char *err_str)
{
	return(game_file_load(game_file_last_save_name,err_str));
}

