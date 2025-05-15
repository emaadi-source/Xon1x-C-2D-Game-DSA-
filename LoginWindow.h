#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;
using namespace sf;

struct PlayerScore {
    string username;
    int score;
    PlayerScore() : username(""), score(0) {}
    PlayerScore(const string& u, int s) : username(u), score(s) {}
};

class MinHeap {
    PlayerScore heap[10];
    int size;

    void swapPlayers(PlayerScore& a, PlayerScore& b) {
        PlayerScore temp = a;
        a = b;
        b = temp;
    }

    void heapifyUp(int index) {
        while (index > 0) {
            int parent = (index - 1) / 2;
            if (heap[index].score < heap[parent].score) {
                swapPlayers(heap[index], heap[parent]);
                index = parent;
            }
            else break;
        }
    }

    void heapifyDown(int index) {
        int smallest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if (left < size && heap[left].score < heap[smallest].score)
            smallest = left;
        if (right < size && heap[right].score < heap[smallest].score)
            smallest = right;

        if (smallest != index) {
            swapPlayers(heap[index], heap[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    MinHeap() : size(0) {}

    void insert(const string& username, int score) {
        for (int i = 0; i < size; i++) {
            if (heap[i].username == username) {
                if (score > heap[i].score) {
                    heap[i].score = score;
                    heapifyUp(i);
                    heapifyDown(i);
                }
                return;
            }
        }

        if (size < 10) {
            heap[size] = PlayerScore(username, score);
            heapifyUp(size++);
        }
        else if (score > heap[0].score) {
            heap[0] = PlayerScore(username, score);
            heapifyDown(0);
        }
    }

    void loadFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error: Could not open " << filename << " for reading leaderboard\n";
            return;
        }

        size = 0;
        string line;
        while (getline(file, line) && size < 10) {
            size_t colon1 = line.find(':');
            size_t colon2 = line.find(':', colon1 + 1);
            if (colon1 == string::npos || colon2 == string::npos) {
                cout << "Warning: Skipping malformed line in " << filename << ": " << line << "\n";
                continue;
            }

            string user = line.substr(0, colon1);
            string scoreStr = line.substr(colon2 + 1);

            try {
                int score = stoi(scoreStr);
                insert(user, score);
                cout << "Loaded player: " << user << " with score: " << score << "\n"; // Debugs output
            }
            catch (...) {
                cout << "Warning: Invalid score in " << filename << " for line: " << line << "\n";
                continue;
            }
        }
        file.close();
        cout << "Loaded " << size << " players from " << filename << "\n"; // Debugs output
    }

    int getSize() const { return size; }

    void getSorted(PlayerScore* output, int& count) {
        count = 0;
        PlayerScore temp[10];
        int tempSize = size;

        for (int i = 0; i < size; i++) temp[i] = heap[i];

        while (tempSize > 0) {
            output[count++] = temp[0];
            temp[0] = temp[--tempSize];

            int index = 0;
            while (true) {
                int left = 2 * index + 1;
                int right = 2 * index + 2;
                int smallest = index;

                if (left < tempSize && temp[left].score < temp[smallest].score)
                    smallest = left;
                if (right < tempSize && temp[right].score < temp[smallest].score)
                    smallest = right;

                if (smallest != index) {
                    swapPlayers(temp[index], temp[smallest]);
                    index = smallest;
                }
                else break;
            }
        }

        for (int i = 0; i < count / 2; i++) {
            swapPlayers(output[i], output[count - 1 - i]);
        }
    }
};

class LoginWindow {
public:
    LoginWindow();
    bool show(RenderWindow& parentWindow);
    static void updateUserScore(const string& username, int score);
    static void reloadLeaderboard(); // New methods to reload leaderboard
    string getUsername() const { return username; }
    static MinHeap leaderboardHeap;

    static bool validateUser(const string& username, const string& password);
    static bool registerUser(const string& username, const string& password);
    static int getUserScore(const string& username);

private:
    bool isUsernameTaken(const string& name);
    bool validateCredentials(const string& username, const string& password);
    void saveUser(const string& name, const string& pass);
    bool isPasswordValid(const string& password);

    Font font;
    Text title;
    Text userPrompt;
    Text passPrompt;
    Text statusText;
    Text buttonTextLogin;
    Text buttonTextSignUp;

    RectangleShape loginButton;
    RectangleShape signUpButton;

    string username;
    string password;
    bool isTypingUser;

    Sprite bgSprite;
    Texture bgTexture;
};