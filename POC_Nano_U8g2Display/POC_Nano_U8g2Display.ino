#include <Arduino.h>
//#include <U8x8lib.h>
//#include <U8glib.h>
#include <U8g2lib.h>


U8X8_SSD1306_128X64_NONAME_HW_I2C u8x8(U8X8_PIN_NONE);
//U8G2_SH1106_128X64_WINSTAR_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
//U8G2_SSD1305_128X64_ADAFRUIT_1_HW_I2C u8g2(U8X8_PIN_NONE);
//U8X8_SSD1306_128X64_ALT0_HW_I2C u8x8(U8X8_PIN_NONE);

//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NO_ACK);
//U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0);
//U8G2_SH1107_SEEED_128X64_F_HW_I2C u8g(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);


void setup(void)
{
  u8x8.begin();
  u8x8.setPowerSave(0);
  //u8g2.begin();
  //u8g2.setFont();

//  u8g.begin();

//  u8x8.setFont(u8x8_font_pxplusibmcgathin_f);
  u8x8.setFont(u8x8_font_px437wyse700a_2x2_r);

//  u8g.
  u8x8.drawString(0,0,"Hello");
  u8x8.drawString(0,2,"12345678");
  u8x8.drawString(0,4,"Bonjour");
  u8x8.drawString(0,6,"Nazdarek");
  //u8x8.drawLine(0, 0, 64, 32);
}

//int r = 1;
//char[] = "HELLO";
void loop(void)
{
//  u8g2.clearBuffer();
////  delay(2000);
////  u8x8.refreshDisplay();		// only required for SSD1606/7  
//  u8g2.drawCircle(u8g2.getDisplayWidth()/2, u8g2.getDisplayHeight()/2, r++, U8G2_DRAW_UPPER_RIGHT);
//  delay(200);
//  u8g2.sendBuffer();
//  r = (r + 1) % u8g2.getDisplayWidth();
}