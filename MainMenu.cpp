#include "Source.h"
#include "LoginWindow.h"
#include "Multiplayer.h"
#include "profile.h"
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <fstream>

using namespace sf;
using namespace std;

#define Max_main_menu 4
#define Max_option_menu 5
const int MAX_LOGIN_HISTORY = 100;
const int MAX_STRING_SIZE = 50;
#define ENTRIES_PER_PAGE 10
#define MAX_ENTRIES 30

// Main menu class
class MainMenu {
public:
    MainMenu(float width, float height);
    void draw(RenderWindow& window);
    void MoveUp();
    void MoveDown();
    int MainMenuPressed() { return MainMenuSelected; }
    ~MainMenu() {}
private:
    int MainMenuSelected;
    Font font;
    Text mainMenu[Max_main_menu];
};


// Option Sub-Class within the main menu
class OptionMenu {
public:
    OptionMenu(float width, float height);
    void draw(RenderWindow& window);
    void MoveUp();
    void MoveDown();
    int OptionMenuPressed() { return OptionMenuSelected; }
private:
    int OptionMenuSelected;
    Font font;
    Text optionMenu[Max_option_menu];
};

// MainMenu constructor
MainMenu::MainMenu(float width, float height) {
    if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
        cout << "No font is here..!!\n";
    }
    mainMenu[0].setFont(font);
    mainMenu[0].setFillColor(Color::Yellow);
    mainMenu[0].setString("Play");
    mainMenu[0].setCharacterSize(70);
    mainMenu[0].setPosition(400, 200);
    mainMenu[1].setFont(font);
    mainMenu[1].setFillColor(Color::White);
    mainMenu[1].setString("Options");
    mainMenu[1].setCharacterSize(70);
    mainMenu[1].setPosition(400, 300);
    mainMenu[2].setFont(font);
    mainMenu[2].setFillColor(Color::White);
    mainMenu[2].setString("Instructions");
    mainMenu[2].setCharacterSize(70);
    mainMenu[2].setPosition(400, 400);
    mainMenu[3].setFont(font);
    mainMenu[3].setFillColor(Color::White);
    mainMenu[3].setString("Exit");
    mainMenu[3].setCharacterSize(70);
    mainMenu[3].setPosition(400, 500);
    MainMenuSelected = 0;
}

// Option menu constructor
OptionMenu::OptionMenu(float width, float height) {
    if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
        cout << "Font loading failed!\n";
    }
    string items[] = { "Leaderboard", "Login History", "Theme", "Multiplayer", "Back" };
    for (int i = 0; i < Max_option_menu; ++i) {
        optionMenu[i].setFont(font);
        optionMenu[i].setFillColor(i == 0 ? Color::Yellow : Color::White);
        optionMenu[i].setString(items[i]);
        optionMenu[i].setCharacterSize(70);
        optionMenu[i].setPosition(300, 250 + i * 80);
    }
    OptionMenuSelected = 0;
}

// Draw MainMenu and OptionMenu
void MainMenu::draw(RenderWindow& window) {
    for (int i = 0; i < Max_main_menu; ++i) {
        window.draw(mainMenu[i]);
    }
}

void OptionMenu::draw(RenderWindow& window) {
    for (int i = 0; i < Max_option_menu; ++i)
        window.draw(optionMenu[i]);
}

// Move Up
void MainMenu::MoveUp() {
    if (MainMenuSelected - 1 >= 0) {
        mainMenu[MainMenuSelected].setFillColor(Color::White);
        MainMenuSelected--;
        mainMenu[MainMenuSelected].setFillColor(Color::Yellow);
    }
}

// Move Down
void MainMenu::MoveDown() {
    if (MainMenuSelected + 1 < Max_main_menu) {
        mainMenu[MainMenuSelected].setFillColor(Color::White);
        MainMenuSelected++;
        mainMenu[MainMenuSelected].setFillColor(Color::Yellow);
    }
}

void OptionMenu::MoveUp() {
    if (OptionMenuSelected > 0) {
        optionMenu[OptionMenuSelected].setFillColor(Color::White);
        OptionMenuSelected--;
        optionMenu[OptionMenuSelected].setFillColor(Color::Yellow);
    }
}

void OptionMenu::MoveDown() {
    if (OptionMenuSelected < Max_option_menu - 1) {
        optionMenu[OptionMenuSelected].setFillColor(Color::White);
        OptionMenuSelected++;
        optionMenu[OptionMenuSelected].setFillColor(Color::Yellow);
    }
}

// Function to read login history from a file
void readLoginHistory(char usernames[MAX_LOGIN_HISTORY][MAX_STRING_SIZE], char passwords[MAX_LOGIN_HISTORY][MAX_STRING_SIZE], int& historyCount) {
    ifstream file("users.txt");
    if (!file.is_open()) {
        std::cout << "Failed to open the users file.\n";
        return;
    }
    char line[MAX_STRING_SIZE];
    historyCount = 0;
    while (file.getline(line, MAX_STRING_SIZE) && historyCount < MAX_LOGIN_HISTORY) {
        int i = 0;
        int delimiterPos = -1;
        while (line[i] != '\0') {
            if (line[i] == ':') {
                delimiterPos = i;
                break;
            }
            i++;
        }
        if (delimiterPos != -1) {
            int j = 0;
            for (int k = 0; k < delimiterPos; k++) {
                usernames[historyCount][j++] = line[k];
            }
            usernames[historyCount][j] = '\0';
            j = 0;
            for (int k = delimiterPos + 1; line[k] != '\0'; k++) {
                passwords[historyCount][j++] = line[k];
            }
            passwords[historyCount][j] = '\0';
            historyCount++;
        }
    }
    file.close();
}


// Theme and save theme section
const string THEME_FILE = "theme.txt";
Sprite bgSprite;

int loadThemeFromFile() {
    ifstream inFile(THEME_FILE);
    if (!inFile.is_open()) {
        cout << "Theme file not found. Using default (0).\n";
        return 0;
    }
    int themeIndex;
    if (!(inFile >> themeIndex)) {
        cout << "Corrupt theme file. Using default (0).\n";
        return 0;
    }
    if (themeIndex < 0 || themeIndex > 3) {
        cout << "Invalid theme index (" << themeIndex << "). Using default (0).\n";
        return 0;
    }
    return themeIndex;
}

void saveThemeToFile(int themeIndex) {
    ofstream outFile(THEME_FILE);
    if (outFile.is_open()) {
        outFile << themeIndex;
        outFile.close();
    }
}

void changeTheme(int index, RectangleShape& background, bool isMainMenu = false, bool forceReload = false) {
    static Texture mainMenuTexture;
    if (isMainMenu || forceReload) {
        if (!mainMenuTexture.loadFromFile("images/Background.jpeg")) {
            cout << "Failed to load background texture.\n";
        }
        background.setTexture(&mainMenuTexture, true);
        background.setFillColor(Color::White);
        return;
    }
    Color newColor;
    switch (index) {
    case 0: newColor = Color(30, 30, 30); break;
    case 1: newColor = Color(15, 20, 40); break;
    case 2: newColor = Color(40, 10, 50); break;
    case 3: newColor = Color(60, 40, 20); break;
    default: newColor = Color::Black;
    }
    background.setFillColor(newColor);
    background.setTexture(nullptr);
}

void showConfirmation(RenderWindow& window, Text& confirmationText, const string& message, const Font& font, RectangleShape& background, MainMenu& menu) {
    confirmationText.setFont(font);
    confirmationText.setString(message);
    confirmationText.setFillColor(Color::Green);
    confirmationText.setPosition(250, 600);
    window.clear();
    window.draw(background);
    menu.draw(window);
    window.draw(confirmationText);
    window.display();
    sleep(seconds(2));
}

// Main Function
int main() {
    RenderWindow MENU(VideoMode(960, 720), "Main Menu", Style::Default);
    MainMenu mainMenu(MENU.getSize().x, MENU.getSize().y);
    RectangleShape background;
    background.setSize(Vector2f(960, 720));
    Texture Maintexture;
    if (!Maintexture.loadFromFile("images/Background.jpeg")) {
        cout << "Failed to load background texture.\n";
    }
    background.setTexture(&Maintexture);
    RectangleShape Obackground;
    Obackground.setSize(Vector2f(960, 720));
    Texture Optiontexture;
    if (!Optiontexture.loadFromFile("images/Options_photo.png")) {
        cout << "Failed to load options photo.\n";
    }
    Obackground.setTexture(&Optiontexture);
    RectangleShape ABbackground;
    ABbackground.setSize(Vector2f(960, 720));
    Texture Abouttexture;
    if (!Abouttexture.loadFromFile("images/instructions.jpeg")) {
        cout << "Failed to load about photo.\n";
    }
    ABbackground.setTexture(&Abouttexture);
    Music backgroundMusic;
    if (!backgroundMusic.openFromFile("audios/background.mp3")) {
        cout << "Failed to load background music.\n";
    }
    SoundBuffer navigationBuffer;
    SoundBuffer selectionBuffer;
    Sound navigationSound;
    Sound selectionSound;
    SoundBuffer mediumLevelBuffer;
    SoundBuffer hardLevelBuffer;
    Sound mediumLevelSound;
    Sound hardLevelSound;
    SoundBuffer level1Buffer;
    if (!level1Buffer.loadFromFile("audios/LEVEL 1.mp3")) {
        cout << "Failed to load LEVEL 1 sound.\n";
    }
    Sound level1Sound;
    level1Sound.setBuffer(level1Buffer);
    if (!mediumLevelBuffer.loadFromFile("audios/LEVEL 2.mp3")) {
        cout << "Failed to load LEVEL MEDIUM sound.\n";
    }
    mediumLevelSound.setBuffer(mediumLevelBuffer);
    if (!hardLevelBuffer.loadFromFile("audios/LEVEL 3.mp3")) {
        cout << "Failed to load LEVEL HARD sound.\n";
    }
    hardLevelSound.setBuffer(hardLevelBuffer);
    if (!navigationBuffer.loadFromFile("audios/navigation.mp3")) {
        cout << "Failed to load navigation sound.\n";
    }
    navigationSound.setBuffer(navigationBuffer);
    if (!selectionBuffer.loadFromFile("audios/selection.mp3")) {
        cout << "Failed to load selection sound.\n";
    }
    selectionSound.setBuffer(selectionBuffer);
    backgroundMusic.setLoop(true);
    backgroundMusic.play();
    Text confirmationText;
    Font confirmationFont;
    if (!confirmationFont.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
        cout << "Failed to load confirmation font.\n";
    }
    confirmationText.setFont(confirmationFont);
    confirmationText.setString("");
    confirmationText.setCharacterSize(40);
    confirmationText.setFillColor(Color::Green);
    confirmationText.setPosition(250, 600);

    while (MENU.isOpen()) {
        Event event;
        while (MENU.pollEvent(event)) {
            if (event.type == Event::Closed) {
                MENU.close();
            }
            if (event.type == Event::KeyReleased) {
                if (event.key.code == Keyboard::Up) {
                    navigationSound.play();
                    mainMenu.MoveUp();
                    break;
                }
                if (event.key.code == Keyboard::Down) {
                    navigationSound.play();
                    mainMenu.MoveDown();
                    break;
                }
                if (event.key.code == Keyboard::Return) {
                    selectionSound.play();
                    backgroundMusic.stop();
                    int x = mainMenu.MainMenuPressed();
                    if (x == 0) { // PLAY
                        LoginWindow login;
                        if (login.show(MENU)) {
                            string currentUser = login.getUsername();
                            ProfileMenu profileMenu(currentUser);
                            RenderWindow PROFILE_WINDOW(VideoMode(960, 720), "Player Profile");
                            bool startGame1 = profileMenu.show(PROFILE_WINDOW);
                            if (startGame1) {
                                PROFILE_WINDOW.close();
                                MENU.close();
                                int themeIndex = loadThemeFromFile();
                                Color gameBgColor;
                                switch (themeIndex) {
                                case 0: gameBgColor = Color(30, 30, 30); break;
                                case 1: gameBgColor = Color(15, 20, 40); break;
                                case 2: gameBgColor = Color(40, 10, 50); break;
                                case 3: gameBgColor = Color(60, 40, 20); break;
                                default: gameBgColor = Color::Black;
                                }
                                while (true) {
                                    level1Sound.play();
                                    int score1 = startGame(3, 4, gameBgColor);
                                    if (score1 == -1 || score1 == -2) break;
                                    if (score1 != -2) {
                                        LoginWindow::updateUserScore(currentUser, score1);
                                    }
                                    if (level_ended || score1 == -2) return 0;
                                    mediumLevelSound.play();
                                    int score2 = startGame(2, 6, gameBgColor);
                                    if (score2 == -1 || score2 == -2) break;
                                    if (score2 != -2) {
                                        LoginWindow::updateUserScore(currentUser, score2);
                                    }
                                    if (level_ended || score2 == -2) return 0;
                                    hardLevelSound.play();
                                    int score3 = startGame(1, 7, gameBgColor);
                                    if (score3 == -1 || score3 == -2) break;
                                    if (score3 != -2) {
                                        LoginWindow::updateUserScore(currentUser, score3);
                                    }
                                    if (level_ended || score3 == -2) return 0;
                                    level_counter = 0;
                                }
                                main();
                                return 0;
                            }
                            else {
                                PROFILE_WINDOW.close();
                            }
                        }
                    }
                    else if (x == 1) { // Options
                        RenderWindow OPTIONS(VideoMode(960, 720), "Options");
                        OptionMenu optMenu(OPTIONS.getSize().x, OPTIONS.getSize().y);
                        while (OPTIONS.isOpen()) {
                            Event e;
                            while (OPTIONS.pollEvent(e)) {
                                if (e.type == Event::Closed || (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape))
                                    OPTIONS.close();
                                if (e.type == Event::KeyReleased) {
                                    if (e.key.code == Keyboard::Up) {
                                        navigationSound.play();
                                        optMenu.MoveUp();
                                    }
                                    else if (e.key.code == Keyboard::Down) {
                                        navigationSound.play();
                                        optMenu.MoveDown();
                                    }
                                    else if (e.key.code == Keyboard::Return) {
                                        selectionSound.play();
                                        int choice = optMenu.OptionMenuPressed();
                                        if (choice == 0) {
                                            RenderWindow leaderboard(VideoMode(960, 720), "Leaderboard");
                                            LoginWindow::leaderboardHeap.loadFromFile("users.txt");
                                            Texture bgTexture;
                                            bgTexture.loadFromFile("images/pause_bg.jpg");
                                            Sprite bgSprite(bgTexture);
                                            bgSprite.setPosition(150, 50);
                                            Font font;
                                            if (!font.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
                                                cerr << "Failed to load leaderboard font!\n";
                                                leaderboard.close();
                                                continue;
                                            }
                                            PlayerScore sorted[10];
                                            int count;
                                            LoginWindow::leaderboardHeap.getSorted(sorted, count);
                                            Text title("LEADERBOARD", font, 80);
                                            title.setPosition(250, 60);
                                            title.setFillColor(Color::Red);
                                            Text entries[10];
                                            float yPos = 180;
                                            for (int i = 0; i < count; i++) {
                                                entries[i].setString(
                                                    to_string(i + 1) + ". " +
                                                    sorted[i].username + " - " +
                                                    to_string(sorted[i].score)
                                                );
                                                entries[i].setFont(font);
                                                entries[i].setCharacterSize(40);
                                                entries[i].setPosition(300, yPos);
                                                entries[i].setFillColor(Color::Yellow);
                                                yPos += 60;
                                            }
                                            while (leaderboard.isOpen()) {
                                                Event e;
                                                while (leaderboard.pollEvent(e)) {
                                                    if (e.type == Event::Closed ||
                                                        (e.type == Event::KeyPressed && e.key.code == Keyboard::Escape)) {
                                                        leaderboard.close();
                                                    }
                                                }
                                                leaderboard.clear();
                                                leaderboard.draw(bgSprite);
                                                leaderboard.draw(title);
                                                for (int i = 0; i < count; i++) {
                                                    leaderboard.draw(entries[i]);
                                                }
                                                leaderboard.display();
                                            }
                                        }
                                        else if (choice == 1) { // Login History
                                            RenderWindow loginHistory(VideoMode(960, 720), "Login History");
                                            SoundBuffer selectionBuffer;
                                            Sound buttonSound;
                                            if (!selectionBuffer.loadFromFile("audios/selection.mp3")) {
                                                cout << "Failed to load button sound.\n";
                                            }
                                            buttonSound.setBuffer(selectionBuffer);
                                            char usernames[MAX_LOGIN_HISTORY][MAX_STRING_SIZE];
                                            char passwords[MAX_LOGIN_HISTORY][MAX_STRING_SIZE];
                                            int historyCount = 0;
                                            readLoginHistory(usernames, passwords, historyCount);
                                            int totalEntries = min(historyCount, MAX_ENTRIES);
                                            int currentPage = 0;
                                            int totalPages = (totalEntries + ENTRIES_PER_PAGE - 1) / ENTRIES_PER_PAGE;
                                            Texture bgTexture;
                                            if (!bgTexture.loadFromFile("images/pause_bg.jpg")) {
                                                std::cout << "Failed to load background image.\n";
                                            }
                                            Sprite bgSprite(bgTexture);
                                            bgSprite.setPosition(150, 50);
                                            RectangleShape bgRect(Vector2f(750, 480));
                                            bgRect.setFillColor(Color(0, 0, 0, 150));
                                            bgRect.setPosition(105, 160);
                                            Font loginHistoryFont;
                                            if (!loginHistoryFont.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
                                                std::cout << "Font loading failed!\n";
                                            }
                                            Text title("Login History", loginHistoryFont, 80);
                                            title.setFillColor(Color::Red);
                                            title.setPosition(230, 60);
                                            Text headerUsername("Username", loginHistoryFont, 40);
                                            Text headerPassword("Password", loginHistoryFont, 40);
                                            headerUsername.setPosition(200, 180);
                                            headerPassword.setPosition(500, 180);
                                            headerUsername.setFillColor(Color::Yellow);
                                            headerPassword.setFillColor(Color::Yellow);
                                            Text prevButton("<< Previous", loginHistoryFont, 35);
                                            Text nextButton("Next >>", loginHistoryFont, 35);
                                            prevButton.setPosition(150, 650);
                                            nextButton.setPosition(650, 650);
                                            prevButton.setFillColor(Color::White);
                                            nextButton.setFillColor(Color::White);
                                            Text pageText("Page 1/" + to_string(totalPages), loginHistoryFont, 35);
                                            FloatRect pageBounds = pageText.getGlobalBounds();
                                            pageText.setPosition((960 - pageBounds.width) / 2, 650);
                                            while (loginHistory.isOpen()) {
                                                Event ev;
                                                while (loginHistory.pollEvent(ev)) {
                                                    if (ev.type == Event::Closed || (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape)) {
                                                        loginHistory.close();
                                                    }
                                                    if (ev.type == Event::MouseButtonReleased) {
                                                        Vector2f mousePos = loginHistory.mapPixelToCoords(Mouse::getPosition(loginHistory));
                                                        if (prevButton.getGlobalBounds().contains(mousePos)) {
                                                            buttonSound.play();
                                                            if (currentPage == 0) {
                                                                loginHistory.close();
                                                                changeTheme(0, background, true);
                                                            }
                                                            else {
                                                                currentPage--;
                                                                pageText.setString("Page " + to_string(currentPage + 1) + "/" + to_string(totalPages));
                                                            }
                                                        }
                                                        if (nextButton.getGlobalBounds().contains(mousePos) && currentPage < totalPages - 1) {
                                                            buttonSound.play();
                                                            currentPage++;
                                                            pageText.setString("Page " + to_string(currentPage + 1) + "/" + to_string(totalPages));
                                                        }
                                                    }
                                                }
                                                Vector2f mousePos = loginHistory.mapPixelToCoords(Mouse::getPosition(loginHistory));
                                                if (prevButton.getGlobalBounds().contains(mousePos)) {
                                                    prevButton.setFillColor(Color::Yellow);
                                                }
                                                else {
                                                    prevButton.setFillColor(Color::White);
                                                }
                                                if (nextButton.getGlobalBounds().contains(mousePos)) {
                                                    nextButton.setFillColor(Color::Yellow);
                                                }
                                                else {
                                                    nextButton.setFillColor(Color::White);
                                                }
                                                loginHistory.clear();
                                                loginHistory.draw(bgSprite);
                                                loginHistory.draw(bgRect);
                                                loginHistory.draw(title);
                                                loginHistory.draw(headerUsername);
                                                loginHistory.draw(headerPassword);
                                                loginHistory.draw(prevButton);
                                                loginHistory.draw(nextButton);
                                                loginHistory.draw(pageText);
                                                int start = currentPage * ENTRIES_PER_PAGE;
                                                int end = min(start + ENTRIES_PER_PAGE, totalEntries);
                                                float yPos = 240.0f;
                                                for (int i = start; i < end; i++) {
                                                    Text userText(usernames[i], loginHistoryFont, 35);
                                                    userText.setPosition(200, yPos);
                                                    userText.setFillColor(Color::White);
                                                    string realPass(passwords[i]);
                                                    string maskedPass(realPass.length(), '*');
                                                    Text passText(maskedPass, loginHistoryFont, 35);
                                                    passText.setPosition(500, yPos);
                                                    passText.setFillColor(Color::White);
                                                    loginHistory.draw(userText);
                                                    loginHistory.draw(passText);
                                                    yPos += 40.0f;
                                                }
                                                loginHistory.display();
                                            }
                                        }
                                        else if (choice == 2) { // Theme
                                            RenderWindow themeWindow(VideoMode(960, 720), "Theme");
                                            Font themeFont;
                                            if (!themeFont.loadFromFile("Fonts/AlexandriaFLF.ttf")) {
                                                cout << "Font loading failed!\n";
                                            }
                                            Texture bgTexture;
                                            if (!bgTexture.loadFromFile("images/pause_bg.jpg")) {
                                                cout << "Failed to load background image.\n";
                                            }
                                            Sprite bgSprite;
                                            bgSprite.setTexture(bgTexture);
                                            bgSprite.setPosition(150, 50);
                                            Text title;
                                            title.setFont(themeFont);
                                            title.setString("Theme");
                                            title.setCharacterSize(100);
                                            title.setFillColor(Color::Red);
                                            title.setPosition(340, 60);
#define MAX_THEME_OPTIONS 5
                                            Text themeOptions[MAX_THEME_OPTIONS];
                                            string themeNames[MAX_THEME_OPTIONS] = {
                                                "Dark Gray",
                                                "Dark Navy",
                                                "Space Purple",
                                                "Muted Brown",
                                                "Back"
                                            };
                                            for (int i = 0; i < MAX_THEME_OPTIONS; i++) {
                                                themeOptions[i].setFont(themeFont);
                                                themeOptions[i].setString(themeNames[i]);
                                                themeOptions[i].setCharacterSize(60);
                                                themeOptions[i].setPosition(300, 200 + i * 100);
                                                themeOptions[i].setFillColor(i == 0 ? Color::Yellow : Color::White);
                                            }
                                            int themeSelected = 0;
                                            while (themeWindow.isOpen()) {
                                                Event ev;
                                                while (themeWindow.pollEvent(ev)) {
                                                    if (ev.type == Event::Closed || (ev.type == Event::KeyPressed && ev.key.code == Keyboard::Escape)) {
                                                        themeWindow.close();
                                                    }
                                                    if (ev.type == Event::KeyReleased) {
                                                        if (ev.key.code == Keyboard::Up) {
                                                            navigationSound.play();
                                                            if (themeSelected > 0) {
                                                                themeOptions[themeSelected].setFillColor(Color::White);
                                                                themeSelected--;
                                                                themeOptions[themeSelected].setFillColor(Color::Yellow);
                                                            }
                                                        }
                                                        else if (ev.key.code == Keyboard::Down) {
                                                            navigationSound.play();
                                                            if (themeSelected < MAX_THEME_OPTIONS - 1) {
                                                                themeOptions[themeSelected].setFillColor(Color::White);
                                                                themeSelected++;
                                                                themeOptions[themeSelected].setFillColor(Color::Yellow);
                                                            }
                                                        }
                                                        else if (ev.key.code == Keyboard::Return) {
                                                            selectionSound.play();
                                                            if (themeSelected == 4) {
                                                                themeWindow.close();
                                                            }
                                                            else {
                                                                saveThemeToFile(themeSelected);
                                                                confirmationText.setString("Theme selected successfully!");
                                                                MENU.draw(confirmationText);
                                                                MENU.display();
                                                                sleep(seconds(1));
                                                                confirmationText.setString("");
                                                                themeWindow.close();
                                                            }
                                                        }
                                                    }
                                                }
                                                themeWindow.clear();
                                                themeWindow.draw(bgSprite);
                                                themeWindow.draw(title);
                                                for (int i = 0; i < MAX_THEME_OPTIONS; i++) {
                                                    themeWindow.draw(themeOptions[i]);
                                                }
                                                themeWindow.display();
                                            }
                                        }
                                        else if (choice == 3) { // Multiplayer
                                            Multiplayer multiplayer;
                                            if (multiplayer.showPlayerSubmission(OPTIONS)) {
                                                OPTIONS.close();
                                                multiplayer.startMultiplayerGame();
                                            }
                                        }
                                        else if (choice == 4) { // Back
                                            OPTIONS.close();
                                            changeTheme(0, background, true);
                                        }
                                    }
                                }
                            }
                            OPTIONS.clear();
                            OPTIONS.draw(Obackground);
                            optMenu.draw(OPTIONS);
                            OPTIONS.display();
                        }
                        backgroundMusic.play();
                    }


                    else if (x == 2) { // About (Instructions)

                        // Storing the current texture reference before opening instructions
                        const Texture* currentTexture = background.getTexture();

                        RenderWindow ABOUT(VideoMode(960, 720), "ABOUT");
                        while (ABOUT.isOpen()) {
                            Event aevent;
                            while (ABOUT.pollEvent(aevent)) {
                                if (aevent.type == Event::Closed || (aevent.type == Event::KeyPressed && aevent.key.code == Keyboard::Escape)) {
                                    // Restoring the original texture reference
                                    background.setTexture(currentTexture, true);
                                    background.setFillColor(Color::White);
                                    ABOUT.close();
                                }
                            }
                            ABOUT.clear();
                            ABOUT.draw(ABbackground);
                            ABOUT.display();
                        }
                        backgroundMusic.play();
                    }
                    else if (x == 3) { // Exit
                        MENU.close();
                    }
                    break;
                }
            }
        }
        MENU.clear();
        MENU.draw(background);
        mainMenu.draw(MENU);
        MENU.display();
    }

    return 0;
}