class Osoba:
    imię = "Ala"
    nazwisko = "Elementarzowa"
    
    def przedstaw_się(self):
        return self.imię + " " + self.nazwisko


os = Osoba()
print(os.przedstaw_się())