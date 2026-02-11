#include "StatManager.h"
#include <fstream>
#include <iostream>

StatManager::StatManager(const std::string& filename) : filename(filename) {
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

void StatManager::RecordGame(bool won, bool lost, float time, int minesFlagged) {
    if (won) {
        data.gamesWon++;
        data.winTimes.push_back(time);
    } else if (lost) {
        data.gamesLost++;
    }
    data.totalMinesFlagged += minesFlagged;
    Save();
}

void StatManager::Save() {
    std::ofstream out(filename);
    if (out.is_open()) {
        out << data.gamesStarted << " " 
            << data.gamesWon << " " 
            << data.gamesLost << " "
            << data.gamesIncomplete << " "
            << data.totalMinesFlagged << " " 
            << data.noGuessMode << " "
            << data.winTimes.size() << "\n";
        for (float t : data.winTimes) out << t << " ";
    }
}

void StatManager::Load() {
    std::ifstream in(filename);
    if (in.is_open()) {
        int count = 0;
        in >> data.gamesStarted >> data.gamesWon >> data.gamesLost >> data.gamesIncomplete >> data.totalMinesFlagged >> data.noGuessMode >> count;
        data.winTimes.clear();
        for (int i = 0; i < count; i++) {
            float t;
            in >> t;
            data.winTimes.push_back(t);
        }
    }
}

float StatManager::GetFastestTime() const {
    if (data.winTimes.empty()) return 0.0f;
    float fastest = data.winTimes[0];
    for (float t : data.winTimes) if (t < fastest) fastest = t;
    return fastest;
}

float StatManager::GetSlowestTime() const {
    if (data.winTimes.empty()) return 0.0f;
    float slowest = data.winTimes[0];
    for (float t : data.winTimes) if (t > slowest) slowest = t;
    return slowest;
}

float StatManager::GetAverageTime() const {
    if (data.winTimes.empty()) return 0.0f;
    float sum = 0.0f;
    for (float t : data.winTimes) sum += t;
    return sum / data.winTimes.size();
}
