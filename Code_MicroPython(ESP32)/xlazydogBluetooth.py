# -*- coding:utf-8 -*-
# xlazydog蓝牙

from ustruct import unpack
import bluetooth,ble_uart_peripheral
        
class BLEpack:
    def __init__(self, mybool=0, mybyte=0, myshort=0, myint=0, myfloat=0,BLE_name='ESP32',max_bytes=128):
        self.ble = bluetooth.BLE()
        self.uart = ble_uart_peripheral.BLEUART(self.ble,name=BLE_name)
        self.max_bytes = max_bytes
        self.myhex = 0
        self.Nmbo = str(mybool)
        self.Nmby = str(mybyte)
        self.Nmsh = str(int(myshort/2))
        self.Nmin = str(int(myint/4))
        self.Nmfl = str(int(myfloat/4))
        self.Lebo = (1 , 1 + mybool)
        self.Leby = (1 + mybool , 1 + mybool + mybyte)
        self.Lesh = (1 + mybool + mybyte , 1 + mybool + mybyte + myshort)
        self.Lein = (1 + mybool + mybyte + myshort , 1 + mybool + mybyte + myshort + myint)
        self.Lefl = (1 + mybool + mybyte + myshort + myint , 1 + mybool + mybyte + myshort + myint + myfloat)
        self.Leck = (1 + mybool + mybyte + myshort + myint + myfloat , 1 + mybool + mybyte + myshort + myint + myfloat + 1)
        
    def read_buffer(self):
        self.myhex=self.uart.read(self.max_bytes)
        
    def get_bool(self):
        mybool = self.myhex[self.Lebo[0] : self.Lebo[1]]
        D = (128,64,32,16,8,4,2,1)
        Tbool = []
        for b in mybool :
            Tb = []
            for i in D:
                if b/i < 1:
                    Tb.append(0)
                else:
                    Tb.append(1)
                    b = b % i
            Tb.reverse()
            Tbool.extend(Tb)
        return Tbool

    def get_byte(self):
        mybyte = self.myhex[self.Leby[0] : self.Leby[1]]
        Tbyte = unpack(self.Nmby + "s", mybyte)[0].decode()
        return Tbyte

    def get_short(self):
        myshort = self.myhex[self.Lesh[0] : self.Lesh[1]]
        Tshort = unpack(self.Nmsh + "h", myshort)
        return Tshort

    def get_int(self):
        myint = self.myhex[self.Lein[0] : self.Lein[1]]
        Tint = unpack(self.Nmin + "i", myint)
        return Tint

    def get_float(self):
        myfloat = self.myhex[self.Lefl[0] : self.Lefl[1]]
        Tfloat = unpack(self.Nmfl + "f", myfloat)
        return Tfloat

    def check_all(self):
        mydata = self.myhex[1 : self.Lefl[1]]
        chk = myhex[self.Leck[0] : self.Leck[1]][0]
        sums = 0
        for i in mydata:
            sums = sums + i
        if hex(sums&0xff) == hex(chk):
            return True
        else:
            return False
        
    def sent_two_int(self,data):
        sums=0
        self.uart.write(0xA5)
        sums=data[0]+data[1]
        self.uart.write(data[0])
        self.uart.write(data[1])
        self.uart.write(sums)
        self.uart.write(0x5A)
