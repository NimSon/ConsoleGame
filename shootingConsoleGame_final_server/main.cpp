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
			printf("����ü : ��");
			Sleep(1000);
			cls(hStdout);
		}
	}
	
	int life = xLife;
	int enemyCount;
	int score = xScore;

	while (1) {
		playerOut(&player);		/* server playe ��ġ ��� */
		playerMove(&player);	/*server playe ��ġ ���� */

		/* server player�� HP ���� ǥ��, life���� 0�� �Ǹ� ����(game over) */
		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) break;

		Enemy_init(Enemy);									/* Enemy[] ���� */
		enemyCount = Enemy_play(Enemy, player_bullet);		/* Enemy[] ��ġ �̵� �� ���, ���� ���� �� ��ȯ */


		/* server player�� ���� ǥ��, enemy ���� ���� ��� */
		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);	/* server player �̻��� �߻� */
		playerBulletOut(player_bullet);			/* server player �̻��� ��ġ �̵� �� ��� */

		Bullet_init(Enemy, Enemy_bullet);	/* Enemy[] �̻��� �߻� */
		Bullet_play(Enemy, Enemy_bullet);	/* Enemy[] �̻��� ��ġ �̵� �� ��� */


		/* esc Ű�� ������ ���� */
		if (GetAsyncKeyState(VK_ESCAPE) && 0x8000) {
			cursor(WIDTH * 0.6, COLUMN * 0.7);
			system("pause");
		}

		Sleep(100);
		cls(hStdout);
	}
	/* game over�� (life == 0) mysql�� �̸� �� ���ھ� ���� */
	mysqlUser(score);
	cls(hStdout);
}

void multi() {
	WSADATA wsaData;
	SOCKET hServSock, hClntSock;		// ���� ������ ���� ����
	SOCKADDR_IN servAddr, clntAddr;		// �����ּҸ� �����ϱ� ���� ����ü

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	// WSASatrtup() : �䱸�ϴ� ������ ���� ���� �˸���, ���̺귯�� �ʱ�ȭ �۾�
		error_handling("WSAStartup() error!");

	hServSock = socket(PF_INET, SOCK_STREAM, 0);	// socket() : ���� ����
	if (hServSock == INVALID_SOCKET)				// socket() �Լ��� ���� ������ �����ϸ�(-1 ��ȯ),
		error_handling("socket() error");

	memset(&servAddr, 0, sizeof(servAddr));			// ����ü servAddr�� 0���� �ʱ�ȭ
	servAddr.sin_family = AF_INET;					// �ּ� Ÿ��
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);	// INADDR_ANY : IP ���� ���, 192. �� �ǰ� 127. ��
	servAddr.sin_port = htons(PORT);				// ������ �ְ� ���� ��Ʈ ��ȣ

	if (bind(hServSock, (SOCKADDR*)& servAddr, sizeof(servAddr)) == SOCKET_ERROR)	// bind() : server, ���Ͽ� �ּ�����(IP, port) �Ҵ�
		error_handling("bind() error");

	if (listen(hServSock, 5) == SOCKET_ERROR) // ������ ����										// listen() : ������ �����û�ޱ� ������ ���·� �����.
		error_handling("listen() error");

	cursor(WIDTH * 0.4, COLUMN * 0.5);
	printf("����� ������ ������Դϴ�..");

	int szClntAddr = sizeof(clntAddr);	// client address ũ�� ����
	hClntSock = accept(hServSock, (SOCKADDR*)& clntAddr, &szClntAddr);			// accept() : client, �����û ������, �� ��û ����
	if (hClntSock == INVALID_SOCKET)
		error_handling("accept() error");

	cls(hStdout);
	for (int i = 3; i >= 0; --i) {
		cursor(WIDTH * 0.48, COLUMN * 0.5 - 1);
		printf("GAME  START");
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 1);
		printf("     %d", i);
		cursor(WIDTH * 0.48, COLUMN * 0.5 + 3);
		printf("����ü : ��");
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
		/* client�� server player �����͸� ������. */
		sendValue = send(hClntSock, (char*)& player, sizeof(player), 0);		// ����ü �����͸� client�� ������.
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		playerOut(&player);		/* server playe ��ġ ��� */
		playerMove(&player);	/* server playe ��ġ ���� */
		
		/* client�κ��� client player �����͸� �޴´�. */
		strLen = recv(hClntSock, buf, sizeof(Object), 0);	// recv() ������ �������� ����Ʈ ���� strLen�� �����Ѵ�.
		if (strLen == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		buf[strLen] = '\0';
		playerClient = (Object*)buf;

		playerOutClient(playerClient);		/* client playe ��ġ ��� */
		//playerMove(playerClient);			/* client playe ��ġ ���� */

		/* server player�� HP ���� ǥ��, life���� 0�� �Ǹ� ����(game over) */
		life = printHpState(&player, Enemy, Enemy_bullet, life);
		if (!life) {
			shutdown(hClntSock, SD_SEND);
			break;
		}

		Enemy_init(Enemy);		/* Enemy[] ���� */
		
		/* Enemy ��ġ �� Ȱ��ȭ ����(state)�� client�� ������. */
		sendValue = send(hClntSock, (char*)& Enemy, sizeof(Enemy), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		/* Enemy Ȱ��ȭ ����(state)�� client�� ���� �޴´�. */
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
		
		enemyCount = Enemy_play(Enemy, player_bullet);		/* Enemy[] ��ġ �̵� �� ���, ���� ���� �� ��ȯ */

		/* server player�� ���� ǥ��, enemy ���� ���� ��� */
		printScore(&score, enemyCount);

		playerBullet(&player, player_bullet);	/* server player �̻��� �߻� */
		playerBulletOut(player_bullet);			/* server player �̻��� ��ġ �̵� �� ��� */

		
		/* server player �̻��� ��ġ �� Ȱ��ȭ ����(state) client�� ���� */
		sendValue = send(hClntSock, (char*)& player_bullet, sizeof(player_bullet), 0);
		if (sendValue == SOCKET_ERROR) {
			mode = 3;
			score += enemyCount * 100;
			single(life, score);
			break;
		}
		
		/* client player �̻��� ��ġ �� Ȱ��ȭ ����(state) ���� */
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

		//playerBullet(playerClient, *playerClient_bullet);	/* client player �̻��� �߻� */
		playerClientBulletOut(*playerClient_bullet);		/* client player �̻��� �߻� */

		Bullet_init(Enemy, Enemy_bullet);	/* Enemy[] �̻��� �߻� */
		Bullet_play(Enemy, Enemy_bullet);	/* Enemy[] �̻��� ��ġ �̵� �� ��� */

		/* esc Ű�� ������ ���� */
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
		/* game over�� (life == 0) mysql�� �̸� �� ���ھ� ���� */
		cls(hStdout);
		mysqlUser(score);
	}
	
	WSACleanup();	/* ���̺귯�� ���� */
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