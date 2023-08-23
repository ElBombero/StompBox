#include "u8g2_display.h"

U8x8Display::U8x8Display(U8X8* u8x8, const uint8_t rows, const uint8_t cols) :
  m_u8x8(u8x8),
  m_displayCols(cols),
  m_displayRows(rows),
  m_sectionsCnt(0)
{
  m_u8x8->begin();
  m_u8x8->clear();
  //m_u8x8.setContrast(0x7f);
  //m_u8x8.backlight();
}

U8x8Display::~U8x8Display() {
}

void U8x8Display::Write(const uint8_t row, const uint8_t col, const char *buffer, const uint8_t fontId) {
  m_u8x8->setFont(fontId);
  m_u8x8->drawString(col, row, buffer);
}

void U8x8Display::SetSection(const uint8_t sectionId, const uint8_t row, const uint8_t col, const uint8_t width, const uint8_t fontId)
{
  if(sectionId >= c_maxSections) {
    return;
  }  
  m_sections[sectionId].row = row;
  m_sections[sectionId].col = col;
  m_sections[sectionId].width = Min<const uint8_t>(width, (m_displayCols - col));
  if(fontId != 255) {
    m_sections[sectionId].fontId = fontId;
  }
}

void U8x8Display::ClearSection(const uint8_t sectionId)
{
  char buffer[] = "                                "; // 32 x ' '
  size_t len = Min<uint8_t>(strlen(buffer) - 1, m_sections[sectionId].width);
  buffer[len] = '\0';
  Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer);
}

void U8x8Display::WriteSection(const uint8_t sectionId, const char* buff, const bool inverted) //, bool clear = true)
{
  if(sectionId >= c_maxSections) {
    return;
  }
  char buffer[] = "                                "; // 32 x ' '
  size_t len = Min<size_t>(strlen(buffer) -1, Min<size_t>(strlen(buff), m_sections[sectionId].width));
//  size_t len = Min(strlen(buffer) - 1, m_sections[sectionId].width);
  //buffer[len] = '\0';
  buffer[m_sections[sectionId].width] = '\0';
  /*if(clear) {
    Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer);
  }*/    
  memcpy(buffer, buff, len);
  if(inverted) {
    m_u8x8->inverse();
  }
  Write(m_sections[sectionId].row, m_sections[sectionId].col, buffer, m_sections[sectionId].fontId);
  m_u8x8->noInverse();
  //m_u8x8.flush();
}

void U8x8Display::Backlight(const bool on)
{
  //on ? m_u8x8->backlight() : m_u8x8->noBacklight();
}

void U8x8Display::SetContrast(const uint8_t contrast)
{
  m_u8x8->setContrast(contrast);
}