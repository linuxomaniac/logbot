char lower(char in) {
	return ((in >= 65 && in <= 90)?in + 32:in);
}

void strlstrip(char *in) {
// Retirer ':' au début d'une string
	if(in[0] == ':') {
		unsigned int len = strlen(in), i;

		for(i = 0; i < len - 1; i++) {
			in[i] = in[i + 1];
		}

		in[len - 1] = '\0';
	}
}

void strstrip(char *in) {
	unsigned int len = strlen(in), i;

	for(i = len; i > 0 && (in[i] == 10 || in[i] == 13 || in[i] == 0 || in[i] == 32); i--) {}

	if(i > 0) {// Si i == 0, on veut carrément vider la chaîne, sinon on met \0 à i + 1
		i++;
	}

	in[i] = '\0';
}

char *strsplit(char *in, char delim) {
	char *res;
	unsigned int i, len = strlen(in);

	for(i = 0; i < len && in[i] != delim; i++) {}

	if(i != len) {
		res = (char *)malloc(len - i + 1);
		memcpy(res, in + i + 1, len - i);
		res[len - i] = '\0';

		in[i] = '\0';
	} else {
		res = (char *)calloc(1,1);
	}

	return res;
}

/* WOW PUTAIN BALÈZE
 * Tout ça juste pour splitter une chaîne de caractère dans un array de pointeurs, en fonction d'un délimiteur et un certain nombre de fois...
 * exemple de boucle : for(i = 0; i < occ; i++) { ; avec occ = strarraysplit([...])
 */
unsigned int strarraysplit(char *in, char ***out, char delim, unsigned int limit) {
	unsigned int i, len = strlen(in), found = 0, *occurences, len_ = 0, shift = 0;
	char *tmp;

	occurences = (unsigned int *)malloc(2 * sizeof(unsigned int));// 2 c'est un pour après
	occurences[0] = 0;// Pour la première position

	for(i = 0; i < len && (found < limit || limit == 0); i++) {// Si limite != 0, tant que found < limit, sinon (si limit == 0) toujours vrai
		if(in[i] == delim) {// On a trouvé un délimiteur
			// On va noter sa position
			found++;
			occurences = (unsigned int *)realloc(occurences, (found + 2) * sizeof(unsigned int));
			occurences[found] = i;// Car on commence à 1 puisque [0] vaut 0
		}
	}
	// Voilà, ici on n'a pas besoin de realloc, grâce au 2 du début
	found++;// Pour mettre ce qui est après ce qui a été trouvé
	occurences[found] = len;

	if(found > 0) {
		// On alloue la nombre d'occurences trouvées fois la taille du pointeur
		*out = (char **)malloc(found * sizeof(char *));

		for(i = 0; i < found; i++) {
			len_ = occurences[i + 1] - occurences[i];

			if(i > 0 && shift == 0) {// Un truc crade pour prendre la première lettre si c'est le premier tour de boucle
				shift = 1;
			}

			// Le très sale (1 - shift), car on veut garder le délimiteur dans le string

			tmp = (char *)malloc(len_ + 1 + (1 - shift));

			memcpy(tmp, in + occurences[i] + shift, len_ + (1 - shift));
			tmp[len_ + (1 - shift)] = '\0';
			(*out)[i] = tmp;
		}
	} else {
		*out = NULL;
	}

	free(occurences);

	return found;
}

bool strstartswith(char *str, char *check) {
	unsigned int len = strlen(check), i;

	// str ne peut pas commencer par un truc plus long
	if(len > strlen(str)) {
		return false;
	}

	for(i = 0; i < len && str[i] == check[i]; i++) {}

	if(i == len) {
		return true;
	} else {
		return false;
	}
}

bool strcasecmp(char *a, char *b) {
	unsigned int i, len = strlen(a);

	if(len != strlen(b)) {
		return false;
	}

	for(i = 0; i < len; i++) {
		if(lower(a[i]) != lower(b[i])) {
			return false;
		}
	}

	return true;
}

char *strdup(char *a) {
	unsigned int len = strlen(a) + 1;
	char *ret;

	ret = (char *)malloc(len);
	memcpy(ret, a, len);

	return ret;
}

bool charinstr(char *in, char delim) {
	unsigned int i, len = strlen(in);

	for(i = 0; i < len && in[i] != delim; i++) {}

	return i != len;
}

void _strcat(char **in, char *tocat) {// Ne s'utilise que pour des in alloc !
	unsigned int len_in = strlen(*in), len_tocat = strlen(tocat);

	*in = (char *)realloc(*in, len_in + len_tocat + 1);
	memcpy(*in + len_in, tocat, len_tocat + 1);// + 1 pour prendre aussi le \0
}

void strtolower(char *in) {
	unsigned int i, len = strlen(in);

	for(i = 0; i < len; i++) {
		in[i] = lower(in[i]);
	}
}

unsigned int ctoi(char *in) {
	// Une alternative perso à atoi, retourne un unsigned int. Extrait les nombres d'une chaîne de caractères et les transforme en int
	unsigned int i, out = 0;

	for(i = 0; i < strlen(in); i++) {
		if(in[i] >= 48 && in[i] <= 57) {
			out *= 10;
			out += in[i] - 48;
		}
	}

	return out;
}

unsigned int intlen(int n) {
	unsigned int len = 0;

	if(n < 0) {
		len++;// Pour le signe -
		n *= -1;// On prend l'opposé (positif)
	}

	while(n > 0) {
		n = n / 10;
		len++;
	}
	return len;
}

unsigned int longlen(long n) {
	unsigned int len = 0;

	if(n < 0) {
		len++;// Pour le signe -
		n *= -1;// On prend l'opposé (positif)
	}

	while(n > 0) {
		n = n / 10;
		len++;
	}
	return len;
}
