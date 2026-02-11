#pragma once
#include <string>
#include <vector>

struct StatsData {
    int gamesStarted = 0;
    int gamesWon = 0;
    int gamesLost = 0;
    int gamesIncomplete = 0;
    std::vector<float> winTimes;
    int totalMinesFlagged = 0;
    bool noGuessMode = false;
};

class StatManager {
public:
    StatManager(const std::string& filename);
    
    void RecordGame(bool won, bool lost, float time, int minesFlagged);
    void RecordStart();
    void RecordIncomplete();
    void Save();
    void Load();

    void SetNoGuessMode(bool enabled) { data.noGuessMode = enabled; Save(); }
    bool GetNoGuessMode() const { return data.noGuessMode; }

    const StatsData& GetData() const { return data; }
    
    float GetFastestTime() const;
    float GetSlowestTime() const;
    float GetAverageTime() const;

private:
    std::string filename;
    StatsData data;
};
