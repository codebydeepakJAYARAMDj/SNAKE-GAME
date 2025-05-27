# SNAKE-GAME

*COMPANY* : CODTECH IT SOLUTIONS

*NAME* : VANKAPATI DEEPAK JAYA RAM YADAV

*INTERN ID* : CT08DN1557

*DOMAIN NAME* : C++ PROGRAMMING

*DURATION* : 8 WEEKS

*MENTOR* : NEELA SANTOSH

# Snake Game - README

## Table of Contents
1. [Description](#description)
2. [Features](#features)
3. [Installation](#installation)
4. [How to Play](#how-to-play)
5. [Controls](#controls)
6. [Game Mechanics](#game-mechanics)
7. [Customization](#customization)
8. [Technical Details](#technical-details)
9. [Future Improvements](#future-improvements)
10. [License](#license)

## Description
This is a classic Snake game implemented in C++ using Windows Console API. The game features a snake that grows in length as it eats food, with increasing difficulty through obstacles and speed levels.

## Features
- Classic Snake gameplay with score tracking
- Multiple speed levels (adjustable during gameplay)
- Randomly generated obstacles
- Persistent high score system
- Colorful console interface
- Pause functionality

## Installation
1. **Requirements**:
   - Windows OS
   - C++ compiler (g++ or Visual Studio)
   
2. **Compilation**:
   ```bash
   g++ snake_game.cpp -o snake_game.exe

3. **Execution**:
   ```bash
   snake_game.exe

**How to Play**
Run the executable

Select "1. Start Game" from menu

Control the snake to eat food (@)

Avoid walls, obstacles (X), and your tail

Game ends on collision

Press 'R' to restart or 'Q' to quit

**Controls**
- Key	Action
- WASD	Movement
- (+)	Increase Speed
- (-)	Decrease Speed
- P	Pause/Resume
- Q	Quit Game
- R	Restart

**Game Mechanics**

Scoring: 10 points × speed level per food

Speed Levels: 3 levels (50-150ms delay)

Obstacles: 5 + (speedLevel × 2)

Growth: +1 segment per food

**Customization**
Modify these constants:
```bash
const int WIDTH = 30;          // Game width
const int HEIGHT = 20;         // Game height
const int MAX_SPEED_LEVEL = 3; // Speed levels
```

**Technical Details**
-Windows Console API

-Traditional game loop

-Vector<Point> for snake/obstacles

-File I/O for high scores
