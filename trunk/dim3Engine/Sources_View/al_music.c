/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Music

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

(c) 2000-2007 Klink! Software www.klinksoftware.com
 
*********************************************************************/

#ifdef D3_PCH
	#include "dim3engine.h"
#endif

#include "sounds.h"

int							audio_music_fade_mode,audio_music_fade_start_tick,audio_music_fade_msec,
							audio_music_fade_next_msec,audio_music_original_volume;
char						audio_music_fade_next_name[name_str_len];
bool						audio_music_state_on,audio_music_paused;

extern int					audio_buffer_count,audio_global_music_volume,
							audio_music_buffer_idx;
extern float				audio_music_stream_pos;
extern bool					audio_music_playing;
extern audio_buffer_type	audio_buffers[audio_max_buffer];

extern setup_type			setup;


// supergumba -- testing

#include <mpg123.h>
/*
void cleanup(mpg123_handle *mh)
{
	mpg123_close(mh);
	mpg123_delete(mh);
	mpg123_exit();
}

void music_test(void)
{
	SNDFILE* sndfile = NULL;
	SF_INFO sfinfo;
	mpg123_handle *mh = NULL;
	unsigned char* buffer = NULL;
	size_t buffer_size = 0;
	size_t done = 0;
	int  channels = 0, encoding = 0;
	long rate = 0;
	int  err  = MPG123_OK;
	off_t samples = 0;
	
	err = mpg123_init();
	if( err != MPG123_OK || (mh = mpg123_new(NULL, &err)) == NULL
	    -- Let mpg123 work with the file, that excludes MPG123_NEED_MORE messages. --
	    || mpg123_open(mh, "/Users/ggadwa/dim3/zRadio.mp3") != MPG123_OK
	    -- Peek into track and get first output format.--
	    || mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK )
	{
		fprintf( stderr, "Trouble with mpg123: %s\n",
		         mh==NULL ? mpg123_plain_strerror(err) : mpg123_strerror(mh) );
		cleanup(mh);
		return;
	}

	if(encoding != MPG123_ENC_SIGNED_16)
	{ -- Signed 16 is the default output format anyways; it would actually by only different if we forced it.
	     So this check is here just for this explanation. --
		cleanup(mh);
		fprintf(stderr, "Bad encoding: 0x%x!\n", encoding);
		return;
	}
	-- Ensure that this output format will not change (it could, when we allow it). --
	mpg123_format_none(mh);
	mpg123_format(mh, rate, channels, encoding);

	--Buffer could be almost any size here, mpg123_outblock() is just some recommendation.
	   Important, especially for sndfile writing, is that the size is a multiple of sample size. --
	buffer_size = mpg123_outblock( mh );
	buffer = malloc( buffer_size );

	bzero(&sfinfo, sizeof(sfinfo) );
	sfinfo.samplerate = rate;
	sfinfo.channels = channels;
	sfinfo.format = SF_FORMAT_WAV|SF_FORMAT_PCM_16;
	printf("Creating 16bit WAV with %i channels and %liHz.\n", channels, rate);

	sndfile = sf_open("/Users/ggadwa/dim3/zRadio2.wav", SFM_WRITE, &sfinfo);
	if(sndfile == NULL){ fprintf(stderr, "Cannot open output file!\n"); cleanup(mh); return -2; }

	do
	{
		err = mpg123_read( mh, buffer, buffer_size, &done );
		sf_write_short( sndfile, (short*)buffer, done/sizeof(short) );
		samples += done/sizeof(short);
		-- We are not in feeder mode, so MPG123_OK, MPG123_ERR and MPG123_NEW_FORMAT are the only possibilities.
		   We do not handle a new format, MPG123_DONE is the end... so abort on anything not MPG123_OK. --
	} while (err==MPG123_OK);

	if(err != MPG123_DONE)
	fprintf( stderr, "Warning: Decoding ended prematurely because: %s\n",
	         err == MPG123_ERR ? mpg123_strerror(mh) : mpg123_plain_strerror(err) );

	sf_close( sndfile );

	samples /= channels;
	cleanup(mh);
}
*/

/*
SDL_AudioSpec *SDL_LoadMP3(const char *file, SDL_AudioSpec *spec, Uint8 **audio_buf, Uint32 *audio_len)
{
	int				err,channels,encoding;
	long			rate;
	mpg123_handle	*mh;
	
		// initialize the library
		
	if (mpg123_init()!=MPG123_OK) return(NULL);// supergumba -- obviously move this
	
	mh=mpg123_new(NULL,&err);
	if (mh==NULL) return(FALSE);
	
	if (mp123_open(mh,file)!=MPG123_OK) {
		mpg123_delete(mh);
		mpg123_exit();			// supergumba -- move this!
		return(FALSE);
	}
		
	if (mp123_getformat(mh,&rate,&channels,&encoding)!=MPG123_OK) {
		mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();			// supergumba -- move this!
		return(FALSE);
	}
	
	mpg123_format_none(mh);
	mpg123_format(mh,rate,channels,encoding);
	
		// setup SDL audio spec
		
	spec->freq=rate;
	spec->format=AUDIO_S16SYS;
	spec->channels=channels;
	spec->samples=4096;
	
	fprintf(stdout,"%s:\nrate=%d, channels=%d, encoding=%d\n",file,(int)rate,channels,encoding);
	
	
			mpg123_close(mh);
		mpg123_delete(mh);
		mpg123_exit();			// supergumba -- move this!
		return(FALSE);



}
*/


/*
int al_open_buffer2(char *name,char *path,int min_dist,int max_dist)
{
	int					len,org_len,new_len;
	unsigned char		*data;
	audio_buffer_type	*buffer;
	SDL_AudioSpec		aspec;
	SDL_AudioCVT		acvt;

		// any more buffers?

	if (audio_buffer_count>=audio_max_buffer) return(-1);

		// get next buffer

	buffer=&audio_buffers[audio_buffer_count];

		// load wav data

	if (SDL_LoadWAV(path,&aspec,(Uint8**)&data,(Uint32*)&len)==NULL) return(-1);

		// convert data to required format
		// we only care about removing stereo
		// and getting the signed short format
		// correct, we handle frequencies in place

	if (SDL_BuildAudioCVT(&acvt,aspec.format,aspec.channels,aspec.freq,audio_format,1,aspec.freq)==-1) {
		SDL_FreeWAV(data);
		return(-1);
	}

	org_len=len*acvt.len_mult;

	acvt.buf=malloc(org_len);
	acvt.len=len;
	memcpy(acvt.buf,data,len);

	SDL_FreeWAV(data);

	if (SDL_ConvertAudio(&acvt)==-1) return(-1);

		// got good data?
		// we need at least one short

	new_len=(int)(((double)acvt.len)*acvt.len_ratio);
	if (new_len<2) return(-1);

	if (new_len>org_len) new_len=org_len;		// just in case the conversion messes up

		// setup buffer

	strcpy(buffer->name,name);

	buffer->data=(short*)acvt.buf;
	buffer->len=new_len;
	buffer->sample_len=buffer->len>>1;						// size of 16 bit samples
	buffer->f_sample_len=(float)buffer->sample_len;
	
	buffer->min_dist=(float)min_dist;
	buffer->max_dist=(float)max_dist;

	buffer->freq_factor=((float)aspec.freq)/((float)audio_frequency);

	buffer->loaded=TRUE;

		// goto next buffer

	audio_buffer_count++;
	return(audio_buffer_count-1);
}
*/

/* =======================================================

      Music Initialize and Shutdown
      
======================================================= */

bool al_music_initialize(char *err_str)
{
	audio_music_state_on=TRUE;

	audio_music_playing=FALSE;
	audio_music_paused=FALSE;

	audio_music_buffer_idx=-1;

	audio_global_music_volume=600;
	
	return(TRUE);
}

void al_music_shutdown(void)
{
	if (audio_music_buffer_idx!=-1) al_close_buffer(audio_music_buffer_idx);
}

/* =======================================================

      Start and Stop Music
      
======================================================= */

bool al_music_play(char *name)
{
	char				path[1024];
	bool				load;
	audio_buffer_type	*buffer;

	SDL_LockAudio();
	
		// start with no fade
		
	audio_music_fade_mode=music_fade_mode_none;
	
		// if same music, don't reload
		
	load=TRUE;
	
	if (audio_music_buffer_idx!=-1) {
		buffer=&audio_buffers[audio_music_buffer_idx];
		load=(strcmp(buffer->name,name)!=0);
	}
	
		// open music
		
	if (load) {
		file_paths_data(&setup.file_path_setup,path,"Music",name,"wav");
		audio_music_buffer_idx=al_open_buffer(name,path,0,0);

		if (audio_music_buffer_idx==-1) {
			SDL_UnlockAudio();
			return(FALSE);
		}
	}
	
	buffer=&audio_buffers[audio_music_buffer_idx];
	
		// play

	audio_music_stream_pos=0.0f;
	audio_music_paused=FALSE;
		
	if (audio_music_state_on) audio_music_playing=TRUE;

	SDL_UnlockAudio();

	return(TRUE);
}

void al_music_stop(void)
{
	SDL_LockAudio();

	audio_music_stream_pos=0.0f;
	audio_music_playing=FALSE;

	SDL_UnlockAudio();
}

void al_music_pause(void)
{
	SDL_LockAudio();

	if (audio_music_playing) {
		audio_music_paused=TRUE;
		audio_music_playing=FALSE;
	}

	SDL_UnlockAudio();
}

void al_music_resume(void)
{
	SDL_LockAudio();

	if (audio_music_paused) {
		audio_music_paused=FALSE;
		audio_music_playing=TRUE;
	}

	SDL_UnlockAudio();
}

bool al_music_playing(void)
{
	return(audio_music_playing);
}

bool al_music_playing_is_name(char *name)
{
	audio_buffer_type  *buffer;
	
	if (!audio_music_playing) return(FALSE);
	
	buffer=&audio_buffers[audio_music_buffer_idx];
	return(strcmp(buffer->name,name)==0);
}

/* =======================================================

      Set Music Volume and State
      
======================================================= */

void al_music_set_volume(float music_volume)
{
	audio_global_music_volume=(int)(1024.0f*music_volume);
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

      Run Music
      
======================================================= */

bool al_music_fade_in(int tick,char *name,int msec)
{
		// start music

	if (!al_music_play(name)) return(FALSE);

		// if no msec, then just play music

	if (msec<=0) {
		audio_music_fade_mode=music_fade_mode_none;
		return(TRUE);
	}
	
		// start fade in

	audio_music_fade_mode=music_fade_mode_in;
	audio_music_fade_start_tick=tick;
	audio_music_fade_msec=msec;
	
	audio_music_original_volume=audio_global_music_volume;
	audio_global_music_volume=0;

	return(TRUE);
}

void al_music_fade_out(int tick,int msec)
{
		// if no music playing, no fade out

	if (!audio_music_playing) {
		audio_music_fade_mode=music_fade_mode_none;
		return;
	}
	
		// if no msec, then just stop music

	if (msec<=0) {
		al_music_stop();
		audio_music_fade_mode=music_fade_mode_none;
		return;
	}

		// start fade out

	audio_music_fade_mode=music_fade_mode_out;
	audio_music_fade_start_tick=tick;
	audio_music_fade_msec=msec;
	
	audio_music_original_volume=audio_global_music_volume;
}

bool al_music_fade_out_fade_in(int tick,char *name,int fade_out_msec,int fade_in_msec)
{
		// if no fade out or no music playing, go directly to fade in

	if ((fade_out_msec<=0) || (!audio_music_playing)) {
		return(al_music_fade_in(tick,name,fade_in_msec));
	}

		// setup next music for fade in

	strcpy(audio_music_fade_next_name,name);
	audio_music_fade_next_msec=fade_in_msec;

		// start fade

	al_music_fade_out(tick,fade_out_msec);

	audio_music_fade_mode=music_fade_mode_out_fade_in;		// switch to fade out/fade in mode

	return(TRUE);
}

void al_music_run(int tick)
{
	int				dif;
	
		// is there a fade on?
		
	if (audio_music_fade_mode==music_fade_mode_none) return;
	
		// is music even playing?
		
	if (!audio_music_playing) {
		audio_music_fade_mode=music_fade_mode_none;
		audio_global_music_volume=audio_music_original_volume;
		return;
	}
	
		// time to stop fade?
		
	dif=tick-audio_music_fade_start_tick;
	if (dif>=audio_music_fade_msec) {

		switch (audio_music_fade_mode) {

			case music_fade_mode_in:
				audio_global_music_volume=audio_music_original_volume;
				audio_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out:
				al_music_stop();
				audio_global_music_volume=audio_music_original_volume;
				audio_music_fade_mode=music_fade_mode_none;
				break;

			case music_fade_mode_out_fade_in:
				al_music_fade_in(tick,audio_music_fade_next_name,audio_music_fade_next_msec);
				break;

		}

		return;
	}
	
		// set the fade volume
		
	switch (audio_music_fade_mode) {
		
		case music_fade_mode_in:
			audio_global_music_volume=(int)((float)audio_music_original_volume*(((float)dif)/(float)audio_music_fade_msec));
			break;

		case music_fade_mode_out:
		case music_fade_mode_out_fade_in:
			audio_global_music_volume=(int)((float)audio_music_original_volume*((float)(audio_music_fade_msec-dif)/(float)audio_music_fade_msec));
			break;

	}

}
