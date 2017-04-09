/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_connection.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifndef __TCP_CONNECTION_H
#define __TCP_CONNECTION_H

#include "lwip/netif.h"
#include "lwip/ip.h"
#include "lwip/tcp.h"
#include "lwip/init.h"
#include "lwip/pbuf.h"
#include "lwip/inet.h"

#include "ring_buf.h"
#define CONN_TYPE_CLIENT 0x8000
#define CONN_TYPE_SERVER 0x0000

#define CONN_CLIENT_INIT (CONN_TYPE_CLIENT + 0x0000)
#define CONN_CLIENT_CONNECTING (CONN_TYPE_CLIENT + 0x0100)
#define CONN_CLIENT_CONNECTED (CONN_TYPE_CLIENT + 0x0200)
#define CONN_CLIENT_RECONNECTING (CONN_TYPE_CLIENT + 0x0400)
#define CONN_CLIENT_CLOSED (CONN_TYPE_CLIENT + 0x0800)

#define CONN_SERVER_INIT (CONN_TYPE_SERVER + 0x0000)
#define CONN_SERVER_CONNECTED (CONN_TYPE_SERVER + 0x0200)
#define CONN_SERVER_CLOSED (CONN_TYPE_SERVER + 0x0800)


typedef struct tcp_conn_arg
{
	struct ip_addr dst_ip;	  //dst ip
	u16_t dst_port; 			  //dst port
    u16_t conn_status;
	struct tcp_pcb *pcb;
    RING_BUF snd_buf;
    RING_BUF rcv_buf;
}tcp_conn_args;

//adjust the send buffer and recv buffer according to your application.
tcp_conn_args *tcp_conn_new_arg(u32_t snd_buf_len, u32_t rcv_buf_len);
void tcp_conn_del_arg(tcp_conn_args * arg);
void tcp_conn_client_reconnect(tcp_conn_args * arg);

void tcp_conn_err(void *arg, err_t err);
err_t tcp_conn_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err);
err_t tcp_conn_sent(void *arg, struct tcp_pcb *pcb, u16_t len);
err_t tcp_conn_poll(void *arg, struct tcp_pcb *pcb);
err_t tcp_conn_client_connected(void *arg, struct tcp_pcb *pcb, err_t err);
err_t tcp_conn_server_accept(void *arg, struct tcp_pcb *pcb, err_t err);
s32_t tcp_conn_snd_buf_unused(tcp_conn_args * arg);
s32_t tcp_conn_rcv_buf_used(tcp_conn_args * arg);
s32_t tcp_conn_send_data(tcp_conn_args * arg, s8_t *buf, s32_t len);
s32_t tcp_conn_recv_data(tcp_conn_args * arg, s8_t *buf, s32_t len);

#endif

