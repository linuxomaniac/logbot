// Pour C99
typedef unsigned int uint;
typedef unsigned long ulong;

#include <my_global.h>
#include <mysql.h>

bool my_mysql_init(MYSQL *mysql, struct CONFIG config);
bool my_mysql_query(MYSQL *mysql, struct CONFIG config, char *query, char **res);

#include "mysql.c"