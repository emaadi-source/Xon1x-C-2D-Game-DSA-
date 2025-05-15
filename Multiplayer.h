#pragma once
#include "LoginWindow.h"
#include "Source.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <fstream>

using namespace sf;
using namespace std;

enum TrailState {
    EMPTY = 0,
    COMPLETED = 1,  // Blue trail
    INCOMPLETE = 2  // Green trail
};

const int M = 44;
const int N = 64;
const int ts = 15;

class Tournament;

class MultiplayerModeMenu {
private:
    Font font;
    Texture bgTexture;
    Sprite bgSprite;
    int selectedItemIndex;
    Text modeMenu[2]; // Two options: 1v1 and Tournament

public:
    MultiplayerModeMenu() {
        if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
            cout << "Failed to load multiplayer mode menu font!\n";
        }

        if (!bgTexture.loadFromFile("images/pause_bg.jpg")) {
            cout << "Failed to load multiplayer mode menu background!\n";
        }
        bgSprite.setTexture(bgTexture);

        // Initializing menu items
        modeMenu[0].setFont(font);
        modeMenu[0].setFillColor(Color::Yellow);
        modeMenu[0].setString("1 vs 1");
        modeMenu[0].setCharacterSize(70);
        modeMenu[0].setPosition(360, 300);

        modeMenu[1].setFont(font);
        modeMenu[1].setFillColor(Color::White);
        modeMenu[1].setString("Tournament");
        modeMenu[1].setCharacterSize(70);
        modeMenu[1].setPosition(300, 400);

        selectedItemIndex = 0;
    }

    void draw(RenderWindow& window) {
        window.draw(bgSprite);

        Text title("CHOOSE MULTIPLAYER MODE", font, 50);
        title.setPosition(200, 150);
        title.setFillColor(Color::Green);
        window.draw(title);

        for (int i = 0; i < 2; i++) {
            window.draw(modeMenu[i]);
        }
    }

    void MoveUp() {
        if (selectedItemIndex - 1 >= 0) {
            modeMenu[selectedItemIndex].setFillColor(Color::White);
            selectedItemIndex--;
            modeMenu[selectedItemIndex].setFillColor(Color::Yellow);
        }
    }

    void MoveDown() {
        if (selectedItemIndex + 1 < 2) {
            modeMenu[selectedItemIndex].setFillColor(Color::White);
            selectedItemIndex++;
            modeMenu[selectedItemIndex].setFillColor(Color::Yellow);
        }
    }

    int getSelectedItem() { return selectedItemIndex; }
};

// Priority Queue Node
struct PlayerNode {
    string username;
    int score;
    PlayerNode* next;

    PlayerNode(const string& uname, int scr) : username(uname), score(scr), next(nullptr) {}
};

// Priority Queue for Matchmaking
class PlayerPriorityQueue {
private:
    PlayerNode* head;
    int size;

public:
    PlayerPriorityQueue() : head(nullptr), size(0) {}

    ~PlayerPriorityQueue() {
        while (head) {
            PlayerNode* temp = head;
            head = head->next;
            delete temp;
        }
    }

    void enqueue(const string& username, int score) {
        PlayerNode* newNode = new PlayerNode(username, score);

        if (!head || score > head->score) {
            newNode->next = head;
            head = newNode;
        }
        else {
            PlayerNode* current = head;
            while (current->next && score <= current->next->score) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
        size++;
    }

    bool dequeue(string& username, int& score) {
        if (!head) return false;

        PlayerNode* temp = head;
        username = head->username;
        score = head->score;
        head = head->next;
        delete temp;
        size--;
        return true;
    }

    bool peekTop5(string topPlayers[5], int topScores[5]) {
        if (!head) return false;

        PlayerNode* current = head;
        for (int i = 0; i < 5 && current; i++) {
            topPlayers[i] = current->username;
            topScores[i] = current->score;
            current = current->next;
        }
        return true;
    }

    bool isEmpty() { return head == nullptr; }
    int getSize() { return size; }
};

//  Multiplayer class 
class Multiplayer {
private:
    struct Player {
        string name;
        int score;
        int collisions;
        bool moved;
    };

    Player player1, player2;
    Clock gameClock;
    Font font;
    Texture bgTexture;
    Sprite bgSprite;

    void drop(int y, int x, int grid[M][N]) {
        if (y < 0 || x < 0 || y >= M || x >= N) return;
        if (grid[y][x] != 0) return;

        grid[y][x] = -1;

        drop(y - 1, x, grid);
        drop(y + 1, x, grid);
        drop(y, x - 1, grid);
        drop(y, x + 1, grid);
    }

    bool handleTournamentMode(RenderWindow& window);

public:
    //  getter and setter methods for player1 and player2
    Player& getPlayer1() { return player1; }
    Player& getPlayer2() { return player2; }
    void setPlayer1(const Player& p) { player1 = p; }
    void setPlayer2(const Player& p) { player2 = p; }

    Multiplayer() {
        if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
            cout << "Failed to load multiplayer font!\n";
        }

        if (!bgTexture.loadFromFile("images/pause_bg.jpg")) {
            cout << "Failed to load multiplayer background!\n";
        }
        bgSprite.setTexture(bgTexture);
    }

    bool showPlayerSubmission(RenderWindow& window) {
        RenderWindow modeWindow(VideoMode(960, 720), "Multiplayer Mode");
        MultiplayerModeMenu modeMenu;
        SoundBuffer navigationBuffer, selectionBuffer;
        Sound navigationSound, selectionSound;

        if (!navigationBuffer.loadFromFile("audios/navigation.mp3")) {
            cout << "Failed to load navigation sound.\n";
        }
        navigationSound.setBuffer(navigationBuffer);

        if (!selectionBuffer.loadFromFile("audios/selection.mp3")) {
            cout << "Failed to load selection sound.\n";
        }
        selectionSound.setBuffer(selectionBuffer);

        int selectedMode = -1;

        while (modeWindow.isOpen()) {
            Event event;
            while (modeWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    modeWindow.close();
                    return false;
                }

                if (event.type == Event::KeyReleased) {
                    if (event.key.code == Keyboard::Up) {
                        navigationSound.play();
                        modeMenu.MoveUp();
                    }
                    else if (event.key.code == Keyboard::Down) {
                        navigationSound.play();
                        modeMenu.MoveDown();
                    }
                    else if (event.key.code == Keyboard::Return) {
                        selectionSound.play();
                        selectedMode = modeMenu.getSelectedItem();
                        modeWindow.close();
                    }
                    else if (event.key.code == Keyboard::Escape) {
                        modeWindow.close();
                        return false;
                    }
                }
            }

            modeWindow.clear();
            modeMenu.draw(modeWindow);
            modeWindow.display();
        }

        if (selectedMode == -1) return false; // User exited without selecting

        if (selectedMode == 1) { // Tournament mode
            return handleTournamentMode(window);
        }

       // if (selectedMode == 1) { // Tournament mode
         //   Tournament tournament;
           // if (tournament.showLogin(window)) {
             //   tournament.showGameRoom();
            //}
            //return false;
        //}

        RenderWindow submissionWindow(VideoMode(960, 720), "Multiplayer Login");

        // Player 1 elements
        Text player1Text("PLAYER 1", font, 50);
        player1Text.setPosition(150, 100);
        player1Text.setFillColor(Color::Yellow);

        Text p1NameText("Enter name:", font, 30);
        p1NameText.setPosition(150, 200);

        RectangleShape p1NameBox(Vector2f(250, 40));
        p1NameBox.setPosition(150, 240);
        p1NameBox.setFillColor(Color::White);
        p1NameBox.setOutlineThickness(2);
        p1NameBox.setOutlineColor(Color::Black);

        Text p1NameInput("", font, 30);
        p1NameInput.setPosition(155, 245);
        p1NameInput.setFillColor(Color::Black);

        Text p1PassText("Enter password:", font, 30);
        p1PassText.setPosition(150, 300);

        RectangleShape p1PassBox(Vector2f(250, 40));
        p1PassBox.setPosition(150, 340);
        p1PassBox.setFillColor(Color::White);
        p1PassBox.setOutlineThickness(2);
        p1PassBox.setOutlineColor(Color::Black);

        Text p1PassInput("", font, 30);
        p1PassInput.setPosition(155, 345);
        p1PassInput.setFillColor(Color::Black);

        RectangleShape p1LoginBtn(Vector2f(120, 50));
        p1LoginBtn.setPosition(120, 420);
        p1LoginBtn.setFillColor(Color(100, 250, 50));

        Text p1LoginText("LOGIN", font, 30);
        p1LoginText.setPosition(135, 430);
        p1LoginText.setFillColor(Color::Black);

        RectangleShape p1SignupBtn(Vector2f(120, 50));
        p1SignupBtn.setPosition(300, 420);
        p1SignupBtn.setFillColor(Color(50, 150, 250));

        Text p1SignupText("SIGNUP", font, 30);
        p1SignupText.setPosition(310, 430);
        p1SignupText.setFillColor(Color::White);

        Text p1Status("", font, 25);
        p1Status.setPosition(150, 490);

        // Player 2 elements
        Text player2Text("PLAYER 2", font, 50);
        player2Text.setPosition(550, 100);
        player2Text.setFillColor(Color::Yellow);

        Text p2NameText("Enter name:", font, 30);
        p2NameText.setPosition(550, 200);

        RectangleShape p2NameBox(Vector2f(250, 40));
        p2NameBox.setPosition(550, 240);
        p2NameBox.setFillColor(Color::White);
        p2NameBox.setOutlineThickness(2);
        p2NameBox.setOutlineColor(Color::Black);

        Text p2NameInput("", font, 30);
        p2NameInput.setPosition(555, 245);
        p2NameInput.setFillColor(Color::Black);

        Text p2PassText("Enter password:", font, 30);
        p2PassText.setPosition(550, 300);

        RectangleShape p2PassBox(Vector2f(250, 40));
        p2PassBox.setPosition(550, 340);
        p2PassBox.setFillColor(Color::White);
        p2PassBox.setOutlineThickness(2);
        p2PassBox.setOutlineColor(Color::Black);

        Text p2PassInput("", font, 30);
        p2PassInput.setPosition(555, 345);
        p2PassInput.setFillColor(Color::Black);

        RectangleShape p2LoginBtn(Vector2f(120, 50));
        p2LoginBtn.setPosition(530, 420);
        p2LoginBtn.setFillColor(Color(100, 250, 50));

        Text p2LoginText("LOGIN", font, 30);
        p2LoginText.setPosition(545, 430);
        p2LoginText.setFillColor(Color::Black);

        RectangleShape p2SignupBtn(Vector2f(120, 50));
        p2SignupBtn.setPosition(700, 420);
        p2SignupBtn.setFillColor(Color(50, 150, 250));

        Text p2SignupText("SIGNUP", font, 30);
        p2SignupText.setPosition(710, 430);
        p2SignupText.setFillColor(Color::White);

        Text p2Status("", font, 25);
        p2Status.setPosition(550, 490);

        // Divider line
        RectangleShape divider(Vector2f(2, 450));
        divider.setPosition(480, 60);
        divider.setFillColor(Color::White);

        // Start game button
        RectangleShape startBtn(Vector2f(345, 60));
        startBtn.setPosition(374, 590);
        startBtn.setFillColor(Color(150, 150, 150));

        Text startText("START GAME", font, 35);
        startText.setPosition(390, 600);
        startText.setFillColor(Color::Black);

        bool p1LoggedIn = false;
        bool p2LoggedIn = false;
        string p1Name = "", p1Pass = "";
        string p2Name = "", p2Pass = "";

        while (submissionWindow.isOpen()) {
            Event event;
            while (submissionWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    submissionWindow.close();
                    return false;
                }

                // Handles text input for both players
                if (event.type == Event::TextEntered) {
                    if (p1NameBox.getGlobalBounds().contains(Mouse::getPosition(submissionWindow).x, Mouse::getPosition(submissionWindow).y)) {
                        if (event.text.unicode == '\b' && !p1Name.empty()) {
                            p1Name.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            p1Name += static_cast<char>(event.text.unicode);
                        }
                        p1NameInput.setString(p1Name);
                    }
                    else if (p1PassBox.getGlobalBounds().contains(Mouse::getPosition(submissionWindow).x, Mouse::getPosition(submissionWindow).y)) {
                        if (event.text.unicode == '\b' && !p1Pass.empty()) {
                            p1Pass.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            p1Pass += static_cast<char>(event.text.unicode);
                        }
                        p1PassInput.setString(string(p1Pass.length(), '*'));
                    }
                    else if (p2NameBox.getGlobalBounds().contains(Mouse::getPosition(submissionWindow).x, Mouse::getPosition(submissionWindow).y)) {
                        if (event.text.unicode == '\b' && !p2Name.empty()) {
                            p2Name.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            p2Name += static_cast<char>(event.text.unicode);
                        }
                        p2NameInput.setString(p2Name);
                    }
                    else if (p2PassBox.getGlobalBounds().contains(Mouse::getPosition(submissionWindow).x, Mouse::getPosition(submissionWindow).y)) {
                        if (event.text.unicode == '\b' && !p2Pass.empty()) {
                            p2Pass.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            p2Pass += static_cast<char>(event.text.unicode);
                        }
                        p2PassInput.setString(string(p2Pass.length(), '*'));
                    }
                }

                // Handle button clicks
                if (event.type == Event::MouseButtonReleased) {
                    Vector2f mousePos = submissionWindow.mapPixelToCoords(Mouse::getPosition(submissionWindow));

                    // Player 1 buttons
                    if (p1LoginBtn.getGlobalBounds().contains(mousePos) && !p1LoggedIn) {
                        if (p1Name.empty() || p1Pass.empty()) {
                            p1Status.setString("Fields cannot be empty!");
                            p1Status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::validateUser(p1Name.c_str(), p1Pass.c_str())) {
                            p1Status.setString("Login successful!");
                            p1Status.setFillColor(Color::Green);
                            p1LoggedIn = true;
                            player1.name = p1Name;
                        }
                        else {
                            p1Status.setString("Invalid credentials!");
                            p1Status.setFillColor(Color::Red);
                        }
                    }
                    else if (p1SignupBtn.getGlobalBounds().contains(mousePos) && !p1LoggedIn) {
                        if (p1Name.empty() || p1Pass.empty()) {
                            p1Status.setString("Fields cannot be empty!");
                            p1Status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::registerUser(p1Name.c_str(), p1Pass.c_str())) {
                            p1Status.setString("Signup successful!");
                            p1Status.setFillColor(Color::Green);
                            p1LoggedIn = true;
                            player1.name = p1Name;
                        }
                        else {
                            p1Status.setString("Username exists!");
                            p1Status.setFillColor(Color::Red);
                        }
                    }

                    // Player 2 buttons
                    if (p2LoginBtn.getGlobalBounds().contains(mousePos) && !p2LoggedIn) {
                        if (p2Name.empty() || p2Pass.empty()) {
                            p2Status.setString("Fields cannot be empty!");
                            p2Status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::validateUser(p2Name.c_str(), p2Pass.c_str())) {
                            p2Status.setString("Login successful!");
                            p2Status.setFillColor(Color::Green);
                            p2LoggedIn = true;
                            player2.name = p2Name;
                        }
                        else {
                            p2Status.setString("Invalid credentials!");
                            p2Status.setFillColor(Color::Red);
                        }
                    }
                    else if (p2SignupBtn.getGlobalBounds().contains(mousePos) && !p2LoggedIn) {
                        if (p2Name.empty() || p2Pass.empty()) {
                            p2Status.setString("Fields cannot be empty!");
                            p2Status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::registerUser(p2Name.c_str(), p2Pass.c_str())) {
                            p2Status.setString("Signup successful!");
                            p2Status.setFillColor(Color::Green);
                            p2LoggedIn = true;
                            player2.name = p2Name;
                        }
                        else {
                            p2Status.setString("Username exists!");
                            p2Status.setFillColor(Color::Red);
                        }
                    }

                    // Start game button
                    if (startBtn.getGlobalBounds().contains(mousePos) && p1LoggedIn && p2LoggedIn) {
                        submissionWindow.close();
                        return true;
                    }
                }
            }

            // Updates start button appearance based on login status
            if (p1LoggedIn && p2LoggedIn) {
                startBtn.setFillColor(Color(100, 250, 50));
                startText.setString("START GAME!");
            }
            else {
                startBtn.setFillColor(Color(150, 150, 150));
                startText.setString("NEED BOTH PLAYERS");
            }

            // Drawing everything
            submissionWindow.clear();
            submissionWindow.draw(bgSprite);
            submissionWindow.draw(player1Text);
            submissionWindow.draw(p1NameText);
            submissionWindow.draw(p1NameBox);
            submissionWindow.draw(p1NameInput);
            submissionWindow.draw(p1PassText);
            submissionWindow.draw(p1PassBox);
            submissionWindow.draw(p1PassInput);
            submissionWindow.draw(p1LoginBtn);
            submissionWindow.draw(p1LoginText);
            submissionWindow.draw(p1SignupBtn);
            submissionWindow.draw(p1SignupText);
            submissionWindow.draw(p1Status);

            submissionWindow.draw(player2Text);
            submissionWindow.draw(p2NameText);
            submissionWindow.draw(p2NameBox);
            submissionWindow.draw(p2NameInput);
            submissionWindow.draw(p2PassText);
            submissionWindow.draw(p2PassBox);
            submissionWindow.draw(p2PassInput);
            submissionWindow.draw(p2LoginBtn);
            submissionWindow.draw(p2LoginText);
            submissionWindow.draw(p2SignupBtn);
            submissionWindow.draw(p2SignupText);
            submissionWindow.draw(p2Status);

            submissionWindow.draw(divider);
            submissionWindow.draw(startBtn);
            submissionWindow.draw(startText);
            submissionWindow.display();
        }
        return false;
    }

    void startMultiplayerGame() {
        int grid[M][N] = { 0 };

        int** gridPtr = new int* [M];
        for (int i = 0; i < M; ++i) {
            gridPtr[i] = grid[i];
        }

        // Initializing game state
        player1.score = 0;
        player1.collisions = 0;
        player1.moved = false;
        player2.score = 0;
        player2.collisions = 0;
        player2.moved = false;
        gameClock.restart();

        SoundBuffer bufferMultiplayerMode;
        bufferMultiplayerMode.loadFromFile("audios/multiplayer.mp3");
        Sound soundMultiplayerMode;
        soundMultiplayerMode.setBuffer(bufferMultiplayerMode);
        soundMultiplayerMode.play();

        // Creates game window
        RenderWindow gameWindow(VideoMode(960, 720), "Xonix Multiplayer!");
        gameWindow.setFramerateLimit(60);

        // Loads textures
        Texture t1, t2, t3;
        t1.loadFromFile("images/tiles.png");
        t2.loadFromFile("images/gameover.png");
        t3.loadFromFile("images/enemy.png");

        Sprite sTile(t1), sGameover(t2), sEnemy(t3);
        sGameover.setPosition(100, 100);
        sEnemy.setOrigin(20, 20);

        // Sounds for collision
        SoundBuffer bufferLifeLost;
        bufferLifeLost.loadFromFile("audios/life_lost.mp3");
        Sound soundLifeLost;
        soundLifeLost.setBuffer(bufferLifeLost);

        // Initializes players
        struct PlayerGame {
            int x, y, dx, dy;
            bool active;
            int trailStartX, trailStartY;
        } p1, p2;

        p1.x = 10; p1.y = 10; p1.dx = p1.dy = 0; p1.active = true;
        p1.trailStartX = p1.x; p1.trailStartY = p1.y;

        p2.x = N - 10; p2.y = 10; p2.dx = p2.dy = 0; p2.active = true;
        p2.trailStartX = p2.x; p2.trailStartY = p2.y;

        // Initializes enemies
        const int enemyCount = 6;
        Enemy enemies[enemyCount];

        for (int i = 0; i < enemyCount; i++) {
            enemies[i].x = (rand() % (N - 2) + 1) * ts;
            enemies[i].y = (rand() % (M - 2) + 1) * ts;
            enemies[i].dx = 4 - rand() % 8;
            enemies[i].dy = 4 - rand() % 8;
        }

        // Initializes grids
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < N; j++) {
                grid[i][j] = (i == 0 || j == 0 || i == M - 1 || j == N - 1) ? COMPLETED : EMPTY;
            }
        }

        // Game loop
        bool Game = true;
        float timer = 0, delay = 0.07f;
        Clock clock;

        while (gameWindow.isOpen() && Game) {
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;

            // Checks game time (40 seconds)
            float remainingTime = static_cast<float>(40.0 - gameClock.getElapsedTime().asSeconds());
            if (remainingTime <= 0) {
                Game = false;
                gameWindow.close();
            }

            Event e;
            while (gameWindow.pollEvent(e)) {
                if (e.type == Event::Closed) {
                    gameWindow.close();
                }

                // Player 1 controls (WASD)
                if (e.type == Event::KeyPressed) {
                    if (e.key.code == Keyboard::A) { p1.dx = -1; p1.dy = 0; player1.moved = true; }
                    if (e.key.code == Keyboard::D) { p1.dx = 1; p1.dy = 0; player1.moved = true; }
                    if (e.key.code == Keyboard::W) { p1.dx = 0; p1.dy = -1; player1.moved = true; }
                    if (e.key.code == Keyboard::S) { p1.dx = 0; p1.dy = 1; player1.moved = true; }

                    // Player 2 controls (Arrow keys)
                    if (e.key.code == Keyboard::Left) { p2.dx = -1; p2.dy = 0; player2.moved = true; }
                    if (e.key.code == Keyboard::Right) { p2.dx = 1; p2.dy = 0; player2.moved = true; }
                    if (e.key.code == Keyboard::Up) { p2.dx = 0; p2.dy = -1; player2.moved = true; }
                    if (e.key.code == Keyboard::Down) { p2.dx = 0; p2.dy = 1; player2.moved = true; }
                }

                // Stops movement when key is released
                if (e.type == Event::KeyReleased) {
                    if (e.key.code == Keyboard::A || e.key.code == Keyboard::D ||
                        e.key.code == Keyboard::W || e.key.code == Keyboard::S) {
                        player1.moved = false;
                    }
                    if (e.key.code == Keyboard::Left || e.key.code == Keyboard::Right ||
                        e.key.code == Keyboard::Up || e.key.code == Keyboard::Down) {
                        player2.moved = false;
                    }
                }
            }

            if (timer > delay) {
                // Move players only if they're actively moving
                if (p1.active && player1.moved) {
                    p1.x += p1.dx;
                    p1.y += p1.dy;

                    // Boundary checks
                    if (p1.x < 0) p1.x = 0;
                    if (p1.x > N - 1) p1.x = N - 1;
                    if (p1.y < 0) p1.y = 0;
                    if (p1.y > M - 1) p1.y = M - 1;

                    // Collision detections for player 1
                    if (grid[p1.y][p1.x] == 2) {
                        soundLifeLost.play();
                        player1.score = max(0, player1.score - 2);
                        player1.collisions++;

                        // Resets only the current trail
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == 2) grid[i][j] = 0;
                            }
                        }

                        // Returns player to start of trail
                        p1.x = p1.trailStartX;
                        p1.y = p1.trailStartY;
                        p1.dx = p1.dy = 0;
                        player1.moved = false;
                    }
                    else if (grid[p1.y][p1.x] == 0) {
                        // If starting a new trail, records the start position
                        if (!player1.moved || (p1.dx == 0 && p1.dy == 0)) {
                            p1.trailStartX = p1.x;
                            p1.trailStartY = p1.y;
                        }
                        grid[p1.y][p1.x] = INCOMPLETE; // Trail
                    }
                    else if (grid[p1.y][p1.x] == 1) {
                        p1.dx = p1.dy = 0;
                        player1.moved = false;

                        for (int i = 0; i < enemyCount; i++) {
                            drop(enemies[i].y / ts, enemies[i].x / ts, grid);
                        }

                        int area = 0;
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == -1) {
                                    grid[i][j] = EMPTY;
                                }
                                else if (grid[i][j] == 2) {
                                    grid[i][j] = COMPLETED;
                                    area++;
                                }
                            }
                        }

                        if (area > 0) {
                            int points = area;
                            if (area > 10) points *= 2;
                            player1.score += points;
                        }
                    }
                }

                if (p2.active && player2.moved) {
                    p2.x += p2.dx;
                    p2.y += p2.dy;

                    // Boundary checks
                    if (p2.x < 0) p2.x = 0;
                    if (p2.x > N - 1) p2.x = N - 1;
                    if (p2.y < 0) p2.y = 0;
                    if (p2.y > M - 1) p2.y = M - 1;

                    // Collision detections for player 2
                    if (grid[p2.y][p2.x] == 2) {
                        soundLifeLost.play();
                        player2.score = max(0, player2.score - 2);
                        player2.collisions++;

                        // Resets only the current trail
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == 2) grid[i][j] = 0;
                            }
                        }

                        // Return players to start of trail
                        p2.x = p2.trailStartX;
                        p2.y = p2.trailStartY;
                        p2.dx = p2.dy = 0;
                        player2.moved = false;
                    }
                    else if (grid[p2.y][p2.x] == 0) {
                        // If starting a new trail, records the start position
                        if (!player2.moved || (p2.dx == 0 && p2.dy == 0)) {
                            p2.trailStartX = p2.x;
                            p2.trailStartY = p2.y;
                        }
                        grid[p2.y][p2.x] = INCOMPLETE; // Trail
                    }
                    else if (grid[p2.y][p2.x] == 1) {
                        p2.dx = p2.dy = 0;
                        player2.moved = false;

                        for (int i = 0; i < enemyCount; i++) {
                            drop(enemies[i].y / ts, enemies[i].x / ts, grid);
                        }

                        int area = 0;
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == -1) {
                                    grid[i][j] = 0;
                                }
                                else if (grid[i][j] == 2) {
                                    grid[i][j] = 1;
                                    area++;
                                }
                            }
                        }

                        if (area > 0) {
                            int points = area;
                            if (area > 10) points *= 2;
                            player2.score += points;
                        }
                    }
                }

                // Checks for mid-trail collision
                bool p1InTrail = (grid[p1.y][p1.x] == 2);
                bool p2InTrail = (grid[p2.y][p2.x] == 2);

                // Players collided at same position before either touched boundary
                if (p1.x == p2.x && p1.y == p2.y && p1InTrail && p2InTrail) {
                    soundLifeLost.play();
                    player1.score = max(0, player1.score - 4);
                    player2.score = max(0, player2.score - 4);
                    player1.collisions++;
                    player2.collisions++;

                    // Stops both players
                    p1.dx = p1.dy = 0;
                    p2.dx = p2.dy = 0;
                    player1.moved = false;
                    player2.moved = false;
                }

                // Moves enemies and checks collisions
                for (int i = 0; i < enemyCount; i++) {
                    enemies[i].move(gridPtr, ts, M, N);

                    if (enemies[i].x / ts >= N) enemies[i].x = (N - 2) * ts;
                    if (enemies[i].y / ts >= M) enemies[i].y = (M - 2) * ts;

                    // Checks enemy positions in grid
                    int ex = enemies[i].x / ts;
                    int ey = enemies[i].y / ts;

                    // Checks if enemy is on any incomplete trail (green)
                    if (grid[ey][ex] == INCOMPLETE) {
                        soundLifeLost.play();

                        // Determines which player's trail was hit based on proximity
                        bool hitP1Trail = (abs(p1.x - ex) + abs(p1.y - ey) <
                            abs(p2.x - ex) + abs(p2.y - ey));

                        if (hitP1Trail) {
                            player1.score = max(0, player1.score - 1);
                            player1.collisions++;
                        }
                        else {
                            player2.score = max(0, player2.score - 1);
                            player2.collisions++;
                        }

                        // Reset all incomplete trails (both players)
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == INCOMPLETE) {
                                    grid[i][j] = EMPTY;
                                }
                            }
                        }

                        // Returns affected player to trail start
                        if (hitP1Trail) {
                            p1.x = p1.trailStartX;
                            p1.y = p1.trailStartY;
                            p1.dx = p1.dy = 0;
                            player1.moved = false;
                        }
                        else {
                            p2.x = p2.trailStartX;
                            p2.y = p2.trailStartY;
                            p2.dx = p2.dy = 0;
                            player2.moved = false;
                        }
                    }
                    if (ex == p1.x && ey == p1.y) {
                        soundLifeLost.play();
                        player1.score = max(0, player1.score - 1);
                        player1.collisions++;

                        // Resets only the current trail
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == 2) grid[i][j] = 0;
                            }
                        }

                        // Returns player to start of trail
                        p1.x = p1.trailStartX;
                        p1.y = p1.trailStartY;
                        p1.dx = p1.dy = 0;
                        player1.moved = false;
                    }

                    // Player 2 enemy collisions
                    int ex2 = enemies[i].x / ts;
                    int ey2 = enemies[i].y / ts;
                    if (ex == p2.x && ey == p2.y) {
                        soundLifeLost.play();
                        player2.score = max(0, player2.score - 1);
                        player2.collisions++;

                        // Resets only the current trail
                        for (int i = 0; i < M; i++) {
                            for (int j = 0; j < N; j++) {
                                if (grid[i][j] == 2) grid[i][j] = 0;
                            }
                        }

                        // Returns player to start of trail
                        p2.x = p2.trailStartX;
                        p2.y = p2.trailStartY;
                        p2.dx = p2.dy = 0;
                        player2.moved = false;
                    }
                }

                timer = 0;
            }

            // Draws game
            gameWindow.clear(Color(30, 30, 30));

            // Draws UI
            Text timerText("Time: " + to_string((int)remainingTime), font, 30);
            timerText.setPosition(gameWindow.getSize().x / 2 - timerText.getLocalBounds().width / 2, 10);
            timerText.setFillColor(Color::White);

            Text p1Text(player1.name + ": " + to_string(player1.score), font, 30);
            p1Text.setPosition(20, 10);
            p1Text.setFillColor(Color::Cyan);

            Text p2Text(player2.name + ": " + to_string(player2.score), font, 30);
            p2Text.setPosition(gameWindow.getSize().x - p2Text.getLocalBounds().width - 20, 10);
            p2Text.setFillColor(Color::Magenta);

            gameWindow.draw(timerText);
            gameWindow.draw(p1Text);
            gameWindow.draw(p2Text);

            // Draws player 1's and player's 2 grid 
            for (int i = 0; i < M; i++) {
                for (int j = 0; j < N; j++) {
                    if (grid[i][j] == 0) continue;
                    if (grid[i][j] == COMPLETED) sTile.setTextureRect(IntRect(0, 0, ts, ts));
                    if (grid[i][j] == INCOMPLETE) sTile.setTextureRect(IntRect(54, 0, ts, ts));
                    sTile.setPosition(j * ts, i * ts + 50);
                    gameWindow.draw(sTile);
                }
            }

            // Draws players
            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(p1.x * ts, p1.y * ts + 50);
            gameWindow.draw(sTile);

            sTile.setTextureRect(IntRect(36, 0, ts, ts));
            sTile.setPosition(p2.x * ts, p2.y * ts + 50);
            gameWindow.draw(sTile);

            // Draws enemies
            sEnemy.rotate(10);
            for (int i = 0; i < enemyCount; i++) {
                sEnemy.setPosition(enemies[i].x, enemies[i].y + 50);
                gameWindow.draw(sEnemy);
            }

            gameWindow.display();
        }

        // Game over - show results
        showResults();
    }

    void showResults() {
        RenderWindow resultsWindow(VideoMode(800, 520), "Game Results");

        SoundBuffer bufferGameOver;
        bufferGameOver.loadFromFile("audios/Game Over.mp3");
        Sound soundGameOver;
        soundGameOver.setBuffer(bufferGameOver);
        soundGameOver.play();

        // Background
        Texture bgTexture;
        bgTexture.loadFromFile("images/pause_bg.jpg");
        Sprite bgSprite(bgTexture);

        // Results text
        Text title("GAME RESULTS", font, 50);
        title.setPosition(245, 30);
        title.setFillColor(Color::Yellow);

        Text p1Result(player1.name + ": " + to_string(player1.score) + " points", font, 30);
        p1Result.setPosition(300, 120);
        p1Result.setFillColor(Color::Cyan);

        Text p1Collisions("Collisions: " + to_string(player1.collisions), font, 25);
        p1Collisions.setPosition(300, 160);

        Text p2Result(player2.name + ": " + to_string(player2.score) + " points", font, 30);
        p2Result.setPosition(300, 200);
        p2Result.setFillColor(Color::Magenta);

        Text p2Collisions("Collisions: " + to_string(player2.collisions), font, 25);
        p2Collisions.setPosition(300, 240);

        // Buttons
        RectangleShape menuBtn(Vector2f(200, 50));
        menuBtn.setPosition(300, 300);
        menuBtn.setFillColor(Color::Blue);

        Text menuText("MAIN MENU", font, 30);
        menuText.setPosition(310, 310);

        RectangleShape exitBtn(Vector2f(200, 50));
        exitBtn.setPosition(300, 390);
        exitBtn.setFillColor(Color::Red);

        Text exitText("EXIT", font, 30);
        exitText.setPosition(385, 400);

        // Updates leaderboard with player scores
        LoginWindow::updateUserScore(player1.name, player1.score);
        LoginWindow::updateUserScore(player2.name, player2.score);

        while (resultsWindow.isOpen()) {
            Event event;
            while (resultsWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    resultsWindow.close();
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    resultsWindow.close();
                    return; 
                }

                if (event.type == Event::MouseButtonReleased) {
                    Vector2f mousePos = resultsWindow.mapPixelToCoords(Mouse::getPosition(resultsWindow));

                    if (menuBtn.getGlobalBounds().contains(mousePos)) {
                        resultsWindow.close();
                        return; // Returns to main menu
                    }

                    if (exitBtn.getGlobalBounds().contains(mousePos)) {
                        resultsWindow.close();
                        exit(0); // Exits game
                    }
                }
            }

            resultsWindow.clear();
            resultsWindow.draw(bgSprite);
            resultsWindow.draw(title);
            resultsWindow.draw(p1Result);
            resultsWindow.draw(p1Collisions);
            resultsWindow.draw(p2Result);
            resultsWindow.draw(p2Collisions);
            resultsWindow.draw(menuBtn);
            resultsWindow.draw(menuText);
            resultsWindow.draw(exitBtn);
            resultsWindow.draw(exitText);
            resultsWindow.display();
        }
    }
};

class Tournament {
private:
    PlayerPriorityQueue matchmakingQueue;
    string currentPlayer;
    int currentPlayerScore;
    int round;
    int winCount; // Tracks consecutive wins
    Font font;
    Texture bgTexture;
    Sprite bgSprite;
    string playedOpponents[5]; // Fixed-size array to track played opponents
    int playedCount; // Number of played opponents

public:
    Tournament() : round(1), winCount(0), playedCount(0) {
        for (int i = 0; i < 5; i++) {
            playedOpponents[i] = ""; // Initialize array
        }

        if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
            cout << "Failed to load tournament font!\n";
        }

        if (!bgTexture.loadFromFile("images/pause_bg.jpg")) {
            cout << "Failed to load tournament background!\n";
        }
        bgSprite.setTexture(bgTexture);

        // Explicitly reload leaderboard
        LoginWindow::reloadLeaderboard();

        // Loads top players into queue
        PlayerScore topPlayers[10];
        int count;
        LoginWindow::leaderboardHeap.getSorted(topPlayers, count);

        cout << "Tournament: Loaded " << count << " players from leaderboard\n"; // Debugs output
        for (int i = 0; i < count; i++) {
            cout << "Player " << i + 1 << ": " << topPlayers[i].username << ", Score: " << topPlayers[i].score << "\n";
            matchmakingQueue.enqueue(topPlayers[i].username, topPlayers[i].score);
        }
    }
    bool showLogin(RenderWindow& window) {
        RenderWindow loginWindow(VideoMode(960, 720), "Tournament Login");

        Text title("TOURNAMENT LOGIN", font, 50);
        title.setPosition(240, 80);
        title.setFillColor(Color::Yellow);

        Text nameText("Enter name:", font, 30);
        nameText.setPosition(300, 200);

        RectangleShape nameBox(Vector2f(300, 40));
        nameBox.setPosition(300, 240);
        nameBox.setFillColor(Color::White);
        nameBox.setOutlineThickness(2);
        nameBox.setOutlineColor(Color::Black);

        Text nameInput("", font, 30);
        nameInput.setPosition(305, 245);
        nameInput.setFillColor(Color::Black);

        Text passText("Enter password:", font, 30);
        passText.setPosition(300, 300);

        RectangleShape passBox(Vector2f(300, 40));
        passBox.setPosition(300, 340);
        passBox.setFillColor(Color::White);
        passBox.setOutlineThickness(2);
        passBox.setOutlineColor(Color::Black);

        Text passInput("", font, 30);
        passInput.setPosition(305, 345);
        passInput.setFillColor(Color::Black);

        RectangleShape loginBtn(Vector2f(150, 50));
        loginBtn.setPosition(265, 420);
        loginBtn.setFillColor(Color(100, 250, 50));

        Text loginText("LOGIN", font, 30);
        loginText.setPosition(290, 430);
        loginText.setFillColor(Color::Black);

        RectangleShape signupBtn(Vector2f(150, 50));
        signupBtn.setPosition(500, 420);
        signupBtn.setFillColor(Color(50, 150, 250));

        Text signupText("SIGNUP", font, 30);
        signupText.setPosition(520, 430);
        signupText.setFillColor(Color::White);

        Text status("", font, 25);
        status.setPosition(310, 490);

        string username, password;
        bool loggedIn = false;

        while (loginWindow.isOpen()) {
            Event event;
            while (loginWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    loginWindow.close();
                    return false;
                }
                if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape) {
                    loginWindow.close();
                    return false; 
                }

                if (event.type == Event::TextEntered) {
                    if (nameBox.getGlobalBounds().contains(Mouse::getPosition(loginWindow).x, Mouse::getPosition(loginWindow).y)) {
                        if (event.text.unicode == '\b' && !username.empty()) {
                            username.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            username += static_cast<char>(event.text.unicode);
                        }
                        nameInput.setString(username);
                    }
                    else if (passBox.getGlobalBounds().contains(Mouse::getPosition(loginWindow).x, Mouse::getPosition(loginWindow).y)) {
                        if (event.text.unicode == '\b' && !password.empty()) {
                            password.pop_back();
                        }
                        else if (event.text.unicode < 128 && event.text.unicode != '\r' && event.text.unicode != '\t') {
                            password += static_cast<char>(event.text.unicode);
                        }
                        passInput.setString(string(password.length(), '*'));
                    }
                }

                if (event.type == Event::MouseButtonReleased) {
                    Vector2f mousePos = loginWindow.mapPixelToCoords(Mouse::getPosition(loginWindow));

                    if (loginBtn.getGlobalBounds().contains(mousePos) && !loggedIn) {
                        if (username.empty() || password.empty()) {
                            status.setString("Fields cannot be empty!");
                            status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::validateUser(username.c_str(), password.c_str())) {
                            status.setString("Login successful!");
                            status.setFillColor(Color::Green);
                            loggedIn = true;
                            currentPlayer = username;
                            currentPlayerScore = LoginWindow::getUserScore(username.c_str());
                        }
                        else {
                            status.setString("Invalid credentials!");
                            status.setFillColor(Color::Red);
                        }
                    }
                    else if (signupBtn.getGlobalBounds().contains(mousePos) && !loggedIn) {
                        if (username.empty() || password.empty()) {
                            status.setString("Fields cannot be empty!");
                            status.setFillColor(Color::Red);
                        }
                        else if (LoginWindow::registerUser(username.c_str(), password.c_str())) {
                            status.setString("Signup successful! Please login");
                            status.setFillColor(Color::Green);
                            currentPlayer = username;
                            currentPlayerScore = 0;
                        }
                        else {
                            status.setString("Username exists!");
                            status.setFillColor(Color::Red);
                        }
                    }
                }
            }

            if (loggedIn) {
                loginWindow.close();
                return true;
            }

            loginWindow.clear();
            loginWindow.draw(bgSprite);
            loginWindow.draw(title);
            loginWindow.draw(nameText);
            loginWindow.draw(nameBox);
            loginWindow.draw(nameInput);
            loginWindow.draw(passText);
            loginWindow.draw(passBox);
            loginWindow.draw(passInput);
            loginWindow.draw(loginBtn);
            loginWindow.draw(loginText);
            loginWindow.draw(signupBtn);
            loginWindow.draw(signupText);
            loginWindow.draw(status);
            loginWindow.display();
        }
        return false;
    }

    void showGameRoom() {
        RenderWindow gameRoomWindow(VideoMode(960, 720), "Tournament Game Room");

        string topPlayers[5];
        int topScores[5];
        bool hasPlayers = matchmakingQueue.peekTop5(topPlayers, topScores);

        string opponents[3];
        int opponentCount = 0;


        if (hasPlayers) {
            for (int i = 0; i < 5 && opponentCount < 3; ++i) {
                if (topPlayers[i] != currentPlayer && !topPlayers[i].empty()) {
                    opponents[opponentCount++] = topPlayers[i];
                }
            }

        }
        // Fallback: Add dummy opponents if leaderboard is empty
        if (opponentCount == 0) {
            opponents[0] = "Bot1";
            opponents[1] = "Bot2";
            opponents[2] = "Bot3";
            opponentCount = 3;

            cout << "Warning: Leaderboard empty, using dummy opponents\n"; // Debug output
        }

        Clock timer;
        int currentOpponent = 0;
        bool showRequest = false;
        bool showWarning = false;
        bool opponentSelected = false;
        bool waitingForNext = false;

        Text delayText("Next request in: 3", font, 25);
        delayText.setPosition(400, 500);
        delayText.setFillColor(Color::White);

        while (gameRoomWindow.isOpen()) {
            float elapsed = timer.getElapsedTime().asSeconds();
            int remainingDelay = 3 - static_cast<int>(elapsed);

            Event event;
            while (gameRoomWindow.pollEvent(event)) {
                if (event.type == Event::Closed || (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
                    gameRoomWindow.close();
                    return;
                }

                if (showRequest && !opponentSelected && currentOpponent < opponentCount
                    ) {
                    if (event.type == Event::MouseButtonReleased) {
                        Vector2f mousePos = gameRoomWindow.mapPixelToCoords(Mouse::getPosition(gameRoomWindow));

                        RectangleShape acceptBtn(Vector2f(150, 50));
                        acceptBtn.setPosition(400, 440);
                        RectangleShape rejectBtn(Vector2f(150, 50));
                        rejectBtn.setPosition(600, 440);

                        if (acceptBtn.getGlobalBounds().contains(mousePos)) {
                            opponentSelected = true;
                            startMatch(opponents[currentOpponent]);
                            gameRoomWindow.close();
                            return;
                        }
                        else if (rejectBtn.getGlobalBounds().contains(mousePos)) {
                            showRequest = false;
                            waitingForNext = true;
                            timer.restart();
                            currentOpponent++;
                            if (currentOpponent >= opponentCount
                                ) { // Uses size() instead of hardcoded 3
                                showWarning = true;
                                waitingForNext = false;
                            }
                        }
                    }
                }

                if (showWarning && event.type == Event::MouseButtonReleased) {
                    Vector2f mousePos = gameRoomWindow.mapPixelToCoords(Mouse::getPosition(gameRoomWindow));
                    RectangleShape leaveBtn(Vector2f(150, 50));
                    leaveBtn.setPosition(300, 340);
                    RectangleShape stayBtn(Vector2f(150, 50));
                    stayBtn.setPosition(500, 340);

                    if (leaveBtn.getGlobalBounds().contains(mousePos)) {
                        gameRoomWindow.close();
                        return;
                    }
                    else if (stayBtn.getGlobalBounds().contains(mousePos)) {
                        showWarning = false;
                        opponentSelected = true;
                        // Plays remaining opponents in reverse order
                        for (int i = opponentCount
                            - 1; i >= 0; --i) {
                            startMatch(opponents[i]);
                        }
                        gameRoomWindow.close();
                        return;
                    }
                }
            }

            if (!showRequest && !opponentSelected && waitingForNext && elapsed >= 3.0f) {
                showRequest = true;
                waitingForNext = false;
            }

            if (!showRequest && !opponentSelected && !waitingForNext && !showWarning && currentOpponent < opponentCount
                ) {
                waitingForNext = true;
                timer.restart();
            }

            gameRoomWindow.clear();
            gameRoomWindow.draw(bgSprite);

            Text title("TOURNAMENT GAME ROOM", font, 50);
            title.setPosition(210, 50);
            title.setFillColor(Color::Yellow);
            gameRoomWindow.draw(title);

            Text playerText("Player: " + currentPlayer, font, 30);
            playerText.setPosition(50, 150);
            gameRoomWindow.draw(playerText);

            Text scoreText("Score: " + to_string(currentPlayerScore), font, 30);
            scoreText.setPosition(50, 200);
            gameRoomWindow.draw(scoreText);

            Text roundText("Round: " + to_string(round), font, 30);
            roundText.setPosition(50, 250);
            gameRoomWindow.draw(roundText);

            Text topText("TOP PLAYERS:", font, 30);
            topText.setPosition(600, 150);
            gameRoomWindow.draw(topText);

            // Displays top players or dummy players if empty
            if (hasPlayers) {
                for (int i = 0, dispIndex = 0; i < 5 && dispIndex < 5; ++i) {
                    if (topPlayers[i] != currentPlayer && !topPlayers[i].empty()) {
                        Text player(to_string(dispIndex + 1) + ". " + topPlayers[i], font, 25);
                        player.setPosition(600, 200 + dispIndex * 50);
                        gameRoomWindow.draw(player);

                        Text score(to_string(topScores[i]), font, 25);
                        score.setPosition(800, 200 + dispIndex * 50);
                        gameRoomWindow.draw(score);

                        dispIndex++;
                    }
                }
            }
            else {
                // Displays dummy players
                for (int i = 0; i < 3; ++i) {
                    Text player(to_string(i + 1) + ". Bot" + to_string(i + 1), font, 25);
                    player.setPosition(600, 200 + i * 50);
                    gameRoomWindow.draw(player);

                    Text score("0", font, 25);
                    score.setPosition(800, 200 + i * 50);
                    gameRoomWindow.draw(score);
                }
            }

            if (showRequest && currentOpponent < opponentCount
                ) {
                string opponent = opponents[currentOpponent];
                Text request("Accept match from " + opponent + "?", font, 25);
                request.setPosition(400, 400);
                request.setFillColor(Color::Green);
                gameRoomWindow.draw(request);

                RectangleShape acceptBtn(Vector2f(150, 50));
                acceptBtn.setPosition(400, 440);
                acceptBtn.setFillColor(Color::Blue);
                gameRoomWindow.draw(acceptBtn);

                Text acceptText("YES", font, 25);
                acceptText.setPosition(425, 450);
                gameRoomWindow.draw(acceptText);

                RectangleShape rejectBtn(Vector2f(150, 50));
                rejectBtn.setPosition(600, 440);
                rejectBtn.setFillColor(Color::Red);
                gameRoomWindow.draw(rejectBtn);

                Text rejectText("NO", font, 25);
                rejectText.setPosition(625, 450);
                gameRoomWindow.draw(rejectText);
            }

            if (waitingForNext && currentOpponent < opponentCount
                && !showWarning) {
                delayText.setString("Next request in: " + to_string(remainingDelay));
                gameRoomWindow.draw(delayText);
            }

            if (showWarning) {
                RectangleShape warningBg(Vector2f(500, 200));
                warningBg.setPosition(230, 260);
                warningBg.setFillColor(Color(50, 50, 50, 200));
                warningBg.setOutlineThickness(2);
                warningBg.setOutlineColor(Color::Red);
                gameRoomWindow.draw(warningBg);

                Text warning("Leave Tournament?", font, 30);
                warning.setPosition(280, 280);
                gameRoomWindow.draw(warning);

                RectangleShape leaveBtn(Vector2f(150, 50));
                leaveBtn.setPosition(300, 340);
                leaveBtn.setFillColor(Color::Red);
                gameRoomWindow.draw(leaveBtn);

                Text leaveText("YES", font, 30);
                leaveText.setPosition(345, 350);
                gameRoomWindow.draw(leaveText);

                RectangleShape stayBtn(Vector2f(150, 50));
                stayBtn.setPosition(500, 340);
                stayBtn.setFillColor(Color::Green);
                gameRoomWindow.draw(stayBtn);

                Text stayText("NO", font, 30);
                stayText.setPosition(545, 350);
                gameRoomWindow.draw(stayText);
            }

            gameRoomWindow.display();
        }
    }


    void startMatch(const string& opponent) {
        Multiplayer matchGame; 

        matchGame.getPlayer1().name = currentPlayer;
        matchGame.getPlayer1().score = currentPlayerScore;
        matchGame.getPlayer2().name = opponent;
        matchGame.getPlayer2().score = LoginWindow::getUserScore(opponent.c_str());

        matchGame.startMultiplayerGame();

        bool playerWon = (matchGame.getPlayer1().score > matchGame.getPlayer2().score);

        if (playedCount < 5) {
            playedOpponents[playedCount] = opponent;
            playedCount++;
        }

        if (playerWon) {
            winCount++;
            currentPlayerScore = matchGame.getPlayer1().score;
            if (winCount >= 3) {
                showTournamentResult(true);
                winCount = 0;
                playedCount = 0;
                for (int i = 0; i < 5; i++) {
                    playedOpponents[i] = "";
                }
            }
            else {
                showMatchResult(true);
                string nextOpponent = findNextOpponent();
                if (nextOpponent.empty()) {
                    showTournamentResult(true);
                }
                else {
                    startMatch(nextOpponent);
                }
            }
        }
        else {
            winCount = 0;
            playedCount = 0;
            for (int i = 0; i < 5; i++) {
                playedOpponents[i] = "";
            }
            showTournamentResult(false);
        }
    }

    string findNextOpponent() {
        string topPlayers[5];
        int topScores[5];
        if (!matchmakingQueue.peekTop5(topPlayers, topScores)) {
            return "";
        }

        for (int i = 0; i < 5; i++) {
            if (topPlayers[i].empty()) break;
            if (topPlayers[i] == currentPlayer) continue;
            // Checks if this opponent has been played
            bool alreadyPlayed = false;
            for (int j = 0; j < playedCount; j++) {
                if (playedOpponents[j] == topPlayers[i]) {
                    alreadyPlayed = true;
                    break;
                }
            }
            if (!alreadyPlayed) {
                return topPlayers[i];
            }
        }
        return ""; // No more opponents
    }

    void showMatchResult(bool won) {
        RenderWindow resultWindow(VideoMode(600, 400), "Match Result");

        Text resultText(won ? "YOU WON THIS MATCH!" : "YOU LOST THIS MATCH", font, 40);
        resultText.setPosition(115, 100);
        resultText.setFillColor(won ? Color::Green : Color::Red);

        Text nextText("Press ESC to continue", font, 30);
        nextText.setPosition(180, 200);

        while (resultWindow.isOpen()) {
            Event event;
            while (resultWindow.pollEvent(event)) {
                if (event.type == Event::Closed ||
                    (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)) {
                    resultWindow.close();
                }
            }

            resultWindow.clear();
            resultWindow.draw(bgSprite);
            resultWindow.draw(resultText);
            resultWindow.draw(nextText);
            resultWindow.display();
        }
    }

    void showTournamentResult(bool won) {
        RenderWindow resultWindow(VideoMode(600, 400), "Tournament Result");

        Text resultText(won ? "YOU WON THE TOURNAMENT!" : "TOURNAMENT OVER", font, 35);
        resultText.setPosition(70, 100);
        resultText.setFillColor(won ? Color::Yellow : Color::Green);

        RectangleShape continueBtn(Vector2f(200, 50));
        continueBtn.setPosition(100, 250);
        continueBtn.setFillColor(Color::Blue);

        Text continueText(won ? "CONTINUE?" : "MAIN MENU", font, 30);
        continueText.setPosition(120, 260);

        RectangleShape exitBtn(Vector2f(200, 50));
        exitBtn.setPosition(350, 250);
        exitBtn.setFillColor(Color::Red);

        Text exitText(won ? "EXIT" : "RESTART", font, 30);
        exitText.setPosition(380, 260);

        while (resultWindow.isOpen()) {
            Event event;
            while (resultWindow.pollEvent(event)) {
                if (event.type == Event::Closed) {
                    resultWindow.close();
                }

                if (event.type == Event::MouseButtonReleased) {
                    Vector2f mousePos = resultWindow.mapPixelToCoords(Mouse::getPosition(resultWindow));

                    if (continueBtn.getGlobalBounds().contains(mousePos)) {
                        resultWindow.close();
                        if (won) {
                            round = 1;
                            showGameRoom(); // Restarts procedure
                        }
                        return;
                    }

                    if (exitBtn.getGlobalBounds().contains(mousePos)) {
                        resultWindow.close();
                        if (!won) {
                            round = 1;
                            showGameRoom(); // Restarts for loss
                        }
                        return;
                    }
                }
            }

            resultWindow.clear();
            resultWindow.draw(bgSprite);
            resultWindow.draw(resultText);
            resultWindow.draw(continueBtn);
            resultWindow.draw(continueText);
            resultWindow.draw(exitBtn);
            resultWindow.draw(exitText);
            resultWindow.display();
        }
    }
};
bool Multiplayer::handleTournamentMode(RenderWindow& window) {
    Tournament tournament;
    if (tournament.showLogin(window)) {
        tournament.showGameRoom();
    }
    return false;
}