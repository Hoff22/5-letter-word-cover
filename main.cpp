#include <bits/stdc++.h>
#include <concurrent_vector.h>
#include <fstream>
#include <chrono>

#define fi first
#define se second
#define MOD 1000000007
#define N 67108864
 
using namespace std;

bool seen[N+1];

vector<string> open_file(string file_name, int word_size){
    ifstream file(file_name);

    if(!file.is_open()){
        cerr << "couldn't open file" << endl;
    }

    vector<string> s;

    string st;
    while(file >> st){
        string st_clean = "";
        for(auto& c : st){
            if(c >= 'A' and c <= 'Z'){
                c += ('a'-'A');
            }
            if(c >= 'a' and c <= 'z'){
                st_clean.push_back(c);
            }
        }
        if(st_clean.size() != word_size) continue;
        s.push_back(st_clean);
    }

    return s;
}

int getWordHash(string& s){
    int res = 0;

    for(int i = 0; i < s.size(); i++){
        int idx = s[i] - 'a';
        res |= (1<<idx);
    }

    return res;
}

int hashPopCount(int hs){
    return __builtin_popcount(hs);
}

#define LOG_MAX 25

struct bit_trie{
    int sz, child[2];
    int string_idx;

    bit_trie(){
        this->sz = 0;
        this->child[0] = this->child[1] = -1;
    }
};

// O(log(N));
void insert(int n, int w_idx, vector<bit_trie>& t){

    int cur = 0;

    for(int i = LOG_MAX; i >= 0; i--){
        
        t[cur].sz++;

        if(t[cur].child[0] == -1){
            t[cur].child[0] = t.size();
            t.push_back(bit_trie());
            t[cur].child[1] = t.size();
            t.push_back(bit_trie());
        }

        cur = t[cur].child[(n>>i) & 1];
    }

    t[cur].sz++;
    t[cur].string_idx = w_idx;
}



int main(int argc, char** argv){ 
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    assert(argc > 1);

    auto begin = chrono::high_resolution_clock::now();

    vector<string> words = open_file(argv[1], 5);
    sort(words.begin(), words.end());
    
    int big_n = words.size();
    
    vector<pair<int,int>> words_clean(words.size(), {0,-1});

    memset(seen, 0, sizeof(seen));

    for(int i = 0; i < big_n; i++){
        int word_hash = getWordHash(words[i]);        
        if(hashPopCount(word_hash) != 5) continue;
        if(seen[word_hash]) continue;
        seen[word_hash] = 1;
        words_clean[i] = {word_hash, i};
    }

    int last_zero = -1;
    for(int i = 0; i < big_n; i++){
        if(words_clean[i].fi == 0 and last_zero == -1) last_zero = i;
        else if(words_clean[i].fi != 0){
            if(last_zero != -1){
                swap(words_clean[last_zero], words_clean[i]);
                last_zero++; 
            }
        }
    }

    int n = 0;
    for(int i = 0; words_clean[i].fi > 0; i++) n++;

    auto end = chrono::high_resolution_clock::now();
    cout << "-> words excluding anagrams: " << n << endl;
    cout << "-> time taken: " << chrono::duration_cast<chrono::milliseconds>(end-begin).count() << "ms" << endl;

    return 0;
}