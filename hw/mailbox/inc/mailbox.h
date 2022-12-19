#ifndef __MAILBOX_H__
#define __MAILBOX_H__

#include "chip_mem_layout.h"

#define MAILBOX_A2B_ADDR  (Mailbox_BASE+0x000)
#define MAILBOX_B2A_ADDR  (Mailbox_BASE+0x028)
#define MAILBOX_LOCK_ADDR (Mailbox_BASE+0x100)

typedef struct {
    // 0x000
    volatile uint32_t a2b_int_en;
    // 0x004
    volatile uint32_t a2b_status;
    // 0x008
    volatile uint32_t a2b_cmd0;
    // 0x00c
    volatile uint32_t a2b_dat0;
    // 0x010
    volatile uint32_t a2b_cmd1;
    // 0x014
    volatile uint32_t a2b_dat1;
    // 0x018
    volatile uint32_t a2b_cmd2;
    // 0x01c
    volatile uint32_t a2b_dat2;
    // 0x020
    volatile uint32_t a2b_cmd3;
    // 0x024
    volatile uint32_t a2b_dat3;
} mailbox_a2b;

typedef struct {
    // 0x028
    volatile uint32_t b2a_int_en;
    // 0x02c
    volatile uint32_t b2a_status;
    // 0x030
    volatile uint32_t b2a_cmd0;
    // 0x034
    volatile uint32_t b2a_dat0;
    // 0x038
    volatile uint32_t b2a_cmd1;
    // 0x03c
    volatile uint32_t b2a_dat1;
    // 0x040
    volatile uint32_t b2a_cmd2;
    // 0x044
    volatile uint32_t b2a_dat2;
    // 0x048
    volatile uint32_t b2a_cmd3;
    // 0x04c
    volatile uint32_t b2a_dat3;
} mailbox_b2a;

typedef struct {
    volatile uint32_t atomic_lock[32];
} mailbox_lock;

#define MAILBOX_A2B  ((volatile mailbox_a2b*)MAILBOX_A2B_ADDR)
#define MAILBOX_B2A  ((volatile mailbox_b2a*)MAILBOX_B2A_ADDR)
#define MAILBOX_LOCK ((volatile mailbox_lock*)MAILBOX_LOCK_ADDR)

void a2b_send(int ch, uint32_t cmd, uint32_t data);
uint32_t a_get_cmd(int ch);
uint32_t a_get_data(int ch);

void b2a_send(int ch, uint32_t cmd, uint32_t data);
uint32_t b_get_cmd(int ch);
uint32_t b_get_data(int ch);

#endif // __MAILBOX_H__
