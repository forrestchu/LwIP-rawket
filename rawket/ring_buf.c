/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : ring_buf.c
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifdef RAWKET
#include "ring_buf.h"
#include "mem.h"

s32_t ring_buf_create(RING_BUF *ring, u32_t len)
{
    if(ring == NULL || len == 0 || len > MAX_RING_BUF)
    {
        return -1;
    }

    ring->buff = (u8_t *)mem_malloc((mem_size_t)len);
    if(ring->buff == NULL)
    {
        return -2;
    }

    ring->total = len;
    ring->start = 0;
    ring->end = 0;
    return 0;
}
s32_t ring_buf_used(RING_BUF *ring)
{
    if(ring->end == ring->start)
    {
        return 0;
    }

    if(ring->end > ring->start)
    {
        return (ring->end - ring->start);
    }
    else
    {
        return ring->end + (ring->total - ring->start);
    }
}

s32_t ring_buf_unused(RING_BUF *ring)
{
    if(ring->end == ring->start)
    {
        return ring->total;
    }

    if(ring->end > ring->start)
    {
        return ring->start + (ring->total - ring->end);
    }
    else
    {
        return (ring->start - ring->end);
    }
}

s32_t ring_buf_take(RING_BUF *ring, s8_t *buf, u32_t buf_len)
{
    u32_t remain = 0;
    u32_t ret = 0;
    u32_t i = 0;
    if(ring == NULL || buf == NULL || buf_len == 0)
    {
        //return -1;
        return 0;
    }

    if(ring->end == ring->start)
    {
        return 0;
    }

    if(buf_len== 0)
    {
        return 0;
    }

    if(ring->end > ring->start)
    {
        remain = ring->end - ring->start;
    }
    else
    {
        remain = ring->end + (ring->total - ring->start);
    }

    ret = buf_len < remain?buf_len:remain;
	
    if(ring->end > ring->start)
    {
        memcpy(buf, &ring->buff[ring->start], ret);
    }
    else
    {
        i = (ring->total - ring->start);
        if(i >= ret)
        {
            memcpy(buf, &ring->buff[ring->start], ret);
        }
        else
        {
            memcpy(buf, &ring->buff[ring->start], i);
            memcpy(&buf[i], &ring->buff[0], (ret - i));
        }

    }

    ring->start += ret;
    ring->start = ring->start%ring->total;

    return ret;

}
s32_t ring_buf_push(RING_BUF *ring, s8_t *buf, u32_t buf_len)
{
    u32_t free = 0;
    u32_t i = 0;
    if((ring->end + 1)%ring->total == ring->start)
    {
        //return -1;    //full
        return 0;
    }
	
    if(ring->end < ring->start)
    {
        free = ring->start - ring->end;
    }
    else
    {
        free = ring->start + (ring->total - ring->end);
    }

    if(buf_len > (free - 1))
    {
        //return -2; //no enough buffer
        return 0;
    }

    if(ring->end < ring->start)
    {
        memcpy(&ring->buff[ring->end], buf, buf_len);
    }
    else
    {
        i = (ring->total - ring->end);
        if(i >= buf_len)
        {
            memcpy(&ring->buff[ring->end], buf, buf_len);
        }
        else
        {
            memcpy(&ring->buff[ring->end], buf, i);
            memcpy(&ring->buff[0], &buf[i],  (buf_len - i));
        }
	
    }

    ring->end += buf_len;
    ring->end = ring->end%ring->total;

    return buf_len;
}
void ring_buf_delete(RING_BUF *ring)
{
    if(ring == NULL || ring->buff == NULL)
    {
        return;
    }

    mem_free(ring->buff);
    ring->buff = NULL;

    ring->total = 0;
    ring->start = 0;
    ring->end = 0;
}

void ring_buf_reset(RING_BUF *ring)
{
    ring->start = 0;
    ring->end = 0;
}

#endif //LWIP_TCP
