#include <stdio.h>
#include <stdint.h>
#include "mailbox.h"


void a2b_send(int ch,uint32_t cmd,uint32_t data)
{
    switch (ch)
    {
        case 0:
            MAILBOX_A2B -> a2b_int_en |= (1U << 0U);
            MAILBOX_A2B -> a2b_cmd0 = cmd;
            MAILBOX_A2B -> a2b_dat0 = data;
            break;
        case 1:
            MAILBOX_A2B -> a2b_int_en |= (1U << 1U);
            MAILBOX_A2B -> a2b_cmd1 = cmd; 
            MAILBOX_A2B -> a2b_dat1 = data; 
            break;
        case 2:
            MAILBOX_A2B -> a2b_int_en |= (1U << 2U);
            MAILBOX_A2B -> a2b_cmd2 = cmd;
            MAILBOX_A2B -> a2b_dat2 = data;
            break;
        case 3:
            MAILBOX_A2B -> a2b_int_en |= (1U << 3U);
            MAILBOX_A2B -> a2b_cmd3 = cmd;
            MAILBOX_A2B -> a2b_dat3 = data;
            break;
        default:
            break;
    }
}

uint32_t a_get_cmd(int ch)
{
    uint32_t cmd;

    switch (ch)
    {
        case 0:
            cmd = MAILBOX_B2A -> b2a_cmd0;
            break;
        case 1:
            cmd = MAILBOX_B2A -> b2a_cmd1;
            break;
        case 2:
            cmd = MAILBOX_B2A -> b2a_cmd2;
            break;
        case 3:
            cmd = MAILBOX_B2A -> b2a_cmd3;
            break;
        default:
            cmd = 0;
            break;
    }

    return cmd;
}

uint32_t a_get_data(int ch)
{
    uint32_t data;

    switch (ch)
    {
        case 0:
            data = MAILBOX_B2A -> b2a_dat0;
            break;
        case 1:
            data = MAILBOX_B2A -> b2a_dat1;
            break;
        case 2:
            data = MAILBOX_B2A -> b2a_dat2;
            break;
        case 3:
            data = MAILBOX_B2A -> b2a_dat3;
            break;
        default:
            data = 0;
            break;
    }

    return data;
}

void b2a_send(int ch,uint32_t cmd,uint32_t data)
{
    switch (ch)
    {
        case 0:
            MAILBOX_B2A -> b2a_int_en |= (1U << 0U);
            MAILBOX_B2A -> b2a_cmd0 = cmd;
            MAILBOX_B2A -> b2a_dat0 = data;
            break;
        case 1:
            MAILBOX_B2A -> b2a_int_en |= (1U << 1U);
            MAILBOX_B2A -> b2a_cmd1 = cmd;
            MAILBOX_B2A -> b2a_dat1 = data;
            break;
        case 2:
            MAILBOX_B2A -> b2a_int_en |= (1U << 2U);
            MAILBOX_B2A -> b2a_cmd2 = cmd;
            MAILBOX_B2A -> b2a_dat2 = data;
            break;
        case 3:
            MAILBOX_B2A -> b2a_int_en |= (1U << 3U);
            MAILBOX_B2A -> b2a_cmd3 = cmd;
            MAILBOX_B2A -> b2a_dat3 = data;
            break;
        default:
            break;
    }
}

uint32_t b_get_cmd(int ch)
{
    uint32_t cmd;

    switch (ch)
    {
        case 0:
            cmd = MAILBOX_A2B -> a2b_cmd0;
            break;
        case 1:
            cmd = MAILBOX_A2B -> a2b_cmd1;
            break;
        case 2:
            cmd = MAILBOX_A2B -> a2b_cmd2;
            break;
        case 3:
            cmd = MAILBOX_A2B -> a2b_cmd3;
            break;
        default:
            cmd = 0;
            break;
    }
    return cmd;
}

uint32_t b_get_data(int ch)
{
    uint32_t data;

    switch (ch)
    {
        case 0:
            data = MAILBOX_A2B -> a2b_dat0;
            break;
        case 1:
            data = MAILBOX_A2B -> a2b_dat1;
            break;
        case 2:
            data = MAILBOX_A2B -> a2b_dat2;
            break;
        case 3:
            data = MAILBOX_A2B -> a2b_dat3;
            break;
        default:
            data = 0;
            break;
    }

    return data;
}
