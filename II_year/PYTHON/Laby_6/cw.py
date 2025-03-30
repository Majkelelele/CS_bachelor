from functools import *
from itertools import *
import operator
import inspect  

# zadanie 1
lista = [1,1,1,2,2,2,2,1,1,3,4,5,5,5]
g = groupby(lista)
def fun(p):
    return len(p)
print(len(max(map(lambda p: list(p[1]), g), key=fun)))


# zadanie 2
def maps(a):
    return (a,a+1)
def starmaps(*a):
    return(a[0],a[1],operator.pow(a[0],a[1]))
iterator = map(maps,range(1,7))
result = list(starmap(starmaps,iterator))
print(result)

# zadanie 3 part1
def zrob_drzewo(length,word):
    if(len(word) > 0):
        lewe = zrob_drzewo(length//2,operator.itemgetter(slice(0,length//2))(word))
        prawe = zrob_drzewo(length//2,operator.itemgetter(slice(length//2+1,length))(word))
        return(lewe,word[length//2],prawe)
print((zrob_drzewo(7,"alakota")))


# zadanie 3 part2
def zrobIterator(preorder,inorder,postorder,tree):
    if(tree is not None and len(tree) > 0):
        yield from zrobIterator(preorder,inorder,postorder,tree[0])
        yield tree[1]
        yield from zrobIterator(preorder,inorder,postorder,tree[2])
print(list(zrobIterator(0,0,0,zrob_drzewo(7,"alakota"))))

# zadanie 4
def info(func):
    def wrapper(*args):
        print(f"Function {func.__name__} called with arguments:")
        print(f"arguments: {args}")
        result = func(*args)
        print(f"Function {func.__name__} returned: {result}")
        return result

    return wrapper

# Przykład użycia dekoratora
@info
def add(x, y):
    return x + y

result = add(3, 5)


