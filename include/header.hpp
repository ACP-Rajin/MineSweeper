#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/component.hpp>

#include "board.hpp"

using namespace ftxui;

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

struct Settings{
  int bannerMode; // Default = 0, Big = 1, Small = 2, Disabled = 3
  int gameMode;   // Easy = 0, Medium = 1, Hard = 2, Custom = 3
  int colorTheme; // Classic = 0, Matrix = 1, Midnight = 2

  int width;
  int height;
  int mineCount;
};

Color getThemeColor(int index){
  if(index == 1) return Color::Green;
  if(index == 2) return Color::Black;

  return Color::Blue;
}

struct Position{
  int x;
  int y;
};

Component NumberInput(int *val, int min, int max, bool isTextCenter = false, const std::string &hint = "value"){
  auto buf = std::make_shared<std::string>(std::to_string(*val));
  InputOption opt;
  opt.on_change = [buf, val, min, max]{
    if(buf->empty()) return;
    std::string filtered;
    for(char c : *buf) if(std::isdigit(c)) filtered += c;
    *buf = filtered;
    try{
      *val = std::clamp(std::stoi(*buf), min, max);
    }catch(...){}
  };

  auto input = Input(buf.get(), hint, opt);
  return Renderer(input, [=]{
    if(!buf->empty() && std::stoi(*buf) != *val) *buf = std::to_string(*val);
    return isTextCenter ? input->Render() | center : input->Render();
  });
}

Element GetBanner(const Settings &settings, ScreenInteractive &screen){
  if(settings.bannerMode == 3) return text("");

  int mode = settings.bannerMode;

  if(mode == 0){
    if(screen.dimx() >= 140) mode = 1;
    else if(screen.dimx() >= 90) mode = 2;
    else return text("");
  }

  if(mode == 1 && screen.dimx() < 140) return text("");
  if(mode == 2 && screen.dimx() < 90) return text("");

  const auto &target = (mode == 1) ? bannerBig : bannerSmall;
  if(target.empty()) return text("");

  Elements lines;
  for(const auto &line : target) lines.emplace_back(text(line));

  return vbox(std::move(lines)) | color(Color::Cyan);
}
