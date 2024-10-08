#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LENGTH 10000001

char T[LENGTH];

int suffixCmp(const void *a, const void *b) {
	return(strcmp(&T[*(int *) a], &T[*(int *) b]));
}

int main() {
	int SA[LENGTH];

	for (int i = 0; i < LENGTH; i++) {
		T[i] = (char) getchar();
		SA[i] = i;
	}

	qsort(SA, LENGTH, sizeof(int), suffixCmp);

	for (int i = 0; i < LENGTH; i++) {
		putchar(T[(SA[i] + (LENGTH-1)) % LENGTH]);
	}

	return EXIT_SUCCESS;
}
