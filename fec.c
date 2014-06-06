#include <stdio.h>

#include "HammingCodec.h"

#include "fec.h"

/*
 * This is the parity checking matrix we are using.
 * H =
 * 0 1 1 1 | 1 0 0 0*
 * 1 0 1 1 | 0 1 0 0
 * 1 1 0 1 | 0 0 1 0
 * 1 1 1 0 | 0 0 0 1
 *
 */

/* This is the generator matrix we are using
 * G =
 * 1 0 0 0
 * 0 1 0 0
 * 0 0 1 0
 * 0 0 0 1
 * -------
 * 0 1 1 1
 * 1 0 1 1
 * 1 1 0 1
 * 1 1 1 0
 */

/**
 * @brief generator matrix
 */
static uint8_t G[] =
{
    0x8,
    0x4,
    0x2,
    0x1,
    0x7,
    0xB,
    0xD,
    0xE
};

/**
 * @brief parity checking matrix
 */
static uint8_t H[] =
{
    0x78,
    0xB4,
    0xD2,
    0xE1
};

/**
 * @brief transposed parity matrix
 */
static uint8_t HT[] =
{
    0x7,
    0xB,
    0xD,
    0xE,
    0x8,
    0x4,
    0x2,
    0x1
};


#define H_ROW       4
#define HT_ROW      8
#define G_ROW       8

/**
 * @brief print a byte in binary format
 */
static void binary_print(uint8_t input)
{
    for (int i = 7; i >= 0; i--) {
        if (input & (1 << i)) {
            fputc('1', stderr);
        } else {
            fputc('0', stderr);
        }
    }
    fputc('\n', stderr);
}

/**
 * @brief XOR all the bits in a byte
 */
static uint8_t self_xor(uint8_t input)
{
    uint8_t result = 0;
    for (int i = 0; i < 8; i++) {
        result ^= (input >> i) & 1;
    }
    return result;
}

/**
 * @brief multiply binary matrices with a vector
 */
static uint8_t m_mul(uint8_t *matrix, uint8_t vector, int m_row)
{
    uint8_t temp = 0;
    uint8_t result = 0;
    for (int i = 0; i < m_row; i++) {
        temp = matrix[i] & vector;
        result |= (self_xor(temp) << (m_row - i - 1));
    }
#ifdef MUL_DBG
    binary_print(vector);
    binary_print(result);
#endif
    return result;
}

void encode(int input, uint8_t *outByteA, uint8_t *outByteB)
{
    uint8_t inByteA = (input >> 4) & 0xf;
    uint8_t inByteB = input & 0xf;

#ifdef ENC_DBG
    printf("inByteA: %x\n", inByteA);
#endif
    *outByteA = m_mul(G, inByteA, G_ROW);

#ifdef ENC_DBG
    printf("inByteB: %x\n", inByteB);
#endif
    *outByteB = m_mul(G, inByteB, G_ROW);
}

/**
 * @brief look up the syndrome table and correct for error
 */
static uint8_t correct(uint8_t inByte, uint8_t syndrome)
{
    for (int i = 0; i < HT_ROW; i++) {
        if (syndrome == HT[i]) {
            fprintf(stderr,
                    "Single bit flip detected, byte: %ld, bit: %d\n",
                    FILE_POS, i+1);
            return inByte ^ (1 << (HT_ROW - 1 - i));
        }
    }
    fprintf(stderr,
            "Uncorrectable error detected, byte: %ld, syndrome: 0x%02x\n",
            FILE_POS, syndrome);
    binary_print(syndrome);
    return inByte;
}

uint8_t decode(uint8_t inByteA, uint8_t inByteB)
{
    uint8_t parityA = m_mul(H, inByteA, H_ROW);
    uint8_t parityB = m_mul(H, inByteB, H_ROW);

    if (parityA) {
        inByteA = correct(inByteA, parityA);
    }

    if (parityB) {
        inByteB = correct(inByteB, parityB);
    }

    return ((inByteA & 0xf0) | (inByteB & 0xf0) >> 4);
}

