#define PATHLENGTH 128
#define MAXRECORDS 100

// This data structure is used by the workers to prepare the output
// to be sent to the master process.

typedef struct {
	int freq;
	char filename[PATHLENGTH];
} FreqRecord;

void print_freq_records(FreqRecord *frp);
void run_worker(char *dirname, int in, int out);
FreqRecord* get_word(char *word, Node *head, char **filename);
void insert_sort(FreqRecord *master_array, FreqRecord *cur_freq);