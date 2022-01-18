/**
 * @file myfont.h
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief 
 * @date 2022-01-18 13:32:40
 * @modified: 2022-01-19 00:03:00
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MYFONT_H
#define __MYFONT_H


/* Max size of bitmap will based on a font72 (48x77) */
#define MAX_HEIGHT_MYFONT         77
#define MAX_WIDTH_MYFONT          48
#define OFFSET_MYBITMAP           54

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include "DEV_Config.h"

//GB2312
typedef struct                                          // 汉字字模数据结构
{
  unsigned char index[3];                               // 汉字内码索引
  const char matrix[MAX_HEIGHT_MYFONT*MAX_WIDTH_MYFONT/8];  // 点阵码数据
}MY_CN;

typedef struct
{    
  const MY_CN *table;
  uint16_t size;
  uint16_t Symbol_Width;
  uint16_t ASCII_Width;
  uint16_t Width;
  uint16_t Height;
  
}myFONT;

// extern sFONT Font8;

extern myFONT FourFun72;
// extern const unsigned char Font16_Table[];

#ifdef __cplusplus
}
#endif

void Paint_DrawString_MY(UWORD Xstart, UWORD Ystart, const char* pString,
                         myFONT* font, UWORD Color_Foreground,
                         UWORD Color_Background);
void Paint_DrawImage_Scale(const unsigned char* image_buffer, UWORD xStart,
    UWORD yStart, UWORD W_Image, UWORD H_Image, const int scale);
#endif /* __MYFONT_H */
 