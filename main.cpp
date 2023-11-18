#include <bits/stdc++.h>
#include <fstream>
#include <chrono>
#include "trie.h"

#define fi first
#define se second
#define MOD 1000000007
#define N ((1<<27)-1)
#define MAX_W 700000
 
using namespace std;

bool seen[N+1];
vector<string> words;
pair<int,int> words_clean[MAX_W];

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

int getWordHash(const string& s){
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

int lsb(int mask) {
    return mask & (-mask);
}

int main(int argc, char** argv){ 
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    assert(argc > 1);

    auto begin = chrono::high_resolution_clock::now();

    words = open_file(argv[1], 5);
    
    int big_n = words.size();

    // <------------- removing anagrams --------------->

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

    // <--------------- find pairs --------------->

    vector<pair<int,int>> pairs;
    vector<pair<int,int>> pairs_map(N+1, {-1,-1});
    set<pair<int,int>> seen_set;

    for(int i = 0; i < n; i++){
        for(int j = i+1; j < n; j++){
            if((words_clean[i].fi & words_clean[j].fi) == 0){
                pairs.push_back({i,j});
            }
        }
    }

    vector<bit_trie> bt = {bit_trie()};

    for(int i = 0; i < pairs.size(); i++){
        int a,b;
        tie(a,b) = pairs[i];
        int hs = words_clean[a].fi | words_clean[b].fi;
        insert(hs, i, bt);
        pairs_map[hs] = {a, b};
    }

    // <--------------- finding quadruples --------------->

    vector<tuple<int,int,int,int>> quads(N+1, {-1,-1,-1,-1});
    
    long long cnt_d = 0;

    #pragma omp parallel for
    for(auto [a,b] : pairs){
        int hs = words_clean[a].fi | words_clean[b].fi;

        int mask = N-hs;

        vector<int> submasks;
        for (int submask = mask; submask; submask = (submask - 1) & mask) submasks.push_back(submask);

        #pragma omp parallel for
        for (int j = 0; j < submasks.size(); j++) {
            int sub = submasks[j];
            if(__builtin_popcount(sub) != 10) continue;
            int c,d;
            tie(c, d) = pairs_map[sub];
            if(c == -1) continue;
            while(get<0>(quads[cnt_d]) != -1) cnt_d++;
            quads[cnt_d] = {a,b,c,d};
            cnt_d++;
        }   
        // if(cnt_d % 10000 == 0){
        //     auto end = chrono::high_resolution_clock::now();
        //     cout << "-> time taken: " << chrono::duration_cast<chrono::milliseconds>(end-begin).count() << "ms " << cnt_d << endl;
        // }
        // cnt_d++;
    }

    // <--------------- timing measures --------------->

    auto end = chrono::high_resolution_clock::now();
        
    // char ant = 0;
    // for(int i = 0; i < n; i++){
    //     if(words[words_clean[i].se][0] != ant){
    //         cout << words[words_clean[i].se][0];
    //     }
    //     ant = words[words_clean[i].se][0];
    // }
    // cout << "\n";

    // for(auto [i, j] : pairs){
    //     cout << words[words_clean[i].se] << ", " << words[words_clean[j].se] << endl;
    // }

    cout << "\n";

    cout << "-> words excluding anagrams: " << n << endl;
    cout << "-> number of pairs found: " << pairs.size() << endl;
    cout << "-> number of nodes in trie: " << bt.size() << endl;
    cout << "-> time taken: " << chrono::duration_cast<chrono::milliseconds>(end-begin).count() << "ms" << endl;

    return 0;
}