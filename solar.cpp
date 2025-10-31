#include <graphics.h>
#include <math.h>
#include <iostream>
using namespace std;

const int WIDTH = 1080;
const int HEIGHT = 768;
const double PI = 3.14159265358979323846;

// ---------- Text Helper ----------
void drawText(int x, int y, const char* text) {
    outtextxy(x, y, const_cast<char*>(text));
}

// ---------- DDA Line ----------
void ddaLine(int x0, int y0, int x1, int y1, int color) {
    int dx = x1 - x0;
    int dy = y1 - y0;
    int steps = abs(dx) > abs(dy) ? abs(dx) : abs(dy);
    if (steps == 0) {
        putpixel(x0, y0, color);
        return;
    }
    double xInc = dx / (double)steps;
    double yInc = dy / (double)steps;
    double x = x0;
    double y = y0;

    for (int i = 0; i <= steps; i++) {
        putpixel((int)round(x), (int)round(y), color);
        x += xInc;
        y += yInc;
    }
}

// ---------- Midpoint Circle ----------
void midpointCircle(int cx, int cy, int r, int color) {
    int x = 0, y = r;
    int d = 1 - r;

    auto plot8 = [&](int px, int py) {
        putpixel(cx + px, cy + py, color);
        putpixel(cx - px, cy + py, color);
        putpixel(cx + px, cy - py, color);
        putpixel(cx - px, cy - py, color);
        putpixel(cx + py, cy + px, color);
        putpixel(cx - py, cy + px, color);
        putpixel(cx + py, cy - px, color);
        putpixel(cx - py, cy - px, color);
    };

    plot8(x, y);
    while (x < y) {
        x++;
        if (d < 0)
            d += 2 * x + 1;
        else {
            y--;
            d += 2 * (x - y) + 1;
        }
        plot8(x, y);
    }
}

// ---------- Filled Circle ----------
void filledCircle(int cx, int cy, int r, int color) {
    for (int yy = -r; yy <= r; yy++) {
        int dx = (int)floor(sqrt((double)max(0, r * r - yy * yy)));
        for (int xx = -dx; xx <= dx; xx++) {
            putpixel(cx + xx, cy + yy, color);
        }
    }
}

// ---------- Orbit using DDA ----------
void drawOrbit(int cx, int cy, int r, int color) {
    const int segments = 180;
    double step = 2 * PI / segments;
    int prevX = cx + (int)round(r * cos(0));
    int prevY = cy + (int)round(r * sin(0));

    for (int i = 1; i <= segments; i++) {
        double theta = i * step;
        int x = cx + (int)round(r * cos(theta));
        int y = cy + (int)round(r * sin(theta));
        ddaLine(prevX, prevY, x, y, color);
        prevX = x;
        prevY = y;
    }
}

int main() {
    initwindow(WIDTH, HEIGHT, "Flicker-Free Rotating Solar System");

    int cx = getmaxx() / 2;
    int cy = getmaxy() / 2;

    const int PLANETS = 7;
    int orbitRadius[PLANETS] = {100, 150, 200, 260, 330, 390, 460};
    int planetSize[PLANETS] = {6, 8, 9, 11, 12, 9, 8};
    int planetColor[PLANETS] = {BLUE, GREEN, LIGHTBLUE, RED, MAGENTA, CYAN, LIGHTGREEN};
    double orbitSpeed[PLANETS] = {1.8, 1.5, 1.2, 1.0, 0.8, 0.6, 0.5};

    double angle[PLANETS] = {0};

    // -------- Enable Double Buffering --------
    int activePage = 0;
    int visualPage = 1;

    while (true) {
        setactivepage(activePage);
        setvisualpage(visualPage);
        cleardevice();
        setbkcolor(BLACK);
        setcolor(WHITE);
        drawText(10, 10, "Flicker-Free Rotating Solar System (DDA + Midpoint Circle)");

        // Draw Orbits
        for (int i = 0; i < PLANETS; i++) {
            drawOrbit(cx, cy, orbitRadius[i], DARKGRAY);
        }

        // Draw Sun
        filledCircle(cx, cy, 35, YELLOW);
        midpointCircle(cx, cy, 35, YELLOW);

        // Draw Planets
        for (int i = 0; i < PLANETS; i++) {
            double theta = angle[i] * PI / 180.0;

            int px = cx + (int)(orbitRadius[i] * cos(theta));
            int py = cy + (int)(orbitRadius[i] * sin(theta) * 0.9); // small 3D effect

            filledCircle(px, py, planetSize[i], planetColor[i]);
            midpointCircle(px, py, planetSize[i], BLACK);

            angle[i] += orbitSpeed[i];
            if (angle[i] > 360.0) angle[i] -= 360.0;
        }

        delay(30); // smooth speed

        // Swap pages
        swap(activePage, visualPage);

        if (kbhit()) {
            int ch = getch();
            if (ch == 27) break; // ESC to exit
        }
    }

    closegraph();
    return 0;
}
