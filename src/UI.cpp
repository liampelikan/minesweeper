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

void UI::Update(float currentTime) {
  if (!board.IsGameWon()) {
    highscoreEntered = false;
  }

  if (board.IsGameWon() && !enteringName && !highscoreEntered &&
      stats.IsNewHighScore(currentTime)) {
    enteringName = true;
    lastTimeRecord = currentTime;
    nameCharCount = 0;
    playerName[0] = '\0';
    invalidName = false;
    currentRank = stats.GetRankForTime(currentTime);
  }

  if (enteringName) {
    int key = GetCharPressed();
    while (key > 0) {
      if ((key >= 32) && (key <= 125) && (nameCharCount < 15)) {
        playerName[nameCharCount] = (char)key;
        playerName[nameCharCount + 1] = '\0';
        nameCharCount++;
      }
      key = GetCharPressed();
    }

    if (IsKeyDown(KEY_BACKSPACE)) {
      backspaceTimer += GetFrameTime();
      if (IsKeyPressed(KEY_BACKSPACE) ||
          (backspaceTimer > 0.5f &&
           (backspaceTimer - 0.5f) > backspaceInterval)) {
        if (backspaceTimer > 0.5f) {
          backspaceTimer = 0.5f + backspaceInterval * 0.5f;
        }

        if (nameCharCount > 0) {
          nameCharCount--;
          playerName[nameCharCount] = '\0';
        }
      }
    } else {
      backspaceTimer = 0.0f;
    }

    if (IsKeyPressed(KEY_ENTER) && nameCharCount > 0) {
      if (stats.IsValidName(playerName)) {
        stats.AddHighScore(playerName, lastTimeRecord);
        enteringName = false;
        highscoreEntered = true;
      } else {
        invalidName = true;
      }
    }
  }
}

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

  if (enteringName) {
    DrawNameEntry();
  } else if (board.IsGameWon()) {
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

  if (showStats && !enteringName) {
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
    DrawCircle((float)cx, (float)cy, size / 2.5f, BLACK);
    float spikeLen = size / 2.0f;
    for (int i = 0; i < 8; i++) {
      float angle = i * PI / 4.0f;
      DrawLineEx({(float)cx, (float)cy},
                 {(float)cx + cos(angle) * spikeLen,
                  (float)cy + sin(angle) * spikeLen},
                 2.0f, BLACK);
    }
    DrawCircle((float)cx - size / 7.0f, (float)cy - size / 7.0f, size / 10.0f,
               GRAY);
  }
}

void UI::DrawFlag(int cx, int cy, int size) {
  float baseW = size * 0.7f;
  float poleH = size * 1.1f;

  DrawRectangle((int)((float)cx - baseW / 2.0f),
                (int)((float)cy + poleH / 2.0f - 4.0f), (int)baseW, 4, BLACK);
  DrawRectangle((int)((float)cx - baseW / 4.0f),
                (int)((float)cy + poleH / 2.0f - 8.0f), (int)(baseW / 2.0f), 4,
                BLACK);

  DrawRectangle((int)((float)cx - 2.0f), (int)((float)cy - poleH / 2.0f), 4,
                (int)poleH, BLACK);

  Vector2 top = {(float)cx, (float)cy - poleH / 2};
  Vector2 bottom = {(float)cx, (float)cy - poleH / 2 + size * 0.5f};
  Vector2 tip = {(float)cx - size * 0.5f, (float)cy - poleH / 2 + size * 0.25f};
  DrawTriangle(bottom, top, tip, RED);
}

void UI::DrawStatsOverlay() {
  int w = 650;
  int h = 400;
  int x = (GetScreenWidth() - w) / 2;
  int y = (GetScreenHeight() - h) / 2;

  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});
  DrawRectangleRounded({(float)x, (float)y, (float)w, (float)h}, 0.05f, 8,
                       Color{33, 37, 43, 255});
  DrawRectangleRoundedLines({(float)x, (float)y, (float)w, (float)h}, 0.05f, 8,
                            2.0f, DARKGRAY);

  DrawText("STATISTICS", x + 40, y + 25, 25, SKYBLUE);
  DrawLine(x + 300, y + 60, x + 300, y + h - 60,
           DARKGRAY); // Vertical divider

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
  f << "Fastest: " << std::fixed << std::setprecision(1)
    << stats.GetFastestTime() << "s";
  s << "Slowest: " << std::fixed << std::setprecision(1)
    << stats.GetSlowestTime() << "s";
  a << "Average: " << std::fixed << std::setprecision(1)
    << stats.GetAverageTime() << "s";

  int textY = y + 75;
  int textX = x + 40;
  DrawText(s1.str().c_str(), textX, textY, 18, RAYWHITE);
  textY += 30;
  DrawText(s2.str().c_str(), textX, textY, 18, GREEN);
  textY += 30;
  DrawText(s3.str().c_str(), textX, textY, 18, RED);
  textY += 30;
  DrawText(s4.str().c_str(), textX, textY, 18, ORANGE);

  textY += 60; // Increased gap to 60
  DrawText("TIMING RECORDS", textX, textY - 25, 16, GRAY);
  DrawText(f.str().c_str(), textX, textY, 18, LIGHTGRAY);
  textY += 25;
  DrawText(s.str().c_str(), textX, textY, 18, LIGHTGRAY);
  textY += 25;
  DrawText(a.str().c_str(), textX, textY, 18, LIGHTGRAY);

  // Leaderboard Section
  int lbX = x + 330;
  int lbY = y + 75;
  DrawText("TOP 10 RECORDS", lbX, y + 25, 20, YELLOW);

  const auto &highScores = stats.GetHighScores();
  for (int i = 0; i < 10; i++) {
    std::string rank = std::to_string(i + 1) + ". ";
    std::string name = (i < highScores.size()) ? highScores[i].name : "---";
    std::stringstream ts;
    if (i < highScores.size())
      ts << std::fixed << std::setprecision(1) << highScores[i].time << "s";
    else
      ts << "---";

    DrawText(rank.c_str(), lbX, lbY + (i * 24), 16, GRAY);
    DrawText(name.c_str(), lbX + 35, lbY + (i * 24), 16,
             (i < highScores.size() ? RAYWHITE : DARKGRAY));
    DrawText(ts.str().c_str(), lbX + 210, lbY + (i * 24), 16,
             (i < highScores.size() ? SKYBLUE : DARKGRAY));
  }

  DrawText("Press 'S' to close",
           x + (w - MeasureText("Press 'S' to close", 15)) / 2, y + h - 25, 15,
           GRAY);
}

void UI::DrawNameEntry() {
  int w = 400;
  int h = 200;
  int x = (GetScreenWidth() - w) / 2;
  int y = (GetScreenHeight() - h) / 2;

  DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Color{0, 0, 0, 150});
  DrawRectangleRounded({(float)x, (float)y, (float)w, (float)h}, 0.1f, 8,
                       Color{33, 37, 43, 255});
  DrawRectangleRoundedLines({(float)x, (float)y, (float)w, (float)h}, 0.1f, 8,
                            2.0f, SKYBLUE);

  std::string title = (currentRank == 1) ? "NEW RECORD!" : "TOP 10 SCORE!";
  Color titleColor = (currentRank == 1) ? GOLD : YELLOW;

  DrawText(title.c_str(), x + (w - MeasureText(title.c_str(), 25)) / 2, y + 30,
           25, titleColor);

  std::string timeMsg = "Time: " + std::to_string(lastTimeRecord).substr(0, 4) +
                        "s (Rank #" + std::to_string(currentRank) + ")";
  DrawText(timeMsg.c_str(), x + (w - MeasureText(timeMsg.c_str(), 20)) / 2,
           y + 65, 20, RAYWHITE);

  DrawText("Enter your name:", x + 40, y + 100, 18, LIGHTGRAY);

  // Input Box
  DrawRectangle(x + 40, y + 125, w - 80, 40, Color{20, 24, 30, 255});
  DrawRectangleLines(x + 40, y + 125, w - 80, 40, invalidName ? RED : DARKGRAY);
  DrawText(playerName, x + 50, y + 135, 20, RAYWHITE);

  DrawText("_", x + 55 + MeasureText(playerName, 20), y + 135, 20, SKYBLUE);

  if (invalidName) {
    DrawText("Invalid or inappropriate name!", x + 40, y + 168, 15, RED);
  }

  DrawText("Press ENTER to save",
           x + (w - MeasureText("Press ENTER to save", 15)) / 2, y + 185, 15,
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
