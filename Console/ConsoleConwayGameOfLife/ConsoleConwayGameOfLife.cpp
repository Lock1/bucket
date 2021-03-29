// Using standard Conway's Game of Life rule
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "../windows_console_common.hpp"

#define SIZE_X 80
#define SIZE_Y 25

using namespace std;



class Plane {
private:
    vector<vector<char>> universe;
    vector<vector<char>> next_universe;

    bool is_valid_index(int a, int b) {
        return 0 <= a && a < (int) universe.size() && 0 <= b && b < (int) universe[0].size();
    }

    vector<char> get_surround(int a, int b) {
        vector<char> surrounding;
        for (int i = -1; i < 2; i++) {
            for (int j = -1; j < 2; j++) {
                if ((i != 0 || j != 0) && is_valid_index(a+i, b+j)) {
                    surrounding.push_back(universe[a+i][b+j]);
                }
            }
        }
        return surrounding;
    }

public:
    Plane() {

    }

    void load_init(ifstream& file) {
        int y_coor = 0;
        string buffer;
        vector<vector<char>> flipped_universe;
        while (getline(file, buffer) && y_coor < SIZE_Y) {
            vector<char> row_char;
            for (unsigned i = 0; i < buffer.length(); i++)
                row_char.push_back(buffer[i]);

            flipped_universe.push_back(row_char);
            y_coor++;
        }

        for (unsigned i = 0; i < SIZE_X; i++) {
            vector<char> column_char;
            for (unsigned j = 0; j < SIZE_Y; j++) {
                column_char.push_back(flipped_universe[j][i]);
            }
            universe.push_back(column_char);
        }
        next_universe = universe;
    }


    bool is_died() {
        for (unsigned i = 0; i < universe.size(); i++) {
            for (unsigned j = 0; j < universe[i].size(); j++) {
                if (universe[i][j] == 'o')
                    return false;
            }
        }
        return true;
    }

    void next_tick() {
        for (unsigned i = 0; i < universe.size(); i++) {
            for (unsigned j = 0; j < universe[i].size(); j++) {
                vector<char> surrounding = get_surround(i, j);
                int live_cell_count = 0;
                for (unsigned k = 0; k < surrounding.size(); k++) {
                    if (surrounding[k] == 'o')
                        live_cell_count++;
                }

                // Live cell branch
                if (universe[i][j] == 'o') {
                    if (live_cell_count < 2 || live_cell_count > 3)
                        next_universe[i][j] = '-';
                }
                // Dead cell branch
                else {
                    if (live_cell_count == 3)
                        next_universe[i][j] = 'o';
                }
            }
        }
        universe = next_universe;
    }

    vector<vector<char>>& get_plane_reference() {
        return universe;
    }
};



int main() {
    ifstream initial = ifstream("init.txt");
    Plane pl;
    pl.load_init(initial);
    Render rndr = Render(pl.get_plane_reference());
    long counter = 0;
    // while (not pl.is_died()) {
    cout << "Enter to go\n";
    string buffer;
    getline(cin, buffer);
    while (true) {
        rndr.update_frame(pl.get_plane_reference());
        rndr.draw();

        Render::set_cursor_position(0, SIZE_Y+1);
        counter++;
        cout << "Current iter : " << counter;

        Utility::sleep_thread_millisecond(250);
        pl.next_tick();
    }

    return 0;
}
