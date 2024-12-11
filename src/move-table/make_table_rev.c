#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define VERBOSE 0

typedef struct row {
	int start;
	char head;
	int length;
	int headLF;
	int pointer;
	int offset;
} row;

char *T;

int suffixCmp(const void *a, const void *b) {
	return(strcmp(&T[*(int *) a], &T[*(int *) b]));
}

void main(int argc, char *argv[]) {

	FILE *textFile = fopen(argv[1], "r");
	fseek(textFile, 0, SEEK_END);
	int n = (int) ftell(textFile);
	rewind(textFile);

	fprintf(stderr, "n = %i\n", n);

	T = (char *) malloc(n);

	int *SA = (int *) malloc(n * sizeof(int));

	for (int i = 0; i < n; i++) {
		T[i] = (char) fgetc(textFile);
		SA[i] = i;
	}

	fprintf(stderr, "read text T from file %s\n", argv[1]);

	if (VERBOSE) {
		fprintf(stderr, "T = %.*s\n", n, T);
	}

	fclose(textFile);

	qsort(SA, n, sizeof(int), suffixCmp);

	char *bwt = (char *) malloc(n);

	int freq[6] = {0, 0, 0, 0, 0, 0};

	int *prank = (int *) malloc(n * sizeof(int));

	for (int i = 0; i < n; i++) {
		bwt[i] = T[(SA[i] + n - 1) % n];

		switch (bwt[i]) {
		case '#':
			prank[i] = ++freq[0]; 
			break;
		case '$':
			prank[i] = ++freq[1];
			break;
		case 'A':
			prank[i] = ++freq[2];
			break;
		case 'C':
			prank[i] = ++freq[3];
			break;
		case 'G':
			prank[i] = ++freq[4];
			break;
		case 'T':
			prank[i] = ++freq[5];
			break;
		}
	}

	fprintf(stderr, "computed bwt\n");

	if (VERBOSE) {
		fprintf(stderr, "bwt = %.*s\n", n, bwt);
	}

	int C[6];
	C[0] = 0;

	for (int i = 1; i < 6; i++) {
		C[i] = C[i - 1] + freq[i - 1];
	}

	fprintf(stderr, "computed C array\n");

	if (VERBOSE) {

		for (int i = 0; i < 6; i++) {
			fprintf(stderr, "%i ", C[i]);
		}

		fprintf(stderr, "\n");
	}

	int r = 0;

	for (int i = 0; i < n;) {
		int j;
		for (j = i;; j++) {
			if (j + 1 == n || bwt[j + 1] != bwt[i] || j - i + 1 == 255) {
				r++;
				i = j + 1;
				break;
			}
		}
	}

	fprintf(stderr, "computed r = %i\n", r);

	row *table = (row *) malloc(r * sizeof(row));

	int run = 0;

	for (int i = 0; i < n;) {

		table[run].start = i;

		int j;
		for (j = i;; j++) {
			if (j + 1 == n || bwt[j + 1] != bwt[i] || j - i + 1 == 255) {
				table[run].length = j - i + 1;
				break;
			}
		}

		switch (bwt[i]) {
		case '#':
			table[run].head = '#';
			table[run].headLF = C[0] + prank[i] - 1;
			break;
		case '$':
			table[run].head = '$';
			table[run].headLF = C[1] + prank[i] - 1;
			break;
		case 'A':
			table[run].head = 'A';
			table[run].headLF = C[2] + prank[i] - 1;
			break;
		case 'C':
			table[run].head = 'C';
			table[run].headLF = C[3] + prank[i] - 1;
			break;
		case 'G':
			table[run].head = 'G';
			table[run].headLF = C[4] + prank[i] - 1;
			break;
		case 'T':
			table[run].head = 'T';
			table[run].headLF = C[5] + prank[i] - 1;
			break;
		}

		run++;
		i = j + 1;
	}

	fprintf(stderr, "partly filled table\n");

	if (VERBOSE) {

		for (int i = 0; i < r; i++) {
			fprintf(stderr, "%i) %i, %c, %i, %i\n", i, table[i].start, table[i].head, table[i].length, table[i].headLF);
		}
	}

	char alphabet[] = {'#', '$', 'A', 'C', 'G', 'T'};

	for (int i = 0; i < 6; i++) {

		int k = 0;
	
		for (int j = 0; j < r; j++) {

			if (table[j].head == alphabet[i]) {

				for (; k + 1 < r && table[k + 1].start <= table[j].headLF; k++) {}

				table[j].pointer = k;
				table[j].offset = table[j].headLF - table[k].start;
			}
		}
	}

	fprintf(stderr, "filled table\n");

	if (VERBOSE) {

		for (int i = 0; i < r; i++) {
			fprintf(stderr, "%c, %i, %i, %i\n", table[i].head, table[i].length, table[i].pointer, table[i].offset);
		}
	}

	FILE *move = fopen(argv[2], "wb");
	rewind(move);

	fwrite(&n, sizeof(int), 1, move);
	fwrite(&r, sizeof(int), 1, move);

	for (int i = 0; i < r; i++) {
		unsigned char temp;

		fwrite(&table[i].head, 1, 1, move);

		temp = (unsigned char) table[i].length;
		fwrite(&temp, 1, 1, move);

		fwrite(&table[i].pointer, sizeof(int), 1, move);
		
		temp = (unsigned char) table[i].offset;
		fwrite(&temp, 1, 1, move);
	}

	fclose(move);

	fprintf(stderr, "wrote move table to file %s\n", argv[2]);

	fprintf(stderr, "reading move table from file %s\n", argv[2]);

	move = fopen(argv[2], "rb");
	rewind(move);

	fread(&n, sizeof(int), 1, move);
	fprintf(stderr, "read n = %i\n", n);

	fread(&r, sizeof(int), 1, move);
	fprintf(stderr, "read r = %i\n", r);

	row *newTable = (row *) malloc(r * sizeof(row));

	for (int i = 0; i < r; i++) {
		unsigned char temp;

		fread(&newTable[i].head, 1, 1, move);
		fread(&temp, 1, 1, move);
		newTable[i].length = (int) temp;
		fread(&newTable[i].pointer, sizeof(int), 1, move);
		fread(&temp, 1, 1, move);
		newTable[i].offset = (int) temp;
	}

	fprintf(stderr, "read table\n");

	fclose(move);

	for (int i = 0; i < r; i++) {

		if (newTable[i].head != table[i].head ||
		  newTable[i].length != table[i].length ||
		  newTable[i].pointer != table[i].pointer ||
		  newTable[i].offset != table[i].offset) {
			fprintf(stderr, "error reading table at row %i\n", i);
			fprintf(stderr, "newTable[i].head = %c, table[i].head = %c\n", newTable[i].head, table[i].head);
			fprintf(stderr, "newTable[i].length = %i, table[i].length = %i\n", newTable[i].length, table[i].length);
			fprintf(stderr, "newTable[i].pointer = %i, table[i].pointer = %i\n", newTable[i].pointer, table[i].pointer);
			fprintf(stderr, "newTable[i].offset = %i, table[i].offset = %i\n", newTable[i].offset, table[i].offset);
			break;
		}
	}

	fprintf(stderr, "loading correct\n");

	run = 0;
	int offset = 0;

	for (int i = n - 1; i > 0; i--) {

		while (offset >= newTable[run].length) {
			offset -= newTable[run].length;
			run++;
		}

		if (newTable[run].head != T[i]) {
			fprintf(stderr, "decompression error at position %i\n", i);
			return;
		}

		offset += (int) newTable[run].offset;
		run = newTable[run].pointer;
	}

	fprintf(stderr, "inversion correct\n");

	free(T);
	free(SA);
	free(bwt);
	free(prank);
	free(table);
	free(newTable);

	return;
}