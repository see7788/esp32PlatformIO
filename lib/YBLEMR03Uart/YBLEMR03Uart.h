#ifndef _EMR03SERIAL_H_
#define _EMR03SERIAL_H_

#include <HardwareSerial.h>
// FSK串口接收
class YBLEMR03Uart
{
    // void model_bm1(); //英式小按钮
    // void model_d1();  //门磁
    // void model_x();   //合页侧开关
public:
    YBLEMR03Uart(HardwareSerial *iuartObj, int rxGpio);
    typedef unsigned char Rx7e[12];
    typedef struct
    {
        String id;
        String type;
        unsigned char state;
    } ParseRx7e;

protected:
    HardwareSerial *iuartObj;
    typedef void (*Rx7eCB)(Rx7e op);
    typedef void (*ParseRx7eCB)(ParseRx7e op);
public:
    void rx7e(Rx7eCB cb);
    void parseRx7e(ParseRx7eCB cb);
};

#endif
