#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>
#include <iomanip>
#include <thread>

#include "board.hpp"

const std::vector<std::string>bannerSmall={
"                         ██    ██                                                       ",
"                        ████  ████  █                                                 ",
"                       ████████ █                                                 ",
"                       ███  ████  ███     ████   ██  ████████                        ",
"                       ███   ██   ███ ███ █████  ███ ██    ██                       ",
"                       ███          ███ ███ ███ ██ ███ ███████████                       ",
"                       ███          ███ ███ ███  █████ ██                               ",
"   ███████████       ███          ███ ███ ██   ████  ████████                        ",
"  ██       █                                                                            ",
"  ██                                                                                      ",
"   ████      ██        ██  ████████   ████████   ████████   ████████  ████████",
"       ████  ███        ███ ██    ██ ██    ██ ██    ██ ██    ██ ███   ██",
"          ██ ███  ██  ███ ███████████ ███████████ ███      ███ ███████████ ███       ",
"█       ██ ████████ ██         ██         ███     ██ ██         ███       ",
"███████████   ████████   ████████   ████████  ██████████   ████████  ███       ",
"                                                        ███                                 ",
"                                                        ███                                 ",
"                                                        ██                                 "
},bannerBig={
"  ██    ██                                   ███████████                                                                            ",
" ████  ████  █                            ██       █                                                                            ",
"████████ █                            ██                                                                                      ",
"███  ████  ███     ████   ██  ████████    ████      ██        ██  ████████   ████████   ████████   ████████  ████████",
"███   ██   ███ ███ █████  ███ ██    ██       ████  ███        ███ ██    ██ ██    ██ ██    ██ ██    ██ ███   ██",
"███          ███ ███ ███ ██ ███ ███████████          ██ ███  ██  ███ ███████████ ███████████ ███      ███ ███████████ ███       ",
"███          ███ ███ ███  █████ ██        █       ██ ████████ ██         ██         ███     ██ ██         ███       ",
"███          ███ ███ ██   ████  ████████ ███████████   ████████   ████████   ████████  ██████████   ████████  ███       ",
"                                                                                                      ███                                 ",
"                                                                                                      ███                                 ",
"                                                                                                      ██                                 "
};

// --- Settings Struct ---
struct Settings{
  int bannerMode = 0; // 0: DEFAULT, 1: BIG,SMALL, 2: DISABLE
  int difficulty = 0; // 0: Easy, 1: Medium, 2: Hard, 3: Custom
  int theme = 0;      // 0: Classic, 1: Matrix,    2: Midnight
  int width = 8;
  int height = 8;
  int mines = 7;
  bool has_save = false;

  constexpr float getDensity()const{ // Easy: 12%, Medium: 16%, Hard: 21%
    switch(difficulty){
      case 0: return 0.12f;
      case 1: return 0.16f;
      case 2: return 0.21f;
      default: return 0.15f;
    }
  }

  // Updates dimensions and mines based on difficulty
  void sync(){
    if(difficulty != 3){
      // Scale: 8x8, 16x16, 24x24
      width = 8 * (difficulty + 1);
      height = 8 * (difficulty + 1);
      mines = static_cast<int>(width * height * getDensity());
    }
    // Safety check: mines should never exceed total area
    if(mines >= width * height) mines = (width * height) - 1;
    if(mines < 1) mines = 1;
  }

  Color getThemeColor(){
    if(theme == 1) return Color::Green;
    if(theme == 2) return Color::Black;

    return Color::Blue;
  }
};

// --- Banner Logic ---
Element GetBanner(const Settings &s, const std::vector<std::string> &big, const std::vector<std::string> &small){
  if(s.bannerMode == 3) return text("");
  auto term = Terminal::Size();
  int mode = s.bannerMode;

  if(mode == 0){
    if(term.dimx >= 140) mode = 1;
    else if(term.dimx >= 90) mode = 2;
    else return text(""); 
  }
  if(mode == 1 && term.dimx < 140) return text("");
  if(mode == 2 && term.dimx < 90) return text("");

  const auto &target = (mode == 1) ? big : small;
  if(target.empty()) return text("");
  Elements lines;
  for(const auto &line : target) lines.push_back(text(line));

  return vbox(std::move(lines)) | color(Color::Cyan);
}

Component makeInput(std::string *buf, int *val, int min, int max){
  InputOption opt;
  opt.on_change = [buf, val, min, max]{
    std::string filtered;
    for(char c : *buf) if(std::isdigit(c)) filtered += c;
    *buf = filtered;
    try{
      if(!buf->empty()){
        int parsed = std::stoi(*buf);
        *val = std::max(min, std::min(parsed, max));
      }
    }catch(...){}
  };
  return Input(buf, "Num", opt);
};
