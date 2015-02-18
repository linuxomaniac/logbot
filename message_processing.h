int privmsg(int sock, char *target, char *message);
int join(int sock, char *channel);
void mysql_user_quit(struct CONFIG *config, MYSQL *mysql, char *source);
bool process_message(int sock, struct CONFIG *config, MYSQL *mysql, struct MESSAGE *message);

#include "message_processing.c"
