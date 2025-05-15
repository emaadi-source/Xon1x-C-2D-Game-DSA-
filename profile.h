#pragma once
#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;
using namespace std;

// Forward declarations for friend system structures
struct FriendNode;
struct PendingRequest;
struct Player;
struct HashNode;

// Friend system structures
struct FriendNode {
    char username[50]; // Player ID (username)
    FriendNode* next;
    FriendNode(const char* name);
};

struct PendingRequest {
    char from[50]; // Sender's username
    PendingRequest* next;
    PendingRequest(const char* sender);
};

struct Player {
    char username[50]; // Player ID
    FriendNode* friends; // appellate
    PendingRequest* requests; // Linked list of pending requests
    Player(const char* name);
    ~Player();
};

struct HashNode {
    char username[50];
    int playerIndex; // Index in players array
    HashNode* next;
    HashNode(const char* name, int index);
};

class ProfileMenu {
public:
    ProfileMenu(const std::string& username);
    ~ProfileMenu();
    bool show(sf::RenderWindow& window);

private:
    void loadResources();
    void updateSelected(sf::Vector2f mousePos);
    void loadUserData();
    void updateLoginCount();

    // Friend system functions
    void initializeFriendSystem();
    void loadFriendsFromFile();
    void saveFriendsToFile();
    int hashFunction(const char* username);
    int findPlayerIndex(const char* username);
    bool addPlayer(const char* username);
    bool sendFriendRequest(const char* from, const char* to);
    bool acceptFriendRequest(const char* from, const char* to);
    bool rejectFriendRequest(const char* from, const char* to);
    void getPendingRequests(char* output, int maxLen);
    void getFriendsList(char* output, int maxLen);
    bool isFriend(const char* username1, const char* username2);
    bool hasPendingRequest(const char* from, const char* to);

    // UI Elements
    Font font;
    Texture backgroundTexture;
    Sprite background;

    RectangleShape playButton;
    RectangleShape backButton;
    RectangleShape friendsListButton;

    Text playText;
    Text backText;
    Text friendsListText;
    Text titleText;
    Text usernameText;
    Text totalPointsText;
    Text matchHistoryText;

    string currentUser;
    int totalPoints;
    int selected;
    int userId;
    int loginCount;
    int leaderboardRank;
    Text userIdText;
    Text loginCountText;
    Text leaderboardText;

    // Friend system UI
    RectangleShape searchBox;
    RectangleShape sendRequestButton;
    RectangleShape requestsBox;
    RectangleShape friendsBox;
    Text searchText;
    Text requestText;
    Text friendsText;
    string searchInput;
    bool isTypingSearch;

    // Friend system data
    Player** players; // Dynamic array of players
    int playerCount;
    int playerCapacity;
    HashNode** hashTable; // Hash table for username-to-index lookup
    int hashTableSize;
};