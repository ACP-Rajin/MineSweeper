#include "header.hpp"

int main(){
  auto screen = ScreenInteractive::Fullscreen();

  Settings settings;

  Board board;

  int cursorX = 0;
  int cursorY = 0;

  bool flagMode = false;

  auto container = Container::Vertical({
  });

  auto renderer = Renderer(container, [&]{
    return board.Render(cursorX, cursorY, getThemeColor(settings.colorTheme));
  });

  renderer |= CatchEvent([&](Event event){
    if(event == Event::Character('q')) screen.Exit();

    if(event == Event::ArrowUp)    cursorY = (cursorY == 0) ? board.getColumn() - 1 : cursorY - 1;
    if(event == Event::ArrowDown)  cursorY = (cursorY == board.getColumn() - 1) ? 0 : cursorY + 1;
    if(event == Event::ArrowLeft)  cursorX = (cursorX == 0) ? board.getRow() - 1 : cursorX - 1;
    if(event == Event::ArrowRight) cursorX = (cursorX == board.getRow() - 1) ? 0 : cursorX + 1;

    if(event == Event::Character('f')) flagMode = !flagMode;
    if(event == Event::Return) board.reveal(cursorX, cursorY);

    return true;
  });

  screen.Loop(renderer);
}

// Main Menu
// int main(){
//   auto screen = ScreenInteractive::Fullscreen();
//
//   Settings settings{0};
//
//   std::vector<std::string> entry_mainMenu = { "New Game", "Settings", "Exit" };
//
//   int mainMenu_selection = 0;
//
//   auto option_mainMenu = MenuOption::Vertical();
//   option_mainMenu.entries_option.transform = [&](const EntryState &state){
//     Element suffix = text(state.active ? "> " : "  ") | color(Color::CyanLight) | bold;
//     Element prefix = text(state.active ? " <" : "  ") | color(Color::CyanLight) | bold;
//     Element label = text(" " + entry_mainMenu[state.index] + " ") | bold;
//
//     if(state.index == 0) label = label | color(Color::Blue);
//     else if(state.index == 1) label = label | color(Color::GrayDark);
//     else if(state.index == 2) label = label | color(Color::RedLight);
//
//     if(state.active) label = label | bgcolor(Color::White);
//
//     return hbox({ prefix, label, suffix }) | center;
//   };
//
//   auto mainMenu = Menu(&entry_mainMenu, &mainMenu_selection, option_mainMenu);
//
//   auto container = Container::Vertical({
//     mainMenu
//   });
//
//   auto renderer = Renderer(container, [&]{
//     return vbox({
//       filler(),
//       GetBanner(settings, screen) | center,
//       mainMenu->Render() | border | size(WIDTH, EQUAL, 16) | center,
//       filler()
//     });
//   });
//
//   screen.Loop(renderer);
// }

// Settings
// int main(){
//   Settings settings{0};
//
//   std::vector<std::string> entry_bannerMode = { "Default", "Big", "Small", "Disabled" };
//   std::vector<std::string> entry_gameMode = { "Easy", "Medium", "Hard", "Custom" };
//   std::vector<std::string> entry_colorTheme = { "Classic", "Matrix", "Midnight" };
//
//   auto dropdown_bannerMode = Dropdown(&entry_bannerMode, &settings.bannerMode);
//   auto dropdown_gameMode = Dropdown(&entry_gameMode, &settings.gameMode);
//   auto dropdown_colorTheme = Dropdown(&entry_colorTheme, &settings.colorTheme);
//
//   auto slider_width = Slider("W", &settings.width, 1, 200, 1);
//   auto slider_height = Slider("H", &settings.height, 1, 200, 1);
//   auto slider_mineCount = Slider("M", &settings.mineCount, 1, 40000, 1);
//
//   auto input_width = NumberInput(&settings.width, 1, 200, true);
//   auto input_height = NumberInput(&settings.height, 1, 200, true);
//   auto input_mineCount = NumberInput(&settings.mineCount, 1, 40000, true);
//
//   ButtonOption option_button_back;
//   option_button_back.transform = [&](const EntryState &s){
//     return text(s.label) | bold | border;
//   };
//   option_button_back.animated_colors.foreground.Set(Color::GrayLight, Color::CyanLight);
//   option_button_back.animated_colors.background.Set(Color::Interpolate(0.0, Color::Black, Color::Black), Color::Interpolate(0.0, Color::Black, Color::Black)); // To prevent waiting time
//
//   auto button_back = Button("Back to menu", [&]{ /*TODO: IMPLEMENT BACK LOGIC */ }, option_button_back);
//
//   auto container = Container::Vertical({
//     dropdown_bannerMode,
//     dropdown_gameMode,
//     dropdown_colorTheme,
//     Container::Horizontal({ slider_width, input_width }),
//     Container::Horizontal({ slider_height, input_height }),
//     Container::Horizontal({ slider_mineCount, input_mineCount }),
//     button_back
//   });
//
//   auto renderer = Renderer(container, [&]{
//     return vbox({
//       text("Settings") | bold | center | color(Color::Yellow),
//       separator(),
//       hbox(text("Banner Style: ") | size(WIDTH, EQUAL, 15), dropdown_bannerMode->Render() | flex),
//       hbox(text("Difficulty:   ") | size(WIDTH, EQUAL, 15), dropdown_gameMode->Render() | flex),  
//       hbox(text("Theme:        ") | size(WIDTH, EQUAL, 15), dropdown_colorTheme->Render() | flex), 
//       separator(),
//       hbox({vbox({filler(), text("Width:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_width->Render(), filler()}) | flex, input_width->Render() | size(WIDTH, EQUAL, 6) | border}),
//       hbox({vbox({filler(), text("Height:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_height->Render(), filler()}) | flex, input_height->Render() | size(WIDTH, EQUAL, 6) | border}),
//       hbox({vbox({filler(), text("Mines:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_mineCount->Render(), filler()}) | flex, input_mineCount->Render() | size(WIDTH, EQUAL, 6) | border}),
//       separator(),
//       button_back->Render() | center
//     }) | border | size(WIDTH, EQUAL, 50) | center;
//   });
//
//   ScreenInteractive::Fullscreen().Loop(renderer);
// }
