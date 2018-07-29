#include <linux/uaccess.h>
#include <linux/slab.h>
#include <linux/circ_buf.h>

#include "cbuffer.h"

struct cbuffer* create_buffer(unsigned long size)
{
    struct cbuffer *buffer = kmalloc(sizeof(struct cbuffer), GFP_KERNEL);

    if(buffer == NULL)
	return NULL;

    buffer->begin = kmalloc(size, GFP_KERNEL);
    if(buffer->begin == NULL) {
	kfree(buffer);
	return NULL;
    }
    buffer->head = 0;
    buffer->tail = 0;
    buffer->end = buffer->begin + size;
    buffer->size = size;

    return buffer;
}

int read_buffer(struct cbuffer *buffer, char __user *out, unsigned long size)
{

    unsigned long err_bytes;
    unsigned long bytes_to_read;
    unsigned long nr_elements;

    /* Only read max buffer->size bytes */
    size = min(size, buffer->size);
    nr_elements = CIRC_CNT_TO_END(buffer->head, buffer->tail, buffer->size);

    if(buffer->head == buffer->tail) {
	return 0;
    }
    bytes_to_read = min(size, nr_elements);
    err_bytes = copy_to_user(out, &buffer->begin[buffer->tail], bytes_to_read);
    if(err_bytes) {
	return -EFAULT;
    }

    buffer->tail += bytes_to_read;
    if(buffer->tail == buffer->size) {
	buffer->tail = 0;
    }

    return bytes_to_read;
}

int write_buffer(struct cbuffer *buffer, const char __user *in, unsigned long size)
{
    unsigned long err_bytes;
    unsigned long bytes_to_write;
    unsigned long nr_elements;

    nr_elements = CIRC_SPACE_TO_END(buffer->head, buffer->tail, buffer->size);
    bytes_to_write = min(size, nr_elements);

    err_bytes = copy_from_user(&buffer->begin[buffer->head], in, bytes_to_write);
    if(err_bytes) {
	return -EFAULT;
    }

    buffer->head += bytes_to_write;
    if(buffer->head == buffer->size) {
	buffer->head = 0;
    }

    return bytes_to_write;
}
void trim_buffer(struct cbuffer *buffer) {
    buffer->head = 0;
    buffer->tail = 0;
}
void destroy_buffer(struct cbuffer *buffer)
{
    if(buffer == NULL)
	return;

    kfree(buffer->begin);
    kfree(buffer);
}


