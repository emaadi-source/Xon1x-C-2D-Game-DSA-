#include "profile.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
using namespace sf;

// Helper function to manually copy a string with a maximum length
void manual_str_copy(char* dest, const char* src, int maxLen) {
    int i = 0;
    while (i < maxLen - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    dest[i] = '\0';
}

// Helper function to manually append a string with buffer size check
void manual_str_append(char* dest, const char* src, int maxLen) {
    int destLen = 0;
    while (destLen < maxLen && dest[destLen] != '\0') {
        ++destLen;
    }
    int i = 0;
    while (destLen + i < maxLen - 1 && src[i] != '\0') {
        dest[destLen + i] = src[i];
        ++i;
    }
    dest[destLen + i] = '\0';
}

// Helper function to manually tokenize a string
char* manual_str_tokenize(char* str, const char* delim, char** context) {
    if (str == nullptr) {
        str = *context;
    }
    if (str == nullptr || *str == '\0') {
        *context = nullptr;
        return nullptr;
    }

    // Skip leading delimiters
    while (*str != '\0') {
        bool isDelim = false;
        for (int i = 0; delim[i] != '\0'; ++i) {
            if (*str == delim[i]) {
                isDelim = true;
                break;
            }
        }
        if (!isDelim) break;
        ++str;
    }

    if (*str == '\0') {
        *context = nullptr;
        return nullptr;
    }

    // Find the end of the token
    char* tokenStart = str;
    while (*str != '\0') {
        bool isDelim = false;
        for (int i = 0; delim[i] != '\0'; ++i) {
            if (*str == delim[i]) {
                isDelim = true;
                break;
            }
        }
        if (isDelim) break;
        ++str;
    }

    // Null-terminate the token and updating context
    if (*str != '\0') {
        *str = '\0';
        *context = str + 1;
    }
    else {
        *context = nullptr;
    }

    return tokenStart;
}

// FriendNode constructor
FriendNode::FriendNode(const char* name) {
    manual_str_copy(username, name, 50);
    next = nullptr;
}

// PendingRequest constructor
PendingRequest::PendingRequest(const char* sender) {
    manual_str_copy(from, sender, 50);
    next = nullptr;
}

// Player constructor
Player::Player(const char* name) {
    manual_str_copy(username, name, 50);
    friends = nullptr;
    requests = nullptr;
}

// HashNode constructor
HashNode::HashNode(const char* name, int index) {
    manual_str_copy(username, name, 50);
    playerIndex = index;
    next = nullptr;
}

// Player destructor
Player::~Player() {
    FriendNode* currentFriend = friends;
    while (currentFriend) {
        FriendNode* next = currentFriend->next;
        delete currentFriend;
        currentFriend = next;
    }
    PendingRequest* currentRequest = requests;
    while (currentRequest) {
        PendingRequest* next = currentRequest->next;
        delete currentRequest;
        currentRequest = next;
    }
}

ProfileMenu::ProfileMenu(const string& username) :
    currentUser(username),
    totalPoints(0),
    selected(-1),
    userId(0),
    loginCount(0),
    leaderboardRank(0),
    playerCount(0),
    playerCapacity(10),
    hashTableSize(100)
{
    // Initialize friend system
    players = new Player * [playerCapacity];
    for (int i = 0; i < playerCapacity; ++i) {
        players[i] = nullptr;
    }
    hashTable = new HashNode * [hashTableSize];
    for (int i = 0; i < hashTableSize; ++i) {
        hashTable[i] = nullptr;
    }
    initializeFriendSystem();

    loadResources();
    loadUserData();
    updateLoginCount();

    // Button setup
    playButton.setSize(sf::Vector2f(200, 40));
    playButton.setPosition(640, 650);
    playButton.setFillColor(sf::Color(0, 150, 0));

    backButton.setSize(sf::Vector2f(200, 40));
    backButton.setPosition(100, 650);
    backButton.setFillColor(sf::Color(150, 0, 0));

    friendsListButton.setSize(sf::Vector2f(200, 40));
    friendsListButton.setPosition(350, 650);
    friendsListButton.setFillColor(sf::Color(0, 0, 150));

    // Text setup
    titleText.setString("PLAYER PROFILE");
    titleText.setFont(font);
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::Red);
    titleText.setPosition(230, 60);

    usernameText.setString("Username: " + currentUser);
    usernameText.setFont(font);
    usernameText.setCharacterSize(30);
    usernameText.setFillColor(sf::Color(255, 215, 0));
    usernameText.setStyle(sf::Text::Bold);
    usernameText.setPosition(180, 160);

    totalPointsText.setString("Total Points: " + to_string(totalPoints));
    totalPointsText.setFont(font);
    totalPointsText.setCharacterSize(36);
    totalPointsText.setFillColor(sf::Color::Cyan);
    totalPointsText.setStyle(sf::Text::Bold);
    totalPointsText.setPosition(180, 240);

    matchHistoryText.setString("Match History:\n- Win: Level 1\n- Loss: Level 2\n- Win: Level 3");
    matchHistoryText.setFont(font);
    matchHistoryText.setCharacterSize(30);
    matchHistoryText.setFillColor(sf::Color(220, 220, 220));
    matchHistoryText.setPosition(180, 360);
    matchHistoryText.setLineSpacing(1.3f);

    userIdText.setString("Player ID: " + to_string(userId));
    userIdText.setFont(font);
    userIdText.setCharacterSize(30);
    userIdText.setFillColor(sf::Color::Yellow);
    userIdText.setStyle(sf::Text::Bold);
    userIdText.setPosition(180, 200);

    loginCountText.setString("Total Logins: " + to_string(loginCount));
    loginCountText.setFont(font);
    loginCountText.setCharacterSize(36);
    loginCountText.setFillColor(sf::Color::Magenta);
    loginCountText.setStyle(sf::Text::Bold);
    loginCountText.setPosition(180, 320);

    leaderboardText.setString("Leaderboard Rank: " + to_string(leaderboardRank));
    leaderboardText.setFont(font);
    leaderboardText.setCharacterSize(36);
    leaderboardText.setFillColor(sf::Color::Cyan);
    leaderboardText.setStyle(sf::Text::Bold);
    leaderboardText.setPosition(180, 280);

    // Button texts
    playText.setString("Play Game");
    playText.setFont(font);
    playText.setCharacterSize(30);
    playText.setPosition(670, 650);

    backText.setString("Back");
    backText.setFont(font);
    backText.setCharacterSize(30);
    backText.setPosition(160, 650);

    friendsListText.setString("Friends List");
    friendsListText.setFont(font);
    friendsListText.setCharacterSize(30);
    friendsListText.setPosition(380, 650);

    // Friend system UI
    searchBox.setSize(sf::Vector2f(400, 40));
    searchBox.setFillColor(sf::Color(240, 240, 240));
    searchBox.setOutlineThickness(2);
    searchBox.setOutlineColor(sf::Color(50, 50, 50));
    searchBox.setPosition(280, 180);

    sendRequestButton.setSize(sf::Vector2f(150, 40));
    sendRequestButton.setPosition(700, 180);
    sendRequestButton.setFillColor(sf::Color(0, 150, 0));
    sendRequestButton.setOutlineThickness(2);
    sendRequestButton.setOutlineColor(sf::Color::Black);

    requestsBox.setSize(sf::Vector2f(700, 200));
    requestsBox.setPosition(130, 250);
    requestsBox.setFillColor(sf::Color(255, 255, 255, 30));
    requestsBox.setOutlineThickness(2);
    requestsBox.setOutlineColor(sf::Color(100, 100, 100));

    friendsBox.setSize(sf::Vector2f(700, 200));
    friendsBox.setPosition(130, 480);
    friendsBox.setFillColor(sf::Color(255, 255, 255, 30));
    friendsBox.setOutlineThickness(2);
    friendsBox.setOutlineColor(sf::Color(100, 100, 100));

    searchText.setFont(font);
    searchText.setCharacterSize(28);
    searchText.setFillColor(sf::Color::Black);
    searchText.setPosition(290, 185);

    requestText.setFont(font);
    requestText.setCharacterSize(24);
    requestText.setFillColor(sf::Color(220, 220, 220));
    requestText.setPosition(150, 260);

    friendsText.setFont(font);
    friendsText.setCharacterSize(24);
    friendsText.setFillColor(sf::Color(220, 220, 220));
    friendsText.setPosition(150, 490);

    // Add button labels
    sf::Text sendButtonText("Send Request", font, 24);
    sendButtonText.setPosition(705, 185);
    sendButtonText.setFillColor(sf::Color::White);
}

ProfileMenu::~ProfileMenu() {
    for (int i = 0; i < playerCount; ++i) {
        delete players[i];
    }
    delete[] players;
    for (int i = 0; i < hashTableSize; ++i) {
        HashNode* current = hashTable[i];
        while (current) {
            HashNode* next = current->next;
            delete current;
            current = next;
        }
    }
    delete[] hashTable;
}

void ProfileMenu::loadResources() {
    if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
        cerr << "Error loading profile menu font!\n";
    }
    if (!backgroundTexture.loadFromFile("images/pause_bg.jpg")) {
        cerr << "Error loading profile background!\n";
    }
    background.setTexture(backgroundTexture);
    background.setPosition(150, 50);
}

void ProfileMenu::loadUserData() {
    struct UserRecord { string name; int score; };
    UserRecord allUsers[100];
    int totalUsers = 0;

    ifstream file("users.txt");
    string line;
    bool userFound = false;

    while (getline(file, line)) {
        size_t colon1 = line.find(':');
        size_t colon2 = line.find(':', colon1 + 1);

        if (colon1 != string::npos && colon2 != string::npos) {
            string user = line.substr(0, colon1);
            int score = stoi(line.substr(colon2 + 1));

            if (userId == 0 && user == currentUser) {
                userId = totalUsers + 1;
            }

            allUsers[totalUsers].name = user;
            allUsers[totalUsers].score = score;
            totalUsers++;

            if (user == currentUser) {
                totalPoints = score;
                userFound = true;
            }
        }
    }

    for (int i = 0; i < totalUsers - 1; i++) {
        for (int j = 0; j < totalUsers - i - 1; j++) {
            if (allUsers[j].score < allUsers[j + 1].score) {
                UserRecord temp = allUsers[j];
                allUsers[j] = allUsers[j + 1];
                allUsers[j + 1] = temp;
            }
        }
    }

    for (int i = 0; i < totalUsers; i++) {
        if (allUsers[i].name == currentUser) {
            leaderboardRank = i + 1;
            break;
        }
    }
}

void ProfileMenu::updateLoginCount() {
    struct LoginRecord { char username[50]; int count; };
    LoginRecord record;
    bool exists = false;

    fstream file("logins.dat", ios::binary | ios::in | ios::out);
    if (!file.is_open()) {
        file.open("logins.dat", ios::binary | ios::out);
        file.close();
        file.open("logins.dat", ios::binary | ios::in | ios::out);
    }

    while (file.read(reinterpret_cast<char*>(&record), sizeof(LoginRecord))) {
        if (currentUser == record.username) {
            record.count++;
            file.seekp(-static_cast<int>(sizeof(LoginRecord)), ios::cur);
            file.write(reinterpret_cast<char*>(&record), sizeof(LoginRecord));
            loginCount = record.count;
            exists = true;
            break;
        }
    }

    if (!exists) {
        file.clear();
        file.close();
        file.open("logins.dat", ios::binary | ios::app);
        manual_str_copy(record.username, currentUser.c_str(), 50);
        record.count = 1;
        file.write(reinterpret_cast<char*>(&record), sizeof(LoginRecord));
        loginCount = 1;
    }
    file.close();
}

void ProfileMenu::updateSelected(sf::Vector2f mousePos) {
    if (backButton.getGlobalBounds().contains(mousePos)) {
        selected = 0;
    }
    else if (friendsListButton.getGlobalBounds().contains(mousePos)) {
        selected = 1;
    }
    else if (playButton.getGlobalBounds().contains(mousePos)) {
        selected = 2;
    }
    else {
        selected = -1;
    }
}

// Friend system implementation
void ProfileMenu::initializeFriendSystem() {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        size_t colon1 = line.find(':');
        if (colon1 != string::npos) {
            string user = line.substr(0, colon1);
            addPlayer(user.c_str());
        }
    }
    file.close();
    loadFriendsFromFile();
}

void ProfileMenu::loadFriendsFromFile() {
    ifstream file("friends.txt");
    char line[200];
    while (file.getline(line, 200)) {
        char lineCopy[200];
        manual_str_copy(lineCopy, line, 200);
        char* context = nullptr;
        char* sender = manual_str_tokenize(lineCopy, ":", &context);
        char* receiver = manual_str_tokenize(nullptr, ":", &context);
        char* status = manual_str_tokenize(nullptr, ":", &context);
        if (sender && receiver && status && strcmp(status, "accepted") == 0) {
            int senderIndex = findPlayerIndex(sender);
            int receiverIndex = findPlayerIndex(receiver);
            if (senderIndex != -1 && receiverIndex != -1) {
                FriendNode* newFriend1 = new FriendNode(receiver);
                newFriend1->next = players[senderIndex]->friends;
                players[senderIndex]->friends = newFriend1;

                FriendNode* newFriend2 = new FriendNode(sender);
                newFriend2->next = players[receiverIndex]->friends;
                players[receiverIndex]->friends = newFriend2;
            }
        }
        else if (sender && receiver && status && strcmp(status, "pending") == 0) {
            int receiverIndex = findPlayerIndex(receiver);
            if (receiverIndex != -1) {
                PendingRequest* newRequest = new PendingRequest(sender);
                newRequest->next = players[receiverIndex]->requests;
                players[receiverIndex]->requests = newRequest;
            }
        }
    }
    file.close();
}

void ProfileMenu::saveFriendsToFile() {
    ofstream file("friends.txt");
    for (int i = 0; i < playerCount; ++i) {
        FriendNode* currentFriend = players[i]->friends; // Renamed variable
        while (currentFriend) {
            file << players[i]->username << ":" << currentFriend->username << ":accepted\n";
            currentFriend = currentFriend->next;
        }
        PendingRequest* request = players[i]->requests;
        while (request) {
            file << request->from << ":" << players[i]->username << ":pending\n";
            request = request->next;
        }
    }
    file.close();
}

int ProfileMenu::hashFunction(const char* username) {
    unsigned int hash = 0;
    for (int i = 0; username[i] != '\0'; ++i) {
        hash = hash * 31 + username[i];
    }
    return hash % hashTableSize;
}

int ProfileMenu::findPlayerIndex(const char* username) {
    int hashIndex = hashFunction(username);
    HashNode* current = hashTable[hashIndex];
    while (current) {
        if (strcmp(current->username, username) == 0) {
            return current->playerIndex;
        }
        current = current->next;
    }
    return -1;
}

bool ProfileMenu::addPlayer(const char* username) {
    if (findPlayerIndex(username) != -1) {
        return false; // Player already exists
    }
    if (playerCount >= playerCapacity) {
        int newCapacity = playerCapacity * 2;
        Player** newPlayers = new Player * [newCapacity];
        for (int i = 0; i < playerCount; ++i) {
            newPlayers[i] = players[i];
        }
        for (int i = playerCount; i < newCapacity; ++i) {
            newPlayers[i] = nullptr;
        }
        delete[] players;
        players = newPlayers;
        playerCapacity = newCapacity;
    }
    players[playerCount] = new Player(username);
    int hashIndex = hashFunction(username);
    HashNode* newNode = new HashNode(username, playerCount);
    newNode->next = hashTable[hashIndex];
    hashTable[hashIndex] = newNode;
    ++playerCount;
    return true;
}

bool ProfileMenu::sendFriendRequest(const char* from, const char* to) {
    int fromIndex = findPlayerIndex(from);
    int toIndex = findPlayerIndex(to);
    if (fromIndex == -1 || toIndex == -1) {
        return false; // One or both users not found
    }
    if (strcmp(from, to) == 0) {
        return false; // Cannot add self
    }
    if (isFriend(from, to) || hasPendingRequest(from, to)) {
        return false; // Already friends or request pending
    }
    PendingRequest* newRequest = new PendingRequest(from);
    newRequest->next = players[toIndex]->requests;
    players[toIndex]->requests = newRequest;
    saveFriendsToFile();
    return true;
}

bool ProfileMenu::acceptFriendRequest(const char* from, const char* to) {
    int toIndex = findPlayerIndex(to);
    if (toIndex == -1) {
        return false;
    }
    PendingRequest* current = players[toIndex]->requests;
    PendingRequest* prev = nullptr;
    while (current && strcmp(current->from, from) != 0) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        return false; // Requests not found
    }
    if (prev) {
        prev->next = current->next;
    }
    else {
        players[toIndex]->requests = current->next;
    }
    delete current;

    FriendNode* newFriend1 = new FriendNode(to);
    newFriend1->next = players[findPlayerIndex(from)]->friends;
    players[findPlayerIndex(from)]->friends = newFriend1;

    FriendNode* newFriend2 = new FriendNode(from);
    newFriend2->next = players[toIndex]->friends;
    players[toIndex]->friends = newFriend2;

    saveFriendsToFile();
    return true;
}

bool ProfileMenu::rejectFriendRequest(const char* from, const char* to) {
    int toIndex = findPlayerIndex(to);
    if (toIndex == -1) {
        return false;
    }
    PendingRequest* current = players[toIndex]->requests;
    PendingRequest* prev = nullptr;
    while (current && strcmp(current->from, from) != 0) {
        prev = current;
        current = current->next;
    }
    if (!current) {
        return false; // Requests not found
    }
    if (prev) {
        prev->next = current->next;
    }
    else {
        players[toIndex]->requests = current->next;
    }
    delete current;
    saveFriendsToFile();
    return true;
}

void ProfileMenu::getPendingRequests(char* output, int maxLen) {
    output[0] = '\0';
    int toIndex = findPlayerIndex(currentUser.c_str());
    if (toIndex == -1) {
        return;
    }
    PendingRequest* current = players[toIndex]->requests;
    while (current) {
        int currentLen = 0;
        while (output[currentLen] != '\0') ++currentLen;
        if (currentLen + strlen(current->from) + 2 < maxLen) {
            manual_str_append(output, current->from, maxLen);
            manual_str_append(output, " ", maxLen);
        }
        current = current->next;
    }
}

void ProfileMenu::getFriendsList(char* output, int maxLen) {
    output[0] = '\0';
    int index = findPlayerIndex(currentUser.c_str());
    if (index == -1) {
        return;
    }
    FriendNode* current = players[index]->friends;
    while (current) {
        int currentLen = 0;
        while (output[currentLen] != '\0') ++currentLen;
        if (currentLen + strlen(current->username) + 2 < maxLen) {
            manual_str_append(output, current->username, maxLen);
            manual_str_append(output, "\n", maxLen);
        }
        current = current->next;
    }
}

bool ProfileMenu::isFriend(const char* username1, const char* username2) {
    int index1 = findPlayerIndex(username1);
    if (index1 == -1) {
        return false;
    }
    FriendNode* current = players[index1]->friends;
    while (current) {
        if (strcmp(current->username, username2) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool ProfileMenu::hasPendingRequest(const char* from, const char* to) {
    int toIndex = findPlayerIndex(to);
    if (toIndex == -1) {
        return false;
    }
    PendingRequest* current = players[toIndex]->requests;
    while (current) {
        if (strcmp(current->from, from) == 0) {
            return true;
        }
        current = current->next;
    }
    return false;
}

bool ProfileMenu::show(sf::RenderWindow& window) {
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return false;
            }

            if (event.type == sf::Event::MouseMoved) {
                sf::Vector2f mousePos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
                updateSelected(mousePos);
            }

            if (event.type == sf::Event::MouseButtonReleased) {
                if (selected == 2) { // Playing Game
                    return true;
                }
                else if (selected == 0) { // Back
                    return false;
                }
                else if (selected == 1) { // Friends List
                    sf::RenderWindow friendsWindow(sf::VideoMode(960, 720), "Friends List");
                    sf::Texture bgTexture;
                    if (bgTexture.loadFromFile("images/pause_bg.jpg")) {
                        sf::Sprite bgSprite(bgTexture);
                        bgSprite.setPosition(150, 50);

                        sf::Font font;
                        if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
                            std::cerr << "Error loading font for friends window!\n";
                        }

                        sf::RectangleShape mainPanel(sf::Vector2f(800, 600));
                        mainPanel.setFillColor(sf::Color(30, 30, 30, 220));
                        mainPanel.setPosition(80, 60);

                        sf::Text searchLabel("Search Friend:", font, 30);
                        searchLabel.setPosition(100, 80);
                        searchLabel.setFillColor(sf::Color::White);

                        sf::RectangleShape searchBox(sf::Vector2f(400, 40));
                        searchBox.setFillColor(sf::Color::White);
                        searchBox.setPosition(100, 120);

                        sf::Text searchText("", font, 28);
                        searchText.setFillColor(sf::Color::Black);
                        searchText.setPosition(110, 125);

                        sf::RectangleShape sendButton(sf::Vector2f(180, 40));
                        sendButton.setFillColor(sf::Color(0, 150, 0));
                        sendButton.setPosition(520, 120);

                        sf::Text sendButtonText("SEND REQUEST", font, 24);
                        sendButtonText.setFillColor(sf::Color::White);
                        sendButtonText.setPosition(530, 127);

                        sf::Text pendingLabel("Pending Requests:", font, 30);
                        pendingLabel.setPosition(100, 180);
                        pendingLabel.setFillColor(sf::Color::White);

                        sf::RectangleShape pendingBox(sf::Vector2f(700, 200));
                        pendingBox.setFillColor(sf::Color(40, 40, 40, 200));
                        pendingBox.setPosition(100, 220);

                        sf::Text friendsLabel("Your Friends:", font, 30);
                        friendsLabel.setPosition(100, 440);
                        friendsLabel.setFillColor(sf::Color::White);

                        sf::RectangleShape friendsBox(sf::Vector2f(700, 200));
                        friendsBox.setFillColor(sf::Color(40, 40, 40, 200));
                        friendsBox.setPosition(100, 480);

                        sf::RectangleShape statusBar(sf::Vector2f(700, 30));
                        statusBar.setFillColor(sf::Color(0, 0, 0, 200));
                        statusBar.setPosition(100, 660);

                        sf::Text statusText("", font, 24);
                        statusText.setFillColor(sf::Color::Yellow);
                        statusText.setPosition(110, 663);

                        std::string searchInput;
                        bool isTypingSearch = false;

                        while (friendsWindow.isOpen()) {
                            char pendingRequests[1000] = "";
                            char friendsList[1000] = "";
                            getPendingRequests(pendingRequests, 1000);
                            getFriendsList(friendsList, 1000);

                            sf::Event event;
                            while (friendsWindow.pollEvent(event)) {
                                if (event.type == sf::Event::Closed ||
                                    (event.type == sf::Event::KeyPressed &&
                                        event.key.code == sf::Keyboard::Escape)) {
                                    friendsWindow.close();
                                }

                                if (event.type == sf::Event::MouseButtonPressed) {
                                    sf::Vector2f mousePos = friendsWindow.mapPixelToCoords(
                                        sf::Mouse::getPosition(friendsWindow));
                                    isTypingSearch = searchBox.getGlobalBounds().contains(mousePos);
                                }

                                if (event.type == sf::Event::TextEntered && isTypingSearch) {
                                    if (event.text.unicode == 8) {
                                        if (!searchInput.empty()) searchInput.pop_back();
                                    }
                                    else if (event.text.unicode < 128) {
                                        searchInput += static_cast<char>(event.text.unicode);
                                    }
                                }

                                if (event.type == sf::Event::MouseButtonReleased) {
                                    sf::Vector2f mousePos = friendsWindow.mapPixelToCoords(
                                        sf::Mouse::getPosition(friendsWindow));

                                    if (sendButton.getGlobalBounds().contains(mousePos)) {
                                        if (searchInput.empty()) {
                                            statusText.setString("Please enter a username!");
                                        }
                                        else if (searchInput == currentUser) {
                                            statusText.setString("Cannot add yourself!");
                                        }
                                        else if (findPlayerIndex(searchInput.c_str()) == -1) {
                                            statusText.setString("User not found!");
                                        }
                                        else if (sendFriendRequest(currentUser.c_str(), searchInput.c_str())) {
                                            statusText.setString("Request sent to " + searchInput);
                                            searchInput.clear();
                                        }
                                        else {
                                            statusText.setString("Request already sent or already friends!");
                                        }
                                    }

                                    float requestY = 230;
                                    char pendingCopy[1000];
                                    manual_str_copy(pendingCopy, pendingRequests, 1000);
                                    char* context = nullptr;
                                    char* token = manual_str_tokenize(pendingCopy, " ", &context);
                                    while (token) {
                                        sf::FloatRect acceptArea(650, requestY, 100, 30);
                                        sf::FloatRect rejectArea(770, requestY, 100, 30);

                                        if (acceptArea.contains(mousePos)) {
                                            if (acceptFriendRequest(token, currentUser.c_str())) {
                                                statusText.setString("Accepted request from " + string(token));
                                            }
                                        }
                                        if (rejectArea.contains(mousePos)) {
                                            if (rejectFriendRequest(token, currentUser.c_str())) {
                                                statusText.setString("Rejected request from " + string(token));
                                            }
                                        }

                                        requestY += 40;
                                        token = manual_str_tokenize(nullptr, " ", &context);
                                    }
                                }
                            }

                            searchText.setString(searchInput);

                            friendsWindow.clear(sf::Color(25, 25, 25));
                            friendsWindow.draw(bgSprite);
                            friendsWindow.draw(mainPanel);

                            friendsWindow.draw(searchLabel);
                            friendsWindow.draw(searchBox);
                            friendsWindow.draw(searchText);
                            friendsWindow.draw(sendButton);
                            friendsWindow.draw(sendButtonText);

                            friendsWindow.draw(pendingLabel);
                            friendsWindow.draw(pendingBox);

                            float requestY = 230;
                            char pendingCopy[1000];
                            manual_str_copy(pendingCopy, pendingRequests, 1000);
                            char* context = nullptr;
                            char* token = manual_str_tokenize(pendingCopy, " ", &context);
                            while (token) {
                                sf::Text requestText(token, font, 26);
                                requestText.setPosition(110, requestY);
                                requestText.setFillColor(sf::Color::White);
                                friendsWindow.draw(requestText);

                                sf::RectangleShape acceptButton(sf::Vector2f(100, 30));
                                acceptButton.setFillColor(sf::Color(50, 200, 50));
                                acceptButton.setPosition(650, requestY);

                                sf::RectangleShape rejectButton(sf::Vector2f(100, 30));
                                rejectButton.setFillColor(sf::Color(200, 50, 50));
                                rejectButton.setPosition(770, requestY);

                                friendsWindow.draw(acceptButton);
                                friendsWindow.draw(rejectButton);

                                sf::Text acceptLabel("Accept", font, 20);
                                acceptLabel.setPosition(660, requestY + 5);
                                sf::Text rejectLabel("Reject", font, 20);
                                rejectLabel.setPosition(780, requestY + 5);

                                friendsWindow.draw(acceptLabel);
                                friendsWindow.draw(rejectLabel);

                                requestY += 40;
                                token = manual_str_tokenize(nullptr, " ", &context);
                            }

                            friendsWindow.draw(friendsLabel);
                            friendsWindow.draw(friendsBox);

                            float friendY = 490;
                            char friendsCopy[1000];
                            manual_str_copy(friendsCopy, friendsList, 1000);
                            context = nullptr;
                            token = manual_str_tokenize(friendsCopy, "\n", &context);
                            while (token) {
                                sf::Text friendText(token, font, 26);
                                friendText.setPosition(110, friendY);
                                friendText.setFillColor(sf::Color::White);
                                friendsWindow.draw(friendText);

                                friendY += 40;
                                token = manual_str_tokenize(nullptr, "\n", &context);
                            }

                            friendsWindow.draw(statusBar);
                            friendsWindow.draw(statusText);

                            friendsWindow.display();
                        }
                    }
                }
            }
        }

        backButton.setFillColor(selected == 0 ? sf::Color(200, 0, 0) : sf::Color(150, 0, 0));
        friendsListButton.setFillColor(selected == 1 ? sf::Color(0, 0, 200) : sf::Color(0, 0, 150));
        playButton.setFillColor(selected == 2 ? sf::Color(0, 200, 0) : sf::Color(0, 150, 0));

        window.clear();
        window.draw(background);
        window.draw(titleText);
        window.draw(usernameText);
        window.draw(userIdText);
        window.draw(loginCountText);
        window.draw(leaderboardText);
        window.draw(totalPointsText);
        window.draw(matchHistoryText);
        window.draw(playButton);
        window.draw(backButton);
        window.draw(friendsListButton);
        window.draw(playText);
        window.draw(backText);
        window.draw(friendsListText);
        window.display();
    }
    return false;
}