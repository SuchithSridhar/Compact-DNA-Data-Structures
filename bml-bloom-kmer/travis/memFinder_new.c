#include <stdio.h>
#include <stdlib.h>
#include <time.h>


typedef struct row {
	char head;
	unsigned char length;
	int pointer;
	unsigned char offset;
} row;


typedef struct searchResult {
	int matchLength;
	int sRun;
	int sOffset;
	int eRun;
	int eOffset;
} searchResult;


void loadTable(char *tableFileName);
void loadRevTable(char *revTableFileName);

void loadPattern(char *patternFileName);

searchResult *prefSearch(int x);
int sufSearch(int y);

int width(searchResult *result);


// === functions for filter ===

void loadFilter(char *filterFileName);
void windowLeft(char leftChar, char rightChar);
int checkFilter();

// ============================


int n;

int r;
row *table;

int rBar;
row *revTable;

int m;
char *P;

int L;

int stepCount;


// === variables for filter ===

int k;
int filterSize;
int insPar;

char *filter;

int window;
int pow5;

// ============================


void main(int argc, char *argv[]) {

	fprintf(stderr, "usage: memFinder DATASET.mvt TESATAD.mvt PATTERN.txt ");
	fprintf(stderr, "[Y/N : print MEMs] [Y/N : print frequencies] [Y/N : use BML] [min MEM length] ");
	fprintf(stderr, "[Y/N : use filter] DATASET.blm\n");

	loadTable(argv[1]);
	loadRevTable(argv[2]);
	loadPattern(argv[3]);

	int PRINT;

	if (argv[4][0] == 'Y') {
		PRINT = 1;
	} else {
		PRINT = 0;
	}

	int WIDTH;

	if (argv[5][0] == 'Y') {
		WIDTH = 1;
	} else {
		WIDTH = 0;
	}

	int BML;

	if (argv[6][0] == 'Y') {
		BML = 1;
		fprintf(stderr, "using BML\n");
	} else {
		BML = 0;
		fprintf(stderr, "using forward-backward\n");
	}

	L = atoi(argv[7]);


	// === filter setup ===

	int FILTER;

	if (argv[8][0] == 'Y') {
		FILTER = 1;
		loadFilter(argv[9]);
		fprintf(stderr, "using filter: k = %i, filterSize = %i, insPar = %i\n", k, filterSize, insPar);
	} else {
		FILTER = 0;
	}

	pow5 = 1;

	for (int i = 1; i < k; i++) {
		pow5 *= 5;
	}

	// ====================

	int memCount = 0;
	stepCount = 0;
	clock_t startTime = clock();

	for (int x = 0; x + L - 1 <= m - 1;) {

		int xPrime = x;

		// === using filter ===

		if (FILTER) {

			window = 0;

			for (int i = x + L - 1; i >= x; i--) {

				windowLeft(P[i], (i + k <= x + L - 1 ? P[i + k] : '#'));

				if (i <= x + L - k &&
					((94607073 * (long long) window + 54204618) % 55586519) % insPar == 0 &&
					!checkFilter()) {
					xPrime = i + 1;
					break;
				}
			}
		}

		// ====================

		if (xPrime == x && BML) {
			xPrime = (x + L - 1) - sufSearch(x + L - 1) + 1;
		}

		if (xPrime == x) {
			searchResult *result = prefSearch(x);

			if (result -> matchLength >= L) {

				memCount++;

				if (PRINT) {
					printf("P[%i..%i] = %.*s is a MEM.\n", x, x + (result -> matchLength) - 1, result -> matchLength, &P[x]);
				}

				if (WIDTH) {
					printf("It occurs %i time(s).\n", width(result));
				}
			}

			if (x + (result -> matchLength) - 1 == m - 1) {
				break;
			}

			xPrime = (x + result -> matchLength) - sufSearch(x + result -> matchLength) + 1;

			free(result);
		}

		x = xPrime;
	}

	printf("%i MEM(s) of length at least %i found.\n", memCount, L);
	printf("%f seconds elapsed.\n", (double)(clock() - startTime) / CLOCKS_PER_SEC);
	printf("%i steps taken.\n", stepCount);

	if (FILTER) {
		free(filter);
	}

	free(table);	
	free(revTable);
	free(P);

	return;
}


void loadTable(char *tableFileName) {

	FILE *tableFile = fopen(tableFileName, "rb");

	fread(&n, sizeof(int), 1, tableFile);
	fread(&r, sizeof(int), 1, tableFile);

	table = (row *) malloc(r * sizeof(row));

	for (int i = 0; i < r; i++) {
		fread(&table[i].head, 1, 1, tableFile);
		fread(&table[i].length, 1, 1, tableFile);
		fread(&table[i].pointer, sizeof(int), 1, tableFile);
		fread(&table[i].offset, 1, 1, tableFile);
	}

	fclose(tableFile);

	return;
}


void loadRevTable(char *revTableFileName) {

	FILE *revTableFile = fopen(revTableFileName, "rb");

	fread(&n, sizeof(int), 1, revTableFile);
	fread(&rBar, sizeof(int), 1, revTableFile);

	revTable = (row *) malloc(rBar * sizeof(row));

	for (int i = 0; i < rBar; i++) {
		fread(&revTable[i].head, 1, 1, revTableFile);
		fread(&revTable[i].length, 1, 1, revTableFile);
		fread(&revTable[i].pointer, sizeof(int), 1, revTableFile);
		fread(&revTable[i].offset, 1, 1, revTableFile);
	}

	fclose(revTableFile);

	return;
}


void loadPattern(char *patternFileName) {

	FILE *patternFile = fopen(patternFileName, "r");

	fseek(patternFile, 0, SEEK_END);
	m = (int) ftell(patternFile);
	rewind(patternFile);

	P = (char *) malloc(m);

	fread(P, 1, m, patternFile);
	fclose(patternFile);

	return;
}


searchResult *prefSearch(int x) {

	searchResult *result = (searchResult *) malloc(sizeof(searchResult));

	int sRun = 0;
	int sOffset = 0;

	int eRun = rBar - 1;
	int eOffset = (int) revTable[rBar - 1].length - 1;

	result -> matchLength = 0;
	result -> sRun = sRun;
	result -> sOffset = sOffset;
	result -> eRun = eRun;
	result -> eOffset = eOffset;

	for (int i = x; i < m; i++) { 

		while (sRun < rBar && revTable[sRun].head != P[i]) {
			sRun++;
			sOffset = 0;
		}

		while (eRun >= 0 && revTable[eRun].head != P[i]) {
			eRun--;
			eOffset = revTable[eRun].length - 1;
		}

		if (sRun > eRun || (sRun == eRun && sOffset > eOffset) || revTable[sRun].head != P[i]) {
			break;
		}

		sOffset += (int) revTable[sRun].offset;
		sRun = revTable[sRun].pointer;

		eOffset += (int) revTable[eRun].offset;
		eRun = revTable[eRun].pointer;

		while (sOffset >= (int) revTable[sRun].length) {
			sOffset -= (int) revTable[sRun].length;
			sRun++;
		}

		while (eOffset >= (int) revTable[eRun].length) {
			eOffset -= (int) revTable[eRun].length;
			eRun++;
		}

		result -> matchLength++;
		result -> sRun = sRun;
		result -> sOffset = sOffset;
		result -> eRun = eRun;
		result -> eOffset = eOffset;

		stepCount++;
	}

	return(result);
}


int sufSearch(int y) {

	int matchLength = 0;

	int sRun = 0;
	int sOffset = 0;

	int eRun = r - 1;
	int eOffset = (int) table[r - 1].length - 1;

	for (int i = y; i >= 0 && matchLength < L; i--) { 

		while (sRun < r && table[sRun].head != P[i]) {
			sRun++;
			sOffset = 0;
		}

		while (eRun >= 0 && table[eRun].head != P[i]) {
			eRun--;
			eOffset = table[eRun].length - 1;
		}

		if (sRun > eRun || (sRun == eRun && sOffset > eOffset) || table[sRun].head != P[i]) {
			break;
		}

		sOffset += (int) table[sRun].offset;
		sRun = table[sRun].pointer;

		eOffset += (int) table[eRun].offset;
		eRun = table[eRun].pointer;

		while (sOffset >= (int) table[sRun].length) {
			sOffset -= (int) table[sRun].length;
			sRun++;
		}

		while (eOffset >= (int) table[eRun].length) {
			eOffset -= (int) table[eRun].length;
			eRun++;
		}

		matchLength++;

		stepCount++;
	}

	return(matchLength);
}


int width(searchResult *result) {

	int w = 0;

	while (result -> sRun < result -> eRun) {
		w += (int) revTable[result -> sRun].length - (result -> sOffset);
		result -> sRun++;
		result -> sOffset = 0;
	}

	w += (result -> eOffset) - (result -> sOffset) + 1;

	return(w);
}


// === code for filter ===

void loadFilter(char *filterFileName) {

	FILE *filterFile = fopen(filterFileName, "rb");

	fread(&k, 1, sizeof(int), filterFile);
	fread(&filterSize, 1, sizeof(int), filterFile);
	fread(&insPar, 1, sizeof(int), filterFile);

	filter = (char *) malloc(filterSize / 8 + 1);

	fread(filter, 1, filterSize / 8 + 1, filterFile);

	fclose(filterFile);

	return;
}


void windowLeft(char leftChar, char rightChar) {

	switch (rightChar) {
		case 'A':
			window -= 1;
			break;
		case 'C':
			window -= 2;
			break;
		case 'G':
			window -= 3;
			break;
		case 'T':
			window -= 4;
			break;
	}

	window = window / 5;

	switch (leftChar) {
		case 'A':
			window += 1 * pow5;
			break;
		case 'C':
			window += 2 * pow5;
			break;
		case 'G':
			window += 3 * pow5;
			break;
		case 'T':
			window += 4 * pow5;
			break;
	}

	return;
}


int checkFilter() {

	long long v = (long long) window;
	int bitPos;

	bitPos = (int) (((59515879 * v + 95578445) % 56887169) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	bitPos = (int) (((65835776 * v + 22503992) % 34543247) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	bitPos = (int) (((63372674 * v + 9287645) % 31428521) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	bitPos = (int) (((58184635 * v + 49547574) % 64362491) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	bitPos = (int) (((79157700 * v + 94338060) % 15976133) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	bitPos = (int) (((88994189 * v + 67949655) % 63991913) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		return(0);
	}

	return(1);
}