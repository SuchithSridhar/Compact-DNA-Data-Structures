/**
 * MEM finder built using FM Indexes. 
 * @author Suchith Sridhar (B00932400)
 * @author Anas Alhadi (B00895875)
 * @date 22 Sep 2024
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "fm_index.h"

#define MAX_PATTERN_LENGTH 1000000
#define MIN_MEM_LENGTH 10

#define FORWARD 1
#define BACKWARD -1

typedef struct row {
	char head;
	unsigned char length;
	int pointer;
	unsigned char offset;
} row;

// direction = 1 -> right (forward)
// direction = -1 -> left (backward)
// I'm going to assume you gave me the right fm index
// returns the steps I was able to walk
int find(char *pattern) {
    //load move table
    FILE *move = fopen("dataset.mvt", "rb");

	int n;
	fread(&n, sizeof(int), 1, move);
	fprintf(stderr, "read n = %i\n", n);

	int r;
	fread(&r, sizeof(int), 1, move);
	fprintf(stderr, "read r = %i\n", r);

    row* table = (row *) malloc(r * sizeof(row));

	for (int i = 0; i < r; i++) {
		fread(&table[i].head, 1, 1, move);
		fread(&table[i].length, 1, 1, move);
		fread(&table[i].pointer, sizeof(int), 1, move);
		fread(&table[i].offset, 1, 1, move);
	}

	fclose(move);

	fprintf(stderr, "loaded table\n");

    // Load move table from "tesatad.mvt"
    FILE *rev_move = fopen("tesatad.mvt", "rb");

    int rev_n;
    fread(&rev_n, sizeof(int), 1, rev_move);
    fprintf(stderr, "read rev_n = %i\n", rev_n);

    int rev_r;
    fread(&rev_r, sizeof(int), 1, rev_move);
    fprintf(stderr, "read rev_r = %i\n", rev_r);

    // Allocate memory for rev_table
    row* rev_table = (row *) malloc(rev_r * sizeof(row));

    for (int i = 0; i < rev_r; i++) {
        fread(&rev_table[i].head, 1, 1, rev_move);
        fread(&rev_table[i].length, 1, 1, rev_move);
        fread(&rev_table[i].pointer, sizeof(int), 1, rev_move);
        fread(&rev_table[i].offset, 1, 1, rev_move);
    }

    fclose(rev_move);

    fprintf(stderr, "loaded rev_table\n");


    // END OF TABLE LOADING CODE

    uint64_t mem_counts = 0;
    uint32_t current_index = 0;

    // check to see if the substring of length 40 occurs in text. If it doesn't,
    // then that subtring cannot be a MEM of length at least 40, so move to the
    // next MEM.
    // int s = 0;
    // int e = rev_r - 1;
    // int offset = 0;
    
    while (current_index < strlen(pattern)) {

        int i = current_index;

        int s_run = 0;
        int e_run = rev_r - 1;
        int s_offset = 0;
        int e_offset = 0;

        while (i < strlen(pattern) && (s_run > e_run || (s_run == e_run && s_offset > e_offset))) {

            while(rev_table[s_run].head != pattern[i] && s_run <= rev_r-1) {
                s_run ++;
                // s = (s + rev_table[s_run].length) - s_offset; 
                s_offset = 0;
            }

            while(rev_table[e_run].head != pattern[i] && e_run >= 0) {
                e_run --;
                // e = (e - e_offset) - 1;
                e_offset = rev_table[e_run].length - 1;
            }

            if (s_run > e_run || (s_run == e_run && s_offset > e_offset))
                break;
            
            s_offset = rev_table[s_run].offset + s_offset;
            s_run = rev_table[s_run].pointer;    
            while(s_offset > rev_table[s_run].length) {
                s_offset -= rev_table[s_run].length;
                s_run++;
            }

            e_offset = rev_table[e_run].offset + e_offset;
            e_run = rev_table[e_run].pointer;    
            while(e_offset > rev_table[e_run].length) {
                e_offset -= rev_table[e_run].length;
                e_run++;
            }

            i++;
        }

        // check to see if the mem is at least 40 characters long
        if (i - current_index >= MIN_MEM_LENGTH) {
            printf("(%d, %d)\n", current_index, i - 1);
            mem_counts++;
        }

        // if we reach the EOS, then the entire pattern is a MEM
        if (i == strlen(pattern)) {
            break;
        }

        s_offset = 0;
        e_offset = 0;
        s_run = 0;
        e_run = r - 1;

        int j = i;
        while (j >= 0 && (s_run > e_run || (s_run == e_run && s_offset > e_offset))) {

            while(table[s_run].head != pattern[i] && s_run <= r-1) {
                s_run ++;
                // s = (s + rev_table[s_run].length) - s_offset; 
                s_offset = 0;
            }

            while(table[e_run].head != pattern[i] && e_run >= 0) {
                e_run --;
                // e = (e - e_offset) - 1;
                e_offset = table[e_run].length - 1;
            }

            if (s_run > e_run || (s_run == e_run && s_offset > e_offset))
                break;
            
            s_offset = table[s_run].offset + s_offset;
            s_run = table[s_run].pointer;    
            while(s_offset > table[s_run].length) {
                s_offset -= table[s_run].length;
                s_run++;
            }

            e_offset = table[e_run].offset + e_offset;
            e_run = table[e_run].pointer;    
            while(e_offset > table[e_run].length) {
                e_offset -= table[e_run].length;
                e_run++;
            }

            j--;
        }

        if (j == i) {
            current_index = j;
        } else {
            current_index = j + 1;
        }
    }
        free(table);
        free(rev_table);

    return mem_counts;   
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    // Open the file provided as a command-line argument
    FILE *file = fopen(argv[1], "r");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

    // Get the size of the file
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for the pattern and read it from the file
    char *pattern = malloc(file_size + 1);
    if (pattern == NULL) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    fread(pattern, 1, file_size, file);
    pattern[file_size] = '\0'; // Null-terminate the string

    // Close the file
    fclose(file);

    // Run the find function with the pattern
    int result = find(pattern);

    // Output the returned value
    printf("Result: %d\n", result);

    // Free allocated memory
    free(pattern);

    return 0;
}