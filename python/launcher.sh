#!/bin/bash
pids=""
for i in `find ./ -name main.py -printf "%P\n"|tr "/" "."`; do
    app=${i%.py}
    python -m $app &
    pids="$pids $!"
done

function killall {
    for pid in $pids; do
        kill $pid
    done
}

trap "killall" SIGINT

wait $pids
