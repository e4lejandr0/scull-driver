#ifndef SCULL_DRV
#define SCULL_DRV

#define scull_major 133
#define scull_minor 1

static ssize_t scull_read(struct file *filp, char __user *data, size_t count, loff_t *unknown);
static ssize_t scull_write(struct file *filp, const char __user *data, size_t count, loff_t *unknown);

static int scull_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long unknown);

static int scull_open(struct inode *inode, struct file *filp);
static int scull_release(struct inode *inode, struct file *filp);

#endif
