import random
amount = 100
count = 0
i = 0

while(i < amount):
    x = random.uniform(0, 1)
    y = random.uniform(0, 1)
    if(x**2 + y**2 <= 1):
        count += 1
    i += 1   

print(count/amount*4)