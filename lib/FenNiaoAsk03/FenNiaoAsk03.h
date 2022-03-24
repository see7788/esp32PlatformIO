//
// Created by 13520 on 2022/3/23.
//

#ifndef ESP32PLATFORMIO_FENNIAOASK03_H
#define ESP32PLATFORMIO_FENNIAOASK03_H
#include "RCSwitch.h"

class FenNiaoAsk03
{
private:
    RCSwitch *iSwitch;
public:
    FenNiaoAsk03(RCSwitch *RCSwitchObj,int RXgpio);
    void available();
};

#endif // ESP32PLATFORMIO_FENNIAOASK03_H
