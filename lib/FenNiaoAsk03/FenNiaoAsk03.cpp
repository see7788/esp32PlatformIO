//
// Created by 13520 on 2022/3/23.
//

#include "FenNiaoAsk03.h"
#include <RCSwitch.h>

FenNiaoAsk03::FenNiaoAsk03(RCSwitch *RCSwitchObj,int RXgpio)
{
  pinMode(RXgpio, INPUT);
  this->iSwitch = RCSwitchObj;
  this->iSwitch->enableReceive(digitalPinToInterrupt(RXgpio));
};
void FenNiaoAsk03::available(callbackFun cb)
{
  if (this->iSwitch->available())
  {
    ParseDb v;
    v.id = this->iSwitch->getReceivedValue();
    v.state = this->iSwitch->getReceivedProtocol();
    v.stateBitLenght = this->iSwitch->getReceivedBitlength();
    cb(v);
    this->iSwitch->resetAvailable();
    this->available(cb);
  }
};