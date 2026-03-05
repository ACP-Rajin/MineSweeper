#pragma once

#include <ftxui/component/screen_interactive.hpp>
#include <iomanip>
#include <thread>

#include "board.hpp"

using namespace ftxui;

const std::vector<std::string> bannerSmall={
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
}, bannerBig={
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
  int bannerMode; // Default: 0, Big: 1, Small: 2, Disabled: 3
  int difficulty; // Easy: 0, Medium: 1, Hard: 2, Custom: 3
  int colorTheme; // Classic: 0, Matrix: 1, Midnight: 2

  int width;
  int height;
  int mineCount;

  void sync(){
    if(difficulty != 3){
      // Scale: 8x8, 16x16, 24x24
      width = 8 * (difficulty + 1);
      height = 8 * (difficulty + 1);
      mineCount = width * height * (difficulty == 0 ? 0.12f : (difficulty == 1 ? 0.16f : 0.21f));
    }

    if(mineCount >= width * height) mineCount = (width * height) - 1;
    if(mineCount < 1) mineCount = 1;
  }
};

Color getThemeColor(int index){
  if(index == 1) return Color::Green;
  if(index == 2) return Color::Black;

  return Color::Blue;
}

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

Component NumberInput(int *val, int min, int *max, bool isTextCenter = false, const std::string &hint = "value"){
  auto buf = std::make_shared<std::string>(std::to_string(*val));
  InputOption opt;
  opt.on_change = [buf, val, min, max]{
    if(buf->empty()) return;
    std::string filtered;
    for(char c : *buf) if(std::isdigit(c)) filtered += c;
    *buf = filtered;
    try{
      *val = std::clamp(std::stoi(*buf), min, *max);
    }catch(...){}
  };

  auto input = Input(buf.get(), hint, opt);
  return Renderer(input, [=]{
    if(!buf->empty() && std::stoi(*buf) != *val) *buf = std::to_string(*val);
    return isTextCenter ? input->Render() | center : input->Render();
  });
}

Element GetBanner(const Settings &settings){
  if(settings.bannerMode == 3) return text("");

  auto term = Terminal::Size();
  int mode = settings.bannerMode;

  if(mode == 0){
    if(term.dimx >= 140) mode = 1;
    else if(term.dimx >= 90) mode = 2;
    else return text("");
  }

  if(mode == 1 && term.dimx < 140) return text("");
  if(mode == 2 && term.dimx < 90) return text("");

  const auto &target = (mode == 1) ? bannerBig : bannerSmall;
  if(target.empty()) return text("");

  Elements lines;
  for(const auto &line : target) lines.emplace_back(text(line));

  return vbox(std::move(lines)) | color(Color::Cyan);
}
