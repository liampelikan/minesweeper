#pragma once
#include "Cell.h"
#include <vector>

class Board {
public:
  Board(int width, int height, int mines);

  void Reset();
  void Reveal(int x, int y);
  void ToggleFlag(int x, int y);
  void Chord(int x, int y);
  bool IsValid(int x, int y) const;

  int GetWidth() const { return width; }
  int GetHeight() const { return height; }
  const Cell &GetCell(int x, int y) const;
  bool IsGameOver() const { return gameOver; }
  bool IsGameWon() const { return gameWon; }
  int GetMinesLeft() const;
  bool IsFirstClick() const { return firstClick; }
  void GetClickedMine(int &x, int &y) const {
    x = clickedMineX;
    y = clickedMineY;
  }
  bool IsSolvable(int startX, int startY);
  void TriggerLose();
  void GenerateNoGuess(int startX, int startY);

private:
  int width;
  int height;
  int totalMines;
  std::vector<std::vector<Cell>> grid;
  bool firstClick;
  bool gameOver;
  bool gameWon;

  int clickedMineX = -1;
  int clickedMineY = -1;

  void PlaceMines(int safeX, int safeY);
  void CalculateNumbers();
  void FloodFill(int x, int y);
  void CheckWinCondition();
  void RevealAllMines();
};
