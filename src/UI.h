#include "Board.h"
#include "StatManager.h"
#include "raylib.h"

class UI {
public:
  UI(Board &board, StatManager &stats);
  ~UI();
  void Update(float currentTime);
  void Draw(float currentTime, bool showStats);

  // Window controls
  bool IsOverClose(Vector2 mouse) const;
  bool IsOverMinimize(Vector2 mouse) const;
  bool IsOverTitleBar(Vector2 mouse) const;
  bool IsEnteringName() const { return enteringName; }

private:
  Board &board;
  StatManager &stats;
  Texture2D mineTexture;
  bool textureLoaded = false;

  const int cellSize = 32;
  const int titleBarHeight = 35;
  const int statusHeaderHeight = 60;
  const int topBarHeight = 95; // titleBarHeight + statusHeaderHeight

  // High Score Entry State
  bool enteringName = false;
  bool highscoreEntered = false; // Prevent multiple entries per win
  bool invalidName = false;      // Feedback for bad names
  char playerName[16] = "\0";
  int nameCharCount = 0;
  float lastTimeRecord = 0.0f;
  int currentRank = -1;
  float backspaceTimer = 0.0f;
  float backspaceInterval = 0.05f; // Rapid repetition

  void DrawCustomTitleBar();
  void DrawStatusHeader(float currentTime);
  void DrawCell(int x, int y, int offsetX, int offsetY);
  void DrawStatsOverlay();
  void DrawNameEntry();
  void DrawMine(int cx, int cy, int size);
  void DrawFlag(int cx, int cy, int size);
  Color GetNumberColor(int number);
};
