#include <LPH8731_3C_MC60.h>
//#include <SimpleTimer.h>

/*
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

LPH8731_3C_MC60 lcd(D8, D2, D3);


static byte x = 0;
static byte y = 0;
static int color = 0;

//SimpleTimer timer;

void setup ()
{
  Serial.begin(115200);
  lcd.begin();
  lcd.white();
  delay(1000);
  Serial.println("step1");
  lcd.setSymbol(164, 40, 20, 0, 255, 6, 6, 0);
  delay(4000);
  lcd.white();
  Serial.println("step2");
  lcd.setSymbolShadow(164, 40, 20, 0, 6, 6, 90);
  delay(4000);
  Serial.println("step3");
  lcd.white();
  lcd.setChar(164, 40, 20, 0, 255, 6, 6, 180);
  delay(4000);
  lcd.white();
  Serial.println("step4");
  lcd.string("test", 0, 2, 0, 255, 1, 1, 0);
  lcd.string("test", 0, 10, 0, 255, 2, 2, 0);
  lcd.stringShadow("test", 0, 25, 0, 3, 3, 0);
  lcd.stringShadow("test", 0, 50, 0, 4, 4, 0);
  delay(5000);
  Serial.println("step5");
  lcd.white();
  lcd.drawRect(20, 10, 60, 60, 3, 0);
  delay(7000);
  Serial.println("step6");
  lcd.fillScreen(161);
  lcd.drawLine(0, 0, 101, 80, 0);
  delay(5000);
  Serial.println("step7");
  lcd.white();
  lcd.fillRect(20, 10, 60, 60, 2500);
  delay(5000);
  Serial.println("step8");
  lcd.fillScreen(2320);
  lcd.drawCircle(50, 40, 30, 0);
  delay(5000);
  Serial.println("step9");
  lcd.white();
  lcd.fillCircle(50, 40, 30, 2580);
  delay(5000);
  Serial.println("step10");
  lcd.white();
  lcd.drawTriangle(50, 5, 5, 70, 95, 70, 0);
  delay(5000);
  Serial.println("step11");
  lcd.white();
  lcd.fillTriangle(50, 5, 5, 70, 95, 70, 0); 
  delay(5000);
  Serial.println("step12,5");
  lcd.white();
  for (char ii=0; ii<9; ii++){
    lcd.drawRing(50, 40, 10, 20, 0, ii);
    delay(1000);
  }
  for (char ii=0; ii<9; ii++){
    lcd.drawRing(50, 40, 10, 20, 255, ii);
    delay(1000);
  }
  delay(5000);
  Serial.println("step12");
  lcd.black();
  lcd.string("THE", 30, 10, 1000, 3096, 2, 2, 0);
  lcd.stringShadow("END", 25, 45, 2000, 3, 3, 0);
  delay(5000);
  lcd.white();
  Serial.println("Finish");

//  timer.setInterval(1L, drawPixel());
}

void loop ()
{
  if (y != 80) {
    for (x = 0; x != 101; x++) {
      lcd.setPixColor(x, y, color);
    }
    if (x >= 101) {
      y++;
      x = 0;
    }
  } else {
    color = color + 10;
    Serial.println(color);
    y = 0; x = 0;
  }
  delay(50);

}

//void drawPixel() {
//  static byte x = 0;
//  static byte y = 0;
//  static int color = 0;
//  if (y != 80) {
//    for (x = 0; x != 101; x++) {
//      lcd.setPixColor(x, y, color);
//    }
//    if (x >= 101) {
//      y++;
//      x = 0;
//    }
//  } else {
//    color = color + 10;
//    Serial.println(color);
//    y = 0; x = 0;
//  }
//}
