#include <stdio.h>
#include <stdlib.h>


void loadText(char *textFileName);
void windowLeft(char leftChar, char rightChar);
int checkFilter();
void addToFilter();
void saveFilter(char *filterFileName);


int n;

char *T;

int k;
int filterSize;
int insPar;

char *filter;

int pow5;
int window;
int tupleCount;
int setCount;


void main(int argc, char *argv[]) {

	fprintf(stderr, "usage: buildFilter DATASET.txt DATASET.blm ");
	fprintf(stderr, "[k-mer length] [filter size in bits] [insertion parameter]\n");

	loadText(argv[1]);

	k = atoi(argv[3]);

	filterSize = atoi(argv[4]);

	filter = (char *) malloc(filterSize / 8 + 1);

	insPar = atoi(argv[5]);

	pow5 = 1;

	for (int i = 1; i < k; i++) {
		pow5 *= 5;
	}

	window = 0;
	tupleCount = 0;
	setCount = 0;

	for (int i = n - 1; i >= 0; i--) {

		windowLeft(T[i], (i + k < n ? T[i + k] : '#'));

		if (((94607073 * (long long) window + 54204618) % 55586519) % insPar == 0) {

			if (!checkFilter()) {
				addToFilter();
				tupleCount++;
			}
		}
	}

	fprintf(stderr, "Filter built, %i %i-tuples added to filter, %i bits set to 1.\n", tupleCount, k, setCount);

	saveFilter(argv[2]);

	free(T);
	free(filter);

	return;
}


void loadText(char *textFileName) {

	FILE *textFile = fopen(textFileName, "r");
	fseek(textFile, 0, SEEK_END);
	n = (int) ftell(textFile);
	rewind(textFile);

	T = (char *) malloc(n);

	fread(T, 1, n, textFile);

	fclose(textFile);

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


void addToFilter() {

	long long v = (long long) window;
	int bitPos;

	bitPos = (int) (((59515879 * v + 95578445) % 56887169) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	bitPos = (int) (((65835776 * v + 22503992) % 34543247) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	bitPos = (int) (((63372674 * v + 9287645) % 31428521) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	bitPos = (int) (((58184635 * v + 49547574) % 64362491) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	bitPos = (int) (((79157700 * v + 94338060) % 15976133) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	bitPos = (int) (((88994189 * v + 67949655) % 63991913) % filterSize);

	if (!(filter[bitPos / 8] & 1 << (bitPos % 8))) {
		filter[bitPos / 8] = filter[bitPos / 8] | 1 << (bitPos % 8);
		setCount++;
	}

	return;
}


void saveFilter(char *filterFileName) {

	FILE *filterFile = fopen(filterFileName, "wb");

	fwrite(&k, 1, sizeof(int), filterFile);
	fwrite(&filterSize, 1, sizeof(int), filterFile);
	fwrite(&insPar, 1, sizeof(int), filterFile);
	fwrite(filter, 1, filterSize / 8 + 1, filterFile);

	fclose(filterFile);

	return;
}