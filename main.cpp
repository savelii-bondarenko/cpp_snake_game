#include <iostream>
#include <random>
#include <chrono>
#include <thread>
#include <vector>


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
struct Map
{
    unsigned char x, y;
};
struct Fruit
{
    unsigned char x, y;
    Fruit& operator=(const Map& m) 
    { 
        x = m.x;
        y = m.y; 
        return *this;
    }
};

bool operator==(const Map& m, const Fruit& f) {return m.x == f.x && m.y == f.y;}
bool operator==(const Map& m, const Snake& s) {return m.x == s.x && m.y == s.y;}
bool operator==(const Snake& s, const Map& m) {return s.x == m.x && s.y == m.y;}
bool operator==(const Fruit& f, const Map& m) {return f.x == m.x && f.y == m.y;}
bool operator==(const Snake& s, const Fruit& f) {return s.x == f.x && s.y == f.y;}
bool operator==(const Fruit& f, const Snake& s) {return f.x == s.x && f.y == s.y;}

void ClearTerminal(unsigned char* map)
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = { 0, 0 }; 
    SetConsoleCursorPosition(hOut, coord);
    cout << map;
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
    vector<Snake> tail
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
            else if (tail.size() > 0)
            {
                isTailSegmentDraw = false;
                for (int i = 0; i < tail.size(); i++)
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
        ClearTerminal(map);
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
                if(dir == eDirection::RIGHT) break;
                dir = eDirection::LEFT;
                break;
            case 'd':
            case 'D':
                if (dir == eDirection::LEFT) break;
                dir = eDirection::RIGHT;
                break;
            case 'w':
            case 'W':
                if (dir == eDirection::DOWN) break;
                dir = eDirection::UP;
                break;
            case 's':
            case 'S':
                if (dir == eDirection::UP) break;
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

void moveTail(vector<Snake>& tail, Snake snakeHead)
{
    if (tail.size() != 0)
    {
        Snake beforePos = tail[0];
        tail[0] = snakeHead;

        for (short i = 1; i < tail.size(); i++)
        {
            swap(tail[i], beforePos);
        }
    }
}

void GetFreeSpeceFromMap(unsigned char* map, const Map MAPCOORDINATES, unsigned char* ptrMap,
    vector<Map>& freeSpace)
{
    ptrMap = map;
    for (unsigned char y = 0; y <= MAPCOORDINATES.y; y++)
    {
        for (unsigned char x = 0; x <= MAPCOORDINATES.x; x++)
        {
            if (*(ptrMap) == ' ')
            {
                freeSpace.push_back(Map{ x, y });
            }
            *(ptrMap++);
        }
        *(ptrMap++);
    }
}

void generateFruitPosition(Fruit& fruit, const vector<Map>& freeSpace,
    mt19937& mt)
{
    uniform_int_distribution<int> freePointForFruit(0, freeSpace.size() - 1);

    unsigned char freeIndex = freePointForFruit(mt);
    fruit = freeSpace[freeIndex];
}

void wasEaten(Snake snakeHead, Fruit& fruit,
    const vector<Map>& freeSpace, mt19937& mt, vector<Snake>& tail)
{
    if (snakeHead == fruit)
    {
        tail.push_back(Snake{0,0});
        generateFruitPosition(fruit, freeSpace, mt);
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

    vector<Snake> tail;
    tail.reserve(25);

    random_device rd;
    mt19937 mt(rd());
    uniform_int_distribution<int> friutX(0, MAPCOORDINATES.x - 1);
    uniform_int_distribution<int> friutY(0, MAPCOORDINATES.y - 1);
    fruit.x = friutX(mt);
    fruit.y = friutY(mt);

    snakeHead.x = MAPCOORDINATES.x/2;
    snakeHead.y = MAPCOORDINATES.y/2;

    char dir = eDirection::STOP;
 
    vector<Map> freeSpace;
    freeSpace.reserve(MAPCOORDINATES.x * MAPCOORDINATES.y);

    while (true)
    {
        buttonPressed(dir);
        moveTail(tail, snakeHead);
        changeDirection(dir, snakeHead);
        cout << "Score: "<< tail.size() << endl;
        if (isLoose(snakeHead, MAPCOORDINATES)) break;
        Draw(ptrMap, MAPCOORDINATES, snakeHead, fruit, FPS, map, tail);
        GetFreeSpeceFromMap(map, MAPCOORDINATES, ptrMap, freeSpace);
        wasEaten(snakeHead, fruit, freeSpace, mt, tail);
        freeSpace.clear();
    }

    delete[] map;
#ifdef __APPLE__
    resetTermios();
#endif
    return 0;
}
