#include "header.hpp"

using namespace ftxui;

int main(){
  ScreenInteractive screen = ScreenInteractive::Fullscreen();
  int screen_state = 0; // Main menu = 0, Settings = 1, Game = 2

  // --- Settings ---
  Settings settings;

  // entries
  static std::vector<std::string> banner_list = {"Default", "Big", "Small", "Disabled"},
                                  mode_list = {"Easy", "Medium", "Hard", "Custom"},
                                  theme_list = {"Classic", "Matrix", "Midnight"};

  std:: string w_str = std::to_string(settings.width),
               h_str = std::to_string(settings.height),
               m_str = std::to_string(settings.mines);

  auto banner_drop = Dropdown(&banner_list, &settings.bannerMode),
       mode_drop = Dropdown(&mode_list, &settings.difficulty),
       theme_drop = Dropdown(&theme_list, &settings.theme);

  auto slider_w = Slider("W", &settings.width, 3, 100),
       slider_h = Slider("H", &settings.height, 3, 100),
       slider_m = Slider("M", &settings.mines, 3, 1000);

  auto input_w = makeInput(&w_str, &settings.width, 3, 100),
       input_h = makeInput(&h_str, &settings.height, 3, 100),
       input_m = makeInput(&m_str, &settings.mines, 3, 1000);

  auto back_button = Button("Back to Menu", [&]{ screen_state = 0; }, ButtonOption::Animated());

  auto settings_container = Container::Vertical({
    banner_drop,
    mode_drop,
    theme_drop,
    Container::Horizontal({slider_w, input_w}),
    Container::Horizontal({slider_h, input_h}),
    Container::Horizontal({slider_m, input_m}),
    back_button
  });

  // --- Game ---
  Board gameBoard(settings.width, settings.height, settings.mines);
  std::function<void()> init_game;

  std:: string status_msg = "";
  std::string mode_text;

  uint32_t cursor_pos_x, cursor_pos_y;

  bool flag_mode = false; // Reveal/Pick = false, Flage Mode = true
  bool game_over = false;

  auto restart_btn = Button(" RESTART ", [&]{ init_game(); }, ButtonOption::Animated());

  std::chrono::time_point<std::chrono::steady_clock> start_time;
  int elapsed_seconds = 0;
  bool timer_running = false;
  std::thread timer_thread([&]{
    while(true){
      std::this_thread::sleep_for(std::chrono::seconds(1));
      if(timer_running && screen_state == 2 && !game_over){
        elapsed_seconds++;
        screen.Post(Event::Custom); // Update main loop for time
      }
    }
  });
  timer_thread.detach();

  init_game = [&]{
    // Optional Refresh inputs
    settings.sync();

    w_str = std::to_string(settings.width);
    h_str = std::to_string(settings.height);
    m_str = std::to_string(settings.mines);

    gameBoard = Board(settings.width, settings.height, settings.mines);

    cursor_pos_x = 0;
    cursor_pos_y = 0;
    game_over = false;
    status_msg = "";
    screen_state = 2;
    elapsed_seconds = 0;
    timer_running = false;
    flag_mode = false;

    restart_btn->TakeFocus();
  };

  ButtonOption mode_opt;
  mode_opt.transform = [&](const EntryState& s){
    return text(s.label) | bold | color(flag_mode ? Color::Red : Color::Cyan) | border;
  };

  auto mode_toggle_btn = Button(&mode_text, [&]{ flag_mode = !flag_mode; }, mode_opt);

  // --- Main menu ---
  std::vector<std::string> menu_entries = {"New Game ", "Settings ", "Exit"};
  int menu_selected_idx = 0;
  MenuOption menu_option;
  menu_option.on_enter = [&]{
    if(menu_selected_idx == 0) init_game();
    else if(menu_selected_idx == 1) screen_state = 1;
    else if(menu_selected_idx == 2) screen.Exit();
  };

  auto menu = Menu(&menu_entries, &menu_selected_idx, menu_option);

  auto main_tabs = Container::Tab({
    menu,
    settings_container,
    Container::Vertical({ // For Game
      mode_toggle_btn,
      restart_btn
    })
  }, &screen_state);

  auto main_renderer = Renderer(main_tabs, [&]{
    // Difficulty Logic
    if(settings.difficulty != 3){
      settings.sync();
      w_str = std::to_string(settings.width);
      h_str = std::to_string(settings.height);
      m_str = std::to_string(settings.mines);
    }

    if(!input_w->Focused()) w_str = std::to_string(settings.width);
    if(!input_h->Focused()) h_str = std::to_string(settings.height);
    if(!input_m->Focused()) m_str = std::to_string(settings.mines);

    if(screen_state == 0){ // Menu
      return vbox({
        filler(),
        GetBanner(settings, bannerBig, bannerSmall) | center,
        filler(),
        hbox({filler(), menu->Render() | border, filler()}),
        filler()
      });
    }else if(screen_state == 1){ // Settings
      return vbox({
        text(" SETTINGS ") | bold | center | color(Color::Yellow),
        separator(),
        hbox({
          filler(),
          vbox({
            hbox(text("Banner Style: ") | size(WIDTH, EQUAL, 15), banner_drop->Render() | flex),
            hbox(text("Difficulty:   ") | size(WIDTH, EQUAL, 15), mode_drop->Render() | flex),
            hbox(text("Theme:        ") | size(WIDTH, EQUAL, 15), theme_drop->Render() | flex),
            separator(),
            hbox({vbox({filler(), text("Width:        ") | size(WIDTH, EQUAL, 15), filler()}), vbox({filler(), slider_w->Render(), filler()}) | flex, input_w->Render() | size(WIDTH, EQUAL, 6) | border}),
            hbox({vbox({filler(), text("Height:       ") | size(WIDTH, EQUAL, 15), filler()}), vbox({filler(), slider_h->Render(), filler()}) | flex, input_h->Render() | size(WIDTH, EQUAL, 6) | border}),
            hbox({vbox({filler(), text("Mines:        ") | size(WIDTH, EQUAL, 15), filler()}), vbox({filler(), slider_m->Render(), filler()}) | flex, input_m->Render() | size(WIDTH, EQUAL, 6) | border}),
            filler(),
            back_button->Render() | center
          }) | size(WIDTH, EQUAL, 60),
          filler()
        })
      }) | border | center;
    }else{ // Game
      mode_text = flag_mode ? " 🚩 MODE: FLAG " : " ⛏️ MODE: REVEAL ";

      // Timer String
      std::stringstream ss;
      ss << std::setfill('0') << std::setw(2) << elapsed_seconds / 60 << ":" << std::setw(2) << elapsed_seconds % 60;
      std::string time_str = ss.str();

      int mines_left = (int)settings.mines - (int)gameBoard.getFlagCount();

      // Top Info Bar
      auto info_bar = hbox({
        text(" ⏱️ " + time_str + " ") | border,
        filler(),
        mode_toggle_btn->Render(),
        filler(),
        text(" 💣 " + std::to_string(mines_left) + " ") | border
      }) | size(HEIGHT, EQUAL, 3);

      auto game_view = vbox({
        info_bar | size(HEIGHT, EQUAL, 3),
        filler(),
        gameBoard.Render(cursor_pos_x, cursor_pos_y, settings.getThemeColor()) | center,
        filler(),
        text(game_over ? "GAME OVER" : "Mouse: Tap to Click | F: Toggle Mode | Q: Quit") | center | dim
      });

      if(game_over){
        return dbox({
          game_view,
          vbox({
            filler(),
            hbox({
              filler(),
              vbox({
                text(status_msg) | bold | center |color(status_msg.find("WON") != std::string::npos ? Color::Green : Color::Red),
                separator(), restart_btn->Render() | center,
                text("Press Q to Return to Menu") | center | dim
              }) | borderDouble | bgcolor(Color::Black),
              filler()
            }),
            text(" ") // Small padding at bottom
          })
        });
      }
      return game_view;
    }
  });

  main_renderer |= CatchEvent([&](Event event){
    if(screen_state != 2) return false;

    if(event == Event::Character('q')){
      screen_state = 0;
      return true;
    }

    if(game_over) return false;

    auto handle_reveal = [&]{
      if(!timer_running){
        timer_running = true;
        start_time = std::chrono::steady_clock::now();
      }
      gameBoard.chord(cursor_pos_x, cursor_pos_y);
      if(flag_mode) gameBoard.toggleFlag(cursor_pos_x, cursor_pos_y);
      else gameBoard.reveal(cursor_pos_x, cursor_pos_y);

      if(gameBoard.isLoss()){
        game_over = true;
        timer_running = false;
        status_msg = " KABOOM! YOU LOST! ";
        gameBoard.revealAllMines();
      }else if(gameBoard.isWin()){
        game_over = true;
        timer_running = false;
        status_msg = " CONGRATULATIONS! YOU WON! ";
      }
    };

    // MOUSE LOGIC
    if(event.is_mouse()){
      auto mouse = event.mouse();
      if(mouse.button != Mouse::None && mouse.motion == Mouse::Released){
        int b_w = (int)gameBoard.getWidth() * 3 + 2;
        int b_h = (int)gameBoard.getHeight() + 2;
        int term_w = screen.dimx();
        int term_h = screen.dimy();
        int remaining_h = term_h - (3 + 1 + b_h);
        int top_gap = remaining_h / 2;
        int offset_x = (term_w - b_w) / 2;
        int offset_y = 3 + top_gap;

        if(mouse.x > offset_x && mouse.x < offset_x + b_w - 1 && mouse.y > offset_y && mouse.y < offset_y + b_h - 1){
          cursor_pos_x = (mouse.x - offset_x - 1) / 3;
          cursor_pos_y = (mouse.y - offset_y - 1);

          if(mouse.button == Mouse::Left){
            handle_reveal();
          }else if(mouse.button == Mouse::Right){
            if(!timer_running){
              timer_running = true;
              start_time = std::chrono::steady_clock::now();
            }
            gameBoard.toggleFlag(cursor_pos_x, cursor_pos_y);
          }
          return true;
        }
      }
    }

    // 2. KEYBOARD LOGIC
    if(event == Event::ArrowUp){
      cursor_pos_y = (cursor_pos_y == 0) ? gameBoard.getHeight() - 1 : cursor_pos_y - 1;
      return true;
    }
    if(event == Event::ArrowDown){
      cursor_pos_y = (cursor_pos_y == gameBoard.getHeight() - 1) ? 0 : cursor_pos_y + 1;
      return true;
    }
    if(event == Event::ArrowLeft){
      cursor_pos_x = (cursor_pos_x == 0) ? gameBoard.getWidth() - 1 : cursor_pos_x - 1;
      return true;
    }
    if(event == Event::ArrowRight){
      cursor_pos_x = (cursor_pos_x == gameBoard.getWidth() - 1) ? 0 : cursor_pos_x + 1;
      return true;
    }
    if(event == Event::Character('f')){
      flag_mode = !flag_mode;
      return true;
    }
    if(event == Event::Return){
      handle_reveal();
      return true;
    }

    return false;
  });

  screen.Loop(main_renderer);
}
