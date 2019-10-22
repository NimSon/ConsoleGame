#include"header.h"

void Enemy_init(Object* oEnemy) {
	for (int i = 0; i < ENEMYNUM; ++i) {
		if (!oEnemy[i].state) {
			oEnemy[i].x = i * 5;	//적기 생성 좌표
			oEnemy[i].y = 2;
			oEnemy[i].state = true;
		}
	}
}

int Enemy_play(Object* oEnemy, Object* oPlayerBullet) {
	int enemyCount = 0;
	for (int i = 0; i < ENEMYNUM; ++i) {
		if (oEnemy[i].state) {
			cursor(oEnemy[i].x, oEnemy[i].y);
			printf(">0<");
			for (int j = 0; j < PLAYERBULLETNUM; ++j) {
				if (oPlayerBullet[j].state && oEnemy[i].y <= oPlayerBullet[j].y && oEnemy[i].y > oPlayerBullet[j].y - YRANGE) {
					if (oEnemy[i].x >= oPlayerBullet[j].x && oEnemy[i].x < oPlayerBullet[j].x + MISSILERANGE) {
						oPlayerBullet[j].state = false;
						oEnemy[i].state = false;
						if (oEnemy[i].state == false)
							++enemyCount;
					}
				}
			}

			if (!oEnemy[i].trigger) oEnemy[i].x += ENERMYMOVE;
			else oEnemy[i].x -= ENERMYMOVE;

			if (oEnemy[i].x > WIDTH - 3) {		//우측 벽에 닿으면 좌로 이동
				oEnemy[i].x -= ENERMYMOVE;
				oEnemy[i].y += ENERMYMOVE;
				oEnemy[i].trigger = true;
			}
			else if (oEnemy[i].x < 0) {			//좌측 벽에 닿으면 우로 이동
				oEnemy[i].x += ENERMYMOVE;
				oEnemy[i].y += ENERMYMOVE;
				oEnemy[i].trigger = false;
			}
		}
	}
	return enemyCount;
}

void Bullet_init(Object* oEnemy, Object* oEnemyBullet) {
	srand(time(NULL));
	for (int i = 0; i < ENEMYNUM; ++i) {
		if (rand() % 10 < 5) {
			if (!oEnemyBullet[i].state) {			//적기의 총알이 발사되지 않은 경우
				oEnemyBullet[i].state = true;		// 총알이 발사 됐다고 명시해줌
				oEnemyBullet[i].x = oEnemy[i].x;
				oEnemyBullet[i].y = oEnemy[i].y + 1;
				break;
			}
		}
	}
}

void Bullet_play(Object* oEnemy, Object* oEnemyBullet) {
	for (int i = 0; i < ENEMYNUM; ++i) {
		if (oEnemyBullet[i].state) {
			cursor(oEnemyBullet[i].x, oEnemyBullet[i].y);
			printf("*");

			oEnemyBullet[i].y += ENERMYMOVE;

			if (oEnemyBullet[i].y > COLUMN - 2)
				oEnemyBullet[i].state = false;
		}
	}
}