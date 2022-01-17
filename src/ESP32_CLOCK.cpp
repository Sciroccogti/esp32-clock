
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

const char* ntpServer = "cn.ntp.org.cn";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

const char* wday[7] = {"一", "二", "三", "四", "五", "六", "日"};

std::map<std::string, std::string> WEATHER = {
    { "小雨", "WXYU.BMP" }, { "中雨", "WZYU.BMP" }, { "大雨", "WDYU.BMP" },
    { "暴雨", "WWET.BMP" }, { "晴", "WQING.BMP" }, { "多云", "WDYZQ.BMP" },
    { "阴", "WYIN.BMP" }, { "雷阵雨", "WLZYU.BMP" }, { "阵雨", "WYGTQ.BMP" },
    { "霾", "WFOG.BMP" }, { "雾", "WWU.BMP" }, { "雪", "WXUE.BMP" },
    { "雨夹雪", "WYJX.BMP" }, { "冰雹", "WBBAO.BMP" }, { "月亮", "WMOON.BMP" },
    { "深夜", "WSLEEP.BMP" }, { "日落", "SUMSET.BMP" },
    { "日出", "SUNRISE.BMP" }, { "雨", "WZYU.BMP" }
};

// Create a new image cache named IMAGE_BW and fill it with white
UBYTE* BlackImage;

void mydisp()
{
    Paint_SelectImage(BlackImage);
    Paint_Clear(WHITE);
    char str[100];
    Paint_DrawString_EN(0, 0, "SSID:", &Font16, WHITE, BLACK);
    Paint_DrawString_EN(0, 16, "IP:", &Font16, WHITE, BLACK);
    Paint_Clear(WHITE);
    sprintf(str, "%s", ssid);
    Paint_DrawString_EN(66, 0, str, &Font16, WHITE, BLACK);
    sprintf(str, "%s", myIP.toString().c_str());
    Paint_DrawString_EN(66, 16, str, &Font16, WHITE, BLACK);
}

void setup()
{
    wifi_setup();

    DEV_Module_Init();

    printf("e-Paper Init and Clear...\r\n");
    EPD_3IN7_4Gray_Init();
    EPD_3IN7_4Gray_Clear();
    DEV_Delay_ms(500);

    UWORD Imagesize = ((EPD_3IN7_WIDTH % 4 == 0) ? (EPD_3IN7_WIDTH / 4) : (EPD_3IN7_WIDTH / 4 + 1)) * EPD_3IN7_HEIGHT;
    if ((BlackImage = (UBYTE*)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    printf("Welcome!\r\n");
    Paint_NewImage(BlackImage, EPD_3IN7_WIDTH, EPD_3IN7_HEIGHT, 270, WHITE);
    Paint_SetScale(4);
    Paint_Clear(WHITE);
    mydisp();
    EPD_3IN7_4Gray_Display(BlackImage);
    DEV_Delay_ms(2000);

    EPD_3IN7_1Gray_Init(); //init 1 Gray mode
    EPD_3IN7_1Gray_Clear();
    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(WHITE);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop()
{
    static int count = 0;
    static const char* now_weather;
    static char dateStr[11]; // YYYY:mm:dd
    static char timeStr[6]; // hh:mm
    struct tm timeinfo;
    static String time2;
    static bool isTimeOK = false;
    static int curSec = 0;
    static int curDay = 0;

    if ((WiFi.status() == WL_CONNECTED)) {
        // update time
        if (count % 60 == 0 || !isTimeOK || curSec == 0) {
            if (!getLocalTime(&timeinfo)) {
                printf("WARN: Unable to fetch time!\n");
                isTimeOK = false;
            } else {
                curSec = timeinfo.tm_sec;
                isTimeOK = true;
            }
        }

        // twinkle
        if (count % 2) {
            sprintf(timeStr, "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
        } else {
            sprintf(timeStr, "%02d %02d", timeinfo.tm_hour, timeinfo.tm_min);
        }

        Paint_ClearWindows(187, 100, 323, 124, WHITE);
        Paint_DrawString_EN(187, 100, timeStr, &Font24, WHITE, BLACK);

        if (timeinfo.tm_mday != curDay) {
            sprintf(dateStr, "%02d-%02d", timeinfo.tm_mon, timeinfo.tm_mday);
            Paint_ClearWindows(0, 100, 187, 141, WHITE);
            Paint_DrawString_EN(0, 100, dateStr, &Font24, WHITE, BLACK);
            Paint_DrawString_CN(102, 100, wday[timeinfo.tm_wday], &Font24CN, BLACK, WHITE);
            curDay = timeinfo.tm_mday;
        }

        // update weather
        DynamicJsonDocument now_doc(512);
        if (count == 0 && getWeatherInfo(now_doc) == 0) {
            std::string weather = now_doc["lives"][0]["weather"].as<std::string>();
            now_weather = WEATHER[weather].c_str();

            printf("weather: %s\n", now_weather);
            count = 5 * 60;
            Paint_DrawString_EN(0, 32, now_weather, &Font16, WHITE, BLACK);
        }
        EPD_3IN7_1Gray_Display(BlackImage);
    }
    count--;
    curSec = curSec >= 59 ? 0 : curSec + 1;
    delay(1000);
}