import math

class Ułamek:
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

# Przykłady testowe
ułamek1 = Ułamek(1, 2)
ułamek2 = Ułamek(3, 4)

wynik_dodawania = ułamek1 + ułamek2
print(f"Dodawanie: {ułamek1} + {ułamek2} = {wynik_dodawania}")

wynik_odejmowania = ułamek1 - ułamek2
print(f"Odejmowanie: {ułamek1} - {ułamek2} = {wynik_odejmowania}")

wynik_mnożenia = ułamek1 * ułamek2
print(f"Mnożenie: {ułamek1} * {ułamek2} = {wynik_mnożenia}")

wynik_dzielenia = ułamek1 / ułamek2
print(f"Dzielenie: {ułamek1} / {ułamek2} = {wynik_dzielenia}")

wynik_równości = ułamek1 == ułamek2
print(f"Równość: {ułamek1} == {ułamek2} = {wynik_równości}")



