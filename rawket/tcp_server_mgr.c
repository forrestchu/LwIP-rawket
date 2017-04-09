/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_server_mgr.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "tcp_server_mgr.h"

tcp_server_struct *server_mgr = NULL;
void tcp_server_mgr_init(void)
{
    server_mgr = (tcp_server_struct *)mem_malloc(sizeof(tcp_server_struct)*MAX_SERVER);
    if(server_mgr == NULL)
    {
        return;
    }

    memset((void *)server_mgr, 0, sizeof(tcp_server_struct)*MAX_SERVER);
}
void tcp_server_mgr_destroy(void)
{
    if(server_mgr != NULL)
    {
        mem_free(server_mgr);
        server_mgr = NULL;
    }
}

s32_t tcp_server_mgr_get_free_sock(void)
{
    s32_t i = 0;
    for(i = 0; i < MAX_SERVER; i++)
    {
        if(server_mgr[i].in_use == 0)
        {
            return i;
        }
    }

    return -1;
}

void tcp_server_mgr_release_sock(s32_t srv_sock)
{
    server_mgr[srv_sock].in_use = 0;
}

tcp_server_struct *tcp_server_mgr_get_struct(s32_t srv_sock)
{

    s32_t i = srv_sock&SERVER_SOCK_MASK;

    if(i < 0 || i >= MAX_SERVER)
    {
        return NULL;
    }

    if(server_mgr == NULL)
    {
        return NULL;
    }

    return &server_mgr[i];
}


s32_t tcp_conn_server_get_client_sock(tcp_server_struct *serv)
{
    s32_t i = 0;
	
    if(serv == NULL)
    {
        return -1;
    }

    for(i = 0; i < MAX_CLIENT_PER_SERVER; i++)
    {
        if(((0x0001<<i) & serv->cli_mask) == 0)
        {
            return i;
        }
    }

    return -1;
}

void tcp_conn_server_set_client_sock(tcp_server_struct *serv, s32_t sock, tcp_conn_args *cli)
{
    if(serv == NULL || sock >= MAX_CLIENT_PER_SERVER || cli == NULL)
    {
        return;
    }

    serv->cli_mask |= (0x0001<<sock);

    serv->client[sock] = cli;
    
    return;
}
void tcp_conn_server_clear_client_sock(tcp_server_struct *serv, s32_t sock)
{
    if(serv == NULL || sock >= MAX_CLIENT_PER_SERVER)
    {
        return;
    }
	
    serv->cli_mask &= ~(0x0001<<sock);
	
    serv->client[sock] = NULL;

    return;
}

err_t tcp_conn_server_accept(void *arg, struct tcp_pcb *pcb, err_t err)
{
    tcp_server_struct *serv = (tcp_server_struct *)arg;
    s32_t cli_sock = 0;
    tcp_conn_args *cli = NULL;
    do
    {
        if(serv == NULL)
        {
            break;
        }

        cli_sock = tcp_conn_server_get_client_sock(serv);
        if(cli_sock < 0)
        {
            break;
        }

        cli = tcp_conn_new_arg(SERVER_SND_BUF_SIZE, SERVER_RCV_BUF_SIZE);
        if(cli == NULL)
        {
            break;
        }

        cli->conn_status = CONN_SERVER_CONNECTED;
        cli->dst_ip = pcb->remote_ip;
        cli->dst_port = pcb->remote_port;
        cli->pcb = pcb;

        tcp_recv(pcb, tcp_conn_recv);
        tcp_sent(pcb, tcp_conn_sent);
        tcp_poll(pcb, tcp_conn_poll, 1);
        tcp_arg(pcb, (void *)cli);
        tcp_err(pcb, tcp_conn_err);
        
        tcp_conn_poll((void *)cli, pcb);

        tcp_conn_server_set_client_sock(serv, cli_sock, cli);

        return ERR_OK;

    }while(0);

    tcp_close(pcb);
    return ERR_MEM;
	
}


#endif //LWIP_TCP
