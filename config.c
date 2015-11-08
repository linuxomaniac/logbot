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
	} else if(strcmp(a, "AliasRandomLine") == 0) {
		config->aliasrandomline = (char *)malloc(len_b + 1);
		memcpy(config->aliasrandomline, b, len_b + 1);
	} else if(strcmp(a, "Alias") == 0) {
		struct ALIAS *alias, *alias_loop;
		unsigned int i;

		alias = (struct ALIAS *)malloc(sizeof(struct ALIAS));

		for(i = 0; i < len_b && b[i] != '|'; i++) {}

		alias->trigger = (char *)malloc(i + 1);
		memcpy(alias->trigger, b, i);
		alias->trigger[i] = '\0';

		alias->message = (char *)malloc(len_b - i);
		memcpy(alias->message, b + i + 1, len_b - i - 1);
		alias->message[len_b - i - 1] = '\0';

		alias->next = NULL;

		if(config->alias != NULL) {
			alias_loop = config->alias;
			while(1) {
				if(alias_loop->next == NULL) {
					alias_loop->next = alias;
					break;
				}
				alias_loop = alias_loop->next;	
			}
		} else {
			config->alias = alias;
		}

		return 2;
	} else {
		return 1;
	}

	return 0;
}

void init_config(struct CONFIG *config, char *configfile) {
	FILE *file;
	char tmp[BUFSIZE], *arg;
	unsigned int len, count = 0, num_params = 0;
	unsigned short res = 0;

	file = fopen(configfile, "rb");
	if(file == NULL) {
		printf("FATAL: configuration file \"%s\" not found!\n", configfile);
		exit(1);
	}

	config->alias = NULL;// On n'est pas obligé d'avoir des lignes d'aliases

	while(fgets(tmp, BUFSIZE, file) != NULL) {
		count++;

		if(tmp[0] == 35) {// 35 c'est pour #
			continue;
		}

		// Tout ce merdier pour virer les caractères inutiles en fin de ligne (32 = espace, 13 & 10 = CRLF)
		for(len = strlen(tmp); len > 0 && (tmp[len - 1] == 32 || tmp[len - 1] == 13 || tmp[len - 1] == 10 || tmp[len - 1] == 0); len--) {}

		if(len == 0) {
			continue;
		}

		tmp[len] = '\0';

		arg = strsplit(tmp, '=');

		res = parse_config(config, tmp, arg);

		free(arg);

		if(res == 1) {
			break;
		} else if(res == 0) {
			num_params++;
		}// pour res == 2, ce sont les aliases, donc on n'incrémente pas, parce que leur nombre ne peut pas être prédéfini
	}

	fclose(file);

	if(res == 1) {
		printf("FATAL: the line number %d is invalid in \"%s\"!\n", count, configfile);
		exit(1);
	}

	if(num_params != 16) {
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

	free(config.aliasrandomline);

	if(config.alias != NULL) {
		struct ALIAS *alias = config.alias;

		while(alias != NULL) {
			free(alias->trigger);
			free(alias->message);

			alias = alias->next;
		}
	}
}
