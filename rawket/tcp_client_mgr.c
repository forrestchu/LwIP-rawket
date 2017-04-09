/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_client_mgr.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "tcp_client_mgr.h"

tcp_client_struct *client_mgr = NULL;

void tcp_client_mgr_init(void)
{

    client_mgr = (tcp_client_struct *)mem_malloc(sizeof(tcp_client_struct));
    if(client_mgr == NULL)
    {
        return;
    }

    memset((void *)client_mgr, 0, sizeof(tcp_client_struct));
}
void tcp_client_mgr_destroy(void)
{
    if(client_mgr != NULL)
    {
        mem_free(client_mgr);
        client_mgr = NULL;
    }
}

s32_t tcp_client_mgr_get_client_sock(void)
{
    s32_t i = 0;
	
    for(i = 0; i < MAX_CLIENT; i++)
    {
        if(((0x0001<<i) & client_mgr->cli_mask) == 0)
        {
            return i;
        }
    }

    return -1;
}

void tcp_client_mgr_set_client_sock(s32_t sock, tcp_conn_args *cli)
{
    if(sock >= MAX_CLIENT || cli == NULL)
    {
        return;
    }

    client_mgr->cli_mask |= (0x0001<<sock);

    client_mgr->client[sock] = cli;

    return;
}
void tcp_client_mgr_clear_client_sock(s32_t sock)
{
    if(sock < 0 || sock >= MAX_CLIENT)
    {
        return;
    }
	
    client_mgr->cli_mask &= ~(0x0001<<sock);
	
    client_mgr->client[sock] = NULL;

    return;
}

tcp_conn_args *tcp_client_mgr_get_conn_arg(s32_t sock)
{
    if(sock < 0 || sock >= MAX_CLIENT)
    {
        return NULL;
    }

    return client_mgr->client[sock];
}

#endif //LWIP_TCP
