//
// Created by 13520 on 2022/3/23.
//

#ifndef ESP32PLATFORMIO_FENNIAOASK03_H
#define ESP32PLATFORMIO_FENNIAOASK03_H
#include <RCSwitch.h>
// ASK串口接收
class FenNiaoAsk03
{
public:
    typedef struct
    {
        String id;
        unsigned int state;
        unsigned int stateBitLenght;//bit
    } ParseDb;
protected:
    RCSwitch *iSwitch;
    typedef void (*callbackFun)(ParseDb c);

public:
    FenNiaoAsk03(RCSwitch *RCSwitchObj,int RXgpio);
    void available(callbackFun cb);
};

#endif // ESP32PLATFORMIO_FENNIAOASK03_H
