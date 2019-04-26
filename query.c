#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/wait.h>
#include "freq_list.h"
#include "worker.h"



int main(int argc, char **argv) {
	
	char ch;
	char path[PATHLENGTH];
	char *startdir = ".";
    int num_proc = 0;

	while((ch = getopt(argc, argv, "d:")) != -1) {
		switch (ch) {
			case 'd':
			startdir = optarg;
			break;
			default:
			fprintf(stderr, "Usage: queryone [-d DIRECTORY_NAME]\n");
			exit(1);
		}
	}
	// Open the directory provided by the user (or current working directory)
	
	DIR *dirp;
	if((dirp = opendir(startdir)) == NULL) {
		perror("opendir");
		exit(1);
	}
    struct dirent *dp; 
    while((dp = readdir(dirp)) != NULL) {

        if(strcmp(dp->d_name, ".") == 0 || 
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0){
            continue;
        }
        strncpy(path, startdir, PATHLENGTH);
        strncat(path, "/", PATHLENGTH - strlen(path) - 1);
        strncat(path, dp->d_name, PATHLENGTH - strlen(path) - 1);

        struct stat sbuf;
        if(stat(path, &sbuf) == -1) {
            //This should only fail if we got the path wrong
            // or we don't have permissions on this entry.
            perror("stat");
            exit(1);
        } 
        num_proc++;
            
	}
    closedir(dirp);
	/* For each entry in the directory, eliminate . and .., and check
	* to make sure that the entry is a directory, then call run_worker
	* to process the index file contained in the directory.
    */
	int p[num_proc][2];
    int parent_p[num_proc][2];

    char buf[MAXWORD]; // buffer for reading in a word
    int rsize; // read size
    pid_t pid;
	if((dirp = opendir(startdir)) == NULL) {
		perror("opendir");
		exit(1);
	}
    fprintf(stderr, "Enter a word: \n");
    // read from a word from STDIN until it is closed
    while ((rsize = read(STDIN_FILENO, &buf, MAXWORD)) > 0) {
        FreqRecord *master_freq = malloc(MAXRECORDS*(sizeof(FreqRecord)));
        if (master_freq == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        // initialize the master_freq array with a terminal FreqRecord of 0 freq
        FreqRecord *last = malloc(sizeof(FreqRecord));
        last->freq = 0;
        strncpy(last->filename, "", 1);
        last->filename[strlen(last->filename)-1] = '\0';
        master_freq[0] = *last;
        FreqRecord *freq_buf = malloc(sizeof(FreqRecord)); // buffer to read in a FreqRecord
        int cur_proc = 0;
        if (freq_buf == NULL) {
            perror("malloc");
            exit(EXIT_FAILURE);
        }
        if((dirp = opendir(startdir)) == NULL) {
		    perror("opendir");
		    exit(1);
	    }
        // for every subdirectory, run the worker and write the FreqRecords back to the master
        while((dp = readdir(dirp)) != NULL) {

            if(strcmp(dp->d_name, ".") == 0 || 
            strcmp(dp->d_name, "..") == 0 ||
            strcmp(dp->d_name, ".svn") == 0){
                continue;
            }
            strncpy(path, startdir, PATHLENGTH);
            strncat(path, "/", PATHLENGTH - strlen(path) - 1);
            strncat(path, dp->d_name, PATHLENGTH - strlen(path) - 1);

            struct stat sbuf;
            if(stat(path, &sbuf) == -1) {
                //This should only fail if we got the path wrong
                // or we don't have permissions on this entry.
                perror("stat");
                exit(1);
            }
            if (S_ISDIR(sbuf.st_mode)) {
                // create a 2 pipes for every subdirectory
                // to communicate the master process with the workers
                if (pipe(p[cur_proc]) == -1){
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
                if (pipe(parent_p[cur_proc]) == -1) {
                    perror("pipe");
                    exit(EXIT_FAILURE);
                }
                // write the word to the current process pipe and close it
                write(p[cur_proc][1], &buf, MAXWORD);
                close(p[cur_proc][1]);
                pid = fork();
                if (pid == -1) {
                    perror("fork");
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0) { // child, runs the worker to get the FreqRecords
                    close(p[cur_proc][1]);
                    run_worker(path, p[cur_proc][0], parent_p[cur_proc][1]);
                    close(parent_p[cur_proc][1]);
                    exit(EXIT_SUCCESS);
                }
                else { // parent, master process, collects the FreqRecords
                    close(p[cur_proc][0]);
                    wait(NULL);
                    int rrsize;
                    close(parent_p[cur_proc][1]);
                    rrsize = read(parent_p[cur_proc][0], freq_buf, sizeof(FreqRecord)); // read the FreqRecords from each worker
                    while (freq_buf->freq != 0 && rrsize > 0) {
                        insert_sort(master_freq, freq_buf);
                        rrsize = read(parent_p[cur_proc][0], freq_buf, sizeof(FreqRecord));
                    }
                    close(parent_p[cur_proc][0]);

                    cur_proc++;
                }
            }
        }
        print_freq_records(master_freq);
        if (master_freq[0].freq == 0){
            fprintf(stderr, "Could not find the word %s", buf);
        }
        free(master_freq);
        free(freq_buf);
        free(last);
        closedir(dirp);
        fprintf(stderr, "Enter a word: \n");
        memset(buf, 0, sizeof(buf));
    }
	
	return 0;
}
