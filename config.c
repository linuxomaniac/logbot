unsigned char parse_config(struct CONFIG *config, char *a, char *b) {
	unsigned int len_b = strlen(b);

	if(len_b == 0 || strlen(a) == 0) {
		return 1;
	}

	if(strcmp(a, "Host") == 0) {
		config->host = (char *)malloc(len_b + 1);
		memcpy(config->host, b, len_b + 1);
	} else if(strcmp(a, "Port") == 0) {
		config->port = ctoi(b);
		if(config->port == 0 || config->port > 65535) {
			return 1;
		}
	} else if(strcmp(a, "DCCHost") == 0) {
		config->dcchost = (char *)malloc(len_b + 1);
		memcpy(config->dcchost, b, len_b + 1);
	} else if(strcmp(a, "DCCPort") == 0) {
		config->dccport = ctoi(b);
		if(config->dccport == 0 || config->dccport > 65535) {
			return 1;
		}
	} else if(strcmp(a, "DCCTimeout") == 0) {
		config->dcctimeout = ctoi(b);
		if(config->dcctimeout == 0) {
			return 1;
		}
	} else if(strcmp(a, "DCCAlertDelay") == 0) {
		config->dccalertdelay = ctoi(b);
		if(config->dccalertdelay == 0) {
			return 1;
		}
	} else if(strcmp(a, "LogChat") == 0) {
		if(!strcasecmp(b, "none")) {// LogXXX=None -> désactivé
			config->logchat = (char *)malloc(len_b + 1);
			memcpy(config->logchat, b, len_b + 1);
		} else {
			config->logchat = NULL;
		}
	}  else if(strcmp(a, "LogSystem") == 0) {
		if(!strcasecmp(b, "none")) {// LogXXX=None -> désactivé
			config->logsystem = (char *)malloc(len_b + 1);
			memcpy(config->logsystem, b, len_b + 1);
		} else {
			config->logsystem = NULL;
		}
	} else if(strcmp(a, "Channel") == 0) {
		config->channel = (char *)malloc(len_b + 1);
		memcpy(config->channel, b, len_b + 1);
	} else if(strcmp(a, "Nick") == 0) {
		config->nick = (char *)malloc(len_b + 1);
		memcpy(config->nick, b, len_b + 1);
	} else if(strcmp(a, "Password") == 0) {
		config->password = (char *)malloc(len_b + 1);
		memcpy(config->password, b, len_b + 1);
	} else if(strcmp(a, "MysqlHost") == 0) {
		if(!strcasecmp(b, "none")) {// None -> désactivé
			config->mysqlhost = (char *)malloc(len_b + 1);
			memcpy(config->mysqlhost, b, len_b + 1);
		} else {
			config->mysqlhost = NULL;
		}
	} else if(strcmp(a, "MysqlDB") == 0) {
		config->mysqldb = (char *)malloc(len_b + 1);
		memcpy(config->mysqldb, b, len_b + 1);
	} else if(strcmp(a, "MysqlUser") == 0) {
		config->mysqluser = (char *)malloc(len_b + 1);
		memcpy(config->mysqluser, b, len_b + 1);
	} else if(strcmp(a, "MysqlPassword") == 0) {
		config->mysqlpassword = (char *)malloc(len_b + 1);
		memcpy(config->mysqlpassword, b, len_b + 1);
	} else {
		return 1;
	}

	return 0;
}

void init_config(struct CONFIG *config, char *configfile) {
	FILE *file;
	char tmp[BUFSIZE], *line, *arg;
	unsigned int len, count = 0, num_params = 0;
	unsigned short res;

	file = fopen(configfile, "rb");
	if(file == NULL) {
		printf("FATAL: configuration file \"%s\" not found!\n", configfile);
		exit(1);
	}

	while(fgets(tmp, BUFSIZE, file) != NULL) {
		count++;

		if(tmp[0] == 35) {// 35 c'est pour #
			continue;
		}

		// Tout ce merdier pour virer les caractères inutiles en fin de ligne (32 = espace, 13 & 10 = CRLF)
		for(len = strlen(tmp); len > 0 && (tmp[len - 1] == 32 || tmp[len - 1] == 13 || tmp[len - 1] == 10 || tmp[len - 1] == 0); len--) {}

		if(len == 0)
			continue;

		line = (char *)malloc(len + 1);
		memcpy(line, tmp, len);
		line[len] = '\0';

		arg = strsplit(line, '=');

		res = parse_config(config, line, arg);

		free(arg);
		free(line);

		if(res) {
			break;
		} else {
			num_params++;
		}
	}

	fclose(file);

	if(res) {
		printf("FATAL: the line number %d is invalid in \"%s\"!\n", count, configfile);
		exit(1);
	}

	if(num_params != 15) {
		printf("FATAL: one or more missing parameter(s) in the config file \"%s\"!\n", configfile);
		exit(1);
	}
}

void clear_config(struct CONFIG config) {
	free(config.host);
	free(config.dcchost);
	if(config.logchat != NULL) {
		free(config.logchat);
	}
	if(config.logsystem != NULL) {
		free(config.logsystem);
	}
	free(config.channel);
	free(config.nick);
	free(config.password);
	if(config.mysqlhost != NULL) {
		free(config.mysqlhost);
	}
	free(config.mysqldb);
	free(config.mysqluser);
	free(config.mysqlpassword);
}
