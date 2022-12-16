#include "jstdlib.h"

void memcpy(void *dest, const void *src, unsigned long bytes) {
    for (unsigned long i = 0; i < bytes; i++) {
        *(char*)(dest + i) = *(char*)(src + i);
    }
}
