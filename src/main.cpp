#include "header.hpp"

int main(){
  ScreenInteractive screen = ScreenInteractive::Fullscreen();
  int screenState = 0; // Main Menu: 0, Settings: 1, Game: 2

  // — — — — — — —
  //   Settings
  // — — — — — — —
  Settings settings{0};
  int maxMineCount = (settings.width * settings.height) - 1;

  std::vector<std::string> entry_bannerMode = { "Default", "Big", "Small", "Disabled" };
  std::vector<std::string> entry_gameMode = { "Easy", "Medium", "Hard", "Custom" };
  std::vector<std::string> entry_colorTheme = { "Classic", "Matrix", "Midnight" };

  auto dropdown_bannerMode = Dropdown(&entry_bannerMode, &settings.bannerMode);
  auto dropdown_gameMode = Dropdown(&entry_gameMode, &settings.difficulty);
  auto dropdown_colorTheme = Dropdown(&entry_colorTheme, &settings.colorTheme);

  auto slider_width = Slider("W", &settings.width, 1, 200, 1);
  auto slider_height = Slider("H", &settings.height, 1, 200, 1);
  auto slider_mineCount = Slider("M", &settings.mineCount, 1, &maxMineCount, 1);

  auto input_width = NumberInput(&settings.width, 1, 200, true);
  auto input_height = NumberInput(&settings.height, 1, 200, true);
  auto input_mineCount = NumberInput(&settings.mineCount, 1, &maxMineCount, true);

  ButtonOption option_button_back;
  option_button_back.transform = [&](const EntryState &s){
    return text(s.label) | bold | border;
  };
  option_button_back.animated_colors.foreground.Set(Color::GrayLight, Color::CyanLight);
  option_button_back.animated_colors.background.Set(Color::Interpolate(0.0, Color::Black, Color::Black), Color::Interpolate(0.0, Color::Black, Color::Black)); // To prevent waiting time

  auto button_back = Button("Back to menu", [&]{ screenState = 0; }, option_button_back);

  // — — — — — — —
  //     Game
  // — — — — — — —
  Board board;

  int cursorX = 0;
  int cursorY = 0;

  bool gameOver = false;
  bool flagMode = false;

  std::chrono::time_point<std::chrono::steady_clock> startTime;
  int elapsedSeconds = 0;
  bool timerRunning = false;
  std::thread timeThread([&]{
    while(true){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      if(timerRunning && screenState == 2 && !gameOver){
        ++elapsedSeconds;
        screen.Post(Event::Custom);
      }
    }
  });
  timeThread.detach();

  ButtonOption flagToggleButtonOption;
  flagToggleButtonOption.transform = [&](const EntryState entryState){
    return text(flagMode ? " MODE: FLAG " : " MODE: REVEAL ") | bold | color(flagMode ? Color::Red : Color::Cyan);
  };
  flagToggleButtonOption.on_click = [&]{ flagMode = !flagMode; };

  auto flagToggleButton = Button(flagToggleButtonOption);

  ButtonOption restartButtonOption;
  restartButtonOption.transform = [&](const EntryState entryState){
    Element label = text(" RESTART ") | border;
    if(entryState.focused) label |= bold | color(Color::Cyan);

    return label;
  };
  restartButtonOption.on_click = [&]{
    settings.sync();
    board = Board(settings.width, settings.height, settings.mineCount);

    cursorX = 0;
    cursorY = 0;

    gameOver = false;
    screenState = 2;
    elapsedSeconds = 0;
    timerRunning = false;
    flagMode = false;
  };

  auto restartButton = Button(restartButtonOption);

  // — — — — — — —
  //   Main Menu
  // — — — — — — —
  std::vector<std::string> entry_mainMenu = { "New Game", "Settings", "Exit" };

  int mainMenu_selection = 0;

  auto option_mainMenu = MenuOption::Vertical();
  option_mainMenu.entries_option.transform = [&](const EntryState &state){
    Element suffix = text(state.active ? "> " : "  ") | color(Color::CyanLight) | bold;
    Element prefix = text(state.active ? " <" : "  ") | color(Color::CyanLight) | bold;
    Element label = text(" " + entry_mainMenu[state.index] + " ") | bold;

    if(state.index == 0) label = label | color(Color::Blue);
    else if(state.index == 1) label = label | color(Color::GrayDark);
    else if(state.index == 2) label = label | color(Color::RedLight);

    if(state.active) label = label | bgcolor(Color::White);

    return hbox({ prefix, label, suffix }) | center;
  };
  option_mainMenu.on_enter = [&]{
    if(mainMenu_selection == 0) restartButtonOption.on_click();
    else if(mainMenu_selection == 1) screenState = 1;
    else if(mainMenu_selection == 2) screen.Exit();
  };

  auto mainMenu = Menu(&entry_mainMenu, &mainMenu_selection, option_mainMenu);

  auto container = Container::Tab({
    mainMenu,
    Container::Vertical({
      dropdown_bannerMode,
      dropdown_gameMode,
      dropdown_colorTheme,
      Container::Horizontal({ slider_width, input_width }),
      Container::Horizontal({ slider_height, input_height }),
      Container::Horizontal({ slider_mineCount, input_mineCount }),
      button_back
    }),
    Container::Vertical({
      flagToggleButton,
      restartButton
    })
  }, &screenState);

  auto renderer = Renderer(container, [&]{
    if(screenState == 0){
      return vbox({
        filler(),
        GetBanner(settings) | center,
        mainMenu->Render() | border | size(WIDTH, EQUAL, 16) | center,
        filler()
      });
    }else if(screenState == 1){
      settings.sync();
      maxMineCount = (settings.width * settings.height) - 1;

      return vbox({
        text("Settings") | bold | center | color(Color::Yellow),
        separator(),
        hbox(text("Banner Style: ") | size(WIDTH, EQUAL, 15) | center, dropdown_bannerMode->Render() | flex),
        hbox(text("Difficulty:   ") | size(WIDTH, EQUAL, 15) | center, dropdown_gameMode->Render() | flex),  
        hbox(text("Theme:        ") | size(WIDTH, EQUAL, 15) | center, dropdown_colorTheme->Render() | flex), 
        separator(),
        hbox({vbox({filler(), text("Width:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_width->Render(), filler()}) | flex, input_width->Render() | size(WIDTH, EQUAL, 6) | border}),
        hbox({vbox({filler(), text("Height:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_height->Render(), filler()}) | flex, input_height->Render() | size(WIDTH, EQUAL, 6) | border}),
        hbox({vbox({filler(), text("Mines:") | size(WIDTH, EQUAL, 14), filler()}), vbox({filler(), slider_mineCount->Render(), filler()}) | flex, input_mineCount->Render() | size(WIDTH, EQUAL, 6) | border}),
        separator(),
        button_back->Render() | center
      }) | border | size(WIDTH, EQUAL, 50) | center;
    }else{
      std::stringstream stringStream;
      stringStream << std::setfill('0') << std::setw(2) << elapsedSeconds / 60 << ":" << std::setw(2) << elapsedSeconds % 60;
      std::string timeString = stringStream.str();

      auto gameView = vbox({
        hbox({
          text(" Time: " + timeString + " ") | border,
          filler(),
          flagToggleButton->Render() | border | center,
          filler(),
          text(" Mines: " + std::to_string(board.getMineCount() - board.getFlagCount()) + " ") | border
        }),
        filler(),
        board.Render(cursorX, cursorY, getThemeColor(settings.colorTheme)) | center,
        filler(),
        text("Mouse: Click | F: Toggle Mode | Q: Quit") | center | dim
      });

      if(gameOver){
        restartButton->TakeFocus();
        return dbox({
          gameView,
          vbox({
            filler(),
            vbox({
              board.isWin() ? text("!YOU WIN!") | center | bold | color(Color::Green) : text("YOU LOSE!") | center | bold | color(Color::Red),
              separator(),
              restartButton->Render() | center,
              text("Press Q to return to menu") | center | dim
            }) | borderDouble | bgcolor(Color::Black) | size(WIDTH, EQUAL, 27) | center,
            text("")
          })
        });
      }

      return gameView;
    }
  });

  renderer |= CatchEvent([&](Event event){
    if(screenState != 2) return false;

    if(event == Event::Character('q')){
      screenState = 0;
      return true;
    }

    if(gameOver) return false;

    // Mouse Logic
    if(event.is_mouse()){
      auto mouse = event.mouse();
      if(mouse.button != Mouse::None && mouse.motion == Mouse::Released){
        int b_w = (int)board.getWidth() + 2;
        int b_h = (int)board.getHeight() + 2;
        int term_w = screen.dimx();
        int term_h = screen.dimy();
        int remaining_h = term_h - (3 + 1 + b_h);
        int top_gap = remaining_h / 2;
        int offset_x = (term_w - b_w) / 2;
        int offset_y = 3 + top_gap;

        if(mouse.x > offset_x && mouse.x < offset_x + b_w - 1 && mouse.y > offset_y && mouse.y < offset_y + b_h - 1){
          cursorX = (mouse.x - offset_x - 1) / 3;
          cursorY = (mouse.y - offset_y - 1);

          if(mouse.button == Mouse::Left){
            goto REVEAL_LOGIC;
          }else if(mouse.button == Mouse::Right){
            board.chord(cursorX, cursorY);
            if(flagMode){
              board.reveal(cursorX, cursorY);
            }else{
              board.toggleFlag(cursorX, cursorY);
            }

            if(!timerRunning){
              timerRunning = true;
              startTime = std::chrono::steady_clock::now();
            }

            if(board.isLoss()){
              gameOver = true;
              timerRunning = false;
              board.revealAllMine();
            }else if(board.isWin()){
              gameOver = true;
              timerRunning = false;
            }
          }
          return true;
        }
      }
    }

    // Keyboard logic
    if(event == Event::ArrowUp){
      cursorY = (cursorY == 0) ? board.getRow() - 1 : cursorY - 1;
      return true;
    }
    if(event == Event::ArrowDown){
      cursorY = (cursorY == board.getRow() - 1) ? 0 : cursorY + 1;
      return true;
    }
    if(event == Event::ArrowLeft){
      cursorX = (cursorX == 0) ? board.getColumn() - 1 : cursorX - 1;
      return true;
    }
    if(event == Event::ArrowRight){
      cursorX = (cursorX == board.getColumn() - 1) ? 0 : cursorX + 1;
      return true;
    }
    if(event == Event::Character('f')){
      flagMode = !flagMode;
      return true;
    }
    if(event == Event::Return){
      REVEAL_LOGIC:

      board.chord(cursorX, cursorY);
      if(flagMode){
        board.toggleFlag(cursorX, cursorY);
      }else{
        board.reveal(cursorX, cursorY);
      }

      if(!timerRunning){
        timerRunning = true;
        startTime = std::chrono::steady_clock::now();
      }

      if(board.isLoss()){
        gameOver = true;
        timerRunning = false;
        board.revealAllMine();
      }else if(board.isWin()){
        gameOver = true;
        timerRunning = false;
      }

      return true;
    }

    return false;
  });

  screen.Loop(renderer);
}
