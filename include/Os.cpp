#include <ArduinoJson.h>
#include <RCSwitch.h>
#include <HardwareSerial.h>
#include <WiFi.h>
#include <WiFiAP.h>
#include <SPIFFS.h>
#include "YBLEMR03Uart.h"
#include "FenNiaoAsk03.h"
DynamicJsonDocument globalDoc(2054);
JsonObject globalc = globalDoc.to<JsonObject>();
JsonObject websocketCliConfig = globalc.createNestedObject("websocketCli");
JsonObject WebsocketServeConfig = globalc.createNestedObject("WebsocketServe");
JsonObject wifiCliConfig = globalc.createNestedObject("wifiCli");
JsonObject wifiServeConfig = globalc.createNestedObject("wifiServe");
JsonObject fangjianConfig = globalc.createNestedObject("fangjian");
JsonObject kenWeisConfig = globalc.createNestedObject("kenWeis");
class Os
{
private:
    YBLEMR03Uart *iYbl = new YBLEMR03Uart(new HardwareSerial(1), 23);
    FenNiaoAsk03 *iFenNiaoAsk03 = new FenNiaoAsk03(new RCSwitch(), 25);
    bool hallState = false;
    void hallStateLoop()
    {
        if (hallRead() < 0)
        {
            hallState = true;
            //  digitalWrite(hallLedGpio, HIGH);
        }
        else
        {
            hallState = false;
            // digitalWrite(hallLedGpio, LOW);
        }
    };
    void hallLedInit(){

    };
    void hallLedLoop(){

    };
    const String globalFileName = "/global.json";
    void globalDocInit(void)
    {
        while (!SPIFFS.begin(true))
        {
            Serial.print("...SPIFFS.begin()");
            delay(200);
        }

        // serializeJson(globalDoc, Serial);
        if (SPIFFS.exists(globalFileName) == true)
        {
            File file = SPIFFS.open(globalFileName);
            String c = file.readString();
            deserializeJson(globalDoc, c);
            file.close();
        }
        else
        {
            websocketCliConfig["uri"] = "8080";
            WebsocketServeConfig["port"] = "8080";
            wifiCliConfig["ssid"] = "920wifi";
            wifiCliConfig["password"] = "12345678@";
            wifiServeConfig["name"] = "8080test";
            wifiServeConfig["ssid"] = "8080";
            wifiServeConfig["password"] = "8080";
            fangjianConfig["floor"] = "1";
            fangjianConfig["name"] = "1-2A";
            fangjianConfig["sex"] = "boy";
            fangjianConfig["kengadd"] = true;
            fangjianConfig["kengshu"] = 12;
            fangjianConfig["renshu"] = 0;
        }
    };
    void globalDocToFileWrite()
    {
        while (!SPIFFS.begin(true))
        {
            Serial.print("...SPIFFS.begin()");
            delay(200);
        }
        // Serial.printf("SPIFFS1 totalBytes: %d \r\n", SPIFFS.totalBytes());
        // Serial.printf("SPIFFS1 usedBytes: %d \r\n", SPIFFS.usedBytes());
        File file = SPIFFS.open(globalFileName, FILE_WRITE);
        String c;
        serializeJson(globalDoc, c);
        file.print(c);
        file.close();
    };
    void apiInit(){};
    void apiOn(){};
    static void kenWeisSet(String id, bool state)
    {
        // globalDoc["kenWeis"].getMember(id);
        if (kenWeisConfig.containsKey(id))
        {
            kenWeisConfig[id]["state"] = state;
        }
        else if (fangjianConfig["kengadd"])
        {
            kenWeisConfig.createNestedObject(id);
            kenWeisConfig[id]["state"] = state;
        }
    };
    static void kenWeisFromYbl(YBLEMR03Uart::ParseRx7e v)
    {
        String id = "ybl" + String(v.id);
        String type = String(v.type);
        bool state = false;
        if (type == "1")
        {
            //门磁
            state = String(v.state) == "8" ? true : false;
            kenWeisSet(id, state);
        }
        else if (type == "2")
        {
            //英式按钮
            state = String(v.state) == "1" ? true : false;
            kenWeisSet(id, state);
        }
    }
    static void kenWeisFromFendNiao(FenNiaoAsk03::ParseDb v)
    {
        String id = "x1";
        String cid = String(v.id);
        if (cid == "13217070")
        {
            kenWeisSet(id, true); //门磁合并
        }
        else if (cid == "13217060")
        {
            kenWeisSet(id, false); //门磁分离
        }
    };

    WiFiAPClass WiFiAP;
    void wifiServeInit()
    {
        // Set these to your desired credentials.
        const char *ssid = "tastAP";
        const char *password = "123456";

        uint8_t x = 1;
        IPAddress local_IP(192, 168, x, 1);
        IPAddress gateway(192, 168, x, 1);
        IPAddress subnet(255, 255, 255, 0);
        // WiFi.softAP(wifiCli["ssid"].as<const char *>());
        WiFiAP.softAPConfig(local_IP, gateway, subnet);
        WiFiAP.softAP(ssid, password);
    };
    void wifiServeStatePrint()
    {
        delay(2000);
        Serial.print("AP IP address: ");
        Serial.println(WiFiAP.softAPIP());

        Serial.print("softAP Broadcast IP: ");
        Serial.println(WiFiAP.softAPBroadcastIP());

        Serial.print("softAP NetworkID: ");
        Serial.println(WiFiAP.softAPNetworkID());

        Serial.print("softAP SubnetCIDR: ");
        Serial.println(WiFiAP.softAPSubnetCIDR());

        Serial.print("softAP Hostname: ");
        Serial.println(WiFiAP.softAPgetHostname());

        Serial.print("softAP macAddress: ");
        Serial.println(WiFiAP.softAPmacAddress());

        Serial.print("softAP StationNum: ");
        Serial.println(WiFiAP.softAPgetStationNum());
    };
    void wifiCliInit()
    {
        const char *wifi_SSID = "920wifi";       //设置连接的wifi名称信息
        const char *wifi_Password = "12345678@"; //设置连接的wifi密码信息
        WiFi.begin(wifi_SSID, wifi_Password);
        // WiFi.onfig配置
        //  WiFi.disconnect();关闭
    };
    void wifiCliStatePrint()
    {
        if (!WiFi.isConnected())
        {
            delay(500);
            Serial.print("WIFI STA loading...");
        }
        else
        {
            Serial.println("WIFI STA Success");
        }
    };
    unsigned long fangjianSendTolastTime = 0;
    void fangjianLoopTo(void)
    {
        unsigned long now = millis();
        if (now - fangjianSendTolastTime > 1000)
        {
            unsigned int nowRenshu = 0;
            for (auto kv : kenWeisConfig)
            {
                if (kv.value()["state"] == true)
                {
                    nowRenshu += 1;
                };
            };
            fangjianConfig["renshu"] = nowRenshu;
            // serializeJson(globalDoc, Serial); //打印
            serializeJsonPretty(globalDoc, Serial); //带格式打印
            fangjianSendTolastTime = now;
        }
    }

public:
    Os(void)
    {
        Serial.begin(9600);
        // wifiCliInit();
        // wifiServeInit();
        // iYbl = new YBLEMR03Uart(new HardwareSerial(1), 23);
        // iFenNiaoAsk03 = new FenNiaoAsk03(new RCSwitch(), 25);
        //  ESP.restart();//重启
        //  ESP.getFreeHeap();//剩余内存
        globalDocInit();
        // globalDocToFileWrite();
        //   globalDocFromFileRead();
        //   hallLedInit();
    };
    void loop()
    {
        Serial.println(SPIFFS.exists(globalFileName) ? "y" : "n");
        delay(1000);
        globalDocToFileWrite();
        delay(1000);
        deserializeJson(globalDoc,Serial);
        //  serializeJson(globalDoc, Serial);
        // wifiServeStatePrint();
        //  iYbl->parseRx7e(kenWeisFromYbl);
        //  delay(5);
        //  iFenNiaoAsk03->available(kenWeisFromFendNiao);
        //  delay(5);
        //  hallStateLoop();
        //  hallLedLoop();
        //  delay(5);
        //  fangjianLoopTo();
        //  serializeJson(globalDoc, Serial);
    };
};
