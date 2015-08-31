#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct bits bits;
struct bits {
    char* data;
    char* pos;
    int shift;
};

#define MIN(x, y) ((x) > (y) ? (y) : (x));

void printBin(char* bytes, int num) {
    for (int i = 0; i < num; i++) {
        unsigned char mask = 0x80;
        while (mask > 0) {
            printf("%d", (mask & bytes[i]) != 0 ? 1 : 0);
            mask >>= 1;
        }
    }
}

bits* bits_new(size_t size) {
    bits* b = malloc(sizeof(bits));
    b->data = calloc(size, size);
    b->pos = b->data;
    b->shift = 0;
    return b;
}

void bits_free(bits* b) {
    free(b->data);
    free(b);
}

void bits_write(bits* b, int val, int valSizeBits) {
    const int vs = sizeof(val) * 8;
    int space = (8 - b->shift);
    int valBit = 0;
    
    while (valBit < valSizeBits) {
        int toWrite = MIN(space, (valSizeBits - valBit));
        
        int toLsb = (valBit + (vs - valSizeBits) - (vs - 8));
        // printf("toLsb: %d, val: %d \n", toLsb, val);
        
        // unsigned char v = val << (valBit + (vs - valSizeBits));
        unsigned char v;
        if (toLsb > 0)
            v = (val << toLsb);
        else if (toLsb < 0)
            v = val >> (-toLsb);
        else
            v = val;
            
        // printf("v: %u \n", (char)val);
        
        v >>= b->shift;
        b->pos[0] |= v;
        
        b->pos += (b->shift + toWrite) / 8;
        b->shift = (b->shift + toWrite) % 8;
        valBit += toWrite;
    }
}

int bits_read(bits* b, int valSizeBits) {
    int out = 0;
    int space = (8 - b->shift);
    int valBit = 0;
    
    while (valBit < valSizeBits) {
        int toRead = MIN(space, (valSizeBits - valBit));
        
        unsigned char mask = (toRead == 8 ? -1 : ((1 << toRead) - 1));
        mask <<= 8 - toRead;
        mask >>= b->shift;
        // print((char*)&mask, 1);
        
        int addOut = (b->pos[0] & mask) << b->shift;
        int toPlace = (8 - valSizeBits + valBit);
        
        if (toPlace > 0)
            out |= (addOut >> toPlace);
        else if (toPlace < 0)
            out |= (addOut << (-toPlace));
        else
            out |= addOut;
        
        b->pos += (b->shift + toRead) / 8;
        b->shift = (b->shift + toRead) % 8;
        valBit += toRead;
    }
    
    return out;
}

void bits_reset(bits* b) {
    b->pos = b->data;
    b->shift = 0;
}

int main() {
    char* txt = "Hello, world!";
    int len = strlen(txt);
    
    bits* b = bits_new(100);
    for (char* c = txt; *c != '\0'; c++) {
        bits_write(b, *c, 7);
    }
    
    bits_reset(b);
    char res[50] = {0};
    for (int i = 0; i < len; i++) {
        res[i] = bits_read(b, 7);
    }
    
    bits_free(b);
    
    printf("%s", res);
    return 0;
}

int main2() {
    const int width = 7;
    
    bits* b = bits_new(200);
    
    bits_write(b, 63, width);
    bits_write(b, 63, width);
    
    printBin(b->data, 3);
    bits_reset(b);
    
    printf("\nreading\n");
    
    printf("%d\n", bits_read(b, width));
    printf("%d\n", bits_read(b, width));
    
    bits_free(b);
    return 0;
}
