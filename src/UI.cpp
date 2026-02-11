#include "UI.h"
#include <iomanip>
#include <sstream>
#include <string>


UI::UI(Board &board, StatManager &stats) : board(board), stats(stats) {
  Image img = LoadImage("flag.ico");
  if (img.data != nullptr) {
    mineTexture = LoadTextureFromImage(img);
    UnloadImage(img);
    textureLoaded = true;
  }
}

UI::~UI() {
  if (textureLoaded) {
    UnloadTexture(mineTexture);
  }
}

void UI::Update() {}

void UI::Draw(float currentTime, bool showStats) {
  DrawCustomTitleBar();
  DrawStatusHeader(currentTime);

  int offsetX = (GetScreenWidth() - board.GetWidth() * cellSize) / 2;
  int offsetY = topBarHeight + 20;

  DrawRectangle(offsetX - 10, offsetY - 10, board.GetWidth() * cellSize + 20,
                board.GetHeight() * cellSize + 20, Color{33, 37, 43, 255});

  for (int y = 0; y < board.GetHeight(); y++) {
    for (int x = 0; x < board.GetWidth(); x++) {
      DrawCell(x, y, offsetX, offsetY);
    }
  }

  if (board.IsGameWon()) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Color{0, 255, 0, 40});
    DrawText("VICTORY!", GetScreenWidth() / 2 - MeasureText("VICTORY!", 40) / 2,
             offsetY + board.GetHeight() * cellSize / 2 - 40, 40, GREEN);
    DrawText("Press any key to restart",
             GetScreenWidth() / 2 -
                 MeasureText("Press any key to restart", 20) / 2,
             offsetY + board.GetHeight() * cellSize / 2 + 10, 20, RAYWHITE);
  } else if (board.IsGameOver()) {
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(),
                  Color{255, 0, 0, 40});
    DrawText("GAME OVER",
             GetScreenWidth() / 2 - MeasureText("GAME OVER", 40) / 2,
             offsetY + board.GetHeight() * cellSize / 2 - 40, 40, RED);
    DrawText("Press any key to restart",
             GetScreenWidth() / 2 -
                 MeasureText("Press any key to restart", 20) / 2,
             offsetY + board.GetHeight() * cellSize / 2 + 10, 20, RAYWHITE);
  }

  if (showStats) {
    DrawStatsOverlay();
  }
}

void UI::DrawCustomTitleBar() {
  DrawRectangle(0, 0, GetScreenWidth(), titleBarHeight, Color{20, 24, 30, 255});

  const char *title = "Minesweeper";
  int fontSize = 18;
  DrawText(title, GetScreenWidth() / 2 - MeasureText(title, fontSize) / 2,
           (titleBarHeight - fontSize) / 2, fontSize, RAYWHITE);

  Color minColor = IsOverMinimize(GetMousePosition()) ? LIGHTGRAY : GRAY;
  DrawRectangle(GetScreenWidth() - 70, 0, 35, titleBarHeight,
                IsOverMinimize(GetMousePosition()) ? Color{60, 60, 60, 255}
                                                   : BLANK);
  DrawRectangle(GetScreenWidth() - 60, titleBarHeight / 2, 15, 2, minColor);

  bool overClose = IsOverClose(GetMousePosition());
  DrawRectangle(GetScreenWidth() - 35, 0, 35, titleBarHeight,
                overClose ? RED : BLANK);
  DrawLineEx({(float)GetScreenWidth() - 25, 10},
             {(float)GetScreenWidth() - 10, (float)titleBarHeight - 10}, 2.0f,
             overClose ? RAYWHITE : GRAY);
  DrawLineEx({(float)GetScreenWidth() - 10, 10},
             {(float)GetScreenWidth() - 25, (float)titleBarHeight - 10}, 2.0f,
             overClose ? RAYWHITE : GRAY);
}

void UI::DrawStatusHeader(float currentTime) {
  DrawRectangle(0, titleBarHeight, GetScreenWidth(), statusHeaderHeight,
                Color{33, 37, 43, 255});
  DrawLine(0, topBarHeight, GetScreenWidth(), topBarHeight, DARKGRAY);

  std::stringstream ss;
  ss << std::fixed << std::setprecision(1) << currentTime << "s";
  std::string timeStr = ss.str();
  DrawText(timeStr.c_str(),
           GetScreenWidth() / 2 - MeasureText(timeStr.c_str(), 30) / 2,
           titleBarHeight + 15, 30, RAYWHITE);

  std::string minesText = "MINES: " + std::to_string(board.GetMinesLeft());
  DrawText(minesText.c_str(), 30, titleBarHeight + 20, 20,
           Color{180, 180, 180, 255});

  std::string helpText = "'S' Stats | 'R' Restart | 'G' No-Guess: ";
  helpText += (stats.GetNoGuessMode() ? "ON" : "OFF");
  DrawText(helpText.c_str(), GetScreenWidth() - 380, titleBarHeight + 20, 18,
           Color{150, 150, 150, 255});
}

bool UI::IsOverClose(Vector2 mouse) const {
  return (mouse.x >= GetScreenWidth() - 35 && mouse.x < GetScreenWidth() &&
          mouse.y >= 0 && mouse.y < titleBarHeight);
}

bool UI::IsOverMinimize(Vector2 mouse) const {
  return (mouse.x >= GetScreenWidth() - 70 && mouse.x < GetScreenWidth() - 35 &&
          mouse.y >= 0 && mouse.y < titleBarHeight);
}

bool UI::IsOverTitleBar(Vector2 mouse) const {
  return (mouse.y >= 0 && mouse.y < titleBarHeight && !IsOverClose(mouse) &&
          !IsOverMinimize(mouse));
}

void UI::DrawCell(int x, int y, int offsetX, int offsetY) {
  int posX = offsetX + x * cellSize;
  int posY = offsetY + y * cellSize;
  const Cell &cell = board.GetCell(x, y);

  float roundness = 0.2f;
  int segments = 8;
  Rectangle rect = {(float)posX + 2, (float)posY + 2, (float)cellSize - 4,
                    (float)cellSize - 4};

  if (!cell.isRevealed) {
    DrawRectangleRounded(rect, roundness, segments, Color{45, 50, 60, 255});
    DrawRectangleRoundedLines(rect, roundness, segments, 1.0f,
                              Color{60, 65, 75, 255});

    if (cell.isFlagged) {
      DrawFlag(posX + cellSize / 2, posY + cellSize / 2, cellSize / 2);
    }
  } else {
    int clickedX, clickedY;
    board.GetClickedMine(clickedX, clickedY);

    if (cell.isMine && x == clickedX && y == clickedY) {
      DrawRectangleRounded(rect, roundness, segments, Color{255, 50, 50, 220});
    } else {
      DrawRectangleRounded(rect, roundness, segments, Color{28, 32, 38, 255});
    }

    if (cell.isMine) {
      DrawMine(posX + cellSize / 2, posY + cellSize / 2, cellSize / 2);
      if (cell.isFlagged) {
        DrawLineEx({(float)posX + 5, (float)posY + 5},
                   {(float)posX + cellSize - 5, (float)posY + cellSize - 5},
                   2.0f, GREEN);
        DrawLineEx({(float)posX + cellSize - 5, (float)posY + 5},
                   {(float)posX + 5, (float)posY + cellSize - 5}, 2.0f, GREEN);
      }
    } else if (cell.neighborMines > 0) {
      Color color = GetNumberColor(cell.neighborMines);
      std::string text = std::to_string(cell.neighborMines);
      int width = MeasureText(text.c_str(), 20);
      DrawText(text.c_str(), posX + (cellSize - width) / 2, posY + 6, 20,
               color);
    }
  }
}

void UI::DrawMine(int cx, int cy, int size) {
  if (textureLoaded) {
    Rectangle source = {0.0f, 0.0f, (float)mineTexture.width,
                        (float)mineTexture.height};
    Rectangle dest = {(float)cx, (float)cy, (float)size, (float)size};
    Vector2 origin = {(float)size / 2.0f, (float)size / 2.0f};
    DrawTexturePro(mineTexture, source, dest, origin, 0.0f, WHITE);
  } else {
    DrawCircle(cx, cy, size / 2.5f, BLACK);
    float spikeLen = size / 2.0f;
    for (int i = 0; i < 8; i++) {
      float angle = i * PI / 4.0f;
      DrawLineEx({(float)cx, (float)cy},
                 {(float)cx + cos(angle) * spikeLen,
                  (float)cy + sin(angle) * spikeLen},
                 2.0f, BLACK);
    }
    DrawCircle(cx - size / 7, cy - size / 7, size / 10, GRAY);
  }
}

void UI::DrawFlag(int cx, int cy, int size) {
  float baseW = size * 0.7f;
  float poleH = size * 1.1f;

  DrawRectangle(cx - baseW / 2, cy + poleH / 2 - 4, baseW, 4, BLACK);
  DrawRectangle(cx - baseW / 4, cy + poleH / 2 - 8, baseW / 2, 4, BLACK);

  DrawRectangle(cx - 2, cy - poleH / 2, 4, poleH, BLACK);

  Vector2 top = {(float)cx, (float)cy - poleH / 2};
  Vector2 bottom = {(float)cx, (float)cy - poleH / 2 + size * 0.5f};
  Vector2 tip = {(float)cx - size * 0.5f, (float)cy - poleH / 2 + size * 0.25f};
  DrawTriangle(bottom, top, tip, RED);
}

void UI::DrawStatsOverlay() {
  int w = 450;
  int h = 350;
  int x = (GetScreenWidth() - w) / 2;
  int y = (GetScreenHeight() - h) / 2;

  DrawRectangle(x - 5, y - 5, w + 10, h + 10, Color{0, 0, 0, 200});
  DrawRectangleRounded({(float)x, (float)y, (float)w, (float)h}, 0.1f, 8,
                       Color{45, 50, 60, 255});
  DrawRectangleRoundedLines({(float)x, (float)y, (float)w, (float)h}, 0.1f, 8,
                            2.0f, DARKGRAY);

  DrawText("STATISTICS", x + (w - MeasureText("STATISTICS", 30)) / 2, y + 20,
           30, SKYBLUE);

  auto data = stats.GetData();
  auto getPerc = [&](int val) {
    if (data.gamesStarted == 0)
      return 0.0f;
    return (val * 100.0f) / data.gamesStarted;
  };

  int incomplete = data.gamesStarted - data.gamesWon - data.gamesLost;
  if (incomplete < 0)
    incomplete = 0;

  std::stringstream s1, s2, s3, s4;
  s1 << "Games Played:  " << data.gamesStarted;
  s2 << "Games Won:     " << data.gamesWon << " (" << std::fixed
     << std::setprecision(1) << getPerc(data.gamesWon) << "%)";
  s3 << "Games Lost:    " << data.gamesLost << " (" << std::fixed
     << std::setprecision(1) << getPerc(data.gamesLost) << "%)";
  s4 << "Incomplete:    " << incomplete << " (" << std::fixed
     << std::setprecision(1) << getPerc(incomplete) << "%)";

  std::stringstream f, s, a;
  f << "Fastest Time: " << std::fixed << std::setprecision(1)
    << stats.GetFastestTime() << "s";
  s << "Slowest Time: " << std::fixed << std::setprecision(1)
    << stats.GetSlowestTime() << "s";
  a << "Average Time: " << std::fixed << std::setprecision(1)
    << stats.GetAverageTime() << "s";

  int textY = y + 70;
  DrawText(s1.str().c_str(), x + 40, textY, 20, RAYWHITE);
  textY += 30;
  DrawText(s2.str().c_str(), x + 40, textY, 20, GREEN);
  textY += 30;
  DrawText(s3.str().c_str(), x + 40, textY, 20, RED);
  textY += 30;
  DrawText(s4.str().c_str(), x + 40, textY, 20, ORANGE);
  textY += 40;

  DrawText(f.str().c_str(), x + 40, textY, 18, LIGHTGRAY);
  textY += 25;
  DrawText(s.str().c_str(), x + 40, textY, 18, LIGHTGRAY);
  textY += 25;
  DrawText(a.str().c_str(), x + 40, textY, 18, LIGHTGRAY);

  DrawText("Press 'S' to close",
           x + (w - MeasureText("Press 'S' to close", 15)) / 2, y + h - 30, 15,
           GRAY);
}

Color UI::GetNumberColor(int number) {
  switch (number) {
  case 1:
    return Color{99, 151, 255, 255};
  case 2:
    return Color{80, 250, 123, 255};
  case 3:
    return Color{255, 85, 85, 255};
  case 4:
    return Color{189, 147, 249, 255};
  case 5:
    return Color{255, 184, 108, 255};
  case 6:
    return Color{139, 233, 253, 255};
  case 7:
    return Color{255, 121, 198, 255};
  case 8:
    return Color{241, 250, 140, 255};
  default:
    return RAYWHITE;
  }
}
