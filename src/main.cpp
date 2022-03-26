#include <Arduino.h>
#include "Os.cpp"
Os *ios;
void setup()
{
 ios=new Os();
};
void loop()
{
  ios->loop();//这个看上去没有初始化类
}
