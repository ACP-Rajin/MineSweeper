#pragma once

#include <ftxui/component/component.hpp>
#include <random>

/*
 * Safe spots, ⬜ = 0
 * Danger spots = 1-8
 * Mine spots, 💥 = 9
 * Flag spots, 🚩 on another vector as it is a visual stuff
*/

using namespace ftxui;

class Board{
  int m_columns, m_rows, m_mines;
  int m_flagCount = 0;
  bool m_isWin = false, m_isLoss = false;

  std::vector<uint8_t> m_hiddenBoard;  // Safe spots = 0, Hints = 1-8, Mine spot = 9
  std::vector<uint8_t> m_visibleBoard; // Not Revealed = 0, Reveal = 1, Flag = 2

  public:
  Board(int columnCount = 8, int rowCount = 8, int mineCount = 7);

  Element Render(int cursorX, int cursorY, Color colorTheme);

  void generate();

  void reveal(int x, int y);

  void toggleFlag(int x, int y);

  void chord(int x, int y);

  void revealAllMine();

  void setColumn(int coulmnCount);
  void setRow(int rowCount);
  void setMineCount(int mineCount);

  int getColumn()const;
  int getRow()const;
  int getMineCount()const;
  int getFlagCount()const;

  int getWidth()const;
  int getHeight()const;

  bool isWin()const;
  bool isLoss()const;

  private:
  inline bool isValid(int x, int y)const;
  Color GetNumberColor(uint8_t number)const;
};
