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
    printf("\x1b[32m[+] Input 0(set) or 1(get) or 2(migration) or other\n");

    int command;
    scanf("%d", &command);

    switch (command) {
      int key, val;
      case 0:
        printf("\x1b[32m[+] Set Mode: Please input 'key', 'value'\x1b[m\n");
        scanf("%d, %d", &key, &val);
        insert(key, val, db);
        break;
      case 1:
        printf("\x1b[32m[+] Get Mode: Please input 'key'\x1b[m\n");
        scanf("%d", &key);
        select(key, db);
        break;
      case 2:
        printf("\x1b[32m[+] Migration Mode: sleep(10s)\x1b[m\n");
        sleep(10);
        printf("\x1b[32m[+] End sleep\x1b[m\n");
        break;
      default:
        printf("Exit\n");
        sqlite3_close(db);
        return 0;
    }
  }

  return 0;
}

int insert(int key, int val, sqlite3 *db) {
  char *sql_command = 
    "INSERT INTO Sample (key, value) VALUES (?, ?);";
  sqlite3_stmt* pStmt;
  sqlite3_prepare_v2(db, sql_command, -1, &pStmt, NULL);

  sqlite3_bind_int(pStmt, 1, key);
  sqlite3_bind_int(pStmt, 2, val);

  while(sqlite3_step(pStmt) == SQLITE_BUSY) {}
  sqlite3_reset(pStmt);
  sqlite3_clear_bindings(pStmt);

  sqlite3_finalize(pStmt);

  return 0;
}

int select(int key, sqlite3 *db) {
  char *sql_command = 
    "SELECT value FROM Sample WHERE key = ?";
  sqlite3_stmt* pStmt;
  sqlite3_prepare_v2(db, sql_command, -1, &pStmt, NULL);

  sqlite3_bind_int(pStmt, 1, key);

  while(sqlite3_step(pStmt) == SQLITE_ROW) {
    int value = sqlite3_column_int(pStmt, 0);
    printf("{key, value} = {%d, %d}\n", key, value);
  }
  sqlite3_reset(pStmt);
  sqlite3_clear_bindings(pStmt);

  sqlite3_finalize(pStmt);

  return 0;
}


// Note: While simple to read, the callback approach is deprecated!
int read_lines_cb(void *linePtr, int columns, char **values, char **names)
{
  int *line = (int *)linePtr;
  // Print headers on first line
  if (*line == 0)
  {
    for (int i = 0; i < columns; ++i)
      printf("|%-20s", names[i]);

    printf("|\n");

    for (int i = 0; i < columns; ++i)
      printf("+====================");

    printf("+\n");
  }

  (*line)++;

  // Print values
  for (int i = 0; i < columns; ++i)
    printf("|%-20s", values[i] ? values[i] : "NULL");

  printf("|\n");
  return 0;
}

int read_sample_data(char *db_file)
{
  sqlite3 *db;
  if (sqlite3_open(db_file, &db) != SQLITE_OK)
  {
    fprintf(stderr, "Failed to open db in '%s' with error: %s\n", db_file, sqlite3_errmsg(db));
    sqlite3_close(db);
    return -1;
  }

  char *sql_command = "SELECT * FROM Sample";
  char *error_message = NULL;
  int line_number = 0;
  if (sqlite3_exec(db, sql_command, read_lines_cb, &line_number, &error_message) != SQLITE_OK)
  {
    fprintf(stderr, "SQL execution failed with error: %s\n", error_message);

    sqlite3_free(error_message);
    sqlite3_close(db);

    return -1;
  }
  printf("%d total record(s).\n", line_number);

  sqlite3_close(db);
  return 0;
}
