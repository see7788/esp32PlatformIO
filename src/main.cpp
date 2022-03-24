#include <Arduino.h>
#include <RCSwitch.h>
#include "YBLEMR03Uart.h"
#include "FenNiaoAsk03.h"
YBLEMR03Uart *iYbl = new YBLEMR03Uart(new HardwareSerial(1), 23);
FenNiaoAsk03 *iFenNiaoAsk03 = new FenNiaoAsk03(new RCSwitch(), 25);

void setup()
{
  Serial.begin(9600);
};
void fromYbl(YBLEMR03Uart::Db7e v)
{
  Serial.print("{ID:");
  Serial.print(v.id[0]);
  Serial.print(v.id[1]);
  Serial.print(v.id[2]);
  Serial.print(v.id[3]);
  Serial.print(",TYPE:");
  Serial.print(v.type);
  Serial.print(",VALUE:");
  Serial.print(v.state);
  Serial.print("}");
};

void loop()
{
  iYbl->available(fromYbl);
  delay(5);
  iFenNiaoAsk03->available();
  delay(5);
}
