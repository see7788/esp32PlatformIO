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
JsonObject websocketCli = globalc.createNestedObject("websocketCli");
JsonObject WebsocketServe = globalc.createNestedObject("WebsocketServe");
JsonObject wifiCli = globalc.createNestedObject("wifiCli");
JsonObject wifiServe = globalc.createNestedObject("wifiServe");
JsonObject kenWeis = globalc.createNestedObject("kenWeis");
JsonObject fangjian = globalc.createNestedObject("fangjian");
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
    void globalDocInit(void)
    {
        websocketCli["uri"] = "8080";
        WebsocketServe["port"] = "8080";
        wifiCli["ssid"] = "920wifi";
        wifiCli["password"] = "12345678@";
        wifiServe["name"] = "8080test";
        wifiServe["ssid"] = "8080";
        wifiServe["password"] = "8080";
        fangjian["floor"] = "1";
        fangjian["name"] = "1-2A";
        fangjian["sex"] = "boy";
        fangjian["kengshu"] = 12;
        fangjian["kenging"] = 0;
        serializeJson(globalDoc, Serial);
    };
    static void kenWeisSet(bool isAdd, String id, bool state)
    {
        DynamicJsonDocument obj(255);
        obj["id"] = id;
        obj["state"] = state;
        globalDoc["kenWeis"].getMember(id);
        if (isAdd || globalDoc["kenWeis"].containsKey(id))
        {
            globalDoc["kenWeis"][id].set(obj);
        }
        // serializeJson(globalDoc, Serial); //打印
        serializeJsonPretty(globalDoc, Serial); //带格式打印
    };
    const String globalFileName = "/global.json";
    String SPIFFSMsg = "SPIFFS loadIng";
    void SPIFFSinit()
    {
        if (!SPIFFS.begin(true))
        {
            SPIFFSMsg = "SPIFFS error";
        }
        else
        {
            SPIFFSMsg = "SPIFFS success";
        }
    };
    void globalDocFromFileRead()
    {
        File file = SPIFFS.open(globalFileName);
        String str = "";
        if (file && file.isDirectory())
        {
            while (file.available())
            {
                str += file.read();
            }
            DeserializationError error = deserializeJson(globalDoc, str); //反序列化
            if (error)
            {
                Serial.println("deserializeJson error");
            }
        }
        file.close();
    };
    void globalDocToFileWrite()
    {
        File file = SPIFFS.open(globalFileName, FILE_WRITE);
        if (!file)
        {
            Serial.println("- failed to open file for writing");
            return;
        }
        if (file.print("{}"))
        {
            Serial.println("- file written");
        }
        else
        {
            Serial.println("- write failed");
        }
        file.close();
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
    static void fromYbl(YBLEMR03Uart::ParseRx7e v)
    {
        String id = "ybl" + String(v.id);
        String type = String(v.type);
        bool state = false;
        if (type == "1")
        {
            state = String(v.state) == "8" ? true : false;
            kenWeisSet(true, id, state);
        }
        else if (type == "2")
        {
            state = String(v.state) == "1" ? true : false;
            kenWeisSet(true, id, state);
        }
    }
    static void fromFendNiao(FenNiaoAsk03::ParseDb v)
    {
        String id = "x1";
        String cid = String(v.id);
        if (cid == "13217070")
        {
            kenWeisSet(true, id, true); //门磁合并
        }
        else if (cid == "13217060")
        {
            kenWeisSet(true, id, false); //门磁分离
        }
    };
    static void sendToLaozhou(void)
    {
        JsonObject laozhouObj = globalDoc["fangjian"];
        laozhouObj["louceng"] = "1";
        laozhouObj["Name"] = "1-2A";
        laozhouObj["renshu"] = laozhouObj["kenging"];
        serializeJsonPretty(laozhouObj, Serial);
    };
    unsigned long sendTolastTime = 0;
    void sendToloop(void)
    {
        unsigned long now = millis();
        if (now - sendTolastTime > 1000)
        {
            unsigned int nowRenshu = 0;
            JsonObject kenWeisInfo = globalDoc["kenWeis"];
            for (auto kv : kenWeisInfo)
            {
                if (kv.value()["state"])
                {
                    nowRenshu += 1;
                };
            };
            globalDoc["fangjian"]["kenging"] = nowRenshu;
            sendToLaozhou();
            sendTolastTime = now;
        }
    }

public:
    Os(void)
    {
        Serial.begin(1200);
        // SPIFFSinit();
        // wifiCliInit();
        // wifiServeInit();
        iYbl = new YBLEMR03Uart(new HardwareSerial(1), 23);
        iFenNiaoAsk03 = new FenNiaoAsk03(new RCSwitch(), 25);
        //  ESP.restart();//重启
        //  ESP.getFreeHeap();//剩余内存
        globalDocInit();
        //  globalDocFromFileRead();
        //  hallLedInit();
    };
    void loop()
    {
        // wifiServeStatePrint();
        delay(100);
        iYbl->parseRx7e(fromYbl);
        delay(5);
        iFenNiaoAsk03->available(fromFendNiao);
        delay(5);
        // hallStateLoop();
        // hallLedLoop();
        // delay(5);
        sendToloop();
        // serializeJson(globalDoc, Serial);
    };
};
