#!/bin/bash
pids=""
apps=""

if [[ $# -ne 0 ]]; then
    for x in $*; do
        apps="$apps $x.main"
    done
else
    for x in `find ./ -name main.py -printf "%P\n"|tr "/" "."`; do
        apps="$apps ${x%.py}"
    done
fi

for app in $apps; do
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
