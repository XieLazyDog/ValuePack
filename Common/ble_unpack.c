#include <stdio.h>
#include <string.h>

#include "ble_value.h"

// 缓存数组
static uint8_t bits[PACK_RECEIVE_BIT_MAX];
static uint8_t bytes[PACK_RECEIVE_BYTE_MAX];
static int16_t i16s[PACK_RECEIVE_I16_MAX];
static int32_t i32s[PACK_RECEIVE_I32_MAX];
static float f32s[PACK_RECEIVE_F32_MAX];

// 设置 buffer、size 及各类型数据个数
emStat initValueUnpack(stPack *pack, uint8_t *buffer, uint16_t size_buff,
                       uint16_t nbit, uint16_t nbyte, uint16_t ni16, uint16_t ni32, uint16_t nf32) {
    // 检测参数合法性
    if (pack == NULL || buffer == NULL ||
        PACK_RECEIVE_SIZE_MAX < 3 ||
        size_buff > PACK_RECEIVE_SIZE_MAX ||
        nbit > PACK_RECEIVE_BIT_MAX ||
        nbyte > PACK_RECEIVE_BYTE_MAX ||
        ni16 > PACK_RECEIVE_I16_MAX ||
        ni32 > PACK_RECEIVE_I32_MAX ||
        nf32 > PACK_RECEIVE_F32_MAX) {
        // TODO: 打印 warning 信息
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    memset(&bits, 0, sizeof(bits));
    memset(&bytes, 0, sizeof(bytes));
    memset(&i16s, 0, sizeof(i16s));
    memset(&i32s, 0, sizeof(i32s));
    memset(&f32s, 0, sizeof(f32s));
    // 初始化解包参数
    pack->datas.bits = bits;
    pack->datas.bytes = bytes;
    pack->datas.i16s = i16s;
    pack->datas.i32s = i32s;
    pack->datas.f32s = f32s;
    pack->buffer = buffer;
    pack->index = size_buff;
    pack->datas.nums[TYPE_BIT] = nbit;
    pack->datas.nums[TYPE_BYTE] = nbyte;
    pack->datas.nums[TYPE_I16] = ni16;
    pack->datas.nums[TYPE_I32] = ni32;
    pack->datas.nums[TYPE_F32] = nf32;
    pack->endian = checkEndian();
    pack->is_start = 2;
    return STAT_OK;
}

// 使用前检查数据边界
#define CHECK_BUFFER()                               \
    if (pbuffer - pack->buffer >= pack->index - 2) { \
        return STAT_ERR;                             \
    }
// 保证后续读取不出错
#define CHECK_OFFSET()     \
    if (bit_offset != 0) { \
        pbuffer++;         \
        bit_offset = 0;    \
    }
// 根据传入的类型大小，分大小端解 buffer 到 data
#define FOR_UNPACK(PTR)                            \
    do {                                           \
        paddr = (uint8_t *)(PTR);                  \
        sizeV = (uint8_t)sizeof(*(PTR));           \
        for (ind = 0; ind < sizeV; ++ind) {        \
            CHECK_BUFFER();                        \
            if (ENDIAN_LITTLE == pack->endian) {   \
                paddr[ind] = *pbuffer;             \
            } else {                               \
                paddr[sizeV - ind - 1] = *pbuffer; \
            }                                      \
            pbuffer++;                             \
        }                                          \
    } while (0)
// 将 buffer 解析为 datas
static emStat unpackBuffer(stPack *pack) {
    // bit 类型位偏移量
    uint16_t bit_offset = 0;
    uint8_t *pbuffer = pack->buffer + 1;
    emType i;
    uint16_t n;
    uint8_t *paddr;
    uint8_t sizeV;
    uint8_t ind;
    // 检测参数合法性
    if (pack == NULL || pack->is_start != 2) {
        // TODO: 打印 warning 信息
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    // 按顺序打包数据
    for (i = TYPE_BIT; i < TYPE_MAX; ++i) {
        for (n = 0; n < pack->datas.nums[i]; ++n) {
            switch (i) {
                case TYPE_BIT: {
                    CHECK_BUFFER();
                    pack->datas.bits[n] = ((*pbuffer) >> bit_offset) & 0x01;
                    bit_offset++;
                    if (bit_offset >= 8) {
                        bit_offset = 0;
                        pbuffer++;
                    }
                } break;
                case TYPE_BYTE: {
                    CHECK_OFFSET();
                    FOR_UNPACK(pack->datas.bytes + n);
                } break;
                case TYPE_I16: {
                    CHECK_OFFSET();
                    FOR_UNPACK(pack->datas.i16s + n);
                } break;
                case TYPE_I32: {
                    CHECK_OFFSET();
                    FOR_UNPACK(pack->datas.i32s + n);
                } break;
                case TYPE_F32: {
                    CHECK_OFFSET();
                    FOR_UNPACK(pack->datas.f32s + n);
                } break;
                default: {
#ifdef DEBUG
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
                    return STAT_ERR;
                }
            }
        }
    }
    return STAT_OK;
}
#undef CHECK_BUFFER
#undef CHECK_OFFSET
#undef FOR_UNPACK

// 解析接收到的 buffer 包
emStat runValueUnpack(stPack *pack) {
    uint8_t sum = 0;
    uint16_t i;
    // 检测参数合法性
    if (pack == NULL || pack->is_start != 2) {
        // TODO: 打印 warning 信息
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    // 检测包结构，检验校验和
    for (i = 1; i < pack->index - 2; ++i) {
        sum += pack->buffer[i];
    }
    if (pack->buffer[0] != PACK_HEAD ||
        pack->buffer[pack->index - 1] != PACK_TAIL ||
        sum != pack->buffer[pack->index - 2]) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    // 从 buffer 解出数据到 datas
    if (STAT_OK == unpackBuffer(pack)) {
        pack->is_start = 0;
        return STAT_OK;
    } else {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
}
