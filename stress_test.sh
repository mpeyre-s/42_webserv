#!/bin/bash

URL="http://127.0.0.1:8080/cgi/hello.py?name=Alice"
NUM_REQUESTS=1000
CONCURRENCY=100

for ((i=1; i<=NUM_REQUESTS; i++))
do
    curl -s -o /dev/null -w "%{http_code}\n" "$URL" &

    # Limite le nombre de jobs en parallèle
    if (( i % CONCURRENCY == 0 )); then
        wait
    fi
done

wait
echo "Stress test terminé."

