/* ebd, encrypted block driver for assignment 3 CS444 group 12-08 Spring 2017 */

#include <linux/blkdev.h>
#include <linux/errno.h>
#include <linux/fs.h>
#include <linux/genhd.h>
#include <linux/hdreg.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/types.h>
#include <linux/vmalloc.h>


/* data structure to represent "disk" */
static struct ebd_device {
	unsigned long size;		/* size of disk in bytes */
	spinlock_t lock;  		/* locks or unlocks the "disk" */
	u8 *data;				/* data, obviously */
	struct gendisk *gd;		/* kernel view of disk */
} Device;

/* init the device */

stat int __init ebd_init(void)
{

}
