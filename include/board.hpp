#pragma once

#include <ftxui/dom/node.hpp>
#include <random>
#include <vector>
/*
 * Safe spots, ⬜ = 0
 * Danger spots = 1-8
 * Mine spots, 💥 = 9
 * Flag spots, 🚩 on another vector as it is a visual stuff
*/

using namespace ftxui;

class Board{
  // Width Height
  int m_columns, m_rows, m_mines;
  int m_flagCount = 0;
  bool m_isWin = false, m_isLoss = false;

  std::vector<uint8_t> m_hiddenBoard;
  std::vector<uint8_t> m_visibleBoard;

  public:
  Board(int columnCount = 8, int rowCount = 8, int mineCount = 7);

  Element Render(int cursorPosX, int cursorPosY, Color colorTheme);

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

  inline uint8_t& getHiddenCell(int x, int y);
  inline uint8_t& getVisibleCell(int x, int y);

  bool isWin()const;
  bool isLoss()const;

  private:
  inline bool isValid(int x, int y)const;

  Color GetNumberColor(uint8_t number)const;

  class BoardNode : public Node{
    Board &m_board;
    int m_cursorX, m_cursorY;
    Color m_colorTheme;

    public:
    BoardNode(Board &board, int cursorPosX, int cursorPosY, Color colorTheme) : m_board(board), m_cursorX(cursorPosX), m_cursorY(cursorPosY), m_colorTheme(colorTheme) {}

    void ComputeRequirement()override;
    void Render(Screen &screen)override;
  };
};
