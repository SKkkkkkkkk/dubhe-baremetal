// #line 70000

#include "mailbox.h"
// #include "sh100_print.h"
#include <stdio.h>

void a_send_message(int id,int cmd,int dat)
{
    switch (id)
    {
        case 0:
            MAILBOX_A2B -> a2b_int_en = 0x1;
            MAILBOX_A2B -> a2b_cmd0 =cmd;
            MAILBOX_A2B -> a2b_dat0 =dat;
            break;
        case 1:
            MAILBOX_A2B -> a2b_int_en = 0x2;
            MAILBOX_A2B -> a2b_cmd1 = cmd; 
            MAILBOX_A2B -> a2b_dat1 = dat; 
            break;
        case 2:
            MAILBOX_A2B -> a2b_int_en = 0x4;
            MAILBOX_A2B -> a2b_cmd2 = cmd;
            MAILBOX_A2B -> a2b_dat2 = dat;
            break;
        case 3:
            MAILBOX_A2B -> a2b_int_en = 0x3;
            MAILBOX_A2B -> a2b_cmd3 = cmd;
            MAILBOX_A2B -> a2b_dat3 = dat;
            break;
        default:
            break;
    }

    printf("%s line %d cmd%d=0x%x data%d=0x%x\n\r", __func__, __LINE__, id, cmd, id, dat);
 
    //while(MAILBOX_A2B -> a2b_status == 0x1 );

    // return 0x0;

}

void a_recv_cmd(int id)
{
    int cmd;

    switch (id)
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
            break;
    }

    printf("%s line %d  cmd%d=0x%x\n\r", __func__, __LINE__, id, cmd);

    // return cmd;

}

void a_recv_dat(int id)
{
    int dat;

    switch (id)
    {
        case 0:
            dat = MAILBOX_B2A -> b2a_dat0;
            break;
        case 1:
            dat = MAILBOX_B2A -> b2a_dat1;
            break;
        case 2:
            dat = MAILBOX_B2A -> b2a_dat2;
            break;
        case 3:
            dat = MAILBOX_B2A -> b2a_dat3;
            break;
        default:
            break;
    }

    printf("%s line %d  dat%d=0x%x\n\r", __func__, __LINE__, id, dat);

    // return dat;

}

void b_send_message(int id,int cmd,int dat)
{

    MAILBOX_B2A -> b2a_int_en = 0x0f; 

    switch (id)
    {
        case 0:
            // MAILBOX_B2A -> b2a_int_en = 0x1;
            MAILBOX_B2A -> b2a_cmd0 = cmd;
            MAILBOX_B2A -> b2a_dat0 = dat;
            break;
        case 1:
            // MAILBOX_B2A -> b2a_int_en = 0x2;
            MAILBOX_B2A -> b2a_cmd1 = cmd;
            MAILBOX_B2A -> b2a_dat1 = dat;
            break;
        case 2:
            // MAILBOX_B2A -> b2a_int_en = 0x4;
            MAILBOX_B2A -> b2a_cmd2 = cmd;
            MAILBOX_B2A -> b2a_dat2 = dat;
            break;
        case 3:
            // MAILBOX_B2A -> b2a_int_en = 0x8;
            MAILBOX_B2A -> b2a_cmd3 = cmd;
            MAILBOX_B2A -> b2a_dat3 = dat;
            break;
        default:
            break;
    }

    printf("%s line %d cmd%d=0x%x data%d=0x%x\n\r", __func__, __LINE__, id, cmd, id, dat);

    //while(MAILBOX_B2A -> b2a_status == 0x1 );

    // return 0x0;

}

void b_recv_cmd(int id)
{
    int cmd;

    switch (id)
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
            break;
    }

    printf("%s line %d  cmd%d=0x%x\n\r", __func__, __LINE__, id, cmd);

    // return cmd;
}

void b_recv_dat(int id)
{
    int dat;

    switch (id)
    {
        case 0:
            dat = MAILBOX_A2B -> a2b_dat0;
            break;
        case 1:
            dat = MAILBOX_A2B -> a2b_dat1;
            break;
        case 2:
            dat = MAILBOX_A2B -> a2b_dat2;
            break;
        case 3:
            dat = MAILBOX_A2B -> a2b_dat3;
            break;
        default:
            break;
    }

    printf("%s line %d  dat%d=0x%x\n\r", __func__, __LINE__, id, dat);

    // return dat;

}
