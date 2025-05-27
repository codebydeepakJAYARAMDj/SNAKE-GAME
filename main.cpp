#include <iostream>
#include <vector>
#include <windows.h>
#include <conio.h>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <algorithm>

using namespace std;

// Game settings
const int WIDTH = 30;
const int HEIGHT = 20;
const int MAX_SPEED_LEVEL = 3;

// Game elements
struct Point { int x, y; };
Point snakeHead;
vector<Point> snakeBody;
vector<Point> obstacles;
Point food;
int score;
int highScore;
int speedLevel;
int gameSpeed;
bool quitGame;

// Game states
enum GameState { MENU, PLAYING, GAME_OVER, PAUSED };
GameState gameState;

// Directions
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;

// Colors
enum ConsoleColor {
    BLACK = 0,
    DARKGREEN = 2,
    GREEN = 10,
    RED = 12,
    YELLOW = 14,
    WHITE = 15
};

// Function prototypes
void Setup();
void Draw();
void Input();
void Logic();
void SaveHighScore();
void LoadHighScore();
void SetConsoleColor(ConsoleColor color);
void ClearScreen();
void DrawMenu();
void DrawGameOver();
void GenerateObstacles();
void GenerateFood();
void TogglePause();
void HideCursor();
void DrawBorder();

int main() {
    // Initialize console
    system(("mode con cols=" + to_string(WIDTH + 10) + " lines=" + to_string(HEIGHT + 10)).c_str());
    HideCursor();
    srand(time(0));

    // Initialize game
    LoadHighScore();
    Setup();

    // Main game loop
    while (!quitGame) {
        Input();
        if (gameState == PLAYING) {
            Logic();
        }
        Draw();
        Sleep(gameSpeed);
    }

    return 0;
}

void Setup() {
    gameState = MENU;
    quitGame = false;
    snakeHead = { WIDTH / 2, HEIGHT / 2 };
    snakeBody.clear();
    snakeBody.push_back(snakeHead);
    score = 0;
    speedLevel = 1;
    gameSpeed = 150;
    dir = STOP;
    obstacles.clear();
    GenerateFood();
}

void Draw() {
    ClearScreen();

    switch (gameState) {
        case MENU: {
            DrawMenu();
            break;
        }
        case PLAYING:
        case PAUSED: {
            DrawBorder();

            // Draw obstacles
            SetConsoleColor(YELLOW);
            for (const auto& obs : obstacles) {
                COORD pos = { (SHORT)(obs.x + 1), (SHORT)(obs.y + 1) };
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
                cout << "X";
            }

            // Draw food
            SetConsoleColor(RED);
            COORD foodPos = { (SHORT)(food.x + 1), (SHORT)(food.y + 1) };
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), foodPos);
            cout << "@";

            // Draw snake
            SetConsoleColor(GREEN);
            for (size_t i = 0; i < snakeBody.size(); i++) {
                COORD pos = { (SHORT)(snakeBody[i].x + 1), (SHORT)(snakeBody[i].y + 1) };
                SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
                cout << (i == 0 ? "O" : "o");
            }

            // Draw info
            SetConsoleColor(WHITE);
            COORD infoPos = { 0, (SHORT)(HEIGHT + 2) };
            SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), infoPos);
            cout << "Score: " << score << " | High Score: " << highScore;
            cout << " | Speed: " << speedLevel << "/" << MAX_SPEED_LEVEL;

            if (gameState == PAUSED) {
                SetConsoleColor(YELLOW);
                cout << " | PAUSED (Press P to resume)";
            }
            break;
        }
        case GAME_OVER: {
            DrawGameOver();
            break;
        }
    }
}

void Input() {
    if (_kbhit()) {
        char key = _getch();

        switch (gameState) {
            case MENU: {
                switch (tolower(key)) {
                    case '1':
                        gameState = PLAYING;
                        GenerateObstacles();
                        dir = RIGHT;
                        break;
                    case '2':
                        quitGame = true;
                        break;
                }
                break;
            }
            case PLAYING: {
                switch (tolower(key)) {
                    case 'w': if (dir != DOWN) dir = UP; break;
                    case 's': if (dir != UP) dir = DOWN; break;
                    case 'a': if (dir != RIGHT) dir = LEFT; break;
                    case 'd': if (dir != LEFT) dir = RIGHT; break;
                    case 'p': TogglePause(); break;
                    case 'q': quitGame = true; break;
                    case '+':
                        if (speedLevel < MAX_SPEED_LEVEL) {
                            speedLevel++;
                            gameSpeed = max(50, 200 - (speedLevel - 1) * 50);
                        }
                        break;
                    case '-':
                        if (speedLevel > 1) {
                            speedLevel--;
                            gameSpeed = max(50, 200 - (speedLevel - 1) * 50);
                        }
                        break;
                }
                break;
            }
            case PAUSED: {
                if (tolower(key) == 'p') TogglePause();
                break;
            }
            case GAME_OVER: {
                switch (tolower(key)) {
                    case 'r': Setup(); break;
                    case 'q': quitGame = true; break;
                }
                break;
            }
        }
    }
}

void Logic() {
    for (size_t i = snakeBody.size() - 1; i > 0; i--) {
        snakeBody[i] = snakeBody[i - 1];
    }

    switch (dir) {
        case UP: snakeHead.y--; break;
        case DOWN: snakeHead.y++; break;
        case LEFT: snakeHead.x--; break;
        case RIGHT: snakeHead.x++; break;
        default: break;
    }

    snakeBody[0] = snakeHead;

    if (snakeHead.x < 0 || snakeHead.x >= WIDTH || snakeHead.y < 0 || snakeHead.y >= HEIGHT) {
        gameState = GAME_OVER;
        return;
    }

    for (const auto& obs : obstacles) {
        if (snakeHead.x == obs.x && snakeHead.y == obs.y) {
            gameState = GAME_OVER;
            return;
        }
    }

    for (size_t i = 1; i < snakeBody.size(); i++) {
        if (snakeHead.x == snakeBody[i].x && snakeHead.y == snakeBody[i].y) {
            gameState = GAME_OVER;
            return;
        }
    }

    if (snakeHead.x == food.x && snakeHead.y == food.y) {
        score += 10 * speedLevel;
        if (score > highScore) {
            highScore = score;
            SaveHighScore();
        }

        snakeBody.push_back(snakeBody.back());
        GenerateFood();
    }
}

void SaveHighScore() {
    ofstream file("highscore.txt");
    if (file.is_open()) {
        file << highScore;
        file.close();
    }
}

void LoadHighScore() {
    ifstream file("highscore.txt");
    if (file.is_open()) {
        file >> highScore;
        file.close();
    } else {
        highScore = 0;
    }
}

void SetConsoleColor(ConsoleColor color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

void ClearScreen() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD topLeft = {0, 0};
    CONSOLE_SCREEN_BUFFER_INFO screen;
    DWORD written;

    GetConsoleScreenBufferInfo(hConsole, &screen);
    FillConsoleOutputCharacterA(
        hConsole, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
    );
    SetConsoleCursorPosition(hConsole, topLeft);
}

void DrawBorder() {
    SetConsoleColor(DARKGREEN);
    COORD pos;

    pos.X = 0;
    pos.Y = 0;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    for (int i = 0; i < WIDTH + 2; i++) cout << "#";

    for (int i = 1; i <= HEIGHT; i++) {
        pos.X = 0;
        pos.Y = i;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        cout << "#";

        pos.X = WIDTH + 1;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
        cout << "#";
    }

    pos.X = 0;
    pos.Y = HEIGHT + 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    for (int i = 0; i < WIDTH + 2; i++) cout << "#";
}

void DrawMenu() {
    SetConsoleColor(YELLOW);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << "=== SNAKE GAME ===\n\n";

    SetConsoleColor(WHITE);
    cout << "1. Start Game\n";
    cout << "2. Quit\n\n";

    cout << "Controls:\n";
    cout << "WASD - Move\n";
    cout << "P - Pause\n";
    cout << "+/- - Change Speed\n";
    cout << "Q - Quit\n";
}

void DrawGameOver() {
    SetConsoleColor(RED);
    COORD pos = {0, 0};
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
    cout << "=== GAME OVER ===\n\n";

    SetConsoleColor(WHITE);
    cout << "Final Score: " << score << "\n";
    cout << "High Score: " << highScore << "\n\n";
    cout << "R. Restart Game\n";
    cout << "Q. Quit\n";
}

void GenerateObstacles() {
    obstacles.clear();
    int obstacleCount = 5 + speedLevel * 2;

    for (int i = 0; i < obstacleCount; i++) {
        Point obs;
        do {
            obs.x = rand() % WIDTH;
            obs.y = rand() % HEIGHT;
        } while (
            (obs.x == snakeHead.x && obs.y == snakeHead.y) ||
            (obs.x == food.x && obs.y == food.y)
        );
        obstacles.push_back(obs);
    }
}

void GenerateFood() {
    do {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    } while (
        (food.x == snakeHead.x && food.y == snakeHead.y) ||
        any_of(obstacles.begin(), obstacles.end(), [](const Point& obs) {
            return food.x == obs.x && food.y == obs.y;
        }) ||
        any_of(snakeBody.begin(), snakeBody.end(), [](const Point& s) {
            return food.x == s.x && food.y == s.y;
        })
    );
}

void TogglePause() {
    gameState = (gameState == PLAYING) ? PAUSED : PLAYING;
}

void HideCursor() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = false;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

