#define BUFSIZE 512

#include <string.h>

struct CONFIG {
	char *host,
	*dcchost,
	*logchat,
	*logsystem,
	*channel,
	*nick,
	*password,
	*mysqlhost,
	*mysqldb,
	*mysqluser,
	*mysqlpassword;
	unsigned int port,
	dccport,
	dcctimeout,
	dccalertdelay;
};

unsigned char parse_config(struct CONFIG *config, char *a, char *b);
void init_config(struct CONFIG *config, char *configfile);
void clear_config(struct CONFIG config);

#include "config.c"
