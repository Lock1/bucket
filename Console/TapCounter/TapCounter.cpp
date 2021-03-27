// TapCounter.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>

using namespace std;

int zCtr = 0, xCtr = 0;

void setCursorPosition(int x, int y) {
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

void xCount() {
    while (true) {
        bool update = false;
        if (GetKeyState('X') & 0x8000) {
            while (GetKeyState('X') & 0x8000);
            xCtr++;
            update = true;
        }
        else if ((GetKeyState('O') & 0x8000) && (GetKeyState('P') & 0x8000))
            break;
        if (update) {
            setCursorPosition(4, 2);
            cout << xCtr;
            setCursorPosition(0, 3);
        }
    }
}

void zCount() {
    while (true) {
        bool update = false;
        if (GetKeyState('Z') & 0x8000) {
            while (GetKeyState('Z') & 0x8000);
            zCtr++;
            update = true;
        }
        else if ((GetKeyState('O') & 0x8000) && (GetKeyState('P') & 0x8000))
            break;
        if (update) {
            setCursorPosition(4, 1);
            cout << zCtr;
            setCursorPosition(0, 3);
        }
    }
}

int main() {
    cout << "Current Session\n";
    cout << "Z : \n";
    cout << "X : ";
    thread zth = thread(zCount);
    thread xth = thread(xCount);
    xth.join();
    zth.join();
    auto time_now = time(0);
    auto now = localtime(&time_now);
    string filename = to_string(now->tm_mday);
    filename = filename + "-" + to_string(now->tm_mon + 1);
    filename = filename + "-" + to_string(now->tm_year + 1900);
    filename = filename + "-" + to_string(now->tm_hour);
    filename = filename + "." + to_string(now->tm_min);
    filename = filename + "." + to_string(now->tm_sec);
    filename = filename + ".txt";
    ofstream targetFile = ofstream(filename);
    targetFile << "Z : " << zCtr << endl;
    targetFile << "X : " << xCtr;
    targetFile.close();
    return 0;
}
