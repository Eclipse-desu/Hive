#include "Hive.h"
#include <queue>
#include <cmath>
#include <cstdio>
#include <chrono>
#include <random>
#include <cstring>

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

PIMAGE Icon[7] = {0};

void loadIcon(PIMAGE& pimg, const char* _filename) {
    float rate = 0.9;
    float radius = Base * rate * std::sqrt(3);
    int w, h, c;
    unsigned char *tmp = stbi_load(_filename, &w, &h, &c, 0);
    unsigned char *out = new unsigned char[(int)radius * (int)radius * c];
    stbir_resize_uint8_linear(tmp, w, h, 0, out, (int)radius, (int)radius, 0, STBIR_RGBA);
    pimg = newimage((int)radius, (int)radius);
    unsigned int *buf = getbuffer(pimg);
    for (int i = 0; i < getwidth(pimg) * getheight(pimg); i++)
    {
        buf[i] = EGERGBA(out[4 * i], out[4 * i + 1], out[4 * i + 2], out[4 * i + 3]);
    }
    delete[] tmp;
    delete[] out;
}

Game::Game(int _width, int _height)
{
    // std::printf("Game\n");
    width = _width;
    height = _height;
}

void Game::init()
{
    for (int color = 0; color <= 1; color++) {
        for (int name = 0; name < gameSize; name++) {
            int num = Rule[name];
            while(num--) {
                goke[color].push_back(Ishi(color, Type(name), -1, -1));
            }
        }
    }
    picking = nullptr;
    loadIcon(Icon[蜂后], "./img/sparkle.png");
    loadIcon(Icon[甲虫], "./img/SAM.png");
    loadIcon(Icon[蚱蜢], "./img/topaz.png");
    loadIcon(Icon[蜘蛛], "./img/sampo.png");
    loadIcon(Icon[蚂蚁], "./img/firefly.png");

    // 测试模块
    move(&goke[0][0], 0, 0);
    move(&goke[1][4], 0, 1);
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
    int barycenterx = 0, barycentery = 0;
    if (picking != nullptr) {
        picking->render(mouseStat.x, mouseStat.y);
        if (picking->getPosition().first != -1) {
            barycenterx += picking->getPosition().first;
            barycentery += picking->getPosition().second;
        }
    }
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
        barycenterx /= (renderQueue.size() + ((picking != nullptr && picking->getPosition().first != -1) ? 1 : 0));
        barycentery /= (renderQueue.size() + ((picking != nullptr && picking->getPosition().first != -1) ? 1 : 0));
        for (std::vector<Ishi*>::iterator iter = renderQueue.begin(); iter != renderQueue.end(); iter++) {
            int rx = (*iter)->getPosition().first - barycenterx;
            int ry = (*iter)->getPosition().second - barycentery;
            rx = rx + ((rx > gridSize / 2) ? (-gridSize) : ((rx < -gridSize / 2) ? gridSize : 0));
            ry = ry + ((ry > gridSize / 2) ? (-gridSize) : ((ry < -gridSize / 2) ? gridSize : 0));
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
                if (&goke[0][pos] != picking) {
                    goke[0][pos].render(水平到边界距离 + 复叠距离 * cnt, 竖直距离 * (name + 1));
                }
                cnt++;
            }
        }
    }
    for (int name = 0, pos = 0; name < gameSize; name++) {
        int num = Rule[name];
        int cnt = 0;
        for (int i = 0; i < num; i++, pos++) {
            if (goke[1][pos].getPosition().first == -1) {
                if (&goke[1][pos] != picking) {
                    goke[1][pos].render(width - 水平到边界距离 - 复叠距离 * cnt, 竖直距离 * (name + 1));
                }
                cnt++;
            }
        }
    }
    for (int i = 0; i < possibleDest.size(); i++) {
        int rx = possibleDest[i].first - barycenterx;
        int ry = possibleDest[i].second - barycentery;
        rx = rx + ((rx > 15) ? (-30) : ((rx < -15) ? 30 : 0));
        ry = ry + ((ry > 15) ? (-30) : ((ry < -15) ? 30 : 0));
        float cx = calcx(rx, ry), cy = calcy(rx, ry);
        printf("rendering %d %d\n", rx, ry);
        setfillcolor(EGEARGB(0xFF, 0x82, 0xff, 0xfc));
        ege_fillellipse(cx + width / 2, cy + height / 2, 10, 10);
    }
}

void Game::mouseEvent()
{
    if (mouseStat.is_down() && mouseStat.is_left()) {
        // 如果没有选中棋子, 那么按下左键应该选中棋子.
        // 点击棋子后, 先计算其可移动点, 如果没有可移动点, 则选中失败.
        if (picking == nullptr) {
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
                    return ;
                }
                if (picking->x() != -1) {
                    Ishis[picking->x()][picking->y()].pop();
                }
            }
        } else {
            // 如果选中了棋子, 那么这次点击必须是在可行位置, 即 PossibleDest 中.
        }
    } else if (mouseStat.is_down() && mouseStat.is_right()) {
        if (picking != nullptr) {
            Ishis[picking->getPosition().first][picking->getPosition().second].push(picking);
            picking = nullptr;
            possibleDest.clear();
        }
    }
}

void Game::move(Ishi *_ishi, int _nx, int _ny)
{
    if (_ishi->x() != -1) {
        Ishis[_ishi->x()][_ishi->y()].pop();
    }
    _ishi->setPosition(_nx, _ny);
    Ishis[_nx][_ny].push(_ishi);
}

int Game::checkWin() const
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

int Game::checkConnect() const {
    std::queue<const Ishi*> BfsQueue;
    int emptyBoard = true;
    for (int color = 0; color < 2 && emptyBoard; color++) {
        for (std::vector<Ishi>::const_iterator iter = goke[color].cbegin(); iter != goke[color].cend(); iter++) {
            if (iter->x() != -1) {
                BfsQueue.push(&*iter);
                emptyBoard = false;
                break;
            }
        }
    }
    if (emptyBoard) {
        return true;
    }
    char vis[gridSize][gridSize];
    memset(vis, 0, sizeof(vis));
    while (!BfsQueue.empty()) {
        const Ishi* top = BfsQueue.front();
        BfsQueue.pop();
        for (int i = 0; i < 6; i ++) {
            int nx = nbrx(top->x(), i, gridSize);
            int ny = nbrx(top->y(), i, gridSize);
            if (!vis[nx][ny] && !Ishis[nx][ny].empty()) {
                BfsQueue.push(Ishis[nx][ny].top());
                vis[nx][ny] = 1;
            }
        }
    }
    for (int color = 0; color < 2; color++) {
        for (std::vector<Ishi>::const_iterator iter = goke[color].cbegin(); iter != goke[color].cend(); iter++) {
            if (iter->x() != -1 && vis[iter->x()][iter->y()] == 0) {
                return false;
            }
        }
    }
    return true;
}

int Game::getPossibleDest(Ishi *_ishi)
{
    // printf("getting\n");
    possibleDest.clear();
    if (_ishi->getPosition().first == -1) {
        // 棋子从棋篓中拿出来, 那么应该放在同色棋子边上.
        int okColor[30][30];
        memset(okColor, -1, sizeof(okColor));
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                if (!Ishis[i][j].empty()) {
                    Ishi* curIshi = Ishis[i][j].top();
                    for (int k = 0; k < 6; k++) {
                        int nx = nbrx(i, k, gridSize);
                        int ny = nbry(j, k, gridSize);
                        int& oc = okColor[nx][ny];
                        if (oc == -2)
                            continue;
                        if (oc == -1) {
                            oc = curIshi->getColor();
                            continue;
                        }
                        if (oc != curIshi->getColor()) {
                            oc = -2;
                        }
                    }
                }
            }
        }
        // printf("test1\n");
        for (int i = 0; i < gridSize; i++) {
            for (int j = 0; j < gridSize; j++) {
                if (Ishis[i][j].empty() && okColor[i][j] == _ishi->getColor()) {
                    possibleDest.push_back(std::make_pair(i, j));
                }
            }
        }
        if (possibleDest.empty()) {
            // 第一颗棋子放下时, 棋盘是空的, 所以所有格子都不可行.
            // 此时手动将 0, 0 放入.
            possibleDest.push_back(std::make_pair(0, 0));
        }
        // printf("test2\n");
    } else {
        // 已经在棋盘上的棋子, 则考虑如何移动.
        // 首先, 如果棋子移动后会断开连接, 那么就不能移动.
        // 根据规则, 移动期间蜂巢也不能分离, 所以首先直接去掉这个棋子, 来看是否可以移动.
        int x = _ishi->x(), y = _ishi->y();
        Ishis[x][y].pop();          // 先把棋子拿出去.
        if (!checkConnect()) {      // 如果不连通, 直接返回空 vector.
            Ishis[x][y].push(_ishi);
            return 0;
        }
        Ishis[x][y].push(_ishi);    // 当然, 还要记得放回来.
        // 如果去掉后不会断开, 那么就可以计算移动方式.
        if (_ishi->getType() == 蜂后) {
            int isOccupied[6];
            memset(isOccupied, 0, sizeof(isOccupied));
            for (int i = 0; i < 6; i ++) {
                int nx = nbrx(x, i, gridSize);
                int ny = nbrx(y, i, gridSize);
                isOccupied[i] = Ishis[nx][ny].empty();
            }
            // 上面统计了周围 6 格的棋子分布.
            // 如果一个空位的周围两个格子都被棋子控制了, 那么就不能走到这个空位.
            for (int i = 0; i < 6; i++) {
                if (isOccupied[i])
                    continue;
                int prev = (i - 1 + 6) % 6, post = (i + 1) % 6;
                if (isOccupied[prev] && isOccupied[post]) {
                    continue;
                }
                int nx = nbrx(x, i, gridSize);
                int ny = nbrx(y, i, gridSize);
                // 为了避免移动到不连通区域, 还需要看看目标点附近有没有棋子.
                int island = true;
                for (int j = 0; j < 6; j++) {
                    int nnx = nbrx(nx, j, gridSize);
                    int nny = nbry(ny, j, gridSize);
                    if (!Ishis[nnx][nny].empty()) {
                        island = false;
                        break;
                    }
                }
                if (!island)
                    possibleDest.push_back(std::make_pair(nx, ny));
            }
        } else if (_ishi->getType() == 甲虫) {
            // 只要移动不断开, 甲虫就可以移动, 因此不需要计算 isOccupied 这一步. 因此直接所有格子都可以走
            for (int i = 0; i < 6; i++) {
                int nx = nbrx(x, i, gridSize);
                int ny = nbrx(y, i, gridSize);
                int island = true;
                for (int j = 0; j < 6; j++) {
                    int nnx = nbrx(nx, j, gridSize);
                    int nny = nbry(ny, j, gridSize);
                    if (!Ishis[nnx][nny].empty()) {
                        island = false;
                        break;
                    }
                }
                // 对于甲虫, 如果爬到其他棋子上, 也可能出现周围没有棋子的情况. 但是这种情况是可行的.
                if (!island || !Ishis[nx][ny].empty())
                    possibleDest.push_back(std::make_pair(nx, ny));
                possibleDest.push_back(std::make_pair(nx, ny));
            }
        } else if (_ishi->getType() == 蚱蜢) {
            // 蚱蜢是跳跃式走法.
        }
    }
    return possibleDest.size();
}

Game::Ishi::Ishi(int _color, Type _type, int _px, int _py)
    : color(_color), type(_type), posx(_px), posy(_py) {
    }

Game::Type Game::Ishi::getType() const
{
    return type;
}

int Game::Ishi::getColor() const
{
    return color;
}

int Game::Ishi::x() const
{
    return posx;
}

int Game::Ishi::y() const
{
    return posy;
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
    float radius = Base * rate * std::sqrt(3);
    putimage_withalpha(NULL, Icon[type], (int)x - (int)radius / 2, (int)y - (int)radius / 2);
}

bool Game::Ishi::inside(int x, int y) const
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
