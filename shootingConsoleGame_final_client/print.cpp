#include"header.h"

extern HANDLE hStdout;

// HP
int printHpState(Object* playerO, Object* enemyO, Object* enemyBulletO, int life) {
	for (int i = 0; i < ENEMYNUM; ++i) {
		if (enemyO[i].y == playerO->y)
			if (enemyO[i].x == playerO->x)
				--life;
		if (enemyBulletO[i].y == playerO->y)
			if (enemyBulletO[i].x <= playerO->x && enemyBulletO[i].x + 2 > playerO->x)
				--life;
	}

	cursor(2, COLUMN - 2);
	printf("HP : ");
	for (int i = 0; i < life; ++i) {
		printf("★");
	}

	return life;
}

// score
void printScore(int* score, int enemyCount) {
	cursor(WIDTH - 20, COLUMN - 2);
	*score += enemyCount * 100;
	printf("SCORE : %d", *score);
}

// Start Menu Screen
void printMenu() {
	int cursorY = COLUMN * 0.45;

	while (1) {
		cursor(WIDTH * 0.5 - 5, cursorY);
		printf("☞");
		cursor(WIDTH * 0.5, COLUMN * 0.5 - 2);
		printf("START");
		cursor(WIDTH * 0.5, COLUMN * 0.5);
		printf("EXIT");
		cursor(WIDTH * 0.5, COLUMN * 0.5 + 2);
		printf("SCORE");
		Sleep(50);

		if (GetAsyncKeyState(VK_DOWN) & 0x8000) {
			cursorY += 2;
			if (cursorY > COLUMN * 0.5 + 2) cursorY = COLUMN * 0.5 + 2;
			cls(hStdout);
		}
		if (GetAsyncKeyState(VK_UP) & 0x8000) {
			cursorY -= 2;
			if (cursorY < COLUMN * 0.5 - 2) cursorY = COLUMN * 0.5 - 2;
			cls(hStdout);
		}
		if (GetAsyncKeyState(VK_RETURN) & 0x8000) {
			if (cursorY == (int)(COLUMN * 0.5 - 2)) break;
			if (cursorY == (int)(COLUMN * 0.5)) exit(0);			// 게임 종료
			if (cursorY == (int)(COLUMN * 0.5 + 2)) printRank();	// 스코어 및 랭킹 페이지
		}
	}
}

void printRank() {
	printMysqlData();

	cursor(WIDTH * 0.6, COLUMN * 0.7);
	system("pause");
	cls(hStdout);
}
/*
void gameover() {
	printf("\n\n\n\n\n");
	printf("\t\t\t\t##########          #           ##     ##       ########\n");
	printf("\t\t\t\t#                 ##  #        #  #  #  #       #       \n");
	printf("\t\t\t\t#     ####       ##    #       #   # #  #       ########\n");
	printf("\t\t\t\t#        #      ## ######      #    #   #       #       \n");
	printf("\t\t\t\t##########     ##        #     #        #       ########\n");
	printf("\t\t\t\t==========     ==        =     =        =       ========\n");
	printf("\n");
	printf("\n");
	printf("\t\t\t\t   #####         #          #   ##########       #########\n");
	printf("\t\t\t\t #       #        #        #    #                #        #\n");
	printf("\t\t\t\t#         #        #      #     ##########       #########\n");
	printf("\t\t\t\t #        #         #    #      #                #     #  \n");
	printf("\t\t\t\t  #      #           #  #       #                #       # \n");
	printf("\t\t\t\t   ######             ##        ##########       #        #\n");
	printf("\t\t\t\t   =====              ==        ==========       =        =\n");
}
*/

void gameover() {
	cursor(WIDTH * 0.3, COLUMN * 0.1);
	printf("##########          #           ##     ##       ########");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 1);
	printf("#                 ##  #        #  #  #  #       #       ");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 2);
	printf("#     ####       ##    #       #   # #  #       ########");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 3);
	printf("#        #      ## ######      #    #   #       #       ");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 4);
	printf("##########     ##        #     #        #       ########");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 5);
	printf("==========     ==        =     =        =       ========");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 8);
	printf("   #####         #          #   ##########       #########");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 9);
	printf(" #       #        #        #    #                #        #");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 10);
	printf("#         #        #      #     ##########       #########");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 11);
	printf(" #        #         #    #      #                #     #  ");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 12);
	printf("  #      #           #  #       #                #       # ");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 13);
	printf("   ######             ##        ##########       #        #");
	cursor(WIDTH * 0.3, COLUMN * 0.1 + 14);
	printf("   =====              ==        ==========       =        =");
}