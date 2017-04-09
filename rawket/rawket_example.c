/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : rawket_example.c
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
#include "raw_server_api.h"

void raw_socket_client_demo(void)
{
    static CLI_STATUS client_status = CLI_INIT;
    static int client_sock = -1;
    struct ip_addr dst_ip;
    #define DALEN 512
    char data[DALEN];
    int len = 0;
		
    if(client_status == CLI_INIT)
    {
        IP4_ADDR(&dst_ip,192,168,1,11);
        client_sock = client_connect_to_server(dst_ip, 8081);
        if(client_sock < 0)
        {
            client_status = CLI_ERR;
        }
        else
        {
            client_status = CLI_CONNECTING;
        }
    }
    else if(client_status == CLI_CONNECTING || client_status == CLI_RECONNECTING)
    {
        client_status = client_status_check(client_sock);
        printf("rawket client: 8081 CLI_CONNECTING\n");
    }
    else if(client_status == CLI_CONNECTED)
    {
        len = client_recv_data(client_sock, (s8_t *)data, DALEN);
        if(len > 0)
        {
            client_snd_data(client_sock, (s8_t *)data, len);//echo
        }
        else
        {
            client_status = client_status_check(client_sock);
        }
    }
    else if(client_status == CLI_CLOSED)
    {
        client_close(client_sock);
        client_sock = -1;
        // stop client. or set to CLI_INIT to restart client
        client_status = CLI_ERR; 
        printf("rawket client: close socket\n");
    }
    else
    {
        printf("rawket client: connect error\n");
    }
    
}
void raw_socket_server_demo(void)
{
    static int srv = -1;
    static int srv_stat = SRV_INIT;
    #define DALEN 512
    char data[DALEN];
    int len = 0;
    int cli_map = 0;
    int i = 0;
    int cli_sock = 0;
    int cli_stat = SRV_INIT;
	
    if(srv_stat == SRV_INIT)
    {
        srv = server_open(8888);
        srv_stat = (srv < 0)?SRV_ERR : SRV_CONNECTED;
        printf("rawket server: server opened\n");
    }
    else if(srv_stat == SRV_CONNECTED)
    {
        cli_map = server_check_client(srv);
        if(cli_map < 0)
        {
            srv_stat = SRV_ERR;
            server_close(srv);
        }
        else if(cli_map == 0)
        {
            //no client connected
            //printf("rawket server: server no connections\n");
        }
        else
        {
            //process all clients
            for(i = 0; i < MAX_CLIENT_PER_SERVER; i++)
            {
                if(((CLIENT_FLAG_BEGIN<<i) & cli_map) != 0)  //check  whether client  i is valid. 
                {
                    cli_sock = CLIENT_SOCK(srv, i);
                    cli_stat = server_check_client_status(cli_sock);
                    if(cli_stat == SRV_CONNECTED)                     //read data from client and echo it.
                    {
                        len = server_recv_client_data(cli_sock, (s8_t *)data, DALEN);
                        if(len > 0)
                        {
                            server_snd_client_data(cli_sock, (s8_t *)data, len);     //echo, ignore the result here?
                        }
                    }
                    else
                    {
                        printf("rawket server: server close client\n");
                        server_close_client(cli_sock);
                    }
                }
            }

        }

    }
    else
    {
        printf("rawket server: server error\n");
    }
}

#endif //LWIP_TCP
