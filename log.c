void logger(char *filename, char *in) {
	if(filename == NULL) {// Log désactivé
		return;
	}

	FILE *f;
	char date[17];
	time_t rawtime;
	struct tm *info;
	int t;

	f = fopen(filename, "ab");
	if(f == NULL) {
		printf("FATAL: could not open %s!\n", filename);
		exit(1);
	}

	time(&rawtime);
	info = localtime(&rawtime);

	strftime(date, 17, "%d/%m/%Y %H:%M", info);
	t = fprintf(f, "[%s] %s\n", date, in);

	fclose(f);

	if(t <= 0) {
		printf("FATAL: unable to write %s!\n", filename);
		exit(1);
	}
}
