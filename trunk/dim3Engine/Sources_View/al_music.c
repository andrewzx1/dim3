/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Music

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

(c) 2000-2011 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "interface.h"

bool						audio_music_state_on;

audio_music_song_type		audio_music_song,audio_music_alt_song;

extern int					audio_buffer_count;
extern audio_buffer_type	audio_buffers[audio_max_buffer];

extern setup_type			setup;

/* =======================================================

      Music Initialize and Shutdown
      
======================================================= */

void al_music_initialize_song(audio_music_song_type *song)
{
	song->fade_mode=music_fade_mode_none;
	song->name[0]=0x0;
	song->data=NULL;
	song->playing=FALSE;
	song->loop=TRUE;
	song->paused=FALSE;
	song->volume=600;
	song->org_volume=512;
}

bool al_music_initialize(char *err_str)
{
		// initialize the mp3 decoder library
		
	if (mpg123_init()!=MPG123_OK) {
		strcpy(err_str,"Could not initialize mp3 decoder");
		return(FALSE);
	}
	
		// setup music
		
	audio_music_state_on=TRUE;
	
	al_music_initialize_song(&audio_music_song);
	al_music_initialize_song(&audio_music_alt_song);
	
	return(TRUE);
}

void al_music_shutdown(void)
{
		// free any loaded music
		
	if (audio_music_song.data!=NULL) free(audio_music_song.data);
	if (audio_music_alt_song.data!=NULL) free(audio_music_alt_song.data);
	
		// shut down the mp3 decoder library
		
	mpg123_exit();
}

/* =======================================================

      Load MP3
      
======================================================= */

bool al_open_music(audio_music_song_type *song,char *name,char *err_str)
{
	int						err,channels,encoding;
	size_t					sample_size,read_bytes;
	long					rate;
	char					path[1024];
	unsigned char			*data;
	mpg123_handle			*mh;

		// have we already load this music?

	if (song->data!=NULL) {
		if (strcmp(song->name,name)==0) return(TRUE);
	}

		// load mp3
		
	file_paths_data(&setup.file_path_setup,path,"Music",name,"mp3");

	mh=mpg123_new(NULL,&err);
	if (mh==NULL) {
		strcpy(err_str,"Out of Memory");
		return(FALSE);
	}
	
	if (mpg123_open(mh,path)!=MPG123_OK) {
		mpg123_delete(mh);
		sprintf(err_str,"Unable to open %s\n",path);
		return(FALSE);
	}
	
		// we need stereo and 16 signed
		
	mpg123_getformat(mh,&rate,&channels,&encoding);
	
	if ((channels!=2) || (encoding!=MPG123_ENC_SIGNED_16)) {
		strcpy(err_str,"Music requires 16 bit stereo MP3");
		mpg123_close(mh);
		mpg123_delete(mh);
		return(FALSE);
	}
	
		// make sure it's 16 bit and stereo
		
	mpg123_format_none(mh);
	mpg123_format(mh,rate,2,MPG123_ENC_SIGNED_16);
	
		// get size
		
	if (mpg123_scan(mh)!=MPG123_OK) {
		mpg123_close(mh);
		mpg123_delete(mh);
		sprintf(err_str,"Unable to read %s\n",path);
		return(FALSE);
	}
	
		// get buffer size
		// each sample is 16 bits * 2 channels
		
	sample_size=mpg123_length(mh);
	sample_size*=(2*channels);
	data=(unsigned char*)malloc(sample_size);
	if (data==NULL) {
		mpg123_close(mh);
		mpg123_delete(mh);
		sprintf(err_str,"Unable to read %s\n",path);
		return(FALSE);
	}

		// read it
	
	err=mpg123_read(mh,data,sample_size,&read_bytes);
	
		// end the file
		
	mpg123_close(mh);
	mpg123_delete(mh);
	
		// check for success of read
		
	if ((err!=MPG123_OK) && (err!=MPG123_DONE)) {
		sprintf(err_str,"Unable to read %s\n",path);
		return(FALSE);
	}
	
		// setup buffer
		// we need to alter some of these factors
		// for stereo and 16-bit music

	strcpy(song->name,name);
	
	if (song->data!=NULL) free(song->data);
	
	song->data=(short*)data;
	song->f_sample_len=(float)(read_bytes/2);
	song->freq_factor=((float)rate)/((float)audio_frequency)*2.0f;

	return(TRUE);
}

/* =======================================================

      Start and Stop Music
      
======================================================= */

bool al_music_play_song(audio_music_song_type *song,char *name,char *err_str)
{
	SDL_LockAudio();
	
		// open music
		
	if (!al_open_music(song,name,err_str)) {
		SDL_UnlockAudio();
		return(FALSE);
	}
	
		// play

	song->stream_pos=0.0f;
	song->volume=audio_music_song.org_volume;
	song->paused=FALSE;
	song->fade_mode=music_fade_mode_none;
		
	song->playing=audio_music_state_on;

	SDL_UnlockAudio();

	return(TRUE);
}

bool al_music_play(char *name,char *err_str)
{
	return(al_music_play_song(&audio_music_song,name,err_str));
}

void al_music_stop(void)
{
	SDL_LockAudio();

	audio_music_song.stream_pos=0.0f;
	audio_music_song.playing=FALSE;
	
	audio_music_alt_song.stream_pos=0.0f;
	audio_music_alt_song.playing=FALSE;

	SDL_UnlockAudio();
}

void al_music_set_loop(bool loop)
{
	SDL_LockAudio();
	audio_music_song.loop=loop;
	SDL_UnlockAudio();
}

void al_music_pause(void)
{
	SDL_LockAudio();

	if (audio_music_song.playing) {
		audio_music_song.paused=TRUE;
		audio_music_song.playing=FALSE;
	}
	if (audio_music_alt_song.playing) {
		audio_music_alt_song.paused=TRUE;
		audio_music_alt_song.playing=FALSE;
	}

	SDL_UnlockAudio();
}

void al_music_resume(void)
{
	SDL_LockAudio();

	if (audio_music_song.paused) {
		audio_music_song.paused=FALSE;
		audio_music_song.playing=TRUE;
	}
	
	if (audio_music_alt_song.paused) {
		audio_music_alt_song.paused=FALSE;
		audio_music_alt_song.playing=TRUE;
	}

	SDL_UnlockAudio();
}

bool al_music_playing(void)
{
	return(audio_music_song.playing);
}

bool al_music_playing_is_name(char *name)
{
	if (!audio_music_song.playing) return(FALSE);
	
	return(strcmp(audio_music_song.name,name)==0);
}

/* =======================================================

      Set Music Volume and State
      
======================================================= */

void al_music_set_volume(float music_volume)
{
	int				volume;
	
	volume=(int)(1024.0f*music_volume);
	
	audio_music_song.volume=audio_music_song.org_volume=volume;
	audio_music_alt_song.volume=audio_music_alt_song.org_volume=volume;
}

void al_music_set_state(bool music_on)
{
	if (audio_music_state_on==music_on) return;
	
	audio_music_state_on=music_on;
	if (music_on) {
		al_music_resume();
	}
	else {
		al_music_pause();
	}
}

/* =======================================================

      Music Fades and Crosses
      
======================================================= */

bool al_music_fade_in(char *name,int msec,char *err_str)
{
		// start music

	if (!al_music_play_song(&audio_music_song,name,err_str)) return(FALSE);

		// if no msec, then just play music

	if (msec<=0) {
		audio_music_song.fade_mode=music_fade_mode_none;
		audio_music_song.volume=audio_music_song.org_volume;
		return(TRUE);
	}
	
		// start fade in

	audio_music_song.fade_mode=music_fade_mode_in;
	audio_music_song.fade_start_tick=game_time_get();
	audio_music_song.fade_msec=msec;
	
	audio_music_song.volume=0;

	return(TRUE);
}

void al_music_fade_out(int msec)
{
		// if no music playing, no fade out

	if (!audio_music_song.playing) {
		audio_music_song.fade_mode=music_fade_mode_none;
		return;
	}
	
		// if no msec, then just stop music

	if (msec<=0) {
		al_music_stop();
		audio_music_song.fade_mode=music_fade_mode_none;
		return;
	}

		// start fade out

	audio_music_song.fade_mode=music_fade_mode_out;
	audio_music_song.fade_start_tick=game_time_get();
	audio_music_song.fade_msec=msec;
	audio_music_song.volume=audio_music_song.org_volume;
}

bool al_music_fade_out_fade_in(char *name,int fade_out_msec,int fade_in_msec,char *err_str)
{
		// if no fade out or no music playing, go directly to fade in

	if ((fade_out_msec<=0) || (!audio_music_song.playing)) {
		return(al_music_fade_in(name,fade_in_msec,err_str));
	}

		// setup next music for fade in

	strcpy(audio_music_song.fade_next_name,name);
	audio_music_song.fade_next_msec=fade_in_msec;

		// start fade

	al_music_fade_out(fade_out_msec);

	audio_music_song.fade_mode=music_fade_mode_out_fade_in;		// switch to fade out/fade in mode

	return(TRUE);
}

bool al_music_cross_fade(char *name,int cross_msec,char *err_str)
{
	int				tick;
	
		// if no fade out or no music playing, go directly to fade in
		
	if ((cross_msec<=0) || (!audio_music_song.playing)) {
		return(al_music_fade_in(name,cross_msec,err_str));
	}
		
		// start alt music

	if (!al_music_play_song(&audio_music_alt_song,name,err_str)) return(FALSE);

		// setup fade out for current song
		
	tick=game_time_get();
		
	audio_music_song.fade_mode=music_fade_mode_cross_fade;
	audio_music_song.fade_start_tick=tick;
	audio_music_song.fade_msec=cross_msec;
	audio_music_song.volume=audio_music_song.org_volume;

		// setup fade in for alt song
		
	audio_music_alt_song.fade_mode=music_fade_mode_cross_fade;
	audio_music_alt_song.fade_start_tick=tick;
	audio_music_alt_song.fade_msec=cross_msec;
	audio_music_alt_song.volume=0;

	return(TRUE);
}

/* =======================================================

      Run Music
      
======================================================= */

void al_music_run_song(audio_music_song_type *song,bool alt_song)
{
	int				tick,dif;
	char			err_str[256];
	bool			old_loop;
	
		// is there a fade on?
		
	if (song->fade_mode==music_fade_mode_none) return;
	
		// is music even playing?
		
	if (!song->playing) {
		song->fade_mode=music_fade_mode_none;
		song->volume=song->org_volume;
		return;
	}
	
		// time to stop fade?
		// if it's a cross fade, then make sure to
		// swap music

	tick=game_time_get();
		
	dif=tick-song->fade_start_tick;
	if (dif>=song->fade_msec) {
	
		switch (song->fade_mode) {

			case music_fade_mode_in:
				song->volume=song->org_volume;
				song->fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out:
				al_music_stop();
				song->volume=song->org_volume;
				song->fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out_fade_in:
				al_music_fade_in(song->fade_next_name,song->fade_next_msec,err_str);
				break;
				
			case music_fade_mode_cross_fade:
			
					// only do this for alt music
					
				if (!alt_song) break;
			
					// swap the music
					
				old_loop=audio_music_song.loop;
				free(audio_music_song.data);
				
				memmove(&audio_music_song,&audio_music_alt_song,sizeof(audio_music_song_type));
				audio_music_song.loop=old_loop;
					
					// set all the flags
					
				song->fade_mode=music_fade_mode_none;
				song->volume=song->org_volume;
				audio_music_song.playing=TRUE;
				audio_music_alt_song.playing=FALSE;
				break;

		}

		return;
	}
	
		// set the fade volume
		
	switch (song->fade_mode) {
		
		case music_fade_mode_in:
			song->volume=(int)((float)song->org_volume*(((float)dif)/(float)song->fade_msec));
			break;

		case music_fade_mode_out:
		case music_fade_mode_out_fade_in:
			song->volume=(int)((float)song->org_volume*((float)(song->fade_msec-dif)/(float)song->fade_msec));
			break;
			
		case music_fade_mode_cross_fade:
			if (alt_song) {
				song->volume=(int)((float)song->org_volume*(((float)dif)/(float)song->fade_msec));
			}
			else {
				song->volume=(int)((float)song->org_volume*((float)(song->fade_msec-dif)/(float)song->fade_msec));
			}
			break;


	}

}

void al_music_run(void)
{
	al_music_run_song(&audio_music_song,FALSE);
	al_music_run_song(&audio_music_alt_song,TRUE);
}

