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

struct Snake
{
    char x, y;
};
struct Fruit
{
    char x, y;
};

void Draw(char* ptrMap,
    short X,
    short Y,
    Snake snakeHead,
    Fruit fruit,
    char FPS,
    char* map,
    Snake tail[],
    short tailLength
    )
{
    ptrMap = map;
    bool isTailSegmentDraw;
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
            else if (y == fruit.y && x == fruit.x)
            {
                *(ptrMap++) = 'F';
            }
            else if (y == snakeHead.y && x == snakeHead.x)
            {
                *(ptrMap++) = 'O';
            }
            else if (tailLength > 0)
            {
                isTailSegmentDraw = false;
                for (int i = 0; i < tailLength; i++)
                {
                    if (tail[i].x == x && tail[i].y == y)
                    {
                        *(ptrMap++) = '@';
                        isTailSegmentDraw = true;
                        break;//просто чтобы цыкл не работал больше чем надо
                    }
                }
                if (!isTailSegmentDraw) *(ptrMap++) = ' ';
            }
            else *(ptrMap++) = ' ';
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

bool isLoose(char snakeHeadX,char snakeHeadY, short X, short Y)
{
    if (snakeHeadX == 0 || snakeHeadX == X || snakeHeadY == Y || snakeHeadY == 0)
    {
        return true;
    } else {return false;}
}

void moveTail(Snake tail[], short tailLength, Snake snakeHead)
{
    if (tailLength != 0)
    {
        Snake beforePos = tail[0];
        tail[0] = snakeHead;

        for (short i = 1; i < tailLength; i++)
        {
            swap(tail[i], beforePos);
        }
    }
}

void generateFruitPosition(const char mapX, const char mapY, Fruit& fruit)
{
    random_device rd;
    mt19937 mt(rd());

    uniform_int_distribution<int> distX(1, mapX-1);
    uniform_int_distribution<int> distY(1, mapY-1);

    fruit.x = distX(mt);
    fruit.y = distY(mt);
}

void wasEaten(Snake snakeHead, Fruit& fruit, short& tailLength, const char mapX,
    const char mapY)
{
    if (snakeHead.x == fruit.x && snakeHead.y == fruit.y)
    {
        tailLength++;
        generateFruitPosition(mapX, mapY, fruit);
    }
}

int main()
{
    initTermios();
    const char FPS = 10;

    const char mapX = 30;
    const char mapY = 30;
    char* map = new char[(mapX+2)*(mapY+2)];
    char* ptrMap = map;

    Snake snakeHead;
    Fruit fruit;
    Snake tail[(mapX-1) * (mapY-1)];

    generateFruitPosition(mapX, mapY, fruit);

    snakeHead.x = mapX/2;
    snakeHead.y = mapY/2;

    char dir = eDirection::STOP;
    short tailLength = 0;

    while (true)
    {
        buttonPressed(dir);
        moveTail(tail, tailLength, snakeHead);
        changeDirection(dir, snakeHead.x, snakeHead.y);
        cout << "Score: "<<tailLength<< endl;
        if (isLoose(snakeHead.x, snakeHead.y, mapX, mapY))
        {
            break;
        }
        wasEaten(snakeHead, fruit, tailLength, mapX, mapY);
        Draw(ptrMap, mapX, mapY, snakeHead, fruit, FPS, map, tail, tailLength);
    }

    delete[] map;
    resetTermios();
    return 0;
}
