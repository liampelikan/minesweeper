#pragma once
#include <string>
#include <vector>

struct HighScore {
  std::string name;
  float time;
};

struct StatsData {
  int gamesStarted = 0;
  int gamesWon = 0;
  int gamesLost = 0;
  int gamesIncomplete = 0;
  std::vector<float> winTimes;
  std::vector<HighScore> highScores;
  int totalMinesFlagged = 0;
  bool noGuessMode = false;
};

class StatManager {
public:
  StatManager(const std::string &filename);

  void RecordGame(bool won, bool lost, float time, int minesFlagged);
  void RecordStart();
  void RecordIncomplete();
  void Save();
  void Load();

  void AddHighScore(const std::string &name, float time);
  bool IsNewHighScore(float time) const;
  int GetRankForTime(float time) const;
  bool IsValidName(const std::string &name) const;

  void SetNoGuessMode(bool enabled) {
    data.noGuessMode = enabled;
    Save();
  }
  bool GetNoGuessMode() const { return data.noGuessMode; }

  const StatsData &GetData() const { return data; }

  float GetFastestTime() const;
  float GetSlowestTime() const;
  float GetAverageTime() const;
  const std::vector<HighScore> &GetHighScores() const {
    return data.highScores;
  }

private:
  std::string filename;
  std::string baseFilename;
  StatsData data;
};
