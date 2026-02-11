

#include "Game.h"
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

Game::Game()
    : screenWidth(800), screenHeight(600), board(30, 16, 99),
      statManager("stats.dat"), ui(board, statManager),
      state(GameState::PLAYING) {
  int cellS = 32;
  int boardW = 30 * cellS;
  int boardH = 16 * cellS;

  screenWidth = boardW + 40;
  screenHeight = boardH + 120;

  SetConfigFlags(FLAG_WINDOW_UNDECORATED | FLAG_MSAA_4X_HINT);
  InitWindow(screenWidth, screenHeight, "Minesweeper");

  Image icon = LoadImage("flag.ico");
  if (icon.data != nullptr) {
    SetWindowIcon(icon);
    UnloadImage(icon);
  }

  SetTargetFPS(60);
}

void Game::UpdateFrame() {
  Update();
  Draw();
}

void Game::Run() {
#if defined(PLATFORM_WEB)
  emscripten_set_main_loop_arg(
      [](void *arg) { static_cast<Game *>(arg)->UpdateFrame(); }, this, 0, 1);
#else
  while (!WindowShouldClose()) {
    UpdateFrame();
  }
#endif
  CloseWindow();
}

void Game::ResetGame() {
  if (!board.IsFirstClick() && state == GameState::PLAYING) {
    statManager.RecordIncomplete();
  }
  board.Reset();
  state = GameState::PLAYING;
  sessionTime = 0.0f;
}

void Game::Update() {
  HandleInput();

  if (state == GameState::PLAYING && !board.IsGameOver() &&
      !board.IsGameWon()) {
    if (!board.IsFirstClick()) {
      sessionTime += GetFrameTime();
      if (sessionTime >= 2000.0f) {
        state = GameState::GAMEOVER;
        statManager.RecordGame(false, true, sessionTime, 0);
        board.TriggerLose(); // We need to make sure this exists or simulate it
      }
    }
  } else if (state == GameState::PLAYING) {
    if (board.IsGameWon()) {
      state = GameState::WIN;
      statManager.RecordGame(true, false, sessionTime, 99);
    } else if (board.IsGameOver()) {
      state = GameState::GAMEOVER;
      statManager.RecordGame(false, true, sessionTime, 0);
    }
  }

  ui.Update();
}

void Game::HandleInput() {
  Vector2 mousePos = GetMousePosition();

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (ui.IsOverClose(mousePos)) {
    } else if (ui.IsOverMinimize(mousePos)) {
      MinimizeWindow();
    } else if (ui.IsOverTitleBar(mousePos)) {
      isDragging = true;
      dragOffset = mousePos;
    }
  }

  if (isDragging) {
    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
      Vector2 currentMouse = GetMousePosition();
      Vector2 delta = {currentMouse.x - dragOffset.x,
                       currentMouse.y - dragOffset.y};
      Vector2 windowPos = GetWindowPosition();
      SetWindowPosition((int)(windowPos.x + delta.x),
                        (int)(windowPos.y + delta.y));
      // dragOffset = currentMouse; // Removed to fix jitter
    } else {
      isDragging = false;
    }
  }

  if (ui.IsOverClose(mousePos) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
    exit(0);
  }

  if (IsKeyPressed(KEY_R)) {
    ResetGame();
  }

  if ((board.IsGameOver() || board.IsGameWon())) {
    if (GetKeyPressed() != 0) {
      ResetGame();
    }
  }

  if (IsKeyPressed(KEY_S)) {
    showStats = !showStats;
  }

  if (IsKeyPressed(KEY_G)) {
    statManager.SetNoGuessMode(!statManager.GetNoGuessMode());
  }

  if (showStats || isDragging)
    return;

  int cellSize = 32;
  int offsetX = (screenWidth - board.GetWidth() * cellSize) / 2;
  int offsetY = 95 + 20;
  int gridX = (int)(mousePos.x - offsetX) / cellSize;
  int gridY = (int)(mousePos.y - offsetY) / cellSize;

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
    if (board.IsValid(gridX, gridY)) {
      bool wasFirst = board.IsFirstClick();

      float currentTime = (float)GetTime();
      if (board.GetCell(gridX, gridY).isRevealed) {
        board.Chord(gridX, gridY);
      } else {
        board.Reveal(gridX, gridY);
      }

      if (wasFirst && !board.IsFirstClick()) {
        if (statManager.GetNoGuessMode()) {
          board.GenerateNoGuess(gridX, gridY);
        }
        statManager.RecordStart();
      }

      lastClickTime = currentTime;
      lastX = gridX;
      lastY = gridY;
    }
  } else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    if (board.IsValid(gridX, gridY)) {
      board.ToggleFlag(gridX, gridY);
    }
  } else if (IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) {
    if (board.IsValid(gridX, gridY)) {
      board.Chord(gridX, gridY);
    }
  }
}

void Game::Draw() {
  BeginDrawing();
  ClearBackground(Color{28, 32, 38, 255});
  ui.Draw(sessionTime, showStats);
  EndDrawing();
}
