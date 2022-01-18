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
#include <map>

DynamicJsonDocument doc_time(512);

const char* ntpServer = "cn.ntp.org.cn";
const long gmtOffset_sec = 8 * 3600;
const int daylightOffset_sec = 0;

const char* wday[7] = { "一", "二", "三", "四", "五", "六", "日" };

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

    // 输出高电平，点亮LED
    gpio_set_level(GPIO_NUM_2, 1);

    timer_config_t config = {
        alarm_en : TIMER_ALARM_EN, //到达计数值启动报警(计数值溢出,进入中断)
        counter_en : TIMER_PAUSE, //调用timer_init函数以后不启动计数,调用timer_start时才开始计数
        intr_type : TIMER_INTR_LEVEL, /*!< Interrupt mode */
        counter_dir : TIMER_COUNT_UP, //计数方式是向上计数
        auto_reload : true, //自动重新装载预装值
        divider : 80, //分频系数[2-65535]，频率为 80 MHz，故设为 80 时每 1us 计数一次
    };
    timer_init(TIMER_GROUP_0, TIMER_0, &config);
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0); // 初始计数值
    timer_set_alarm_value(TIMER_GROUP_0, TIMER_0, 1000000); // 计数到 100000
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer00_isr, (void*)TIMER_0, ESP_INTR_FLAG_IRAM, NULL);
    timer_start(TIMER_GROUP_0, TIMER_0);
}

void setup()
{
    wifi_setup();

    DEV_Module_Init();

    ESP_LOGI("e-Paper Init and Clear...\r\n");
    EPD_3IN7_4Gray_Init();
    EPD_3IN7_4Gray_Clear();
    DEV_Delay_ms(500);

    UWORD Imagesize = ((EPD_3IN7_WIDTH % 4 == 0) ? (EPD_3IN7_WIDTH / 4) : (EPD_3IN7_WIDTH / 4 + 1)) * EPD_3IN7_HEIGHT;
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

    EPD_3IN7_1Gray_Init(); //init 1 Gray mode
    EPD_3IN7_1Gray_Clear();
    Paint_SelectImage(BlackImage);
    Paint_SetScale(2);
    Paint_Clear(WHITE);

    Paint_DrawImage(gImage_wdyu, 10, 200, 80, 80);
    EPD_3IN7_1Gray_Display(BlackImage);

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    gpio_init();
}

void loop()
{
    static const char* now_weather;
    static char tmpStr[11]; // YYYY:mm:dd
    struct tm remoteTm;
    static bool isTimeOK = false;
    static int curDay = 0;
    static bool tweak = false;

    tm* pLocalTm = localtime(&t);
    printf("%ld %02d:%02d\n", t, pLocalTm->tm_min, pLocalTm->tm_sec);

    // twinkle every sec
    if (tweak) {
        sprintf(tmpStr, "%02d:%02d", pLocalTm->tm_hour, pLocalTm->tm_min);
    } else {
        sprintf(tmpStr, "%02d %02d", pLocalTm->tm_hour, pLocalTm->tm_min);
    }
    tweak = !tweak;
    Paint_ClearWindows(187, 100, 395, 177, WHITE);
    Paint_DrawString_MY(187, 100, tmpStr, &FourFun72, BLACK, WHITE);

    // update Day
    if (pLocalTm->tm_mday != curDay) {
        sprintf(tmpStr, "%02d-%02d", pLocalTm->tm_mon + 1, pLocalTm->tm_mday);
        Paint_ClearWindows(0, 100, 187, 141, WHITE);
        Paint_DrawString_EN(0, 100, tmpStr, &Font24, WHITE, BLACK);
        Paint_DrawString_CN(102, 100, wday[pLocalTm->tm_wday], &Font24CN, BLACK, WHITE);
        Paint_DrawImage(gImage_wdyu, 10, 200, 80, 80);
        curDay = pLocalTm->tm_mday;
    }

    if ((WiFi.status() == WL_CONNECTED)) {
        // update weather every hour
        DynamicJsonDocument now_doc(512);
        if ((pLocalTm->tm_min == 0 || now_weather == NULL) && getWeatherInfo(now_doc) == 0) {
            std::string weather = now_doc["lives"][0]["weather"].as<std::string>();
            now_weather = WEATHER[weather].c_str();

            ESP_LOGI("weather: %s\n", now_weather);
            Paint_DrawString_EN(0, 32, now_weather, &Font16, WHITE, BLACK);
        }

        // fetch time every hour
        if (!isTimeOK || pLocalTm->tm_min == 0) {
            isTimeOK = false;
            if (!getLocalTime(&remoteTm)) {
                ESP_LOGI("WARN: Unable to fetch time!\n");
                isTimeOK = false;
            } else {
                pLocalTm = &remoteTm;
                t = mktime(pLocalTm);
                isTimeOK = true;
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
    TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN; //再次使能alarm
    t++;
    static int level = 0;
    gpio_set_level(GPIO_NUM_2, level);
    level = 1 - level;
}