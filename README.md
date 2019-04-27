# Simple C Search Engine
***Note**: For best results, test this on the `testing/big` directory that already contains the required files*

A simple search engine for a systems programming course written in C.

This is a very simple search engine that looks for a word in `.txt` files and outputs the frequency and the `.txt` files that contains the word sorted from highest to lowest frequency.

It creates index files from the `.txt` files and creates an array of `FreqRecords` that holds the frequency of the word, and the name of the file in which it found the word using the following struct.

```C
#define PATHLENGTH  128

typedef struct {
  int freq;
  char filename[PATHLENTH];
} FreqRecord;
```

The parent process forks for every subdirectory containing `.txt` files. Each child process writes back to the parent a `FreqRecord`. The parent reads each `FreqRecord` and creates an array of `FreqRecords` ordered by the `.txt` file that contains the highest occurrence of the given word.

## To Build
Simply call make on the working directory.
```bash
$  make
```
To remove the executables and object files simply clean the directory using:
```bash
make clean
```

## To create index files
This will create the `index` files and `filenames` needed to run the search engine in the current directory.
```bash
$  ./indexer
```
This also supports a `-d` option to create the `index` and `filenames` in a specific directory.
```bash
./indexer -d testing/big/
```

## To Run
After building, run the executable `query`. This will look for subdirectories containing `index` files and `filenames` in the current directory.
```bash
$  ./query
```

To run on a specific directory use the `-d` option (recommended to use this to try on the following directory).
```bash
$  ./query -d testing/big/
```

## An example on the word 'fire'
```bash
$  ./query -d testing/big/
Enter a word:
fire
245    ./ws110.txt
221    ./fire.txt
177    ./quo.txt
89    ./desert.txt
80    ./cstwy11.txt
71    ./cross.txt
69    ./truss10.txt
44    ./utrkj10.txt
39    ./mohwk10.txt
33    ./mbova10.txt
25    ./poe5v10.txt
24    ./8year10.txt
21    ./wflsh10.txt
21    ./wflsh10.txt
20    ./hphnc10.txt
19    ./2yb4m10.txt
18    ./poe1v10.txt
18    ./poe2v10.txt
15    ./poe4v10.txt
13    ./esymn10.txt
12    ./cyrus10.txt
9    ./mpolo10.txt
9    ./poe3v11.txt
9    ./icfsh10.txt
7    ./rbddh10.txt
6    ./8rbaa10.txt
6    ./bgita10.txt
5    ./andsj10.txt
5    ./cptcr11a.txt
4    ./utopi10.txt
3    ./beheb10.txt
3    ./rlchn10.txt
3    ./sign410.txt
2    ./chldh10.txt
2    ./dmsnd11.txt
2    ./remus10.txt
2    ./shkdd10.txt
1    ./hioaj10.txt
```

End the execution by closing stdin `ctrl-D` or an interrupt `ctrl-C`
