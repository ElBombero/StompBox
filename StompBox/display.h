#include <LiquidCrystal_I2C.h>
#include "utils.h"


class I2cDisplay {
public:
  
  I2cDisplay(const uint8_t address, const uint8_t rows, const uint8_t cols);
  ~I2cDisplay();

  void Write(const uint8_t row, const uint8_t col, const char* buffer);
  void SetSection(const uint8_t sectionId, const uint8_t row, const uint8_t col, const uint8_t width);
  void ClearSection(const uint8_t sectionId);
  void WriteSection(const uint8_t sectionId, const char* buff); //, bool clear = true);
  void Backlight(const bool on);


private:
  static const uint8_t c_maxSections = 8;
  struct Section {
    Section(uint8_t r = 0, uint8_t c = 0, uint8_t w = 0) :
      row(r),
      col(c),
      width(w)
      {}
    uint8_t row;
    uint8_t col;
    uint8_t width;
  };

  LiquidCrystal_I2C m_lcd;
  uint8_t m_sectionsCnt;
  uint8_t m_displayRows;
  uint8_t m_displayCols;
  Section m_sections[c_maxSections];
};