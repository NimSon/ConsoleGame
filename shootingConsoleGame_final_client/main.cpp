#pragma comment(lib, "ws2_32.lib")
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include<stdlib.h>
#include<WinSock2.h>
#include"header.h"

#define PORT 9998
#define SERVER_IP "192.168.0.9"

Object player;
Object *playerServer;
Object player_bullet[PLAYERBULLETNUM];
Object *playerServer_bullet[PLAYERBULLETNUM];
Object Enemy[ENEMYNUM];
Object Enemy_bullet[ENEMYNUM];

HANDLE hStdout;

void error_handling(const char* message);

void single(int xLife, int xScore) {
	cls(hStdout);

	int life = xLife;
	int enemyCount;
	int score = xScore;

	while (1) {
		playerOutClient(&player);		/* server playe 위치 출력 */
		playerMove(&player);			/*server playe 위치 제어 */

		/* server player의 HP 상태 표시, life값이 0이 되면 종료(game over) */
		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) break;

		Enemy_init(Enemy);									/* Enemy[] 생성 */
		enemyCount = Enemy_play(Enemy, player_bullet);		/* Enemy[] 위치 이동 및 출력, 죽은 적의 수 반환 */

		/* server player의 점수 표시, enemy 죽인 수에 비례 */
		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);	/* server player 미사일 발사 */
		playerClientBulletOut(player_bullet);	/* server player 미사일 위치 이동 및 출력 */

		Bullet_init(Enemy, Enemy_bullet);	/* Enemy[] 미사일 발사 */
		Bullet_play(Enemy, Enemy_bullet);	/* Enemy[] 미사일 위치 이동 및 출력 */


		/* esc 키를 누르면 멈춤 */
		if (GetAsyncKeyState(VK_ESCAPE) && 0x8000) {
			cursor(WIDTH * 0.6, COLUMN * 0.7);
			system("pause");
		}

		Sleep(100);
		cls(hStdout);
	}
	/* game over시 (life == 0) mysql에 이름 및 스코어 저장 */
	cls(hStdout);
	mysqlUser(score);
}

int main(int argc, char* argv[]) {
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	consoleSize(WIDTH, COLUMN);

	WSADATA wsaData;
	SOCKET hSocket;
	SOCKADDR_IN servAddr;

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
		error_handling("WSAStartup() error!");

	hSocket = socket(PF_INET, SOCK_STREAM, 0);
	if (hSocket == INVALID_SOCKET)
		error_handling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
	servAddr.sin_port = htons(PORT);

	if (connect(hSocket, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)
		error_handling("connect() error");

	hideCursor();
	for (int i = 3; i >= 0; --i) {
		cursor(WIDTH * 0.48, COLUMN * 0.5 - 1);
		printf("GAME  START");
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 1);
		printf("     %d", i);
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 3);
		printf("비행체 : O");
		Sleep(1000);
		cls(hStdout);
	}

	int life = LIFE;
	int enemyCount = 0;
	int score = 0;
	int mode = 2;

	playerInit(&player);

	/* TCPIP에 필요한 변수 선언 */
	int sendValue;
	char buf[250];
	int strLen;
	char* ptr;   

	while (1) {
		sendValue = send(hSocket, (char*)& player, sizeof(player), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}

		playerOutClient(&player);
		playerMove(&player);

		strLen = recv(hSocket, buf, sizeof(Object), 0);
		if (strLen == SOCKET_ERROR) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		playerServer = (Object*)buf;

		playerOut(playerServer);
		//playerMove(playerServer);

		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) {
			shutdown(hSocket, SD_SEND);		// shutdown()하면 0 전달
			break;
		}

		/* Enemy 위치 및 활성화 상태(state)를 server로 부터 받는다. */
		strLen = recv(hSocket, buf, sizeof(Object) * ENEMYNUM, 0);
		if (strLen <= 0) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		ptr = buf;
		for (int i = 0; i < ENEMYNUM; ++i) {
			memcpy(&Enemy[i], (Object*)ptr, sizeof(Object));
			ptr += sizeof(Object);
		}

		// Enemy_init(Enemy);
		enemyCount = Enemy_play(Enemy, player_bullet);

		/* Enemy 활성화 상태(state)를 server에 보낸다. */
		bool enemyState[ENEMYNUM];
		for (int i = 0; i < ENEMYNUM; ++i) {
			enemyState[i] = Enemy[i].state;
		}
		sendValue = send(hSocket, (char*)enemyState, sizeof(enemyState), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}

		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);
		playerClientBulletOut(player_bullet);

		sendValue = send(hSocket, (char*)& player_bullet, sizeof(player_bullet), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}

		strLen = recv(hSocket, buf, sizeof(Object) * PLAYERBULLETNUM, 0);
		if (strLen <= 0) {
			mode = 1;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		ptr = buf;
		for (int i = 0; i < PLAYERBULLETNUM; ++i) {
			playerServer_bullet[i] = (Object*)ptr;
			ptr += sizeof(Object);
		}

		//playerBullet(playerServer, *playerServer_bullet);
		playerBulletOut(*playerServer_bullet);

		Bullet_init(Enemy, Enemy_bullet);
		Bullet_play(Enemy, Enemy_bullet);

		if (GetAsyncKeyState(VK_ESCAPE) && 0x8000) {
			cursor(WIDTH * 0.6, COLUMN * 0.7);
			system("pause");
		}
		Sleep(100);
		cls(hStdout);
	}
	closesocket(hSocket);	// closesocket()하면 -1 전달

	if (mode == 2) {
		cls(hStdout);
		mysqlUser(score);
	}

	WSACleanup();
	
	return 0;
}


void error_handling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}