#!/bin/bash

#afl-fuzz -m 1000 -t 5000 -i in -o out ./afl @@
#AFL_PERSISTENT=1
gnome-terminal -e 'afl-fuzz -m 2000 -t 5000 -i in -o out -M fuzzer1 ./pcbnew/pcbnew @@'
sleep 10
gnome-terminal -e 'afl-fuzz -m 2000 -t 5000 -i in -o out -M fuzzer2 ./pcbnew/pcbnew @@'
gnome-terminal -e 'afl-fuzz -m 2000 -t 5000 -i in -o out -M fuzzer3 ./pcbnew/pcbnew @@'
gnome-terminal -e 'afl-fuzz -m 2000 -t 5000 -i in -o out -M fuzzer4 ./pcbnew/pcbnew @@'



