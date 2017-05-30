/* ebd, encrypted block driver for assignment 3 CS444 group 12-08 Spring 2017 */
/* authors Isaac Stallcup Austin Nguyen Alex Garcia */

#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>

#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/vmalloc.h>
#include <linux/genhd.h>
#include <linux/blkdev.h>
#include <linux/hdreg.h>

#include <linux/crypto.h>

MODULE_LICENSE("Dual BSD/GPL");
/* ceaceless feeble dead beef fell off 0 0 */
static char *mykey = "ceace1e55feeb1edeadbeeffe110ff00";
module_param(mykey, charp, 0000); 
static int major_num = 0;	
module_param(major_num, int, 0);
static int hardsect_size = 512;
module_param(hardsect_size, int, 0);
static int nsectors = 1024; 
module_param(nsectors, int, 0);
static char *key = "ceace1e55feeb1edeadbeeffe110ff00";
module_param(key, charp, 0400);

#define KERNEL_SECTOR_SIZE 512
#define KEY_SIZE 32


static struct request_queue *Queue;

/* data structure to represent "disk" */
static struct ebd_device {
	unsigned long size;			/* size of disk in bytes */
	spinlock_t lock;  			/* locks or unlocks the "disk" */
	u8 *data;					/* data, obviously */
	struct gendisk *gd;			/* kernel view of disk */

	/* crypto stuff goes here */
	struct crypto_cipher *blk_cipher;
	u8 key[KEY_SIZE];
	struct crypto_cipher *custom_cipher;
	u8 mykey[KEY_SIZE];

} Device;

/* crypto goes here */
static void ebd_transfer(struct ebd_device *dev, sector_t sector, unsigned long nsect, char *buffer, int write)
{
    unsigned long offset = sector*hardsect_size;
    unsigned long nbytes = nsect*hardsect_size;
	unsigned long blocksize = crypto_cipher_blocksize(dev->blk_cipher);
	unsigned long i;
    
    if ((offset + nbytes) > dev->size) { 
		printk (KERN_NOTICE "ebd: Beyond-end write (%ld %ld)\n", offset, nbytes);
		return;
    }
    if (write) {
		char* destination = dev->data + offset;
 		for (i = 0; i < nbytes; i+= blocksize)
 			crypto_cipher_encrypt_one(dev->custom_cipher, &destination[i], &buffer[i]);
/*		memcpy(dev->data + offset, buffer, nbytes);*/
	}
    else {
		char* source = dev->data + offset;
 		for (i = 0; i < nbytes; i+= blocksize)
 			crypto_cipher_decrypt_one(dev->blk_cipher, &buffer[i], &source[i]);
/* 			crypto_cipher_decrypt_one(dev->blk_cipher, &buffer[i], &source[i]); */ /*verion without custom key*/
/*		memcpy(buffer, dev->data + offset, nbytes);*/
	}
}

static void ebd_request(struct request_queue *q)
{
	struct request *req; /* pointer to request */
	req = blk_fetch_request(q);
	while (req != NULL) { 
		if (req->cmd_type != REQ_TYPE_FS) {
			printk(KERN_NOTICE "skip non-command request\n");
			__blk_end_request_all(req, EIO);
			continue;
		}
		ebd_transfer(&Device, blk_rq_pos(req), blk_rq_cur_sectors(req),
			   req->buffer, rq_data_dir(req));	
		if (!__blk_end_request_cur(req, 0))
			req = blk_fetch_request(q);
	}
}

/*int ebd_ioctl(struct inode *inode, struct file *filp,
	   	unsigned int cmd, unsigned long arg)
{
	long size;
	struct hd_geometry geo;
	switch (cmd) {
		case HDIO_GETGEO:
			size = Device.size*(hardsect_size/KERNEL_SECTOR_SIZE);
			geo.cylinders = (size & ~0x3f) >> 6;
			geo.heads = 4;
			geo.sectors = 16;
			geo.start = 4;
			if (copy_to_user((void *) arg, &geo, sizeof(geo)))
				return -EFAULT;
			return 0;
	}
}*/

int edb_getgeo(struct block_device *blk_dev, struct hd_geometry *geo)
{
	long size;
	size = Device.size*(hardsect_size/KERNEL_SECTOR_SIZE);
	geo->cylinders = (size & ~0x3f) >> 6;
	geo->heads = 4;
	geo->sectors = 16;
	geo->start = 4;
	return 0;
}

/* data structure to hold device operations */
static struct block_device_operations ebd_ops = {
	.owner		= THIS_MODULE,
	.getgeo		= edb_getgeo,
};



/* init the device */
static int __init ebd_init(void)
{
	unsigned int crypt_key;
	unsigned int cust_crypt_key;
	/* device bookkeeping */
	Device.size = nsectors*hardsect_size;	/* size = # sectors in drive * size of sector */
	spin_lock_init(&Device.lock);			/* initialize the spinlock */
	Device.data = vmalloc(Device.size);		/* allocate the space for the data */
	if (Device.data == NULL)
		return -ENOMEM; 					/* error out if no memory detected */

	/* crypto shutff!!! */
	Device.blk_cipher = crypto_alloc_cipher("aes", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(Device.blk_cipher)) {
		printk("ebd: Failed to load crypto\n");
		goto out;
	}
	Device.custom_cipher = crypto_alloc_cipher("aes", 0, CRYPTO_ALG_ASYNC);
	if (IS_ERR(Device.custom_cipher)) {
		printk("ebd: Failed to load custom crypto\n");
		goto out;
	}
	crypt_key = crypto_cipher_setkey(Device.blk_cipher, key, 32);
	if (crypt_key != 0) {
		printk("Failed to set key\n");
		goto out;
	}
	cust_crypt_key = crypto_cipher_setkey(Device.custom_cipher, mykey, 32);
	if (cust_crypt_key != 0) {
		printk("Failed to set custom key\n");
		goto out;
	}
	else {
		printk("Custom key set, reference is %s!", mykey);
	}
	
	/* get request queue */
	Queue = blk_init_queue(ebd_request, &Device.lock);
	if (Queue == NULL)
		goto out;
	
	blk_queue_logical_block_size(Queue, hardsect_size);		

	/* register with kernel so it shows up in /block/devices/ */
	major_num = register_blkdev(major_num, "ebd");		/* get major number */
	if (major_num <= 0) {								/* error out */
		printk(KERN_WARNING "ebd: unable to get major number\n");
		goto out;
	}

	/* generic disk functionality */

	Device.gd = alloc_disk(16); /* device will support N-1 partitions where N is alloc_disk(N) */
	if (! Device.gd)
		goto out_unregister;

	Device.gd->major = major_num;	/*set gd major #*/
	Device.gd->first_minor = 0;		/*set gd minor #s to start at 0*/
	Device.gd->fops = &ebd_ops;		/*block device operations; see appropriate data structure */
	Device.gd->private_data = &Device;
	strncpy(Device.gd->disk_name, "ebd0", 4);
	set_capacity(Device.gd, nsectors*(hardsect_size/KERNEL_SECTOR_SIZE));
	Device.gd->queue = Queue; /* set up earlier, see section on "get request queue" */
	/* once we're done setting up, add disk */
	
	add_disk(Device.gd);

	return 0;

	/* emergency exits */
	out_unregister:
		unregister_blkdev(major_num, "ebd");
	out: 						/* go here if error in allocating memory */
		vfree(Device.data);
	return -ENOMEM;
} /* end init device */

static void __exit ebd_exit(void) {
	del_gendisk(Device.gd);
	put_disk(Device.gd);
	unregister_blkdev(major_num, "ebd");
	blk_cleanup_queue(Queue);
	vfree(Device.data);
}

module_init(ebd_init);
module_exit(ebd_exit);

MODULE_AUTHOR("Group 12-08 CS444 Spring 2017");
