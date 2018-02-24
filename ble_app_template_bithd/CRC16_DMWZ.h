#ifndef crc16_DMWZ_H_
#define crc16_DMWZ_H_
extern uint16_t const crc16_table[256];
extern uint16_t bd_crc16(uint16_t crc, const uint8_t *buffer, uint16_t len);

#endif
