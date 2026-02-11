#include "Board.h"
#include <algorithm>
#include <ctime>
#include <random>

Board::Board(int width, int height, int mines)
    : width(width), height(height), totalMines(mines), firstClick(true),
      gameOver(false), gameWon(false) {
  grid.resize(height, std::vector<Cell>(width));
}

void Board::Reset() {
  grid.assign(height, std::vector<Cell>(width));
  firstClick = true;
  gameOver = false;
  gameWon = false;
  clickedMineX = -1;
  clickedMineY = -1;
}

const Cell &Board::GetCell(int x, int y) const {
  if (IsValid(x, y)) {
    return grid[y][x];
  }
  static Cell empty;
  return empty;
}

bool Board::IsValid(int x, int y) const {
  return x >= 0 && x < width && y >= 0 && y < height;
}

void Board::ToggleFlag(int x, int y) {
  if (!IsValid(x, y) || gameOver || gameWon)
    return;

  Cell &cell = grid[y][x];
  if (!cell.isRevealed) {
    cell.isFlagged = !cell.isFlagged;
  }
}

void Board::Reveal(int x, int y) {
  if (!IsValid(x, y) || gameOver || gameWon)
    return;

  Cell &cell = grid[y][x];

  if (cell.isFlagged || cell.isRevealed)
    return;

  if (firstClick) {
    PlaceMines(x, y);
    CalculateNumbers();
    firstClick = false;
  }

  if (cell.isMine) {
    cell.isRevealed = true;
    gameOver = true;
    clickedMineX = x;
    clickedMineY = y;
    RevealAllMines();
    return;
  }

  FloodFill(x, y);
  CheckWinCondition();
}

void Board::Chord(int x, int y) {
  if (!IsValid(x, y) || gameOver || gameWon)
    return;

  Cell &cell = grid[y][x];
  if (!cell.isRevealed || cell.neighborMines == 0)
    return;

  int flagCount = 0;
  for (int dy = -1; dy <= 1; dy++) {
    for (int dx = -1; dx <= 1; dx++) {
      if (dx == 0 && dy == 0)
        continue;
      int nx = x + dx;
      int ny = y + dy;
      if (IsValid(nx, ny) && grid[ny][nx].isFlagged) {
        flagCount++;
      }
    }
  }

  if (flagCount == cell.neighborMines) {
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx == 0 && dy == 0)
          continue;
        Reveal(x + dx, y + dy);
      }
    }
  }
}

void Board::FloodFill(int x, int y) {
  if (!IsValid(x, y))
    return;
  Cell &cell = grid[y][x];

  if (cell.isRevealed || cell.isFlagged)
    return;

  cell.isRevealed = true;

  if (cell.neighborMines == 0) {
    for (int dy = -1; dy <= 1; dy++) {
      for (int dx = -1; dx <= 1; dx++) {
        if (dx != 0 || dy != 0) {
          FloodFill(x + dx, y + dy);
        }
      }
    }
  }
}

void Board::PlaceMines(int safeX, int safeY) {
  int minesPlaced = 0;
  std::random_device rd;
  std::mt19937 rng(rd());
  std::uniform_int_distribution<int> distX(0, width - 1);
  std::uniform_int_distribution<int> distY(0, height - 1);

  while (minesPlaced < totalMines) {
    int x = distX(rng);
    int y = distY(rng);

    if (std::abs(x - safeX) <= 1 && std::abs(y - safeY) <= 1)
      continue;

    if (!grid[y][x].isMine) {
      grid[y][x].isMine = true;
      minesPlaced++;
    }
  }
}

void Board::GenerateNoGuess(int startX, int startY) {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      grid[y][x].isMine = false;
      grid[y][x].isFlagged = false;
      grid[y][x].isRevealed = false;
      grid[y][x].neighborMines = 0;
    }
  }
  firstClick = false;

  PlaceMines(startX, startY);
  CalculateNumbers();

  grid[startY][startX].isRevealed = true;
  if (grid[startY][startX].neighborMines == 0) {
    FloodFill(startX, startY);
  }

  int attempts = 0;
  while (!IsSolvable(startX, startY) && attempts < 1000) {
    attempts++;

    std::mt19937 rng(static_cast<unsigned int>(std::time(nullptr)) + attempts);
    std::uniform_int_distribution<int> distX(0, width - 1);
    std::uniform_int_distribution<int> distY(0, height - 1);

    int mX, mY;
    do {
      mX = distX(rng);
      mY = distY(rng);
    } while (!grid[mY][mX].isMine);

    grid[mY][mX].isMine = false;

    int nX, nY;
    do {
      nX = distX(rng);
      nY = distY(rng);
    } while (grid[nY][nX].isMine ||
             (std::abs(nX - startX) <= 1 && std::abs(nY - startY) <= 1));

    grid[nY][nX].isMine = true;

    CalculateNumbers();
  }

  if (attempts >= 1000) {
    PlaceMines(startX, startY);
    CalculateNumbers();
  }

  grid[startY][startX].isRevealed = false;
  if (grid[startY][startX].neighborMines == 0) {
    FloodFill(startX, startY);
  } else {
    grid[startY][startX].isRevealed = true;
  }
}

void Board::CalculateNumbers() {
  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (grid[y][x].isMine)
        continue;

      int mines = 0;
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx == 0 && dy == 0)
            continue;
          int nx = x + dx;
          int ny = y + dy;
          if (IsValid(nx, ny) && grid[ny][nx].isMine) {
            mines++;
          }
        }
      }
      grid[y][x].neighborMines = mines;
    }
  }
}

void Board::RevealAllMines() {
  for (auto &row : grid) {
    for (auto &cell : row) {
      if (cell.isMine) {
        cell.isRevealed = true;
      }
    }
  }
}

void Board::CheckWinCondition() {
  int revealedCount = 0;
  for (const auto &row : grid) {
    for (const auto &cell : row) {
      if (cell.isRevealed)
        revealedCount++;
    }
  }

  if (revealedCount == (width * height - totalMines)) {
    gameWon = true;
    gameOver = true;
    for (auto &row : grid) {
      for (auto &cell : row) {
        if (cell.isMine)
          cell.isFlagged = true;
      }
    }
  }
}

int Board::GetMinesLeft() const {
  int flags = 0;
  for (const auto &row : grid) {
    for (const auto &cell : row) {
      if (cell.isFlagged)
        flags++;
    }
  }
  return totalMines - flags;
}
bool Board::IsSolvable(int startX, int startY) {
  struct SolverCell {
    bool revealed = false;
    bool flagged = false;
  };
  std::vector<std::vector<SolverCell>> solverGrid(
      height, std::vector<SolverCell>(width));

  auto simulateReveal = [&](auto self, int x, int y) -> void {
    if (!IsValid(x, y) || solverGrid[y][x].revealed)
      return;
    solverGrid[y][x].revealed = true;
    if (grid[y][x].neighborMines == 0) {
      for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
          if (dx != 0 || dy != 0)
            self(self, x + dx, y + dy);
        }
      }
    }
  };

  simulateReveal(simulateReveal, startX, startY);

  bool changed = true;
  while (changed) {
    changed = false;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        if (solverGrid[y][x].revealed && grid[y][x].neighborMines > 0) {
          int unrevealed = 0;
          int flags = 0;
          std::vector<std::pair<int, int>> unrevealedCells;

          for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
              int nx = x + dx, ny = y + dy;
              if (IsValid(nx, ny)) {
                if (!solverGrid[ny][nx].revealed &&
                    !solverGrid[ny][nx].flagged) {
                  unrevealed++;
                  unrevealedCells.push_back({nx, ny});
                }
                if (solverGrid[ny][nx].flagged)
                  flags++;
              }
            }
          }

          if (flags == grid[y][x].neighborMines && unrevealed > 0) {
            for (auto p : unrevealedCells) {
              simulateReveal(simulateReveal, p.first, p.second);
              changed = true;
            }
          } else if (unrevealed + flags == grid[y][x].neighborMines &&
                     unrevealed > 0) {
            for (auto p : unrevealedCells) {
              solverGrid[p.second][p.first].flagged = true;
              changed = true;
            }
          }
        }
      }
    }

    if (changed)
      continue;

    for (int y1 = 0; y1 < height; y1++) {
      for (int x1 = 0; x1 < width; x1++) {
        if (!solverGrid[y1][x1].revealed || grid[y1][x1].neighborMines == 0)
          continue;

        std::vector<std::pair<int, int>> neighborsA;
        int flagsA = 0;
        for (int dy = -1; dy <= 1; dy++) {
          for (int dx = -1; dx <= 1; dx++) {
            int nx = x1 + dx, ny = y1 + dy;
            if (IsValid(nx, ny)) {
              if (solverGrid[ny][nx].flagged)
                flagsA++;
              else if (!solverGrid[ny][nx].revealed)
                neighborsA.push_back({nx, ny});
            }
          }
        }

        if (neighborsA.empty())
          continue;
        int minesNeededA = grid[y1][x1].neighborMines - flagsA;

        for (int dy = -2; dy <= 2; dy++) {
          for (int dx = -2; dx <= 2; dx++) {
            int x2 = x1 + dx, y2 = y1 + dy;

            if (!IsValid(x2, y2) || (x1 == x2 && y1 == y2))
              continue;
            if (!solverGrid[y2][x2].revealed || grid[y2][x2].neighborMines == 0)
              continue;

            std::vector<std::pair<int, int>> neighborsB;
            int flagsB = 0;
            for (int dy2 = -1; dy2 <= 1; dy2++) {
              for (int dx2 = -1; dx2 <= 1; dx2++) {
                int nx = x2 + dx2, ny = y2 + dy2;
                if (IsValid(nx, ny)) {
                  if (solverGrid[ny][nx].flagged)
                    flagsB++;
                  else if (!solverGrid[ny][nx].revealed)
                    neighborsB.push_back({nx, ny});
                }
              }
            }

            if (neighborsB.empty())
              continue;
            int minesNeededB = grid[y2][x2].neighborMines - flagsB;

            bool isSubset = true;
            for (auto &pA : neighborsA) {
              bool found = false;
              for (auto &pB : neighborsB) {
                if (pA == pB) {
                  found = true;
                  break;
                }
              }
              if (!found) {
                isSubset = false;
                break;
              }
            }

            if (isSubset) {
              std::vector<std::pair<int, int>> diff; // B - A
              for (auto &pB : neighborsB) {
                bool shared = false;
                for (auto &pA : neighborsA) {
                  if (pA == pB) {
                    shared = true;
                    break;
                  }
                }
                if (!shared)
                  diff.push_back(pB);
              }

              if (diff.empty())
                continue;

              int minesInDiff = minesNeededB - minesNeededA;

              if (minesInDiff == 0) {
                for (auto &p : diff) {
                  if (!solverGrid[p.second][p.first].revealed) {
                    simulateReveal(simulateReveal, p.first, p.second);
                    changed = true;
                  }
                }
              } else if (minesInDiff == diff.size()) {
                for (auto &p : diff) {
                  if (!solverGrid[p.second][p.first].flagged) {
                    solverGrid[p.second][p.first].flagged = true;
                    changed = true;
                  }
                }
              }
            }
          }
        }
      }
    }

    if (changed)
      continue;

    int totalRevealedAndFlagged = 0;
    int currentFlags = 0;
    std::vector<std::pair<int, int>> unknownCells;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        if (solverGrid[y][x].revealed || solverGrid[y][x].flagged)
          totalRevealedAndFlagged++;
        if (solverGrid[y][x].flagged)
          currentFlags++;
        if (!solverGrid[y][x].revealed && !solverGrid[y][x].flagged)
          unknownCells.push_back({x, y});
      }
    }

    int minesLeft = totalMines - currentFlags;
    if (minesLeft == unknownCells.size() && minesLeft > 0) {
      for (auto &p : unknownCells) {
        solverGrid[p.second][p.first].flagged = true;
        changed = true;
      }
    } else if (minesLeft == 0 && !unknownCells.empty()) {
      for (auto &p : unknownCells) {
        simulateReveal(simulateReveal, p.first, p.second);
        changed = true;
      }
    }
  }

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      if (!grid[y][x].isMine && !solverGrid[y][x].revealed)
        return false;
    }
  }
  return true;
}

void Board::TriggerLose() {
  gameOver = true;
  RevealAllMines();
}
