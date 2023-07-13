/*
 * Copyright (c) 2020 Xilinx, Inc. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/*
 * This file populates resource table for BM remote
 * for use by the Linux host
 */

#include <openamp/open_amp.h>
#include "rsc_table.h"

/* Place resource table in special ELF section */
#define __section_t(S)          __attribute__((__section__(#S)))
#define __resource              __section_t(.resource_table)

#define RPMSG_VDEV_DFEATURES        (1 << VIRTIO_RPMSG_F_NS)

#define SHARED_MEM_BASE 0x79540000UL

#define NUM_OF_VRINGS               0x02
#define VRING_ALIGN                 0x1000
#define VRING0_ADDR                 SHARED_MEM_BASE
#define VRING1_ADDR                 (SHARED_MEM_BASE + 0x8000UL)
#define NUM_OF_VRING_ENTRIES        256

#define NUM_TABLE_ENTRIES           1

struct remote_resource_table __resource resources = {
	/* Version */
	1,

	/* NUmber of table entries */
	NUM_TABLE_ENTRIES,
	/* reserved fields */
	{0, 0,},

	/* Offsets of rsc entries */
	{
	 offsetof(struct remote_resource_table, rpmsg_vdev),
	 },

	/* Virtio device entry */
	{
	 RSC_VDEV, // type
	 VIRTIO_ID_RPMSG, // id
	 31, // notifyid
	 RPMSG_VDEV_DFEATURES, // dfeatures
	 RPMSG_VDEV_DFEATURES, // gfeatures
	 0, // config_len
	 0, // status
	 NUM_OF_VRINGS, // num_of_vrings
	 {0, 0}, // reserved
	 },

	/* Vring rsc entry - part of vdev rsc entry */
	{VRING0_ADDR, VRING_ALIGN, NUM_OF_VRING_ENTRIES, 1 /* notifyid */, 0},
	{VRING1_ADDR, VRING_ALIGN, NUM_OF_VRING_ENTRIES, 2 /* notifyid */, 0},
};

void *get_resource_table(int rsc_id, int *len)
{
	(void)rsc_id;
	*len = sizeof(resources);
	return &resources;
}

