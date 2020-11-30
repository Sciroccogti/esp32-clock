
#include <stdlib.h>

#include <map>
#include <string>

#include "ArduinoJson.h"
#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "ImageData.h"
#include "amap.hpp"
#include "myepd.h"
#include "wifiConnect.h"

DynamicJsonDocument doc_time(512);

int timestamp = 10;

const char* url_time = "http://quan.suning.com/getSysTime.do";
std::map<std::string, std::string> m4 = {{"first", "second"},
                                           {"third", "fourth"},
                                           {"fifth", "sixth"},
                                           {"begin", "end"}};

std::map<std::string, std::string> WEATHER = {
    {"小雨", "WXYU.BMP"},   {"中雨", "WZYU.BMP"},    {"大雨", "WDYU.BMP"},
    {"暴雨", "WWET.BMP"},   {"晴", "WQING.BMP"},     {"多云", "WDYZQ.BMP"},
    {"阴", "WYIN.BMP"},     {"雷阵雨", "WLZYU.BMP"}, {"阵雨", "WYGTQ.BMP"},
    {"霾", "WFOG.BMP"},     {"雾", "WWU.BMP"},       {"雪", "WXUE.BMP"},
    {"雨夹雪", "WYJX.BMP"}, {"冰雹", "WBBAO.BMP"},   {"月亮", "WMOON.BMP"},
    {"深夜", "WSLEEP.BMP"}, {"日落", "SUMSET.BMP"},  {"日出", "SUNRISE.BMP"},
    {"雨", "WZYU.BMP"}};

// Create a new image cache named IMAGE_BW and fill it with white
UBYTE *BlackImage, *RYImage;  // Red or Yellow

void mydisp() {
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    char str[100];
    Paint_DrawString_EN(0, 0, "SSID:", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(0, 16, "IP:", &Font16, WHITE, BLACK);
    Paint_SelectImage(RYImage);
    Paint_Clear(WHITE);
    sprintf(str, "%s", ssid);
    Paint_DrawString_EN(66, 0, str, &Font16, WHITE, BLACK);
    sprintf(str, "%s", myIP.toString().c_str());
    Paint_DrawString_EN(66, 16, str, &Font16, WHITE, BLACK);
}

void setup() {
    // put your setup code here, to run once:
    // Serial port initialization
    Serial.begin(115200);
    delay(10);

    // Server initialization
    wifi_setup();

    printf("EPD_4IN2BC_test Demo\r\n");

    EPD_initSPI();

    // DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_4IN2BC_Init();
    EPD_4IN2BC_Clear();
    DEV_Delay_ms(500);

    // you have to edit the startup_stm32fxxx.s file and set a big enough heap
    // size

    UWORD Imagesize =
        ((EPD_4IN2BC_WIDTH % 8 == 0) ? (EPD_4IN2BC_WIDTH / 8)
                                     : (EPD_4IN2BC_WIDTH / 8 + 1)) *
        EPD_4IN2BC_HEIGHT;
    if ((BlackImage = (UBYTE*)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    if ((RYImage = (UBYTE*)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for red memory...\r\n");
        while (1)
            ;
    }
    printf("Welcome!\r\n");
    Paint_NewImage(BlackImage, EPD_4IN2BC_WIDTH, EPD_4IN2BC_HEIGHT, 180, WHITE);
    Paint_NewImage(RYImage, EPD_4IN2BC_WIDTH, EPD_4IN2BC_HEIGHT, 180, WHITE);

    mydisp();
    EPD_4IN2BC_Display(BlackImage, RYImage);
    DEV_Delay_ms(2000);
}

void loop() {
    static int count = 0;  // 每小时获取一次天气
    static const char *now_weather;

    if ((WiFi.status() == WL_CONNECTED)) {
        mydisp();

        HTTPClient http_time;
        http_time.begin(url_time);
        http_time.GET();
        String payload_time = http_time.getString();
        deserializeJson(doc_time, payload_time);
        String time1 = doc_time["sysTime1"].as<String>();
        String time2 = doc_time["sysTime2"].as<String>();
        Paint_SelectImage(BlackImage);
        Paint_DrawString_EN(0, 100, time2.c_str(), &Font24, WHITE, BLACK);
        http_time.end();

        DynamicJsonDocument now_doc(512);
        if (count == 0 && getWeatherInfo(now_doc) == 0) {
            std::string weather =
                now_doc["lives"][0]["weather"].as<std::string>();
            now_weather = WEATHER[weather].c_str();

            printf("%s\n", now_weather);
            count = 12;
        }
        Paint_SelectImage(RYImage);
        Paint_DrawString_EN(0, 32, now_weather, &Font16, WHITE, BLACK);
        EPD_4IN2BC_Display(BlackImage, RYImage);
    }
    count--;
    delay(300000);
}