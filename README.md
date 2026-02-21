 # 🚩 Terminal MineSweeper

A simple Minesweeper clone for the terminal built with **C++17** and **FTXUI**. Featuring mouse support, custom difficulty scaling, and a responsive TUI.

## Features
 * **Mouse & Keyboard Support:** Play with clicks or arrow keys.
 * **Responsive Layout:** Automatically scales the board based on terminal size.
 * **Custom Difficulty:** Choose from presets (Easy, Medium, Hard) or define your own board dimensions.
 * **No Safe Starts:** Not guaranteed safe zone on your first click.

## 🚀 Building from Source

### Prerequisites
* **CMake** 3.14 or higher
* **C++17** compatible compiler
* **FTXUI** (Automatically fetched via CMake)

### Compilation Steps
1.  **Prepare the build directory:**
    ```bash
    mkdir build
    cd build
    ```

2.  **Configure the project:**
    ```bash
    cmake .. -DCMAKE_BUILD_TYPE=Release
    ```

3.  **Build the executable:**
    You can build directly from the build folder:
    ```bash
    cmake --build . --config Release -j 8
    ```
    *Note: The `-j 8` flag utilizes your 8 CPU cores to speed up compilation.*

## 🎮 How to Play

### Controls
| Action | Mouse | Keyboard |
| :--- | :--- | :--- |
| **Move Cursor** | Hover | `Arrow Keys` |
| **Reveal Cell** | `Left Click` | `Enter` |
| **Flag Cell** | `Right Click` | `F` (Toggle Mode) |
| **Chord (Quick Reveal)** | `Left Click` (on number) | `Enter` (on number) |
| **Quit to Menu** | — | `Q` |

### Rules
* The number on a cell shows how many mines are adjacent to it.
* **Chording:** If you have flagged the correct number of mines around a revealed number, clicking that number again will automatically reveal all remaining adjacent hidden cells.
* Flag all mines and reveal all safe cells to win!

## 🛠️ Project Structure
* `main.cpp`: Entry point and UI state management.
* `Board.hpp`: Game logic, custom rendering node, and board generation.
* `Settings.hpp`: Difficulty and theme configurations.

## ⚖️ License
This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
