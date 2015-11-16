#include "frw.h"
#include <stdlib.h>

size_t fwrite_byte(FILE *fp, const unsigned char byte)
{
    if (fwrite(&byte, 1, 1, fp) != 1) {
        fprintf(stderr, "Error: Could not write byte\n");
        exit(EXIT_FAILURE);
    }
    return 1;
}

size_t fwrite_buf(FILE *fp, const unsigned char *buf, const size_t n)
{
    if (fwrite(buf, 1, n, fp) != n) {
        fprintf(stderr, "Error: Could not write %zu byte(s)\n", n);
        exit(EXIT_FAILURE);
    }
    return n;
}

size_t fwrite_uint32(FILE *fp, const uint32_t dword)
{
    fwrite_byte(fp, (unsigned char)(dword >> 24) & 0xFF);
    fwrite_byte(fp, (unsigned char)(dword >> 16) & 0xFF);
    fwrite_byte(fp, (unsigned char)(dword >>  8) & 0xFF);
    fwrite_byte(fp, (unsigned char)(dword      ) & 0xFF);
    return sizeof(uint32_t);
}

size_t fwrite_uint64(FILE *fp, const uint64_t qword)
{
    fwrite_byte(fp, (unsigned char)(qword >> 56) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >> 48) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >> 40) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >> 32) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >> 24) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >> 16) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword >>  8) & 0xFF);
    fwrite_byte(fp, (unsigned char)(qword      ) & 0xFF);
    return sizeof(uint64_t);
}

size_t fread_byte(FILE *fp, unsigned char *byte)
{
    return fread(byte, 1, 1, fp);
}

size_t fread_buf(FILE *fp, unsigned char *buf, const size_t n)
{
    return fread(buf, 1, n, fp);
}

size_t fread_uint32(FILE *fp, uint32_t *dword)
{
    unsigned char *bytes = (unsigned char *)malloc(sizeof(uint32_t));
    if (!bytes) abort();
    size_t ret = fread(bytes, 1, sizeof(uint32_t), fp);

    if (ret != sizeof(uint32_t)) {
        free(bytes);
        return ret;
    }

    *dword = (uint32_t)bytes[0] << 24 |
             (uint32_t)bytes[1] << 16 |
             (uint32_t)bytes[2] <<  8 |
             (uint32_t)bytes[3];

    free(bytes);
    return ret;
}

size_t fread_uint64(FILE *fp, uint64_t *qword)
{
    unsigned char *bytes = (unsigned char *)malloc(sizeof(uint64_t));
    if (!bytes) abort();
    size_t ret = fread(bytes, 1, sizeof(uint64_t), fp);

    if (ret != sizeof(uint64_t)) {
        free(bytes);
        return ret;
    }

    *qword = (uint64_t)bytes[0] << 56 |
             (uint64_t)bytes[1] << 48 |
             (uint64_t)bytes[2] << 40 |
             (uint64_t)bytes[3] << 32 |
             (uint64_t)bytes[4] << 24 |
             (uint64_t)bytes[5] << 16 |
             (uint64_t)bytes[6] <<  8 |
             (uint64_t)bytes[7];

    free(bytes);
    return ret;
}
