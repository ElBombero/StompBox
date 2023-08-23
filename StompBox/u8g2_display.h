#include <U8x8lib.h>
//#include <U8glib.h>
#include <U8g2lib.h>
#include "utils.h"


class U8x8Display {
public:
  
  U8x8Display(U8X8* u8x8, const uint8_t rows, const uint8_t cols);
  ~U8x8Display();

  void Write(const uint8_t row, const uint8_t col, const char* buffer, const uint8_t fontId = u8x8_font_px437wyse700a_2x2_r);
  void SetSection(const uint8_t sectionId, const uint8_t row, const uint8_t col, const uint8_t width, const uint8_t fontId = 255);
  void ClearSection(const uint8_t sectionId);
  void WriteSection(const uint8_t sectionId, const char* buff, const bool inverted = false); //, bool clear = true);
  void Backlight(const bool on);
  void SetContrast(const uint8_t contrast);


private:
  static const uint8_t c_maxSections = 16;
  struct Section {
    Section(uint8_t r = 0, uint8_t c = 0, uint8_t w = 0, uint8_t font = u8x8_font_8x13B_1x2_f) :
      row(r),
      col(c),
      width(w),
      fontId(font)
      {}
    uint8_t row;
    uint8_t col;
    uint8_t width;
    uint8_t fontId;
  };

  U8X8* m_u8x8;
  uint8_t m_sectionsCnt;
  uint8_t m_displayRows;
  uint8_t m_displayCols;
  Section m_sections[c_maxSections];
};