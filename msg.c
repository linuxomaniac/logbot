bool parse_message(struct MESSAGE *message, char *in) {
	char **split;
	unsigned int occ;

	occ = strarraysplit(in, &split, ' ', 3);
	if(split != NULL) {
		message->source = split[0];
		message->type = split[1];
		message->target = split[2];

		if(occ >= 4) {// Parfois y'a pas d'args
			message->args = split[3];
		} else if(occ >= 3) {
			message->args = (char *)calloc(1, 1);
		} else {
			free(split);
			return false;
		}

		free(split);
	} else {
		return false;
	}
	message->fullsource = strsplit(message->source, '!');

	// On vire le reste du délimiteur PARTOUT
	strstrip(message->source);
	strstrip(message->fullsource);
	strstrip(message->type);
	strstrip(message->target);
	strstrip(message->args);

	// On vire ':' devant, maintenant que message->source est splitté
	strlstrip(message->source);
	strlstrip(message->target);
	strlstrip(message->args);

	return true;
}

void clear_message(struct MESSAGE message) {
	free(message.source);
	free(message.fullsource);
	free(message.type);
	free(message.target);
	free(message.args);
}
