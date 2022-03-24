#ifndef _EMR03SERIAL_H_
#define _EMR03SERIAL_H_

#include "HardwareSerial.h"
class YBLEMR03Uart
{
protected:
    HardwareSerial *iuartObj;
    void model_bm1(); //英式小按钮
    void model_d1();  //门磁
    void model_x();   //合页侧开关
public:
    YBLEMR03Uart(HardwareSerial *iuartObj, int rxGpio);
    typedef struct
    {
        unsigned char id[4];
        unsigned char type;
        unsigned char state;
    } Db7e; //各种开关
    typedef void (*callback7e)(Db7e);
    void available(callback7e useData);
};

#endif
