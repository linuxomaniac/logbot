void logger(char *filename, char *in) {
	FILE *f;
	char date[12];
	time_t rawtime;
	struct tm *info;
	int t;

	if(filename == NULL) {// Log désactivé
		return;
	}

	f = fopen(filename, "ab");
	if(f == NULL) {
		printf("FATAL: could not open %s!\n", filename);
		exit(1);
	}

	time(&rawtime);
	info = localtime(&rawtime);

	strftime(date, 12, "%d/%m %H:%M", info);
	t = fprintf(f, "[%s] %s\n", date, in);

	fclose(f);

	if(t <= 0) {
		printf("FATAL: unable to write %s!\n", filename);
		exit(1);
	}
}
