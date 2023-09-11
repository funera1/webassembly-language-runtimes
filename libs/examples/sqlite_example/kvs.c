#include <sqlite3.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static char *USAGE_FMT =
"Usage: %s DB_FILE\n"
"   DB_FILE always gets overwritten with a database with basic 'Sample' table.\n"
"\n"
"Program exits on any system error!\n"
"\n";

int insert_sample_data(char *db_file);
int read_sample_data(char *db_file);
int insert(int key, int val, sqlite3 *db);
int select(int key, sqlite3 *db);

int file_exists(char *file_path)
{
  struct stat buff;
  return (stat(file_path, &buff) == 0);
}

int main(int argc, char **argv)
{
  if (argc != 2 || 0 == strcmp(argv[1], "--help") || 0 == strcmp(argv[1], "-h"))
  {
    fprintf(stderr, USAGE_FMT, argv[0], argv[0]);
    return -1;
  }

  printf("1. Running with SQLite version %s\n", sqlite3_libversion());

  char *db_file = argv[1];
  printf("2. Using db file %s\n", db_file);
  if (file_exists(db_file))
  {
    printf("File '%s' exists. Removing...\n", db_file);
    int remove_result = remove(db_file);
    if (remove_result)
    {
      perror(db_file);
      return -1;
    }
  }

  // TABLE作成
  sqlite3 *db;
  if (sqlite3_open(db_file, &db) != SQLITE_OK)
  {
    fprintf(stderr, "Failed to open db in '%s' with error: %s\n", db_file, sqlite3_errmsg(db));
    sqlite3_close(db);
    return -1;
  }

  char *err_msg = NULL;
  int err = sqlite3_exec(db, 
      "CREATE TABLE IF NOT EXISTS Sample"
      "(key TEXT, value TEXT);",
      NULL, NULL, &err_msg);
  if (err != SQLITE_OK) {
    printf("%s\n", err_msg);
    return -1;
  }
  printf("CREATE TABLE!\n");


  // ループでsetとgetを選択する
  while (1) {
    // set: 0, get: 1, exit: other
    printf("\x1b[32m[+] Input 0(set) or 1(get) or 2(migration) or other\n\x1b[m");

    int command = 0;
    char input_str[100];
    scanf("%s", input_str);

    printf("input is %s\n", input_str);

    int invalid_input = 0;
    // 文字列で受け取ったものを整数型へ変換
    for (int i = 0; i < strlen(input_str); i++) {
      char c = input_str[i];
      if ('0' <= c && c <= '9') {
        command = 10 * command + (int)(c - '0');
      }
      else {
        printf("入力がおかしいです\n");
        invalid_input = 1;
        break;
      }
    }
    // 入力がおかしければcontinue
    if (invalid_input) {
      continue;
    }

    switch (command) {
      int key, val;
      case 0:
        printf("\x1b[32m[+] SET MODE: Please input 'key', 'value'\n\x1b[m");
        scanf("%d, %d", &key, &val);
        if (insert(key, val, db) != 0) {
          printf("\x1b[31m");
          printf("[ERROR] failed to insert\n");
          printf("\x1b[m");

          sqlite3_close(db);
          return -1;
        }
        break;
      case 1:
        printf("\x1b[32m[+] GET MODE: Please input 'key'\n\x1b[m");
        scanf("%d", &key);
        if (select(key, db) != 0) {
          printf("\x1b[31m");
          printf("[ERROR] failed to select\n");
          printf("\x1b[m");

          sqlite3_close(db);
          return -1;
        }
        break;
      case 2:
        sqlite3_close(db);

        int sleep_seccond = 3;
        printf("\x1b[32m[+] MIGRATION MODE: Sleep(%ds)\n\x1b[m", sleep_seccond);
        sleep(sleep_seccond);

        sqlite3_open(db_file, &db);
        break;
      default:
        printf("Exit\n");
        sqlite3_close(db);
        return 0;
    }
  }

  return 0;
}

int Error(int rc, char* msg) {
    printf("\x1b[31m");
    printf("[ERROR] %s\n", msg);
    printf("[ERROR] errmsg is %s\n", sqlite3_errstr(rc));
    printf("\x1b[m");
    return -1;
}

int insert(int key, int val, sqlite3 *db) {
  char *sql_command = 
    "INSERT INTO Sample (key, value) VALUES (?, ?);";
  sqlite3_stmt* pStmt;

  // prepare
  int status = sqlite3_prepare_v2(db, sql_command, -1, &pStmt, NULL);
  if (status != SQLITE_OK) {
    // printf("[ERROR] failed to sqlite3_prepare_v2.\n");
    return Error(status, "failed to sqlite3_prepare_v2.");
  }

  // bind
  sqlite3_bind_int(pStmt, 1, key);
  sqlite3_bind_int(pStmt, 2, val);

  // SQL statementを実行
  do {
    status = sqlite3_step(pStmt);
  } while(status == SQLITE_BUSY);
  
  if (status != SQLITE_DONE) {
    return Error(status, "failed to execute sql statement");
  }

  // destruct
  sqlite3_reset(pStmt);
  sqlite3_clear_bindings(pStmt);

  sqlite3_finalize(pStmt);

  return 0;
}

int select(int key, sqlite3 *db) {
  char *sql_command = 
    "SELECT value FROM Sample WHERE key = ?";
  sqlite3_stmt* pStmt;

  // prepare
  int status = sqlite3_prepare_v2(db, sql_command, -1, &pStmt, NULL);
  if (status != SQLITE_OK) {
    return Error(status, "failed to sqlite3_prepare_v2.");
  }

  // bind
  sqlite3_bind_int(pStmt, 1, key);

  // SQL statementを実行
  do {
    status = sqlite3_step(pStmt);
    if (status != SQLITE_ROW) break;

    int value = sqlite3_column_int(pStmt, 0);
    printf("{key, value} = {%d, %d}\n", key, value);
  } while(status == SQLITE_ROW);

  if (status != SQLITE_DONE) {
    return Error(status, "failed to execute sql statement");
  }

  // destruct
  sqlite3_reset(pStmt);
  sqlite3_clear_bindings(pStmt);

  sqlite3_finalize(pStmt);

  return 0;
}
