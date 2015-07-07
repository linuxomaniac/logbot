char lower(char in);
void strlstrip(char *in);
void strstrip(char *in);
char *strsplit(char *in, char delim);
unsigned int strarraysplit(char *in, char ***out, char delim, unsigned int limit);
bool strstartswith(char *str, char *check);
bool strcasecmp(char *a, char *b);
char *strdup(char *a);
bool charinstr(char *in, char delim);
void _strcat(char **in, char *tocat);
void strtolower(char *in);
unsigned int ctoi(char *in);
unsigned int intlen(int n);
unsigned int longlen(long n);

#include "str.c"
