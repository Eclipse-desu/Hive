#ifndef __ROMI_DESU_HIVE_H__
#define __ROMI_DESU_HIVE_H__

#include <stack>
#include <vector>
#include <graphics.h>

// 蜂后, 甲虫, 蚱蜢, 蜘蛛, 蚂蚁
const int gameSize = 5;
const int Rule[gameSize] = {1, 2, 3, 2, 3};
// const color_t Color[gameSize + 2] = {WHITE, MAGENTA, CYAN, LIME, PURPLE, BLUE, BLACK};
const color_t ColorA[gameSize + 2] = {
    EGEARGB(0x80, 0xFF, 0xFF, 0xFF),
    EGEARGB(0x80, 0xFF, 0x00, 0xFF),
    EGEARGB(0x80, 0x00, 0xFF, 0xFF),
    EGEARGB(0x80, 0x00, 0xFF, 0x00),
    EGEARGB(0x80, 0x80, 0x00, 0x80),
    EGEARGB(0x80, 0x00, 0x00, 0xFF),
    EGEARGB(0x80, 0x00, 0x00, 0x00),
};


const int Base = 50;
const int dx[6] = {1, 1, 0, -1, -1, 0};
const int dy[6] = {0, 1, 1, 0, -1, -1};

// inline float calcx(int x, int y);
// inline float calcy(int x, int y);

class Game {
    int width;
    int height;

    mouse_msg mouseStat;

    class Ishi;

    static const int gridSize = 30;
    std::stack<Game::Ishi*> Ishis[gridSize][gridSize];
    enum Type {
        蜂后, 甲虫, 蚱蜢, 蜘蛛, 蚂蚁, 幻影, 空相, 
    };

    std::vector<Ishi> goke[2];
    std::vector<std::pair<int, int> > possibleDest;

    Ishi* picking;
public:
    Game(int _width = 1200, int _height = 900);

    // 检查是否有玩家胜利.
    // 返回: 
    // 0 - 无玩家胜利
    // 1 - 玩家 1 胜利
    // 2 - 玩家 2 胜利
    // 3 - 平局
    int checkWin();
    
    // 游戏主循环
    void mainLoop();

    // 渲染棋局
    void display();

    // 鼠标事件
    void mouseEvent();

    // 移动到给定位置.
    void move(Ishi* _ishi, int _nx, int _ny);

    int getPossibleDest(Ishi* _ishi);
};

class Game::Ishi {
    int color;
    Type type;
    // 如果 posx = posy = -1, 说明棋子没有被投入棋盘.
    int posx, posy;
    float dispCenterx, dispCentery;

public:
    Ishi(int _color, Type _type, int _px, int _py);
    // 查询可行的移动终点.
    // 返回: 存储了可行终点的 vector.

    Type getType() const;
    std::pair<int, int> getPosition() const;
    void setPosition(int _nx, int _ny);
    std::pair<float, float> getDispCenter() const;
    void setDispCenter(float _nx, float _ny);

    // 渲染棋子.
    void render(float x, float y);

    bool inside(int x, int y);
};

#endif