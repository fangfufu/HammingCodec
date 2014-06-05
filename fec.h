#ifndef FEC_H
#include <stdint.h>

/**
 * @brief Encode a byte in [8,4] extended binary Hamming code
 * @param[in] input Input character, using int for compatibility with fgetc()
 * @param[out] byteA The first output byte
 * @param[out] byteB the second output byte
 */
void encode(int input, uint8_t *outByteA, uint8_t *outByteB);

/**
 * @brief Decode two bytes containing [8,4] extended Hamming code encoded data
 * @param[in] inByteA The first encoded byte
 * @param[in] inByteB The second encoded byte
 * @return The original data
 */
uint8_t decode(uint8_t inByteA, uint8_t inByteB);
#endif
