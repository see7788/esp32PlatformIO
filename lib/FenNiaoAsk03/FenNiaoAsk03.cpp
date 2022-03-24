//
// Created by 13520 on 2022/3/23.
//

#include "FenNiaoAsk03.h"
#include "RCSwitch.h"

FenNiaoAsk03::FenNiaoAsk03(RCSwitch *RCSwitchObj, int RXgpio)
{
  pinMode(RXgpio, INPUT);
  this->iSwitch = RCSwitchObj;
  this->iSwitch->enableReceive(digitalPinToInterrupt(RXgpio));
};
void FenNiaoAsk03::available()
{
  if (this->iSwitch->available())
  {
    //   String s="abcde";
    // Serial.println("s=");
    // Serial.println(s);
    Serial.print("Received ");
    Serial.print(this->iSwitch->getReceivedValue());
    Serial.print(" / ");
    Serial.print(this->iSwitch->getReceivedBitlength());
    Serial.print("bit ");
    Serial.print("Protocol: ");
    Serial.println(this->iSwitch->getReceivedProtocol());
    this->iSwitch->resetAvailable();
  }
}