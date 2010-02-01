/****************************** File *********************************

Module: dim3 Animator
Author: Brian Barnes
 Usage: Model Import Text Decoder

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

(c) 2000-2006 Klink! Software www.klinksoftware.com
 
*********************************************************************/

int				text_nline,text_data_sz;
char			piece_break=0x9;
char			*text_data;
int				*text_line_starts;

/* =======================================================

      Find Text File Line Starts
      
======================================================= */

int textdecode_count_linestarts(void)
{
	int			n,cnt,offset;
	char		*c;
	
	cnt=offset=0;
	c=text_data;
	
	for (n=0;n!=text_data_sz;n++) {
		if ((*c==0xD) || (*c==0xA)) {
			if ((n-offset)>0) cnt++;
			offset=n+1;
		}
		c++;
	}
	
	return(cnt);
}

void textdecode_linestarts(void)
{
	int			n,idx,offset;
	char		*c;
	
	idx=offset=0;
	c=text_data;
	
	for (n=0;n!=text_data_sz;n++) {
		if ((*c==0xD) || (*c==0xA)) {
			*c=0x0;
			if ((n-offset)>0) text_line_starts[idx++]=offset;
			offset=n+1;
		}
		c++;
	}
	
	text_line_starts[idx]=offset;
}

/* =======================================================

      Open/Close Text File
      
======================================================= */

bool textdecode_open(char *path,char p_piece_break)
{
	FILE			*file;
	struct stat		sb;
	
		// get file size
		
	if (stat(path,&sb)!=0) return(FALSE);
	text_data_sz=sb.st_size;
	
		// open file
		
	file=fopen(path,"r");
	if (file==NULL) return(FALSE);
   
	text_data=malloc(text_data_sz+1);
	if (text_data==NULL) {
		fclose(file);
		return(FALSE);
	}
	
	fread(text_data,1,text_data_sz,file);
	fclose(file);
	
	text_data[text_data_sz]=0x0;
	
		// get line start data
		
	text_nline=textdecode_count_linestarts();
	
	text_line_starts=(int*)malloc((text_nline+1)*sizeof(int));
	if (text_line_starts==NULL) {
		free(text_data);
		return(FALSE);
	}
	
		// decode lines
		
	textdecode_linestarts();
   
    piece_break=p_piece_break;
	
	return(TRUE);
}

void textdecode_close(void)
{
	free(text_line_starts);
	free(text_data);
}

/* =======================================================

      Find a Line
      
======================================================= */

int textdecode_count(void)
{
	return(text_nline);
}

int textdecode_find(int line_idx,char *txt)
{
	int			n,sz;
	char		*c;
	
	sz=strlen(txt);
	
	for (n=line_idx;n!=text_nline;n++) {
		c=text_data+text_line_starts[n];
		if (strncasecmp(c,txt,sz)==0) return(n);
	}
	
	return(-1);
}

/* =======================================================

      Get Line Data
      
======================================================= */

void textdecode_get_line(int line_idx,char *txt,int len)
{
	bzero(txt,len);
	
	strncpy(txt,(text_data+text_line_starts[line_idx]),len);
	txt[len-1]=0x0;
}

void textdecode_get_piece(int line_idx,int piece_idx,char *txt)
{
	int			n;
	char		line[1024];
	char		*c;
	
	textdecode_get_line(line_idx,line,1024);
	
	txt[0]=0x0;
	
		// find start of token
		
	c=line;
	
	for (n=0;n!=piece_idx;n++) {
		c=strchr(c,piece_break);
		if (c==NULL) return;
		c++;
		
		while (*c==piece_break) c++;
	}
	
		// find end of token
		
	strncpy(txt,c,256);
	txt[255]=0x0;
		
	c=strchr(txt,piece_break);
	if (c!=NULL) *c=0x0;
}

