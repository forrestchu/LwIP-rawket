/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : raw_server_api.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifndef __RAW_SERVER_API_H
#define __RAW_SERVER_API_H

#include "tcp_server_mgr.h"

//only call this once to init/deinit server manager in main
void server_init_manager(void);
void server_deinit_manager(void);

//client socket api based on raw, only for tcp server operations.
typedef enum srv_status
{
    SRV_INIT,
	SRV_CONNECTED,
	SRV_CLOSED,
	SRV_ERR
}SRV_STATUS;

s32_t server_open(u16_t local_port);
s32_t server_check_client(s32_t srv_sock);
void server_close(s32_t srv_sock);  //close all client connections on server

#define CLIENT_SOCK(srv, cli) ((cli<<16) + srv)
//cli_sock should be created by CLIENT_SOCK()
struct ip_addr server_get_client_ip(s32_t cli_sock);
u16_t server_get_client_port(s32_t cli_sock);
void server_close_client(s32_t cli_sock);
SRV_STATUS server_check_client_status(s32_t cli_sock);


s32_t server_check_client_snd_buf(s32_t cli_sock);
s32_t server_check_client_recv_buf(s32_t cli_sock);
s32_t server_snd_client_data(s32_t cli_sock, s8_t *buf, s32_t len);
s32_t server_recv_client_data(s32_t cli_sock, s8_t *buf, s32_t len);

#endif

