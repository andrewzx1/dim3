/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Set Vertex Bone Dialog

***************************** License ********************************

This code can be freely used as long as these conditions are met:

1. This header, in its entirety, is kept with the code
2. This credit “Created with dim3 Technology” is given on a single
application screen and in a single piece of the documentation
3. It is not resold, in it's current form or modified, as an
engine-only product

This code is presented as is. The author of dim3 takes no
responsibilities for any version of this code.

Any non-engine product (games, etc) created with this code is free
from any and all payment and/or royalties to the author of dim3,
and can be sold or given away.

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#include "glue.h"
#include "interface.h"
#include "osx_dialog.h"

extern model_type		model;

#define kVertexMajorBone					FOUR_CHAR_CODE('majb')
#define kVertexMinorBone					FOUR_CHAR_CODE('minb')
#define kVertexPercentage					FOUR_CHAR_CODE('perc')

bool						dialog_set_vertex_bone_cancel;
WindowRef					dialog_set_vertex_bone_wind;

/* =======================================================

      Fill Bone Combo
      
======================================================= */

void dialog_set_bone_combo(WindowRef wind,unsigned long sig,int id,int sel_idx)
{
	int				n;
	char			txt[128];
	
	dialog_clear_combo(wind,sig,id);

	dialog_add_combo_item(wind,sig,id,"None",0);
	dialog_add_combo_item(wind,sig,id,"-",0);
	
	for (n=0;n!=model.nbone;n++) {
	
		memmove(txt,&model.bones[n].tag,4);
		txt[4]=0x0;
		
		if (model.bones[n].name[0]!=0x0) {
			strcat(txt," (");
			strcat(txt,model.bones[n].name);
			strcat(txt,")");
		}
		
		dialog_add_combo_item(wind,sig,id,txt,0);
	}
	
	if (sel_idx==-1) {
		dialog_set_combo(wind,sig,id,0);
	}
	else {
		dialog_set_combo(wind,sig,id,(sel_idx+2));
	}
}

int dialog_get_bone_combo(WindowRef wind,unsigned long sig,int id)
{
	int				sel_idx;
	
	sel_idx=dialog_get_combo(wind,sig,id);
	if (sel_idx==0) return(-1);
	
	return(sel_idx-2);
}

/* =======================================================

      Set Vertex Bone Event Handlers
      
======================================================= */

static pascal OSStatus set_vertex_bone_event_proc(EventHandlerCallRef handler,EventRef event,void *data)
{
	int				major_bone_idx,minor_bone_idx,bone_factor;
	HICommand		cmd;
	
	switch (GetEventKind(event)) {
	
		case kEventProcessCommand:
			GetEventParameter(event,kEventParamDirectObject,typeHICommand,NULL,sizeof(HICommand),NULL,&cmd);
			
			switch (cmd.commandID) {
				
				case kHICommandCancel:
					dialog_set_vertex_bone_cancel=TRUE;
					QuitAppModalLoopForWindow(dialog_set_vertex_bone_wind);
					return(noErr);
					
				case kHICommandOK:
				
						// check factor
					
					bone_factor=((float)dialog_get_int(dialog_set_vertex_bone_wind,kVertexPercentage,0))/100.0;
						
					if (bone_factor!=1) {
						major_bone_idx=dialog_get_bone_combo(dialog_set_vertex_bone_wind,kVertexMajorBone,0);
						minor_bone_idx=dialog_get_bone_combo(dialog_set_vertex_bone_wind,kVertexMinorBone,0);
			
						if ((major_bone_idx==-1) || (minor_bone_idx==-1)) {
							os_dialog_alert("Bone Factors","You need both a major and minor bone to set the factor to anything but 100%");
							return(noErr);
						}
					}
					
						// ok to end
						
					QuitAppModalLoopForWindow(dialog_set_vertex_bone_wind);
					return(noErr);
					
			}

			return(eventNotHandledErr);
	
	}
	
	return(eventNotHandledErr);
}

/* =======================================================

      Set Vertex Bone
      
======================================================= */

bool dialog_set_vertex_bone_run(int *major_bone_idx,int *minor_bone_idx,float *factor)
{
	EventHandlerUPP			event_upp;
	EventTypeSpec			event_list[]={{kEventClassCommand,kEventProcessCommand}};
	
		// open the dialog
		
	dialog_open(&dialog_set_vertex_bone_wind,"SetVertexBone");

		// set controls

	dialog_set_bone_combo(dialog_set_vertex_bone_wind,kVertexMajorBone,0,-1);
	dialog_set_bone_combo(dialog_set_vertex_bone_wind,kVertexMinorBone,0,-1);
	dialog_set_int(dialog_set_vertex_bone_wind,kVertexPercentage,0,100);
	
	dialog_set_focus(dialog_set_vertex_bone_wind,kVertexMajorBone,0);
	
		// show window
	
	ShowWindow(dialog_set_vertex_bone_wind);
	
		// install event handler
		
	event_upp=NewEventHandlerUPP(set_vertex_bone_event_proc);
	InstallWindowEventHandler(dialog_set_vertex_bone_wind,event_upp,GetEventTypeCount(event_list),event_list,NULL,NULL);
	
		// modal window
		
	dialog_set_vertex_bone_cancel=FALSE;
	RunAppModalLoopForWindow(dialog_set_vertex_bone_wind);
	
		// dialog to data
		
	if (!dialog_set_vertex_bone_cancel) {
		*major_bone_idx=dialog_get_bone_combo(dialog_set_vertex_bone_wind,kVertexMajorBone,0);
		*minor_bone_idx=dialog_get_bone_combo(dialog_set_vertex_bone_wind,kVertexMinorBone,0);
		*factor=((float)dialog_get_int(dialog_set_vertex_bone_wind,kVertexPercentage,0))/100.0;
	}

		// close window
		
	DisposeWindow(dialog_set_vertex_bone_wind);
	
	return(!dialog_set_vertex_bone_cancel);
}

