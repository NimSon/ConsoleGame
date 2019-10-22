#pragma once
#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<time.h>
#include<Windows.h>
#include<mysql.h>

#define WIDTH 100		// 가로
#define COLUMN 30		// 세로
#define PLAYERMOVE 1
#define ENERMYMOVE 1
#define MISSILERANGE 3
#define YRANGE 2
#define PLAYERBULLETNUM 10
#define ENEMYNUM 20
#define LIFE 3

struct Object
{
	int x;
	int y;
	bool state;
	bool trigger;
};

// player
void playerInit(Object* playerO);
void playerMove(Object* playerO);
void playerOut(Object* playerO);
void playerOutClient(Object* playerO);
void playerBullet(Object* playerO, Object* playerBulletO);
void playerBulletOut(Object* playerBulletO);
void playerClientBulletOut(Object* playerBulletO);

// enemy
void Enemy_init(Object* oEnemy);
int Enemy_play(Object* oEnemy, Object* oPlayerBullet);
void Bullet_init(Object* oEnemy, Object* oEnemyBullet);
void Bullet_play(Object* oEnemy, Object* oEnemyBullet);

// console
void consoleSize(int width, int column);
void cursor(int pos_x, int pos_y);
void hideCursor();
void cls(HANDLE hConsole);

// print
void printMenu();
void printRank();
void gameover();
int printHpState(Object* playerO, Object* enemyO, Object* enemyBulletO, int life);
void printScore(int* score, int enemyCount);

// MySQL
void mysqlUser(int score);
void printMysqlData();
