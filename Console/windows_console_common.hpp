// Commonly used function
#ifndef WIN_CONSOLE_HEADER_HPP
#define WIN_CONSOLE_HEADER_HPP

// Common header
#include <chrono>
#include <thread>
#include <Windows.h>
#include <iostream>
#include <vector>
#include "general_timer_util.hpp"

// Color macro
#define COLOR_DARK_BLUE 0x1
#define COLOR_DARK_GREEN 0x2
#define COLOR_DARK_CYAN 0x3
#define COLOR_DARK_RED 0x4
#define COLOR_DARK_MAGENTA 0x5
#define COLOR_DARK_YELLOW 0x6
#define COLOR_LIGHT_GRAY 0x7
#define COLOR_DARK_GRAY 0x8
#define COLOR_BLUE 0x9
#define COLOR_GREEN 0xA
#define COLOR_CYAN 0xB
#define COLOR_RED 0xC
#define COLOR_MAGENTA 0xD
#define COLOR_YELLOW 0xE
#define COLOR_WHITE 0xF

// Somewhat generalized Render class
class Render {
private:
    std::vector<std::vector<char>> frame_buffer;
    std::vector<std::vector<char>> frame_copy;

public:
    Render(std::vector<std::vector<char>>& frame) {
        frame_buffer = frame_copy = frame;
        for (unsigned i = 0; i < frame_buffer.size(); i++) {
            for (unsigned j = 0; j < frame_buffer[i].size(); j++) {
                frame_buffer[i][j] = ' ';
            }
        }
    }

    void update_frame(std::vector<std::vector<char>>& frame) {
        frame_copy = frame;
    }

    void draw() {
        for (unsigned i = 0; i < frame_buffer.size(); i++) {
            for (unsigned j = 0; j < frame_buffer[i].size(); j++) {
                if (frame_buffer[i][j] != frame_copy[i][j]) {
                    frame_buffer[i][j] = frame_copy[i][j];
                    set_cursor_position(i, j);
                    std::cout << frame_buffer[i][j];
                }
            }
        }
    }


    // In case app need direct control of cursor & drawing
    static void set_cursor_position(int x, int y) {
        static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        std::cout.flush();
        COORD coord = { (SHORT)x, (SHORT)y };
        SetConsoleCursorPosition(hOut, coord);
    }

    static void set_color(unsigned char color_byte) {
        HANDLE hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
        SetConsoleTextAttribute(hstdout, color_byte);
    }
};


#endif
