#include <iostream>
#include <vector>
using namespace std;
vector<char> input;
int find_first_letter() {
    int first = -1;
    for(int i = 0; first < 0 && i < input.size(); i++) {
        if(input[i] != '*') first = i;
    }
    return first;
}

int main() {
    char ch;
    while (!cin.eof()) {
        cin.get(ch);
        if (ch == '\n') {
            break;
        }
        input.push_back(ch);
    }
    if(input.size() == 0) cout << 0;
    else{
        int min_seperation = input.size();
        int i = find_first_letter();
        if(i >= 0) {
            for(int j = i+1; j < input.size(); j++) {
                if(input[j] != '*') {
                    if(input[j] == input[i]) {
                        i = j;
                    }
                    else{
                        min_seperation = min(min_seperation, j-i);
                        i = j;
                    }
                }
            }
        }
        std::cout << input.size() - min_seperation + 1;   
    }
    return 0;
}
