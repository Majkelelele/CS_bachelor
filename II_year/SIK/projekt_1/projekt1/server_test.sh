#!/bin/bash


# Define an array of ports
ports=(3116 3111 3135 3333)





# Loop through the ports and files
for protocol in "udp" "udp" "tcp"; do

    for ((i=0; i<${#ports[@]}; i++)); do
        port="${ports[$i]}"
        

        timeout 20 ./ppcbs "$protocol" "$port" > server_output.txt 2>/dev/null

        
        echo "Execution time with port $port"
    done
done





