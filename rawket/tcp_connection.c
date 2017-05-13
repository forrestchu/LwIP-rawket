/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : tcp_connection.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "tcp_connection.h"

tcp_conn_args *tcp_conn_new_arg(u32_t snd_buf_len, u32_t rcv_buf_len)
{
    tcp_conn_args *ret = NULL;
    ret = (tcp_conn_args *)mem_malloc(sizeof(tcp_conn_args));
    if(ret == NULL)
    {
        return NULL;
    }
	
    do
    {
        if(ring_buf_create(&ret->snd_buf, snd_buf_len) < 0)
        {
            mem_free(ret);
            ret = NULL;
            break;
        }

        if(ring_buf_create(&ret->rcv_buf, rcv_buf_len) < 0)
        {
            ring_buf_delete(&ret->snd_buf);
            mem_free(ret);
            ret = NULL;
            break;
        }

        return ret;
    }while(0);

    return NULL;
}
void tcp_conn_del_arg(tcp_conn_args * arg)
{
    if(arg == NULL)
    {
        return;
    }

    ring_buf_delete(&arg->snd_buf);
    ring_buf_delete(&arg->rcv_buf);
    mem_free(arg);
}

void tcp_conn_client_reconnect(tcp_conn_args * arg)
{
    struct tcp_pcb *pcb = NULL; 					   
    struct ip_addr server_ip;
 
    pcb = tcp_new();
    if(pcb == NULL)
    {
        arg->conn_status = CONN_CLIENT_CLOSED;
	    return;
    }

    if(tcp_connect(pcb, &arg->dst_ip, arg->dst_port, tcp_conn_client_connected) != ERR_OK)
    {
       memp_free(MEMP_TCP_PCB, pcb);
	   arg->conn_status = CONN_CLIENT_CLOSED;
	   return;
    }
 
    //ip_set_option(pcb, SOF_KEEPALIVE);
    tcp_err(pcb, tcp_conn_err);
    tcp_arg(pcb, (void *)arg);
    arg->conn_status = CONN_CLIENT_RECONNECTING;
    arg->pcb = pcb;
    ring_buf_reset(&arg->rcv_buf);
    ring_buf_reset(&arg->snd_buf);   //keep all data in send buffer
}


void tcp_conn_err(void *arg, err_t err)
{
    tcp_conn_args * args = (tcp_conn_args *)arg;
    if(args == NULL)
    {
        return;
    }
	
    if(args->conn_status & CONN_TYPE_CLIENT)
    {
	    
        args->pcb = NULL;
		//don't reconnect when there is RST error from server, to avoid multiple reconnecting clients.
		if(err != ERR_RST)
		{
            tcp_conn_client_reconnect(args);
		}
		else
		{
			args->conn_status = CONN_CLIENT_CLOSED;
		}
    }
    else
    {
        args->pcb = NULL;
        args->conn_status = CONN_SERVER_CLOSED;
    }

    if(args->conn_status == CONN_CLIENT_CLOSED || args->conn_status == CONN_SERVER_CLOSED)
    {
        ring_buf_delete(&args->snd_buf);
        ring_buf_delete(&args->rcv_buf);
		
    }

    return;
}


err_t tcp_conn_recv(void *arg, struct tcp_pcb *pcb, struct pbuf *p, err_t err)
{
    s32_t close_flag = 0;
    struct pbuf *q = NULL;
    tcp_conn_args * args = (tcp_conn_args *)arg;
	
    if(p != NULL) 
    {		 
        //tcp_recved(pcb, p->tot_len);
        q = p;
        while(q != NULL)
        {
            if(ring_buf_push(&args->rcv_buf, (s8_t *)q->payload, q->len) < 0)
            {
                close_flag = 1;
                break;
            }

            q = q->next;
        }
      
        pbuf_free(p);
	  
    } 
    else if (err == ERR_OK) 
    {//closed by peer
        close_flag = 1;
    }

    if(close_flag == 1)
    {
        tcp_poll(pcb, NULL, 0);
        tcp_close(pcb);
        tcp_conn_err((void *)args, ERR_OK);
    }
	
    return ERR_OK;
}
err_t tcp_conn_try_send(struct tcp_pcb *pcb, tcp_conn_args * arg)
{
    err_t ret = ERR_OK;
    u16_t buf_len = 0;
    u32_t len = 0;
    s8_t * send_buffer = NULL;
    u16_t remain = tcp_sndbuf(pcb);

    if(remain == 0)
    {
        return ret;
    }

    buf_len = remain > TCP_MSS?TCP_MSS:remain;
    send_buffer = (s8_t *)mem_malloc(buf_len);
    if(send_buffer == NULL)
    {
        return ERR_MEM;
    }

    len = ring_buf_take(&arg->snd_buf, send_buffer, buf_len);
    if(len > 0)
    {
        ret = tcp_write(pcb, send_buffer, len, 1);
        tcp_output(pcb);
    }
	
    mem_free(send_buffer);
    return ret;
}

err_t tcp_conn_sent(void *arg, struct tcp_pcb *pcb, u16_t len)
{
	//add your code here
	 
    return tcp_conn_poll(arg, pcb);
}
err_t tcp_conn_poll(void *arg, struct tcp_pcb *pcb)
{
    err_t ret = ERR_OK;

    ret = tcp_conn_try_send(pcb, arg);
    if(ret != ERR_OK && ret != ERR_MEM)
    {
        tcp_poll(pcb, NULL, 0);
        tcp_close(pcb);
        tcp_conn_err((void *)arg, ERR_OK);
    }
	 
    return ERR_OK;
}
err_t tcp_conn_client_connected(void *arg, struct tcp_pcb *pcb, err_t err)
{
    tcp_conn_args * cli = (tcp_conn_args *)arg;
	
    cli->conn_status = CONN_CLIENT_CONNECTED;
    ip_set_option(pcb, SOF_KEEPALIVE);
    tcp_recv(pcb, tcp_conn_recv);
    tcp_sent(pcb, tcp_conn_sent);
    tcp_poll(pcb, tcp_conn_poll, 1);
	
    return tcp_conn_poll(arg, pcb);
}

s32_t tcp_conn_snd_buf_unused(tcp_conn_args * arg)
{
    return ring_buf_unused(&arg->snd_buf);
}

s32_t tcp_conn_rcv_buf_used(tcp_conn_args * arg)
{
    return ring_buf_used(&arg->rcv_buf);
}

s32_t tcp_conn_send_data(tcp_conn_args * arg, s8_t *buf, s32_t len)
{
    //return ring_buf_push(&arg->snd_buf, buf, len);
    s32_t ret = ring_buf_push(&arg->snd_buf, buf, len);
    tcp_conn_try_send(arg->pcb, arg);

    return ret;
}

s32_t tcp_conn_recv_data(tcp_conn_args * arg, s8_t *buf, s32_t len)
{
    s32_t ret = ring_buf_take(&arg->rcv_buf, buf, len);
    if(ret > 0)
    {
        tcp_recved(arg->pcb, (u16_t)ret);
    }
	
    return ret;
}

#endif //LWIP_TCP
