#ifndef SCULL_CIRCULAR_BUFFER
#define SCULL_CIRCULAR_BUFFER

struct cbuffer {
    char* begin;
    char* end;
    size_t head;
    size_t tail;
    size_t size;
};

struct cbuffer* create_buffer(unsigned long size);
int read_buffer(struct cbuffer *buffer, char __user *out, unsigned long size);
int write_buffer(struct cbuffer *buffer, const char __user *in, unsigned long size);
void trim_buffer(struct cbuffer *buffer);
void destroy_buffer(struct cbuffer *buffer);

#endif
