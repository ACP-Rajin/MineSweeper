#pragma once

#include <ftxui/dom/elements.hpp>
#include <random>

inline int random(int min, int max){
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_int_distribution<int> dis(min, max - 1);

  return dis(gen);
}

inline double random_double(double min, double max){
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<double> dis(min, max);

  return dis(gen);
}

/*
 * Safe spots, ⬜ = 0
 * Danger spots = 1-8
 * Mine spots, 💥 = 9
 * Flag spots, 🚩 on another vector as it is a visual stuff
*/

using namespace ftxui;

class Board{
  uint32_t m_width, m_height, m_mines;

  uint32_t m_flag_count = 0;
  bool m_isLoss = false, m_isWin = false;

  std::vector<uint8_t> m_hidden;  // 0=empty,  1-8=nums,   9=mine
  std::vector<uint8_t> m_visible; // 0=hidden, 1=revealed, 2=flagged;

  public:
  Board(uint32_t width = 8, uint32_t height = 8, uint32_t mines = 8) : m_width(width), m_height(height), m_mines(mines){
    generate();
  }

  Element Render(uint32_t cur_x, uint32_t cur_y, Color theme_color){
    return std::make_shared<BoardNode>(*this, cur_x, cur_y, theme_color) | border;
  }

  void reveal(uint32_t x, uint32_t y){
    if(x >= m_width || y >= m_height)   return; // If position not valid
    if(m_visible[y * m_width + x] != 0) return; // If already revealed/flagged
    if(m_hidden[y * m_width + x] == 9){         // Game lost if the revealed position has a mine
      m_isLoss = true;
      return;
    }

    m_visible[y * m_width + x] = 1;

    if(m_hidden[y * m_width + x] != 0) return;  // Stop recursion if not safe

    // recurse to 8 neighbors
    for(int dy = -1; dy <= 1; ++dy)
      for(int dx = -1; dx <= 1; ++dx)
        if(dx != 0 || dy != 0)
          reveal(x + dx, y + dy);
  }

  void revealAllMines(){
    for(size_t i = 0; i < m_hidden.size(); ++i)
      if(m_hidden[i] == 9) m_visible[i] = 1;
  }

  void toggleFlag(uint32_t x, uint32_t y){
    if(m_visible[y * m_width + x] == 2){
      m_visible[y * m_width + x] = 0;
      m_flag_count--;
    }else if(m_visible[y * m_width + x] == 0){
      m_visible[y * m_width + x] = 2;
      m_flag_count++;
    }
  }

  void chord(uint32_t x, uint32_t y){
    if(x >= m_width || y >= m_height) return; // If position not valid

    size_t index = y * m_width + x;
    if(m_visible[index] != 1) return; // If not revealed

    uint8_t mineCount = m_hidden[index];
    if(mineCount == 0 || mineCount == 9) return; // To prevent a bug

    uint8_t flagCount = 0;

    for(int8_t dy = -1; dy <= 1; ++dy){
      for(int8_t dx = -1; dx <= 1; ++dx){
        if(dx == 0 && dy == 0) continue;
        uint32_t nx = x + dx;
        uint32_t ny = y + dy;
        if(nx < m_width && ny < m_height){
          if(m_visible[ny * m_width + nx] == 2){
            flagCount++;
          }
        }
      }
    }

    if(flagCount == mineCount){
      for(int8_t dy = -1; dy <= 1; ++dy){
        for(int8_t dx = -1; dx <= 1; ++dx){
          if(dx == 0 && dy == 0) continue;
          uint32_t nx = x + dx;
          uint32_t ny = y + dy;

          if(nx < m_width && ny < m_height){
            size_t nIdx = ny * m_width + nx;
            if(m_visible[nIdx] == 0) reveal(nx, ny);
          }
        }
      }
    }
  }

  void generate(){
    m_hidden = generate(m_width, m_height, m_mines);
    m_visible.assign(m_width * m_height, 0);
  }

  void setWidth(uint32_t w){m_width = w;}
  void setHeight(uint32_t h){m_height = h;}
  void setMineCount(uint32_t m){m_mines = m;}

  uint32_t getWidth()const{return m_width;}
  uint32_t getHeight()const{return m_height;}
  uint32_t getFlagCount()const{return m_flag_count;}

  inline uint8_t& getHiddenCell(uint32_t x, uint32_t y){return m_hidden[y * m_width + x];}
  inline uint8_t& getVisibleCell(uint32_t x, uint32_t y){return m_visible[y * m_width + x];}

  bool isWin(){
    for(size_t i = 0; i < m_hidden.size(); ++i)
      if(m_hidden[i] != 9 && m_visible[i] != 1) return false;

    return true;
  }

  bool isLoss(){return m_isLoss;}

  private:
  static std::vector<uint8_t> generate(uint32_t width, uint32_t height, uint32_t mines){
    std::vector<uint8_t> board(width * height, 0);
    uint32_t placed = 0;

    while(placed < mines){
      uint32_t x = random(0, width);
      uint32_t y = random(0, height);
      if(board[y * width + x] != 9){
        board[y * width + x] = 9;
        placed++;

        for(int dy=-1; dy<=1; ++dy){
          for(int dx=-1; dx<=1; ++dx){
            int ny = y + dy;
            int nx = x + dx;
            if(ny>=0 && ny<height && nx>=0 && nx<width && board[ny * width + nx]!=9)board[ny * width + nx]++;
          }
        }
      }
    }
    return board;
  }

  Color GetNumberColor(uint8_t n)const{
    switch(n){
      case 1: return Color::Blue;
      case 2: return Color::Green;
      case 3: return Color::Red;
      case 4: return Color::BlueLight;
      case 5: return Color::RedLight;
      case 6: return Color::Cyan;
      default: return Color::White;
    }
  }

  class BoardNode : public Node{
    Board& m_board;
    uint32_t m_cx, m_cy;
    Color m_theme;

    public:
    BoardNode(Board& b, uint32_t cx, uint32_t cy, Color theme) : m_board(b), m_cx(cx), m_cy(cy), m_theme(theme) {}

    void ComputeRequirement()override{
      requirement_.min_x = m_board.getWidth() * 3;
      requirement_.min_y = m_board.getHeight();
    }

    void Render(Screen& screen)override{
      for(uint32_t y = 0; y < m_board.getHeight(); ++y){
        for(uint32_t x = 0; x < m_board.getWidth(); ++x){
          int scr_x = box_.x_min + (x * 3);
          int scr_y = box_.y_min + y;

          if(scr_y > box_.y_max) break;
          if(scr_x + 2 > box_.x_max) continue;

          uint8_t hidden = m_board.getHiddenCell(x, y);
          uint8_t visible = m_board.getVisibleCell(x, y);
          bool is_cursor = (x == m_cx && y == m_cy);

          std::string label = " ";
          Color fg = Color::White;
          Color bg;

          bool is_even = (x + y) % 2 == 0;

          if(is_cursor){
            bg = Color::Yellow;
            fg = Color::Black;
          }else if (visible == 1){ 
            bg = is_even ? Color::White : Color::GrayLight;
          }else{ 
            bg = (visible == 2) ? Color::GrayDark : m_theme;
            if(!is_even && visible != 2) bg = Color::Interpolate(0.2f, m_theme, Color::Black);
          }

          if(visible == 2){
            label = "🚩";
            fg = Color::Red;
          }else if(visible == 1){
            if(hidden == 9){
              label = "💣";
              fg = Color::Black;
            }else if(hidden > 0){
              static const std::string nums[] = {" ", "1", "2", "3", "4", "5", "6", "7", "8"};
              label = nums[hidden];
              fg = m_board.GetNumberColor(hidden);
            }
          }

          auto& p_mid = screen.PixelAt(scr_x + 1, scr_y);
          p_mid.character = label;
          p_mid.background_color = bg;
          p_mid.foreground_color = fg;

          auto& p_left = screen.PixelAt(scr_x, scr_y);
          p_left.character = " ";
          p_left.background_color = bg;

          auto& p_right = screen.PixelAt(scr_x + 2, scr_y);
          p_right.character = " ";
          p_right.background_color = bg;
        }
      }
    }
  };
};
