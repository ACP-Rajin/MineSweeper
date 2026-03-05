#include "board.hpp"

Board::Board(int columnCount, int rowCount, int mineCount) : m_columns(columnCount), m_rows(rowCount), m_mines(mineCount){
  generate();
}

Element Board::Render(int cursorX, int cursorY, Color colorTheme){
  std::vector<Elements> grid;

  for(int y = 0; y < m_rows; y++){
    Elements row;
    for(int x = 0; x < m_columns; x++){
      uint8_t hiddenCell = m_hiddenBoard[y * m_columns + x];
      uint8_t visibleCell = m_visibleBoard[y * m_columns + x];
      bool isCursor = x == cursorX && y == cursorY;
      bool isEven = (x + y) % 2 == 0;

      std::string label = "  ";
      Color fg = Color::White;
      Color bg;

      if(isCursor){
        bg = Color::Yellow;
        fg = Color::Black;
      }else if(visibleCell == 1){
        bg = isEven ? Color::White : Color::GrayLight;
      }else if(visibleCell == 2){
        bg = isEven ? Color::White : Color::GrayLight;
      }else{
        bg = isEven ? colorTheme : Color::Interpolate(0.1f, colorTheme, Color::Black);
      }

      if(visibleCell == 2){
        label = "🚩";
        fg = Color::Red;
      }else if(visibleCell == 1){
        if(hiddenCell == 9){
          label = "💣";
          fg = Color::Black;
        }else if(hiddenCell > 0){
          label = " " + std::to_string(hiddenCell) + " ";
          fg = GetNumberColor(hiddenCell);
        }
      }

      auto cell = text(label) | center | size(WIDTH, EQUAL, 3) | bgcolor(bg) | color(fg);

      row.push_back(cell);
    }
    grid.push_back(row);
  }

  return gridbox(grid) | border;
}

void Board::generate(){
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_int_distribution<int> disX(0, m_columns - 1);
  std::uniform_int_distribution<int> disY(0, m_rows - 1);

  int placed = 0;

  m_hiddenBoard.assign(m_columns * m_rows, 0);

  while(placed < m_mines){
    int x = disX(gen);
    int y = disY(gen);

    if(m_hiddenBoard[y * m_columns +x] != 9){
      m_hiddenBoard[y * m_columns +x] = 9;
      ++placed;

      for(int dy = -1; dy <= 1; ++dy){
        for(int dx = -1; dx <= 1; ++dx){
          int nx = x + dx;
          int ny = y + dy;
          if(isValid(nx, ny) && m_hiddenBoard[ny * m_columns + nx] != 9) m_hiddenBoard[ny * m_columns + nx]++;
        }
      }
    }
  }

  m_visibleBoard.assign(m_columns * m_rows, 0);
}

void Board::reveal(int x, int y){
  if(!isValid(x, y)) return;
  if(m_visibleBoard[y * m_columns + x] != 0) return;

  if(m_hiddenBoard[y * m_columns + x] == 9){
    m_isLoss = true;
    return;
  }

  m_visibleBoard[y * m_columns + x] = 1;

  if(m_hiddenBoard[y * m_columns + x] != 0) return;

  for(int dy = -1; dy <= 1; ++dy)
    for(int dx = -1; dx <= 1; ++dx)
      if(dx != 0 || dy != 0)
        reveal(x + dx, y + dy);
}

void Board::toggleFlag(int x, int y){
  if(m_visibleBoard[y * m_columns + x] == 2){
    m_visibleBoard[y * m_columns + x] = 0;
    m_flagCount--;
  }else if(m_visibleBoard[y * m_columns + x] == 0){
    m_visibleBoard[y * m_columns + x] = 2;
    m_flagCount++;
  }
}

void Board::chord(int x, int y){
  if(!isValid(x, y)) return;
  if(m_visibleBoard[y * m_columns + x] != 1) return;

  int mineCount = m_hiddenBoard[y * m_columns + x];
  if(mineCount == 0 || mineCount == 9) return;

  int flagCount = 0;

  for(int dy = -1; dy <= 1; ++dy){
    for(int dx = -1; dx <= 1; ++dx){
      int nx = x + dx;
      int ny = y + dy;

      if(isValid(nx, ny) && m_visibleBoard[ny * m_columns + nx] == 2) flagCount++;
    }
  }

  if(flagCount == mineCount){
    for(int dy = -1; dy <= 1; ++dy){
      for(int dx = -1; dx <= 1; ++dx){
        int nx = x + dx;
        int ny = y + dy;

        if(isValid(nx, ny) && m_visibleBoard[ny * m_columns + nx] == 0) reveal(nx, ny);
      }
    }
  }
}

void Board::revealAllMine(){
  for(int i = 0; i < m_hiddenBoard.size(); ++i)
    if(m_hiddenBoard[i] == 9) m_visibleBoard[i] = 1;
}

void Board::setColumn(int coulmnCount){ m_columns = coulmnCount; }
void Board::setRow(int rowCount){ m_rows = rowCount; }
void Board::setMineCount(int mineCount){ m_mines = mineCount; }

int Board::getColumn()const{ return m_columns; }
int Board::getRow()const{ return m_rows; }
int Board::getMineCount()const{ return m_mines; }
int Board::getFlagCount()const{ return m_flagCount; }

int Board::getWidth()const{ return m_columns * 3; }
int Board::getHeight()const{ return m_rows; }

bool Board::isWin()const{
  for(int i = 0; i < m_hiddenBoard.size(); ++i)
    if(m_hiddenBoard[i] != 9 && m_visibleBoard[i] != 1) return false;

  return true;
}
bool Board::isLoss()const{ return m_isLoss; }

inline bool Board::isValid(int x, int y)const{ return x >= 0 && x < m_columns && y >= 0 && y < m_rows; }

Color Board::GetNumberColor(uint8_t number)const{
  switch(number){
    case 1: return Color::Blue;
    case 2: return Color::Green;
    case 3: return Color::Red;
    case 4: return Color::BlueLight;
    case 5: return Color::RedLight;
    case 6: return Color::Cyan;
    default: return Color::White;
  }
}
