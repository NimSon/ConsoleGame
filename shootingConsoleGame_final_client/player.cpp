#include"header.h"

void playerInit(Object* playerO) {
	playerO->x = WIDTH * 0.3;
	playerO->y = COLUMN * 0.9;
	playerO->state = true;
}

void playerOut(Object* playerO) {
	cursor(playerO->x, playerO->y);
	if (playerO->state) {
		printf("¡ã");
	}
}

void playerOutClient(Object* playerO) {
	cursor(playerO->x, playerO->y);
	if (playerO->state) {
		printf("O");
	}
}

void playerMove(Object* playerO) {
	if (GetAsyncKeyState(VK_UP) & 0x8000) {
		playerO->y -= PLAYERMOVE;
		if (playerO->y < 0)
			playerO->y = 0;
	}
	if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
		playerO->y += PLAYERMOVE;
		if (playerO->y > COLUMN - 2)
			playerO->y = COLUMN - 2;
	}
	if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
		playerO->x -= PLAYERMOVE;
		if (playerO->x < 0)
			playerO->x = 0;
	}
	if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
		playerO->x += PLAYERMOVE;
		if (playerO->x > WIDTH - 2)
			playerO->x = WIDTH - 2;
	}
}

void playerBullet(Object* playerO, Object* playerBulletO) {
	for (int i = 0; i < PLAYERBULLETNUM; ++i) {
		if (!playerBulletO[i].state) {
			if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
				playerBulletO[i].state = true;
				playerBulletO[i].x = playerO->x;
				playerBulletO[i].y = playerO->y - 1;
				break;
			}
		}
	}

}

void playerBulletOut(Object* playerBulletO) {
	for (int i = 0; i < PLAYERBULLETNUM; ++i) {
		if (playerBulletO[i].state) {
			cursor(playerBulletO[i].x, playerBulletO[i].y);
			printf("¢Æ");

			playerBulletO[i].y -= PLAYERMOVE;
			if (playerBulletO[i].y < 0)
				playerBulletO[i].state = false;
		}
	}
}

void playerClientBulletOut(Object* playerBulletO) {
	for (int i = 0; i < PLAYERBULLETNUM; ++i) {
		if (playerBulletO[i].state) {
			cursor(playerBulletO[i].x, playerBulletO[i].y);
			printf("¡è");

			playerBulletO[i].y -= PLAYERMOVE;
			if (playerBulletO[i].y < 0)
				playerBulletO[i].state = false;
		}
	}
}