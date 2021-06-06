#include <iostream>
#include <stdlib.h>
#include "../general_timer_util.hpp"

class Point {
    public:
        const float x;
        const float y;

        Point(float cx, float cy) : x(cx), y(cy) {

        }

        Point operator-(const Point& a) {
            return Point(x - a.x, y - a.y);
        }

        Point operator+(const Point& a) {
            return Point(x + a.x, y + a.y);
        }

        Point operator*(float a) {
            return Point(x * a, y * a);
        }
};

class Screen {
    public:
        char buf[100][20];
        char scr[100][20];
        bool firstdraw;

        Screen() {
            firstdraw = true;
            for (int i = 0; i < 100; i++)
                for (int j = 0; j < 20; j++)
                    buf[i][j] = scr[i][j] = ' ';
        }

        void draw() {
            if (firstdraw) {
                for (int j = 0; j < 20; j++) {
                    for (int i = 0; i < 100; i++) {
                        buf[i][j] = scr[i][j];
                        std::cout << scr[i][j];
                    }
                    std::cout << "\n";
                }
                firstdraw = false;
            }
            else {
                printf("\33\[1;1H");
                for (int j = 0; j < 20; j++) {
                    for (int i = 0; i < 100; i++) {
                        if (buf[i][j] != scr[i][j]) {
                            printf("\33\[%d;%dH", j + 1, i + 1);
                            buf[i][j] = scr[i][j];
                            std::cout << scr[i][j];
                        }
                    }
                    std::cout << "\n";
                }
            }
        }

        void set(Point p, char c) {
            scr[(int) p.x][(int) p.y] = c;
        }
};

Point qbezier_eval(Point a, Point b, Point cp, float percent) {
    // A -> cp, cp -> B
    Point v1 = (cp - a)*percent + a;
    Point v2 = (b - cp)*percent + cp;

    // Mid
    Point vmp = (v2 - v1)*percent + v1;

    return vmp;
}

int main() {
    // Bezier
    Point e1 = Point(10, 2);
    Point e2 = Point(60, 2);

    Point c1 = Point(20, 10);

    // Screen
    Screen s;

    s.set(e1, 'o');
    s.set(e2, 'o');
    s.set(c1, 'x');

    system("clear");
    for (int tick = 1; tick <= 100; tick++) {
        s.set(qbezier_eval(e1, e2, c1, (float) tick/100), '*');
        s.draw();
        std::cout << "Tick " << tick << std::endl;
        UtilityTimer::sleep_thread_millisecond(50);
    }

    return 0;
}
