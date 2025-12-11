#include <iostream>
#include <random>
#include <chrono>
#include <thread>


#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#elif __APPLE__
    #include <termios.h>
    #include <sys/select.h>
#endif

using namespace std;

struct Snake;
struct Fruit;
struct Map;

enum eDirection { STOP, UP, DOWN, LEFT, RIGHT };

#ifdef _WIN32

#elif __APPLE__
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
#endif


struct Snake
{
    unsigned char x, y;
    bool operator==(const Snake& s) { return x == s.x && y == s.y; }                            
};
struct Fruit
{
    unsigned char x, y;
};
struct Map
{
    unsigned char x, y;
};
bool operator==(const Map& m, const Fruit& f) {return m.x == f.x && m.y == f.y;}
bool operator==(const Map& m, const Snake& s) {return m.x == s.x && m.y == s.y;}
bool operator==(const Snake& s, const Map& m) {return s.x == m.x && s.y == m.y;}
bool operator==(const Fruit& f, const Map& m) {return f.x == m.x && f.y == m.y;}
bool operator==(const Snake& s, const Fruit& f) {return s.x == f.x && s.y == f.y;}
bool operator==(const Fruit& f, const Snake& s) {return f.x == s.x && f.y == s.y;}

void ClearTerminal() 
{
#ifdef _WIN32
    COORD coord = { 0,0 };
    DWORD count;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    GetConsoleScreenBufferInfo(h, &csbi);
    FillConsoleOutputCharacter(h, ' ', csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
    FillConsoleOutputAttribute(h, csbi.wAttributes, csbi.dwSize.X * csbi.dwSize.Y, coord, &count);
    SetConsoleCursorPosition(h, coord);
#elif __APPLE__
    cout << "\033[2J\033[1;1H" << map;
#endif
}

void Draw(unsigned char* ptrMap,
    const Map MAPCOORDINATES,
    Snake snakeHead,
    Fruit fruit,
    unsigned char FPS,
    unsigned char* map,
    Snake tail[],
    short tailLength
    )
{
    ptrMap = map;
    bool isTailSegmentDraw;
    for (int y = 0; y <= MAPCOORDINATES.y; y++)
    {
        for (int x = 0; x <= MAPCOORDINATES.x; x++)
        {
            if (y == 0 || y == MAPCOORDINATES.y || x == 0 || x == MAPCOORDINATES.x)
            {
                *(ptrMap++) = '#';
                if (x == MAPCOORDINATES.x)
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
        ClearTerminal();
}

int myKbhit() 
{
#ifdef _WIN32
    return _kbhit();
#elif __APPLE__
    return kbhit();
#endif
}

int myGetchr()
{
#ifdef _WIN32
    return _getch();
#elif __APPLE__
    return getchar();
#endif
}
 
void buttonPressed(char& dir)
{

    if (myKbhit())
    {
        switch (myGetchr())
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

void changeDirection(char& dir, Snake& snakeHead)
{
    switch (dir)
    {
        case eDirection::LEFT:
            snakeHead.x--;
            break;
        case eDirection::RIGHT:
            snakeHead.x++;
            break;
        case eDirection::UP:
            snakeHead.y--;
            break;
        case eDirection::DOWN:
            snakeHead.y++;
            break;
    }
}

bool isLoose(Snake snakeHead, Map MAPCOORDINATES)
{
    if (snakeHead.x == 0 || snakeHead.x == MAPCOORDINATES.x || snakeHead.y == MAPCOORDINATES.y || snakeHead.y == 0)
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

void generateFruitPosition(const Map MAP, Fruit& fruit, Snake tail[], short tailLength)
{
    random_device rd;
    mt19937 mt(rd());

    uniform_int_distribution<int> distX(1, MAP.x-1);
    uniform_int_distribution<int> distY(1, MAP.y-1);
}


void wasEaten(Snake snakeHead, Fruit& fruit, short& tailLength, const Map MAPCOORDINATES)
{
    if (snakeHead == fruit)
    {
        tailLength++;
        generateFruitPosition(MAPCOORDINATES, fruit);
    }
}

int main()
{
#ifdef __APPLE__
    initTermios();
#endif
    const unsigned char FPS = 10;

    const Map MAPCOORDINATES{30,30};
    unsigned char* map = new unsigned char[(MAPCOORDINATES.x+2)*(MAPCOORDINATES.y+2)];
    unsigned char* ptrMap = map;

    Snake snakeHead;
    Fruit fruit;
    Snake tail[(MAPCOORDINATES.x-1) * (MAPCOORDINATES.y-1)];

    generateFruitPosition(MAPCOORDINATES, fruit);

    snakeHead.x = MAPCOORDINATES.x/2;
    snakeHead.y = MAPCOORDINATES.y/2;

    char dir = eDirection::STOP;
    short tailLength = 0;

    while (true)
    {
        buttonPressed(dir);
        moveTail(tail, tailLength, snakeHead);
        changeDirection(dir, snakeHead);
        cout << "Score: "<<tailLength<< endl;
        if (isLoose(snakeHead, MAPCOORDINATES)) break;
        wasEaten(snakeHead, fruit, tailLength, MAPCOORDINATES);
        Draw(ptrMap, MAPCOORDINATES, snakeHead, fruit, FPS, map, tail, tailLength);
    }

    delete[] map;
#ifdef __APPLE__
    resetTermios();
#endif
    return 0;
}
