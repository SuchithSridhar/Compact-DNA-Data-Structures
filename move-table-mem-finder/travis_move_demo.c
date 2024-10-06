#include <stdio.h>
#include <stdlib.h>

typedef struct row {
	char head;
	unsigned char length;
	int pointer;
	unsigned char offset;
} row;


void main(int argc, char *argv[]) {

	FILE *move = fopen(argv[1], "rb");

	int n;
	fread(&n, sizeof(int), 1, move);
	fprintf(stderr, "read n = %i\n", n);

	int r;
	fread(&r, sizeof(int), 1, move);
	fprintf(stderr, "read r = %i\n", r);

	row *table = (row *) malloc(r * sizeof(row));

	for (int i = 0; i < r; i++) {
		fread(&table[i].head, 1, 1, move);
		fread(&table[i].length, 1, 1, move);
		fread(&table[i].pointer, sizeof(int), 1, move);
		fread(&table[i].offset, 1, 1, move);
	}

	fclose(move);

	fprintf(stderr, "loaded table\n");

	char *T = (char *) malloc(n);
	T[n - 1] = '#';

	int run = 0;
	int offset = 0;

	for (int i = n - 2; i >= 0; i--) {

		while (offset >= (int) table[run].length + 1) {
			offset -= (int) table[run].length + 1;
			run++;
		}

		T[i] = table[run].head;

		offset += (int) table[run].offset;
		run = table[run].pointer;
	}

	fprintf(stderr, "decompressed T\n");

	FILE *oldT = fopen(argv[2], "r");

	for (int i = 0; i < n; i++) {

		if (fgetc(oldT) != T[i]) {
			fprintf(stderr, "decompression error at position %i\n", i);
			return;
		}
	}

	fprintf(stderr, "checked T\n");

	free(T);
	free(table);

	return;
}