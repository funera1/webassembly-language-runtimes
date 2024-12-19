#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

int main(int argc, char **argv)
{

  // TABLE作成
  sqlite3 *db;
  char* db_file = "test/test.db";
  sqlite3_open(db_file, &db);
  sqlite3_close(db);
  // 
  // char *err_msg = NULL;
  // int err = sqlite3_exec(db, 
  //     "CREATE TABLE IF NOT EXISTS Sample"
  //     "(key TEXT, value TEXT);",
  //     NULL, NULL, &err_msg);
  // printf("CREATE TABLE!\n");
  // return 0;
}

