/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_server_mgr.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifndef __TCP_SERVER_MGR_H
#define __TCP_SERVER_MGR_H

#include "ring_buf.h"
#include "tcp_connection.h"

#define MAX_SERVER (8)
#define MAX_CLIENT_PER_SERVER (8)  //max is 16!!
#define CLIENT_FLAG_BEGIN                        (0x0001)
#define SERVER_SOCK_MASK (0x0000FFFF)
#define SERVER_SND_BUF_SIZE (2*TCP_WND)
#define SERVER_RCV_BUF_SIZE (TCP_WND)

typedef struct tcp_srv_struct
{
	u16_t srv_sock;
    u16_t cli_mask;
	u32_t in_use;
	struct tcp_pcb *srv_pcb;
	tcp_conn_args * client[MAX_CLIENT_PER_SERVER];
}tcp_server_struct;

void tcp_server_mgr_init(void);
void tcp_server_mgr_destroy(void);
s32_t tcp_server_mgr_get_free_sock(void);
void tcp_server_mgr_release_sock(s32_t srv_sock);
tcp_server_struct *tcp_server_mgr_get_struct(s32_t srv_sock);



s32_t tcp_conn_server_get_client_sock(tcp_server_struct *serv);
void tcp_conn_server_set_client_sock(tcp_server_struct *serv, s32_t sock, tcp_conn_args *cli);
void tcp_conn_server_clear_client_sock(tcp_server_struct *serv, s32_t sock);
err_t tcp_conn_server_accept(void *arg, struct tcp_pcb *pcb, err_t err);

#endif

