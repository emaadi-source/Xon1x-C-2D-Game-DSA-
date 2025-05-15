# 🎮 XON1X DSA based C++ Game Project

Welcome to **XON1X**, a feature-packed, data-structures-driven game built entirely in **C++** using **SFML** — with **zero STL** usage! Designed as a semester project under strict constraints, this game demonstrates mastery in algorithms, memory management, and object-oriented design.

## 🚀 About the Project

**XON1X** is a 2D multiplayer and single-player adventure game, developed with the goal of applying Data Structures and Algorithms (DSA) in a real-world context. From **AVL Trees** to **Min-Heaps**, everything is custom-implemented — no STL, just pure C++!

> "Not just a game — it's a live demonstration of algorithmic thinking."

---

## 🧠 Features

- 🔐 **Login & Registration** (with file-based authentication)
- 🏆 **Leaderboard** (MinHeap for top player ranking)
- 🎨 **Themes & Inventory** (AVL Tree for dynamic theme selection)
- 🧍 **Single-Player Mode** (Power-ups, enemies, scoring)
- 👥 **Multiplayer Mode** (Real-time two-player combat)
- 🤝 **Friend System** (Custom Linked List & Hash Table)
- 🎯 **Matchmaking Queue** (Priority Queue with rematch protection)
- 💾 **Save Game** & **Player Profiles** (Binary + text-based data storage)
- 🔊 **Sound Effects** & 🎮 **Graphics** powered by **SFML**

---

## ⚙️ Technologies Used

| Technology | Purpose |
|------------|---------|
| C++        | Core language (No STL allowed) |
| SFML       | Graphics, audio, and window handling |
| File I/O   | For login, save game, leaderboard |
| Custom DSA | AVL Tree, MinHeap, Hash Table, Stack, Linked List |
| OOP        | Modular class-based structure with inheritance & encapsulation |

---

## 🛠️ How to Run

### ✅ On **Windows (Visual Studio)**

1. **Install SFML:**
   - Download from: [SFML Downloads](https://www.sfml-dev.org/download.php)
   - Extract and set include/lib paths in Visual Studio project settings.

2. **Configure Project:**
   - Create a new C++ project in Visual Studio.
   - Link the following SFML libraries in *Project Properties > Linker > Input*:
     ```
     sfml-graphics.lib
     sfml-window.lib
     sfml-system.lib
     sfml-audio.lib
     ```
   - Set the include path in *C/C++ > General > Additional Include Directories*.
   - Set the library path in *Linker > General > Additional Library Directories*.
   - Copy SFML DLL files (from `bin/`) into your project executable directory.

3. **Build & Run:**
   - Open `Main.cpp` and run the project.
   - Enjoy XON1X! 🎉

---

### 🐧 On **Linux**

> Assumes you're using a Debian-based distro (like Ubuntu)

1. **Install SFML:**
   ```bash
   sudo apt update
   sudo apt install libsfml-dev
