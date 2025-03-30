
#!/bin/bash

# Define an array of ports for UDP, UDPR, and TCP
ports=(3117)

# Define an array of file names
files=("input_example")

# Loop through the ports and files
for protocol in "tcp" ; do
    for ((i=0; i<${#ports[@]}; i++)); do
        port="${ports[$i]}"
        file="${files[$i]}"
        
        echo "Testing with protocol $protocol, port $port, and file $file"
        
        # Measure start time
        start=$(date +%s.%N)
        
        # Run client with the current protocol, port, and file
        ./ppcbc "$protocol"  0.0.0.0 "$port" < "$file"


        # Wait for client to finish
        client_pid=$!
        wait $client_pid

        end=$(date +%s.%N)

        # Calculate execution time
        runtime=$(echo "scale=3; ($end - $start) / 1" | bc)

        # Print execution time
        echo "Execution time with protocol $protocol, port $port, and file $file: $runtime seconds"

        # Compare output
        # diff "server_output.txt" "tests_file/$file"
        if diff -q "server_output.txt" "$file" >/dev/null; then
            echo "    Output matches for file $file"
        else
            echo "    Output differs for file $file"
        fi

        # Clean up
        rm "server_output.txt"
        
        # Measure end time
        
    done
done


