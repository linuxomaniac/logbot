void *dcc_alert_thread(void *rawargs) {
	struct DCC_THREAD_ARGS *args = (struct DCC_THREAD_ARGS *)rawargs;

	char *tpl, *buf;
	unsigned int len;
	unsigned long ip;

	tpl = "PRIVMSG %s :Les messages postés durant ton absence vont t'être envoyés dans %d secondes en DCC. Attention, %d secondes pour accepter le DCC chat.\r\n";
	len = strlen(tpl) + strlen(args->source) + intlen(args->config.dccalertdelay) + intlen(args->config.dcctimeout) - 6;
	buf = (char *)malloc(len + 1);
	sprintf(buf, tpl, args->source, args->config.dccalertdelay, args->config.dcctimeout);
	send(args->sock, buf, len, 0);
	free(buf);

	sleep(args->config.dccalertdelay);// Pour avoir le temps

	ip = __builtin_bswap32(inet_addr(args->config.dcchost));// TRÈS SALE LA CONVERSION ADRESSRE + ENDIANESS (little -> big ou l'inverse, je ne sais plus)
	tpl = "PRIVMSG %s :\001DCC CHAT chat %lu %d\001\r\n";
	len = strlen(tpl) + strlen(args->source) + longlen(ip) + intlen(args->config.dccport) - 7;
	buf = (char *)malloc(len + 1);
	sprintf(buf, tpl, args->source, ip, args->config.dccport);
	send(args->sock, buf, len, 0);
	free(buf);

	return NULL;
}

void *dcc_send_thread(void *rawargs) {
	struct DCC_THREAD_ARGS *args = (struct DCC_THREAD_ARGS *)rawargs;

	int master_sock, client_sock;
	socklen_t addrlen;
	struct sockaddr_in server, client;
	struct timeval timeout;
	timeout.tv_sec = args->config.dcctimeout;
	timeout.tv_usec = 0;
	bool ready = false;
	char *tpl, *buf;
	unsigned int len;
	pthread_t alert_thread;

	if((master_sock = socket(AF_INET , SOCK_STREAM , 0)) < 0)
	{
		logger(args->config.logsystem, "WARN: DCC socket failed!");
		free(args->source);
		free(args->data);
		free(args);
		return NULL;
	}

	// Mettre le timeout pour l'acceptation
	if(setsockopt(master_sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
		logger(args->config.logsystem, "WARN: DCC setsockopt failed!");
		close(master_sock);
		free(args->source);
		free(args->data);
		free(args);
		return NULL;
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(args->config.dccport);

	while(!ready) {
		if(bind(master_sock,(struct sockaddr *)&server , sizeof(server)) < 0) {
			if(errno == EADDRINUSE) {// Port in use
				sleep(5);
			} else {
				logger(args->config.logsystem, "WARN: DCC bind failed!");
				close(master_sock);
				free(args->source);
				free(args->data);
				free(args);
				return NULL;
			}
		}
		ready = true;
	}

	listen(master_sock, 1);

	if(pthread_create(&alert_thread, NULL, dcc_alert_thread, rawargs) != 0) {
		logger(args->config.logsystem, "WARN: DCC thread failed!");
	}

	addrlen = sizeof(struct sockaddr_in);
	client_sock = accept(master_sock, (struct sockaddr *)&client, &addrlen);

	pthread_join(alert_thread, NULL);// On n'a plus besoin de l'autre thread, on l'attend
	close(master_sock);// On n'a plus besoin de ça non-plus

	if(client_sock < 0) {
		if(errno == 11) {// Connexion pas acceptée dans le temps imparti
			tpl = "PRIVMSG %s :La connexion DCC n'a pas été acceptée à temps ! Envoi des derniers logs annulé !\r\n";
			len = strlen(tpl) + strlen(args->source) - 2;
			buf = (char *)malloc(len + 1);
			sprintf(buf, tpl, args->source);
			send(args->sock, buf, len, 0);
			free(buf);

			free(args->source);
			free(args->data);
			free(args);
			return NULL;
		} else {
			logger(args->config.logsystem, "WARN: DCC accept failed!");
			free(args->source);
			free(args->data);
			free(args);
			return NULL;
		}
	}

	tpl = "Voici les messages qui ont été diffusés pendant ton absence :\n";
	send(client_sock, tpl, strlen(tpl), 0);
	send(client_sock, args->data, strlen(args->data), 0);

	close(client_sock);

	free(args->source);
	free(args->data);
	free(args);

	return NULL;
}
