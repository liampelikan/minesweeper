#include "StatManager.h"
#include <fstream>
#include <iostream>

#include <algorithm>
#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h>
#endif

StatManager::StatManager(const std::string &filename) : baseFilename(filename) {
#if defined(PLATFORM_WEB)
  this->filename = "/persistent/" + filename;
#else
  this->filename = filename;
#endif
  Load();
}

void StatManager::RecordStart() {
  data.gamesStarted++;
  Save();
}

void StatManager::RecordIncomplete() {
  data.gamesIncomplete++;
  Save();
}

void StatManager::RecordGame(bool won, bool lost, float time,
                             int minesFlagged) {
  if (won) {
    data.gamesWon++;
    data.winTimes.push_back(time);
  } else if (lost) {
    data.gamesLost++;
  }
  data.totalMinesFlagged += minesFlagged;
  Save();
}

void StatManager::AddHighScore(const std::string &name, float time) {
  data.highScores.push_back({name, time});
  std::sort(
      data.highScores.begin(), data.highScores.end(),
      [](const HighScore &a, const HighScore &b) { return a.time < b.time; });
  if (data.highScores.size() > 10) {
    data.highScores.resize(10);
  }
  Save();
}

bool StatManager::IsNewHighScore(float time) const {
  if (data.highScores.size() < 10)
    return true;
  return time < data.highScores.back().time;
}

int StatManager::GetRankForTime(float time) const {
  for (int i = 0; i < (int)data.highScores.size(); i++) {
    if (time < data.highScores[i].time) {
      return i + 1;
    }
  }
  if (data.highScores.size() < 10) {
    return (int)data.highScores.size() + 1;
  }
  return -1;
}

bool StatManager::IsValidName(const std::string &name) const {
  if (name.empty() || name.length() > 15)
    return false;

  // Bad name filter
  std::vector<std::string> blacklist = {"ass", "fuck", "shit", "bitch",
                                        "nigger"};
  std::string lowerName = name;
  std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                 ::tolower);

  for (const auto &bad : blacklist) {
    if (lowerName.find(bad) != std::string::npos)
      return false;
  }

  return true;
}

void StatManager::Save() {
  std::ofstream out(filename);
  if (out.is_open()) {
    out << data.gamesStarted << " " << data.gamesWon << " " << data.gamesLost
        << " " << data.gamesIncomplete << " " << data.totalMinesFlagged << " "
        << data.noGuessMode << " " << data.winTimes.size() << " "
        << data.highScores.size() << "\n";

    for (float t : data.winTimes)
      out << t << " ";
    out << "\n";

    for (const auto &hs : data.highScores) {
      out << hs.name << " " << hs.time << "\n";
    }
  }

#if defined(PLATFORM_WEB)
  EM_ASM({
    if (window["sync_to_idbfs"]) {
      window["sync_to_idbfs"]();
    }
  });
#endif
}

void StatManager::Load() {
  std::ifstream in(filename);
  if (in.is_open()) {
    int winCount = 0;
    int hsCount = 0;
    in >> data.gamesStarted >> data.gamesWon >> data.gamesLost >>
        data.gamesIncomplete >> data.totalMinesFlagged >> data.noGuessMode >>
        winCount >> hsCount;

    data.winTimes.clear();
    for (int i = 0; i < winCount; i++) {
      float t;
      in >> t;
      data.winTimes.push_back(t);
    }

    data.highScores.clear();
    for (int i = 0; i < hsCount; i++) {
      std::string name;
      float time;
      in >> name >> time;
      data.highScores.push_back({name, time});
    }
  }
}

float StatManager::GetFastestTime() const {
  if (data.winTimes.empty())
    return 0.0f;
  float fastest = data.winTimes[0];
  for (float t : data.winTimes)
    if (t < fastest)
      fastest = t;
  return fastest;
}

float StatManager::GetSlowestTime() const {
  if (data.winTimes.empty())
    return 0.0f;
  float slowest = data.winTimes[0];
  for (float t : data.winTimes)
    if (t > slowest)
      slowest = t;
  return slowest;
}

float StatManager::GetAverageTime() const {
  if (data.winTimes.empty())
    return 0.0f;
  float sum = 0.0f;
  for (float t : data.winTimes)
    sum += t;
  return sum / data.winTimes.size();
}
