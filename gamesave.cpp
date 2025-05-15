
/*

                -------------------- THIS SPECIFIC PART HAS CAUSED ALOT OF DEBUGS SO IGNORE THIS PORTION ------------------ 


#include "gamesave.h"
#include <fstream>
#include <cstring>

GameSave::GameSave(const std::string& username) : username(username) {}

void GameSave::saveGame(int totalPoints, int userId, int loginCount, int leaderboardRank) {
    std::fstream file("gamesave.dat", std::ios::binary | std::ios::in | std::ios::out);
    if (!file.is_open()) {
        file.open("gamesave.dat", std::ios::binary | std::ios::out);
        file.close();
        file.open("gamesave.dat", std::ios::binary | std::ios::in | std::ios::out);
    }
    struct SaveRecord { char username[50]; int totalPoints; int userId; int loginCount; int leaderboardRank; };
    SaveRecord record;
    bool exists = false;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(SaveRecord))) {
        if (username == record.username) {
            record.totalPoints = totalPoints;
            record.userId = userId;
            record.loginCount = loginCount;
            record.leaderboardRank = leaderboardRank;
            file.seekp(-static_cast<int>(sizeof(SaveRecord)), std::ios::cur);
            file.write(reinterpret_cast<char*>(&record), sizeof(SaveRecord));
            exists = true;
            break;
        }
    }
    if (!exists) {
        file.clear();
        file.close();
        file.open("gamesave.dat", std::ios::binary | std::ios::app);
        std::strncpy(record.username, username.c_str(), 50);
        record.username[49] = '\0';
        record.totalPoints = totalPoints;
        record.userId = userId;
        record.loginCount = loginCount;
        record.leaderboardRank = leaderboardRank;
        file.write(reinterpret_cast<char*>(&record), sizeof(SaveRecord));
    }
    file.close();
}

bool GameSave::loadGame(int& totalPoints, int& userId, int& loginCount, int& leaderboardRank) {
    std::ifstream file("gamesave.dat", std::ios::binary);
    if (!file.is_open()) {
        return false;
    }
    struct SaveRecord { char username[50]; int totalPoints; int userId; int loginCount; int leaderboardRank; };
    SaveRecord record;
    while (file.read(reinterpret_cast<char*>(&record), sizeof(SaveRecord))) {
        if (username == record.username) {
            totalPoints = record.totalPoints;
            userId = record.userId;
            loginCount = record.loginCount;
            leaderboardRank = record.leaderboardRank;
            file.close();
            return true;
        }
    }
    file.close();
    return false;
}
*/