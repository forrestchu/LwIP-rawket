/*
*********************************************************************************************************
*                                     lwIP TCP/IP Stack & Rawket
*                               a socket-like API based on LwIP raw API
*
* File : ring_buf.h
*
* Author : WuMu (forrest)
*
* Contact: 540535649@qq.com
*
*          easynetworking.cn, Nanjing, P.R.China, <2017.01>
*********************************************************************************************************
*/

#ifndef __RING_BUF_H
#define __RING_BUF_H
//#include "system_Include.h"
//#include "stm32f4xx.h"
#include "cc.h"
#include "mem.h"
#include "memp.h"
#include <string.h>

#define MAX_RING_BUF (8*1024)
typedef struct ringbuf
{
    u32_t start;
	u32_t end;
	u32_t total;
	u8_t * buff;
}RING_BUF;

s32_t ring_buf_create(RING_BUF *ring, u32_t len);
s32_t ring_buf_used(RING_BUF *ring);
s32_t ring_buf_unused(RING_BUF *ring);

s32_t ring_buf_take(RING_BUF *ring, s8_t *buf, u32_t buf_len);
s32_t ring_buf_push(RING_BUF *ring, s8_t *buf, u32_t buf_len);
void ring_buf_delete(RING_BUF *ring);
void ring_buf_reset(RING_BUF *ring);

#endif
