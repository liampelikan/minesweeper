#include "Board.h"
#include "StatManager.h"
#include "raylib.h"

class UI {
public:
    UI(Board& board, StatManager& stats);
    ~UI();
    void Update();
    void Draw(float currentTime, bool showStats);
    
    // Window control helpers (public for Game to access)
    bool IsOverClose(Vector2 mouse) const;
    bool IsOverMinimize(Vector2 mouse) const;
    bool IsOverTitleBar(Vector2 mouse) const;

private:
    Board& board;
    StatManager& stats;
    Texture2D mineTexture;
    bool textureLoaded = false;
    
    const int cellSize = 32;
    const int titleBarHeight = 35;
    const int statusHeaderHeight = 60;
    const int topBarHeight = 95; // titleBarHeight + statusHeaderHeight
    
    void DrawCustomTitleBar();
    void DrawStatusHeader(float currentTime);
    void DrawCell(int x, int y, int offsetX, int offsetY);
    void DrawStatsOverlay();
    void DrawMine(int cx, int cy, int size);
    void DrawFlag(int cx, int cy, int size);
    Color GetNumberColor(int number);
};
