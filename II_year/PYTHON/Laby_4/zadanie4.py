import argparse
import os 
import random 


parser = argparse.ArgumentParser()
parser.add_argument('data', metavar='N', type=str, nargs='*',
                    help='a list of strings')
args = parser.parse_args()



months = ["styczeń","luty","marzec","kwiecień","maj","czerwiec","lipiec","sierpień",
          "wrzesień","październik","listopad","grudzień"]
days = {"pn" : ["poniedziałek"],
        "wt" : ["wtorek"],
        "śr" : ["środa"],
        "czw" : ["czwartek"],
        "pt" : ["piątek"],
        "sob" : ["sobota"],
        "nd" : ["niedziela"],
        "pn-nd" : ["poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela"],
        "pn-sob" : ["poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota"],
        "pn-pt" : ["poniedziałek", "wtorek", "środa", "czwartek", "piątek"],
        "pn-czw" : ["poniedziałek", "wtorek", "środa", "czwartek"],
        "pn-sr" : ["poniedziałek", "wtorek", "środa"],
        "pn-wt" : ["poniedziałek", "wtorek"],
        "wt-nd" : ["wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela"],
        "wt-sob" : ["wtorek", "środa", "czwartek", "piątek", "sobota"],
        "wt-pt" : ["wtorek", "środa", "czwartek", "piątek"],
        "wt-czw" : ["wtorek", "środa", "czwartek"],
        "wt-śr" : ["wtorek", "środa"],
        "śr-nd" : ["środa", "czwartek", "piątek", "sobota", "niedziela"],
        "śr-sob" : ["środa", "czwartek", "piątek", "sobota"],
        "śr-pt" : ["środa", "czwartek", "piątek"],
        "śr-czw" : ["środa", "czwartek"],
        "czw-nd" : ["czwartek", "piątek", "sobota", "niedziela"],
        "czw-sob" : ["czwartek", "piątek", "sobota"],
        "czw-pt" : ["czwartek", "piątek"],
        "pt-nd" : ["piątek", "sobota", "niedziela"],
        "pt-sob" : ["piątek", "sobota"],
        "sob-nd" : ["sobota", "niedziela"],
        }
timeOfTheDay = {"r" : "rano",
                "w" : "wieczorem"
                }


monthsArg = []
daysArg = []
timeOfTheDayArg = []




for i in args.data:
    i = i.replace("[", "")
    i = i.replace("]","")
    i = i.split(",")
    for j in i :
        if(j in months):
            monthsArg.append(j)
        if(j in days): 
            daysArg.append(days.get(j))
        if(j in timeOfTheDay):
            timeOfTheDayArg.append(timeOfTheDay.get(j)) 

Letters = ['A','B','C']

# tworzę foldery
for i in months:
    directory = i
    parent_dir = "/home/students/mismap/m/mz438834/II_ROK/PYTHON/Laby_4"
    path = os.path.join(parent_dir, directory)  
    os.mkdir(path)
    for j in ["poniedziałek", "wtorek", "środa", "czwartek", "piątek", "sobota", "niedziela"]:
        sub_directory = j
        sub_path = os.path.join(path, sub_directory)
        os.mkdir(sub_path)
        for time in ["rano", "wieczorem"]:
            sub_sub_directory = time
            sub_sub_path = os.path.join(sub_path, sub_sub_directory)
            os.mkdir(sub_sub_path)
            f = open(os.path.join(sub_sub_path, "Solutions.csv"), "w")
            f.write(" Model; Output value; Time of computation; \n")
            n = random.randint(0,2)
            letter  = Letters[n]
            num  = random.randint(0,1000)
            time = random.randint(0,1000)
            f.write(" " + letter + " ; " + str(num) + " ; " + str(time) + "s ; ")
            f.close()



# pierwsza część zadania z wypisywaniem 
count = 0      
for i in range(len(daysArg)):
    month = monthsArg[i]
    for day in daysArg[i]:
        if count >= len(timeOfTheDayArg):
            time = "rano (domyślnie)"
        else :
            time = timeOfTheDayArg[count]
        print(month + " " + day + " " + time)  
        count += 1      

# druga cześć zadania z zliczaniem czasów
count = 0 
sum = 0     
for i in range(len(daysArg)):
    month = monthsArg[i]
    for day in daysArg[i]:
        if count >= len(timeOfTheDayArg):
            time = "rano"
        else :
            time = timeOfTheDayArg[count]
        directory = "/home/students/mismap/m/mz438834/II_ROK/PYTHON/Laby_4/" + month + "/" + day + "/" + time + "/Solutions.csv"
        f = open(directory, "r")
        f.readline()
        line = f.readline()
        tokens = line.split(" ")
        for token in tokens:
            if 's' in token:
               sum += int(token.replace("s", "")) 
        count += 1 
print(sum)
         