#include "main.h"

// gcc -std=c11 -Wextra -o logbot -O3 -Wall $(mysql_config --cflags) main.c $(mysql_config --libs)

int main(int argc, char *argv[]) {
	struct CONFIG config;
	struct sockaddr_in servaddr;
	int sock, n;
	MYSQL mysql;
	unsigned int len, i, occ;
	char *buf, rawbuf[SOCKBUF + 1], *tmpbuf, *tmp, *reste = NULL, **split;
	struct MESSAGE message;
	bool t = true;

	if(argc != 2) {
		printf("Usage: %s <configuration file>\n", argv[0]);
		exit(1);
	}

	init_config(&config, argv[1]);

	if((sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
		perror("FATAL: socket");
		clear_config(config);
		exit(1);
	}

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(config.port);

	if(inet_pton(AF_INET, config.host, &servaddr.sin_addr) <= 0) {
		perror("FATAL: inet_pton");
		clear_config(config);
		close(sock);
		exit(1);
	}

	if(connect(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0 ) {
		perror("FATAL: connect");
		clear_config(config);
		close(sock);
		exit(1);
	}

	logger(config.logsystem, "LogBot started!");

	if(config.mysqlhost != NULL) {
		my_mysql_init(&mysql, config);
	}

	// On peut vraiment commencer !
	tmp = "PASS %s\r\nUSER %s Gamelon %s :%s\r\nNICK %s\r\n";
	len = strlen(tmp) + strlen(config.password) + 3 * strlen(config.nick) + strlen(config.host) - 10;
	buf = (char *)malloc(len + 1);
	sprintf(buf, tmp, config.password, config.nick, config.host, config.nick, config.nick);
	send(sock, buf, len, 0);
	free(buf);

	while((n = recv(sock, rawbuf, SOCKBUF, 0)) > 0 && t) {// t sert à vérifier si on s'est fait bannir
		buf = (char *)malloc(n + 1);
		memcpy(buf, rawbuf, n);
		buf[n] = '\0';

		// On ajoute le reste à ce qu'on a reçu (s'il y en a un (de reste))
		if(reste != NULL) {
			_strcat(&reste, buf);
			free(buf);
			buf = reste;
			reste = NULL;
		}

		occ = strarraysplit(buf, &split, '\n', 0);
		// Normalement, occ ne peut jamais être null
		if(occ != 0 && split != NULL) {
			for(i = 0; i < occ; i++) {
				if(t) {
					// Normalement, y'a encore les délimiteurs dans ce qu'on a splitté.
					// S'il n'y est pas, c'est que c'est le dernier élément, donc on va le garder comme reste, sinon on free.
					if(charinstr(split[i], '\r') && charinstr(split[i], '\n')) {
						// Ici on peut ocmmencer à process les trucs (avec la variable split[i])
						strstrip(split[i]);// On vire les résidus de délimiteurs

						if(strlen(split[i]) != 0) {// Après le strip, ça peut être nul
							if(strstartswith(split[i], "PING")) {
								tmp = strsplit(split[i], ' ');
								len = strlen(tmp) + 7;
								tmpbuf = (char *)malloc(len + 1);
								sprintf(tmpbuf, "PONG %s\r\n", tmp);
								send(sock, tmpbuf, len, 0);
								free(tmpbuf);
								free(tmp);
							} else {
								if(parse_message(&message, split[i])) {

								/* On passe un pointeur vers config, car le nick peut être modifié;
								* tout comme le message (on n'a pas besoin de faire de copie pour travailler dessus)
								* On peut être amené à se déconnecter, donc process_message() retourne un bool (magique)
								*/
								t = process_message(sock, &config, &mysql, &message);
								clear_message(message);
								} else {
									logger(config.logsystem, "WARN: message parsing failed! Illegal message?");
								}
							}
						}
						free(split[i]);
					} else {// C'est un résidu
						if(reste != NULL) {
							_strcat(&reste, split[i]);
							free(split[i]);
						} else {
							reste = split[i];
						}
					}
				} else {
					free(split[i]);
				}
			}
			free(split);
		} else {
			logger(config.logsystem, "WARN: messages separation failed!");
		}
		free(buf);
	}
	if(n <= 0) {
		logger(config.logsystem, "Connection closed; bye-bye.");
	}

	close(sock);
	if(config.mysqlhost != NULL) {
		mysql_close(&mysql);
		mysql_library_end();// MySQL library, pls
	}
	clear_config(config);

	return 0;
}
