struct MESSAGE {
	char *source,
	*fullsource,
	*type,
	*target,
	*args;
};

bool parse_message(struct MESSAGE *msg, char *in);
void clear_message(struct MESSAGE msg);

#include "msg.c"
