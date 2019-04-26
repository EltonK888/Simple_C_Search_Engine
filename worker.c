#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "freq_list.h"
#include "worker.h"

/* Helper function to sort the master freq array */
void insert_sort(FreqRecord *master_array, FreqRecord *cur_freq) {
	int i = 0;
	if (master_array[0].freq == 0) { // if nothing in the array
		master_array[1] = master_array[0];
		master_array[0] = *cur_freq;
		return;
	}
	while (i<MAXRECORDS && master_array[i].freq != 0) { // loop to see where to insert the FreqRecord
		if (master_array[i].freq < cur_freq->freq) { // found where the order is violated
			int j;
			int n;
			for (j=i; j<MAXRECORDS; j++) { // find the index of last FreqRecord
				if (master_array[j].freq == 0) {
					break;
				}
			}
			if (j == MAXRECORDS-1) { // case where there's overflow
				for (n=j-1; n>i; n--)  {
					master_array[n] = master_array[n-1];
				}
				master_array[i] = *cur_freq;
			} else { // shift everything down by one and insert the freq record
				for (n=j+1; n>i; n--)  {
					master_array[n] = master_array[n-1];
				}
				master_array[i] = *cur_freq;
			}
			return;
		}
		i++;
	}
	if (i < MAXRECORDS) { // if we get here, then just add at the back of the array
		master_array[i+1] = master_array[i];
		master_array[i] = *cur_freq;
	}
	return;
}

/* Looks for the given word in the index given by the Node* head
   and returns an array of FreqRecords that holds the number of
   occurrences of word in that particular file, minimum 1 occurence
   to be included in the array. */

FreqRecord* get_word(char *word, Node *head, char **filename) {
	FreqRecord* freq_array = malloc(MAXFILES*sizeof(FreqRecord)); // allocate memory for the array
	if (freq_array == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	if (head == NULL) { // if no index, then return an array with just one FreqRecord of freq 0
		FreqRecord *new_freq = malloc(sizeof(FreqRecord));
		if (new_freq == NULL) {
			free(freq_array);
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		new_freq->freq = 0;
		strncpy(new_freq->filename, "", 1);
		new_freq->filename[strlen(new_freq->filename)-1] = '\0';
		*(freq_array + 0) = *new_freq;
		return freq_array;
	}
	Node *cur = head;
	int j = 0;
	while(cur != NULL) { // loop through the index until we find the word
		if (strcmp(cur->word, word) == 0) { // if found the word
			int i = 0;
			while (i < MAXFILES) {
				if (cur->freq[i] > 0) { // if the frequency is at least 1, then add it to freq_array
					FreqRecord *new_freq = malloc(sizeof(FreqRecord));
					if (new_freq == NULL) {
						free(freq_array);
						perror("malloc");
						exit(EXIT_FAILURE);
					}
					new_freq->freq = cur->freq[i]; // add the frequency of the word
					strncpy(new_freq->filename, filename[i], strlen(filename[i])+1); // add the file name
					*(freq_array + j) = *new_freq;
					j++;
				}
				i++;
			}
			break;
		}
		cur = cur->next;
	}
	// terminate the array with a FreqRecord of freq 0
	FreqRecord *new_freq = malloc(sizeof(FreqRecord));
	if (new_freq == NULL) {
		free(freq_array);
		perror("malloc");
		exit(EXIT_FAILURE);
	}
	new_freq->freq = 0; // 0 for the freq
	strncpy(new_freq->filename, "", 1); // empty string for the file name
	new_freq->filename[strlen(new_freq->filename)-1] = '\0';
	freq_array[j] = *new_freq; // add it to the end of the freq_arry
	return freq_array;
}

/* Print to standard output the frequency records for a word.
* Used for testing.
*/
void print_freq_records(FreqRecord *frp) {
	int i = 0;
	while(frp != NULL && frp[i].freq != 0) {
		printf("%d    %s\n", frp[i].freq, frp[i].filename);
		i++;
	}
}

/* run_worker
* - load the index found in dirname
* - read a word from the file descriptor "in"
* - find the word in the index list
* - write the frequency records to the file descriptor "out"
*/
void run_worker(char *dirname, int in, int out){
	char file_dir[PATHLENGTH];
	char index_dir[PATHLENGTH];
	// create the index file path and filnames path
	strncpy(file_dir, dirname, strlen(dirname)+1);
	strncpy(index_dir, dirname, strlen(dirname)+1);
	strncat(index_dir, "/index", 7);
	strncat(file_dir, "/filenames", 11);
	Node *head = NULL;
	char **filenames = init_filenames();
	//int i = 0;
	int rsize;
	// create the data structures from the index files and filenames
	char buf[MAXWORD];
	read_list(index_dir, file_dir, &head, filenames);
	// read in the word from the in fd
	while ((rsize = read(in, &buf, MAXWORD)) > 0) {
		//Node *head = NULL;
		//char **filenames = init_filenames();
		int i = 0;
		//read_list(index_dir, file_dir, &head, filenames);
		if (rsize == -1) {
			perror("read");
			exit(EXIT_FAILURE);
		}
		int j;
		// add Null char to the end of buf
		for (j = 0; j < MAXWORD; j++) {
			if (buf[j] == '\n') {
				buf[j] = '\0';
				break;
			}
		}
		FreqRecord *freq = malloc(sizeof(FreqRecord));
		if (freq == NULL) {
			perror("malloc");
			exit(EXIT_FAILURE);
		}
		freq = get_word(buf, head, filenames); // get the FreqRecord array containing the word
		while (freq != NULL) {
			if (write(out, &freq[i], sizeof(FreqRecord)) == -1) { // write to out fd each FreqRecord
				fprintf(stderr, "Error writing to pipe");
			}
			if (freq[i].freq == 0) {
				break;
			}
			i++;
		}
		free(freq);
	}
}

