/*
* 
* 
*              -------------------- THIS SPECIFIC PART HAS CAUSED ALOT OF DEBUGS SO IGNORE THIS PORTION ------------------ 
* 
* 
#pragma once
#include <string>

class GameSave {
public:
    GameSave(const std::string& username);
    void saveGame(int totalPoints, int userId, int loginCount, int leaderboardRank);
    bool loadGame(int& totalPoints, int& userId, int& loginCount, int& leaderboardRank);
private:
    std::string username;
};
#endif
*/