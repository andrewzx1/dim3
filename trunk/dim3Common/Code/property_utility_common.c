/****************************** File *********************************

Module: dim3 Common
Author: Brian Barnes
 Usage: Property Utilities

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

#ifdef D3_PCH
	#ifdef D3_EDITOR
		#include "dim3editor.h"
	#else
		#include "dim3Animator.h"
	#endif
#endif

#include "glue.h"
#include "interface.h"
#include "ui_common.h"

/* =======================================================

      Property String Utilities
      
======================================================= */

void property_string_get_values(int value_type,void *value,int i_min,int i_max,char *str,char *desc)
{
	str[0]=0x0;
	desc[0]=0x0;
	
	switch (value_type) {
	
		case list_string_value_string:
			strcpy(str,(char*)value);
			strcpy(desc,"Enter a string.");
			break;
			
		case list_string_value_int:
			sprintf(str,"%d",*((int*)value));
			strcpy(desc,"Enter a integer.");
			break;
			
		case list_string_value_positive_int:
			sprintf(str,"%d",*((int*)value));
			strcpy(desc,"Enter a positive integer.");
			break;
			
		case list_string_value_range_int:
			sprintf(str,"%d",*((int*)value));
			sprintf(desc,"Enter a integer between %d and %d.",i_min,i_max);
			break;
			
		case list_string_value_float:
			sprintf(str,"%.2f",*((float*)value));
			strcpy(desc,"Enter a float.");
			break;

		case list_string_value_positive_float:
			sprintf(str,"%.2f",*((float*)value));
			strcpy(desc,"Enter a positive float.");
			break;

		case list_string_value_0_to_1_float:
			sprintf(str,"%.2f",*((float*)value));
			strcpy(desc,"Enter a float between 0.0 and 1.0.");
			break;

		case list_string_value_tag:
			memmove(str,((int*)value),4);
			str[4]=0x0;
			strcpy(desc,"Enter a 4 character tag.");
			break;
			
	}
}

void property_string_set_values(int value_type,void *value,int value_len,int i_min,int i_max,char *str)
{
	int				len;
	char			*v_str;
	
	switch (value_type) {
	
		case list_string_value_string:
			v_str=(char*)value;
			strncpy(v_str,str,value_len);
			v_str[value_len-1]=0x0;
			break;
			
		case list_string_value_int:
			*((int*)value)=atoi(str);
			break;
			
		case list_string_value_positive_int:
			*((int*)value)=abs(atoi(str));
			break;
			
		case list_string_value_range_int:
			*((int*)value)=atoi(str);
			if ((*((int*)value))<i_min) *((int*)value)=i_min;
			if ((*((int*)value))>i_max) *((int*)value)=i_max;
			break;
		
		case list_string_value_float:
			*((float*)value)=(float)atof(str);
			break;
			
		case list_string_value_positive_float:
			*((float*)value)=(float)fabs(atof(str));
			break;
			
		case list_string_value_0_to_1_float:
			*((float*)value)=(float)atof(str);
			if ((*((float*)value))<0.0f) *((float*)value)=0.0f;
			if ((*((float*)value))>1.0f) *((float*)value)=1.0f;
			break;

		case list_string_value_tag:
			len=strlen(str);
	
			while (len<4) {
				str[len++]=' ';
			}

			memmove(((int*)value),str,4);
			break;

	}
}

/* =======================================================

      Property Chord Utilities
      
======================================================= */

bool property_chord_get_values(int value_type,void *value,char *str_x,char *str_y,char *str_z,char *desc)
{
	d3pnt			*pnt;
	d3ang			*ang;
	d3vct			*vct;
	d3fpnt			*fpnt;

	str_x[0]=0x0;
	str_y[0]=0x0;
	str_z[0]=0x0;
	desc[0]=0x0;
	
	switch (value_type) {
	
		case list_chord_value_point:
			pnt=(d3pnt*)value;
			sprintf(str_x,"%d",pnt->x);
			sprintf(str_y,"%d",pnt->y);
			sprintf(str_z,"%d",pnt->z);
			strcpy(desc,"Enter a three value (x,y,z) int point.");
			return(TRUE);

		case list_chord_value_angle:
			ang=(d3ang*)value;
			sprintf(str_x,"%.2f",ang->x);
			sprintf(str_y,"%.2f",ang->y);
			sprintf(str_z,"%.2f",ang->z);
			strcpy(desc,"Enter a three value (x,y,z) float angle.");
			return(TRUE);

		case list_chord_value_vector:
			vct=(d3vct*)value;
			sprintf(str_x,"%.2f",vct->x);
			sprintf(str_y,"%.2f",vct->y);
			sprintf(str_z,"%.2f",vct->z);
			strcpy(desc,"Enter a three value (x,y,z) float vector.");
			return(TRUE);

		case list_chord_value_uv:
			fpnt=(d3fpnt*)value;
			sprintf(str_x,"%.2f",fpnt->x);
			sprintf(str_y,"%.2f",fpnt->y);
			str_z[0]=0x0;
			strcpy(desc,"Enter a two value (u,v) float uv.");
			return(FALSE);
			
	}

	return(TRUE);
}

void property_chord_set_values(int value_type,void *value,char *str_x,char *str_y,char *str_z)
{
	d3pnt			*pnt;
	d3ang			*ang;
	d3vct			*vct;
	d3fpnt			*fpnt;
	
	switch (value_type) {
	
		case list_chord_value_point:
			pnt=(d3pnt*)value;
			pnt->x=atoi(str_x);
			pnt->y=atoi(str_y);
			pnt->z=atoi(str_z);
			break;

		case list_chord_value_angle:
			ang=(d3ang*)value;
			ang->x=(float)atof(str_x);
			ang->y=(float)atof(str_y);
			ang->z=(float)atof(str_z);
			break;

		case list_chord_value_vector:
			vct=(d3vct*)value;
			vct->x=(float)atof(str_x);
			vct->y=(float)atof(str_y);
			vct->z=(float)atof(str_z);
			break;

		case list_chord_value_uv:
			fpnt=(d3fpnt*)value;
			fpnt->x=(float)atof(str_x);
			fpnt->y=(float)atof(str_y);
			fpnt->z=0.0f;
			break;

	}
}

/* =======================================================

      Property List Utilities
      
======================================================= */

void property_pick_list(char *list,int *idx)
{
	int			count;
	char		*c;

		// get count

	c=list;
	count=0;

	while (TRUE) {
		if (*c==0x0) break;
		c+=name_str_len;
		count++;
	}

		// run dialog

	dialog_property_list_run(list,count,name_str_len,0,FALSE,idx);
}

/* =======================================================

      Property Parameter Utilities
      
======================================================= */

void property_get_parameter(int idx,char *param_list,char *str)
{
	int				n;
	char			*c;
	
	*str=0x0;
	
	c=param_list;
	
	for (n=0;n!=idx;n++) {
		c=strchr(c,'|');
		if (c==NULL) return;
		c++;
	}

	strncpy(str,c,256);
	str[255]=0x0;

	c=strchr(str,'|');
	if (c!=NULL) *c=0x0;
}

void property_set_parameter(int idx,char *param_list,char *str)
{
	int				n,count;
	char			*c,*c2,tstr[10][256];
	
		// clear param list

	for (n=0;n!=10;n++) {
		tstr[n][0]=0x0;
	}

		// break up param list

	c=param_list;
	
	for (n=0;n!=10;n++) {
		if (c==0x0) break;
		
		c2=strchr(c,'|');
		if (c2==NULL) {
			strcpy(tstr[n],c);
			break;
		}
		
		strcpy(tstr[n],c);
		c=strchr(tstr[n],'|');
		*c=0x0;
		
		c=c2+1;
	}
	
		// fix the list

	strncpy(tstr[idx],str,256);
	tstr[idx][255]=0x0;

		// find last item

	count=0;

	for (n=9;n>=0;n--) {
		if (tstr[n][0]!=0x0) {
			count=n+1;
			break;
		}
	}

		// rebuild the list

	param_list[0]=0x0;

	for (n=0;n!=count;n++) {

		if (n!=0) {
			strncat(param_list,"|",255);
			param_list[254]=0x0;
		}
		
		strncat(param_list,tstr[n],255);
		param_list[254]=0x0;
	}

	param_list[254]=0x0;
}

