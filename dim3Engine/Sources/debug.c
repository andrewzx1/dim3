/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Application Debugging

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

#include "scripts.h"
#include "objects.h"
#include "weapons.h"
#include "projectiles.h"
#include "models.h"
#include "video.h"
#include "inputs.h"
#include "interfaces.h"
#include "consoles.h"
#include "timing.h"

extern int					os_vers_major,os_vers_minor_1,os_vers_minor_2;
extern char					arch_type[64];

extern render_info_type		render_info;
extern map_type				map;
extern view_type			view;
extern server_type			server;
extern js_type				js;
extern setup_type			setup;
extern hud_type				hud;
extern network_setup_type	net_setup;

char						object_type_str[][32]={"Player","Remote","Bot Multiplayer","Bot Map","Object"},
							bind_type_str[][16]={"Game","Map","Remote"},
							effect_type_str[][16]={"Flash","Particle","Lightning","Ray","Globe","Shake"};

/* =======================================================

      Debugging Dump Routines
      
======================================================= */

void debug_header(char *txt,int count,int memory)
{
	fprintf(stdout,"****************************************************************************\n");
	if (memory!=-1) {
		fprintf(stdout,"%s | Count=%d | Memory=%.2fM\n",txt,count,(((float)memory)/1000000.0f));
	}
	else {
		fprintf(stdout,"%s | Count=%d\n",txt,count);
	}
	fprintf(stdout,"****************************************************************************\n\n");
}

void debug_space(char *txt,int fieldsz)
{
	int			strsz;
	char		str[256];
	
	strsz=strlen(txt);
	
	if (strsz>=fieldsz) {
		strcpy(str,txt);
	}
	else {
		memset(str,0x20,256);
		memmove(str,txt,strsz);
	}
	
	str[fieldsz]=0x0;
	fprintf(stdout,str);
}

void debug_int_space(int n,int fieldsz)
{
	char		str[256];
	
	sprintf(str,"%d",n);
	debug_space(str,fieldsz);
}

void debug_space_tag(unsigned long tag,int fieldsz)
{
	char		str[256];
	
	memmove(str,&tag,4);
	str[4]=0x0;
	
	debug_space(str,fieldsz);
}

void debug_return(void)
{
	fprintf(stdout,"\n");
}

/* =======================================================

      Debug Dump
      
======================================================= */

void debug_dump(void)
{
	int					n,k,i,idx,cnt,mem_sz;
	obj_type			*obj;
	effect_type			*effect;
	proj_type			*proj;
	weapon_type			*weap;
	proj_setup_type		*proj_setup;
	model_type			*mdl;
	script_type			*script;
	timer_type			*timer;
	SDL_version			*sdl_ver;
	
	console_add_system("Debugging info dumped to stdout");
	
	fprintf(stdout,"\n\n");
	fprintf(stdout,"#########################################################################\n");
	fprintf(stdout,"Dump: dim3 Debugging Info\n");
	fprintf(stdout,"Engine v%s\n",dim3_version);
	fprintf(stdout,"(c) 2000-2007 Klink! Software\n");
	fprintf(stdout,"#########################################################################\n\n");

		// game info
		
	fprintf(stdout,"**************************************\n");
	fprintf(stdout,"Game\n");
	fprintf(stdout,"**************************************\n\n");
	fprintf(stdout,"Project:  %s\n",hud.proj_name);
	fprintf(stdout,"Tick: %d\n",game_time_get());
	
	debug_return();
	
		// system info
		
	fprintf(stdout,"**************************************\n");
	fprintf(stdout,"System\n");
	fprintf(stdout,"**************************************\n\n");
	fprintf(stdout,"Arch Type: %s\n",arch_type);

#ifdef D3_OS_MAC
	fprintf(stdout,"OS Version: %d.%d.%d\n",os_vers_major,os_vers_minor_1,os_vers_minor_2);
#endif
	
	sdl_ver=(SDL_version*)SDL_Linked_Version();
	fprintf(stdout,"SDL Version: %d.%d.%d\n",sdl_ver->major,sdl_ver->minor,sdl_ver->patch);
// supergumba -- js -- get version
//	fprintf(stdout,"JavaScript Version: %.2f\n",((float)JS_VERSION/100.0f));
	fprintf(stdout,"PNG Version: %s\n",PNG_LIBPNG_VER_STRING);
	
	debug_return();
	
		// video info
		
	fprintf(stdout,"**************************************\n");
	fprintf(stdout,"Video\n");
	fprintf(stdout,"**************************************\n\n");
	fprintf(stdout,"Engine: %s\n",render_info.name);
	fprintf(stdout,"Screen: %d,%d at %d\n",render_info.monitor_x_sz,render_info.monitor_y_sz,render_info.monitor_refresh_rate);
	fprintf(stdout,"Max Texture Units: %d\n",render_info.texture_unit_count);
	fprintf(stdout,"Max Texture Size: %d\n",render_info.texture_max_size);
	
	if (!gl_check_shader_ok()) fprintf(stdout,"GLSL (shaders) unsupported\n");
	if (!gl_check_frame_buffer_ok()) fprintf(stdout,"FBO unsupported\n");
	if (!gl_check_texture_rectangle_ok()) fprintf(stdout,"Rectangular Textures unsupported\n");
	if (!gl_check_fsaa_ok()) fprintf(stdout,"FSAA unsupported\n");

	fprintf(stdout,"Extensions:\n%s\n",render_info.ext_string);

#ifdef D3_OS_WINDOWS
	fprintf(stdout,"WGL Extensions:\n%s\n",wglGetExtensionsStringARB(wglGetCurrentDC()));
#endif
	
	debug_return();

		// map info
		
	fprintf(stdout,"**************************************\n");
	fprintf(stdout,"Map\n");
	fprintf(stdout,"**************************************\n\n");
	fprintf(stdout,"Map:  %s\n",map.info.name);
	fprintf(stdout,"Author:  %s\n",map.info.author);
	fprintf(stdout,"Mesh Count:  %d\n",map.mesh.nmesh);
	fprintf(stdout,"Liquid Count:  %d\n",map.liquid.nliquid);
	fprintf(stdout,"Spot Count:  %d\n",map.nspot);
	fprintf(stdout,"Scenery Count:  %d\n",map.nscenery);
	fprintf(stdout,"Node Count:  %d\n",map.nnode);
	
	debug_return();
	
		// objects

	cnt=0;

	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj!=NULL) cnt++;
	}

	debug_header("Objects",cnt,(sizeof(obj_type)*cnt));
	
	debug_space("Index",6);
	debug_space("uid",4);
	debug_space("Name",15);
	debug_space("Weapon",15);
	debug_space("Projectile",15);
	debug_space("Type",15);
	debug_space("Model",15);
	debug_space("Script",25);
	debug_space("Binding",10);
	debug_return();
	debug_space("-----",6);
	debug_space("---",4);
	debug_space("--------------",15);
	debug_space("--------------",15);
	debug_space("--------------",15);
	debug_space("--------------",15);
	debug_space("--------------",15);
	debug_space("------------------------",25);
	debug_space("---------",10);
	debug_return();
	
	for (n=0;n!=max_obj_list;n++) {
		obj=server.obj_list.objs[n];
		if (obj==NULL) continue;

		debug_int_space(n,6);
		debug_int_space(obj->uid,4);
		debug_space(obj->name,15);
		debug_space("",30);
		debug_space(object_type_str[obj->type],15);
		
		if (obj->draw.model_idx==-1) {
			debug_space("*",15);
		}
		else {
			debug_space(server.model_list.models[obj->draw.model_idx]->name,15);
		}
		
		if (!obj->scenery.on) {
			idx=scripts_find_uid(obj->attach.script_uid);
			debug_space(js.scripts[idx].name,25);
		}
		else {
			debug_space("*",25);
		}
		
		debug_space(bind_type_str[obj->bind],10);
		debug_return();
		
			// object weapons
			
		weap=server.weapons;
		
		for (k=0;k!=server.count.weapon;k++) {
			if (weap->obj_uid==obj->uid) {
			
				debug_space("",25);
				debug_space(weap->name,15);
				debug_return();
				
					// weapon projectiles
				
				proj_setup=server.proj_setups;
				
				for (i=0;i!=server.count.proj_setup;i++) {
					if ((proj_setup->obj_uid==obj->uid) && (proj_setup->weap_uid==weap->uid)) {
						debug_space("",40);
						debug_space(proj_setup->name,15);
						debug_return();
					}
					
					proj_setup++;
				}
			}
			
			weap++;
		}
	}
	
	debug_return();
	
		// models

	cnt=0;
	mem_sz=0;

	for (i=0;i!=max_model_list;i++) {
		mdl=server.model_list.models[i];
		if (mdl==NULL) continue;

		cnt++;
		mem_sz+=(sizeof(model_type)+model_memory_size(mdl));
	}
		
	debug_header("Models",cnt,mem_sz);
	
	debug_space("Name",32);
	debug_space("Vertexes",10);
	debug_space("Trigs",10);
	debug_space("Ref Count",10);
	debug_return();
	debug_space("------------------------------",32);
	debug_space("---------",10);
	debug_space("---------",10);
	debug_space("---------",10);
	debug_return();

	for (i=0;i!=max_model_list;i++) {
		mdl=server.model_list.models[i];
		if (mdl==NULL) continue;

		debug_space(mdl->name,32);
		debug_int_space(mdl->meshes[0].nvertex,10);
		debug_int_space(mdl->meshes[0].ntrig,10);
		debug_int_space(mdl->reference_count,10);
		debug_return();
		mdl++;
	}
	
	debug_return();
	
		// projectiles
		
	debug_header("Projectiles",server.count.proj,(sizeof(proj_type)*max_projectile));
	
	debug_space("Name",20);
	debug_space("Object",20);
	debug_space("Weapon",20);
	debug_return();
	debug_space("-------------------",20);
	debug_space("-------------------",20);
	debug_space("-------------------",20);
	debug_return();
	
	proj=server.projs;
	
	for ((i=0);(i!=server.count.proj);i++) {
		obj=object_find_uid(proj->obj_uid);
		weap=weapon_find_uid(proj->weap_uid);
		proj_setup=proj_setups_find_uid(proj->proj_setup_uid);
		
		debug_space(proj_setup->name,20);
		debug_space(obj->name,20);
		debug_space(weap->name,20);
		debug_return();
		proj++;
	}
	
	debug_return();
	
		// effects
		
	debug_header("Effects",server.count.effect,(sizeof(effect_type)*max_effect));
	
	debug_space("Type",10);
	debug_space("Life Tick",10);
	debug_return();
	debug_space("---------",10);
	debug_space("---------",10);
	debug_return();
	
	effect=server.effects;
	
	for ((i=0);(i!=server.count.effect);i++) {
		debug_space(effect_type_str[effect->effecttype],10);
		debug_int_space(effect->life_tick,10);
		debug_return();
		effect++;
	}
	
	debug_return();
	
		// scripts
		
	script=js.scripts;
	
	cnt=0;
	for ((i=0);(i!=max_scripts);i++) {
		if (script->used) cnt++;
		script++;
	}
		
	debug_header("Scripts",cnt,-1);
	
	debug_space("Name",32);
	debug_return();
	debug_space("-------------------------------",32);
	debug_return();
	
	script=js.scripts;
	
	for ((i=0);(i!=max_scripts);i++) {
		if (script->used) {
			debug_space(script->name,32);
			debug_return();
		}
		script++;
	}
	
	debug_return();
	
		// timers
		
	debug_header("Timers",js.count.timer,-1);
	
	debug_space("Script",32);
	debug_space("Count",10);
	debug_space("Type",10);
	debug_return();
	debug_space("-------------------------------",32);
	debug_space("---------",10);
	debug_space("---------",10);
	debug_return();
	
	timer=js.timers;
	
	for ((i=0);(i!=js.count.timer);i++) {
		script=&js.scripts[scripts_find_uid(timer->attach.script_uid)];
		debug_space(script->name,32);
		debug_int_space(timer->count,10);
		debug_space((timer->mode==timer_mode_repeat)?"Timer":"Wait",10);
		debug_return();
		timer++;
	}
	
	debug_return();

	fflush(stdout);
}

/* =======================================================

      Debug Game
      
======================================================= */

void debug_game(void)
{
	view.debug.on=!view.debug.on;
}

/* =======================================================

      Debug ScreenShot
      
======================================================= */

void debug_screenshot(void)
{
	char				path[1024],file_name[256],str[256];
	struct tm			*tm;
	time_t				curtime;
	
	curtime=time(NULL);
	tm=localtime(&curtime);

	sprintf(file_name,"%.4d%.2d%.2d_%.2d%.2d%.2d",(tm->tm_year+1900),(tm->tm_mon+1),tm->tm_mday,tm->tm_hour,tm->tm_min,tm->tm_sec);
	file_paths_documents(&setup.file_path_setup,path,"Screenshots",file_name,"png");
		
	if (!gl_screen_shot(render_info.view_x,render_info.view_y,setup.screen.x_sz,setup.screen.y_sz,FALSE,path)) {
		console_add_system("Unable to save screenshot");
		return;
	}
	
	snprintf(str,256,"Screenshot: %s",path);
	str[255]=0x0;

	console_add_system(str);
}

/* =======================================================

      Debug Input
      
======================================================= */

void debug_input(void)
{
		// screenshot always works
		
	if (input_action_get_state_single(nc_debug_screenshot)) {
		debug_screenshot();
		return;
	}
	
		// others require debug mode to be on
		
	if (!hud.debug) return;
	
	if (input_action_get_state_single(nc_debug_status)) {
		debug_dump();
		return;
	}
	
	if (input_action_get_state_single(nc_debug_game)) {
		debug_game();
		return;
	}
}

/* =======================================================

      Debug Console Map Change
      
======================================================= */

void debug_change_map(char *name)
{
	int							n;
	char						str[256];
	bool						map_ok;
	file_path_directory_type	*fpd;

		// verify the map
		
	map_ok=FALSE;
	
	fpd=file_paths_read_directory_data(&setup.file_path_setup,"Maps","xml");
	
	for (n=0;n!=fpd->nfile;n++) {
		if (strcasecmp(fpd->files[n].file_name,name)==0) {
			map_ok=TRUE;
			break;
		}
	}

	file_paths_close_directory(fpd);
	
		// map exists?
		
	if (!map_ok) {
		sprintf(str,"map '%s' does not exist",name);
		console_add_error(str);
		return;
	}
	
		// set map
		
	strncpy(map.info.name,name,name_str_len);
	strcpy(map.info.player_start_name,"Start");
	server.map_change=TRUE;
	server.skip_media=TRUE;
}
