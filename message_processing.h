bool get_random_line_file(char *fname, unsigned int maxsize, char **line);
int privmsg(int sock, char *target, char *message);
int say(int sock, struct CONFIG *config, char *message);
int join(int sock, char *channel);
void mysql_user_quit(struct CONFIG *config, MYSQL *mysql, char *source);
void react_to_message(int sock, struct CONFIG *config, struct MESSAGE *message);
bool process_message(int sock, struct CONFIG *config, MYSQL *mysql, struct MESSAGE *message);

#include "message_processing.c"
