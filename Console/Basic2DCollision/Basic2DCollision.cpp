#include <iostream>
#include <Windows.h>	// Cursor handler
#include <chrono>		// Time and tick system
#include <thread>		// For sleep()

#define BOX_WIDTH 50
#define BOX_HEIGHT 25
#define UPDATE_MS 1

class render {
public:
	render() {
		for (int i = 0; i < BOX_HEIGHT; i++)
			for (int j = 0; j < BOX_WIDTH; j++) {
				oldFrame[i][j] = newFrame[i][j] = ' ';
				locationMatrix[i][j] = 0;
			}

	}
	// Render
	void createBorder() {
		// \xde\xdd Horizontal half block
		// \xdf Upper vertical half block
		// \xdc Lower vertical half block
		// \xdb Full block
		for (int i = 0; i < BOX_HEIGHT + 1; i++) {
			setCursorPosition(0, i);
			std::cout << '\xdb';
			setCursorPosition(BOX_WIDTH + 1, i);
			std::cout << '\xdb';
		}
		setCursorPosition(0, 0);
		for (int i = 0; i < BOX_WIDTH + 2; i++)
			std::cout << '\xdc';
		setCursorPosition(0, BOX_HEIGHT + 1);
		for (int i = 0; i < BOX_WIDTH + 2; i++)
			std::cout << '\xdf';
	}
	void forceDraw() {
		for (int i = 0; i < BOX_HEIGHT; i++) {
			setCursorPosition(1, 1 + i);
			for (int j = 0; j < BOX_WIDTH; j++) {
				std::cout << newFrame[i][j];
				oldFrame[i][j] = newFrame[i][j];
			}
		}
	}
	void draw() {
		for (int i = 0; i < BOX_HEIGHT; i++)
			for (int j = 0; j < BOX_WIDTH; j++)
				if (oldFrame[i][j] != newFrame[i][j]) {
					setCursorPosition(1 + j, 1 + i);
					std::cout << newFrame[i][j];
					oldFrame[i][j] = newFrame[i][j];
				}
		// Super expensive operation, drop over 200 FPS from 300+ to 120
		setCursorPosition(BOX_WIDTH + 4, 1);
		std::cout << "Ball X velocity : " << ballVelocityX << std::endl;
		setCursorPosition(BOX_WIDTH + 4, 2);
		std::cout << "Ball Y velocity : " << ballVelocityY << std::endl;
		setCursorPosition(BOX_WIDTH + 4, 3);
		std::cout << "Ball X : " << ballX << std::endl;
		setCursorPosition(BOX_WIDTH + 4, 4);
		std::cout << "Ball Y : " << ballY << std::endl;
		setCursorPosition(0, BOX_HEIGHT + 2);
	}
	void processFrame() {
		for (int i = 0; i < BOX_HEIGHT; i++)
			for (int j = 0; j < BOX_WIDTH; j++) {
				switch (locationMatrix[i][j]) {
				case 0:
					newFrame[i][j] = ' ';
					break;
				case 1:
					newFrame[i][j] = '\xdc';
					break;
				case 2:
					newFrame[i][j] = '\xdb';
					break;
				}
			}
	}

	// Physics
	void computePhysics() {
		// for (int i = 0; i < BOX_HEIGHT; i++)
		// 	for (int j = 0; j < BOX_WIDTH; j++)
		// 		locationMatrix[i][j] = 0;
		locationMatrix[lastTruncatedY][lastTruncatedX] = 0;
		// TODO : Instead delete everything, just record old location

		// Other features
		// ballVelocityY += 0.001; // Gravity, due frame is flipped its +

		// Air Resistance
		if (ballVelocityX > 0)
			ballVelocityX -= 0.0001;
		else
			ballVelocityX += 0.0001;

		if (ballVelocityY > 0)
			ballVelocityY -= 0.0001;
		else
			ballVelocityY += 0.0001;


		// Speed truncation
		if (-0.00001 < ballVelocityX && ballVelocityX < 0.00001)
			ballVelocityX = 0;
		if (-0.00001 < ballVelocityY && ballVelocityY < 0.00001)
			ballVelocityY = 0;

		// TODO : Bar momentum transfer

		// Border collision
		if ((ballX + ballVelocityX >= BOX_WIDTH) || (ballX + ballVelocityX <= 0)) {
			ballVelocityX *= -1;
		}
		if ((ballY + ballVelocityY >= BOX_HEIGHT) || (ballY + ballVelocityY <= 0)) {
			ballVelocityY *= -1;
		}

		// Wall collision
		if (locationMatrix[(int)(ballY + ballVelocityY)][(int)(ballX + ballVelocityX)] == 2) {
			// ballVelocityX *= -1;
			// Only basic Y reflect
			ballVelocityY *= -1;
		}


		ballX += ballVelocityX;
		ballY += ballVelocityY;
		int truncatedX, truncatedY;

		if (((int)ballX) >= BOX_WIDTH)
			truncatedX = BOX_WIDTH - 1;
		else
			truncatedX = (int)ballX;

		if (((int)ballY) >= BOX_HEIGHT)
			truncatedY = BOX_HEIGHT - 1;
		else
			truncatedY = (int)ballY;

		lastTruncatedX = truncatedX;
		lastTruncatedY = truncatedY;
		locationMatrix[truncatedY][truncatedX] = 1;
	}

	void inputBar() {
		for (int j = 0; j < 5; j++)
			locationMatrix[barLocationY][barLocationX + j] = 0;

		if ((GetKeyState(VK_UP) & 0x8000) && barLocationY > 0)
			barLocationY -= 1;
		if ((GetKeyState(VK_DOWN) & 0x8000) && barLocationY < (BOX_HEIGHT - 1))
			barLocationY += 1;
		if ((GetKeyState(VK_LEFT) & 0x8000) && barLocationX > 0)
			barLocationX -= 1;
		if ((GetKeyState(VK_RIGHT) & 0x8000) && barLocationX < (BOX_WIDTH - 5))
			barLocationX += 1;

		for (int j = 0; j < 5; j++)
			locationMatrix[barLocationY][barLocationX + j] = 2;
	}



private:
	void setCursorPosition(int x, int y) {
		static const HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
		std::cout.flush();
		COORD coord = { (SHORT)x, (SHORT)y };
		SetConsoleCursorPosition(hOut, coord);
	}
	int locationMatrix[BOX_HEIGHT][BOX_WIDTH];
	int lastTruncatedX = 5, lastTruncatedY = 3;
	int barLocationX = 3, barLocationY = 23;
	char oldFrame[BOX_HEIGHT][BOX_WIDTH];
	char newFrame[BOX_HEIGHT][BOX_WIDTH];
	double ballX = 5, ballVelocityX = 0.5;
	double ballY = 3, ballVelocityY = 0.4;
	// const float updateFrequency = 1000/(UPDATE_MS);
};




int main() {
	render engine;
	engine.createBorder();
	engine.forceDraw();
	long long int frameCount = 0;
	auto startClock = std::chrono::system_clock::now();

	while (true) {
		std::this_thread::sleep_for(std::chrono::milliseconds(UPDATE_MS)); // Update cap
		if (frameCount % 4 == 0)
			engine.inputBar();
		engine.computePhysics();
		engine.processFrame();
		frameCount++;
		engine.draw();
		std::cout << "Frame count : " << frameCount << std::endl;
		auto endClock = std::chrono::system_clock::now();
		std::chrono::duration<double> timeDelta = endClock - startClock;
		std::cout << "FPS : " << frameCount / timeDelta.count() << std::endl;
	}
	return 0;
}
