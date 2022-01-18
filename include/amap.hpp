#include <HTTPClient.h>

#include <string>

#include "ArduinoJson.h"

/**
 * @brief Get the Weather Info object
 *
 * @param now_doc
 * @param useFore: True to use forecast, False to use base
 * @return int : 0 if ok, -1 if error
 */
int getWeatherInfo(DynamicJsonDocument& now_doc, bool useFore)
{
    static bool isInited = false;
    static String adcode;
    if (!isInited) {
        const char* urlip = "https://restapi.amap.com/v3/"
                            "ip?key=2f84cf79e4e4e7b7b055fdb65bdb7d2c";
        DynamicJsonDocument ip_doc(512);
        HTTPClient ip_http;
        ip_http.begin(urlip);
        if (ip_http.GET() > 0) {
            String amap_payload = ip_http.getString();
            deserializeJson(ip_doc, amap_payload);
            adcode = ip_doc["adcode"].as<String>();
            printf("adcode: %s\n", adcode.c_str());
            isInited = true;
        } else {
            adcode = "320211";
        }
    }

    char urlnow[256];
    if (useFore) {
        sprintf(urlnow,
            "https://restapi.amap.com/v3/weather/"
            "weatherInfo?city=%s&key=2f84cf79e4e4e7b7b055fdb65bdb7d2c&"
            "extensions="
            "all",
            adcode.c_str());
    } else {
        sprintf(urlnow,
            "https://restapi.amap.com/v3/weather/"
            "weatherInfo?city=%s&key=2f84cf79e4e4e7b7b055fdb65bdb7d2c&"
            "extensions="
            "base",
            adcode.c_str());
    }

    HTTPClient now_http;
    now_http.begin(urlnow);
    if (now_http.GET() > 0) {
        String amap_payload = now_http.getString();
        deserializeJson(now_doc, amap_payload);
        return 0;
    } else {
        return -1;
    }
}