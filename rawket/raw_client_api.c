/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : raw_client_api.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "raw_client_api.h"
void client_init_manager(void)
{
    tcp_client_mgr_init();
}
void client_deinit_manager(void)
{
    tcp_client_mgr_destroy();
}

s32_t client_connect_to_server(struct ip_addr dst_ip, u16_t dst_port)
{
    s32_t sock = -1;
    struct tcp_pcb *pcb = NULL;	            		
    tcp_conn_args * cli = NULL;  
  
    do
    {
        sock = tcp_client_mgr_get_client_sock();
        if(sock < 0)
        {
            sock = -1;
            break;
        }

        cli = tcp_conn_new_arg(CLIENT_SND_BUF_SIZE, CLIENT_RCV_BUF_SIZE);
        if(cli == NULL)
        {
            sock = -2;
            break;

        }

        pcb = tcp_new();
        if(pcb == NULL)
        {
            tcp_conn_del_arg(cli);
            sock = -3;
            break;

        }

        if(tcp_connect(pcb, &dst_ip, dst_port, tcp_conn_client_connected) != ERR_OK)
        {
            tcp_close(pcb);
            tcp_conn_del_arg(cli);
            sock = -4;
            break;
        }

        ip_set_option(pcb, SOF_KEEPALIVE);
        tcp_err(pcb, tcp_conn_err);
        tcp_arg(pcb, (void *)cli);

        cli->conn_status = CONN_CLIENT_CONNECTING;
        cli->dst_ip = dst_ip;
        cli->dst_port = dst_port;
        cli->pcb = pcb;

        tcp_client_mgr_set_client_sock(sock, cli);
        return sock;
		
    }while(0);

    return sock;
}

CLI_STATUS client_status_check(s32_t sock)
{
    CLI_STATUS ret = CLI_ERR;
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return ret;
    }

    switch(cli->conn_status)
    {
        case CONN_CLIENT_CONNECTING:
            ret = CLI_CONNECTING;
            break;
        case CONN_CLIENT_CONNECTED:
            ret = CLI_CONNECTED;
            break;
        case CONN_CLIENT_RECONNECTING:
            ret = CLI_RECONNECTING;
            break;
        case CONN_CLIENT_CLOSED:
            ret = CLI_CLOSED;
            break;
        default:
            break;
    }

    return ret;
}
void client_close(s32_t sock)
{
    struct tcp_pcb *pcb = NULL;
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return;
    }

    pcb = cli->pcb;
    if(pcb)
    {
        tcp_close(pcb);
        tcp_recv(pcb, NULL);
        tcp_sent(pcb, NULL);
        tcp_poll(pcb, NULL, 1);
        tcp_err(pcb, NULL);
        tcp_arg(pcb, NULL);
    }
	
    tcp_conn_del_arg(cli);
    tcp_client_mgr_clear_client_sock(sock);

}
s32_t client_check_snd_buf(s32_t sock)
{
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return -1;
    }

    if(cli->conn_status == CONN_CLIENT_CLOSED)
    {
        return -2;
    }

    return tcp_conn_snd_buf_unused(cli);
	
}
s32_t client_check_recv_buf(s32_t sock)
{
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return -1;
    }
	
    if(cli->conn_status == CONN_CLIENT_CLOSED)
    {
        return -2;
    }
	
    return tcp_conn_rcv_buf_used(cli);
}
s32_t client_snd_data(s32_t sock, s8_t *buf, s32_t len)
{
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return -1;
    }

    if(cli->conn_status == CONN_CLIENT_CLOSED)
    {
        return -2;
    }

    return tcp_conn_send_data(cli, buf, len);

}
s32_t client_recv_data(s32_t sock, s8_t *buf, s32_t len)
{
    tcp_conn_args *cli = tcp_client_mgr_get_conn_arg(sock);
    if(cli == NULL)
    {
        return -1;
    }
	
    if(cli->conn_status == CONN_CLIENT_CLOSED)
    {
        return -2;
    }
	
    return tcp_conn_recv_data(cli, buf, len);
}

#endif //LWIP_TCP
