/**
 * @file my_Paint.cpp
 * @author Sciroccogti (scirocco_gti@yeah.net)
 * @brief
 * @date 2022-01-18 14:27:59
 * @modified: 2022-01-19 00:23:04
 */
#include "GUI_Paint.h"
#include "myfont.h"
/******************************************************************************
function: Display the string
parameter:
    Xstart  ：X coordinate
    Ystart  ：Y coordinate
    pString ：The first address of the Chinese string and English
              string to be displayed
    Font    ：A structure pointer that displays a character size
    Color_Foreground : Select the foreground color
    Color_Background : Select the background color
******************************************************************************/
void Paint_DrawString_MY(UWORD Xstart, UWORD Ystart, const char* pString,
    myFONT* font, UWORD Color_Foreground, UWORD Color_Background)
{
    const char* p_text = pString;
    int x = Xstart, y = Ystart;
    int i, j, Num;

    /* Send the string character by character on EPD */
    while (*p_text != 0) {
        if (*p_text == ':' or *p_text == ' ') {
            for (Num = 0; Num < font->size; Num++) {
                if (*p_text == font->table[Num].index[0]) {
                    const char* ptr = &font->table[Num].matrix[0];

                    for (j = 0; j < font->Height; j++) {
                        for (i = 0; i < font->Symbol_Width; i++) {
                            // this process is to speed up the scan
                            if (FONT_BACKGROUND == Color_Background) {
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Foreground);
                                }
                            } else {
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Foreground);
                                } else {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Background);
                                }
                            }
                            if (i % 8 == 7) {
                                ptr++;
                            }
                        }
                        if (font->Symbol_Width % 8 != 0) {
                            ptr++;
                        }
                    }
                    break;
                }
            }
            /* Point on the next character */
            p_text += 1;
            /* Decrement the column position by 16 */
            x += font->Symbol_Width;
        } else if (*p_text <= 0x7F) { // ASCII < 126
            for (Num = 0; Num < font->size; Num++) {
                if (*p_text == font->table[Num].index[0]) {
                    const char* ptr = &font->table[Num].matrix[0];

                    for (j = 0; j < font->Height; j++) {
                        for (i = 0; i < font->Width; i++) {
                            // this process is to speed up the scan
                            if (FONT_BACKGROUND == Color_Background) {
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Foreground);
                                }
                            } else {
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Foreground);
                                } else {
                                    Paint_SetPixel(
                                        x + i, y + j, Color_Background);
                                }
                            }
                            if (i % 8 == 7) {
                                ptr++;
                            }
                        }
                        if (font->Width % 8 != 0) {
                            ptr++;
                        }
                    }
                    break;
                }
            }
            /* Point on the next character */
            p_text += 1;
            /* Decrement the column position by 16 */
            x += font->ASCII_Width;
        } /* else { // Chinese
            for (Num = 0; Num < font->size; Num++) {
                if ((*p_text == font->table[Num].index[0]) && (*(p_text + 1) ==
        font->table[Num].index[1]) && (*(p_text + 2) ==
        font->table[Num].index[2])) { const char* ptr =
        &font->table[Num].matrix[0];

                    for (j = 0; j < font->Height; j++) {
                        for (i = 0; i < font->Width; i++) {
                            if (FONT_BACKGROUND == Color_Background) { // this
        process is to speed
                                // up the scan
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(x + i, y + j,
                                        Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j,
                                    // Color_Foreground, DOT_PIXEL_DFT,
                                    // DOT_STYLE_DFT);
                                }
                            } else {
                                if (*ptr & (0x80 >> (i % 8))) {
                                    Paint_SetPixel(x + i, y + j,
                                        Color_Foreground);
                                    // Paint_DrawPoint(x + i, y + j,
                                    // Color_Foreground, DOT_PIXEL_DFT,
                                    // DOT_STYLE_DFT);
                                } else {
                                    Paint_SetPixel(x + i, y + j,
                                        Color_Background);
                                    // Paint_DrawPoint(x + i, y + j,
                                    // Color_Background, DOT_PIXEL_DFT,
                                    // DOT_STYLE_DFT);
                                }
                            }
                            if (i % 8 == 7) {
                                ptr++;
                            }
                        }
                        if (font->Width % 8 != 0) {
                            ptr++;
                        }
                    }
                    break;
                }
            }
            // Point on the next character
            p_text += 3;
            // Decrement the column position by 16
            x += font->Width;
        }*/
    }
}

/******************************************************************************
function:	Display image
parameter:
    image            ：Image start address
    xStart           : X starting coordinates
    yStart           : Y starting coordinates
    xEnd             ：Image width
    yEnd             : Image height
******************************************************************************/

/**
 * @brief Display in smaller scale
 *
 * @param image_buffer ：Image start address
 * @param xStart : X starting coordinates
 * @param yStart : Y starting coordinates
 * @param W_Image ：Original image width
 * @param H_Image : Original image height
 * @param scale : picture will be W_Image * scale / 8, so scale should be 1 ~ 8
 */
void Paint_DrawImage_Scale(const unsigned char* image_buffer, UWORD xStart,
    UWORD yStart, UWORD W_Image, UWORD H_Image, const int scale)
{
    assert(scale == 6);
    UWORD x, y;
    UWORD w_byte = (W_Image % 8) ? (W_Image / 8) + 1 : W_Image / 8;
    UDOUBLE Addr = 0;
    UDOUBLE pAddr = 0;
    switch (Paint.Rotate) {
    case 0:
        for (y = 0; y < H_Image; y++) {
            for (x = 0; x < w_byte; x++) { // 8 pixel =  1 byte
                Addr = x + y * w_byte; // 一个字符代表 x 方向 8 个像素点
                int realI = 0;
                for (int i = 0; i < 8; i++) {
                    if (i == 2 || i == 5) {
                        continue;
                    } else {
                        realI++;
                    }

                    int X = x * scale + realI + xStart;
                    int Y = yStart + y * scale / 8;
                    pAddr = X / 8 + Y * Paint.WidthByte;
                    if ((image_buffer[Addr] << i) & 0x80) {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] | (0x80 >> (X % 8));
                    } else {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] & ~(0x80 >> (X % 8));
                    }
                }
            }
        }
        break;
    case 90:
        for (y = 0; y < H_Image; y++) {
            for (x = 0; x < w_byte; x++) { // 8 pixel =  1 byte
                Addr = x + y * w_byte;
                for (int i = 0; i < 8; i++) {
                    int X = Paint.WidthMemory - y - yStart - 1;
                    int Y = x * 8 + i + xStart;
                    pAddr = X / 8 + Y * Paint.WidthByte;
                    if ((image_buffer[Addr] << i) & 0x80) {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] | (0x80 >> (X % 8));
                    } else {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] & ~(0x80 >> (X % 8));
                    }
                }
            }
        }
        break;
    case 180:
        for (y = 0; y < H_Image; y++) {
            for (x = 0; x < w_byte; x++) { // 8 pixel =  1 byte
                Addr = x + y * w_byte;
                for (int i = 0; i < 8; i++) {
                    int X = Paint.HeightMemory - (x * 8 + i) - xStart - 1;
                    int Y = Paint.WidthMemory - y - yStart - 1;
                    pAddr = X / 8 + Y * Paint.WidthByte;
                    if ((image_buffer[Addr] << i) & 0x80) {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] | (0x80 >> (X % 8));
                    } else {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] & ~(0x80 >> (X % 8));
                    }
                }
            }
        }
        break;
    case 270:
        for (y = 0; y < H_Image; y++) {
            for (x = 0; x < w_byte; x++) { // 8 pixel =  1 byte
                Addr = x + y * w_byte;
                int realI = 0;
                for (int i = 0; i < 8; i++) {
                    if (i == 2 || i == 5) {
                        continue;
                    } else {
                        realI++;
                    }
                    int X = yStart + y * scale / 8;
                    int Y = Paint.HeightMemory - (x * scale + realI) - xStart - 1;
                    pAddr = X / 8 + Y * Paint.WidthByte;
                    if ((image_buffer[Addr] << i) & 0x80) {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] | (0x80 >> (X % 8));
                    } else {
                        Paint.Image[pAddr]
                            = Paint.Image[pAddr] & ~(0x80 >> (X % 8));
                    }
                }
            }
        }
        break;
    default:
        return;
    }
}
