#include <Arduino.h>
#include <ArduinoJson.h>
#include <RCSwitch.h>
#include <HardwareSerial.h>
#include <SPIFFS.h>
#include "YBLEMR03Uart.h"
#include "FenNiaoAsk03.h"
struct SuccessInit
{
    bool Serial;
    bool SPIFFS;
    bool WifiStation;
} successInit{
    Serial : false,
    SPIFFS : false,
    WifiStation : false,
};

unsigned long fangjianSendTolastTime = 0;
DynamicJsonDocument globaldoc(1024);
JsonObject globalConfigObject = globaldoc.to<JsonObject>();
JsonObject websocketCliConfig = globalConfigObject.createNestedObject("websocketCli");
JsonObject websocketServeConfig = globalConfigObject.createNestedObject("websocketServe");
JsonObject wifiCliConfig = globalConfigObject.createNestedObject("wifiCli");
JsonObject wifiServeConfig = globalConfigObject.createNestedObject("wifiServe");
JsonObject fangjianConfig = globalConfigObject.createNestedObject("fangjian");
JsonObject kenWeisConfig = globalConfigObject.createNestedObject("kenWeis");
YBLEMR03Uart *iYbl;          // = new YBLEMR03Uart(new HardwareSerial(1), 23);
FenNiaoAsk03 *iFenNiaoAsk03; // = new FenNiaoAsk03(new RCSwitch(), 25);

bool inHomeIng()
{
    return hallRead() < 0 ? true : false;
};
void globalFile(bool isWrite = false)
{
    while (!SPIFFS.begin(true))
    {
        Serial.print(".");
    }
    const String globalFileName = "/global.json";
    if (isWrite)
    {
        // Serial.printf("SPIFFS1 totalBytes: %d \r\n", SPIFFS.totalBytes());
        // Serial.printf("SPIFFS1 usedBytes: %d \r\n", SPIFFS.usedBytes());
        File file = SPIFFS.open(globalFileName, FILE_WRITE);
        serializeJson(globalConfigObject, file);
        file.close();
    }
    else if (SPIFFS.exists(globalFileName) == true)
    {
        // const size_t l = JSON_OBJECT_SIZE(8);
        DynamicJsonDocument doc(1024);
        File file = SPIFFS.open(globalFileName, FILE_READ);
        deserializeJson(doc, file);
        file.close();
        globalConfigObject["websocketCli"].set(doc["websocketCli"]);
        globalConfigObject["websocketServe"].set(doc["websocketServe"]);
        globalConfigObject["wifiCli"].set(doc["wifiCli"]);
        globalConfigObject["wifiServe"].set(doc["wifiServe"]);
        globalConfigObject["fangjian"].set(doc["fangjian"]);
        globalConfigObject["kenWeis"].set(doc["kenWeis"]);
    }
    else
    {
        websocketCliConfig["uri"] = "8080";
        websocketServeConfig["port"] = "8080";
        wifiCliConfig["ssid"] = "920wifi";
        wifiCliConfig["password"] = "12345678@";
        wifiServeConfig["name"] = "8080test";
        wifiServeConfig["ssid"] = "8080";
        wifiServeConfig["password"] = "8080";
        fangjianConfig["floor"] = "1";
        fangjianConfig["name"] = "1-2A";
        fangjianConfig["sex"] = "boy";
        fangjianConfig["kengadd"] = true;
    };
    // serializeJsonPretty(globalConfigObject, Serial);
    // serializeJsonPretty(kenWeis, Serial);
};
void globalSettingApi(JsonObject globalConfigJson, String cilFunction){

};

void wifiCliInit(){};
void wifiServeInit(){};
void websocketCliInit(){};
void websocketServeInit(){};
void httpServeInit(){};
void httpSerialServeInit(){};

void kenWeisSet(String id, bool state)
{
    //  globalSuccess =false;
    // kenWeis.getMember(id);
    if (kenWeisConfig.containsKey(id))
    {
        kenWeisConfig[id]["state"] = state;
    }
    else if (fangjianConfig["kengadd"])
    {
        kenWeisConfig.createNestedObject(id);
        kenWeisConfig[id]["state"] = state;
    }
    serializeJsonPretty(globalConfigObject, Serial);
    //  globalSuccess =true;
};

void kenWeisFromYbl(YBLEMR03Uart::ParseRx7e v)
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
};

void kenWeisFromFendNiao(FenNiaoAsk03::ParseDb v)
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

void fangjianLoopTo(void)
{
    unsigned long nowtime = millis();
    if (nowtime - fangjianSendTolastTime < 1000)
    {
        return;
    };

    unsigned int nowRenshu = 0;
    for (auto kv : kenWeisConfig)
    {
        if (kv.value()["state"] == true)
        {
            nowRenshu += 1;
        };
    };
    fangjianConfig["renshu"] = nowRenshu;
    fangjianConfig["kengshu"] = kenWeisConfig.size();
    serializeJsonPretty(globalConfigObject, Serial); //带格式打印
    // serializeJson(fangjianConfig, Serial); //打印
    fangjianSendTolastTime = nowtime;
};
void setup()
{
    Serial.begin(9600);
    while (!Serial)
    {
        Serial.print(".");
    }
    successInit.Serial = true;
    globalFile(false);
    successInit.SPIFFS = true;
    wifiCliInit();
    successInit.WifiStation = true;
    iYbl = new YBLEMR03Uart(new HardwareSerial(1), 23);
    iFenNiaoAsk03 = new FenNiaoAsk03(new RCSwitch(), 25);
    // wifiCliInit();
    // wifiServeInit();
    //  ESP.restart();//重启
    //  ESP.getFreeHeap();//剩余内存
    //   hallLedInit();
};

void loop()
{
    // Serial.println(1);
    // Serial.println(SPIFFS.exists(globalFileName) ? "y" : "n");
    // delay(1000);
    // globalDocToFileWrite();
    // delay(1000);
    // wifiServeStatePrint();
    //  hallStateLoop();
    //  hallLedLoop();
    iYbl->parseRx7e(kenWeisFromYbl);
    delay(5);
    iFenNiaoAsk03->available(kenWeisFromFendNiao);
    delay(5);
    fangjianLoopTo();
    delay(5);
};
