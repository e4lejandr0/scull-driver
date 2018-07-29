#ifndef SCULL_DRV
#define SCULL_DRV

#include <asm/page.h>

#include "cbuffer.h"

#define SCULL_MAJOR 0 
#define SCULL_MINOR 0
#define SCULL_SIZE (2 * PAGE_SIZE)
#define SCULL_NR_DEVS 1

/**
 * Main scull data structure
 */
struct sculldev_t {
	struct cdev cdev;
	struct cbuffer *data;
};

static int scull_open(struct inode *inode, struct file *filp);
static int scull_release(struct inode *inode, struct file *filp);

static ssize_t scull_read(struct file *filp, char __user *data, size_t count, loff_t *unknown);
static ssize_t scull_write(struct file *filp, const char __user *data, size_t count, loff_t *unknown);

static int scull_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long unknown);

static int setup_sculldev(struct sculldev_t *dev, int index);


#endif
