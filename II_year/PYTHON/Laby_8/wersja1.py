import math
import sys
import random
import time
import tracemalloc


class Ułamek:
    # Wersja2 __slots__ = ('gcd', 'licznik', 'mianownik') 

    def __init__(self, licznik, mianownik):
        assert mianownik != 0, "Mianownik nie może być zerem."
        gcd = math.gcd(licznik, mianownik)
        self.licznik = licznik // gcd
        self.mianownik = mianownik // gcd

    def __add__(self, other):
        return Ułamek(self.licznik * other.mianownik + other.licznik * self.mianownik, self.mianownik * other.mianownik)

    def __sub__(self, other):
        return Ułamek(self.licznik * other.mianownik - other.licznik * self.mianownik, self.mianownik * other.mianownik)

    def __mul__(self, other):
        return Ułamek(self.licznik * other.licznik, self.mianownik * other.mianownik)

    def __truediv__(self, other):
        assert other.licznik != 0, "Nie można dzielić przez zero."
        return Ułamek(self.licznik * other.mianownik, self.mianownik * other.licznik)

    def __eq__(self, other):
        return self.licznik == other.licznik and self.mianownik == other.mianownik

    def __ne__(self, other):
        return not self.__eq__(other)

    def __lt__(self, other):
        return self.licznik * other.mianownik < other.licznik * self.mianownik

    def __le__(self, other):
        return self.licznik * other.mianownik <= other.licznik * self.mianownik

    def __gt__(self, other):
        return self.licznik * other.mianownik > other.licznik * self.mianownik

    def __ge__(self, other):
        return self.licznik * other.mianownik >= other.licznik * self.mianownik

    def __str__(self):
        return f"{self.licznik}/{self.mianownik}"



def generujUłamki():
    mianownik = random.randint(1, 100)    
    licznik = random.randint(1, 100)  
    return Ułamek(licznik,mianownik)


def main():
    if len(sys.argv) != 3:
        print("Zła liczba argumentów")
        sys.exit(1)  

    n = int(sys.argv[1])
    k = int(sys.argv[2])

   
    
    start_time = time.time()
    tracemalloc.start()
    beforeMemory = tracemalloc.get_traced_memory()

    ułamki = [generujUłamki() for i in range(n + 10**6)]
    n = len(ułamki)

    
    
    afterMemory = tracemalloc.get_traced_memory()

    print(f"Memory used: {afterMemory[0]-beforeMemory[0]}")
    tracemalloc.stop()

    for i in range(k + 10**7):
        k += 1
        ułamki[i % n] = ułamki[i % n] + ułamki[(i + 1) % n]

    
    
    end_time = time.time()

    elapsed_time = end_time - start_time
    print(f"Execution time: {elapsed_time} seconds")


 



if __name__ == "__main__":
    main()
    



