#ifndef BITS_H_
#define BITS_H_

typedef struct bits bits;
/**
 * A stream of bits that can be written to and read from with
 * non-standard sizes.
 */
struct bits {
    /** @brief Underlying storage */
    char* data;
    /** @brief Pointer to the current stream position in #data */
    char* pos;
    /** Bit offset within the current byte */
    int shift;
};

#define MIN(x, y) ((x) > (y) ? (y) : (x));

/**
 * Allocate a new bits struct with the specified amount of available space.
 */
bits* bits_new(size_t size);
/**
 * Free a bits pointer.
 */
void bits_free(bits* b);
/**
 * Write a value to b using valSizeBits of space.
 */
void bits_write(bits* b, int val, int valSizeBits);
/**
 * Read a value from b. Take valSizeBits from b and return them as an int.
 */
int bits_read(bits* b, int valSizeBits);
/**
 * Reset a bits stream to the beginning.
 */
void bits_reset(bits* b);

void printBin(char* bytes, int num);

#endif