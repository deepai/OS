#include <iostream>
#include <string>
#include <vector>

using namespace std;

vector<int> computeLPSArray(string &pattern) {
    vector<int> LPS(pattern.size(), 0);
    LPS[0] = 0;
    int l = 0;
    for (int i = 1; i < pattern.size();) {
        if (pattern[l] == pattern[i]) {
            l++;
            LPS[i] = l;
            i++;
        } else if (l != 0) {
            l = LPS[l - 1];
        } else {
            LPS[i] = 0;
            i++;
        }
    }

    return LPS;
}

vector<int> findMultipleMatches(string &str, vector<int> LPS) {
    vector<int> matches;
    int l = 0;
    for (int i = 0; i < str.size();) {
        if (str[i] == str[l]) {
            l++;
            i++;
            if (l == LPS.size()) {
                matches.push_back(i - l);
                l = l - 1;
            }
        } else if (l != 0) {
            l = LPS[l - 1];
        } else {
            i++;
        }
    }

    return matches;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Invalid number of arguments: " << "Pattern and String" << endl;
        return -1;
    }

    string pattern(argv[1]);
    string str(argv[2]);

    vector<int> LPS = computeLPSArray(pattern);

    vector<int> matches = findMultipleMatches(str, LPS);
    for (auto &each_match_idx : matches) {
        cout << "Match found at " << each_match_idx << endl;
    }

    return 0;
}