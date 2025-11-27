//==============================================================================
// example_minesweeper.cpp - Console-based Minesweeper using libattoboy
//==============================================================================
// A classic Minesweeper game with mouse controls and colorful display.
// Uses a 16x16 grid with 40 mines. Click to reveal, Ctrl+Click to flag.
//
// Controls:
//   Left-click        - Reveal a cell
//   Ctrl+Left-click   - Toggle flag on a cell
//   'r' or 'R'        - Restart the game
//   'q' or 'Q' / Esc  - Quit
//
// Build with libattoboy (no CRT/STL dependencies).
//==============================================================================

#include <attoboy/attoboy.h>

using namespace attoboy;

// Grid dimensions and mine count
static const int GRID_SIZE = 16;
static const int MINE_COUNT = 40;

// Cell states
static const int CELL_HIDDEN = 0;
static const int CELL_REVEALED = 1;
static const int CELL_FLAGGED = 2;

// Game state
struct GameState {
  int grid[GRID_SIZE][GRID_SIZE];  // -1 = mine, 0-8 = adjacent mine count
  int state[GRID_SIZE][GRID_SIZE]; // CELL_HIDDEN, CELL_REVEALED, CELL_FLAGGED
  bool gameOver;
  bool won;
  bool firstClick;
  int flagsPlaced;
  int cellsRevealed;
  int hoverX; // Hovered grid X position (-1 if none)
  int hoverY; // Hovered grid Y position (-1 if none)
};

// Initialize the game board
void initGame(GameState &game) {
  // Clear the grid
  for (int y = 0; y < GRID_SIZE; y++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      game.grid[y][x] = 0;
      game.state[y][x] = CELL_HIDDEN;
    }
  }
  game.gameOver = false;
  game.won = false;
  game.firstClick = true;
  game.flagsPlaced = 0;
  game.cellsRevealed = 0;
  game.hoverX = -1;
  game.hoverY = -1;
}

// Place mines avoiding the first click position
void placeMines(GameState &game, int avoidX, int avoidY) {
  int placed = 0;
  while (placed < MINE_COUNT) {
    int x = Math::RandomRange(0, GRID_SIZE);
    int y = Math::RandomRange(0, GRID_SIZE);

    // Avoid placing mine at or near the first click
    int dx = Math::Abs(x - avoidX);
    int dy = Math::Abs(y - avoidY);
    if (dx <= 1 && dy <= 1)
      continue;

    // Don't place mine if already there
    if (game.grid[y][x] == -1)
      continue;

    game.grid[y][x] = -1;
    placed++;
  }

  // Calculate adjacent mine counts
  for (int y = 0; y < GRID_SIZE; y++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      if (game.grid[y][x] == -1)
        continue;

      int count = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          int nx = x + dx;
          int ny = y + dy;
          if (nx >= 0 && nx < GRID_SIZE && ny >= 0 && ny < GRID_SIZE) {
            if (game.grid[ny][nx] == -1)
              count++;
          }
        }
      }
      game.grid[y][x] = count;
    }
  }
}

// Flood-fill reveal empty cells
void revealCell(GameState &game, int x, int y) {
  if (x < 0 || x >= GRID_SIZE || y < 0 || y >= GRID_SIZE)
    return;
  if (game.state[y][x] != CELL_HIDDEN)
    return;

  game.state[y][x] = CELL_REVEALED;
  game.cellsRevealed++;

  // If it's a mine, game over
  if (game.grid[y][x] == -1) {
    game.gameOver = true;
    return;
  }

  // If it's an empty cell (0 adjacent mines), reveal neighbors
  if (game.grid[y][x] == 0) {
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx != 0 || dy != 0) {
          revealCell(game, x + dx, y + dy);
        }
      }
    }
  }
}

// Check if the player has won
void checkWin(GameState &game) {
  int totalSafe = GRID_SIZE * GRID_SIZE - MINE_COUNT;
  if (game.cellsRevealed == totalSafe) {
    game.won = true;
    game.gameOver = true;
  }
}

// Get color for a number
ConsoleColor getNumberColor(int num) {
  switch (num) {
  case 1:
    return CON_BLUE;
  case 2:
    return CON_GREEN;
  case 3:
    return CON_RED;
  case 4:
    return CON_DARK_BLUE;
  case 5:
    return CON_DARK_RED;
  case 6:
    return CON_CYAN;
  case 7:
    return CON_DARK_MAGENTA;
  case 8:
    return CON_DARK_GRAY;
  default:
    return CON_WHITE;
  }
}

// Draw the game board
void drawBoard(Console &con, GameState &game, int offsetX, int offsetY) {
  // Draw title
  con.setCursor(offsetX, offsetY - 2);
  con.print(" MINESWEEPER ", CON_YELLOW, CON_DARK_BLUE);
  con.print("  Mines: ", CON_GRAY);
  con.print(String(MINE_COUNT - game.flagsPlaced), CON_YELLOW);
  con.print("  [R]estart [Q]uit", CON_DARK_GRAY);

  // Draw top border
  con.setCursor(offsetX, offsetY);
  con.print("+", CON_DARK_CYAN);
  for (int x = 0; x < GRID_SIZE; x++) {
    con.print("--", CON_DARK_CYAN);
  }
  con.print("-+", CON_DARK_CYAN);

  // Draw grid
  for (int y = 0; y < GRID_SIZE; y++) {
    con.setCursor(offsetX, offsetY + 1 + y);
    con.print("| ", CON_DARK_CYAN);

    for (int x = 0; x < GRID_SIZE; x++) {
      int cellState = game.state[y][x];
      int cellValue = game.grid[y][x];

      // Check if this cell is being hovered
      bool isHovered = (x == game.hoverX && y == game.hoverY);

      if (cellState == CELL_HIDDEN) {
        if (isHovered) {
          con.print("\xB2", CON_WHITE,
                    CON_DARK_CYAN);            // Highlighted hover symbol
          con.print(" ", CON_GRAY, CON_BLACK); // Normal space
        } else {
          con.print("\xB2 ", CON_GRAY); // Shaded block
        }
      } else if (cellState == CELL_FLAGGED) {
        if (isHovered) {
          con.print("\xF0", CON_YELLOW,
                    CON_DARK_CYAN);           // Highlighted flag symbol
          con.print(" ", CON_RED, CON_BLACK); // Normal space
        } else {
          con.print("\xF0 ", CON_RED); // Flag symbol
        }
      } else { // REVEALED
        if (cellValue == -1) {
          con.print("* ", CON_RED, CON_DARK_RED); // Mine
        } else if (cellValue == 0) {
          con.print("  ", CON_WHITE); // Empty
        } else {
          con.print(String(cellValue) + " ", getNumberColor(cellValue));
        }
      }
    }
    con.print("|", CON_DARK_CYAN);
  }

  // Draw bottom border
  con.setCursor(offsetX, offsetY + 1 + GRID_SIZE);
  con.print("+", CON_DARK_CYAN);
  for (int x = 0; x < GRID_SIZE; x++) {
    con.print("--", CON_DARK_CYAN);
  }
  con.print("-+", CON_DARK_CYAN);

  // Game status message
  con.setCursor(offsetX, offsetY + GRID_SIZE + 3);
  if (game.gameOver) {
    if (game.won) {
      con.print("  YOU WIN! Press R to play again.  ", CON_BLACK, CON_GREEN);
    } else {
      con.print("  GAME OVER! Press R to play again.  ", CON_WHITE, CON_RED);
    }
  } else {
    con.print("  Left-click: Reveal | Ctrl+Click: Flag  ", CON_DARK_GRAY);
  }
}

// Reveal all mines (when game is lost)
void revealAllMines(GameState &game) {
  for (int y = 0; y < GRID_SIZE; y++) {
    for (int x = 0; x < GRID_SIZE; x++) {
      if (game.grid[y][x] == -1) {
        game.state[y][x] = CELL_REVEALED;
      }
    }
  }
}

extern "C" void atto_main() {
  Console con;
  GameState game;

  initGame(game);

  // Calculate board position (center it)
  int boardWidth = GRID_SIZE * 2 + 4;
  int boardHeight = GRID_SIZE + 6;
  int offsetX = Math::Max(0, (con.width() - boardWidth) / 2);
  int offsetY = Math::Max(2, (con.height() - boardHeight) / 2);

  con.clear();
  con.hideCursor();
  drawBoard(con, game, offsetX, offsetY);

  bool running = true;
  while (running) {
    String key = con.readEvent();

    // Handle keyboard input
    if (key == "q" || key == "Q" || key == "Escape") {
      running = false;
    } else if (key == "r" || key == "R") {
      initGame(game);
      con.clear();
      drawBoard(con, game, offsetX, offsetY);
    }
    // Handle mouse input
    else if (key.startsWith("MouseMove")) {
      // Parse mouse move event: "MouseMove@X,Y"
      int atPos = key.getPositionOf("@");
      if (atPos > 0) {
        String coords = key.substring(atPos + 1);
        int commaPos = coords.getPositionOf(",");

        if (commaPos > 0) {
          int mouseX = coords.substring(0, commaPos).toInteger();
          int mouseY = coords.substring(commaPos + 1).toInteger();

          // Convert screen coordinates to grid coordinates
          int gridX = (mouseX - offsetX - 2) / 2;
          int gridY = mouseY - offsetY - 1;

          // Update hover position if within grid bounds
          if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 &&
              gridY < GRID_SIZE) {
            if (game.hoverX != gridX || game.hoverY != gridY) {
              game.hoverX = gridX;
              game.hoverY = gridY;
              drawBoard(con, game, offsetX, offsetY);
            }
          } else {
            // Clear hover if outside grid
            if (game.hoverX != -1 || game.hoverY != -1) {
              game.hoverX = -1;
              game.hoverY = -1;
              drawBoard(con, game, offsetX, offsetY);
            }
          }
        }
      }
    } else if (key.startsWith("Mouse")) {
      // Parse mouse event: "MouseLeft@X,Y" or "MouseRight@X,Y"
      int atPos = key.getPositionOf("@");
      if (atPos > 0) {
        String eventType = key.substring(0, atPos);
        String coords = key.substring(atPos + 1);
        int commaPos = coords.getPositionOf(",");

        if (commaPos > 0) {
          int mouseX = coords.substring(0, commaPos).toInteger();
          int mouseY = coords.substring(commaPos + 1).toInteger();

          // Convert screen coordinates to grid coordinates
          // Account for border and offset: "| " at start means +2
          int gridX = (mouseX - offsetX - 2) / 2;
          int gridY = mouseY - offsetY - 1;

          // Check if click is within grid bounds
          if (gridX >= 0 && gridX < GRID_SIZE && gridY >= 0 &&
              gridY < GRID_SIZE && !game.gameOver) {

            if (eventType == "MouseLeft") {
              // Left click - reveal cell
              if (game.state[gridY][gridX] == CELL_HIDDEN) {
                // First click - place mines avoiding this spot
                if (game.firstClick) {
                  placeMines(game, gridX, gridY);
                  game.firstClick = false;
                }

                revealCell(game, gridX, gridY);

                if (game.gameOver && !game.won) {
                  revealAllMines(game);
                } else {
                  checkWin(game);
                }
              }
            } else if (eventType == "MouseRight") {
              // Right click - toggle flag
              if (game.state[gridY][gridX] == CELL_HIDDEN) {
                game.state[gridY][gridX] = CELL_FLAGGED;
                game.flagsPlaced++;
              } else if (game.state[gridY][gridX] == CELL_FLAGGED) {
                game.state[gridY][gridX] = CELL_HIDDEN;
                game.flagsPlaced--;
              }
            }

            drawBoard(con, game, offsetX, offsetY);
          }
        }
      }
    }
  }

  con.showCursor();
  con.clear();
}
