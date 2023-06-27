/*
 * Copyright (C) 2020 Xilinx, Inc.  All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * Resource table declarations. Resource table is placed in a separate
 * section of a bare-metal binary or firmware. It's used to describe
 * shared memory (virtIO devices) resources the remoteproc host should
 * initialize and / or use to communicate with the firmware.
 */
#ifndef RSC_TABLE_H_
#define RSC_TABLE_H_

#include <stddef.h>
#include <openamp/open_amp.h>

#if defined __cplusplus
extern "C" {
#endif

#define NO_RESOURCE_ENTRIES         8

/* Resource table for the given remote */
struct __packed remote_resource_table {
	unsigned int version;
	unsigned int num;
	unsigned int reserved[2];
	unsigned int offset[NO_RESOURCE_ENTRIES];
	/* rpmsg vdev entry */
	struct fw_rsc_vdev rpmsg_vdev;
	struct fw_rsc_vdev_vring rpmsg_vring0;
	struct fw_rsc_vdev_vring rpmsg_vring1;
};

void *get_resource_table(int rsc_id, int *len);

#define SHARED_MEM_BASE 0xF0000000UL
#define SHARED_MEM_SIZE 0x100000UL

#define NUM_OF_VRINGS               0x02
#define VRING_ALIGN                 0x1000
#define VRING0_ADDR                 SHARED_MEM_BASE
#define VRING1_ADDR                 (SHARED_MEM_BASE + 0x4000UL)
#define NUM_OF_VRING_ENTRIES        8

// #define RPMSG_VDEV_DFEATURES        0
#define RPMSG_VDEV_DFEATURES        (1 << VIRTIO_RPMSG_F_NS)

#define NUM_TABLE_ENTRIES           1

// #define HOST_EPT_ADDRESS                1024
// #define REMOTE_EPT_ADDRESS              1025

#if defined __cplusplus
}
#endif

#endif /* RSC_TABLE_H_ */
