#include "board.hpp"

Board::Board(int columnCount, int rowCount, int mineCount) : m_columns(columnCount), m_rows(rowCount), m_mines(mineCount){
  generate();
}

Element Board::Render(int cursorPosX, int cursorPosY, Color colorTheme){
  return std::make_shared<BoardNode>(*this, cursorPosX, cursorPosY, colorTheme);
}

void Board::generate(){
  // Random Generator
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
          if(isValid(nx, ny) && m_hiddenBoard[ny * m_columns + ny] != 9) m_hiddenBoard[ny * m_columns + nx]++;
        }
      }
    }
  }

  m_visibleBoard.assign(m_columns * m_rows, 0);
}

void Board::reveal(int x, int y){
  if(!isValid(x, y) || m_visibleBoard[y * m_columns + x] != 0) return;

  if(m_hiddenBoard[y * m_columns + x] == 9){
    m_isLoss = true;
    return;
  }

  m_visibleBoard[y * m_columns + x] = 1;

  if(m_hiddenBoard[y * m_columns + x] != 0) return; // Stop recursion

  // recurse to 8 neighbors
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
  if(isValid(x, y) || m_visibleBoard[y * m_columns + x] != 1) return;

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
int Board::getMineCount()const{ return m_columns * m_rows; }
int Board::getFlagCount()const{ return m_flagCount; }

int Board::getWidth()const{ return m_columns * 3; }
int Board::getHeight()const{ return m_rows; }

inline uint8_t& Board::getHiddenCell(int x, int y){ return m_hiddenBoard[y * m_columns + x]; }
inline uint8_t& Board::getVisibleCell(int x, int y){ return m_visibleBoard[y * m_columns + x]; }

bool Board::isWin()const{
  for(int i = 0; i < m_hiddenBoard.size(); ++i)
    if(m_hiddenBoard[i] != 9 && m_visibleBoard[i] != 1) return false;

  return true;
}
bool Board::isLoss()const{ return m_isLoss; }

// private stufff
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

void Board::BoardNode::ComputeRequirement(){
  requirement_.min_x = m_board.getWidth();
  requirement_.min_y = m_board.getHeight();
}
void Board::BoardNode::Render(Screen &screen){
  for(int y = 0; y < m_board.getRow(); ++y){
    for(int x = 0; x < m_board.getColumn(); ++x){
      int screenX = box_.x_min + (x * 3);
      int screenY = box_.y_min + y;

      if(screenY > box_.y_max) break;
      if(screenX + 2 > box_.x_max) continue;

      uint8_t hiddenCell = m_board.getHiddenCell(x, y);
      uint8_t visibleCell = m_board.getVisibleCell(x, y);
      bool isCursor = x == m_cursorX && y == m_cursorY;

      std::string label = " ";
      Color fg = Color::White;
      Color bg;

      bool isEven = (x + y) % 2 == 0;

      if(isCursor){
        bg = Color::Yellow;
        fg = Color::Black;
      }else if(visibleCell == 1){
        bg = visibleCell == 2 ? Color::GrayDark : m_colorTheme;
        if(!isEven && visibleCell != 2) bg = Color::Interpolate(0.2f, m_colorTheme, Color::Black);
      }

      if(visibleCell == 2){
        label = "🚩";
        fg = Color::Red;
      }else if(visibleCell == 1){
        if(hiddenCell == 9){
          label = "💣";
          fg = Color::Black;
        }else if(hiddenCell > 0){
          static const std::string nums[] = {" ", "1", "2", "3", "4", "5", "6", "7", "8"};
          label = nums[hiddenCell];
          fg = m_board.GetNumberColor(hiddenCell);
        }
      }

      auto &p_mid = screen.PixelAt(screenX + 1, screenY);
      p_mid.character = label;
      p_mid.background_color = bg;
      p_mid.foreground_color = fg;

      auto &p_left = screen.PixelAt(screenX, screenY);
      p_left.character = " ";
      p_left.background_color = bg;

      auto &p_right = screen.PixelAt(screenX + 2, screenY);
      p_right.character = " ";
      p_right.background_color = bg;
    }
  }
}
