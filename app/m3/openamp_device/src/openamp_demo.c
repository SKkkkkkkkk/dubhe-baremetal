#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "openamp/remoteproc.h"
#include "metal/sys.h"
#include "rsc_table.h"


/* Shared memory */
#define SHARED_MEM_PA  		0x00100000UL
#define SHARED_MEM_SIZE 	0x8000UL
#define SHARED_BUF_OFFSET 	0UL

/* Cortex R5 memory attributes */
#define DEVICE_SHARED		0x00000001U /* device, shareable */
#define DEVICE_NONSHARED	0x00000010U /* device, non shareable */
#define NORM_NSHARED_NCACHE	0x00000008U /* Non cacheable  non shareable */
#define NORM_SHARED_NCACHE	0x0000000CU /* Non cacheable shareable */
#define	PRIV_RW_USER_RW		(0x00000003U<<8U) /* Full Access */


#define RPMSG_ADDR_ANY			0xFFFFFFFF



static struct remoteproc * rproc_init(struct remoteproc *rproc,
			const struct remoteproc_ops *ops, void *arg)
{
	if (!rproc || !ops)
		return NULL;
	rproc->priv = arg;
	rproc->ops = ops;
	return rproc;
}

static inline void rproc_remove(struct remoteproc *rproc)
{
	(void)rproc;
}

static void *
rproc_mmap(struct remoteproc *rproc, metal_phys_addr_t *pa,
			metal_phys_addr_t *da, size_t size,
			unsigned int attribute, struct metal_io_region **io)
{
	struct remoteproc_mem *mem;
	metal_phys_addr_t lpa, lda;
	struct metal_io_region *io_region;

	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;

	mem = metal_allocate_memory(sizeof(*mem));
	if (!mem)
		return NULL;
	io_region = metal_allocate_memory(sizeof(*io_region));
	if (!io_region) {
		metal_free_memory(mem);
		return NULL;
	}
	remoteproc_init_mem(mem, NULL, lpa, lda, size, io_region);
	/* va is the same as pa in this platform */
	metal_io_init(io_region, (void *)lpa, &mem->pa, size,
		      sizeof(metal_phys_addr_t) << 3, attribute, NULL);
	remoteproc_add_mem(rproc, mem);
	*pa = lpa;
	*da = lda;
	if (io) {
		*io = io_region;
	} else {
		metal_free_memory(io_region);
		metal_free_memory(mem);
		return NULL;
	}

	return metal_io_phys_to_virt(io_region, mem->pa);
}

static int rproc_start(struct remoteproc *rproc)
{
	return 0;
}


const struct remoteproc_ops rpro_ops = {
	.init = rproc_init,
	.remove = rproc_remove,
	.mmap = rproc_mmap,
	.notify = NULL,
	.start = rproc_start,
	.stop = NULL,
	.shutdown = NULL,
};


static void system_metal_logger(enum metal_log_level level,
			   const char *format, ...)
{
	(void)level;
	(void)format;
}

int init_system(void)
{
	int ret;

	struct metal_init_params metal_param = {
		.log_handler = system_metal_logger,
		.log_level = METAL_LOG_INFO,
	};

	/* Low level abstraction layer for openamp initialization */
	ret = metal_init(&metal_param);
	return ret;
}

void cleanup_system(void)
{
	metal_finish();
}



/*-----------------------------------------------------------------------------*
 *  RPMSG endpoint
 *-----------------------------------------------------------------------------*/
static struct rpmsg_endpoint lept;
static int shutdown_req = 0;
#define SHUTDOWN_MSG	0xEF56A55A

static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len,
			     uint32_t src, void *priv)
{
	(void)priv;
	(void)src;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		printf("shutdown message is received.\r\n");
		shutdown_req = 1;
		return RPMSG_SUCCESS;
	}

	/* Send data back to host */
	if (rpmsg_send(ept, data, len) < 0) {
		printf("rpmsg_send failed\r\n");
	}
	return RPMSG_SUCCESS;
}

static void rpmsg_service_unbind(struct rpmsg_endpoint *ept)
{
	(void)ept;
	printf("unexpected Remote endpoint destroy\r\n");
	shutdown_req = 1;
}


static struct remoteproc rproc_inst;

int openamp_demo(void)
{
	int ret = 0;
	int rsc_size;
	void* rsc_table;
	metal_phys_addr_t pa;
	struct virtio_device* vdev;
	struct rpmsg_virtio_device *rpmsg_vdev;
	struct metal_io_region *shbuf_io;
	struct rpmsg_device * rpmsg_dev;

	init_system();


	rsc_table = get_resource_table(0, &rsc_size);

	/* Initialize remoteproc instance */
	if (!remoteproc_init(&rproc_inst, &rpro_ops, NULL))
		return 1;
	else
		printf("remoteproc_init ok.\n");

	/* mmap resource table */
	pa = (metal_phys_addr_t)rsc_table;
	(void *)remoteproc_mmap(&rproc_inst , &pa,
		NULL, rsc_size,
		NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
		&rproc_inst.rsc_io);

	/* mmap shared memory */
	pa = SHARED_MEM_PA;
	(void *)remoteproc_mmap(&rproc_inst , &pa,
		NULL, SHARED_MEM_SIZE,
		NORM_NSHARED_NCACHE|PRIV_RW_USER_RW,
		&shbuf_io);

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
	if (!rpmsg_vdev)
	{
		metal_free_memory(rpmsg_vdev);
		return 1;
	}

	/* Initialize the underlying virtio device */
	ret = rpmsg_init_vdev(rpmsg_vdev, vdev, NULL,
	shbuf_io, NULL);

	/* Get the rpmsg device */
	rpmsg_dev = rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);


		/* Initialize RPMSG framework */
	printf("Try to create rpmsg endpoint.\r\n");

	ret = rpmsg_create_ept(&lept, rpmsg_dev, "rpmsg-openamp-demo-channel",
			       RPMSG_ADDR_ANY, RPMSG_ADDR_ANY,
			       rpmsg_endpoint_cb,
			       rpmsg_service_unbind);
	if (ret) {
		printf("Failed to create endpoint.\r\n");
		return -1;
	}






	cleanup_system();
	printf("clean up system.\r\n");

	return ret;
}


