#!/bin/bash

ports=(3100)
# Loop through the ports and files
for protocol in "tcp"; do

    for ((i=0; i<${#ports[@]}; i++)); do
        port="${ports[$i]}"
        

        ./ppcbs "$protocol" "$port" > server_output.txt 2>/dev/null

        
        echo "Execution time with port $port"
    done
done