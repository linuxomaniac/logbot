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
	*mysqlpassword,
	*aliasrandomline;

	unsigned int port,
	dccport,
	dcctimeout,
	dccalertdelay;

	struct ALIAS *alias;
};

struct ALIAS {
	char *trigger,
	*message;
	struct ALIAS *next;
};

unsigned char parse_config(struct CONFIG *config, char *a, char *b);
void init_config(struct CONFIG *config, char *configfile);
void clear_config(struct CONFIG config);

#include "config.c"
