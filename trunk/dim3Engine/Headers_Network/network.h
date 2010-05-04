/****************************** File *********************************

Module: dim3 Engine
Author: Brian Barnes
 Usage: Client Functions Header

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

//
// network
//

extern void net_initialize(void);
extern void net_shutdown(void);

extern void net_create_project_hash(void);
extern int net_get_project_hash(void);
extern void net_load_news(void);
extern char* net_get_news(void);
extern setup_network_hosts_type* net_news_get_hosts(void);

extern void net_get_host_name(char *name);
extern void net_get_host_ip(char *ip_name,char *ip_resolve);

extern bool net_ip_local(char *ip);
extern void net_ip_local_broadcast(char *ip,char *broad_ip);

//
// sockets
//

extern d3socket net_open_udp_socket(void);
extern void net_close_socket(d3socket *sock);
extern void net_socket_blocking(d3socket sock,bool blocking);
extern void net_socket_enable_broadcast(d3socket sock);
extern bool net_ip_to_address(char *ip,unsigned long *ip_addr,char *err_str);
extern bool net_bind(d3socket sock,char *ip,int port,char *err_str);
extern bool net_bind_any(d3socket sock,int port,char *err_str);
extern bool net_receive_ready(d3socket sock);
extern bool net_send_ready(d3socket sock);
extern bool net_recvfrom_mesage(d3socket sock,unsigned long *ip_addr,int *port,int *action,int *player_uid,unsigned char *msg,int *msg_len);
extern bool net_sendto_msg(d3socket sock,unsigned long ip_addr,int port,int action,int player_uid,unsigned char *msg,int msg_len);

//
// http reads
//

extern char* net_get_http_file(char *host_name,int port,char *url,char *err_str);

//
// read queues
//

extern bool net_queue_initialize(net_queue_type *queue);
extern void net_queue_initialize_empty(net_queue_type *queue);
extern void net_queue_shutdown(net_queue_type *queue);
extern bool net_queue_feed(d3socket sock,net_queue_type *queue);
extern bool net_queue_push_message(net_queue_type *queue,int action,int remote_uid,unsigned char *msg_data,int msg_len);
extern bool net_queue_check_message(net_queue_type *queue,int *action,int *player_uid,unsigned char *msg_data,int *msg_data_len);

//
// hosting
//

extern bool net_host_initialize(char *err_str);
extern void net_host_shutdown(void);

extern void net_host_player_add_bots(void);

extern void net_host_client_handle_leave(int player_uid);
extern void net_host_client_handle_set_team(int player_uid,network_request_team *team);
extern void net_host_client_handle_update(int player_uid,network_request_remote_update *update);

extern int net_host_client_handler_thread(void *arg);

//
// host local joins
//

extern int net_client_find_game(char *game_name);
extern void net_client_push_queue_local(int action,int remote_uid,unsigned char *data,int len);

extern int net_host_client_handle_local_join(network_request_join *request_join,char *err_str);

//
// host player lists
//

extern void net_host_player_initialize(void);
extern void net_host_player_shutdown(void);

extern int net_host_player_find(int player_uid);
extern int net_host_player_find_ip_addr(unsigned long ip_addr,int port);

extern bool net_host_player_add_ok(char *name,char *deny_reason);
extern int net_host_player_add(unsigned long ip_addr,int port,bool local,char *name,int tint_color_idx,int character_idx);
extern int net_host_player_add_bot(obj_type *obj);
extern void net_host_player_remove(int player_uid);

extern void net_host_player_ready(int player_uid);
extern void net_host_player_create_remote_list(int player_uid,network_reply_join_remotes *remotes);

extern void net_host_player_route_msg(int player_uid,int action,unsigned char *msg,int msg_len);
extern bool net_host_player_check_msg(int player_uid,int *action,unsigned char *msg,int *msg_len);
extern void net_host_player_start_thread(int player_uid);

extern void net_host_player_send_message_single(int to_player_uid,int action,int from_player_uid,unsigned char *msg,int msg_len);
extern void net_host_player_send_message_others(int skip_player_uid,int action,int from_player_uid,unsigned char *msg,int msg_len);
extern void net_host_player_send_message_all(int action,int from_player_uid,unsigned char *data,int len);

extern void net_host_player_update_team(network_request_team *team);
extern void net_host_player_update(network_request_remote_update *update);

//
// client host pinging and joining
//

extern int net_client_join_host_start(char *name,char *game_name,char *map_name,int *tick_offset,int *option_flags,char *deny_reason,network_reply_join_remotes *remotes);
extern void net_client_join_host_end(void);

//
// client receiving messages
//

extern bool net_client_start_message_queue(char *err_str);
extern void net_client_end_message_queue(void);
extern bool net_client_start_message_queue_local(char *err_str);
extern void net_client_end_message_queue_local(void);
extern bool net_client_check_message_queue(int *action,int *player_uid,unsigned char *data);

//
// client sending messages
//

extern void net_client_send_ready(void);
extern void net_client_send_set_team(obj_type *obj);
extern void net_client_send_leave_host(void);
extern void net_client_send_latency_ping(int remote_uid);
extern void net_client_request_group_synch_ping(int remote_uid);
extern void net_client_send_spawn(obj_type *obj,int sub_event);
extern void net_client_send_death(obj_type *obj,bool telefrag);
extern void net_client_send_remote_update(obj_type *obj,bool chat_on);
extern void net_client_send_chat(obj_type *obj,char *str);
extern void net_client_send_sound(d3pnt *pnt,float pitch,char *name);
extern void net_client_send_projectile_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_hitscan_add(obj_type *obj,char *weap_name,char *proj_setup_name,d3pnt *pt,d3ang *ang);
extern void net_client_send_melee_add(obj_type *obj,char *weap_name,int radius,int distance,int damage,int force,d3pnt *pt,d3ang *ang);
extern void net_client_setup_pickup(obj_type *obj,network_request_remote_pickup *pickup);
extern void net_client_send_pickup(network_request_remote_pickup *pickup);
extern void net_client_send_click(obj_type *obj,d3pnt *pt,d3ang *ang);
