#include "fec.h"
#include <stdio.h>
// #define DBG
/*
 H =
 0 1 1 1 | 1 0 0 0
 1 0 1 1 | 0 1 0 0
 1 1 0 1 | 0 0 1 0
 1 1 1 0 | 0 0 0 1
*/

/*
 G =
 1 0 0 0
 0 1 0 0
 0 0 1 0
 0 0 0 1
 -------
 0 1 1 1
 1 0 1 1
 1 1 0 1
 1 1 1 0
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


#define H_ROW       4
#define G_ROW       8

#ifdef DBG
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
#endif

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
#ifdef DBG
    binary_print(vector);
    binary_print(result);
#endif
    return result;
}

void encode(int input, uint8_t *outByteA, uint8_t *outByteB)
{
    uint8_t inByteA = (input >> 4) & 0xf;
    uint8_t inByteB = input & 0xf;

#ifdef DBG
    printf("inByteA: %x\n", inByteA);
#endif
    *outByteA = m_mul(G, inByteA, G_ROW);

#ifdef DBG
    printf("inByteB: %x\n", inByteB);
#endif
    *outByteB = m_mul(G, inByteB, G_ROW);
}

uint8_t decode(uint8_t inByteA, uint8_t inByteB)
{
    
}
