#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "openamp/remoteproc.h"
#include "metal/sys.h"
#include "rsc_table.h"
#include <assert.h>

#ifndef QEMU
#include "chip_mem_layout.h"
#include "mailbox.h"
#include "m3.h"
#endif

/* Shared memory */
#define SHARED_MEM_PA                  0x79500000UL
#define SHARED_MEM_SIZE                0x40000UL
#define SHARED_BUF_OFFSET              0UL

/* Cortex R5 memory attributes */
#define DEVICE_SHARED                  0x00000001U /* device, shareable */
#define DEVICE_NONSHARED               0x00000010U /* device, non shareable */
#define NORM_NSHARED_NCACHE            0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE             0x0000000CU /* Non cacheable shareable */
#define PRIV_RW_USER_RW                (0x00000003U << 8U) /* Full Access */

#define RPMSG_ADDR_ANY                 0xFFFFFFFF
#define LOCAL_EPT_ADDR                 (0x1E)

#define FREERTOS_MSG_CMD               0x4C5A4803
#define MSG_DATA_LINUX_CREATE_TTY_EPT  0x5501
#define MSG_DATA_LINUX_DESTROY_TTY_EPT 0x5502
#define MSG_DATA_M3_ACK                0x0300
#define MAILBOX_B2A_CHN                0
#define MAILBOX_B2A_TX                 0
#define MAILBOX_B2A_RX                 1
#define MAILBOX_B2A_DATA               (MSG_DATA_M3_ACK | (MAILBOX_B2A_TX << 16))

void *rsc_table;

#ifndef QEMU
void Interrupt9_Handler(void)
{
    uint32_t                      sta;
    struct remote_resource_table *rsc =
        (struct remote_resource_table *) rsc_table;
    struct fw_rsc_vdev *vdev_rsc = &rsc->rpmsg_vdev;
    sta                          = MAILBOX_A2B->a2b_status;
    uint32_t data;
    for (int i = 0; i < 4; i++) {
        if (((sta >> i) & 1) == 1) {
            // printf("CM3 receive: cmd: 0x%lx  data: 0x%lx by ch=0x%x\n\r",
            // b_get_cmd(i), b_get_data(i), i);
            switch (i) {
            case 0:
                MAILBOX_A2B->a2b_status = 0x1;
                data                    = b_get_data(0);
                data &= 0xffff;
                if (data == MSG_DATA_LINUX_CREATE_TTY_EPT) {
                    vdev_rsc->status = 0x4;
                } else if (data == MSG_DATA_LINUX_DESTROY_TTY_EPT) {
                    vdev_rsc->status = 0x0;
                }
                // printf("vdev_rsc->status = 0x%x data 0x%lx\n\r",
                // vdev_rsc->status, data);
                break;
            case 1:
                MAILBOX_A2B->a2b_status = 0x2;
                break;
            case 2:
                MAILBOX_A2B->a2b_status = 0x4;
                break;
            case 3:
                MAILBOX_A2B->a2b_status = 0x8;
                break;
            default:
                break;
            }
        }
    }
}

void b2a_init()
{
    MAILBOX_B2A->b2a_status =
        0x0f; // Clear the interrupt by writing 1 to this bit.
    NVIC_SetPriority(Mailbox_IRQn, 0);
    NVIC_SetVector(Mailbox_IRQn, (uint32_t) (uintptr_t) Interrupt9_Handler);
    NVIC_EnableIRQ(Mailbox_IRQn);
}
#endif

static struct remoteproc *rproc_init(struct remoteproc           *rproc,
                                     const struct remoteproc_ops *ops,
                                     void                        *arg)
{
    if (!rproc || !ops) return NULL;
    rproc->priv = arg;
    rproc->ops  = ops;
    return rproc;
}

static inline void rproc_remove(struct remoteproc *rproc) { (void) rproc; }

static void *rproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
                        metal_phys_addr_t *da, size_t size,
                        unsigned int attribute, struct metal_io_region **io)
{
    struct remoteproc_mem  *mem;
    metal_phys_addr_t       lpa, lda;
    struct metal_io_region *tmpio;

    lpa = *pa;
    lda = *da;

    if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS) return NULL;
    if (lpa == METAL_BAD_PHYS) lpa = lda;
    if (lda == METAL_BAD_PHYS) lda = lpa;

    mem = metal_allocate_memory(sizeof(*mem));
    if (!mem) return NULL;
    tmpio = metal_allocate_memory(sizeof(*tmpio));
    if (!tmpio) {
        metal_free_memory(mem);
        return NULL;
    }
    remoteproc_init_mem(mem, NULL, lpa, lda, size, tmpio);
    /* va is the same as pa in this platform */
    metal_io_init(tmpio, (void *) lpa, &mem->pa, size,
                  sizeof(metal_phys_addr_t) << 3, attribute, NULL);
    remoteproc_add_mem(rproc, mem);
    *pa = lpa;
    *da = lda;
    if (io) {
        *io = tmpio;
    } else {
        metal_free_memory(tmpio);
        metal_free_memory(mem);
        return NULL;
    }

    return metal_io_phys_to_virt(tmpio, mem->pa);
}

static int rproc_start(struct remoteproc *rproc) { return 0; }

static int rproc_notify(struct remoteproc *rproc, uint32_t id)
{
#ifndef QEMU
    b2a_send(MAILBOX_B2A_CHN, FREERTOS_MSG_CMD, MAILBOX_B2A_DATA);
    // printf("rproc_notify send mailbox msg!!!\r\n");
#endif
    return 0;
}

const struct remoteproc_ops rpro_ops = {
    .init     = rproc_init,
    .remove   = rproc_remove,
    .mmap     = rproc_mmap,
    .notify   = rproc_notify,
    .start    = rproc_start,
    .stop     = NULL,
    .shutdown = NULL,
};

static void system_metal_logger(enum metal_log_level level, const char *format,
                                ...)
{
    (void) level;
    (void) format;
}

int init_system(void)
{
    int ret;

    struct metal_init_params metal_param = {
        .log_handler = system_metal_logger,
        .log_level   = METAL_LOG_INFO,
    };

    /* Low level abstraction layer for openamp initialization */
    ret = metal_init(&metal_param);
    return ret;
}

void cleanup_system(void) { metal_finish(); }

/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint
 *-----------------------------------------------------------------------------*/
static struct rpmsg_endpoint lept;
static int                   shutdown_req = 0;
static int                   cmd_case     = 0;
static char                  t_data[ 0x10000 ];
#define SHUTDOWN_MSG   0xEF56A55A
#define LINUX_SEND_MSG 0x55030000
#define LINUX_SEND_FIX 0x55030001
#define LINUX_SEND_ACC 0x55030002
#define LINUX_READ_MSG 0x55030003

static void m3_send_data(char c, int size)
{
    assert(size < 0x10000);
    assert(c >= 0x30 && c <= 0x39);

    for (int i = 0; i < size; i++) {
        t_data[ i ] = c;
    }
}

struct rpmsg_cmd {
    int cmd;
    int data;
};

static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
                             uint32_t src, void *priv)
{
    (void) priv;
    (void) src;
    struct rpmsg_cmd *rpmsg_cmd = (struct rpmsg_cmd *) data;
    char              buf[ 32 ];
    static int        cnt   = 1;
    static int        total = 0;
    int               size, msg;

    /* On reception of a shutdown we signal the application to terminate */
    if ((*(unsigned int *) data) == SHUTDOWN_MSG) {
        printf("shutdown message is received.\r\n");
        shutdown_req = 1;
        return RPMSG_SUCCESS;
    }

    switch (rpmsg_cmd->cmd) {
    case LINUX_SEND_MSG:
        cmd_case = 1;
        cnt      = 0;
        break;
    case LINUX_SEND_FIX:
        cmd_case = 2;
        cnt      = 0;
        break;
    case LINUX_SEND_ACC:
        cmd_case = 3;
        cnt      = 0;
        break;
    case LINUX_READ_MSG:
        cmd_case = 4;
        cnt      = 0;
        size     = rpmsg_cmd->data & 0xffffff;
        msg      = rpmsg_cmd->data >> 24;
        // printf("msg 0x%x size 0x%x\n", msg, size);
        m3_send_data((char) msg, size);
        break;
    default:
        // printf("cmd = 0x%x\n\r", cmd);
    }

    if (cmd_case == 1 || cmd_case == 2 || cmd_case == 3) {
        if ((cnt % 10) == 0) {
            printf(
                "Received a55:  len 0x%x total 0x%x cmd_case %d cnt 0x%x\n\r",
                len, total, cmd_case, cnt);
        }

        if (cnt != 0) {
            size = (len > 10) ? 10 : len;
            for (size_t i = 0; i < size; i++) {
                putchar(((char *) data)[ i ]);
            }
            putchar('\r');
            putchar('\n');
        }

        if (cmd_case == 1 && cnt != 0) {
            sprintf(buf, "m3:cnt%d", total);
            /* Send data back to host */
            if (rpmsg_send(ept, buf, strlen(buf) + 1) < 0) {
                printf("rpmsg_send failed\r\n");
            }
        }
    } else if (cmd_case == 4) {
        printf("Received a55:  len 0x%x total 0x%x cmd_case %d size 0x%x\n\r",
               len, total, cmd_case, size);
        while (size) {
            if (size > 0x100) {
                if (rpmsg_send(ept, t_data, 0x100) < 0) {
                    printf("rpmsg_send failed\r\n");
                }
                size -= 0x100;
            } else {
                if (rpmsg_send(ept, t_data, size) < 0) {
                    printf("rpmsg_send failed\r\n");
                }
                size = 0;
            }
        }
    }

    total++;
    cnt++;

    return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
    (void) ept;
    rpmsg_destroy_ept(&lept);
    printf("unexpected Remote endpoint destroy\r\n");
    shutdown_req = 1;
}

static struct remoteproc rproc_inst;

int openamp_demo(void)
{
    int                         ret = 0;
    int                         rsc_size;
    metal_phys_addr_t           pa;
    struct virtio_device       *vdev;
    struct rpmsg_virtio_device *rpmsg_vdev;
    struct metal_io_region     *shbuf_io;
    struct rpmsg_device        *rpmsg_dev;

#ifndef QEMU
    b2a_init();
#endif
    init_system();

    rsc_table = get_resource_table(0, &rsc_size);

    /* Initialize remoteproc instance */
    if (!remoteproc_init(&rproc_inst, &rpro_ops, NULL))
        return 1;
    else
        printf("remoteproc_init ok.\n");

    /* mmap resource table */
    pa = (metal_phys_addr_t) rsc_table;
    (void *) remoteproc_mmap(&rproc_inst, &pa, NULL, rsc_size,
                             NORM_NSHARED_NCACHE | PRIV_RW_USER_RW,
                             &rproc_inst.rsc_io);

    /* mmap shared memory */
    pa = SHARED_MEM_PA;
    (void *) remoteproc_mmap(&rproc_inst, &pa, NULL, SHARED_MEM_SIZE,
                             NORM_NSHARED_NCACHE | PRIV_RW_USER_RW, &shbuf_io);

    /* parse resource table to remoteproc */
    ret = remoteproc_set_rsc_table(&rproc_inst, rsc_table, rsc_size);
    if (ret) {
        printf("Failed to initialize remoteproc, %d\r\n", ret);
        remoteproc_remove(&rproc_inst);
        return 1;
    }

    printf("creating remoteproc virtio\r\n");
    vdev = remoteproc_create_virtio(&rproc_inst, 0, VIRTIO_DEV_DEVICE, NULL);
    if (!vdev) {
        printf("failed remoteproc_create_virtio\r\n");
        return 1;
    }

    rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
    if (!rpmsg_vdev) {
        metal_free_memory(rpmsg_vdev);
        return 1;
    }

    /* Initialize the underlying virtio device */
    ret = rpmsg_init_vdev(rpmsg_vdev, vdev, NULL, shbuf_io, NULL);

    /* Get the rpmsg device */
    rpmsg_dev = rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);

    /* Initialize RPMSG framework */
    printf("Try to create rpmsg endpoint.\r\n");

    ret = rpmsg_create_ept(&lept, rpmsg_dev, "rpmsg-virtual-tty-channel",
                           LOCAL_EPT_ADDR, RPMSG_ADDR_ANY, rpmsg_endpoint_cb,
                           rpmsg_service_unbind);
    if (ret) {
        printf("Failed to create endpoint.\r\n");
        return -1;
    }

    // #define HELLO_MSG "hello cnt "
    // char buf[32];
    // sprintf(buf, "hello cnt %d", 1);
    // rpmsg_send(&lept, buf, strlen(buf));

    while (!shutdown_req) {
        ret = remoteproc_get_notification(&rproc_inst, RSC_NOTIFY_ID_ANY);
        if (ret != 0)
            printf("remoteproc_get_notification failed, ret=%d.\r\n", ret);
    }

    /* Destroy the RPMsg endpoint */
    rpmsg_destroy_ept(&lept);
    cleanup_system();

    printf("clean up system.\r\n");

    return ret;
}
