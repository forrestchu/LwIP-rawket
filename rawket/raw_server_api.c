/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : raw_server_api.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "raw_server_api.h"
void server_init_manager(void)
{
    tcp_server_mgr_init();
}
void server_deinit_manager(void)
{
    tcp_server_mgr_destroy();
}

void tcp_srv_err(void *arg, err_t err)
{
    tcp_server_struct * srv = (tcp_server_struct *)arg;
    srv->srv_pcb = NULL;
    server_close(srv->srv_sock);
}

s32_t server_open(u16_t local_port)
{
    struct tcp_pcb *pcb = NULL;
    struct tcp_pcb *lpcb = NULL;	
    s32_t sock = -1;
    tcp_server_struct * srv = NULL;
    do
    {
        sock = tcp_server_mgr_get_free_sock();
        if(sock < 0)
        {
            break;
        }

        pcb = tcp_new();
        if(pcb == NULL)
        {
            sock = -2;
            break;
        }

        if(tcp_bind(pcb, IP_ADDR_ANY, local_port) != ERR_OK)
        {
            tcp_close(pcb);
            sock = -3;
            break;
        }

        lpcb = tcp_listen(pcb);
        if(lpcb == NULL)
        {
            tcp_close(pcb);
            sock = -4;
            break;
        }

        tcp_accept(lpcb, tcp_conn_server_accept);
        srv = tcp_server_mgr_get_struct(sock);
        srv->in_use = 1;
        srv->srv_pcb = lpcb;
        srv->cli_mask = 0;
        srv->srv_sock = sock;
        tcp_arg(lpcb, (void *)srv);
        //tcp_err(lpcb, tcp_srv_err);          //we can not set an error callback for LISTEN pcbs
    }while(0);

    return sock;
}

s32_t server_check_client(s32_t srv_sock)
{
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }

    return srv->cli_mask;
}

void server_close(s32_t srv_sock)
{
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    tcp_conn_args * conn_arg = NULL;
    struct tcp_pcb * pcb = NULL;
    s32_t i = 0;
    if(srv == NULL || srv->in_use == 0)
    {
        return;
    }

    for(i = 0; i < MAX_CLIENT_PER_SERVER; i++)
    {
        if(((CLIENT_FLAG_BEGIN<<i) & srv->cli_mask) == 1)
        {
            conn_arg = srv->client[i];
            pcb = conn_arg->pcb;
            tcp_close(pcb);
            tcp_recv(pcb, NULL);
            tcp_sent(pcb, NULL);
            tcp_poll(pcb, NULL, 1);
            tcp_err(pcb, NULL);
            tcp_arg(pcb, NULL);
            tcp_conn_del_arg(conn_arg);
            tcp_conn_server_clear_client_sock(srv, i);
        }
    }

    if(srv->srv_pcb)
    {
        tcp_close(srv->srv_pcb);
        tcp_accept(srv->srv_pcb, NULL);
        srv->srv_pcb = NULL;
    }

    srv->cli_mask = 0;
    srv->in_use = 0;

}

//cli_sock consist of 16bit srv_sock and 16bit client socket
struct ip_addr server_get_client_ip(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
    struct ip_addr ip_zero = {0};
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return ip_zero;
    }
	
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return ip_zero;
    }
	
    return srv->client[cli]->dst_ip;

}
u16_t server_get_client_port(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;

    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }
	
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return -1;
    }
	
    return srv->client[cli]->dst_port;
}
void server_close_client(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
	
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    struct tcp_pcb * pcb = NULL;
	
    if(srv == NULL || srv->in_use == 0)
    {
        return;
    }
		
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return;
    }

    conn_arg = srv->client[cli];
    pcb = conn_arg->pcb;
    if(pcb)
    {
        tcp_close(pcb);
        tcp_recv(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_poll(pcb, NULL, 1);
        tcp_err(pcb, NULL);
        tcp_arg(pcb, NULL);
    }
	
	tcp_conn_del_arg(conn_arg);
	tcp_conn_server_clear_client_sock(srv, cli);

}
SRV_STATUS server_check_client_status(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
	
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return SRV_ERR;
    }
	   
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return SRV_ERR;
    }

    conn_arg = srv->client[cli];
    if(conn_arg->conn_status == CONN_SERVER_CONNECTED)
    {
        return SRV_CONNECTED;
    }
    else if(conn_arg->conn_status == CONN_SERVER_CLOSED)
    {
        return SRV_CLOSED;
    }
    else
    {
        return SRV_ERR;
    }

}


s32_t server_check_client_snd_buf(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
		
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }
		   
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return -1;
    }
	
    conn_arg = srv->client[cli];

    if(conn_arg->conn_status != CONN_SERVER_CONNECTED)
    {
        return -2;
    }

    return tcp_conn_snd_buf_unused(conn_arg);

}
s32_t server_check_client_recv_buf(s32_t cli_sock)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
		
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }
		   
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return -1;
    }
	
    conn_arg = srv->client[cli];

    if(conn_arg->conn_status != CONN_SERVER_CONNECTED)
    {
        return -2;
    }

    return tcp_conn_rcv_buf_used(conn_arg);

}
s32_t server_snd_client_data(s32_t cli_sock, s8_t *buf, s32_t len)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
		
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }
		   
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return -1;
    }
	
    conn_arg = srv->client[cli];

    if(conn_arg->conn_status != CONN_SERVER_CONNECTED)
    {
        return -2;
    }

    return tcp_conn_send_data(conn_arg, buf, len);
}
s32_t server_recv_client_data(s32_t cli_sock, s8_t *buf, s32_t len)
{
    s32_t srv_sock = cli_sock & SERVER_SOCK_MASK;
    s32_t cli = (cli_sock>>16) & SERVER_SOCK_MASK;
		
    tcp_conn_args * conn_arg = NULL;
    tcp_server_struct * srv = tcp_server_mgr_get_struct(srv_sock);
    if(srv == NULL || srv->in_use == 0)
    {
        return -1;
    }
		   
    if(((CLIENT_FLAG_BEGIN<<cli)&srv->cli_mask)== 0 || srv->client[cli] == NULL)
    {
        return -1;
    }
	
    conn_arg = srv->client[cli];

    if(conn_arg->conn_status != CONN_SERVER_CONNECTED)
    {
        return -2;
    }

    return tcp_conn_recv_data(conn_arg, buf, len);

}

#endif //LWIP_TCP
