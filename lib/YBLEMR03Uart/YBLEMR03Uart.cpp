#include "HardwareSerial.h"
#include "YBLEMR03Uart.h"
// FSK串口接收
YBLEMR03Uart::YBLEMR03Uart(HardwareSerial *uartObj, int rxGpio)
{
    this->iuartObj = uartObj;
    this->iuartObj->begin(9600, SERIAL_8N1, rxGpio);
};

void YBLEMR03Uart::available(callback7e callback7e)
{
    unsigned char v[50];
    v[0] = this->iuartObj->read();
    // Serial.println(v[0], 16);
    // Serial.println(0x7E);
    if (v[0] == 0x7E)
    {
        v[1] = this->iuartObj->read() + 3;
        for (int i = 2; v[1] > i; i++)
        {
            v[i] = this->iuartObj->read();
            delay(2);
        }
        if (v[8] == 0xEF)
        {
            Db7e c;
            c.id[0] = v[3];
            c.id[1] = v[4];
            c.id[2] = v[5];
            c.id[3] = v[6];
            c.type = v[7];
            c.state = v[9];
            callback7e(c);
            this->available(callback7e);
        }
    }
};
// void YBLEMR03Uart::available(HardwareSerial *uartObj,callback7e callback7e)
// {
//     int l = this->uartObj->available();
//     UART_RX_CHAR v;
//     for (int m = 0; l > 0 && l > m && UART_RX_LENGTH > m; m++)
//     {
//         v[m] = this->uartObj->read();
//         delay(20);
//     };
//     for (int i = 0; l > i;)
//     {
//         if (v[i] == 0x7E)
//         {
//             if (v[i + 8] == 0xEF)
//             {
//                 Serial.println(l);
//                 Serial.println(i);
//                 //     Db7e c;
//                 //     c.id[0] = v[i + 3];
//                 //     c.id[1] = v[i + 4];
//                 //     c.id[2] = v[i + 5];
//                 //     c.id[3] = v[i + 6];
//                 //     c.type = v[i + 7];
//                 //     c.state = v[i + 9];
//                 //    callback7e(c);
//                 i = v[1] + 3; //数据长度
//             };
//         }
//         else
//         {
//             i = i + 1;
//         };
//         delay(20);
//     }
// };