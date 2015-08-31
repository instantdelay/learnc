#ifndef BITS_H_
#define BITS_H_

typedef struct bits bits;
struct bits {
    char* data;
    char* pos;
    int shift;
};

#define MIN(x, y) ((x) > (y) ? (y) : (x));

bits* bits_new(size_t size);
void bits_free(bits* b);
void bits_write(bits* b, int val, int valSizeBits);
int bits_read(bits* b, int valSizeBits);
void bits_reset(bits* b);

void printBin(char* bytes, int num);

#endif