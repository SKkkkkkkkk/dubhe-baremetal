#include <stdio.h>
#include <stdint.h>
#include "metal/sys.h"
#include "openamp/open_amp.h"
#include "openamp/remoteproc.h"
#include "openamp/rpmsg.h"
#include "openamp/remoteproc_loader.h"
#include "rsc_table.h"

/* Memory attributes */
#define NORM_NONCACHE 0x11DE2	/* Normal Non-cacheable */
#define STRONG_ORDERED 0xC02	/* Strongly ordered */
#define DEVICE_MEMORY 0xC06	/* Device memory */
#define RESERVED 0x0		/* reserved memory */

/* Shared Pool */
#define SHPOOL_BASE		(VRING1_ADDR + 0x4000UL)
#define SHPOOL_SIZE		0x80000UL
#define SHARED_BUF_OFFSET 0UL


static struct remoteproc rproc_inst;

/* RPMsg virtio shared buffer pool */
static struct rpmsg_virtio_shm_pool shpool;

/* Endpoint */
static struct rpmsg_endpoint ept;
#define RPMSG_SERVICE_NAME         "rpmsg-openamp-demo-channel"

#define SHUTDOWN_MSG	0xEF56A55A


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
	metal_phys_addr_t lpa, lda;
	struct remoteproc_mem *mem;
	struct metal_io_region *io_region;

	lpa = *pa;
	lda = *da;

	if (lpa == METAL_BAD_PHYS && lda == METAL_BAD_PHYS)
		return NULL;
	if (lpa == METAL_BAD_PHYS)
		lpa = lda;
	if (lda == METAL_BAD_PHYS)
		lda = lpa;

	// if (attribute == 0)
	// 	attribute = NORM_SHARED_NCACHE | PRIV_RW_USER_RW;

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
	if (io)
		*io = io_region;
	return metal_io_phys_to_virt(io_region, mem->pa);
}

static int rproc_start(struct remoteproc *rproc)
{
	return 0;
}

static int rproc_notify(struct remoteproc *rproc, uint32_t id)
{
	return 0;
}

const struct remoteproc_ops rpro_ops = {
	.init = rproc_init,
	.remove = rproc_remove,
	.mmap = rproc_mmap,
	.notify = rproc_notify,
	.start = rproc_start,
	.stop = NULL,
	.shutdown = NULL,
};

/* Example image store ops to fetch the remote image content from the memory 
and load
* them to destination memory - implementation is provided by the 
application.
*/
#define LPERROR(format, ...) printf(format, ##__VA_ARGS__)
#define LPRINTF(format, ...) printf(format, ##__VA_ARGS__)

int mem_image_open(void *store, const char *path, const void **image_data)
{
	(void)(path);
	if (image_data == NULL) {
		LPERROR("%s: input image_data is NULL\r\n", __func__);
		return -EINVAL;
	}
	*image_data = store;
	/* return an abitrary length, as the whole firmware is in memory */
	return 899744;
}

void mem_image_close(void *store)
{
	/* The image is in memory, does nothing */
	(void)store;
}

int mem_image_load(void *store, size_t offset, size_t size,
		   const void **data, metal_phys_addr_t pa,
		   struct metal_io_region *io,
		   char is_blocking)
{
	(void)is_blocking;

	LPRINTF("%s: offset=0x%lx, size=0x%lx\n\r",
		__func__, offset, size);
	if (pa == METAL_BAD_PHYS) {
		if (data == NULL) {
			LPERROR("%s: data is NULL while pa is ANY\r\n",
				__func__);
			return -EINVAL;
		}
		*data = (const void *)((const char *)store + offset);
	} else {
		void *va;

		if (io == NULL) {
			LPERROR("%s, io is NULL while pa is not ANY\r\n",
				__func__);
			return -EINVAL;
		}
		va = metal_io_phys_to_virt(io, pa);
		if (va == NULL) {
			LPERROR("%s: no va is found\r\n", __func__);
			return -EINVAL;
		}
		memcpy(va, (const void *)((const char *)store + offset), size);
	}

	return (int)size;
}

const struct image_store_ops mem_image_store_ops = {
	.open = mem_image_open,
	.close = mem_image_close,
	.load = mem_image_load,
	.features = SUPPORT_SEEK,
};

volatile static bool ns_service_ready = false;
void wait_ns_announcement()
{
	int ret;
	while(!ns_service_ready)
	{
		ret = remoteproc_get_notification(&rproc_inst, RSC_NOTIFY_ID_ANY);
		if( ret != 0) 
			printf("remoteproc_get_notification failed, ret=%d.\r\n", ret);
	}
	return;
}



int init_system(void);
void cleanup_system(void);

/* Endpoint data receive call back function */
static int rpmsg_endpoint_cb(struct rpmsg_endpoint *ept, void *data, size_t len, uint32_t src, void *priv) {
	(void)priv;
	(void)src;

	/* On reception of a shutdown we signal the application to terminate */
	if ((*(unsigned int *)data) == SHUTDOWN_MSG) {
		printf("shutdown message is received.\r\n");
		// shutdown_req = 1;
		return RPMSG_SUCCESS;
	}

	printf("Received message from master core: ");
	for(size_t i = 0; i < len; i++) {
		putchar(((char *)data)[i]);
	}
	putchar('\r');
	putchar('\n');

	/* Send data back to host */
	if (rpmsg_send(ept, data, len) < 0) {
		printf("rpmsg_send failed\r\n");
	}
	return RPMSG_SUCCESS;
}

/* Name service announcement call back function for channel deletion */
static void rpmsg_service_unbind(struct rpmsg_endpoint *ept) {
}

/* Name service announcement call back function for channel creation */
static void rpmsg_name_service_bind_cb(struct rpmsg_device *rdev,
const char *name, uint32_t dest) {
	int ret;
	LPRINTF("Master: new endpoint notification is received.\r\n");
	if (strcmp(name, RPMSG_SERVICE_NAME))
		LPERROR("Unexpected name service %s.\r\n", name);
	else
	{
		ret = rpmsg_create_ept(&ept, rdev, RPMSG_SERVICE_NAME,
				RPMSG_ADDR_ANY, dest,
				rpmsg_endpoint_cb,
				rpmsg_service_unbind);
		if(ret)
		{
			printf("rpmsg_create_ept failed, ret=%d.\r\n", ret);
			return;
		}
		else
		{
			ns_service_ready = true;
		}
	}
}

int openamp_demo(int role)
{
	int ret;
	metal_phys_addr_t pa;
	struct metal_io_region *shbuf_io;
	struct virtio_device* vdev;
	struct rpmsg_virtio_device *rpmsg_vdev;
	void *shbuf;
	struct rpmsg_device * rpmsg_dev;
	int rsc_size;
	void* rsc_table;
	(void)rsc_size;
	(void)rsc_table;
	(void)ret;
	(void)rpmsg_dev;

	printf("this is the host.\n");

	init_system();

	/* Initialize remoteproc instance */
	if (!remoteproc_init(&rproc_inst, &rpro_ops, NULL))
		return 1;
	else
		printf("remoteproc_init ok.\n");

	/* mmap shared memory */
	pa = SHPOOL_BASE;
	(void *)remoteproc_mmap(&rproc_inst , &pa,
				NULL, SHPOOL_SIZE,
				NORM_NONCACHE | STRONG_ORDERED,
				&shbuf_io);

	/* Configure remoteproc to get ready to load executable */
	remoteproc_config(&rproc_inst, NULL);

	/* Load the image */
	#define REMOTE_IMAGE_MEM_ADDR 0x40100000UL
	remoteproc_load(&rproc_inst, NULL, (void*)REMOTE_IMAGE_MEM_ADDR, &mem_image_store_ops, NULL);

	/* Start the remote processor */
	ret = remoteproc_start(&rproc_inst);
	if (ret) {
		printf("failed to start processor\r\n");
		return ret;
	}
	printf("successfully started the processor\r\n");

	printf("Creating virtio...\r\n");
	/* Setup the communication mechanism */
	vdev = remoteproc_create_virtio(&rproc_inst, 0, role, NULL);
	if (!vdev) {
		printf("failed remoteproc_create_virtio\r\n");
		return 1;
	}
	printf("Successfully created virtio device.\r\n");

	/* Only RPMsg virtio driver needs to initialize the
		 * shared buffers pool
		 */
	shbuf = metal_io_phys_to_virt(shbuf_io, SHPOOL_BASE);

	/* RPMsg virtio device can set shared buffers pool
		 * argument to NULL*/
	rpmsg_vdev = metal_allocate_memory(sizeof(*rpmsg_vdev));
	if (!rpmsg_vdev)
		return 1;
	
	/* Only RPMsg virtio driver needs to initialize the
		* shared buffers pool
		*/
	rpmsg_virtio_init_shm_pool(&shpool, shbuf, SHPOOL_SIZE);

	/* RPMsg virtio device can set shared buffers pool
		* argument to NULL
		*/
	ret =  rpmsg_init_vdev(rpmsg_vdev, vdev, rpmsg_name_service_bind_cb,
				shbuf_io, &shpool);

	if (ret) {
		printf("failed rpmsg_init_vdev, error_num: %d.\r\n", ret);
	}
	
	/* Get the rpmsg device */
	rpmsg_dev = rpmsg_virtio_get_rpmsg_device(rpmsg_vdev);

	/* Wait for the name service announcement */
	wait_ns_announcement();

	/* Endpoint is created - sending the first message */
	#define HELLO_MSG "hello"
	rpmsg_send(&ept, HELLO_MSG, strlen(HELLO_MSG));

	while(1)
	{
		ret = remoteproc_get_notification(&rproc_inst, RSC_NOTIFY_ID_ANY);
		if( ret != 0) 
			printf("remoteproc_get_notification failed, ret=%d.\r\n", ret);
	}


	rpmsg_destroy_ept(&ept);
	remoteproc_stop(&rproc_inst);
	remoteproc_shutdown(&rproc_inst);
	rpmsg_deinit_vdev(rpmsg_vdev);
	remoteproc_remove_virtio(&rproc_inst, rpmsg_vdev->vdev);
	remoteproc_remove(&rproc_inst);

	cleanup_system();
	return 0;
}

int init_system(void)
{
	int ret;

	struct metal_init_params metal_param = {
		.log_handler =  metal_default_log_handler,
		.log_level = METAL_LOG_DEBUG
	};

	/* Low level abstraction layer for openamp initialization */
	ret = metal_init(&metal_param);

	return ret;
}

void cleanup_system(void)
{
	metal_finish();
}
