/*
 LPH8731_3C_MC60.cpp

 Created by BAKS
 Date: November 2019.

 PERMISSION TO DISTRIBUTE

 Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 and associated documentation files (the "Software"), to deal in the Software without restriction,
 including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so,
 subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.


 LIMITATION OF LIABILITY

 The software is provided "as is", without warranty of any kind, express or implied,
 including but not limited to the warranties of merchantability, fitness for a particular
 purpose and noninfringement. In no event shall the authors or copyright holders be liable
 for any claim, damages or other liability, whether in an action of contract,
 tort or otherwise, arising from, out of or in connection with the software
 or the use or other dealings in the software.


 * LCD pin   -->   ESP8266 pin
 *   1 (/CS)         D8
 *   2 (/RST)        D2
 *   3 (RS)          D3
 *   4 (Sclk)        D5 (SPI SCK)
 *   5 (Sdata)       D7 (SPI MOSI)
 *   6 (+3.3v)       3.3v
 *   7 (GND)         GND
 *   8 (LED1)        3.3v
 *   9 (GND)         GND
 *  10 (LED2)        3.3v
 
*/


#include "Arduino.h"
#include "LPH8731_3C_MC60.h"
#include "font.h"
#include <SPI.h>


//*************************************************************
//COMMAND/DATA
#define CMD   0
#define DAT   1

//MACROS
#define LCD_RS_ON       digitalWrite(LCD_RS,HIGH)      // Put LCD in command mode
#define LCD_RS_OFF      digitalWrite(LCD_RS,LOW)   // Put LCD in data mode
#define LCD_CS_OFF      digitalWrite(LCD_CS,HIGH)      // Disable LCD
#define LCD_CS_ON       digitalWrite(LCD_CS,LOW)   // Enable LCD
#define LCD_RESET_ON    digitalWrite(LCD_RESET,LOW)   // Put LCD in reset mode
#define LCD_RESET_OFF   digitalWrite(LCD_RESET,HIGH)   // Put LCD in normal mode
#define GET_RS          digitalRead(LCD_RS)

const lcd_packet initData[80] = {
  {CMD, 0xb9}, // Refresh set
  {DAT, 0x00},
  {CMD, 0xb6}, // Display control
  {DAT, 0x80}, //
  {DAT, 0x04}, //
  {DAT, 0x8a}, //
  {DAT, 0x54}, //
  {DAT, 0x45}, //
  {DAT, 0x52}, //
  {DAT, 0x43}, //
  {CMD, 0xb3}, // Gray scale position set 0
  {DAT, 0x02}, //
  {DAT, 0x0a}, //
  {DAT, 0x15}, //
  {DAT, 0x1f}, //
  {DAT, 0x28}, //
  {DAT, 0x30}, //
  {DAT, 0x37}, //
  {DAT, 0x3f}, //
  {DAT, 0x47}, //
  {DAT, 0x4c}, //
  {DAT, 0x54}, //
  {DAT, 0x65}, //
  {DAT, 0x75}, //
  {DAT, 0x80}, //
  {DAT, 0x85}, //
  {CMD, 0xb5}, // Gamma curve
  {DAT, 0x01}, //
  {CMD, 0xb7}, // Temperature gradient
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {DAT, 0x00}, //
  {CMD, 0xbd}, // Common driver output select
  {DAT, 0x00}, //
  {CMD, 0x36}, // Memory access control
  {DAT, 0x48}, //
  {CMD, 0x2d}, // Colour set
  {DAT, 0x00}, //
  {DAT, 0x03}, //
  {DAT, 0x05}, //
  {DAT, 0x07}, //
  {DAT, 0x09}, //
  {DAT, 0x0b}, //
  {DAT, 0x0d}, //
  {DAT, 0x0f}, //
  {DAT, 0x00}, //
  {DAT, 0x03}, //
  {DAT, 0x05}, //
  {DAT, 0x07}, //
  {DAT, 0x09}, //
  {DAT, 0x0b}, //
  {DAT, 0x0d}, //
  {DAT, 0x0f}, //
  {DAT, 0x00}, //
  {DAT, 0x05}, //
  {DAT, 0x0b}, //
  {DAT, 0x0f}, //
  {CMD, 0xba}, // Voltage control
  {DAT, 0x2f}, //
  {DAT, 0x03}, //
  {CMD, 0x25}, // Write contrast
  {DAT, 0x61}, //
  {CMD, 0xbe}, // Power control
  {DAT, 0x58}, // 0x58 before
  {CMD, 0x3a}, // interface pixel format
#ifdef _8_BIT_COLOR
  {DAT, 0x02}, // 0x03 for 12bit 0x02 for 8-bit
#else
  {DAT, 0x03},
#endif
  {CMD, 0x03}, // Booster voltage ON
  {CMD, 0x11}, // sleep out
  {CMD, 0x29} // Display ON
};


LPH8731_3C_MC60::LPH8731_3C_MC60(byte pinCs, byte pinReset, byte pinRs) {
  this->LCD_CS    = pinCs;
  this->LCD_RESET = pinReset;
  this->LCD_RS    = pinRs;
}


//===============================================================
//                           RESET LCD
//===============================================================
void LPH8731_3C_MC60::reset (void)
{
  LCD_CS_ON;
  LCD_RESET_ON;
  LCD_RESET_OFF;
  LCD_RS_OFF;
} // END LPH8731_3C_MC60::reset


//===============================================================
//                   LCD INICIALIZATION
//===============================================================
void LPH8731_3C_MC60::begin (void)
{
  SPI.begin();
  SPI.setFrequency(35000000);
  pinMode(LCD_CS, OUTPUT);
  pinMode(LCD_RESET, OUTPUT);
  pinMode(LCD_RS, OUTPUT);
  reset();
  LCD_CS_ON;
  lcdSend({CMD, 0x01}); // reset sw
  delay(50);
  lcdSend({CMD, 0xc6}); // initial escape
  delay(40);
  for (int i = 0; i < 80; i ++) {
    lcdSend(initData[i]);
  }
  delay(1);
  LCD_CS_OFF;
  delay(1);
} // END LPH8731_3C_MC60::begin


//*******************************************************************
//
//*******************************************************************
void LPH8731_3C_MC60::lcdSend (lcd_packet pkt){
  if ((GET_RS != pkt.RS) || (GET_RS==LOW && pkt.RS==LOW))
  {
    LCD_CS_OFF;
    digitalWrite(LCD_RS, pkt.RS);
  }
  LCD_CS_ON;
  SPI.transfer(pkt.data);
}

void LPH8731_3C_MC60::lcdSendD(unsigned char data ){
  if (GET_RS != DAT)
  {
    LCD_CS_OFF;
    digitalWrite(LCD_RS, DAT);
  }
  LCD_CS_ON;
  SPI.transfer(data);
} // END LPH8731_3C_MC60::Send_to_lcd


void LPH8731_3C_MC60::setArea (char x1, char x2, char y1, char y2)
{
  lcdSend({CMD, 0x2A});
  lcdSendD(x1);
  lcdSendD(x2);
  lcdSend({CMD,0x2B}); 
  lcdSendD(y1 + 1); 
  lcdSendD(y2 + 1);
  lcdSend( {CMD, 0x2C} );
  delayMicroseconds(1);
  LCD_CS_OFF;
} // END LPH8731_3C_MC60::setArea


void LPH8731_3C_MC60::white (void)
{
	fillScreen (WHITE);
}


void LPH8731_3C_MC60::black (void)
{
	fillScreen (BLACK); 
}

//*******************************************************************
//   Функция заливки экрана цветом color
//*******************************************************************
void LPH8731_3C_MC60::fillScreen (unsigned int color)
{
  unsigned int x;
  signed char i;
  setArea( 0, 100, 0, 80 );   //
  for (x = 0; x < 8080; x++)
  {
#ifdef _8_BIT_COLOR        //(8-BIT (256 COLOR))
    lcdSendD( color ); //
#else   
  lcdSendD( color >> 4 );
  lcdSendD((color << 4) | (color >> 8) );
  lcdSendD( color ); x++; //
#endif
  delayMicroseconds(1);
  LCD_CS_OFF;
  }
} // END LPH8731_3C_MC60:fillScreen


void LPH8731_3C_MC60::setPixColor (char x, char y, unsigned int color)
{
  signed char i;
  setArea( x, x, y, y );
#ifdef _8_BIT_COLOR
  lcdSendD( color );
#else
  lcdSendD( color >> 4 );
  lcdSendD( color << 4 );
#endif
}	// LPH8731_3C_MC60


//*******************************************************************
//  Функция вывода одного символа из массива font[]
//  - symbol – выводимый на экран символ
//  - x и y – начальная координата вывода символа
//  - t_color – цвет пикселя; b_color – цвет фона
//  - zoom_width и zoom_height увеличивают символ на экране в указанное число раз
//   ** - плохо работает поворот
//*******************************************************************
void LPH8731_3C_MC60::setSymbol (unsigned char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char temp_symbol, a, b, zw, zh, mask;
  char m, n;
  m = x;
  n = y;
  for ( a = 0; a < 5; a++)
  {
    temp_symbol = font[symbol][a];
    zw = 0;
    while (zw != zoom_width)
    {
      switch (rotation)
      {
        case 0: case 180: n = y; break;
        case 90: case 270: m = x; break;
      }
      mask = 0x01;
      for ( b = 0; b < 8; b++ )
      {
        zh = 0;
        while (zh != zoom_height)
        {
          switch (rotation)
          {
            case 0: case 180:
              if (temp_symbol & mask)
              {
                setPixColor (m + zw, n + zh, t_color);
              }
              break;
            case 90: case 270:
              if (temp_symbol & mask)
              {
                setPixColor (m + zh, n + zw, t_color);
              }
              break;
          }
          zh++;
        }
        mask <<= 1;
        switch (rotation)
        {
          case 0: case 180: n = n + zoom_height; break;
          case 90: case 270: m = m + zoom_height; break;
        }
      }
      zw++;
    }
    switch (rotation)
    {
      case 0: case 180: m = m + zoom_width; break;
      case 90: case 270: n = n + zoom_width; break;
    }
  } 
} // LPH8731_3C_MC60


//*******************************************************************
//   вывод символа без фонового цвета
//  ** - плохо работает поворот
//*******************************************************************
void LPH8731_3C_MC60::setSymbolShadow (unsigned char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char temp_symbol, a, b, zw, zh, mask;
  char m, n;
  m = x;
  n = y;
  for ( a = 0; a < 5; a++)
  {
    temp_symbol = font[symbol][a];
    zw = 0;
    while (zw != zoom_width)
    {
      switch (rotation)
      {
        case 0: case 180: n = y; break;
        case 90: case 270: m = x; break;
      }
      mask = 0x01;
      for ( b = 0; b < 8; b++ )
      {
        zh = 0;
        while (zh != zoom_height)
        {
          switch (rotation)
          {
            case 0: case 180:
              if (temp_symbol & mask)
              {
                setPixColor (m + zw, n + zh, t_color);
              }
              break;
            case 90: case 270:
              if (temp_symbol & mask)
              {
                setPixColor (m + zh, n + zw, t_color);
              }
              break;
          }
          zh++;
        }
        mask <<= 1;
        switch (rotation)
        {
          case 0: case 180: n = n + zoom_height; break;
          case 90: case 270: m = m + zoom_height; break;
        }
      }
      zw++;
    }
    switch (rotation)
    {
      case 0: case 180: m = m + zoom_width; break;
      case 90: case 270: n = n + zoom_width; break;
    }
  }
} // LPH8731_3C_MC60


//*******************************************************************
//  Функция вывода одного символа из массива font[]
//  - symbol – выводимый на экран символ
//  - x и y – начальная координата вывода символа
//  - t_color – цвет пикселя; b_color – цвет фона
//  - zoom_width и zoom_height увеличивают символ на экране в указанное число раз
//  - rotation – угол поворота текста (0º , 90º, 180º, 270º)
//===============================================================
void LPH8731_3C_MC60::setChar (unsigned char symbol, char x, char y, int t_color, int b_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char m;
  if (zoom_width == 0)   zoom_width = 1;
  if (zoom_height == 0)  zoom_height = 1;
  switch (rotation)
  {
    case 0:
      setSymbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rotation);
      break;
    //================================
    case 90:
      m = y + 3; y = x; x = m;
      lcdSend({CMD, 0x36});
      lcdSendD(0x08);
      setSymbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    case 180:
      x += 3; y += 2;
      lcdSend({CMD, 0x36});
      lcdSendD(0x88);
      setSymbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    case 270:
      m = y; y = x + 2; x = m;
      lcdSend({CMD, 0x36});
      lcdSendD(0xC8);
      setSymbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    default:
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      setSymbol( symbol, x, y, t_color, b_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      //=================================
  };
} // LPH8731_3C_MC60


//*******************************************************************
//  Функция вывода одного символа из массива font[] без фонового цвета
//*******************************************************************
void LPH8731_3C_MC60::setCharShadow (unsigned char symbol, char x, char y, int t_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char m;
  if (zoom_width == 0)   zoom_width = 1;
  if (zoom_height == 0)  zoom_height = 1;
  switch (rotation)
  {
    case 0:
      setSymbolShadow( symbol, x, y, t_color, zoom_width, zoom_height, rotation);
      break;
    //================================
    case 90:
      m = y + 3; y = x; x = m;
      lcdSend({CMD, 0x36});
      lcdSendD(0x08);
      setSymbolShadow( symbol, x, y, t_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    case 180:
      x += 3; y += 2;
      lcdSend({CMD, 0x36});
      lcdSendD(0x88);
      setSymbolShadow( symbol, x, y, t_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    case 270:
      m = y; y = x + 2; x = m;
      lcdSend({CMD, 0x36});
      lcdSendD(0xC8);
      setSymbolShadow( symbol, x, y, t_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      break;
    //================================
    default:
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      setSymbolShadow( symbol, x, y, t_color, zoom_width, zoom_height, rotation);
      lcdSend({CMD, 0x36});
      lcdSendD(0x48);
      //=================================
  };
} // LPH8731_3C_MC60

//*******************************************************************
//   вывод строки с фоновым цветом
//*******************************************************************
void LPH8731_3C_MC60::string  (char *str, int x, int y,  int t_color, int b_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char i = 0;

  if (zoom_width == 0)   zoom_width = 1;
  if (zoom_height == 0)  zoom_height = 1;

  while (str[i])
  {
    setSymbol(str[i], x + (i * 6 * zoom_width), y, t_color, b_color, zoom_width, zoom_height, rotation);
    i++;
  }
  delayMicroseconds(1);
  LCD_CS_OFF;
} // LPH8731_3C_MC60


//*******************************************************************
//   вывод строки без фонового цвета
//*******************************************************************
void LPH8731_3C_MC60::stringShadow (char *str, int x, int y,  int t_color, char zoom_width, char zoom_height, int rotation)
{
  unsigned char i = 0;

  if (zoom_width == 0)   zoom_width = 1;
  if (zoom_height == 0)  zoom_height = 1;

  while (str[i])
  {
    setCharShadow(str[i], x + (i * 6 * zoom_width), y, t_color, zoom_width, zoom_height, rotation);
    i++;
  }
  delayMicroseconds(1);
  LCD_CS_OFF;
} //LPH8731_3C_MC60


//*******************************************************************
//			Функция вывода картинки. 
//	x и y начальная позиция выводимого на экран изображения; 
//	width и height – ширина и высота изображения (в пикселях); 
//	*img – указатель на массив данных, составляющих изображение; 
//	rotation – угол поворота изображения (0º , 90º, 180º, 270º). 
//  Данная функция предполагает, что массив данных изображения расположен во flash-памяти микроконтроллера. 
//  Для преобразования необходимого изображения в массив, содержащий цвет пикселей, 
//  можно воспользоваться программами Image2Lcd или Nokia Image Creator. 
//  Единственное требование – направление вывода массива изображения должно выполняться слева-направо, сверху-вниз! 
//  При использовании 12-ти битной палитры, при преобразовании изображения в массив типа char, 
//  обязательно старший байт должен идти первым!
//*******************************************************************

void LPH8731_3C_MC60::outputImage (char x, char y, char width, char height, char *img, int rotation)
{
  unsigned char m;
  switch (rotation)
  {
    case 0:
      sendImage (x, y, width, height, img, rotation);
      break;
    //================================
    case 90:
      m = y + 3; y = x; x = m;
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x08});
      sendImage (x, y, width, height, img, rotation);
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x48});
      break;
    //================================
    case 180:
      x += 3; y += 2;
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x88});
      sendImage (x, y, width, height, img, rotation);
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x8});
      break;
    //================================
    case 270:
      m = y; y = x + 2; x = m;
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0xC8});
      sendImage (x, y, width, height, img, rotation);
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x8});
      break;
    //================================
    default:
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x48});
      sendImage (x, y, width, height, img, rotation);
      lcdSend({CMD, 0x36});
      lcdSend({DAT, 0x48});
      //=================================
  };
  delayMicroseconds(1);
  LCD_CS_OFF;
}   // LPH8731_3C_MC60


void LPH8731_3C_MC60::sendImage (char x, char y, char width, char height, char *img, int rotation)
{
  char x1, y1;
  y += 1;
  switch (rotation)
  {
    case 0: case 180:
      for (y1 = y; y1 < (y + height); y1++)
      {
        setArea( x, x + (width - 1), y1, y1 );
        for (x1 = x; x1 < x + width; x1++)
        {
  #ifdef _8_BIT_COLOR
            lcdSendD( *img++ );
  #else
            lcdSendD( (*img << 4) | (*(img + 1) >> 4) ); img++;
            lcdSendD( (*img << 4) | (*(img + 1)) ); img += 2;
            lcdSendD( *img++ ); x1++;
  #endif
        }
      }
      break;

    case 90: case 270:
      for (x1 = x; x1 < x + height; x1++)
      {
        setArea( x1, x1, y, y + (width - 1) );
        for (y1 = y; y1 < y + width; y1++)
        {
  #ifdef _8_BIT_COLOR
            lcdSendD( *img++ );
  #else
            lcdSendD(  (*img << 4) | (*(img + 1) >> 4) ); img++;
            lcdSendD(  (*img << 4) | (*(img + 1)) ); img += 2;
            lcdSendD(   *img++ ); y1++;
  #endif
        }
      }
      break;
  };
  delayMicroseconds(1);
  LCD_CS_OFF;
} // LPH8731_3C_MC60


//*******************************************************************
//                     GEOMETRIC FIGURES
//*******************************************************************

//*******************************************************************
//	Нарисовать линию. x1 и y1 – начальная позиция линии; x2 и y2 – конечная позиция линии.
//*******************************************************************
void LPH8731_3C_MC60::drawLine (char x1, char y1, char x2, char y2, int color)
{
  short  x, y, d, dx, dy, i, i1, i2, kx, ky;
  signed char flag;

  dx = x2 - x1;
  dy = y2 - y1;
  if (dx == 0 && dy == 0) setPixColor(x1, y1, color);
  else
  {
    kx = 1;
    ky = 1;
    if ( dx < 0 )
    {
      dx = -dx;
      kx = -1;
    }
    else if (dx == 0) kx = 0;
    if (dy < 0)
    {
      dy = -dy;
      ky = -1;
    }
    if (dx < dy)
    {
      flag = 0;
      d = dx;
      dx = dy;
      dy = d;
    }
    else flag = 1;
    i1 = dy + dy;
    d = i1 - dx;
    i2 = d - dx;
    x = x1;
    y = y1;

    for (i = 0; i < dx; i++)
    {
      setPixColor(x, y, color);
      if (flag) x += kx;
      else y += ky;
      if ( d < 0 ) d += i1;
      else
      {
        d += i2;
        if (flag) y += ky;
        else x += kx;
      }
    }
    setPixColor(x, y, color);
  }
  delayMicroseconds(1);
  LCD_CS_OFF;
} // LPH8731_3C_MC60
 
//*******************************************************************
//		Нарисовать рамку. size - толщина рамки.
//*******************************************************************
void LPH8731_3C_MC60::drawRect (char x1, char y1, char width, char height, char size, int color)
{
  unsigned int i;
  char x2 = x1 + (width - 1), y2 = y1 + (height - 1);
  for ( i = 1; i <= size; i++)
  {
    drawLine(x1, y1, x1, y2, color);
    drawLine(x2, y1, x2, y2, color);
    drawLine(x1, y1, x2, y1, color);
    drawLine(x1, y2, x2, y2, color);
    x1++;
    y1++;
    x2--;
    y2--;
  }
  delayMicroseconds(1);
  LCD_CS_OFF;
} // LPH8731_3C_MC60


//*******************************************************************
//              Нарисовать прямоугольник.
//*******************************************************************
void LPH8731_3C_MC60::fillRect (char x1, char y1, char width, char height, int color)
{
    unsigned int x, y;
    setArea( x1, x1 + (width - 1), y1, y1 + (height - 1) );
    y = width * height;
    for (x = 0; x < y; x++)
    {
  #ifdef _8_BIT_COLOR
      lcdSendD( color );
      lcdSendD( color );
  #else
      lcdSendD( color >> 4 );
      lcdSendD((color << 4) | (color >> 8) );
      lcdSendD( color ); x++;
  #endif
    }
  delayMicroseconds(1);
  LCD_CS_OFF;
} // LPH8731_3C_MC60


//*******************************************************************
//     Нарисовать окружность. rad – радиус окружности в пикселях.
//*******************************************************************
void LPH8731_3C_MC60::drawCircle (char xcenter, char ycenter, char rad, int color)
{
  signed int tswitch, x1 = 0, y1;
  char d;
  d = ycenter - xcenter;
  y1 = rad;
  tswitch = 3 - 2 * rad;
  while (x1 <= y1)
  {
    setPixColor(xcenter + x1, ycenter + y1, color);
    setPixColor(xcenter + x1, ycenter - y1, color);
    setPixColor(xcenter - x1, ycenter + y1, color);
    setPixColor(xcenter - x1, ycenter - y1, color);
    setPixColor(ycenter + y1 - d, ycenter + x1, color);
    setPixColor(ycenter + y1 - d, ycenter - x1, color);
    setPixColor(ycenter - y1 - d, ycenter + x1, color);
    setPixColor(ycenter - y1 - d, ycenter - x1, color);

    if (tswitch < 0) tswitch += (4 * x1 + 6);
    else
    {
      tswitch += (4 * (x1 - y1) + 10);
      y1--;
    }
    x1++;
  }
}    // LPH8731_3C_MC60


//*******************************************************************
//		Нарисовать круг, заполненный цветом color.
//*******************************************************************
void LPH8731_3C_MC60::fillCircle (char xcenter, char ycenter, char rad, int color)
{
  signed int x1 = 0, y1, tswitch;
  y1 = rad;
  tswitch = 1 - rad;

  do
  {
    drawLine(xcenter - x1, ycenter + y1, xcenter + x1, ycenter + y1, color);
    drawLine(xcenter - x1, ycenter - y1, xcenter + x1, ycenter - y1, color);
    drawLine(xcenter - y1, ycenter + x1, xcenter + y1, ycenter + x1, color);
    drawLine(xcenter - y1, ycenter - x1, xcenter + y1, ycenter - x1, color);

    if (tswitch < 0)
      tswitch += 3 + 2 * x1++;
    else
      tswitch += 5 + 2 * (x1++ - y1--);
  } while (x1 <= y1);
} // LPH8731_3C_MC60


void LPH8731_3C_MC60::drawRing(char xcenter, char ycenter, char rad_int, char rad_ext, int color, char chunk)
{
  signed int tswitch, x1, y1;
  char d, rad;
  d = ycenter - xcenter;
  for(rad=rad_ext; rad>=rad_int; rad--){
    x1 = 0;
    y1 = rad;
    tswitch = 3 - 2 * rad;
    while (x1 <= y1)
    {
       switch(chunk) {   
       case 8:
          setPixColor(xcenter - x1, ycenter - y1, color); //8
       case 7:
          setPixColor(ycenter - y1 - d, ycenter - x1, color);//7
       case 6:
          setPixColor(ycenter - y1 - d, ycenter + x1, color);//6
       case 5:
          setPixColor(xcenter - x1, ycenter + y1, color); //5
       case 4:         
          setPixColor(xcenter + x1, ycenter + y1, color); //4
       case 3:
          setPixColor(ycenter + y1 - d, ycenter + x1, color);//3
       case 2:
          setPixColor(ycenter + y1 - d, ycenter - x1, color); //2
       case 1:
          setPixColor(xcenter + x1, ycenter - y1, color); //1
       case 0:
       break;
      }
      if (tswitch < 0) tswitch += (4 * x1 + 6);
      else
      {
        tswitch += (4 * (x1 - y1) + 10);
        y1--;
      }
      x1++;
    }
  }
} // LPH8731_3C_MC60



//*******************************************************************
//                     Нарисовать контуры треугольника.
//*******************************************************************
void LPH8731_3C_MC60::drawTriangle(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
 drawLine(x1, y1, x2, y2, color);
 drawLine(x3, y3, x1, y1, color);
 drawLine(x3, y3, x2, y2, color);  
} // LPH8731_3C_MC60


//*******************************************************************
//         Нарисовать треугольник, заполненный цветом color.
//*******************************************************************
void LPH8731_3C_MC60::fillTriangle(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
 fillTriangleA(x1, y1, x2, y2, x3, y3, color);
 fillTriangleA(x3, y3, x1, y1, x2, y2, color);
 fillTriangleA(x3, y3, x2, y2, x1, y1, color);
}    // LPH8731_3C_MC60

//*******************************************************************
//		вспомогательная функцция для fillTriangle
//*******************************************************************
void LPH8731_3C_MC60::fillTriangleA(char x1, char y1, char x2, char y2, char x3, char y3, int color)
{
  signed long x, y, addx, dx, dy;
  signed long P;
  int i;
  long a1, a2, b1, b2;
  if (y1 > y2)  {
    b1 = y2;
    b2 = y1;
    a1 = x2;
    a2 = x1;
  }
  else  {
    b1 = y1;
    b2 = y2;
    a1 = x1;
    a2 = x2;
  }
  dx = a2 - a1;
  dy = b2 - b1;
  if (dx < 0)dx = -dx;
  if (dy < 0)dy = -dy;
  x = a1;
  y = b1;

  if (a1 > a2)    addx = -1;
  else           addx = 1;

  if (dx >= dy)
  {
    P = 2 * dy - dx;
    for (i = 0; i <= dx; ++i)
    {
      drawLine((int)x, (int)y, x3, y3, color);
      if (P < 0)
      {
        P += 2 * dy;
        x += addx;
      }
      else
      {
        P += 2 * dy - 2 * dx;
        x += addx;
        y ++;
      }
    }
  }
  else
  {
    P = 2 * dx - dy;
    for (i = 0; i <= dy; ++i)
    {
      drawLine((int)x, (int)y, x3, y3, color);
      if (P < 0)
      {
        P += 2 * dx;
        y ++;
      }
      else
      {
        P += 2 * dx - 2 * dy;
        x += addx;
        y ++;
      }
    }
  }
} // LPH8731_3C_MC60
