#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include<iostream>
using namespace sf;
using namespace std;

bool level_ended = false; // ended in pause menu
bool mainMenuSelected = false;
int level_counter = 0; // tells on which level we are.

const int M_SINGLE = 25;
const int N_SINGLE = 40;

int grid[M_SINGLE][N_SINGLE] = { 0 };
int ts_SINGLE = 18; // tile size

struct Enemy {
    int x, y, dx, dy;

    Enemy() {
        x = y = 300;
        dx = 4 - rand() % 8;
        dy = 4 - rand() % 8;
    }

    // Used for single-player
    void move() {
        x += dx;
        if (grid[y / ts_SINGLE][x / ts_SINGLE] == 1) { dx = -dx; x += dx; }
        y += dy;
        if (grid[y / ts_SINGLE][x / ts_SINGLE] == 1) { dy = -dy; y += dy; }
    }

    // Used for multiplayer
    void move(int** localGrid, int ts, int maxM, int maxN) {
        x += dx;
        if (x / ts >= maxN || y / ts >= maxM) return; // safety check
        if (localGrid[y / ts][x / ts] == 1) { dx = -dx; x += dx; }

        y += dy;
        if (x / ts >= maxN || y / ts >= maxM) return;
        if (localGrid[y / ts][x / ts] == 1) { dy = -dy; y += dy; }
    }
};


// Powerups variables and functions 
const int MAX_POWERUPS = 10;
int powerUpStack[MAX_POWERUPS];
int top = -1;

bool isPowerUpActive = false;
Clock powerUpClock;
float powerUpDuration = 6.0f;

void pushPowerUp(int value) {
    if (top < MAX_POWERUPS - 1) {
        top++;
        powerUpStack[top] = value;
    }
}

void popPowerUp() {
    if (top >= 0) {
        top--;
    }
}

bool hasPowerUp() {
    return top >= 0;
}


void drop(int y, int x)
{
    if (grid[y][x] == 0) grid[y][x] = -1;
    if (grid[y - 1][x] == 0) drop(y - 1, x);
    if (grid[y + 1][x] == 0) drop(y + 1, x);
    if (grid[y][x - 1] == 0) drop(y, x - 1);
    if (grid[y][x + 1] == 0) drop(y, x + 1);
}


//                 --------------------------   IMPLEMENTATION OF THEMES via AVL TREES -----------------

struct Node {
    string themeName;
    Color themeColor;
    Node* left;
    Node* right;
    int height;

    Node(string name, Color color) : themeName(name), themeColor(color), left(nullptr), right(nullptr), height(1) {}
};

class AVLTree {
private:
    Node* root;

    int height(Node* node) {
        return node ? node->height : 0;
    }

    int balanceFactor(Node* node) {
        return node ? height(node->left) - height(node->right) : 0;
    }

    void updateHeight(Node* node) {
        node->height = max(height(node->left), height(node->right)) + 1;
    }

    Node* rotateRight(Node* y) {
        Node* x = y->left;
        Node* T2 = x->right;
        x->right = y;
        y->left = T2;
        updateHeight(y);
        updateHeight(x);
        return x;
    }

    Node* rotateLeft(Node* x) {
        Node* y = x->right;
        Node* T2 = y->left;
        y->left = x;
        x->right = T2;
        updateHeight(x);
        updateHeight(y);
        return y;
    }

    Node* balance(Node* node) {
        updateHeight(node);
        int balance = balanceFactor(node);
        if (balance > 1) {
            if (balanceFactor(node->left) < 0) {
                node->left = rotateLeft(node->left);
            }
            return rotateRight(node);
        }
        if (balance < -1) {
            if (balanceFactor(node->right) > 0) {
                node->right = rotateRight(node->right);
            }
            return rotateLeft(node);
        }
        return node;
    }

    Node* insert(Node* node, string themeName, Color themeColor) {
        if (!node) return new Node(themeName, themeColor);

        if (themeName < node->themeName) {
            node->left = insert(node->left, themeName, themeColor);
        }
        else if (themeName > node->themeName) {
            node->right = insert(node->right, themeName, themeColor);
        }
        return balance(node);
    }

    Node* find(Node* node, const string& themeName) {
        if (!node || node->themeName == themeName) {
            return node;
        }
        if (themeName < node->themeName) {
            return find(node->left, themeName);
        }
        return find(node->right, themeName);
    }

public:
    AVLTree() : root(nullptr) {}

    void insert(string themeName, Color themeColor) {
        root = insert(root, themeName, themeColor);
    }

    Color getThemeColor(const string& themeName) {
        Node* themeNode = find(root, themeName);
        if (themeNode) {
            return themeNode->themeColor;
        }
        return Color::Black; // Default theme
    }
};

AVLTree themeTree;

void initializeThemes() {
    themeTree.insert("Dark Gray", Color(169, 169, 169));  // Dark Gray
    themeTree.insert("Dark Navy", Color(0, 0, 128));      // Dark Navy
    themeTree.insert("Space Purple", Color(48, 25, 52));  // Space Purple
    themeTree.insert("Muted Brown", Color(139, 69, 19));  // Muted Brown
}

void changeTheme(RenderWindow& window, const string& themeName) {
    Color themeColor = themeTree.getThemeColor(themeName);
    window.clear(themeColor);
}






//                             -------------------------   PASUE GAME MENU --------------------------

int pauseMenu(RenderWindow& window, Font& font, const Color& backgroundColor)
{

    // Load sounds
    SoundBuffer beepBuffer, selectBuffer, pauseBuffer;
    beepBuffer.loadFromFile("audios/navigation.mp3");
    selectBuffer.loadFromFile("audios/selection.mp3");
    pauseBuffer.loadFromFile("audios/selection.mp3");

    Sound beepSound(beepBuffer);
    Sound selectSound(selectBuffer);
    Sound pauseSound(pauseBuffer);
    // Playing pause sound immediately
    pauseSound.play();

    //Loading Textures
    Texture pauseTexture;
    pauseTexture.loadFromFile("images/pause_bg.jpg");

    Sprite pauseBackground(pauseTexture);
    pauseBackground.setScale(
        (float)window.getSize().x / pauseTexture.getSize().x,
        (float)window.getSize().y / pauseTexture.getSize().y
    );

    Text title("GAME PAUSED", font, 75);
    title.setFillColor(Color::White);
    title.setPosition(120, 100);

    Text resumeBtn("RESUME", font, 50);
    Text exitBtn("EXIT", font, 50);
    resumeBtn.setFillColor(Color::White);
    exitBtn.setFillColor(Color::White);
    resumeBtn.setPosition(180, 280);
    exitBtn.setPosition(450, 280);

    int selected = 0; // 0 = Resume, 1 = Exit

    while (true)
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed)
                return 1; // Exit

            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Left || e.key.code == Keyboard::Right) {

                    selected = 1 - selected;
                    beepSound.play();
                }

                if (e.key.code == Keyboard::Enter) {
                    selectSound.play();
                    sleep(milliseconds(200)); // Giving time to play the sound
                    return selected;
                }
            }
        }

        resumeBtn.setFillColor(selected == 0 ? Color::Green : Color::White);
        exitBtn.setFillColor(selected == 1 ? Color::Green : Color::White);

        window.clear(backgroundColor);
        window.draw(pauseBackground);
        window.draw(title);
        window.draw(resumeBtn);
        window.draw(exitBtn);
        window.display();
    }
}

SoundBuffer winBuffer;
Sound winSound;


//                                --------------------------- WIN  MENU --------------------------

int winMenu(RenderWindow& window, Font& font, const Color& backgroundColor)
{
    Texture winBgTexture;
    winBgTexture.loadFromFile("images/win_screen.jpeg");
    Sprite winBg(winBgTexture);
    winBg.setScale(
        (float)window.getSize().x / winBgTexture.getSize().x,
        (float)window.getSize().y / winBgTexture.getSize().y
    );

    Text title("LEVEL COMPLETE !", font, 42);
    title.setFillColor(Color::Yellow);
    title.setPosition(210, 232);

    // Changing button options based on level
    Text exitBtn("EXIT", font, 40);
    Text mainMenuBtn("MAIN MENU", font, 40);
    Text nextLevelBtn(level_counter < 2 ? "NEXT LEVEL" : "RESTART", font, 40);

    exitBtn.setPosition(45, 280);
    mainMenuBtn.setPosition(45, 350);
    nextLevelBtn.setPosition(45, 420);

    int selected = 0;
    SoundBuffer selectBuf, moveBuf;
    selectBuf.loadFromFile("audios/selection.mp3");
    moveBuf.loadFromFile("audios/navigation.mp3");
    Sound selectSound(selectBuf), moveSound(moveBuf);

    while (true)
    {
        Event e;
        while (window.pollEvent(e))
        {
            if (e.type == Event::Closed) return 0;
            if (e.type == Event::KeyPressed)
            {
                if (e.key.code == Keyboard::Up) {
                    selected = (selected + 2) % 3;
                    moveSound.play();
                }
                if (e.key.code == Keyboard::Down) {
                    selected = (selected + 1) % 3;
                    moveSound.play();
                }
                if (e.key.code == Keyboard::Enter) {
                    selectSound.play();
                    sleep(milliseconds(200));

                    // Return values:
                    // 0 = EXIT
                    // 1 = MAIN MENU
                    // 2 = NEXT LEVEL/RESTART
                    return selected;
                }
            }
        }

        exitBtn.setFillColor(selected == 0 ? Color::Green : Color::White);
        mainMenuBtn.setFillColor(selected == 1 ? Color::Green : Color::White);
        nextLevelBtn.setFillColor(selected == 2 ? Color::Green : Color::White);

        window.clear(backgroundColor);
        window.draw(winBg);
        window.draw(title);
        window.draw(exitBtn);
        window.draw(mainMenuBtn);
        window.draw(nextLevelBtn);
        window.display();
    }
}



//                         ------------------------- GAME STARTS ------------------------


int startGame(int livs, int enms, const Color& backgroundColor) {
    srand(time(0));
    RenderWindow window(VideoMode(N_SINGLE * ts_SINGLE, M_SINGLE * ts_SINGLE + 50), "Xonix Game!");
    window.setFramerateLimit(60);

    Texture t1, t2, t3;
    t1.loadFromFile("images/tiles.png");
    t2.loadFromFile("images/gameover.png");
    t3.loadFromFile("images/enemy.png");

    Sprite sTile(t1), sGameover(t2), sEnemy(t3);
    sGameover.setPosition(100, 100);
    sEnemy.setOrigin(20, 20);

    Font font;
    font.loadFromFile("fonts/AlexandriaFLF.ttf");

    Text scoreText, lifeText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(20);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(10, M_SINGLE * ts_SINGLE + 10);

    lifeText.setFont(font);
    lifeText.setCharacterSize(20);
    lifeText.setFillColor(Color::Red);
    lifeText.setPosition(200, M_SINGLE * ts_SINGLE + 10);

    Texture tWin;
    tWin.loadFromFile("images/you win.png");
    Sprite sWin(tWin);
    sWin.setPosition(250, 150);

    if (!winBuffer.loadFromFile("audios/winSound.mp3")) {
        std::cout << "Failed to load victory sound!" << std::endl;
    }

    winSound.setBuffer(winBuffer);


    int enemyCount = enms;
    Enemy a[10];
    bool Game = true;
    bool YouWin = false;
    int x = 0, y = 0, dx = 0, dy = 0;
    float timer = 0, delay = 0.07;
    Clock clock;

    int score = 0;
    int lives = livs;

    for (int i = 0; i < M_SINGLE; i++)
        for (int j = 0; j < N_SINGLE; j++)
            grid[i][j] = (i == 0 || j == 0 || i == M_SINGLE - 1 || j == N_SINGLE - 1) ? 1 : 0;

    bool hasMoved = false;

    bool powerUpGrantedAt50 = false;
    bool powerUpGrantedAt130 = false;


    //BEEPS sounds when a life is lost or Game is over

    SoundBuffer bufferLifeLost, bufferGameOver;
    if (!bufferLifeLost.loadFromFile("audios/life_lost.mp3"))
        return 0; // or handle error
    if (!bufferGameOver.loadFromFile("audios/game_over.mp3"))
        return 0;

    Sound soundLifeLost, soundGameOver;
    soundLifeLost.setBuffer(bufferLifeLost);
    soundGameOver.setBuffer(bufferGameOver);




    //              --------------------------- GAME WINDOW IS OPEN -------------------------

    while (window.isOpen()) {
        float time = clock.getElapsedTime().asSeconds();
        clock.restart();
        timer += time;

        // Disables power-up after 6 seconds
        if (isPowerUpActive && powerUpClock.getElapsedTime().asSeconds() >= 6.f) {
            isPowerUpActive = false;
        }

        Event e;
        while (window.pollEvent(e)) {
            if (e.type == Event::Closed) {
                window.close();
                return -2;
            }
            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Space) {
                if (hasPowerUp() && !isPowerUpActive) {
                    isPowerUpActive = true;
                    powerUpClock.restart();
                    popPowerUp();  // Consumes the power-up
                }
            }

            if (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape) {
                if (Game) {
                    int choice = pauseMenu(window, font, backgroundColor);
                    if (choice == 1) {
                        level_ended = true;
                        return score;
                    }
                }
                else {
                    // Restarting logic on game over or win
                    for (int i = 1; i < M_SINGLE - 1; i++)
                        for (int j = 1; j < N_SINGLE - 1; j++)
                            grid[i][j] = 0;
                    x = 10; y = 0;
                    Game = true;
                    YouWin = false;
                    lives = 3;
                    score = 0;
                    dx = dy = 0;
                }

            }


        }

        bool moved = false;
        if (Keyboard::isKeyPressed(Keyboard::Left)) { dx = -1; dy = 0; moved = true; }
        if (Keyboard::isKeyPressed(Keyboard::Right)) { dx = 1; dy = 0; moved = true; }
        if (Keyboard::isKeyPressed(Keyboard::Up)) { dx = 0; dy = -1; moved = true; }
        if (Keyboard::isKeyPressed(Keyboard::Down)) { dx = 0; dy = 1; moved = true; }

        if (!Game) continue;

        if (timer > delay) {
            if (moved) {
                x += dx;
                y += dy;

                if (x < 0) x = 0; if (x > N_SINGLE - 1) x = N_SINGLE - 1;
                if (y < 0) y = 0; if (y > M_SINGLE - 1) y = M_SINGLE - 1;

                if (grid[y][x] == 2) {
                    soundLifeLost.play(); // 🔊 playing life lost sound
                    lives--;
                    if (lives <= 0) {
                        Game = false;
                        soundGameOver.play(); // 🔊 playing game over sound
                    }
                    else {
                        for (int m = 1; m < M_SINGLE - 1; m++)
                            for (int n = 1; n < N_SINGLE - 1; n++)
                                if (grid[m][n] == 2) grid[m][n] = 0;
                        x = 1; y = 1;
                        dx = dy = 0;
                    }
                }
                else if (grid[y][x] == 1) {
                    dx = dy = 0;

                    for (int i = 0; i < enemyCount; i++)
                        drop(a[i].y / ts_SINGLE, a[i].x / ts_SINGLE);

                    int area = 0;
                    for (int i = 0; i < M_SINGLE; i++)
                        for (int j = 0; j < N_SINGLE; j++) {
                            if (grid[i][j] == -1)
                                grid[i][j] = 0;
                            else if (grid[i][j] == 2) {
                                grid[i][j] = 1;
                                area++;
                            }
                        }

                    // Updateing scoring rule:
                    if (area > 0) {
                        int points = area;
                        if (area > 10) points *= 2; // Doubles points if more than 10 tiles captured
                        score += points;

                        if (score >= 50 && !powerUpGrantedAt50) {
                            pushPowerUp(50);
                            powerUpGrantedAt50 = true;
                        }

                        if (score >= 130 && !powerUpGrantedAt130) {
                            if (top >= 0) popPowerUp(); // Removing older one if unused
                            pushPowerUp(130);
                            powerUpGrantedAt130 = true;
                        }


                        if (score > 100 && !YouWin) {
                            Game = false;
                            YouWin = true;
                            winSound.play();

                            int result = winMenu(window, font, backgroundColor);
                            if (result == 0) { // EXIT
                                level_ended = true;
                                window.close();
                                return score;
                            }
                            else if (result == 1) { // MAIN MENU
                                level_ended = true;
                                window.close();
                                return -1; // Special value to indicate main menu request
                            }
                            else if (result == 2) { // NEXT LEVEL/RESTART
                                level_counter++;
                                if (level_counter > 2) { // If all levels completed
                                    level_counter = 0; // Reset to first level
                                }
                                window.close();
                                return score;
                            }
                        }

                    }

                }
                else if (grid[y][x] == 0) {
                    grid[y][x] = 2;
                }
            }

            timer = 0;
        }

        for (int i = 0; i < enemyCount; i++) {
            if (!isPowerUpActive)
                a[i].move();
        }

        //for (int i = 0; i < enemyCount; i++) a[i].move();

        for (int i = 0; i < enemyCount; i++) {
            int ex = a[i].x / ts_SINGLE;
            int ey = a[i].y / ts_SINGLE;
            if (grid[ey][ex] == 2) {
                soundLifeLost.play(); // BEEP sound 
                lives--;
                if (lives <= 0) {
                    Game = false;
                    soundGameOver.play();
                }
                else {
                    for (int m = 1; m < M_SINGLE - 1; m++)
                        for (int n = 1; n < N_SINGLE - 1; n++)
                            if (grid[m][n] == 2) grid[m][n] = 0;
                    x = 1; y = 1;
                    dx = dy = 0;
                }
                break;
            }
        }

        // draw section
        window.clear(backgroundColor);

        // Drawing tiles
        for (int i = 0; i < M_SINGLE; i++)
            for (int j = 0; j < N_SINGLE; j++) {
                if (grid[i][j] == 0) continue;
                if (grid[i][j] == 1) sTile.setTextureRect(IntRect(0, 0, ts_SINGLE, ts_SINGLE));
                if (grid[i][j] == 2) sTile.setTextureRect(IntRect(54, 0, ts_SINGLE, ts_SINGLE));
                sTile.setPosition(j * ts_SINGLE, i * ts_SINGLE + 40);  // shift down by 40px for UI space
                window.draw(sTile);
            }

        // Drawing enemies
        sEnemy.rotate(10);
        for (int i = 0; i < enemyCount; i++) {
            sEnemy.setPosition(a[i].x, a[i].y + 40);  // shift enemy down too
            window.draw(sEnemy);
        }

        // Drawing player
        sTile.setTextureRect(IntRect(36, 0, ts_SINGLE, ts_SINGLE));
        sTile.setPosition(x * ts_SINGLE, y * ts_SINGLE + 40);  // shift player down too
        window.draw(sTile);

        // Drawing UI (score + lives) at the top
        scoreText.setPosition(10, 5);
        scoreText.setString("Score: " + std::to_string(score));
        window.draw(scoreText);

        for (int i = 0; i < lives; i++) {
            CircleShape heart(10);
            heart.setFillColor(Color::Red);
            heart.setPosition(200 + i * 30, 10);
            window.draw(heart);
        }

        if (!Game) {
            if (YouWin) {
                std::cout << "Player won, showing image.\n";
                window.clear(backgroundColor);
                window.draw(sWin);
                window.display();
                sleep(seconds(2));
            }
            else
                window.draw(sGameover);
        }


        window.display();
    }
    return score;
}