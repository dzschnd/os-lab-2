# !/bin/bash

echo "=== Starting IO Lat Read Benchmark without caching ===" > logs/io_lat_read_log.txt
strace -c exec/io_lat_read cache_impl/cache_impl.cpp 100 0 >> logs/io_lat_read_log.txt 2>&1

echo -e "\n\n=== Starting IO Lat Read Benchmark with caching ===" >> logs/io_lat_read_log.txt
strace -c exec/io_lat_read cache_impl/cache_impl.cpp 100 1 >> logs/io_lat_read_log.txt 2>&1

echo "Output written to logs/io_lat_read_log.txt"


echo "=== Starting IO Lat Write Benchmark without caching ===" > logs/io_lat_write_log.txt
strace -c exec/io_lat_write file_to_write.txt 100 0 >> logs/io_lat_write_log.txt 2>&1

echo -e "\n\n=== Starting IO Lat Write Benchmark with caching ===" >> logs/io_lat_write_log.txt
strace -c exec/io_lat_write file_to_write.txt 100 1 >> logs/io_lat_write_log.txt 2>&1

echo "Output written to logs/io_lat_write_log.txt"