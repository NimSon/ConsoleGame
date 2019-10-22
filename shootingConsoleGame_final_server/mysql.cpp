#include"header.h"

extern HANDLE hStdout;

/* MySQL에 사용자 이름 및 score 저장 */
void mysqlUser(int score) {

	MYSQL* sql = mysql_init(NULL);
	mysql_real_connect(sql, "192.168.0.9", "sn", "1234", "userinfo", 0, NULL, 0);

	cls(hStdout);
	gameover();
	cursor(WIDTH * 0.45, COLUMN * 0.6);
	char name[10];
	printf("user name : ");
	scanf("%s", &name);			// 커서 위치 수정

	char query[100];
	sprintf_s(query, "INSERT INTO user(name, score) VALUES ('%s', '%d')", name, score);
	mysql_query(sql, query);

	mysql_close(sql);
}

/* 1~10위 까지의 순위 보여줌 */
void printMysqlData() {
	cls(hStdout);

	MYSQL* sql = mysql_init(NULL);
	mysql_real_connect(sql, "192.168.0.9", "sn", "1234", "userinfo", 0, NULL, 0);

	mysql_query(sql, "set names euckr");

	mysql_query(sql, "SELECT * FROM user ORDER BY score desc LIMIT 10");
	MYSQL_RES* result = mysql_store_result(sql);
	int count = mysql_num_fields(result);

	int yCursor = 3;
	int rankCount = 0;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(result)) {
		++rankCount;
		cursor(40, yCursor += 2);
		printf("%2d. ", rankCount);
		for (int i = 0; i < count; ++i) {
			printf("%s ", row[i] ? row[i] : NULL);
		}
	}

	mysql_free_result(result);
	mysql_close(sql);
}