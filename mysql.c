bool my_mysql_init(MYSQL *mysql, struct CONFIG config) {
	mysql_init(mysql);
	mysql_options(mysql, MYSQL_READ_DEFAULT_GROUP, "option");

	if(!mysql_real_connect(mysql, config.mysqlhost, config.mysqluser, config.mysqlpassword, config.mysqldb, 0, NULL, 0)) {
		mysql_close(mysql);
		logger(config.logsystem, "WARN: MySQL connection failed!");
		return false;
	}
	return true;
}

bool my_mysql_query(MYSQL *mysql, struct CONFIG config, char *query, char **res) {
	MYSQL_RES *result;
	MYSQL_ROW row;
	unsigned int len;
	*res = NULL;

	if(mysql_commit(mysql) != 0) {
		logger(config.logsystem, "WARN; not connected to MySQL! Trying to reconnect...");
		if(!my_mysql_init(mysql, config)) {
			return false;
		}
	}

	if(mysql_query(mysql, query) != 0) {
		return false;
	}

	result = mysql_use_result(mysql);
	if(result == NULL) {
		// Le résultat est nul si on fait une requête du genre UPDATE ou INSERT
		return true;
	}

	// On ne prend que le premier résultat (de toute façon, les requêtes dans ce programmes n'ont pas plusieurs résultats
	row = mysql_fetch_row(result);
	if(row != NULL) {// row n'est nul que sur les requêtes SELECT s'il n'y a rien
		len = strlen(row[0]);
		*res = malloc(len + 1);
		memcpy(*res, row[0], len + 1);
	}

	mysql_free_result(result);

	mysql_commit(mysql);

	return true;
}