#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct stringMatch {
    uint32_t M = 1e9 + 7;
    uint32_t P = 31;
    vector<uint64_t> pat_pow;
    string &pattern;
    uint64_t hashcode = 0;
    void computeHash() {
        //precompute pow_2
        pat_pow[pat_pow.size() - 1] = 1;
        for (int i = pat_pow.size() - 2; i >= 0; i--) {
            pat_pow[i] = (pat_pow[i + 1] * P)%M;
        }
        for (int i = 0; i < pattern.size(); i++) {
            hashcode = (hashcode + (pat_pow[i] * (pattern[i] - 'a')))%M;
        }
        //cout << "hashcode = " << hashcode << endl;
    }
    stringMatch(string &pattern) : pattern(pattern) {
        pat_pow.resize(pattern.size(), 0);
        computeHash();
    }

    bool checkStringMatch(string &text, int idx) {
        if (idx + pattern.size() > text.size())
            return false;
        for (int i = idx; i < idx + pattern.size(); i++) {
            if (pattern[i - idx] != text[i])
                return false;
        }

        return true;
    }

    bool checkContainsPattern(string &text) {
        int i = 0, l = pattern.size();
        uint64_t temp_hash = 0;
        if (text.size() < pattern.size()) {
            return false;
        }
        for (; i < pattern.size(); i++) {
            temp_hash = (temp_hash + (text[i] - 'a')*pat_pow[i])%M;
        }

       // cout << temp_hash << " " << hashcode << endl;

        if (temp_hash == hashcode) {
            if (checkStringMatch(text, 0)) {
                return true;
            }
        }

        for (; i < text.size(); i++) {
            temp_hash = (temp_hash - pat_pow[0] * (text[i - l] - 'a') + M)%M;
            temp_hash = (temp_hash * P)%M;
            temp_hash = (temp_hash + (text[i] - 'a'))%M;
            //cout << temp_hash << endl;
            if (temp_hash == hashcode) {
                if (checkStringMatch(text, i - l + 1))
                    return true;
            }
        }

        return false;
    }
};

int main(int argc, char **argv) {
    if (argc != 3) {
        cout << "Invalid number of arguments: " << "Pattern and String" << endl;
        return -1;
    }

    string pattern(argv[1]);
    string str(argv[2]);

    stringMatch matcher(pattern);
    bool result = matcher.checkContainsPattern(str);
    cout << "found = " << result << endl;

    return 0;
}