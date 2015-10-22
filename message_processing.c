bool get_random_line_file(char *fname, unsigned int maxsize, char **line) {
	FILE *f;
	unsigned long lines;

	srand(time(NULL));

	f = fopen(fname, "rb");
	if(f == NULL) {
		return false;
	}

	fseek(f, 0, SEEK_END);
	lines = ftell(f);
	
	fseek(f, rand() % lines, SEEK_SET);

	if(fgets(*line, maxsize, f) != NULL) {
		if(fgets(*line, maxsize, f) != NULL) {
			return true;
		}
	}

	return false;
}

int privmsg(int sock, char *target, char *message) {
	char *tpl, *buf;
	unsigned int len;
	int t;

	tpl = "PRIVMSG %s :%s\r\n";
	len = strlen(target) + strlen(message) + strlen(tpl) - 4;
	buf = (char *)malloc(len + 1);
	sprintf(buf, tpl, target, message);
	t = send(sock, buf, len, 0);
	free(buf);
	return t;
}

int say(int sock, struct CONFIG *config, char *message) {
	int t;
	char *buf, *tpl;

	t = privmsg(sock, config->channel, message);

	tpl = "<%s> %s";
	buf = (char *)malloc(strlen(tpl) + strlen(config->nick) + strlen(message) - 3);
	sprintf(buf, tpl, config->nick, message);
	logger(config->logchat, buf);
	free(buf);	

	return t;
}

int join(int sock, char *channel) {
	char *tpl, *buf;
	unsigned int len;
	int t;

	tpl = "JOIN %s\r\n";// 20 JOIN !!!!!!!!!!!!!!
	len = strlen(tpl) + strlen(channel) - 2;
	buf = (char *)malloc(len + 1);
	sprintf(buf, tpl, channel);
	t = send(sock, buf, len, 0);
	free(buf);

	return t;
}

void mysql_user_quit(struct CONFIG *config, MYSQL *mysql, char *source) {
	char *tpl, *buf, *tmp;
	bool t;
	FILE *f;
	long fsize;

	tpl = "SELECT length from logs WHERE user=LOWER('%s')";
	buf = (char *)malloc(strlen(tpl) + strlen(source) - 1);
	sprintf(buf, tpl, source);
	t = my_mysql_query(mysql, *config, buf, &tmp);
	free(buf);

	if(t && tmp != NULL) {
		free(tmp);

		f = fopen(config->logchat, "rb");
		fseek(f, 0, SEEK_END);// Aller à la fin du fichier
		fsize = ftell(f);
		fclose(f);

		tpl = "UPDATE logs set length=%ld WHERE user=LOWER('%s')";
		buf = (char *)malloc(strlen(tpl) + longlen(fsize) + strlen(source) - 4);
		sprintf(buf, tpl, fsize, source);
		my_mysql_query(mysql, *config, buf, &tmp);// tmp non malloc
		free(buf);
	}
}

void react_to_message(int sock, struct CONFIG *config, struct MESSAGE *message) {// On passe la struct message au cas où o nvoufrait utiliser le nom de l'expéditeur
	if(strstartswith(message->args, "!tgnon")) {
		say(sock, config, "Au début delthas a dit pourquoi pas, et ensuite il a dit pourquoi pas. Après il a dit après.");
	} else if(strstartswith(message->args, "!tg")) {
		say(sock, config, "Il suffit.");
	} else if(strstartswith(message->args, "!allez")) {
		say(sock, config, "Je prends la main.");
	} else if(strstartswith(message->args, "!nomejidésabusé")) {
		say(sock, config, "Non, mais j'étais véritablement surpris !");
	} else if(strstartswith(message->args, "!nomejicritique")) {
		say(sock, config, "cnul");
	} else if(strstartswith(message->args, "!génial")) {
		say(sock, config, "Je prends mes affaires !");
	} else if(strstartswith(message->args, "!emersion")) {
		say(sock, config, "Quoi encore, delthas ?");
	} else if(strstartswith(message->args, "!delthas")) {
		say(sock, config, "pffff");
	} else if(strstartswith(message->args, "!yves")) {
		char *line;

		line = (char *)malloc(512);
		if(get_random_line_file(config->logchat, 512, &line)) {
			strstrip(line);
			say(sock, config, line);
		} else {
			say(sock, config, "C'est non.");
		}

		free(line);
	} else if(strstartswith(message->args, "!help")) {
		say(sock, config, "Commandes disponibles : !tg, !tgnon, !allez, !yves, !nomejidésabusé, !nomejicritique, !emersion, !delthas, !help.");
	}

}

bool process_message(int sock, struct CONFIG *config, MYSQL *mysql, struct MESSAGE *message) {
	char *tpl, *buf, *tmp;
	unsigned int len;
	bool t;

	if(strcmp(message->type, "PRIVMSG") == 0) {
		if(strcasecmp(config->channel, message->target)) {
			if(strstartswith(message->args, "\001ACTION")) {
				tmp = strsplit(message->args, ' ');
				len = strlen(tmp) - 1;
				tmp[len] = '\0';// Pour virer le \001 final

				tpl = "* %s %s";
				buf = (char *)malloc(len + strlen(message->source) + strlen(tpl) - 3);
				sprintf(buf, tpl, message->source, tmp);
				logger(config->logchat, buf);
				free(buf);
				free(tmp);
			} else {
				tpl = "<%s> %s";
				buf = (char *)malloc(strlen(tpl) + strlen(message->source) + strlen(message->args) - 3);
				sprintf(buf, tpl, message->source, message->args);
				logger(config->logchat, buf);
				free(buf);

				if(strstartswith(message->args, "!")) {
					react_to_message(sock, config, message);
				}
			}
		} else if(strcasecmp(config->nick, message->target)) {// Message privé
			if(message->args[0] == '\001') {
				if(strcmp(message->args, "\001VERSION\001") == 0) {
					tpl = "NOTICE %s :\001VERSION LogBot : un bot de logs qui te baise. Programmé en C avec la teub. version LARGEMENT AU-DELÀ DES 9000 !\001\r\n";
					len = strlen(message->source) + strlen(tpl) - 2;
					buf = (char *)malloc(len + 1);
					sprintf(buf, tpl, message->source);
					send(sock, buf, len, 0);
					free(buf);
				} else if(strstartswith(message->args, "\001PING")) {
					tpl = "NOTICE %s :%s\r\n";
					len = strlen(tpl) + strlen(message->args) + strlen(message->source) - 4;
					buf = (char *)malloc(len + 1);
					sprintf(buf, tpl, message->source, message->args);
					send(sock, buf, len, 0);
					free(buf);
				}
			} else {
				tmp = strsplit(message->args, ' ');// tmp est toute la merde, message->args contient la message->argse
				free(tmp);
				strtolower(message->args);

				if(strcmp(message->args, "logoui") == 0 && config->mysqlhost != NULL) {
					tpl = "SELECT length FROM logs WHERE user=LOWER('%s')";
					buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
					sprintf(buf, tpl, message->source);
					t = my_mysql_query(mysql, *config, buf, &tmp);
					free(buf);

					if(t) {
						if(tmp == NULL) {// Si on n'a pas trouvé de résultat
							tpl = "INSERT INTO logs VALUES (LOWER('%s'), 0)";
							buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
							sprintf(buf, tpl, message->source);
							t = my_mysql_query(mysql, *config, buf, &tmp);// Normalement, tmp n'est pas alloc
							free(buf);

							if(t) {
								privmsg(sock, message->source, "Souscription bien réalisée !");
							} else {
								privmsg(sock, message->source, "Erreur SQL !");
							}

						} else {
							free(tmp);
							privmsg(sock, message->source, "Erreur, tu es déjà inscrit !");
						}
					} else {
						privmsg(sock, message->source, "Erreur SQL !");
					}
				} else if(strcmp(message->args, "lognon") == 0 && config->mysqlhost != NULL) {
					tpl = "SELECT length FROM logs WHERE user=LOWER('%s')";
					buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
					sprintf(buf, tpl, message->source);
					t = my_mysql_query(mysql, *config, buf, &tmp);
					free(buf);

					if(t) {
						if(tmp != NULL) {// Si on a bien trouvé un résultat
							free(tmp);

							tpl = "DELETE FROM logs WHERE user=LOWER('%s')";
							buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
							sprintf(buf, tpl, message->source);
							t = my_mysql_query(mysql, *config, buf, &tmp);// Normalement, tmp n'est pas alloc
							free(buf);

							if(t) {
								privmsg(sock, message->source, "Souscription bien annulée !");
							} else {
								privmsg(sock, message->source, "Erreur SQL !");
							}

						} else {
							privmsg(sock, message->source, "Erreur, tu n'es pas inscrit !");
						}
					} else {
						privmsg(sock, message->source, "Erreur SQL !");
					}
				} else if(strcmp(message->args, "help") == 0) {
					privmsg(sock, message->source, "Commandes disponibles :");
					if(config->mysqlhost != NULL) {
						privmsg(sock, message->source, "- help : afficher l'aide ;");
						privmsg(sock, message->source, "- logoui : recevoir à la connexion les messages envoyés pendant la non-connexion ;");
						privmsg(sock, message->source, "- lognon : ne plus recevoir les messages lors de l'absence.");
					} else {
						privmsg(sock, message->source, "à part \"help\", les autres commandes sont désactivées car MySQL n'est pas activé !");
					}
				} else {
					privmsg(sock, message->source, "Commande inconnue ; \"help\" pour l'aide !");
				}
			}
		}
	} else if(strcmp(message->type, "JOIN") == 0) {// Plus de vingt par jour !
		if(strcasecmp(message->source, config->nick)) {// C'est nous qui venons de nous connecter
			return true;
		}

		tpl = "%s (%s) s'est connecté !";
		buf = (char *)malloc(strlen(tpl) + strlen(message->fullsource) + strlen(message->source) - 3);
		sprintf(buf, tpl, message->source, message->fullsource);
		logger(config->logchat, buf);
		free(buf);

		if(config->mysqlhost != NULL) {
			tpl = "SELECT length from logs WHERE user=LOWER('%s')";
			buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
			sprintf(buf, tpl, message->source);
			t = my_mysql_query(mysql, *config, buf, &tmp);
			free(buf);

			if(t && tmp != NULL) {
				len = ctoi(tmp);// On réutilise len
				if(len != 0) {
					FILE *f;
					long fsize;
					pthread_t dcc_thread;
					pthread_attr_t thread_attr;
					struct DCC_THREAD_ARGS *thread_args;

					f = fopen(config->logchat, "rb");
					fseek(f, len, SEEK_SET);// Aller du début à la longueur
					fsize = ftell(f);
					buf = (char *)malloc(fsize + 1);// Très sale ! On fout tout le fichier dans une variable !
					fsize = fread(buf, 1, fsize, f);
					fclose(f);
					buf[fsize] = '\0';

					thread_args = (struct DCC_THREAD_ARGS *)malloc(sizeof(struct DCC_THREAD_ARGS));
					thread_args->sock = sock;
					thread_args->source = strdup(message->source);// On duplique le char, car il risque d'être free comme ça
					thread_args->config = *config;
					thread_args->data = buf;
					pthread_attr_init(&thread_attr);
					pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_DETACHED);// Évite quelques erreurs Valgrind
					logger(config->logsystem, "INFO: creating the thread to send the logs.");

					if(pthread_create(&dcc_thread, &thread_attr, dcc_send_thread, thread_args) != 0) {
						logger(config->logsystem, "WARN: unable to create thread!");
					}

					// On free pas buf, ce sera fait dans le thread
				}
				free(tmp);

				tpl = "UPDATE logs set length=0 WHERE user=LOWER('%s')";
				buf = (char *)malloc(strlen(tpl) + strlen(message->source) - 1);
				sprintf(buf, tpl, message->source);
				my_mysql_query(mysql, *config, buf, &tmp);// Normalement, tmp n'est pas alloc
				free(buf);
			}
		}

	} else if(strcmp(message->type, "PART") == 0 || strcmp(message->type, "QUIT") == 0) {
		if(config->mysqlhost != NULL) {
			// On a besoin de mettre à jour les infos pour les logs quand l'utilisateur se barre, mais aussi quand il est kické, donc on fait une fonction
			mysql_user_quit(config, mysql, message->source);
		}

		tpl = "%s est parti (raison : %s) !";
		buf = (char *)malloc(strlen(tpl) + strlen(message->source) + strlen(message->args) - 3);
		sprintf(buf, tpl, message->source, message->args);
		logger(config->logchat, buf);
		free(buf);
	} else if(strcmp(message->type, "NICK") == 0) {
		tpl = "%s est maintenant %s !";
		buf = (char *)malloc(strlen(tpl) + strlen(message->source) + strlen(message->target) - 3);
		sprintf(buf, tpl, message->source, message->target);
		logger(config->logchat, buf);
		free(buf);
	} else if(strcmp(message->type, "433") == 0) {// Nick in use
		_strcat(&(config->nick), "_");
		tpl = "NICK %s \r\n";
		len = strlen(config->nick) + strlen(tpl) - 2;
		buf = (char *)malloc(len + 1);
		sprintf(buf, tpl, config->nick);
		send(sock, buf, len, 0);
		free(buf);
	} else if(strcmp(message->type, "376") == 0) {// End of MOTD
		// Fin des messages de bienvenue, on peut se connecter et rejoindre la channel
		tpl = "IDENTIFY %s";
		buf = (char *)malloc(strlen(tpl) + strlen(config->password) - 1);
		sprintf(buf, tpl, config->password);
		privmsg(sock, "NickServ", buf);
		free(buf);

		join(sock, config->channel);
	} else if(strcmp(message->type, "KICK") == 0) {
		bool t;

		tmp = strsplit(message->args, ' ');
		strlstrip(tmp);

		t = strcasecmp(message->args, config->nick);// On vérifie que c'est pas nous

		if(!t && config->mysqlhost != NULL) {// Un utilisateur s'est fait kicker
			mysql_user_quit(config, mysql, message->source);
		}

		tpl = "%s a été kické par %s (raison : %s) !";
		buf = (char *)malloc(strlen(tpl) + strlen(message->args) + strlen(message->source) + strlen(tmp) - 5);
		sprintf(buf, tpl, message->args, message->source, tmp);
		logger(config->logchat, buf);
		free(buf);
		free(tmp);

		if(t) {
			logger(config->logchat, "--- Kické, on se reconnecte ---");
			join(sock, config->channel);
		}
	} else if(strcmp(message->type, "TOPIC") == 0) {
		tpl = "%s a défini le sujet en : %s";
		buf = (char *)malloc(strlen(tpl) + strlen(message->source) + strlen(message->args) - 3);
		sprintf(buf, tpl, message->source, message->args);
		logger(config->logchat, buf);
		free(buf);
	} else if(strcmp(message->type, "332") == 0) {// Current Topic
		tmp = strsplit(message->args, ' ');
		strlstrip(tmp);
		tpl = "Topic de %s : %s";
		buf = (char *)malloc(strlen(tpl) + strlen(message->args) + strlen(tmp) - 3);
		sprintf(buf, tpl, message->args, tmp);
		logger(config->logchat, buf);
		free(buf);
		free(tmp);
	} else if(strcmp(message->type, "574") == 0 || strcmp(message->type, "474") == 0) {// Banni du chat
		logger(config->logchat, "--- Banni du chat, bonne nuit doux prince ---");
		return false;
	} else if(strcmp(message->type, "MODE") == 0) {
		tmp = strsplit(message->args, ' ');
		if(strcmp(tmp, "+b") == 0) {// Un type s'est fait bannir. On ne prend pas en compte les autres modes
			free(message->args);
			message->args = strsplit(tmp, '!');// On réutilise salement message->args
			tpl = "%s a été banni par %s !";
			buf = (char *)malloc(strlen(tpl) + strlen(tmp) + strlen(message->source) - 3);
			sprintf(buf, tpl, message->source, tmp);
			logger(config->logchat, buf);
			free(buf);
		}
		free(tmp);
	} else if(strcmp(message->type, "404") == 0) {// Cannot send to channel
		join(sock, config->channel);
	}

	return true;
}