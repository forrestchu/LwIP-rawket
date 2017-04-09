/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_client_mgr.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifndef __TCP_CLIENT_MGR_H
#define __TCP_CLIENT_MGR_H

#include "ring_buf.h"
#include "tcp_connection.h"

#define MAX_CLIENT (16)         //max is 32!!
typedef struct tcp_cli_struct
{
    u32_t cli_mask;
	tcp_conn_args * client[MAX_CLIENT];
}tcp_client_struct;

void tcp_client_mgr_init(void);
void tcp_client_mgr_destroy(void);
s32_t tcp_client_mgr_get_client_sock(void);
void tcp_client_mgr_set_client_sock(s32_t sock, tcp_conn_args *cli);
void tcp_client_mgr_clear_client_sock(s32_t sock);
tcp_conn_args *tcp_client_mgr_get_conn_arg(s32_t sock);


#endif

