#pragma comment(lib, "ws2_32.lib")

#include<stdlib.h>
#include<WinSock2.h>
#include"header.h"

#define PORT 9998
#define SERVER_IP "127.0.0.1"

Object player;
Object *playerClient;
Object player_bullet[PLAYERBULLETNUM];
Object *playerClient_bullet[PLAYERBULLETNUM];
Object Enemy[ENEMYNUM];
Object Enemy_bullet[ENEMYNUM];

HANDLE hStdout;

int mode;

void error_handling(const char* message);

void single(int xLife, int xScore) {
	cls(hStdout);
	if (mode == 1) {
		for (int i = 3; i >= 0; --i) {
			cursor(WIDTH * 0.48, COLUMN * 0.5 - 1);
			printf("GAME  START");
			cursor(WIDTH * 0.48, COLUMN * 0.5 + 1);
			printf("     %d", i);
			cursor(WIDTH * 0.48, COLUMN * 0.5 + 3);
			printf("비행체 : ▲");
			Sleep(1000);
			cls(hStdout);
		}
	}
	
	int life = xLife;
	int enemyCount;
	int score = xScore;

	while (1) {
		playerOut(&player);		/* server playe 위치 출력 */
		playerMove(&player);	/*server playe 위치 제어 */

		/* server player의 HP 상태 표시, life값이 0이 되면 종료(game over) */
		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) break;

		Enemy_init(Enemy);									/* Enemy[] 생성 */
		enemyCount = Enemy_play(Enemy, player_bullet);		/* Enemy[] 위치 이동 및 출력, 죽은 적의 수 반환 */


		/* server player의 점수 표시, enemy 죽인 수에 비례 */
		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);	/* server player 미사일 발사 */
		playerBulletOut(player_bullet);			/* server player 미사일 위치 이동 및 출력 */

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
	mysqlUser(score);
	cls(hStdout);
}

void multi() {
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;		// 소켓 생성을 위한 변수
	SOCKADDR_IN servAddr, clntAddr;		// 소켓주소를 저장하기 위한 구조체

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	// WSASatrtup() : 요구하는 윈도우 소켓 버전 알리고, 라이브러리 초기화 작업
		error_handling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);	// socket() : 소켓 생성
	if (hServSock == INVALID_SOCKET)				// socket() 함수로 소켓 생성에 실패하면(-1 반환),
		error_handling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));			// 구조체 servAddr를 0으로 초기화
	servAddr.sin_family = AF_INET;					// 주소 타입
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY : IP 리턴 상수, 192. 도 되고 127. 됨
	servAddr.sin_port = htons(PORT);				// 데이터 주고 받을 포트 번호

	if (bind(hServSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)	// bind() : server, 소켓에 주소정보(IP, port) 할당
		error_handling("bind() error");

	if (listen(hServSock, 5) == SOCKET_ERROR) // 대기상태 실패										// listen() : 소켓을 연결요청받기 가능한 상태로 만든다.
		error_handling("listen() error");

	cursor(WIDTH * 0.4, COLUMN * 0.5);
	printf("사용자 접속을 대기중입니다..");

	int szClntAddr = sizeof(clntAddr);	// client address 크기 저장
	hClntSock = accept(hServSock, (SOCKADDR*)& clntAddr, &szClntAddr);			// accept() : client, 연결요청 받으면, 그 요청 수락
	if (hClntSock == INVALID_SOCKET)
		error_handling("accept() error");

	cls(hStdout);
	for (int i = 3; i >= 0; --i) {
		cursor(WIDTH * 0.48, COLUMN * 0.5 - 1);
		printf("GAME  START");
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 1);
		printf("     %d", i);
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 3);
		printf("비행체 : ▲");
		Sleep(1000);
		cls(hStdout);
	}

	int life = LIFE;
	int enemyCount = 0;
	int score = 0;

	int sendValue;
	char buf[250];
	int strLen;
	char* ptr;

	while (1) {
		/* client에 server player 데이터를 보낸다. */
		sendValue = send(hClntSock, (char*)& player, sizeof(player), 0);		// 구조체 데이터를 client에 보낸다.
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		playerOut(&player);		/* server playe 위치 출력 */
		playerMove(&player);	/* server playe 위치 제어 */
		
		/* client로부터 client player 데이터를 받는다. */
		strLen = recv(hClntSock, buf, sizeof(Object), 0);	// recv() 전달한 데이터의 바이트 수를 strLen에 저장한다.
		if (strLen == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		playerClient = (Object*)buf;

		playerOutClient(playerClient);		/* client playe 위치 출력 */
		//playerMove(playerClient);			/* client playe 위치 제어 */

		/* server player의 HP 상태 표시, life값이 0이 되면 종료(game over) */
		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) {
			shutdown(hClntSock, SD_SEND);
			break;
		}

		Enemy_init(Enemy);		/* Enemy[] 생성 */
		
		/* Enemy 위치 및 활성화 상태(state)를 client에 보낸다. */
		sendValue = send(hClntSock, (char*)& Enemy, sizeof(Enemy), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		/* Enemy 활성화 상태(state)를 client로 부터 받는다. */
		strLen = recv(hClntSock, buf, sizeof(bool) * ENEMYNUM, 0);
		if (strLen <= 0) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		ptr = buf;
		for (int i = 0; i < ENEMYNUM; ++i) {
			memcpy(&Enemy[i].state, ptr, sizeof(Enemy[i].state));
			ptr += sizeof(Enemy[i].state);
		}
		
		enemyCount = Enemy_play(Enemy, player_bullet);		/* Enemy[] 위치 이동 및 출력, 죽은 적의 수 반환 */

		/* server player의 점수 표시, enemy 죽인 수에 비례 */
		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);	/* server player 미사일 발사 */
		playerBulletOut(player_bullet);			/* server player 미사일 위치 이동 및 출력 */

		
		/* server player 미사일 위치 및 활성화 상태(state) client에 전달 */
		sendValue = send(hClntSock, (char*)& player_bullet, sizeof(player_bullet), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		/* client player 미사일 위치 및 활성화 상태(state) 받음 */
		strLen = recv(hClntSock, buf, sizeof(Object) * PLAYERBULLETNUM, 0);
		if (strLen == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		ptr = buf;
		for (int i = 0; i < PLAYERBULLETNUM; ++i) {
			playerClient_bullet[i] = (Object*)ptr;
			ptr += sizeof(Object);
		}

		//playerBullet(playerClient, *playerClient_bullet);	/* client player 미사일 발사 */
		playerClientBulletOut(*playerClient_bullet);		/* client player 미사일 발사 */

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
	closesocket(hClntSock);
	closesocket(hServSock);

	if (mode == 2) {
		/* game over시 (life == 0) mysql에 이름 및 스코어 저장 */
		cls(hStdout);
		mysqlUser(score);
	}
	
	WSACleanup();	/* 라이브러리 해제 */
}

int main(int argc, char* argv[]) {
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);

	consoleSize(WIDTH, COLUMN);

	hideCursor();
	mode = printMenu();
	
	cls(hStdout);

	playerInit(&player, mode);

	switch (mode) {
	case 1: single(LIFE, 0); break;
	case 2: multi();
	}

	return 0;
}

void error_handling(const char* message) {
	fputs(message, stderr);
	fputc('\n', stderr);
	exit(1);
}