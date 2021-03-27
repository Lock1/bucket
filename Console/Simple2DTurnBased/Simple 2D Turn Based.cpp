// Personal - 2D Simple Turn Based.cpp
// First ever project and self made game
// Start date : 31 September 2019
// Target satistfied date : 10 October 2019

/*******************************************************************
*********************** Documentation reminder *********************
Currently wip	: -Target Completed-
To do			: update()
Refine			: -

Target
Create 2D simple game with only 20x20, interaction UI, enemy, trap, crate, and portal to end.
Mechanics = Push, Attack, Examine, Speed, Taking boost item from crate, and status effect
Enemy = Ninja, Turret, Goblin

Updated target
Speed, Ninja, and turret was scrapped from planned feature in target.
Speed are removed due to tick system having bad implementation.
Turret need "AI" for attacking from distance.


List to add more
(Refine) Enemy	// Reimplement for different enemy subclasses
(Add) Equip		// Add equip and drop system
(?)				// Weird draw update for entity hp stat in interaction screen
(Refine) Code	// Too much copying exact code, need refactor badly

Tracking
1 Day for Class and Keyboard Input
1 Day for Basic draw() and everything
2 Day for Attack interaction and Interaction screen
1 Day for Attack interaction with enemy and push interaction
2 Day for debugging and completing the main code

Self note
Learned some gist of pointer, typecasting in c++, classes, time, virtual keyword and inheritance 

Things
int pointed = 4; int* poynter;
int hexadd = int(&pointed);
poynter = (int*)(hexadd);
std::cout << hexadd << endl;
std::cout << *poynter;

//
int ah = 1;
auto *haha=&ah;
std::cout << *haha;
******************************************************************/


// Library importing
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>	// Need for cls() and input()
#include <iostream>		// Usual i/o
#include <string>		// Need string for tooltip, name, etc
#include <queue>		// Queue system for input, action message
#include <vector>		// Maybe needed for keeping data
#include <chrono>		// Time and tick system
#include <thread>		// For sleep()
#include <stdlib.h>		// For random spread attack
//#include <stdlib.h> // for system("CLS") but is has flickering
using namespace std;

// Global variable initialization
queue<string> actmsg;
queue<int> qinp;
char drw[20][20], prvdrw[20][20];						// Game map screen frame, 20x20
char interactdrw[15][50], prvinteractdrw[15][50];		// Interaction screen frame, 15x50
unsigned long long int tickCount = 0;
int boosthp = 0, boostatk = 0;
bool interact = false, cantpush = false, vic = false, died = false; int interactdir = 0;
const int TARGET_UPDATE = 33;		// Target update 33ms
int counterdebug = 0; // debug use only
// Class declaration
class Entity {
protected:
    int posX = 0, posY = 0, hp = 0, str = 0, spd = 0, status = 0, maxhp = 0, id = 0;
    bool moveable = false; char tile = '!';
    string tooltip = ""; string name = "";
public:
    // Set method for entity, currently theres no fractional tick, so spd is useless
    virtual void sethp(int nhp) {}
    void setposX(int x) { posX = x; }
    void setposY(int y) { posY = y; }
    void setstr(int nstr) { str = nstr; }
    void setspd(int nspd) { spd = nspd; }
    void setmoveable(bool move) { moveable = move; }
    void settooltip(string ntool) { tooltip = ntool; }
    void setstatus(int stat) { status = stat; }
    void settile(char ntile) { tile = ntile; }
    void setname(string nname) { name = nname; }
    virtual void setbattack() {}
    void setmaxhp(int nmxhp) { maxhp = nmxhp; }
    virtual void setspreadatk() {}
    void setid(int nid) { id = nid; }
    // Get method for entity, for get coordinate and tile was shortened for quicker typing on update()
    int gethp() { return hp; }
    int gX() { return posX; }
    int gY() { return posY; }
    int getstr() { return str; }
    int getspd() { return spd; }
    bool getmoveable() { return moveable; }
    string gettooltip() { return tooltip; }
    int getstatus() { return status; }
    char gti() { return tile; }
    string getname() { return name; }
    virtual int getbattack() { return 0; }		// Any virtual method, when called from array of class, will call derived class method
    int getmaxhp() { return maxhp; }
    virtual int getspreadatk() { return 0; }
    int getid() { return id; }

    // Other method
    virtual void trapactivate(Entity* s) {}
    virtual void endgame() {}

};
Entity* map[20][20];
class Ground : public Entity {
public:
    Ground(int x, int y) {
        posX = x; posY = y; name = "Ground"; tooltip = "It's a ground.";
        tile = '\xfa';				// (Add) Equip - Add some dropped item on ground if want to implement equip, take & drop system 
    }
};

class Player : public Entity {
protected:
    int battack = 0, spreadatk = 0, range; bool alive = true;
public:
    Player(int x, int y, int nstr, int nspd, string tlt = "") {
        posX = x; posY = y; str = nstr; spd = nspd; tooltip = tlt;
        tile = 'p'; name = "Player"; maxhp = hp = 20;
    }
    void setbattack(int batk) { battack = batk; }
    void setspreadatk(int sprd) { spreadatk = sprd; }
    void setrange(int rng) { range = rng; }
    void sethp(int nhp) { if (nhp <= 0) { alive = false; } else hp = nhp; }
    int getbattack() { return battack; }
    bool getalive() { return alive; }
    int getspreadatk() { return spreadatk; }
};

class Enemy : public Entity { // (Refine) Enemy - Due some problem implementing different monster earlier, I decided to just use single enemy class
protected:
    int battack, spreadatk, range, sight;
public:
    Enemy(int x, int y, int nhp, int nstr, int nspd, char tlt, string nm) {
        posX = x; posY = y; hp = nhp; str = nstr; spd = nspd;
        moveable = true; tile = tlt; name = nm; id = 3; maxhp = nhp;
    }
    ~Enemy() {
        actmsg.push(name + " Killed                  ");
    }
    void setbattack(int batk) { battack = batk; }
    void setspreadatk(int sprd) { spreadatk = sprd; }
    void setrange(int rng) { range = rng; }

    void sethp(int nhp) {
        if (nhp <= 0) {
            interact = false;
            map[posX][posY] = new Ground(posX, posY);
            died = true;
            delete this;
        }
        else hp = nhp;
    }
    int getbattack() { return battack; }
    int getspreadatk() { return spreadatk; }
};
// Inanimate object
class Wall : public Entity {
public:
    Wall(int x, int y, int nstr = 10, int nhp = 999) {
        posX = x; posY = y; str = nstr; hp = nhp; maxhp = nhp; moveable = false;
        tooltip = "That's wall, it made from very thick stone.";
        tile = '\xdb'; name = "Wall";
    }
    ~Wall() {
        actmsg.push("Wow, you managed to destroy wall.");
    }
    void sethp(int nhp) {
        if (nhp <= 0) {
            interact = false;
            map[posX][posY] = new Ground(posX, posY);
            delete this;
        }
        else hp = nhp;
    }
};

class Crate : public Entity {
private:
    string drop = ""; string msg = "";
public:
    Crate(int x, int y, int nstr, int nhp, string ndrop = "HP") {
        posX = x; posY = y; str = nstr; hp = nhp; drop = ndrop; name = "Crate";
        tooltip = "It's a wooden crate."; maxhp = nhp; tile = '#';
    }
    ~Crate() {
        msg += "Crate destroyed, "; msg += drop; msg += " Dropped.";
        actmsg.push(msg);
        actmsg.push(drop + " picked up.");				// (Add) Equip - Due not sure will implement equipment drop system or not
        if (drop == "HP")
            boosthp = 4;
        if (drop == "Sword")
            boostatk = 5;
    }
    void setdrop(string dr) { drop = dr; }
    void sethp(int nhp) {
        if (nhp <= 0) {
            interact = false;
            map[posX][posY] = new Ground(posX, posY);
            delete this;
        }
        else hp = nhp;
    }
};

class Portal : public Entity {
public:
    Portal(int x, int y) {
        posX = x; posY = y; name = "Portal"; tile = 'O';
    }
    void endgame() {
        actmsg.push("Hooray!");
        actmsg.push("Press enter to continue.");
        vic = true;
    }
};

class Trap : public Entity {
private:
    int dmg = 0;
public:
    Trap(int x, int y, int dinf) {
        posX = x; posY = y; dmg = dinf; name = "Trap"; tile = 'x';
    }
    void trapactivate(Entity* actor) {
        actmsg.push(actor->getname() + " hitted by trap.");
        actor->sethp(actor->gethp() - dmg);
        map[posX][posY] = new Ground(posX, posY);
        delete this;
    }
};


// Stack overflow implementation of cls(), and cursor position system for rendering frame
void cls()
{
    // Get the Win32 handle representing standard output.
    // This generally only has to be done once, so we make it static.
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    COORD topLeft = { 0, 0 };

    // std::std::cout uses a buffer to batch writes to the underlying console.
    // We need to flush that to the console because we're circumventing
    // std::std::cout entirely; after we clear the console, we don't want
    // stale buffered text to randomly be written out.
    std::cout.flush();

    // Figure out the current width and height of the console window
    if (!GetConsoleScreenBufferInfo(hOut, &csbi)) {
        // TODO: Handle failure!
        abort();
    }
    DWORD length = csbi.dwSize.X * csbi.dwSize.Y;

    DWORD written;

    // Flood-fill the console with spaces to clear it
    FillConsoleOutputCharacter(hOut, TEXT(' '), length, topLeft, &written);

    // Reset the attributes of every character to the default.
    // This clears all background colour formatting, if any.
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, length, topLeft, &written);

    // Move the cursor back to the top left for the next sequence of writes
    SetConsoleCursorPosition(hOut, topLeft);
}
void setCursorPosition(int x, int y)
{
    static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    std::cout.flush();
    COORD coord = { (SHORT)x, (SHORT)y };
    SetConsoleCursorPosition(hOut, coord);
}

// Player, Enemy initialize
Player* plr = new Player(1, 1, 5, 10);
Enemy* gob1 = new Enemy(1, 4, 8, 3, 0, 'q', "Goblin");
Crate* crate1 = new Crate(5, 4, 3, 6);
Crate* crate2 = new Crate(2, 18, 3, 8, "Sword");
Trap* trap1 = new Trap(3, 3, 2);
Trap* trap2 = new Trap(1, 16, 8);
Portal* end1 = new Portal(18, 18);
Enemy* nin1 = new Enemy(1, 15, 8, 3, 1, 'n', "Ninja");
/* Tiles
\xdb for block tile -> Wall		# for crate		-> Crate
P for player		-> Player	x for trap		-> Trap
q for goblin		-> Goblin	. for ground	-> Ground
j for ninja			-> Ninja	O for end		-> Portal
o for turret		-> Turret

Type Identifier
1 for	Player
2 for	Wall
3 for	Enemy

status effect will indicated with integer
Status effect
1	-> Poison
101 -> Reserved for special goblin bonus
102
*/
// Use system("CLS") for updating, use (GetKeyState(VK_UP) & 0x8000) for key down
// Implement action messages
// Try to keep main() only for calling function as update(), draw(), and input().

// Action message take
string actmsgupdatetake(queue<string> msg, int x) {
    string tp = "";
    x = 2 - x;
    while (0 < x--)
        msg.pop();
    tp += msg.front();
    return tp;
}

// Simple string length
int strlen(string msg) {
    int i = 0;
    while (msg[i] != '\0')
    {
        ++i;
    }
    return i;
}

// Write to interaction
void writeinteract(string msg, int line) {
    if ((strlen(msg) < 48) && (line < 14)) {
        for (int j = 0; j < 48; j++) {
            if (j < strlen(msg)) {
                interactdrw[line][j + 1] = msg[j];
            }
            else {
                interactdrw[line][j + 1] = '\x20';
            }
        }
    }
}

// Fight using inheritance and virtual methods
void plrfight(Entity* e) {
    int atkcalplr = 0, atkcalenem = 0;
    string pushmsg1 = "", pushmsg2 = "";
    atkcalplr = rand() % (plr->getspreadatk());
    atkcalplr += plr->getbattack();
    atkcalenem = rand() % e->getspreadatk();
    atkcalenem += e->getbattack();

    // Action message push
    pushmsg1 += "You attack ";
    pushmsg1 += e->getname();
    pushmsg1 += " with ";
    pushmsg1 += to_string(atkcalplr);
    pushmsg1 += " damage.";
    actmsg.push(pushmsg1);
    pushmsg2 += e->getname();
    pushmsg2 += " attack with ";
    pushmsg2 += to_string(atkcalenem);
    pushmsg2 += " damage.";
    actmsg.push(pushmsg2);
    plr->sethp(plr->gethp() - atkcalenem);
    e->sethp(e->gethp() - atkcalplr);
}

// Push mechanic
void plrpush(Entity* e, int dir) {
    if (plr->getstr() > e->getstr()) {
        cantpush = false;
        switch (dir) {
        case 1:
            e->setposY(e->gY() + 1);
            break;
        case 2:
            e->setposY(e->gY() - 1);
            break;
        case 3:
            e->setposX(e->gX() - 1);
            break;
        case 4:
            e->setposX(e->gX() + 1);
            break;
        }
    }
    else
        cantpush = true;
}

// Draw() also will print action messages, using primitive call system
void draw() {
    setCursorPosition(20, 20); cout << "Frame pushed : " << ++counterdebug;
    // Creating HP, Attack, and str stat UI in interaction screen
    string tphpdrw = "Health "; int tphpctr = ((plr->gethp() * 10) / plr->getmaxhp());
    for (int i = 0; i < 10; i++) {
        if (tphpctr-- > 0)
            tphpdrw += "\xdb";
        else
            tphpdrw += "\x2d";
    }
    tphpdrw += " (";
    if (plr->gethp() < 10) {
        tphpdrw += "\x20";
    }
    if (plr->getalive())
        tphpdrw += to_string(plr->gethp());
    else
        tphpdrw += "0";
    tphpdrw += "/";
    tphpdrw += to_string(plr->getmaxhp());
    tphpdrw += ") ";
    tphpdrw += "Atk ";
    tphpdrw += to_string(plr->getbattack());
    tphpdrw += " Str ";
    tphpdrw += to_string(plr->getstr());

    // Getting every tile on coordinate
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            drw[i][j] = map[i][j]->gti();
        }
    }

    // Map screen draw
    setCursorPosition(0, 0);
    if (prvdrw[0][0] == '0') {
        for (int j = 19; j >= 0; j--) {		// Need from biggest y because printing from highest y to lowest y
            string tempdrw = "";
            for (int i = 0; i < 20; i++) {
                tempdrw += drw[i][j];
            }
            // Action message draw sequence
            if (j == 19) {
                for (int i = 0; i < 20; i++)
                    tempdrw += " ";
                tempdrw += "----Act----";
            }
            else if (j > 15) {
                for (int i = 0; i < 20; i++)
                    tempdrw += " ";
                tempdrw += actmsgupdatetake(actmsg, j - 16);
            }
            std::cout << tempdrw << endl;
        }
    }
    else {		// Double buffering system
        for (int y = 0; y < 20; y++) {
            for (int x = 0; x < 20; x++) {
                if (drw[x][y] != prvdrw[x][y]) {
                    setCursorPosition(x, 19 - y);		// Need to invert y axis due to already messed up map system
                    std::cout << drw[x][y];
                }
                continue;
            }
        }
        std::cout.flush();
    }

    // Action message draw
    setCursorPosition(40, 1);
    std::cout << actmsgupdatetake(actmsg, 2) << "                     ";
    setCursorPosition(40, 2);
    std::cout << actmsgupdatetake(actmsg, 1) << "                     ";
    setCursorPosition(40, 3);
    std::cout << actmsgupdatetake(actmsg, 0) << "                     ";

    // Clock Draw
    switch ((tickCount - 1) % 4) {		// Minus 1 due tick updated for first time before drawing
    case 0:
        setCursorPosition(21, 0);
        std::cout << "\xdb";
        if (tickCount) {
            setCursorPosition(22, 0);
            std::cout << " ";
        }
        break;
    case 1:
        setCursorPosition(22, 0);
        std::cout << "\xdb";
        setCursorPosition(22, 1);
        std::cout << " ";
        break;
    case 2:
        setCursorPosition(22, 1);
        std::cout << "\xdb";
        setCursorPosition(21, 1);
        std::cout << " ";
        break;
    case 3:
        setCursorPosition(21, 1);
        std::cout << "\xdb";
        setCursorPosition(21, 0);
        std::cout << " ";
        break;
    }

    // Interaction UI update
    writeinteract(tphpdrw, 1);
    if (not interact) {
        writeinteract("", 2);
        writeinteract("", 3);
        writeinteract("", 4);
        writeinteract("", 5);
        writeinteract("", 6);
    }

    // Interaction screen draw
    if (interactdrw[0][0] == '0') {			// First frame draw
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 50; j++) {
                interactdrw[i][j] = '\x20';
            }
        }
        for (int i = 0; i < 50; i++) {
            setCursorPosition(40 + i, 5);
            std::cout << "\xdb";
            setCursorPosition(40 + i, 19);
            std::cout << "\xdb";
            interactdrw[0][i] = '\xdb';
            interactdrw[14][i] = '\xdb';
            if (i < 15) {
                setCursorPosition(40, 5 + i);
                std::cout << '\xdb';
                setCursorPosition(89, 5 + i);
                std::cout << '\xdb';
                interactdrw[i][0] = '\xdb';
                interactdrw[i][49] = '\xdb';
            }
        }
    }
    // Frame update
    else {
        for (int i = 0; i < 15; i++) {
            for (int j = 0; j < 50; j++) {
                if (prvinteractdrw[i][j] == interactdrw[i][j])
                    continue;
                setCursorPosition(40 + j, 5 + i);
                std::cout << interactdrw[i][j];
            }
        }
    }
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 50; j++) {
            if (j < 20)
                prvdrw[i][j] = drw[i][j];
            if (i < 15)
                prvinteractdrw[i][j] = interactdrw[i][j];
        }
    }

    // Entity hp stat screen update while interacting
    if (interact) {
        string namehp = "";
        switch (interactdir) {
        case 1:
            namehp += map[plr->gX()][plr->gY() + 1]->getname();
            namehp += "   (";
            // (Add) Probably better implement algorithm for this space adder
            if (map[plr->gX()][plr->gY() + 1]->gethp() < 100)
                namehp += " ";
            if (map[plr->gX()][plr->gY() + 1]->gethp() < 10)
                namehp += " ";
            namehp += to_string(map[plr->gX()][plr->gY() + 1]->gethp());
            namehp += "/";
            namehp += to_string(map[plr->gX()][plr->gY() + 1]->getmaxhp());
            namehp += ")";
            writeinteract(namehp, 2);
            break;
        case 2:
            namehp += map[plr->gX()][plr->gY() - 1]->getname();
            namehp += "   (";
            // (Add) Probably better implement algorithm for this space adder
            if (map[plr->gX()][plr->gY() - 1]->gethp() < 100)
                namehp += " ";
            if (map[plr->gX()][plr->gY() - 1]->gethp() < 10)
                namehp += " ";
            namehp += to_string(map[plr->gX()][plr->gY() - 1]->gethp());
            namehp += "/";
            namehp += to_string(map[plr->gX()][plr->gY() - 1]->getmaxhp());
            namehp += ")";
            writeinteract(namehp, 2);
            break;
        case 3:
            namehp += map[plr->gX() - 1][plr->gY()]->getname();
            namehp += "   (";
            // (Add) Probably better implement algorithm for this space adder
            if (map[plr->gX() - 1][plr->gY()]->gethp() < 100)
                namehp += " ";
            if (map[plr->gX() - 1][plr->gY()]->gethp() < 10)
                namehp += " ";
            namehp += to_string(map[plr->gX() - 1][plr->gY()]->gethp());
            namehp += "/";
            namehp += to_string(map[plr->gX() - 1][plr->gY()]->getmaxhp());
            namehp += ")";
            writeinteract(namehp, 2);
            break;
        case 4:
            namehp += map[plr->gX() + 1][plr->gY()]->getname();
            namehp += "   (";
            // (Add) Probably better implement algorithm for this space adder
            if (map[plr->gX() + 1][plr->gY()]->gethp() < 100)
                namehp += " ";
            if (map[plr->gX() + 1][plr->gY()]->gethp() < 10)
                namehp += " ";
            namehp += to_string(map[plr->gX() + 1][plr->gY()]->gethp());
            namehp += "/";
            namehp += to_string(map[plr->gX() + 1][plr->gY()]->getmaxhp());
            namehp += ")";
            writeinteract(namehp, 2);
            break;
        }
    }

}


// Input() using queue system
void input() {
    if (not interact) {
        // Up = 1	; Down = 2	; Left = 3	; Right = 4
        if ((GetKeyState(VK_UP) & 0x8000))
            qinp.push(1);
        if ((GetKeyState(VK_DOWN) & 0x8000))
            qinp.push(2);
        if ((GetKeyState(VK_LEFT) & 0x8000))
            qinp.push(3);
        if ((GetKeyState(VK_RIGHT) & 0x8000))
            qinp.push(4);
    }
    else {
        // 101 for 1, 102 for 2, etc
        if ((GetKeyState('1') & 0x8000))
            qinp.push(101);
        if ((GetKeyState('2') & 0x8000))
            qinp.push(102);
        if ((GetKeyState('3') & 0x8000))
            qinp.push(103);
        if ((GetKeyState('4') & 0x8000))
            qinp.push(104);
    }
}

// Ignore every input except tick is updated, because it turn based game
// Useful time function : chrono::system_clock::now(), sleep()
void tickupdate() {
    bool stillinteract = interact, moving = false;
    if (boosthp) {
        if (plr->gethp() + boosthp <= plr->getmaxhp()) {
            plr->sethp(plr->gethp() + boosthp);
            boosthp = 0;
        }
        else
            plr->sethp(plr->getmaxhp());
    }
    if (boostatk) {
        plr->setbattack(plr->getbattack() + boostatk);
        boostatk = 0;
    }

    if (not(qinp.empty()) && not interact) {
        switch (qinp.front()) {
        case 1:
            if (plr->gY() < 19) {
                // Move up if and only if its ground, and endgame if it portal
                if (map[plr->gX()][plr->gY() + 1]->getname() == "Portal")
                    map[plr->gX()][plr->gY() + 1]->endgame();
                else if (map[plr->gX()][plr->gY() + 1]->getname() == "Ground") {
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX()][plr->gY() + 1];
                    map[plr->gX()][plr->gY() + 1] = plr;
                    plr->setposY(plr->gY() + 1);
                    moving = true;
                }
                // If trap
                else if (map[plr->gX()][plr->gY() + 1]->getname() == "Trap") {
                    map[plr->gX()][plr->gY() + 1]->trapactivate(plr);
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX()][plr->gY() + 1];
                    map[plr->gX()][plr->gY() + 1] = plr;
                    plr->setposY(plr->gY() + 1);
                }

                // Not ground
                else if (not(map[plr->gX()][plr->gY() + 1]->getname() == "Ground")) {
                    interact = true; string namehp = ""; interactdir = 1;
                    namehp += map[plr->gX()][plr->gY() + 1]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX()][plr->gY() + 1]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX()][plr->gY() + 1]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX()][plr->gY() + 1]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX()][plr->gY() + 1]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
                // Enemy fight
                else if (map[plr->gX()][plr->gY() + 1]->getid() == 3) {
                    interact = true; string namehp = ""; interactdir = 1;
                    namehp += map[plr->gX()][plr->gY() + 1]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX()][plr->gY() + 1]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX()][plr->gY() + 1]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX()][plr->gY() + 1]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX()][plr->gY() + 1]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
            }
            break;
        case 2:
            if (plr->gY() >= 1) {
                // Move down if and only if its ground, end if it portal
                if (map[plr->gX()][plr->gY() - 1]->getname() == "Portal")
                    map[plr->gX()][plr->gY() - 1]->endgame();
                else if (map[plr->gX()][plr->gY() - 1]->getname() == "Ground") {
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX()][plr->gY() - 1];
                    map[plr->gX()][plr->gY() - 1] = plr;
                    plr->setposY(plr->gY() - 1);
                    moving = true;
                }
                // If it trap
                else if (map[plr->gX()][plr->gY() - 1]->getname() == "Trap") {
                    map[plr->gX()][plr->gY() - 1]->trapactivate(plr);
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX()][plr->gY() - 1];
                    map[plr->gX()][plr->gY() - 1] = plr;
                    plr->setposY(plr->gY() - 1);
                }
                // If it wall
                else if (not(map[plr->gX()][plr->gY() - 1]->getname() == "Ground")) {
                    interact = true; string namehp = ""; interactdir = 2;
                    namehp += map[plr->gX()][plr->gY() - 1]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX()][plr->gY() - 1]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX()][plr->gY() - 1]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX()][plr->gY() - 1]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX()][plr->gY() - 1]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
                // Enemy
                else if (map[plr->gX()][plr->gY() - 1]->getid() == 3) {
                    interact = true; string namehp = ""; interactdir = 2;
                    namehp += map[plr->gX()][plr->gY() - 1]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX()][plr->gY() - 1]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX()][plr->gY() - 1]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX()][plr->gY() - 1]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX()][plr->gY() - 1]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
            }
            break;
        case 3:
            if (plr->gX() >= 1) {
                // Move left if and only if its ground, end if it portal
                if (map[plr->gX() - 1][plr->gY()]->getname() == "Portal")
                    map[plr->gX() - 1][plr->gY()]->endgame();
                else if (map[plr->gX() - 1][plr->gY()]->getname() == "Ground") {
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX() - 1][plr->gY()];
                    map[plr->gX() - 1][plr->gY()] = plr;
                    plr->setposX(plr->gX() - 1);
                    moving = true;
                }
                // if it trap
                else if (map[plr->gX() - 1][plr->gY()]->getname() == "Trap") {
                    map[plr->gX() - 1][plr->gY()]->trapactivate(plr);
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX() - 1][plr->gY()];
                    map[plr->gX() - 1][plr->gY()] = plr;
                    plr->setposX(plr->gX() - 1);
                }
                // if it wall
                else if (not(map[plr->gX() - 1][plr->gY()]->getname() == "Ground")) {
                    interact = true; string namehp = ""; interactdir = 3;
                    namehp += map[plr->gX() - 1][plr->gY()]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX() - 1][plr->gY()]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX() - 1][plr->gY()]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX() - 1][plr->gY()]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX() - 1][plr->gY()]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
                // Enemy
                else if (map[plr->gX() - 1][plr->gY()]->getid() == 3) {
                    interact = true; string namehp = ""; interactdir = 3;
                    namehp += map[plr->gX() - 1][plr->gY()]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX() - 1][plr->gY()]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX() - 1][plr->gY()]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX() - 1][plr->gY()]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX() - 1][plr->gY()]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
            }
            break;
        case 4:
            if (plr->gX() < 19) {
                // Move right if and only if its ground
                if (map[plr->gX() + 1][plr->gY()]->getname() == "Portal")
                    map[plr->gX() + 1][plr->gY()]->endgame();
                else if (map[plr->gX() + 1][plr->gY()]->getname() == "Ground") {
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX() + 1][plr->gY()];
                    map[plr->gX() + 1][plr->gY()] = plr;
                    plr->setposX(plr->gX() + 1);
                    moving = true;
                }
                // if it trap
                else if (map[plr->gX() + 1][plr->gY()]->getname() == "Trap") {
                    map[plr->gX() + 1][plr->gY()]->trapactivate(plr);
                    map[plr->gX()][plr->gY()] = new Ground(plr->gX(), plr->gY());
                    delete map[plr->gX() + 1][plr->gY()];
                    map[plr->gX() + 1][plr->gY()] = plr;
                    plr->setposX(plr->gX() + 1);
                }
                // if it wall
                else if (not(map[plr->gX() + 1][plr->gY()]->getname() == "Ground")) {
                    interact = true; string namehp = ""; interactdir = 4;
                    namehp += map[plr->gX() + 1][plr->gY()]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX() + 1][plr->gY()]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX() + 1][plr->gY()]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX() + 1][plr->gY()]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX() + 1][plr->gY()]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }
                else if (map[plr->gX() + 1][plr->gY()]->getid() == 3) {
                    interact = true; string namehp = ""; interactdir = 4;
                    namehp += map[plr->gX() + 1][plr->gY()]->getname();
                    namehp += "   (";
                    // (Add) Probably better implement algorithm for this space adder
                    if (map[plr->gX() + 1][plr->gY()]->gethp() < 100)
                        namehp += " ";
                    if (map[plr->gX() + 1][plr->gY()]->gethp() < 10)
                        namehp += " ";
                    namehp += to_string(map[plr->gX() + 1][plr->gY()]->gethp());
                    namehp += "/";
                    namehp += to_string(map[plr->gX() + 1][plr->gY()]->getmaxhp());
                    namehp += ")";
                    writeinteract(namehp, 2);
                    writeinteract("1. Attack", 3);
                    writeinteract("2. Push", 4);
                    writeinteract("3. Examine", 5);
                    writeinteract("4. Exit", 6);
                }

            }
            break;
        }
    }
    else {
        switch (qinp.front()) {
        case 101:
            switch (interactdir) {
            case 1:
                if (map[plr->gX()][plr->gY() + 1]->getid() == 3)
                    plrfight(map[plr->gX()][plr->gY() + 1]);
                else
                    map[plr->gX()][plr->gY() + 1]->sethp(map[plr->gX()][plr->gY() + 1]->gethp() - plr->getbattack());
                draw();		// (?) Not sure why it need update frame
                break;
            case 2:
                if (map[plr->gX()][plr->gY() - 1]->getid() == 3)
                    plrfight(map[plr->gX()][plr->gY() - 1]);
                else
                    map[plr->gX()][plr->gY() - 1]->sethp(map[plr->gX()][plr->gY() - 1]->gethp() - plr->getbattack());
                draw();		// (?) Not sure why it need update frame
                break;
            case 3:
                if (map[plr->gX() - 1][plr->gY()]->getid() == 3)
                    plrfight(map[plr->gX() - 1][plr->gY()]);
                else
                    map[plr->gX() - 1][plr->gY()]->sethp(map[plr->gX() - 1][plr->gY()]->gethp() - plr->getbattack());
                draw();		// (?) Not sure why it need update frame
                break;
            case 4:
                if (map[plr->gX() + 1][plr->gY()]->getid() == 3)
                    plrfight(map[plr->gX() + 1][plr->gY()]);
                else
                    map[plr->gX() + 1][plr->gY()]->sethp(map[plr->gX() + 1][plr->gY()]->gethp() - plr->getbattack());
                draw();		// (?) Not sure why it need update frame
                break;
            }
            tickCount++;
            break;
        case 102:
            // push
            switch (interactdir) {
            case 1:
                // Push only if behind object is ground or trap
                if (plr->gY() < 18) {
                    if (map[plr->gX()][plr->gY() + 1]->getid() == 3) {
                        string temppush = ""; int tempcalculate = 0;
                        tempcalculate = map[plr->gX()][plr->gY() + 1]->getbattack();
                        tempcalculate += (rand() % map[plr->gX()][plr->gY() + 1]->getspreadatk());
                        temppush += map[plr->gX()][plr->gY() + 1]->getname();
                        temppush += " is attacking you with ";
                        temppush += to_string(tempcalculate);
                        temppush += " damage.";
                        actmsg.push(temppush);
                        plr->sethp(plr->gethp() - tempcalculate);
                    }

                    if (map[plr->gX()][plr->gY() + 2]->getname() == "Ground") {
                        plrpush(map[plr->gX()][plr->gY() + 1], 1);
                        if (cantpush)
                            actmsg.push(map[plr->gX()][plr->gY() + 1]->getname() + " resisted.");
                        else {
                            delete map[plr->gX()][plr->gY() + 2];
                            map[plr->gX()][plr->gY() + 2] = map[plr->gX()][plr->gY() + 1];
                            map[plr->gX()][plr->gY() + 1] = new Ground(plr->gX(), plr->gY() + 1);
                            actmsg.push("You pushed " + map[plr->gX()][plr->gY() + 2]->getname());
                        }
                    }
                    else if (map[plr->gX()][plr->gY() + 2]->getname() == "Trap") {
                        map[plr->gX()][plr->gY() + 2]->trapactivate(map[plr->gX()][plr->gY() + 1]);
                        map[plr->gX()][plr->gY() + 2] = map[plr->gX()][plr->gY() + 1];
                        map[plr->gX()][plr->gY() + 2]->setposY(plr->gY() + 2);
                        map[plr->gX()][plr->gY() + 1] = new Ground(plr->gX(), plr->gY() + 1);
                    }
                    else
                        actmsg.push("You can't push " + map[plr->gX()][plr->gY() + 1]->getname());
                }
                else {
                    actmsg.push("Cannot push further.");
                }
                break;
            case 2:
                // Push only if behind object is ground or trap
                if (plr->gY() > 1) {
                    if (map[plr->gX()][plr->gY() - 1]->getid() == 3) {
                        string temppush = ""; int tempcalculate = 0;
                        tempcalculate = map[plr->gX()][plr->gY() - 1]->getbattack();
                        tempcalculate += (rand() % map[plr->gX()][plr->gY() - 1]->getspreadatk());
                        temppush += map[plr->gX()][plr->gY() - 1]->getname();
                        temppush += " is attacking you with ";
                        temppush += to_string(tempcalculate);
                        temppush += " damage.";
                        actmsg.push(temppush);
                        plr->sethp(plr->gethp() - tempcalculate);
                    }
                    if (map[plr->gX()][plr->gY() - 2]->getname() == "Ground") {
                        plrpush(map[plr->gX()][plr->gY() - 1], 2);
                        if (cantpush)
                            actmsg.push(map[plr->gX()][plr->gY() - 1]->getname() + " resisted.");
                        else {
                            delete map[plr->gX()][plr->gY() - 2];
                            map[plr->gX()][plr->gY() - 2] = map[plr->gX()][plr->gY() - 1];
                            map[plr->gX()][plr->gY() - 1] = new Ground(plr->gX(), plr->gY() - 1);
                            actmsg.push("You pushed " + map[plr->gX()][plr->gY() - 2]->getname());
                        }
                    }
                    else if (map[plr->gX()][plr->gY() - 2]->getname() == "Trap") {
                        map[plr->gX()][plr->gY() - 2]->trapactivate(map[plr->gX()][plr->gY() - 1]);
                        map[plr->gX()][plr->gY() - 2] = map[plr->gX()][plr->gY() - 1];
                        map[plr->gX()][plr->gY() - 2]->setposY(plr->gY() - 2);
                        map[plr->gX()][plr->gY() - 1] = new Ground(plr->gX(), plr->gY() - 1);
                    }
                    else
                        actmsg.push("You can't push " + map[plr->gX()][plr->gY() - 1]->getname());
                }
                else {
                    actmsg.push("Cannot push further.");
                }
                break;
            case 3:
                // If it enemy pushed
                if (plr->gX() > 1) {
                    if (map[plr->gX() - 1][plr->gY()]->getid() == 3) {
                        string temppush = ""; int tempcalculate = 0;
                        tempcalculate = map[plr->gX() - 1][plr->gY()]->getbattack();
                        tempcalculate += (rand() % map[plr->gX() - 1][plr->gY()]->getspreadatk());
                        temppush += map[plr->gX() - 1][plr->gY()]->getname();
                        temppush += " is attacking you with ";
                        temppush += to_string(tempcalculate);
                        temppush += " damage.";
                        actmsg.push(temppush);
                        plr->sethp(plr->gethp() - tempcalculate);
                    }
                    // Push only if behind object is ground or trap
                    if (map[plr->gX() - 2][plr->gY()]->getname() == "Ground") {
                        plrpush(map[plr->gX() - 1][plr->gY()], 3);
                        if (cantpush)
                            actmsg.push(map[plr->gX() - 1][plr->gY()]->getname() + " resisted.");
                        else {
                            delete map[plr->gX() - 2][plr->gY()];
                            map[plr->gX() - 2][plr->gY()] = map[plr->gX() - 1][plr->gY()];
                            map[plr->gX() - 1][plr->gY()] = new Ground(plr->gX() - 1, plr->gY());
                            actmsg.push("You pushed " + map[plr->gX() - 2][plr->gY()]->getname());
                        }
                    }
                    else if (map[plr->gX() - 2][plr->gY()]->getname() == "Trap") {
                        map[plr->gX() - 2][plr->gY()]->trapactivate(map[plr->gX() - 1][plr->gY()]);
                        map[plr->gX() - 2][plr->gY()] = map[plr->gX() - 1][plr->gY()];
                        map[plr->gX() - 2][plr->gY()]->setposX(plr->gX() - 2);
                        map[plr->gX() - 1][plr->gY()] = new Ground(plr->gX() - 1, plr->gY());
                    }
                    // If behind object is wall, boundary or anything else than ground and trap
                    else
                        actmsg.push("You can't push " + map[plr->gX() - 1][plr->gY()]->getname());
                }
                else {
                    actmsg.push("Cannot push further.");
                }
                break;
            case 4:
                // If it enemy pushed
                if (plr->gX() < 18) {
                    if (map[plr->gX() + 1][plr->gY()]->getid() == 3) {
                        string temppush = ""; int tempcalculate = 0;
                        tempcalculate = map[plr->gX() + 1][plr->gY()]->getbattack();
                        tempcalculate += (rand() % map[plr->gX() + 1][plr->gY()]->getspreadatk());
                        temppush += map[plr->gX() + 1][plr->gY()]->getname();
                        temppush += " is attacking you with ";
                        temppush += to_string(tempcalculate);
                        temppush += " damage.";
                        actmsg.push(temppush);
                        plr->sethp(plr->gethp() - tempcalculate);
                    }
                    // Push only if behind object is ground or trap
                    if (map[plr->gX() + 2][plr->gY()]->getname() == "Ground") {
                        plrpush(map[plr->gX() + 1][plr->gY()], 4);
                        if (cantpush)
                            actmsg.push(map[plr->gX() + 1][plr->gY()]->getname() + " resisted.");
                        else {
                            delete map[plr->gX() + 2][plr->gY()];
                            map[plr->gX() + 2][plr->gY()] = map[plr->gX() + 1][plr->gY()];
                            map[plr->gX() + 1][plr->gY()] = new Ground(plr->gX() + 1, plr->gY());
                            actmsg.push("You pushed " + map[plr->gX() + 2][plr->gY()]->getname());
                        }
                    }
                    else if (map[plr->gX() + 2][plr->gY()]->getname() == "Trap") {
                        map[plr->gX() + 2][plr->gY()]->trapactivate(map[plr->gX() + 1][plr->gY()]);
                        map[plr->gX() + 2][plr->gY()] = map[plr->gX() + 1][plr->gY()];
                        map[plr->gX() + 2][plr->gY()]->setposX(plr->gX() + 2);
                        map[plr->gX() + 1][plr->gY()] = new Ground(plr->gX() + 1, plr->gY());
                    }
                    // If behind object is wall, boundary or anything else than ground and trap
                    else
                        actmsg.push("You can't push " + map[plr->gX() + 1][plr->gY()]->getname());
                }
                else {
                    actmsg.push("Cannot push further.");
                }
                break;
            }

            interact = false;
            break;
        case 103:
            // examine
            switch (interactdir) {
            case 1:
                actmsg.push(map[plr->gX()][plr->gY() + 1]->gettooltip());
                break;
            case 2:
                actmsg.push(map[plr->gX()][plr->gY() - 1]->gettooltip());
                break;
            case 3:
                actmsg.push(map[plr->gX() - 1][plr->gY()]->gettooltip());
                break;
            case 4:
                actmsg.push(map[plr->gX() + 1][plr->gY()]->gettooltip());
                break;
            }
            writeinteract("", 2);
            writeinteract("", 3);
            writeinteract("", 4);
            writeinteract("", 5);
            writeinteract("", 6);
            interact = false;
            break;
        case 104:
            interact = false;
            writeinteract("", 2);
            writeinteract("", 3);
            writeinteract("", 4);
            writeinteract("", 5);
            writeinteract("", 6);
            break;
        }
    }
    qinp.pop(); // Clear last input

    if (boosthp) {
        if (plr->gethp() + boosthp <= plr->getmaxhp()) {
            plr->sethp(plr->gethp() + boosthp);
            boosthp = 0;
        }
        else
            plr->sethp(plr->getmaxhp());
    }
    if (boostatk) {
        plr->setbattack(plr->getbattack() + boostatk);
        boostatk = 0;
    }


    if (((not stillinteract) && (not interact)) && (moving))		// This will ignoring tick update when interacting
        tickCount++;

}


int main()
{
    string inp;
    std::cout << "Hello!\nPress enter to start.\n";
    getline(cin, inp);		// Introduction
    std::cout << "Loading...";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    // Debug
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (map[i][j] == 0) {
                map[i][j] = new Ground(i, j);
                prvdrw[i][j] = '0';
            }
        }
    }
    cls();

    // DEBUG DELETE THIS LATER
    plr->sethp(999);
    setCursorPosition(40, 20); std::cout << "In development";
    // First frame initialize
    actmsg.push("Welcome!");
    actmsg.push("Use arrow key to navigate");
    actmsg.push("Press E to exit");
    plr->setbattack(3);
    plr->setspreadatk(3);
    plr->setstr(4);
    nin1->setbattack(4);
    nin1->setspreadatk(4);
    nin1->settooltip("Ninja carrying sharp blade.");
    gob1->setbattack(2);
    gob1->setspreadatk(1);
    gob1->setstr(2);
    gob1->settooltip("Small goblin carrying weapon.");
    map[crate1->gX()][crate1->gY()] = crate1;
    map[plr->gX()][plr->gY()] = plr;
    map[gob1->gX()][gob1->gY()] = gob1;
    map[trap1->gX()][trap1->gY()] = trap1;
    map[end1->gX()][end1->gY()] = end1;
    map[crate2->gX()][crate2->gY()] = crate2;
    map[nin1->gX()][nin1->gY()] = nin1;
    map[3][18] = new Wall(3, 18);
    map[3][17] = new Wall(3, 17);
    map[2][17] = new Wall(2, 17);
    map[1][16] = trap2;
    map[1][2] = new Wall(1, 2);
    map[3][2] = new Wall(3, 2);
    map[2][15] = new Wall(2, 15);
    map[2][4] = new Wall(2, 4);
    map[2][14] = new Wall(2, 14);
    map[17][17] = new Wall(17, 17);
    map[4][4] = new Wall(4, 4);
    map[6][4] = new Wall(6, 4);
    map[6][3] = new Wall(6, 3);
    map[6][2] = new Wall(6, 2);
    map[5][2] = new Wall(5, 2);
    map[4][2] = new Wall(4, 2);
    map[4][5] = new Wall(4, 5);
    map[5][5] = new Wall(5, 5);
    map[6][5] = new Wall(6, 5);
    // Initialize wall
    for (int i = 0; i < 20; i++) {
        map[0][i] = new Wall(0, i);
        map[i][0] = new Wall(i, 0);
        map[19][i] = new Wall(19, i);
        map[i][19] = new Wall(i, 19);
    }
    //writeinteract("Health",1);
    interactdrw[0][0] = '0';
    draw();
    // Draw second frame
    draw();
    setCursorPosition(21, 0);
    std::cout << " \xdb";
    setCursorPosition(21, 1);
    std::cout << "\xdb\xdb";
    // Real sad of manual initialization of map
    // End of initialization


    while (true) {
        if (GetKeyState('E') & 0x8000)
            break;
        else if (vic) {
            getline(cin, inp);
            break;
        }
        input();
        if (not qinp.empty()) {
            //cls();		// Activate this if using old draw algorithm
            tickupdate();
            while (actmsg.size() > 3)
                actmsg.pop();
            draw();
            setCursorPosition(21, 19);
            // Checking whether player is alive or not
            if (not plr->getalive()) {
                break;
            }
            // This sleep used for delaying another input after detected some input
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        setCursorPosition(21, 19);
        // This sleep handle input detect limit per second
        std::this_thread::sleep_for(std::chrono::milliseconds(TARGET_UPDATE));
        //std::cout << "Ticked";
    }

    if (plr->getalive() && vic) {
        cls();
        setCursorPosition(0, 0);
        std::cout << "Yay!\nPress enter to exit.\n";
        getline(cin, inp);
    }
    else if (plr->getalive()) {
        cls();
        setCursorPosition(0, 0);
        std::cout << "Press enter to exit.\n";
        getline(cin, inp);
    }
    else {
        cls();
        setCursorPosition(0, 0);
        std::cout << "You died, Game over!\nPress enter to exit.\n";
        getline(cin, inp);
    }

    cls();
    setCursorPosition(0, 0);
    std::cout << "Bye!";
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
}