#include <cmath>
#include <cstdio>
#include <cstring>
#include <graphics.h>

mouse_msg msg;

void display();
void process();

class Qi {
    float cx, cy;
    float radius;
    bool linked;
    color_t color;

public:
    Qi(float x, float y, float r, color_t color) 
        : cx(x), cy(y), radius(r), color(color), linked(false) {
    }
    
    void render();
    
    int inside(float x, float y);
    
    bool linking() { return linked; }
    
    void setLink(bool stat) { linked = stat; }

    void setCenter(float _x, float _y) {
        cx = _x;
        cy = _y;
    }
    
    void lock() {
        cx = msg.x;
        cy = msg.y;
    }

    float getcx() { return cx; }
    float getcy() { return cy; }
} coin(100, 100, 50, ege::COLORS::RED), house(100, 200, 50, ege::COLORS::BLUE);

Qi coins[2] = {coin, house};

int main() {
    initgraph(640, 480);
    setrendermode(RENDER_MANUAL);
    
    setfont(18, 0, "Consolas");

    display();
    for ( ; is_run() ; delay_fps(60)) {
        while (mousemsg()) {
            msg = getmouse();
            process();
        }
        cleardevice();
        display();
        char buf[200];
        sprintf(buf, "x = %d, y = %d, d = %s, l = %s, r = %s, u = %s, \ni: %s, lk: %s", msg.x, msg.y, msg.is_down() ? "Y" : "N", msg.is_left() ? "Y" : "N", msg.is_right() ? "Y" : "N", msg.is_up() ? "Y" : "N", coin.inside(msg.x, msg.y) ? "Y" : "N", coin.linking() ? "Y" : "N");
        setcolor(EGERGB(0x255, 0x255, 0x255));
        outtextxy(0, 0, buf);
        delay_ms(0);
    }

    closegraph();
    return 0;
}

void display() {
    // setbkcolor(EGERGB(0x0, 0x40, 0x0));
    for (int i = 0; i < 2; i++) {
        coins[i].render();
    }
}

void process() {
    for (int i = 0; i < 2; i++) {
        if (msg.is_down() && msg.is_left() && coins[i].inside(msg.x, msg.y)) {
            if (coins[i].linking() == false) {
                coins[i].setLink(true);
            } else {
                coins[i].setLink(false);
                coins[i].lock();
            }
        } else if (msg.is_down() && msg.is_right()) {
            if (coins[i].linking()) 
                coins[i].setLink(false);
        }
    }
}

void Qi::render() {
    float x = linked ? msg.x : cx;
    float y = linked ? msg.y : cy; 
    setcolor(color);
    setfillcolor(color);
    fillellipsef(x, y, radius, radius);
    setcolor(ege::COLORS::WHITE);
    setfillcolor(ege::COLORS::WHITE);
}

int Qi::inside(float _x, float _y) {
    float x = linked ? msg.x : cx;
    float y = linked ? msg.y : cy; 
    float dx = std::fabs(_x - x);
    float dy = std::fabs(_y - y);
    return dx * dx + dy * dy <= radius * radius ? 1 : 0;
}