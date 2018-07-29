#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/kdev_t.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Alex");
MODULE_DESCRIPTION("Scull Driver, in memory char device");

#include "cbuffer.h"
#include "scull.h"

int scull_major = SCULL_MAJOR;
int scull_minor = 0;
int scull_nr_devs = SCULL_NR_DEVS;
int scull_size = SCULL_SIZE;

module_param(scull_major, int, S_IRUGO);
module_param(scull_minor, int, S_IRUGO);
module_param(scull_nr_devs, int, S_IRUGO);
module_param(scull_size, int, S_IRUGO);

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

struct sculldev_t *scull_devices = NULL;

static ssize_t scull_read(struct file *filp, char __user *data, size_t count, loff_t *unknown)
{
    struct sculldev_t *dev = filp->private_data;
    printk(KERN_ALERT "Reading from scull\n");
    return read_buffer(dev->data, data, count);
}

static ssize_t scull_write(struct file *filp, const char __user *data, size_t count, loff_t *unknown)
{
    struct sculldev_t *dev = filp->private_data;
    printk(KERN_ALERT "Writing to scull\n");

    return write_buffer(dev->data, data, count);
}

static int scull_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long unknown)
{
    printk(KERN_ALERT "Scull ioctl invoked\n");
    return -ENOTTY;
}

static int scull_open(struct inode *inode, struct file *filp)
{
    struct sculldev_t *dev;
    printk(KERN_ALERT "Opening scull");
    dev = container_of(inode->i_cdev, struct sculldev_t, cdev);
    filp->private_data = dev;

    if((filp->f_flags & O_ACCMODE) == O_WRONLY) {
	printk(KERN_NOTICE "scull: trimming\n");
	trim_buffer(dev->data);
    }

    return 0;
}

static int scull_release(struct inode *inode, struct file *filp)
{
    printk(KERN_ALERT "Releasing scull\n");
    return 0;
}

static int __init scull_init(void)
{
    int i, result;
    dev_t dev = 0;
    if(scull_major) {
	dev = MKDEV(scull_major, scull_minor);
	result = register_chrdev_region(dev, scull_nr_devs, "scull");
    }
    else {
	result = alloc_chrdev_region(&dev, scull_minor, scull_nr_devs, "scull");
	scull_major = MAJOR(dev);
    }

    if(result < 0) {
	printk(KERN_WARNING "scull: can't allocate major %d\n", scull_major);
	return result;
    }

    scull_devices = kmalloc(scull_nr_devs * sizeof(struct sculldev_t), GFP_KERNEL);
    if(scull_devices == NULL) {
	goto cleanup;
    }
    memset(scull_devices, 0, scull_nr_devs * sizeof(struct sculldev_t));

    for(i = 0; i < scull_nr_devs; ++i) {
	if(setup_sculldev(&scull_devices[i], i)) {
	    printk(KERN_WARNING "Error setting up device scull%d\n", i);
	}
    }

    printk(KERN_NOTICE "scull: loaded %d devices", scull_nr_devs);
    return 0;

cleanup:
    unregister_chrdev_region(dev, scull_nr_devs);
    return -EBUSY;
}

static int setup_sculldev(struct sculldev_t *dev, int index)
{
    int err, devno = MKDEV(scull_major, scull_minor + index);

    cdev_init(&dev->cdev, &scull_fops);
    dev->cdev.owner = THIS_MODULE;
    dev->cdev.ops = &scull_fops;
    dev->data = create_buffer(scull_size);
    if(dev->data == NULL) {
	printk(KERN_ERR "scull: error allocating device: %d %d\n", scull_major, scull_minor + index);
	return -ENOMEM;
    }

    err = cdev_add(&dev->cdev, devno, 1);
    if(err) {
	printk(KERN_NOTICE "Error adding scull%d", index);
	return err;
    }

    return 0;
}


static void scull_cleanup(struct sculldev_t *dev)
{
    cdev_del(&dev->cdev);
    destroy_buffer(dev->data);
}

static void __exit scull_exit(void)
{
    int i;
    dev_t devno = MKDEV(scull_major, scull_minor);

    if(scull_devices == NULL) {
	return;
    }

    for(i = 0; i < scull_nr_devs; ++i) {
	scull_cleanup(&scull_devices[i]);
    }
    kfree(scull_devices);

    unregister_chrdev_region(devno, scull_nr_devs);
    printk(KERN_NOTICE "scull: unloaded");
}

module_init(scull_init);
module_exit(scull_exit);
