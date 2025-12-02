#include <iostream>
#include <random>
#include <chrono>
#include <thread>
using namespace std;

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
    while (true)
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
}

int main()
{
    const char FPS = 30;

    const char mapX = 10;
    const char mapY = 10;
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

    Draw(ptrMap, mapX, mapY, snakeHeadX, snakeHeadY, fruitHeadX, fruitHeadY, FPS, map);

    delete[] map;
    return 0;
}