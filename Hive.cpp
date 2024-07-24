#include "Hive.h"
#include <queue>
#include <cmath>
#include <cstdio>

// 计算向 dir 方向移动一格之后的 x. nbrx = neighbor x
inline int nbrx(int x, int dir, int gridSize) {
    return (x + dx[dir] + gridSize) % gridSize;
}

// 计算向 dir 方向移动一格之后的 y. nbry = neighbor y
inline int nbry(int y, int dir, int gridSize) {
    return (y + dy[dir] + gridSize) % gridSize;
}

// x => (\sqrt{3}, -1), y => (0, 2), z => (-\sqrt{3}, -1)
template <typename type>
inline float calcx(type x, type y) {
    return Base * (x * std::sqrt(3) + y * (0));
}

template <typename type>
inline float calcy(type x, type y) {
    return Base * (x * (-1) + y * (2));
}

inline float calcx(std::pair<int, int> coord) {
    int x = coord.first, y = coord.second;
    return Base * (x * std::sqrt(3) + y * (2));
}

inline float calcy(std::pair<int, int> coord) {
    int x = coord.first, y = coord.second;
    return Base * (x * (-1) + y * (0));
}

void zoomImage(PIMAGE& pimg, int zoomWidth, int zoomHeight) {
    if (pimg == NULL) return;
    PIMAGE zoomImg = newimage(zoomWidth, zoomHeight);
    putimage_withalpha(zoomImg, pimg, 0, 0, zoomWidth, zoomHeight, 0, 0, getwidth(pimg), getheight(pimg));
    delimage(pimg);
    pimg = zoomImg;
}

Game::Game(int _width, int _height)
{
    // std::printf("Game\n");
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
        if (Ishis[nbrx(cx, i, gridSize)][nbry(cy, i, gridSize)].empty()) {
            res = res - 1;
            break;
        }
    }
    cx = goke[1][0].getPosition().first;
    cy = goke[1][0].getPosition().second;
    for (int i = 0; i < 6; i++) {
        if (Ishis[nbrx(cx, i, gridSize)][nbry(cy, i, gridSize)].empty()) {
            res = res - 2;
            break;
        }
    }
    return res;
}

void Game::mainLoop()
{
    // std::printf("mainloop\n");
    initgraph(width, height);
    setrendermode(RENDER_MANUAL);
    ege_enable_aa(true);
    display();
    for ( ; is_run() ; delay_fps(60)) {
        // while(mousemsg()) {
            // mouseStat = getmouse();
            // mouseEvent();
        // }
        // printf("in loop\n");
        cleardevice();
        char str[100];
        int x, y;
        mousepos(&x, &y);
        sprintf(str, "x = %4d, y = %4d", x, y);
        setcolor(WHITE);
        outtextxy(0, 0, str);
        display();
    }
    closegraph();
}

void Game::display()
{
    // printf("display\n");
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
        // printf("rendering on board %d %d\n", barycenterx, barycentery);
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
    int 水平到边界距离 = (width - height) / 4;
    int 竖直距离 = height / (gameSize + 1);
    int 复叠距离 = Base;

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
    // if (type == 蜂后) {
    //     PIMAGE pimg = newimage();
    //     getimage(pimg, "./img/furina-resized.png");
    //     putimage(x - 25, y - 25, pimg);
    //     delimage(pimg);
    //     return ;
    // }
    float rate = 0.9;
    ege_point* polypoints = new ege_point[6];
    ege_point* polypoints_inner = new ege_point[6];
    for (int i = 0; i < 6; i++) {
        float rx = (dx[i] + dx[(i + 1) % 6]) / 3.0;
        float ry = (dy[i] + dy[(i + 1) % 6]) / 3.0;
        float cx = calcx(rx, ry);
        float cy = calcy(rx, ry);
        polypoints[i].x = cx + x;
        polypoints[i].y = cy + y;
        polypoints_inner[i].x = rate * cx + x;
        polypoints_inner[i].y = rate * cy + y;
    }
    setfillcolor(ColorA[type]);
    ege_fillpoly(6, polypoints);
    setfillcolor(alphablend(ColorA[type], ColorA[0], rate));
    ege_fillpoly(6, polypoints_inner);
    delete[] polypoints;
    delete[] polypoints_inner;
    if (type == 蜂后) {
        float radius = Base * rate * std::sqrt(3) / 2;
        PIMAGE pimg = newimage();
        getimage(pimg, "./img/sparkle.png");
        // zoomImage(pimg, (int)(radius * 2), (int)(radius * 2));
        // imagefilter_blurring(pimg, 0x50, 0x100);
        // putimage((int)(x - 25), (int)(y - 25), pimg);
        putimage_rotatezoom(NULL, pimg, (int)x, (int)y, 0.5, 0.5, 0, radius / 300, true, -1, true);
        delimage(pimg);
    }
}
