lista = [1,"to", 23, "nie", [12,[],[[2]]], "było", -12, "trudne", [1,2,[5]]]

def printList(lista, suma) :
    for element in lista:
        if isinstance(element, int) : suma[0] += element
        if isinstance(element, list): printList(element,suma)
    
suma = [0]
printList(lista=lista,suma=suma)
print(suma[0])