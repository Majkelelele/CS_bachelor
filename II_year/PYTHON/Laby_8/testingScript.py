import subprocess
import time
import resource
import tracemalloc





def main():
   

    
    print("Version 1 without __slots__: ")
    subprocess.run(["python3", "wersja1.py", "100", "100"]) 

    

    command = 'perl -p -e "s/# Wersja2 //" wersja1.py > wersja2.py'
    subprocess.run(command, shell=True, check=True)



    print("Version 2 with __slots__: ")
    subprocess.run(["python3", "wersja2.py", "100", "100"]) 

if __name__ == "__main__":
    main()
