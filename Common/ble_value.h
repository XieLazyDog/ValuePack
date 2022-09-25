#ifndef __BLE_VALUE_H__
#define __BLE_VALUE_H__

#include <assert.h>
#include <stdint.h>

#define PACK_HEAD 0xA5
#define PACK_TAIL 0x5A

#define PACK_SEND_SIZE_MAX 256
#define PACK_SEND_BIT_MAX 8
#define PACK_SEND_BYTE_MAX 8
#define PACK_SEND_I16_MAX 8
#define PACK_SEND_I32_MAX 8
#define PACK_SEND_F32_MAX 8

#define PACK_RECEIVE_SIZE_MAX 256
#define PACK_RECEIVE_BIT_MAX 8
#define PACK_RECEIVE_BYTE_MAX 8
#define PACK_RECEIVE_I16_MAX 8
#define PACK_RECEIVE_I32_MAX 8
#define PACK_RECEIVE_F32_MAX 8

typedef enum {
    STAT_OK = 0,
    STAT_ERR = 1,
} emStat;

typedef enum {
    ENDIAN_CHECK = 0x1234,
    ENDIAN_BIG = 0x12,
    ENDIAN_LITTLE = 0x34,
} emEndian;

typedef enum {
    TYPE_BIT = 0,
    TYPE_BYTE = 1,
    TYPE_I16 = 2,
    TYPE_I32 = 3,
    TYPE_F32 = 4,
    TYPE_MAX,
} emType;

typedef struct {
    uint16_t nums[TYPE_MAX];
    uint8_t *bits;
    uint8_t *bytes;
    int16_t *i16s;
    int32_t *i32s;
    float *f32s;
} stData;

typedef struct {
    emEndian endian;
    uint16_t index;
    uint8_t *buffer;
    uint8_t is_start;
    stData datas;
} stPack;

emEndian checkEndian(void);

emStat startValuePack(stPack *pack);
emStat endValuePack(stPack *pack);
emStat putBool(stPack *pack, uint8_t v);
emStat putByte(stPack *pack, uint8_t v);
emStat putShort(stPack *pack, int16_t v);
emStat putInt(stPack *pack, int32_t v);
emStat putFloat(stPack *pack, float v);

emStat initValueUnpack(stPack *pack, uint8_t *buffer, uint16_t size_buff,
                       uint16_t nbit, uint16_t nbyte, uint16_t ni16, uint16_t ni32, uint16_t nf32);
emStat runValueUnpack(stPack *pack);

#ifdef DEBUG
#define CHECK_ERR(FUNC) assert(STAT_OK == (FUNC))
#else
#define CHECK_ERR(FUNC) FUNC
#endif

#endif
