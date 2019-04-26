
for dir in books  dir1  dir10  dir2  dir3  dir4  dir5  dir6  dir7  dir8  dir9  etext00  t1; do
    echo $dir;
    cd $dir;
    rm index;
    rm filenames;
    ../../indexer;
    cd ..;
done
