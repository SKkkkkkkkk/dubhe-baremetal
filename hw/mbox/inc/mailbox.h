#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include "chip_define.h"

#define MAILBOX_A2B_ADDR  (Mailbox_BASE+0x000)
#define MAILBOX_B2A_ADDR  (Mailbox_BASE+0x028)
#define MAILBOX_LOCK_ADDR (Mailbox_BASE+0x100)

typedef struct {
    // 0x000
    uint32_t a2b_int_en;
    // 0x004
    uint32_t a2b_status;
    // 0x008
    uint32_t a2b_cmd0;
    // 0x00c
    uint32_t a2b_dat0;
    // 0x010
    uint32_t a2b_cmd1;
    // 0x014
    uint32_t a2b_dat1;
    // 0x018
    uint32_t a2b_cmd2;
    // 0x01c
    uint32_t a2b_dat2;
    // 0x020
    uint32_t a2b_cmd3;
    // 0x024
    uint32_t a2b_dat3;
} mailbox_a2b;

typedef struct {
   
    // 0x028
    uint32_t b2a_int_en;
    // 0x02c
    uint32_t b2a_status;
    // 0x030
    uint32_t b2a_cmd0;
    // 0x034
    uint32_t b2a_dat0;
    // 0x038
    uint32_t b2a_cmd1;
    // 0x03c
    uint32_t b2a_dat1;
    // 0x040
    uint32_t b2a_cmd2;
    // 0x044
    uint32_t b2a_dat2;
    // 0x048
    uint32_t b2a_cmd3;
    // 0x04c
    uint32_t b2a_dat3;
} mailbox_b2a;

typedef struct {
    uint32_t atomic_lock[32];
    /*// 0x100
    uint32_t atomic_lock00;
    // 0x104
    uint32_t atomic_lock01;
    // 0x108
    uint32_t atomic_lock02;
    // 0x10c
    uint32_t atomic_lock03;
    // 0x110
    uint32_t atomic_lock04;
    // 0x114
    uint32_t atomic_lock05;
    // 0x118
    uint32_t atomic_lock06;
    // 0x11c
    uint32_t atomic_lock07;
    // 0x120
    uint32_t atomic_lock08;
    // 0x124
    uint32_t atomic_lock09;
    // 0x128
    uint32_t atomic_lock10;
    // 0x12c
    uint32_t atomic_lock11;
    // 0x130
    uint32_t atomic_lock12;
    // 0x134
    uint32_t atomic_lock13;
    // 0x138
    uint32_t atomic_lock14;
    // 0x13c
    uint32_t atomic_lock15;
    // 0x140
    uint32_t atomic_lock16;
    // 0x144
    uint32_t atomic_lock17;
    // 0x148
    uint32_t atomic_lock18;
    // 0x14c
    uint32_t atomic_lock19;
    // 0x150
    uint32_t atomic_lock20;
    // 0x154
    uint32_t atomic_lock21;
    // 0x158
    uint32_t atomic_lock22;
    // 0x15c
    uint32_t atomic_lock23;
    // 0x160
    uint32_t atomic_lock24;
    // 0x164
    uint32_t atomic_lock25;
    // 0x168
    uint32_t atomic_lock26;
    // 0x16c
    uint32_t atomic_lock27;
    // 0x170
    uint32_t atomic_lock28;
    // 0x174
    uint32_t atomic_lock29;
    // 0x178
    uint32_t atomic_lock30;
    // 0x17c
    uint32_t atomic_lock31;*/
} mailbox_lock;

#define MAILBOX_A2B  ((volatile mailbox_a2b*)MAILBOX_A2B_ADDR)
#define MAILBOX_B2A  ((volatile mailbox_b2a*)MAILBOX_B2A_ADDR)
#define MAILBOX_LOCK ((volatile mailbox_lock*)MAILBOX_LOCK_ADDR)



void a_send_message(int id,int cmd,int dat);
void a_recv_cmd(int id);
void a_recv_dat(int id);
void b_send_message(int id,int cmd,int dat);
void b_recv_cmd(int id);
void b_recv_dat(int id);

#endif // __MAILBOX_H__

// vim:set ts=4 sw=4 et fenc=utf-8 fdm=marker:
