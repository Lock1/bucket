#include <stdio.h>
#include <stack>
#include <queue>
#include <chrono>
#include <thread>
#include <vector>
#include <iostream>
#include <Windows.h>
#include <math.h>
#include <map>

using namespace std;

enum Direction {
    Up,
    Left,
    Right,
    Down
};

class dstpl {
public:
    double dist;
    Direction tar;
    dstpl(double d, Direction dr) {
        dist = d;
        tar = dr;
    }
};

void setCursorPosition(int x, int y) {
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

class Position {
public:
    int x;
    int y;

    Position() : x(0), y(0) {

    }

    Position(int cx, int cy) : x(cx), y(cy) {

    }
    bool operator==(const Position& pos) {
        if (pos.x == x && pos.y == y)
            return true;
        else
            return false;
    }

    bool operator!=(const Position& pos) {
        if (pos.x != x || pos.y != y)
            return true;
        else
            return false;
    }

};

bool isWithinBound(int x, int y) {
    if (0 <= x && x < 20 && 0 <= y && y < 20)
        return true;
    else
        return false;
}

double distance(Position a, Position b) {
    double diffx = a.x - b.x;
    double diffy = a.y - b.y;
    return sqrt(diffx*diffx + diffy*diffy);
}


void pathfind(vector<vector<bool>>& map, Position& starting, Position& target) {
    vector<vector<bool>> visitedmap = map; // Let wall also marked as visited
    stack<Position> currentPath;
    stack<stack<Position>> branchList;
    Position curPos = starting;


    // Init Visualizing
    string mapFrame[20][20], mapBuffer[20][20];
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (map[i][j])
                mapBuffer[i][j] = mapFrame[i][j] = "\xDB";
            else
                mapBuffer[i][j] = mapFrame[i][j] = " ";
        }
    }

    system("cls");
    curPos = starting;
    mapBuffer[curPos.x][curPos.y] = mapFrame[curPos.x][curPos.y] = "x";
    mapBuffer[target.x][target.y] = mapFrame[target.x][target.y] = "#";

    for (int j = 0; j < 20; j++) {
        for (int i = 0; i < 20; i++) {
            cout << mapFrame[i][j];
        }
        cout << "\n";
    }





    // Path finding
    visitedmap[curPos.x][curPos.y] = true;
    bool isBacking = false;
    while (curPos != target) {
        // Using short-circuit conditional
        // Branch making
        stack<Position> branch;
        if (not isBacking) {
            double distRight = 1 << 10;
            double distLeft = 1 << 10;
            double distUp = 1 << 10;
            double distDown = 1 << 10;
            Position posRight = Position(curPos.x + 1, curPos.y);
            Position posLeft = Position(curPos.x - 1, curPos.y);
            Position posUp = Position(curPos.x, curPos.y + 1);
            Position posDown = Position(curPos.x, curPos.y - 1);
            if (isWithinBound(curPos.x + 1, curPos.y) && not visitedmap[curPos.x + 1][curPos.y])
                distRight = distance(target, posRight);
            if (isWithinBound(curPos.x - 1, curPos.y) && not visitedmap[curPos.x - 1][curPos.y])
                distLeft = distance(target, posLeft);
            if (isWithinBound(curPos.x, curPos.y + 1) && not visitedmap[curPos.x][curPos.y + 1])
                distUp = distance(target, posUp);
            if (isWithinBound(curPos.x, curPos.y - 1) && not visitedmap[curPos.x][curPos.y - 1])
                distDown = distance(target, posDown);

            vector<dstpl> arr;
            if (distRight < 1024)
                arr.push_back(dstpl(distRight, Right));
            if (distLeft < 1024)
                arr.push_back(dstpl(distLeft, Left));
            if (distUp < 1024)
                arr.push_back(dstpl(distUp, Up));
            if (distDown < 1024)
                arr.push_back(dstpl(distDown, Down));

            for (unsigned i = 0; i < arr.size(); i++) {
                for (unsigned j = i + 1; j < arr.size(); j++) {
                    if (arr[i].dist < arr[j].dist) {
                        // Sorting is flipped due using stack
                        dstpl tp = arr[i];
                        arr[i] = arr[j];
                        arr[j] = tp;
                    }
                }
            }
            // printf("%d   |  %d   >>>>  ", curPos.x, curPos.y);
            for (unsigned i = 0; i < arr.size(); i++) {
                // printf("%lf ", arr[i].dist);
                // std::this_thread::sleep_for(std::chrono::milliseconds(100));
                switch (arr[i].tar) {
                    case Up:
                        branch.push(posUp);
                        // printf("Up    |  ");
                        break;
                    case Down:
                        branch.push(posDown);
                        // printf("Down  |  ");
                        break;
                    case Left:
                        branch.push(posLeft);
                        // printf("Left  |  ");
                        break;
                    case Right:
                        branch.push(posRight);
                        // printf("Right |  ");
                        break;
                }
            }
            // puts("");


            branchList.push(branch);
        }

        // Move taking
        if (not branchList.empty()) {
            stack<Position>& lastBranch = branchList.top();
            if (not lastBranch.empty()) {
                curPos = lastBranch.top();
                visitedmap[curPos.x][curPos.y] = true;
                currentPath.push(curPos);
                isBacking = false;
                lastBranch.pop();
                // if (not branch.empty())
                // printf("branching (%d  |  %d)\n", branch.top().x, branch.top().y);
                // printf("Path used (%d  |  %d)\n", currentPath.top().x, currentPath.top().y);
            }
            else {
                // No direction available
                if (not currentPath.empty()) {
                    visitedmap[currentPath.top().x][currentPath.top().y] = false;
                    currentPath.pop();
                    branchList.pop();
                    isBacking = true;
                    curPos = currentPath.top();
                    // printf("Back  %d  |  %d\n", currentPath.top().x, currentPath.top().y);
                }
            }
        }
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

    // Reversing the path
    stack<Position> startToFinishPath;
    while (not currentPath.empty()) {
        startToFinishPath.push(currentPath.top());
        currentPath.pop();
    }


    cout << "ENTER TO DRAW PATH";
    string db;
    cin >> db;

    Position prev = curPos;
    while (not startToFinishPath.empty()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        // FIXME : Very bizarre visualizing behaviour
        // printf("Pos : %d, %d\n", startToFinishPath.top().x, startToFinishPath.top().y);

        if (not startToFinishPath.empty()) {
            mapFrame[curPos.x][curPos.y] = "o";
            curPos = startToFinishPath.top();
            startToFinishPath.pop();
            // setCursorPosition(curPos.x, curPos.y);
            // printf("x");
        }

        for (int j = 0; j < 20; j++) {
            for (int i = 0; i < 20; i++) {
                if (mapFrame[i][j] != mapBuffer[i][j]) {
                    mapBuffer[i][j] = mapFrame[i][j];
                    setCursorPosition(curPos.x, curPos.y);
                    cout << mapBuffer[i][j];
                }
            }
        }


        prev = curPos;
        mapFrame[prev.x][prev.y] = " ";

    }
}

int main() {
    vector<vector<bool>> map;
    for (int i = 0; i < 20; i++) {
        vector<bool> row;
        for (int j = 0; j < 20; j++)
            row.push_back(false);
        map.push_back(row);
    }

    for (int i = 0; i < 9; i++)
        map[10][i] = true;
    map[1][0] = true;
    map[1][2] = true;
    map[2][2] = true;
    map[3][2] = true;
    map[3][1] = true;

    map[5][1] = true;
    map[6][1] = true;
    map[7][1] = true;
    map[8][1] = true;
    map[8][2] = true;
    map[9][2] = true;

    map[1][4] = true;
    map[2][4] = true;
    map[3][4] = true;
    map[4][4] = true;
    map[5][4] = true;
    map[5][3] = true;
    map[5][5] = true;
    map[5][6] = true;
    map[6][6] = true;
    map[7][6] = true;
    map[8][6] = true;
    map[9][6] = true;
    map[7][4] = true;
    map[8][4] = true;
    map[9][4] = true;

    map[0][6] = true;
    map[1][6] = true;
    map[2][6] = true;
    map[3][6] = true;

    Position target = Position(19, 0);
    Position start = Position(0, 0);

    pathfind(map, start, target);


    return 0;
}
