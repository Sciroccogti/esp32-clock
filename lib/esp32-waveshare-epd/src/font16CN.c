/*
 * Filename: /home/zyf/github/esp32-clock/lib/esp32-waveshare-epd/src/font16CN.c
 * Path: /home/zyf/github/esp32-clock/lib/esp32-waveshare-epd/src
 * Created Date: Tuesday, January 18th 2022, 11:24:25 pm
 * Author: Sciroccogti
 * 
 * Copyright (c) 2022 Your Company
 */
#include "fonts.h"

const CH_CN Font16CN_Table[] =
{
/*--  文字:  今  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"今",
0x00,0x00,0x00,0x00,0x70,0x00,0x00,0xF0,0x00,0x00,0xF0,0x00,0x01,0xD8,0x00,0x03,
0x9C,0x00,0x03,0x8E,0x00,0x07,0xC7,0x00,0x0E,0xE3,0xC0,0x1C,0x71,0xF8,0x70,0x30,
0xF0,0xE0,0x01,0x00,0x1F,0xFF,0x80,0x08,0x03,0x80,0x00,0x07,0x00,0x00,0x0E,0x00,
0x00,0x1C,0x00,0x00,0x1C,0x00,0x00,0x38,0x00,0x00,0x70,0x00,0x00,0x00,0x00,},

/*--  文字:  天  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"天",
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x1F,0xFF,0xC0,0x08,0x60,0x00,0x00,
0x60,0x00,0x00,0x60,0x00,0x00,0x60,0x00,0x00,0x60,0xE0,0x7F,0xFF,0xF0,0x20,0xF0,
0x00,0x00,0xF8,0x00,0x00,0xD8,0x00,0x01,0xDC,0x00,0x01,0x8C,0x00,0x03,0x8E,0x00,
0x07,0x07,0x00,0x0E,0x03,0xC0,0x3C,0x01,0xF8,0x70,0x00,0xF0,0x00,0x00,0x00,},

/*--  文字:  明  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"明",
0x00,0x00,0x00,0x00,0x3F,0xE0,0x3F,0xF8,0xE0,0x39,0xB8,0xE0,0x39,0xB8,0xE0,0x39,
0xB8,0xE0,0x39,0xBF,0xE0,0x3F,0xB8,0xE0,0x39,0xB8,0xE0,0x39,0xB8,0xE0,0x39,0xB8,
0xE0,0x39,0xBF,0xE0,0x3F,0xB8,0xE0,0x39,0xB8,0xE0,0x39,0xB0,0xE0,0x00,0x70,0xE0,
0x00,0xE0,0xE0,0x00,0xC7,0xE0,0x03,0x83,0xE0,0x07,0x01,0xC0,0x00,0x00,0x00,},

/*--  文字:  晚  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"晚",
0x00,0x00,0x00,0x00,0x1C,0x00,0x02,0x38,0x00,0x7F,0x3F,0x80,0x77,0x73,0x80,0x77,
0x67,0x00,0x77,0xC6,0x00,0x77,0xFF,0xE0,0x7F,0xEE,0xE0,0x77,0xEE,0xE0,0x77,0xEC,
0xE0,0x77,0xFF,0xE0,0x77,0xEE,0xE0,0x77,0x1F,0x00,0x7F,0x1E,0x00,0x77,0x3E,0x00,
0x76,0x36,0x30,0x60,0xF6,0x30,0x01,0xC6,0x70,0x07,0x87,0xF0,0x00,0x00,0x00,},

/*--  文字:  早  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"早",
0x00,0x00,0x00,0x0F,0xFF,0x80,0x0E,0x03,0x80,0x0E,0x03,0x80,0x0E,0x03,0x80,0x0F,
0xFF,0x80,0x0E,0x03,0x80,0x0E,0x03,0x80,0x0E,0x03,0x80,0x0F,0xFF,0x80,0x0E,0x73,
0x80,0x00,0x70,0x00,0x00,0x70,0x60,0x7F,0xFF,0xF0,0x60,0x70,0x00,0x00,0x70,0x00,
0x00,0x70,0x00,0x00,0x70,0x00,0x00,0x70,0x00,0x00,0x70,0x00,0x00,0x00,0x00,},

/*--  文字:  后  --*/
/*--  宋体16;  此字体下对应的点阵为：宽x高=21x21   --*/
/*--  宽度不是8的倍数，现调整为：宽度x高度=24x21  --*/
{"后",
0x00,0x00,0x00,0x00,0x01,0xC0,0x08,0x7F,0xE0,0x0F,0xFC,0x00,0x0C,0x00,0x00,0x0C,
0x00,0x00,0x0C,0x00,0x60,0x0F,0xFF,0xF0,0x0C,0x00,0x00,0x0C,0x00,0x00,0x0C,0x00,
0x00,0x1D,0xFF,0xC0,0x1D,0x81,0xC0,0x1D,0x81,0xC0,0x1D,0x81,0xC0,0x19,0x81,0xC0,
0x39,0x81,0xC0,0x31,0xFF,0xC0,0x61,0x81,0xC0,0xE1,0x81,0x80,0x00,0x00,0x00,},
};



cFONT Font16CN = {
    Font16CN_Table,
    sizeof(Font16CN_Table)/sizeof(CH_CN),  /*size of table*/
    21, /* ASCII Width */
    24, /* Width */
    21, /* Height */
};