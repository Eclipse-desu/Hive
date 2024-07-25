#include "Hive.h"
#include <queue>
#include <cmath>
#include <cstdio>
#include <chrono>
#include <random>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "stb/stb_image_resize2.h"

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

ege_point operator+ (ege_point& p1, ege_point& p2) {
    return (ege_point){p1.x + p2.x, p1.y + p2.y};
}

ege_point operator- (ege_point& p1, ege_point& p2) {
    return (ege_point){p1.x - p2.x, p1.y - p2.y};
}

float Cross(ege_point p1, ege_point p2) {
    return p1.x * p2.y - p1.y * p2.x;
}

void loadIcon(char* _filename, float x, float y) {
    float rate = 0.9;
    float radius = Base * rate * std::sqrt(3);
    int w, h, c;
    unsigned char *tmp = stbi_load(_filename, &w, &h, &c, 0);
    unsigned char *out = new unsigned char[(int)radius * (int)radius * c];
    stbir_resize_uint8_linear(tmp, w, h, 0, out, (int)radius, (int)radius, 0, STBIR_RGBA);
    PIMAGE pimg = newimage((int)radius, (int)radius);
    unsigned int *buf = getbuffer(pimg);
    for (int i = 0; i < getwidth(pimg) * getheight(pimg); i++)
    {
        buf[i] = EGERGBA(out[4 * i], out[4 * i + 1], out[4 * i + 2], out[4 * i + 3]);
    }
    delete[] tmp;
    delete[] out;

    putimage_withalpha(NULL, pimg, (int)x - (int)radius / 2, (int)y - (int)radius / 2);
    delimage(pimg);
}

Game::Game(int _width, int _height)
{
    // std::printf("Game\n");
    width = _width;
    height = _height;
    for (int color = 0; color <= 1; color++) {
        for (int name = 0; name < gameSize; name++) {
            int num = Rule[name];
            while(num--) {
                goke[color].push_back(Ishi(color, Type(name), -1, -1));
            }
        }
    }
    picking = nullptr;

    // 测试模块
    move(&goke[0][0], 0, 0);
    move(&goke[1][4], 0, 1);
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
    initgraph(width, height);
    setrendermode(RENDER_MANUAL);
    ege_enable_aa(true);
    display();
    for ( ; is_run() ; delay_fps(60)) {
        while (mousemsg())
        {
            mouseStat = getmouse();
            mouseEvent();
        }
        cleardevice();
        display();
    }
    closegraph();
}

void Game::display()
{
    // printf("display\n");
    if (picking != nullptr) {
        picking->render(mouseStat.x, mouseStat.y);
    }
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
    int 复叠距离 = Base / 2;

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
    if (mouseStat.is_down() && mouseStat.is_left()) {
        // 如果没有选中棋子, 那么按下左键应该选中棋子. 
        // 点击棋子后, 先计算其可移动点, 如果没有可移动点, 则选中失败.
        printf("clicked\n");
        if (picking == nullptr) {
            printf("Start searching.\n");
            for (int color = 0; color < 2; color++) {
                for (int name = 0, pos = 0; name < gameSize; name++) {
                    int num = Rule[name];
                    for (int i = num - 1; i >= 0; i--) {
                        if (goke[color][pos + i].inside(mouseStat.x, mouseStat.y)) {
                            picking = &goke[color][pos + i];
                            goto output;
                        }
                    }
                    pos += num;
                }
            }
        output:
            if (picking != nullptr) {
                int res = getPossibleDest(picking);
                if (res == 0) {
                    picking = nullptr;
                }
            }
        }
    }
}

void Game::move(Ishi *_ishi, int _nx, int _ny)
{
    _ishi->setPosition(_nx, _ny);
    Ishis[_nx][_ny].push(_ishi);
}

int Game::getPossibleDest(Ishi *_ishi)
{
    if (_ishi->getType() == 蜂后) {
        int x = _ishi->getPosition().first, y = _ishi->getPosition().second;
        for (int i = 0; i < 6; i ++) {
            int nx = nbrx(x, i, gridSize);
        }
    }
    return 1;
}

Game::Ishi::Ishi(int _color, Type _type, int _px, int _py)
    : color(_color), type(_type), posx(_px), posy(_py) {
    }

Game::Type Game::Ishi::getType() const
{
    return type;
}

std::pair<int, int> Game::Ishi::getPosition() const
{
    return std::make_pair(posx, posy);
}

void Game::Ishi::setPosition(int _nx, int _ny)
{
    posx = _nx; posy = _ny;
}

std::pair<float, float> Game::Ishi::getDispCenter() const
{
    return std::make_pair(dispCenterx, dispCentery);
}

void Game::Ishi::setDispCenter(float _nx, float _ny)
{
    dispCenterx = _nx;
    dispCentery = _ny;
}

void Game::Ishi::render(float x, float y)
{
    setDispCenter(x, y);
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
        loadIcon("./img/sparkle.png", x, y);
    }
    if (type == 甲虫) {
        loadIcon("./img/SAM.png", x, y);
    }
    if (type == 蜘蛛) {
        loadIcon("./img/sampo.png", x, y);
    }
}

bool Game::Ishi::inside(int x, int y)
{
    ege_point* polypoints = new ege_point[6];
    for (int i = 0; i < 6; i++) {
        float rx = (dx[i] + dx[(i + 1) % 6]) / 3.0;
        float ry = (dy[i] + dy[(i + 1) % 6]) / 3.0;
        float cx = calcx(rx, ry);
        float cy = calcy(rx, ry);
        polypoints[i].x = cx + dispCenterx;
        polypoints[i].y = cy + dispCentery;
    }
    // 端点是顺时针排布的
    int sign = 8;
    ege_point curPos;
    curPos.x = (float)x;
    curPos.y = (float)y;
    for (int i = 0; i < 6; i++) {
        ege_point p1 = polypoints[(i + 1) % 6] - polypoints[i];
        ege_point p2 = curPos - polypoints[(i + 1) % 6];
        float cross = Cross(p1, p2);
        if (cross == 0) {
            return false;
        }
        if (sign == 8) {
            sign = std::signbit(cross);
        }
        if (sign != std::signbit(cross)) {
            return false;
        }
    }
    return true;
}
