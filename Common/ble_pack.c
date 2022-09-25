#include <stdio.h>
#include <string.h>

#include "ble_value.h"

// 缓存数组
static uint8_t buffer[PACK_SEND_SIZE_MAX];
static uint8_t bits[PACK_SEND_BIT_MAX];
static uint8_t bytes[PACK_SEND_BYTE_MAX];
static int16_t i16s[PACK_SEND_I16_MAX];
static int32_t i32s[PACK_SEND_I32_MAX];
static float f32s[PACK_SEND_F32_MAX];

// 检测处理器字节序
emEndian checkEndian(void) {
    uint16_t check = ENDIAN_CHECK;
    return (emEndian)(((uint8_t *)(&check))[0]);
}

// 数据打包之前的准备工作，初始化数组
emStat startValuePack(stPack *pack) {
    // 检测参数合法性
    if (pack == NULL || PACK_SEND_SIZE_MAX < 3) {
        // TODO: 打印 warning 信息
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    memset(&buffer, 0, sizeof(buffer));
    memset(&bits, 0, sizeof(bits));
    memset(&bytes, 0, sizeof(bytes));
    memset(&i16s, 0, sizeof(i16s));
    memset(&i32s, 0, sizeof(i32s));
    memset(&f32s, 0, sizeof(f32s));
    // 初始化打包参数
    pack->buffer = buffer;
    memset(&pack->datas, 0, sizeof(pack->datas));
    pack->datas.bits = bits;
    pack->datas.bytes = bytes;
    pack->datas.i16s = i16s;
    pack->datas.i32s = i32s;
    pack->datas.f32s = f32s;
    pack->endian = checkEndian();
    pack->index = 0;
    pack->buffer[pack->index++] = PACK_HEAD;
    pack->is_start = 1;
    return STAT_OK;
}

// 避免覆盖 bit 字节
#define CHECK_OFFSET()     \
    if (bit_offset != 0) { \
        pack->index++;     \
        bit_offset = 0;    \
    }
// 根据传入的类型大小，分大小端填充 data 到 buffer
#define FOR_PACK(PTR)                                                 \
    do {                                                              \
        paddr = (uint8_t *)(PTR);                                     \
        sizeV = (uint8_t)sizeof(*(PTR));                              \
        if (pack->index + sizeV >= PACK_SEND_SIZE_MAX) {              \
            return STAT_ERR;                                          \
        }                                                             \
        for (ind = 0; ind < sizeV; ++ind) {                           \
            if (ENDIAN_LITTLE == pack->endian) {                      \
                pack->buffer[pack->index++] = paddr[ind];             \
            } else {                                                  \
                pack->buffer[pack->index++] = paddr[sizeV - ind - 1]; \
            }                                                         \
        }                                                             \
    } while (0)
// 将 data 转化为可发送的 buffer
static emStat packValue(stPack *pack) {
    // bit 类型位偏移量
    uint16_t bit_offset = 0;
    emType i;
    uint16_t n;
    uint8_t *paddr;
    uint8_t sizeV;
    uint8_t ind;
    // 检测参数合法性
    if (pack == NULL || pack->is_start != 1) {
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
                    if (pack->datas.bits[n]) {
                        pack->buffer[pack->index] |= 0x01 << bit_offset;
                    } else {
                        pack->buffer[pack->index] &= ~(0x01 << bit_offset);
                    }
                    bit_offset++;
                    if (bit_offset >= 8) {
                        bit_offset = 0;
                        if (pack->index + 1 >= PACK_SEND_SIZE_MAX) {
#ifdef DEBUG
                            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
                            return STAT_ERR;
                        }
                        pack->index++;
                    }
                } break;
                case TYPE_BYTE: {
                    CHECK_OFFSET();
                    FOR_PACK(pack->datas.bytes + n);
                } break;
                case TYPE_I16: {
                    CHECK_OFFSET();
                    FOR_PACK(pack->datas.i16s + n);
                } break;
                case TYPE_I32: {
                    CHECK_OFFSET();
                    FOR_PACK(pack->datas.i32s + n);
                } break;
                case TYPE_F32: {
                    CHECK_OFFSET();
                    FOR_PACK(pack->datas.f32s + n);
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
#undef CHECK_OFFSET
#undef FOR_PACK

// 结束打包流程，将数据转化为可发送的 buffer
emStat endValuePack(stPack *pack) {
    uint8_t sum = 0;
    uint16_t i;
    // 检测参数合法性
    if (pack == NULL || pack->is_start != 1) {
        // TODO: 打印 warning 信息
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    // 将 datas 内数据填充到 buffer 内
    if (STAT_OK == packValue(pack)) {
        if (pack->index + 2 >= PACK_SEND_SIZE_MAX) {
#ifdef DEBUG
            printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
            return STAT_ERR;
        }
        // 计算校验和
        for (i = 1; i < pack->index; ++i) {
            sum += pack->buffer[i];
        }
        pack->buffer[pack->index++] = sum;
        pack->buffer[pack->index++] = PACK_TAIL;
        pack->is_start = 0;
        return STAT_OK;
    } else {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
}

emStat putBool(stPack *pack, uint8_t v) {
    // 检测参数合法性
    if (pack == NULL || pack->datas.nums[TYPE_BIT] >= PACK_SEND_BIT_MAX) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    pack->datas.bits[pack->datas.nums[TYPE_BIT]++] = v;
    return STAT_OK;
}

emStat putByte(stPack *pack, uint8_t v) {
    // 检测参数合法性
    if (pack == NULL || pack->datas.nums[TYPE_BYTE] >= PACK_SEND_BYTE_MAX) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    pack->datas.bytes[pack->datas.nums[TYPE_BYTE]++] = v;
    return STAT_OK;
}

emStat putShort(stPack *pack, int16_t v) {
    // 检测参数合法性
    if (pack == NULL || pack->datas.nums[TYPE_I16] >= PACK_SEND_I16_MAX) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    pack->datas.i16s[pack->datas.nums[TYPE_I16]++] = v;
    return STAT_OK;
}

emStat putInt(stPack *pack, int32_t v) {
    // 检测参数合法性
    if (pack == NULL || pack->datas.nums[TYPE_I32] >= PACK_SEND_I32_MAX) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    pack->datas.i32s[pack->datas.nums[TYPE_I32]++] = v;
    return STAT_OK;
}

emStat putFloat(stPack *pack, float v) {
    // 检测参数合法性
    if (pack == NULL || pack->datas.nums[TYPE_F32] >= PACK_SEND_F32_MAX) {
#ifdef DEBUG
        printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
#endif
        return STAT_ERR;
    }
    pack->datas.f32s[pack->datas.nums[TYPE_F32]++] = v;
    return STAT_OK;
}
