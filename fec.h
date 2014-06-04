#ifndef FEC_H
#include <stdint.h>

/**
 * @brief Encode a byte in [8,4] extended binary Hamming code
 * @param[in] input Input character, using int for compatibility with fgetc()
 * @param[out] byteA The first output byte
 * @param[out] byteB the second output byte
 */
void encode(int input, uint8_t *outByteA, uint8_t *outByteB);
#endif
