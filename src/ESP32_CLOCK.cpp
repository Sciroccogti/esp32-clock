#include "DEV_Config.h"
#include "EPD.h"
#include "GUI_Paint.h"
#include "amap.hpp"
#include "driver/gpio.h"
#include "driver/timer.h"
#include "fonts/myfont.h"
#include "weather/weather.h"
#include "wifiConnect.h"
#include <ctime>
#include <esp_log.h>
#include <lwip/apps/sntp.h>
#include <map>
#include <stdlib.h>

DynamicJsonDocument doc_time(512);

const char* ntpServer = "cn.ntp.org.cn";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

const char* wday[7] = { "一", "二", "三", "四", "五", "六", "日" };

std::map<std::string, unsigned char*> WEATHER = { { "小雨", img_WXYU },
    { "中雨", img_WZYU }, { "大雨", img_WDYU }, { "暴雨", img_WWET },
    { "晴", img_WQING }, { "多云", img_WDYZQ }, { "阴", img_WYIN },
    { "雷阵雨", img_WLZYU }, { "阵雨", img_WYGTQ }, { "霾", img_WFOG },
    { "雾", img_WWU }, { "雪", img_WXUE }, { "雨夹雪", img_WYJX },
    { "冰雹", img_WBBAO }, { "月亮", img_WMOON }, { "深夜", img_SLEEP },
    { "日落", img_SUNSET }, { "日出", img_SUNRISE }, { "雨", img_WZYU } };

// Create a new image cache named IMAGE_BW and fill it with white
UBYTE* BlackImage;

time_t t;

void IRAM_ATTR timer00_isr(void* para);

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

void gpio_init(void)
{
    // 定义一个gpio配置结构体
    gpio_config_t gpio_config_structure;

    // 初始化gpio配置结构体
    gpio_config_structure.pin_bit_mask = (1ULL << 2); // 选择gpio2
    gpio_config_structure.mode = GPIO_MODE_OUTPUT; // 输出模式
    gpio_config_structure.pull_up_en = GPIO_PULLUP_DISABLE; // 不上拉
    gpio_config_structure.pull_down_en = GPIO_PULLDOWN_DISABLE; // 不下拉
    gpio_config_structure.intr_type = GPIO_INTR_DISABLE; // 禁止中断

    // 根据设定参数初始化并使能
    gpio_config(&gpio_config_structure);

    // 初始化gpio配置结构体
    // gpio_config_structure.pin_bit_mask = (1ULL << GPIO_KEY_NUM); // 选择gpio0
    // gpio_config_structure.mode = GPIO_MODE_INPUT; // 输入模式
    // gpio_config_structure.pull_up_en = 0; // 不上拉
    // gpio_config_structure.pull_down_en = 0; // 不下拉
    // gpio_config_structure.intr_type = GPIO_PIN_INTR_DISABLE; // 禁止中断

    // 根据设定参数初始化并使能
    // gpio_config(&gpio_config_structure);

    // timer_config_t config = {
    //     alarm_en : TIMER_ALARM_EN, //到达计数值启动报警(计数值溢出,进入中断)
    //     counter_en :
    //         TIMER_PAUSE,
    //         //调用timer_init函数以后不启动计数,调用timer_start时才开始计数
    //     intr_type : TIMER_INTR_LEVEL, /*!< Interrupt mode */
    //     counter_dir : TIMER_COUNT_UP, //计数方式是向上计数
    //     auto_reload : true, //自动重新装载预装值
    //     divider :
    //         80, //分频系数[2-65535]，频率为 80 MHz，故设为 80 时每 1us
    //         计数一次
    // };
    // timer_init(TIMER_GROUP_0, TIMER_0, &config);
    // timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0); // 初始计数值
    // timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000); // 计数到 100000
    // timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    // timer_isr_register(TIMER_GROUP_0, TIMER_0, timer00_isr, (void*)TIMER_0,
    //     ESP_INTR_FLAG_IRAM, NULL);
    // timer_start(TIMER_GROUP_0, TIMER_0);
}

void setup()
{
    wifi_setup();

    DEV_Module_Init();

    ESP_LOGI("e-Paper Init and Clear...\r\n");
    EPD_3IN7_4Gray_Init();
    EPD_3IN7_4Gray_Clear();
    DEV_Delay_ms(500);

    UWORD Imagesize = ((EPD_3IN7_WIDTH % 4 == 0) ? (EPD_3IN7_WIDTH / 4)
                                                 : (EPD_3IN7_WIDTH / 4 + 1))
        * EPD_3IN7_HEIGHT;
    if ((BlackImage = (UBYTE*)malloc(Imagesize)) == NULL) {
        ESP_LOGI("Failed to apply for black memory...\r\n");
        while (1)
            ;
    }
    ESP_LOGI("Welcome!\r\n");
    Paint_NewImage(BlackImage, EPD_3IN7_WIDTH, EPD_3IN7_HEIGHT, 270, WHITE);
    Paint_SetScale(4);
    Paint_Clear(WHITE);
    mydisp();
    EPD_3IN7_4Gray_Display(BlackImage);

    DEV_Delay_ms(300);

    EPD_3IN7_1Gray_Init(); // init 1 Gray mode
    EPD_3IN7_1Gray_Clear();
    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(WHITE);

    // configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    gpio_init();

    // sntp， 后台自动更新时间
    ESP_LOGI("sntp", "Initializing SNTP");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "ntp1.aliyun.com");
    sntp_setservername(1, "210.72.145.44"); // 国家授时中心服务器 IP 地址
    sntp_setservername(2, "1.cn.pool.ntp.org");
    sntp_init();
    setenv("TZ", "CST-8", 1);
    tzset();
}

void loop()
{
    static const unsigned char* now_weather;
    static char tmpStr[11]; // YYYY:mm:dd
    static bool isTimeOK = false;
    static int curDay = 0;
    static bool tweak = false;

    tm* pLocalTm = localtime(&t);
    time(&t);
    localtime_r(&t, pLocalTm);
    printf("%ld %02d:%02d\n", t, pLocalTm->tm_min, pLocalTm->tm_sec);

    // twinkle every sec
    if (tweak) {
        sprintf(tmpStr, "%02d:%02d", pLocalTm->tm_hour, pLocalTm->tm_min);
    } else {
        sprintf(tmpStr, "%02d %02d", pLocalTm->tm_hour, pLocalTm->tm_min);
    }
    tweak = !tweak;
    Paint_ClearWindows(0, 74, 320, 200, WHITE);
    Paint_DrawString_MY(0, 74, tmpStr, &FourFun118, BLACK, WHITE);

    // update Day
    if (pLocalTm->tm_mday != curDay) {
        sprintf(tmpStr, "%02d-%02d", pLocalTm->tm_mon + 1, pLocalTm->tm_mday);
        Paint_ClearWindows(320, 100, 454, 141, WHITE);
        Paint_DrawString_EN(320, 100, tmpStr, &Font24, WHITE, BLACK);
        Paint_DrawString_CN(
            405, 100, wday[pLocalTm->tm_wday], &Font24CN, BLACK, WHITE);
        curDay = pLocalTm->tm_mday;
    }

    if ((WiFi.status() == WL_CONNECTED)) {
        // update weather every hour
        DynamicJsonDocument now_doc(512);
        if ((pLocalTm->tm_min == 0 || now_weather == NULL)
            && getWeatherInfo(now_doc, false) == 0 && isTimeOK) {
            std::string weather
                = now_doc["lives"][0]["weather"].as<std::string>();
            now_weather = WEATHER[weather];

            printf("weather: %s\n", weather.c_str());
            Paint_ClearWindows(0, 200, 200, 280, WHITE);
            Paint_DrawImage(now_weather, 0, 200, 80, 80);

            if (getWeatherInfo(now_doc, true) == 0) {
                std::string cast1, cast2, name1, name2;
                if (abs(pLocalTm->tm_hour - 12) < 6) { // 白天
                    cast1 = now_doc["forecasts"][0]["casts"][0]["nightweather"]
                                .as<std::string>();
                    name1 = "今晚";
                    cast2 = now_doc["forecasts"][0]["casts"][1]["dayweather"]
                                .as<std::string>();
                    name2 = "明天";
                } else if (pLocalTm->tm_hour <= 6) {
                    cast1 = now_doc["forecasts"][0]["casts"][0]["dayweather"]
                                .as<std::string>();
                    name1 = "今早";
                    cast2 = now_doc["forecasts"][0]["casts"][0]["nightweather"]
                                .as<std::string>();
                    name2 = "今晚";
                } else { // 晚上
                    cast1 = now_doc["forecasts"][0]["casts"][1]["dayweather"]
                                .as<std::string>();
                    name1 = "明天";
                    cast2 = now_doc["forecasts"][0]["casts"][2]["dayweather"]
                                .as<std::string>();
                    name2 = "后天";
                }

                printf("cast1: %s\n", cast1.c_str());
                printf("cast2: %s\n", cast2.c_str());
                Paint_DrawString_CN(
                    86, 259, name1.c_str(), &Font16CN, BLACK, WHITE);
                Paint_DrawString_CN(
                    146, 259, name2.c_str(), &Font16CN, BLACK, WHITE);
                if (cast1 != "null") {
                    Paint_DrawImage_Scale(
                        WEATHER[cast1.c_str()], 80, 200, 80, 80, 6);
                }
                if (cast2 != "null") {
                    Paint_DrawImage_Scale(
                        WEATHER[cast2.c_str()], 140, 200, 80, 80, 6);
                }
            }
        }

        // fetch time every hour
        if (!isTimeOK || (pLocalTm->tm_min == 0 && pLocalTm->tm_sec <= 2)) {
            sntp_init();
            isTimeOK = false;
            while (pLocalTm->tm_year < (2019 - 1900)) {
                ESP_LOGD(
                    TAG, "Waiting for system time to be set... (%d)", ++retry);
                vTaskDelay(100 / portTICK_PERIOD_MS);
                time(&t);
                localtime_r(&t, pLocalTm);
            }
            sntp_stop();
            isTimeOK = true;

            // clear every 6 hour
            if (pLocalTm->tm_hour % 6 == 0) {
                EPD_3IN7_1Gray_Clear();
                delay(300);
            }
        }
    }

    EPD_3IN7_1Gray_Display(BlackImage);
    delay(1000);
}

// 精确 1s 定时器
void IRAM_ATTR timer00_isr(void* para)
{
    int timer_idx = (int)para;
    //获取定时器中断状态
    uint32_t intr_status = TIMERG0.int_st_timers.val;

    if ((intr_status & BIT(timer_idx)) && timer_idx == TIMER_0) {
        TIMERG0.int_clr_timers.t0 = 1; //清除中断标志
    }
    TIMERG0.hw_timer[timer_idx].config.alarm_en
        = TIMER_ALARM_EN; //再次使能alarm
    t++;
    static int level = 0;
    gpio_set_level(GPIO_NUM_2, level);
    level = 1 - level;
}