import math
import random
import os
import re
import pytest
import unittest
from unittest.mock import Mock



class Ułamek:
    # Wersja2 __slots__ = ('gcd', 'licznik', 'mianownik') 

    def __init__(self, licznik, mianownik):
        assert mianownik != 0, "Mianownik nie może być zerem."
        gcd = math.gcd(licznik, mianownik)
        self.licznik = licznik // gcd
        self.mianownik = mianownik // gcd
    # mozliwosc zapisywania
    # def printToFile(self, filename): 
    #     sourceFile = open(filename, 'w')
    #     print(self.__str__, file = sourceFile)
    #     sourceFile.close()

    def zapisz_do_pliku(self, nazwa_pliku):
        with open(nazwa_pliku, 'w') as file:
            file.write(f"{self.licznik}/{self.mianownik}")

    def wczytaj_z_pliku(self, nazwa_pliku):
        with open(nazwa_pliku, 'r') as file:
            data = file.read()
            licznik, mianownik = map(int, data.split('/'))
            return Ułamek(licznik, mianownik)
        
    def convertToFloat(self):
        return self.licznik/self.mianownik

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


# mozliwosc czytania
def read(filename):
    with open(filename, 'r') as file:
        data = file.read()    
    numbers = re.findall(r'\d+', data)
    numbers    




# przykładowe testy parametryzowane
@pytest.mark.parametrize( "a, b, expected",  
[  
    (Ułamek(1,6), Ułamek(1,5), Ułamek(11,30)),  
    (Ułamek(2,7), Ułamek(3,7), Ułamek(5,7)),  
],)  
def test_addition(a, b,expected):  
    assert a.__add__(b) == expected

@pytest.mark.parametrize( "a, b, expected",  
[  
    (Ułamek(1,6), Ułamek(1,5), Ułamek(1,30)),  
    (Ułamek(2,7), Ułamek(3,7), Ułamek(6,49)),  
],)  
def test_multiplication(a, b,expected):  
    assert a.__mul__(b) == expected




class TestUłamek(unittest.TestCase):
    def setUp(self):
        # Przygotowanie mocka pliku
        self.mock_file = Mock()

    def test_str(self):
        ułamek = Ułamek(1, 2)
        self.assertEqual(str(ułamek), "1/2")

    def test_zapisz_do_pliku(self):
        ułamek = Ułamek(3, 4)
        ułamek.zapisz_do_pliku('test_plik.txt')
        self.mock_file.write.assert_called_once_with('3/4')

    def test_wczytaj_z_pliku(self):
        self.mock_file.read.return_value = '5/6'
        with unittest.mock.patch('builtins.open', return_value=self.mock_file):
            ułamek = Ułamek.wczytaj_z_pliku('test_plik.txt')
            self.assertEqual(ułamek.licznik, 5)
            self.assertEqual(ułamek.mianownik, 6)

    def tearDown(self):
        # Sprzątanie po testach - usunięcie pliku testowego
        if os.path.exists('test_plik.txt'):
            os.remove('test_plik.txt')


if __name__ == '__main__':
    unittest.main()

    
    





