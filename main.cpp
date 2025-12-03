#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <termios.h>
#include <sys/select.h>


using namespace std;

enum eDirection { STOP, UP, DOWN, LEFT, RIGHT };

termios oldt, newt;
void initTermios()
{
    tcgetattr(0, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(0, TCSANOW, &newt);
}

void resetTermios()
{
    tcsetattr(0, TCSANOW, &oldt);
}

int kbhit()
{
    timeval tv = { 0, 0 };
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(0, &fds);
    return select(1, &fds, NULL, NULL, &tv) > 0;
}

void Draw(char* ptrMap,
    short X,
    short Y,
    char snakeHeadX,
    char snakeHeadY,
    char fruitHeadX,
    char fruitHeadY,
    char FPS,
    char* map
    )
{
    ptrMap = map;
    for (int y = 0; y <= Y; y++)
    {
        for (int x = 0; x <= X; x++)
        {
            if (y == 0 || y == Y || x == 0 || x == X)
            {
                *(ptrMap++) = '#';
                if (x == X)
                {
                    *(ptrMap++) = '\n';
                }
            }
            else if (y == snakeHeadY && x == snakeHeadX)
            {
                *(ptrMap++) = 'O';
            }
            else if (y == fruitHeadY && x == fruitHeadX)
            {
                *(ptrMap++) = 'F';
            }
            else
            {
                *(ptrMap++) = ' ';
            }
        }
    }
        *(ptrMap++) = '\0';
        this_thread::sleep_for(chrono::milliseconds(1000/FPS));
        cout << "\033[2J\033[1;1H"<<map;
}

void buttonPressed(char& dir)
{
    if (kbhit())
    {
        switch (getchar())
        {
            case 'a':
            case 'A':
                dir = eDirection::LEFT;
                break;
            case 'd':
            case 'D':
                dir = eDirection::RIGHT;
                break;
            case 'w':
            case 'W':
                dir = eDirection::UP;
                break;
            case 's':
            case 'S':
                dir = eDirection::DOWN;
                break;
        }
    }
}

void changeDirection(char& dir, char& snakeHeadX, char& snakeHeadY)
{
    switch (dir)
    {
        case eDirection::LEFT:
            snakeHeadX--;
            break;
        case eDirection::RIGHT:
            snakeHeadX++;
            break;
        case eDirection::UP:
            snakeHeadY--;
            break;
        case eDirection::DOWN:
            snakeHeadY++;
            break;
    }
}

int main()
{
    initTermios();
    const char FPS = 30;

    const char mapX = 50;
    const char mapY = 50;
    char* map = new char[(mapX+2)*(mapY+2)];
    char* ptrMap = map;

    char snakeHeadX;
    char snakeHeadY;

    char fruitHeadX;
    char fruitHeadY;

    random_device rd;
    mt19937 mt(rd());

    uniform_int_distribution<int> distX(1, mapX-1);
    uniform_int_distribution<int> distY(1, mapY-1);

    snakeHeadX = mapX/2;
    snakeHeadY = mapY/2;
    fruitHeadX = distX(mt);
    fruitHeadY = distY(mt);

    char dir = eDirection::STOP;

    while (true)
    {
        buttonPressed(dir);
        changeDirection(dir, snakeHeadX, snakeHeadY);
        Draw(ptrMap, mapX, mapY, snakeHeadX, snakeHeadY, fruitHeadX, fruitHeadY, FPS, map);
    }

    delete[] map;
    resetTermios();
    return 0;
}