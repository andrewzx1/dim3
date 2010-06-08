/****************************** File *********************************

Module: dim3 Base Utility
Author: Brian Barnes
 Usage: Network Queues

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

#include "network.h"

/* =======================================================

      Initialize and Shutdown Queues
      
======================================================= */

bool net_queue_initialize(net_queue_type *queue)
{
	int				sz;

		// create queue
		
	queue->count=0;
	queue->data=NULL;
	
	sz=net_queue_max_message*net_max_msg_size;

	queue->data=(unsigned char*)malloc(sz);
	if (queue->data==NULL) return(FALSE);
	
	bzero(queue->data,sz);
	
		// create queue lock

	queue->lock=SDL_CreateMutex();

	return(TRUE);
}

void net_queue_initialize_empty(net_queue_type *queue)
{
	queue->data=NULL;
}

void net_queue_shutdown(net_queue_type *queue)
{
	if (queue->data==NULL) return;
	
	free(queue->data);
	SDL_DestroyMutex(queue->lock);
}

/* =======================================================

      Feed Data Into Queue
      
======================================================= */

bool net_queue_feed(d3socket sock,net_queue_type *queue)
{
	int				len;
	bool			err;
	unsigned char	*ptr;

	SDL_mutexV(queue->lock);
	
		// read data until there is no more or
		// the queue is full
		
	err=FALSE;
		
	while (net_receive_ready(sock)) {
	
		if (queue->count>=net_queue_max_message) break;
	
			// get the next message

		ptr=queue->data+(queue->count*net_max_msg_size);

		len=recv(sock,ptr,net_max_msg_size,0);
		
			// if len=0, then socket has closed on other end
			// if len<0, then some error, skip read and try again
			
		if (len==0) {
			err=TRUE;
			break;
		}
			
		if (len<0) break;
	
			// got a new message!

		queue->count++;
	}
	
	SDL_mutexV(queue->lock);
	
	return(!err);
}

/* =======================================================

      Push Message On Queue
      
======================================================= */

bool net_queue_push_message(net_queue_type *queue,int remote_uid,int action,unsigned char *msg_data,int msg_len)
{
	unsigned char		*ptr;
	network_header		head;

	SDL_mutexP(queue->lock);
	
		// room for another message?
		
	if (queue->count>=net_queue_max_message) {
		SDL_mutexV(queue->lock);
		return(FALSE);
	}
	
		// put message on queue
		
	ptr=queue->data+(queue->count*net_max_msg_size);
	
	head.remote_uid=htons((short)remote_uid);
	head.action=htons((short)action);
	head.len=htons((short)msg_len);

	memmove(ptr,&head,sizeof(network_header));
	ptr+=sizeof(network_header);
	
	if (msg_data!=NULL) memmove(ptr,msg_data,msg_len);
	
		// another message in queue

	queue->count++;

	SDL_mutexV(queue->lock);

	return(TRUE);
}

/* =======================================================

      Get Next Message From Queue
      
======================================================= */

bool net_queue_check_message(net_queue_type *queue,int *remote_uid,int *action,unsigned char *msg_data,int *msg_data_len)
{
	int					msg_len;
	unsigned char		*ptr;
	network_header		head;
	
	SDL_mutexP(queue->lock);
	
		// any messages any queue?
		
	if (queue->count==0) {
		SDL_mutexV(queue->lock);
		return(FALSE);
	}

		// FIFO, use first message

	ptr=queue->data;
	
		// get message header
		
	memmove(&head,ptr,sizeof(network_header));
	ptr+=sizeof(network_header);
	
	*remote_uid=(int)ntohs(head.remote_uid);
	*action=(int)ntohs(head.action);
	msg_len=(int)ntohs(head.len);

		// get message data
		
	if (msg_len!=0) memmove(msg_data,ptr,msg_len);
	
		// remove retreived mesage
		
	if (queue->count>1) memmove(queue->data,(queue->data+net_max_msg_size),((queue->count-1)*net_max_msg_size));
	queue->count--;
		
	SDL_mutexV(queue->lock);
	
		// return message size

	if (msg_data_len!=NULL) *msg_data_len=msg_len;
		
	return(TRUE);
}

