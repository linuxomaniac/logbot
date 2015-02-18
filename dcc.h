struct DCC_THREAD_ARGS {
	int sock;
	char *source, *data;
	struct CONFIG config;
};

void *dcc_send_thread(void *rawargs);
void *dcc_alert_thread(void *rawargs);
void dcc_send(int sock, struct CONFIG config, char **source, char **data);

#include "dcc.c"