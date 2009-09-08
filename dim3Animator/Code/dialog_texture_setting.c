/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Texture Setting Dialogs

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "window.h"
#include "dialog.h"

#define kTextureSettingFrame						FOUR_CHAR_CODE('frme')
#define kTextureSettingFrameBitmap					FOUR_CHAR_CODE('bitm')
#define kTextureSettingFrameBitmapEdit				FOUR_CHAR_CODE('edbt')
#define kTextureSettingFrameBumpmap					FOUR_CHAR_CODE('bmpm')
#define kTextureSettingFrameSpecularmap				FOUR_CHAR_CODE('spmp')
#define kTextureSettingFrameGlowmap					FOUR_CHAR_CODE('gwmp')
#define kTextureSettingFrameWait					FOUR_CHAR_CODE('watm')
#define kTextureSettingColor						FOUR_CHAR_CODE('colr')
#define kTextureSettingShader						FOUR_CHAR_CODE('shdr')
#define kTextureSettingAnimate						FOUR_CHAR_CODE('anmt')
#define kTextureSettingAlphaAdditive				FOUR_CHAR_CODE('addv')
#define kTextureSettingPixelated					FOUR_CHAR_CODE('pxld')
#define kTextureSettingGlowRate						FOUR_CHAR_CODE('grtm')
#define kTextureSettingGlowMin						FOUR_CHAR_CODE('grmn')
#define kTextureSettingGlowMax						FOUR_CHAR_CODE('grmx')
#define kTextureSettingBumpFactor					FOUR_CHAR_CODE('bfct')
#define kTextureSettingSpecularFactor				FOUR_CHAR_CODE('sfct')
#define kTextureSettingMaterialName					FOUR_CHAR_CODE('mtrl')

#define kTextureSettingButtonAddFrame				FOUR_CHAR_CODE('addt')
#define kTextureSettingButtonSubFrame				FOUR_CHAR_CODE('subt')

extern model_type				model;
extern file_path_setup_type		file_path_setup;

extern 	AGLContext				ctx;
extern WindowRef				model_wind;

int								dialog_texture_wind_current_txt,dialog_texture_wind_current_frame;
texture_type					frame_texture;
WindowRef						dialog_texture_wind;

/* =======================================================

      Texture Drawing
      
======================================================= */

void texture_setting_bitmap_draw(bitmap_type *bitmap,CGrafPtr dport,Rect *dbox)
{
	int					x,y,wid,high,row_add,
						xbyte,k,gray;
	unsigned char		r,g,b;
	unsigned char		*data,*sptr,*dptr;
	float				alpha,fr,fg,fb;
	Rect				box;
	PixMapHandle		texturemap;
	GWorldPtr			gworld;
	
		// draw the texture to the back buffer
		
	wid=dbox->right-dbox->left;
	high=dbox->bottom-dbox->top;
	
	glViewport(0,0,wid,high);
	
	glEnable(GL_SCISSOR_TEST);
	glScissor(0,0,wid,high);
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0,(GLdouble)wid,(GLdouble)0,high);
		
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_BLEND);
	
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_NOTEQUAL,0);
	
	glColor4f(1.0f,1.0f,1.0f,1.0f);
	
	glClearColor(1.0f,1.0f,1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,bitmap->gl_id);
	
	glBegin(GL_QUADS);
	glTexCoord2f(0.0f,0.0f);
	glVertex2i(0,0);
	glTexCoord2f(1.0f,0.0f);
	glVertex2i(wid,0);
	glTexCoord2f(1.0f,1.0f);
	glVertex2i(wid,high);
	glTexCoord2f(0.0f,1.0f);
	glVertex2i(0,high);
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_ALPHA_TEST);
	
		// get the data
		
	data=malloc((wid<<2)*high);
	if (data==NULL) return;
	
	glFinish();
	glReadPixels(0,0,wid,high,GL_RGBA,GL_UNSIGNED_BYTE,data);
	
		// make gworld for texture
		
	SetRect(&box,0,0,wid,high);
	NewGWorld(&gworld,32,&box,NULL,NULL,0);
    
        // copy RGB for gworld
        
	texturemap=GetGWorldPixMap(gworld);
	
	LockPixels(texturemap);
	sptr=(ptr)GetPixBaseAddr(texturemap);
	xbyte=wid<<2;
	row_add=GetPixRowBytes(texturemap)-xbyte;

	dptr=data;
	
	switch (bitmap->alpha_mode) {
	
		case alpha_mode_none:
		
			for (y=0;y!=high;y++) {
				for (x=0;x!=wid;x++) {
					*sptr++=0xFF;
					*sptr++=*dptr++;
					*sptr++=*dptr++;
					*sptr++=*dptr++;
					dptr++;
				}
				sptr+=row_add;
			}
			break;
			
		case alpha_mode_cut_out:
		
			for (y=0;y!=high;y++) {
				for (x=0;x!=wid;x++) {
					r=*dptr++;
					g=*dptr++;
					b=*dptr++;
					alpha=*dptr++;
					
					*sptr++=0xFF;
					
					if (alpha!=255) {
						*sptr++=0x0;
						*sptr++=0x0;
						*sptr++=0xFF;
					}
					else {
						*sptr++=r;
						*sptr++=g;
						*sptr++=b;
					}
				}
				sptr+=row_add;
			}
			break;
		
		case alpha_mode_transparent:

			for (y=0;y!=high;y++) {
				for (x=0;x!=wid;x++) {
					
					r=*dptr++;
					g=*dptr++;
					b=*dptr++;
					alpha=((float)*dptr++)/255.0f;
				
						// calculate alpha
						
					if (alpha!=1.0f) {
						gray=((((y/5)+(x/5))&0x1)==0x0)?0x00:0x55;
						
						fr=((float)r)/255.0f;
						fr*=alpha;
						k=(int)(fr*255.0f)+gray;
						if (k>255) k=255;
						r=(unsigned char)k;
						
						fg=((float)g)/255.0f;
						fg*=alpha;
						k=(int)(fg*255.0f)+gray;
						if (k>255) k=255;
						g=(unsigned char)k;
						
						fb=((float)b)/255.0f;
						fb*=alpha;
						k=(int)(fb*255.0f)+gray;
						if (k>255) k=255;
						b=(unsigned char)k;
					}
					
					*sptr++=0xFF;
					*sptr++=r;
					*sptr++=g;
					*sptr++=b;
				}
				
				sptr+=row_add;
			}
			break;
	}

	CopyBits((BitMap*)(*texturemap),GetPortBitMapForCopyBits(dport),&box,dbox,srcCopy,NULL);
    
	UnlockPixels(texturemap);
	
		// dispose the gworld
		
	DisposeGWorld(gworld);
	
		// free png data
		
	free(data);
}

/* =======================================================

      Texture Frames
      
======================================================= */

void texture_setting_frame_build_combo(bool first)
{
	int						n,nframe;
	char					str[32];
	texture_type			*texture;
	
		// set combo
		
	dialog_hide(dialog_texture_wind,kTextureSettingFrame,0,FALSE);
		
	dialog_clear_combo(dialog_texture_wind,kTextureSettingFrame,0);
	
	nframe=model_count_texture_frames(&model,dialog_texture_wind_current_txt);
	if (nframe<=0) nframe=1;
	
	for (n=0;n!=nframe;n++) {
		sprintf(str,"Frame %d",n);
		dialog_add_combo_item(dialog_texture_wind,kTextureSettingFrame,0,str,0);
	}
	
	if (first) {
		dialog_texture_wind_current_frame=0;
		dialog_set_combo(dialog_texture_wind,kTextureSettingFrame,0,0);
	}
	else {
		dialog_texture_wind_current_frame=nframe-1;
		dialog_set_combo(dialog_texture_wind,kTextureSettingFrame,0,(nframe-1));
	}
	
	dialog_hide(dialog_texture_wind,kTextureSettingFrame,0,TRUE);
	
		// set wait (so next bitmap draw doesn't save wrong wait)
		
	texture=&model.textures[dialog_texture_wind_current_txt];
	dialog_set_int(dialog_texture_wind,kTextureSettingFrameWait,0,texture->animate.wait[dialog_texture_wind_current_frame]);
}

void texture_setting_frame_save(void)
{
	texture_type			*texture;
	
	texture=&model.textures[dialog_texture_wind_current_txt];
	texture->animate.wait[dialog_texture_wind_current_frame]=dialog_get_int(dialog_texture_wind,kTextureSettingFrameWait,0);
}

void texture_setting_frame_reset(void)
{
    int						cframe;
    Rect					box;
	ControlRef				ctrl;
	ControlID				ctrl_id;
    RGBColor				ltgraycolor={0xCCCC,0xCCCC,0xCCCC},blackcolor={0x0,0x0,0x0};
	texture_type			*texture;
	
		// save wait from current frame
		
	texture_setting_frame_save();
	
		// get current frame
		
	texture=&model.textures[dialog_texture_wind_current_txt];
	cframe=dialog_get_combo(dialog_texture_wind,kTextureSettingFrame,0);
	dialog_texture_wind_current_frame=cframe;
		
		// draw bitmap
	
	SetPort(GetWindowPort(dialog_texture_wind));
	
	ctrl_id.signature=kTextureSettingFrameBitmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
		
	if (texture->frames[cframe].bitmap.gl_id==-1) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->frames[cframe].bitmap,GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw bumpmap
		
	ctrl_id.signature=kTextureSettingFrameBumpmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if (texture->frames[cframe].bumpmap.gl_id==-1) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->frames[cframe].bumpmap,GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw specularmap
		
	ctrl_id.signature=kTextureSettingFrameSpecularmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if (texture->frames[cframe].specularmap.gl_id==-1) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->frames[cframe].specularmap,GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// draw glowmap
		
	ctrl_id.signature=kTextureSettingFrameGlowmap;
	ctrl_id.id=0;
	GetControlByID(dialog_texture_wind,&ctrl_id,&ctrl);
	
	GetControlBounds(ctrl,&box);
	
	if (texture->frames[cframe].glowmap.gl_id==-1) {
		RGBForeColor(&ltgraycolor);
		PaintRect(&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	else {
		texture_setting_bitmap_draw(&texture->frames[cframe].glowmap,GetWindowPort(dialog_texture_wind),&box);
		RGBForeColor(&blackcolor);
		FrameRect(&box);
	}
	
		// set wait
		
	dialog_set_int(dialog_texture_wind,kTextureSettingFrameWait,0,texture->animate.wait[cframe]);
}

/* =======================================================

      Edit Textures
      
======================================================= */

bool texture_setting_bitmap_open(char *bitmap_name)
{
    char				err_str[256],sub_path[1024],path[1024];
	unsigned char		p_err_str[256];
	
		// get bitmap
				
	sprintf(sub_path,"Models/%s/Textures",model.name);
	if (!dialog_file_open_run("Open a Bitmap",sub_path,"png",NULL,bitmap_name)) return(FALSE);
	
		// check bitmap
		
	file_paths_data(&file_path_setup,path,sub_path,bitmap_name,"png");
	if (!bitmap_check(path,err_str)) {
		CopyCStringToPascal(err_str,p_err_str);
		StandardAlert(0,"\pTexture Error",p_err_str,NULL,NULL);
		return(FALSE);
	}
	
	return(TRUE);
}

/* =======================================================

      Texture Setting Event Handlers
      
======================================================= */

static pascal OSStatus texture_setting_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	char			bitmap_name[file_str_len];
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
			
				case kTextureSettingFrame:
					texture_setting_frame_reset();
					return(noErr);
			
				case kTextureSettingButtonAddFrame:
					texture_setting_frame_save();
					if (texture_setting_bitmap_open(bitmap_name)) {
						SetThemeCursor(kThemeWatchCursor);
						model_add_texture_frame(&model,dialog_texture_wind_current_txt,bitmap_name);
						texture_setting_frame_build_combo(FALSE);
						texture_setting_frame_reset();
						SetThemeCursor(kThemeArrowCursor);
					}
					return(noErr);
					
				case kTextureSettingButtonSubFrame:
					texture_setting_frame_save();
					SetThemeCursor(kThemeWatchCursor);
					model_delete_texture_frame(&model,dialog_texture_wind_current_txt);
					texture_setting_frame_build_combo(FALSE);
					texture_setting_frame_reset();
					SetThemeCursor(kThemeArrowCursor);
					return(noErr);
					
				case kTextureSettingFrameBitmapEdit:
					if (texture_setting_bitmap_open(bitmap_name)) {
						strcpy(model.textures[dialog_texture_wind_current_txt].frames[dialog_texture_wind_current_frame].name,bitmap_name);
						SetThemeCursor(kThemeWatchCursor);
						model_refresh_textures(&model);
						texture_setting_frame_reset();
						SetThemeCursor(kThemeArrowCursor);
					}
					return(noErr);
					
				case kHICommandOK:
					QuitAppModalLoopForWindow(dialog_texture_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Run Texture Settings
      
======================================================= */

void dialog_texture_setting_run(int txt)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	texture_type			*texture;

		// the texture
		
	texture=&model.textures[txt];

		// open the dialog
		
	dialog_open(&dialog_texture_wind,"TextureSetting");
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(texture_setting_event_proc);
	InstallWindowEventHandler(dialog_texture_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);

		// frames
		
	dialog_texture_wind_current_txt=txt;
	texture_setting_frame_build_combo(TRUE);
	
		// other dialog controls
		
	dialog_set_color(dialog_texture_wind,kTextureSettingColor,0,&texture->col);
		
	dialog_special_combo_fill_shader(dialog_texture_wind,kTextureSettingShader,0,texture->shader_name);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingAnimate,0,texture->animate.on);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingAlphaAdditive,0,texture->additive);
	dialog_set_boolean(dialog_texture_wind,kTextureSettingPixelated,0,texture->pixelated);
	dialog_set_int(dialog_texture_wind,kTextureSettingGlowRate,0,texture->glow.rate);
	dialog_set_float(dialog_texture_wind,kTextureSettingGlowMin,0,texture->glow.min);
	dialog_set_float(dialog_texture_wind,kTextureSettingGlowMax,0,texture->glow.max);
	dialog_set_float(dialog_texture_wind,kTextureSettingBumpFactor,0,texture->bump_factor);
	dialog_set_float(dialog_texture_wind,kTextureSettingSpecularFactor,0,texture->specular_factor);
	dialog_set_text(dialog_texture_wind,kTextureSettingMaterialName,0,texture->material_name);

		// show window
	
	ShowWindow(dialog_texture_wind);
	
		// redraw bitmaps
	
	texture_setting_frame_reset();
	
		// modal window
		
	RunAppModalLoopForWindow(dialog_texture_wind);
	
		// dialog to data

	texture_setting_frame_save();
	
	dialog_get_color(dialog_texture_wind,kTextureSettingColor,0,&texture->col);
		
	dialog_special_combo_get_shader(dialog_texture_wind,kTextureSettingShader,0,texture->shader_name,name_str_len);
	texture->animate.on=dialog_get_boolean(dialog_texture_wind,kTextureSettingAnimate,0);
	texture->additive=dialog_get_boolean(dialog_texture_wind,kTextureSettingAlphaAdditive,0);
	texture->pixelated=dialog_get_boolean(dialog_texture_wind,kTextureSettingPixelated,0);
	texture->glow.rate=dialog_get_int(dialog_texture_wind,kTextureSettingGlowRate,0);
	texture->glow.min=dialog_get_float(dialog_texture_wind,kTextureSettingGlowMin,0);
	texture->glow.max=dialog_get_float(dialog_texture_wind,kTextureSettingGlowMax,0);
	texture->bump_factor=dialog_get_float(dialog_texture_wind,kTextureSettingBumpFactor,0);
	texture->specular_factor=dialog_get_float(dialog_texture_wind,kTextureSettingSpecularFactor,0);
	dialog_get_text(dialog_texture_wind,kTextureSettingMaterialName,0,texture->material_name,name_str_len);
	
	bitmap_texture_recalc_animation(texture);
	
		// close window
		
	DisposeWindow(dialog_texture_wind);
}

