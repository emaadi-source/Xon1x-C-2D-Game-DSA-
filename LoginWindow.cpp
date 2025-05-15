#include "LoginWindow.h"
#include <iostream>
#include <fstream>

MinHeap LoginWindow::leaderboardHeap;

LoginWindow::LoginWindow() {
    leaderboardHeap.loadFromFile("users.txt");
    font.loadFromFile("Fonts/AlexandriaFLF.ttf");

    bgTexture.loadFromFile("images/pause_bg.jpg");
    bgSprite.setTexture(bgTexture);
    bgSprite.setPosition(150, 50);

    title.setFont(font);
    title.setString("Login/ Sign Up");
    title.setCharacterSize(100);
    title.setFillColor(Color::Red);
    title.setPosition(200, 60);

    userPrompt.setFont(font);
    userPrompt.setCharacterSize(25);
    userPrompt.setFillColor(Color::White);
    userPrompt.setPosition(270, 270);

    passPrompt.setFont(font);
    passPrompt.setCharacterSize(25);
    passPrompt.setFillColor(Color::White);
    passPrompt.setPosition(270, 350);

    statusText.setFont(font);
    statusText.setCharacterSize(22);
    statusText.setFillColor(Color::Red);
    statusText.setPosition(280, 500);

    loginButton.setSize(Vector2f(150, 40));
    loginButton.setFillColor(Color(255, 165, 0));
    loginButton.setPosition(440, 400);

    signUpButton.setSize(Vector2f(150, 40));
    signUpButton.setFillColor(Color(0, 255, 0));
    signUpButton.setPosition(440, 460);

    buttonTextLogin.setFont(font);
    buttonTextLogin.setString("Login");
    buttonTextLogin.setCharacterSize(24);
    buttonTextLogin.setFillColor(Color::Black);
    buttonTextLogin.setPosition(460, 405);

    buttonTextSignUp.setFont(font);
    buttonTextSignUp.setString("Sign Up");
    buttonTextSignUp.setCharacterSize(24);
    buttonTextSignUp.setFillColor(Color::Black);
    buttonTextSignUp.setPosition(460, 465);

    isTypingUser = true;
    username = "";
    password = "";
}

void LoginWindow::reloadLeaderboard() {
    leaderboardHeap.loadFromFile("users.txt");
}

bool LoginWindow::isUsernameTaken(const string& name) {
    ifstream inFile("users.txt");
    string line, u, p;
    while (getline(inFile, line)) {
        size_t sep = line.find(':');
        if (sep != string::npos) {
            u = line.substr(0, sep);
            if (u == name) return true;
        }
    }
    return false;
}

void LoginWindow::updateUserScore(const string& username, int newScore) {
    leaderboardHeap.insert(username, newScore);

    ifstream inFile("users.txt");
    if (!inFile.is_open()) {
        cout << "Error: Could not open users.txt for reading in updateUserScore\n";
        return;
    }

    ofstream outFile("temp.txt");
    if (!outFile.is_open()) {
        cout << "Error: Could not open temp.txt for writing in updateUserScore\n";
        inFile.close();
        return;
    }

    string line;
    bool userFound = false;

    while (getline(inFile, line)) {
        size_t firstColon = line.find(':');
        size_t secondColon = line.find(':', firstColon + 1);

        if (firstColon != string::npos && secondColon != string::npos) {
            string storedUser = line.substr(0, firstColon);
            string storedPass = line.substr(firstColon + 1, secondColon - firstColon - 1);
            int currentScore = stoi(line.substr(secondColon + 1));

            if (storedUser == username) {
                currentScore = max(currentScore, newScore);
                userFound = true;
            }
            outFile << storedUser << ":" << storedPass << ":" << currentScore << endl;
        }
        else {
            outFile << line << endl;
        }
    }
    inFile.close();
    outFile.close();

    if (!userFound) {
        cout << "Warning: User " << username << " not found in users.txt\n";
    }

    if (remove("users.txt") != 0) {
        cout << "Error: Could not remove users.txt\n";
    }
    if (rename("temp.txt", "users.txt") != 0) {
        cout << "Error: Could not rename temp.txt to users.txt\n";
    }

    leaderboardHeap.loadFromFile("users.txt");
    cout << "Updated score for " << username << " to " << newScore << "\n"; // Debugs output
}

bool LoginWindow::validateCredentials(const string& username, const string& password) {
    if (password.length() < 6) {
        statusText.setString("Password must be at least 6 characters long!");
        return false;
    }

    ifstream inFile("users.txt");
    string line, u, p;
    while (getline(inFile, line)) {
        size_t sep1 = line.find(':');
        size_t sep2 = line.find(':', sep1 + 1);

        if (sep1 != string::npos) {
            u = line.substr(0, sep1);
            p = (sep2 != string::npos) ?
                line.substr(sep1 + 1, sep2 - sep1 - 1) :
                line.substr(sep1 + 1);

            if (u == username && p == password) {
                return true;
            }
        }
    }
    return false;
}

void LoginWindow::saveUser(const string& name, const string& pass) {
    ofstream outFile("users.txt", ios::app);
    outFile << name << ":" << pass << ":0" << endl;
}

bool LoginWindow::isPasswordValid(const string& password) {
    return password.length() >= 6;
}

bool LoginWindow::validateUser(const string& username, const string& password) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        size_t delim1 = line.find(':');
        if (delim1 != string::npos && line.substr(0, delim1) == username) {
            size_t delim2 = line.find(':', delim1 + 1);
            if (delim2 != string::npos && line.substr(delim1 + 1, delim2 - delim1 - 1) == password) {
                file.close();
                return true;
            }
        }
    }
    file.close();
    return false;
}

bool LoginWindow::registerUser(const string& username, const string& password) {
    if (username.empty() || password.empty()) return false;

    ifstream inFile("users.txt");
    string line;
    while (getline(inFile, line)) {
        size_t delim = line.find(':');
        if (delim != string::npos && line.substr(0, delim) == username) {
            inFile.close();
            return false;
        }
    }
    inFile.close();

    ofstream outFile("users.txt", ios::app);
    outFile << username << ":" << password << ":0" << endl;
    outFile.close();

    leaderboardHeap.insert(username, 0);
    return true;
}

int LoginWindow::getUserScore(const string& username) {
    ifstream file("users.txt");
    string line;
    while (getline(file, line)) {
        size_t delim1 = line.find(':');
        if (delim1 != string::npos && line.substr(0, delim1) == username) {
            size_t delim2 = line.find(':', delim1 + 1);
            if (delim2 != string::npos) {
                try {
                    return stoi(line.substr(delim2 + 1));
                }
                catch (...) {
                    return 0;
                }
            }
        }
    }
    return 0;
}

bool LoginWindow::show(RenderWindow& parentWindow) {
    username = "";
    password = "";
    isTypingUser = true;
    statusText.setString("");

    while (parentWindow.isOpen()) {
        Event event;
        while (parentWindow.pollEvent(event)) {
            if (event.type == Event::Closed)
                return false;

            if (event.type == Event::TextEntered) {
                if (event.text.unicode == 8) {
                    if (isTypingUser && !username.empty()) username.pop_back();
                    else if (!isTypingUser && !password.empty()) password.pop_back();
                }
                else if (event.text.unicode >= 32 && event.text.unicode < 127) {
                    if (isTypingUser) username += static_cast<char>(event.text.unicode);
                    else password += static_cast<char>(event.text.unicode);
                }
            }
            if (event.type == Event::KeyPressed) {
                if (event.key.code == Keyboard::Escape) {
                    return false;
                }
                if (event.key.code == Keyboard::Tab) {
                    isTypingUser = !isTypingUser;
                }
                if (event.key.code == Keyboard::Enter) {
                    if (isTypingUser) {
                        isTypingUser = false;
                    }
                    else {
                        if (username.empty() || password.empty()) {
                            statusText.setString("Enter both fields!");
                        }
                        else if (!isPasswordValid(password)) {
                            statusText.setString("Password must be at least 6 characters!");
                        }
                        else if (isUsernameTaken(username)) {
                            if (validateCredentials(username, password)) {
                                statusText.setString("Login successful!");
                                return true;
                            }
                            else {
                                statusText.setString("Incorrect username or password!");
                            }
                        }
                        else {
                            statusText.setString("Username not found! Please sign up.");
                        }
                    }
                }
            }

            if (event.type == Event::MouseButtonPressed) {
                Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);

                if (loginButton.getGlobalBounds().contains(mousePos)) {
                    if (username.empty() || password.empty()) {
                        statusText.setString("Enter both fields!");
                    }
                    else if (!isPasswordValid(password)) {
                        statusText.setString("Password must be at least 6 characters!");
                    }
                    else if (isUsernameTaken(username)) {
                        if (validateCredentials(username, password)) {
                            statusText.setString("Login successful!");
                            return true;
                        }
                        else {
                            statusText.setString("Incorrect username or password!");
                        }
                    }
                    else {
                        statusText.setString("Username not found! Please sign up.");
                    }
                }

                if (signUpButton.getGlobalBounds().contains(mousePos)) {
                    if (username.empty() || password.empty()) {
                        statusText.setString("Enter both fields to sign up!");
                    }
                    else if (!isPasswordValid(password)) {
                        statusText.setString("Password must be at least 6 characters long!");
                    }
                    else if (isUsernameTaken(username)) {
                        statusText.setString("Player already exists!");
                    }
                    else {
                        saveUser(username, password);
                        statusText.setString("Sign up successful! You can now login.");
                    }
                }
            }
        }

        string userLabel = "Username: " + username + (isTypingUser ? " _" : "");
        string passLabel = "Password: " + string(password.length(), '*') + (!isTypingUser ? " _" : "");
        userPrompt.setString(userLabel);
        passPrompt.setString(passLabel);

        parentWindow.clear();
        parentWindow.draw(bgSprite);
        parentWindow.draw(title);
        parentWindow.draw(userPrompt);
        parentWindow.draw(passPrompt);
        parentWindow.draw(loginButton);
        parentWindow.draw(buttonTextLogin);
        parentWindow.draw(signUpButton);
        parentWindow.draw(buttonTextSignUp);
        parentWindow.draw(statusText);
        parentWindow.display();
    }

    return false;
}