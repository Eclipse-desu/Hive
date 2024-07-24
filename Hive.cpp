#include "Hive.h"
#include <queue>
#include <cmath>
#include <cstdio>

// 计算向 dir 方向移动一格之后的 x. nbrx = neighbor x
inline int Game::nbrx(int x, int dir) {
    return (x + dx[dir] + gridSize) % gridSize;
}

// 计算向 dir 方向移动一格之后的 y. nbry = neighbor y
inline int Game::nbry(int y, int dir) {
    return (y + dy[dir] + gridSize) % gridSize;
}

// x => (\sqrt{3}, -1), y => (2, 0), z => (-\sqrt{3}, -1)
inline float Game::calcx(int x, int y) {
    return Base * (x * std::sqrt(3) + y * (2) + (-x - y) * (-std::sqrt(3)));
}

inline float Game::calcy(int x, int y) {
    return Base * (x * (-1) + y * (0) + (-x - y) * (-1));
}

inline float Game::calcx(std::pair<int, int> coord) {
    int x = coord.first, y = coord.second;
    return Base * (x * std::sqrt(3) + y * (2) + (-x - y) * (-std::sqrt(3)));
}

inline float Game::calcy(std::pair<int, int> coord) {
    int x = coord.first, y = coord.second;
    return Base * (x * (-1) + y * (0) + (-x - y) * (-1));
}

Game::Game(int _width, int _height)
{
    std::printf("Game\n");
    width = _width;
    height = _height;
    for (int color = 0; color <= 1; color++) {
        for (int name = 0; name < gameSize; name++) {
            int num = Rule[name];
            while (num--) {
                goke[color].push_back(Ishi(color, Type(name), -1, -1));
            }
        }
    }

    // 测试模块
    move(&goke[0][0], 0, 0);
    move(&goke[1][2], 0, 1);
}

int Game::checkWin()
{
    int res = 3, cx, cy;
    cx = goke[0][0].getPosition().first;
    cy = goke[0][0].getPosition().second;
    for (int i = 0; i < 6; i++) {
        if (Ishis[nbrx(cx, i)][nbry(cy, i)].empty()) {
            res = res - 1;
            break;
        }
    }
    cx = goke[1][0].getPosition().first;
    cy = goke[1][0].getPosition().second;
    for (int i = 0; i < 6; i++) {
        if (Ishis[nbrx(cx, i)][nbry(cy, i)].empty()) {
            res = res - 2;
            break;
        }
    }
    return res;
}

void Game::mainLoop()
{
    std::printf("mainloop\n");
    initgraph(width, height);
    setrendermode(RENDER_MANUAL);
    display();
    for ( ; is_run() ; delay_fps(60)) {
        // while(mousemsg()) {
            // mouseStat = getmouse();
            // mouseEvent();
        // }
        printf("in loop\n");
        char str[100];
        int x, y;
        mousepos(&x, &y);
        sprintf(str, "%4d %4d", x, y);
        outtextxy(0, 0, str);
        cleardevice();
        display();
    }
    closegraph();
}

void Game::display()
{
    printf("display\n");
    int barycenterx = 0, barycentery = 0;
    std::vector<Ishi*> renderQueue;
    for (int i = 0; i < 30; i++) {
        for (int j = 0; j < 30; j++) {
            if (!Ishis[i][j].empty()) {
                renderQueue.push_back(Ishis[i][j].top());
                barycenterx += Ishis[i][j].top()->getPosition().first;
                barycentery += Ishis[i][j].top()->getPosition().second;
            }
        }
    }
    if (renderQueue.size() != 0) {
        printf("rendering on board %d %d\n", barycenterx, barycentery);
        barycenterx /= renderQueue.size();
        barycentery /= renderQueue.size();
        for (std::vector<Ishi*>::iterator iter = renderQueue.begin(); iter != renderQueue.end(); iter++) {
            int rx = (*iter)->getPosition().first - barycenterx;
            int ry = (*iter)->getPosition().second - barycentery;
            float cx = calcx(rx, ry), cy = calcy(rx, ry);
            (*iter)->render(cx + width / 2, cy + height / 2);
        }
    }
    // printf("test2\n");
    // 渲染还没有被放到棋盘上的棋子
    int 水平到边界距离 = (width - height) / 2;
    int 竖直距离 = height / (gameSize + 1);
    int 复叠距离 = 25;

    for (int name = 0, pos = 0; name < gameSize; name++) {
        int num = Rule[name];
        int cnt = 0;
        for (int i = 0; i < num; i++, pos++) {
            if (goke[0][pos].getPosition().first == -1) {
                goke[0][pos].render(水平到边界距离 + 复叠距离 * cnt, 竖直距离 * (name + 1));
                cnt++;
            }
        }
    }
    for (int name = 0, pos = 0; name < gameSize; name++) {
        int num = Rule[name];
        int cnt = 0;
        for (int i = 0; i < num; i++, pos++) {
            if (goke[1][pos].getPosition().first == -1) {
                goke[1][pos].render(width - 水平到边界距离 - 复叠距离 * cnt, 竖直距离 * (name + 1));
                cnt++;
            }
        }
    }
}

void Game::mouseEvent()
{
}

void Game::move(Ishi *_ishi, int _nx, int _ny)
{
    _ishi->setPosition(_nx, _ny);
    Ishis[_nx][_ny].push(_ishi);
}

Game::Ishi::Ishi(int _color, Type _type, int _px, int _py)
    : color(_color), type(_type), posx(_px), posy(_py) {}

std::vector<std::pair<int, int> >& Game::Ishi::getPossibleMoves() const
{
    std::vector<std::pair<int, int> > temp;
    return temp;
}

std::pair<int, int> Game::Ishi::getPosition() const
{
    return std::make_pair(posx, posy);
}

void Game::Ishi::setPosition(int _nx, int _ny)
{
    posx = _nx; posy = _ny;
}

void Game::Ishi::render(float x, float y)
{
    setfillcolor(Color[type]);
    fillcircle(x, y, 25);
}
