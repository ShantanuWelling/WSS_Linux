#!/bin/bash

# Run the executable in the background
./"$1" > /dev/null &
# Get the PID of the most recent background command
pid=$!
for i in {1..20};
do
    ./wss1 $pid 1 | tail -1 >> ./results/$1-wss1-moving.txt
done
kill $pid

./"$1" > /dev/null &
pid=$!
for i in {1..20};
do
    ./wss.pl $pid 1 | tail -1 >> ./results/$1-wsspl-moving.txt
done
kill $pid

