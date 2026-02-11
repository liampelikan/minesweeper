#pragma once
#include "Board.h"
#include "StatManager.h"
#include "UI.h"


enum class GameState { MENU, PLAYING, GAMEOVER, WIN };

class Game {
public:
  Game();
  void Run();
  void UpdateFrame();

private:
  void Update();
  void Draw();
  void HandleInput();
  void ResetGame();

  int screenWidth;
  int screenHeight;

  GameState state;
  Board board;
  UI ui;
  StatManager statManager;

  float lastClickTime = 0.0f;
  int lastX = -1;
  int lastY = -1;

  float sessionTime = 0.0f;
  bool showStats = false;

  // Window dragging
  bool isDragging = false;
  Vector2 dragOffset = {0, 0};
};
