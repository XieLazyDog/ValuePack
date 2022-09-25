#include <stdint.h>
#include <stdio.h>

#include "ble_value.h"

int main(int argc, char *argv[]) {
    // 打包数据
    stPack pack;
    CHECK_ERR(startValuePack(&pack));
    CHECK_ERR(putBool(&pack, 1));
    CHECK_ERR(putBool(&pack, 1));
    CHECK_ERR(putBool(&pack, 0));
    CHECK_ERR(putBool(&pack, 1));
    CHECK_ERR(putByte(&pack, 0x11));
    CHECK_ERR(putByte(&pack, 0x83));
    CHECK_ERR(putInt(&pack, 0x7856));
    CHECK_ERR(putShort(&pack, 0x1234));
    CHECK_ERR(putFloat(&pack, 3.14159));
    CHECK_ERR(endValuePack(&pack));

    for (int i = 0; i < pack.index; ++i) {
        printf("0x%02x ", pack.buffer[i]);
    }
    putchar('\n');

    // 解包数据
    stPack unpack;
    CHECK_ERR(initValueUnpack(&unpack, pack.buffer, pack.index,
                              pack.datas.nums[TYPE_BIT],
                              pack.datas.nums[TYPE_BYTE],
                              pack.datas.nums[TYPE_I16],
                              pack.datas.nums[TYPE_I32],
                              pack.datas.nums[TYPE_F32]));
    CHECK_ERR(runValueUnpack(&unpack));

    for (emType i = TYPE_BIT; i < TYPE_MAX; ++i) {
        for (uint16_t n = 0; n < unpack.datas.nums[i]; ++n) {
            switch (i) {
                case TYPE_BIT: {
                    printf("%hhu, ", unpack.datas.bits[n]);
                } break;
                case TYPE_BYTE: {
                    printf("%hhu(%#x), ", unpack.datas.bytes[n], unpack.datas.bytes[n]);
                } break;
                case TYPE_I16: {
                    printf("%hd(%#x), ", unpack.datas.i16s[n], unpack.datas.i16s[n]);
                } break;
                case TYPE_I32: {
                    printf("%d(%#x), ", unpack.datas.i32s[n], unpack.datas.i32s[n]);
                } break;
                case TYPE_F32: {
                    printf("%f, ", unpack.datas.f32s[n]);
                } break;
                default: {
                    printf("%s %s:%d\n", __func__, __FILE__, __LINE__);
                }
            }
        }
        putchar('\n');
    }

    return 0;
}
