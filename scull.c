#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex");
MODULE_DESCRIPTION("Scull Driver, in memory char device");

#include "scull.h"

const int scull_nr = 1;

/**
 * Main scull data structure
 */
struct sculldev_t {
	dev_t version;
	struct cdev cdev;
	char* data;
	size_t size;
};

/**
 * File operations for my scull device
 */
struct file_operations scull_fops = {
	.owner = THIS_MODULE,
	.read = scull_read,
	.write = scull_write,
	.open = scull_open,
	.release = scull_release,
};

struct sculldev_t *global_scull;

static ssize_t scull_read(struct file *filp, char __user *data, size_t count, loff_t *unknown)
{
	printk(KERN_ALERT "Reading from scull\n");
	return 0;
}

static ssize_t scull_write(struct file *filp, const char __user *data, size_t count, loff_t *unknown)
{
	printk(KERN_ALERT "Writing to scull\n");
	return 0;
}

static int scull_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long unknown)
{
	printk(KERN_ALERT "Scull ioctl invoked\n");
	return -ENOTTY;
}

static int scull_open(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "Opening scull");
	return 0;
}

static int scull_release(struct inode *inode, struct file *filp)
{
	printk(KERN_ALERT "Releasing scull\n");
	return 0;
}

static int scull_setup_cdev(struct sculldev_t *dev)
{
	int err;

	cdev_init(&dev->cdev, &scull_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &scull_fops;
	if(scull_major) {
	dev->version = MKDEV(scull_major, scull_minor);
	err = register_chrdev_region(dev->version, scull_nr, "scull");
	if(err < 0) goto unregister_chrdev;
	}
	else {
	err = alloc_chrdev_region(&dev->version, scull_minor, scull_nr, "scull");
	if(err < 0) goto unregister_chrdev;
	}
	printk(KERN_NOTICE "Allocated device: %d %d\n", MAJOR(dev->version), MINOR(dev->version));

	err = cdev_add(&dev->cdev, dev->version, scull_nr);

	if(err < 0) goto unregister_chrdev;

	return 0;

unregister_chrdev:
	printk(KERN_ALERT "Error allocation device\n");
	unregister_chrdev_region(dev->version, scull_nr);
	return -EBUSY;
}

static int __init scull_init(void)
{
	int err;

	printk(KERN_ALERT "Initializing scull");
	global_scull = kmalloc(sizeof(*global_scull), GFP_KERNEL);
	if(!global_scull) {
		printk(KERN_ALERT "Couldn't allocate scull\n");
		return -EBUSY;
	}

	err = scull_setup_cdev(global_scull);
	if(err) goto cleanup;

	return 0;

cleanup:
	kfree(global_scull);
	return -EBUSY;
}

static void scull_clean_cdev(struct sculldev_t *dev)
{
	cdev_del(&dev->cdev);
	unregister_chrdev_region(dev->version, scull_nr);
}

static void __exit scull_exit(void)
{
	printk(KERN_ALERT "Goodbye world!\n");
	if(global_scull) {
		scull_clean_cdev(global_scull);
		kfree(global_scull);
	}
}

module_init(scull_init);
module_exit(scull_exit);
