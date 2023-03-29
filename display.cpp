#include "display.h"

I2cDisplay::I2cDisplay(const uint8_t address, const uint8_t rows, const uint8_t cols) :
  m_lcd(address, cols, rows),
  m_displayCols(cols),
  m_displayRows(rows),
  m_sectionsCnt(0)
{
  m_lcd.init();
  m_lcd.clear();
  m_lcd.setContrast(0x7f);
  //m_lcd.backlight();
}

I2cDisplay::~I2cDisplay() {
}

I2cDisplay::Write(const uint8_t row, const uint8_t col, const char *buffer) {
  m_lcd.setCursor(col, row);
  m_lcd.print(buffer);
}

void I2cDisplay::SetSection(const uint8_t sectionId, const uint8_t row, const uint8_t col, const uint8_t width)
{
  if(sectionId >= c_maxSections) {
    return;
  }  
  m_sections[sectionId].row = row;
  m_sections[sectionId].col = col;
  m_sections[sectionId].width = Min(width, (m_displayCols - col));
}

void I2cDisplay::ClearSection(const uint8_t sectionId)
{
  char buffer[] = "                                "; // 32 x ' '
  size_t len = Min(strlen(buffer) - 1, m_sections[sectionId].width);
  buffer[len] = '\0';
  Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer);
}

void I2cDisplay::WriteSection(const uint8_t sectionId, const char* buff) //, bool clear = true)
{
  if(sectionId >= c_maxSections) {
    return;
  }
  char buffer[] = "                                "; // 32 x ' '
  size_t len = Min(strlen(buffer) -1, Min(strlen(buff), m_sections[sectionId].width));
//  size_t len = Min(strlen(buffer) - 1, m_sections[sectionId].width);
  //buffer[len] = '\0';
  buffer[m_sections[sectionId].width] = '\0';
  /*if(clear) {
    Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer);
  }*/    
  memcpy(buffer, buff, len);
  Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer);
  m_lcd.flush();
}

void I2cDisplay::Backlight(const bool on)
{
  on ? m_lcd.backlight() : m_lcd.noBacklight();
}