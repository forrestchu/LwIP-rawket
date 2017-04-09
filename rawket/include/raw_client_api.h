/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : raw_client_api.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/
#ifndef __RAW_CLIENT_API_H
#define __RAW_CLIENT_API_H

#include "tcp_client_mgr.h"

#define CLIENT_SND_BUF_SIZE (2*TCP_WND)
#define CLIENT_RCV_BUF_SIZE (TCP_WND)

//only call this once to init/deinit client manager in main
void client_init_manager(void);
void client_deinit_manager(void);

//client socket api based on raw, only for tcp client operations.
typedef enum cli_status
{
    CLI_INIT,
	CLI_CONNECTING,
	CLI_CONNECTED,
	CLI_RECONNECTING,
	CLI_CLOSED,
	CLI_ERR
}CLI_STATUS;

s32_t client_connect_to_server(struct ip_addr dst_ip, u16_t dst_port);
CLI_STATUS client_status_check(s32_t sock);
void client_close(s32_t sock);
s32_t client_check_snd_buf(s32_t sock);
s32_t client_check_recv_buf(s32_t sock);
s32_t client_snd_data(s32_t sock, s8_t *buf, s32_t len);
s32_t client_recv_data(s32_t sock, s8_t *buf, s32_t len);

#endif

