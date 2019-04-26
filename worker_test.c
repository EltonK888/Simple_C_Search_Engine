#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include "freq_list.h"
#include "worker.h"

int main() {
	Node *head = NULL;
	char **filenames = init_filenames();
    char *namefile = "testing/big/t1/index";
    char *listfile = "testing/big/t1/filenames";
    read_list(namefile, listfile, &head, filenames);
    FreqRecord* freq;
    char word[MAXWORD] = "then";
    freq = get_word(word, head, filenames);
    //printf("%s\n", freq->filename);
    print_freq_records(freq);
    printf("Hi\n");
    //write(STDOUT_FILENO, freq, sizeof(FreqRecord));

    return 0;
}
