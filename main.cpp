#include <bits/stdc++.h>
#include <fstream>

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

#define LOG_MAX 25

struct bit_trie{
    int sz, child[2], c, has;
    vector<int> strs;

    bit_trie(int _has = -1){
        this->sz = 0;
        this->c = -('a');
        this->has = _has;
        this->child[0] = this->child[1] = -1;
    }
};

vector<bit_trie> sl = {bit_trie()};
vector<bit_trie> db = {bit_trie()};
vector<bit_trie> qd = {bit_trie()};

// O(log(N));
void insert(int n, int w_idx, vector<bit_trie>& t){

    int cur = 0;

    for(int i = LOG_MAX; i >= 0; i--){
        
        t[cur].sz++;
        t[cur].c = i;

        if(t[cur].child[0] == -1){
            t[cur].child[0] = t.size();
            t.push_back(bit_trie(0));
            t[cur].child[1] = t.size();
            t.push_back(bit_trie(1));
        }

        cur = t[cur].child[(n>>i) & 1];
    }

    t[cur].c = -1;
    t[cur].sz++;
    t[cur].strs.push_back(w_idx);
}

vector<pair<string, string>> ps;
vector<pair<string, string>> quads;
vector<tuple<string,string,string,string, string>> quints;

void maxMatch(int i, int cur, int cnt,
    vector<bit_trie>& t,
    vector<pair<int, string>>& words,
    vector<pair<string, string>>& store)
{
    if(t[cur].sz == 0) return;
    
    // cout << (char)(t[cur].c + 'a') << " " << t[cur].has << ": " << t[cur].sz << endl;

    int n = words[i].fi;
    int mn = __builtin_popcount(n);
    
    if(cnt > 2*mn) return;

    int idx = t[cur].c;
    
    if(idx == -1){
        if((n & words[t[cur].strs[0]].fi) == 0 and __builtin_popcount((n | words[t[cur].strs[0]].fi)) >= 2*mn){
            store.push_back({words[i].se , words[t[cur].strs[0]].se});
        }
        return;
    }

    maxMatch(i, t[cur].child[0], cnt, t, words, store);

    if(((n>>(idx)) & 1) == 0){
        maxMatch(i, t[cur].child[1], cnt+1, t, words, store);
    }
}

void finalMatch(int i, int cur, int cnt,
    vector<bit_trie>& t,
    vector<pair<int, string>>& words,
    vector<tuple<int, string, string, string, string>>& words_quads,
    vector<tuple<string,string,string,string, string>>& store)
{
    if(t[cur].sz == 0) return;
    
    // cout << (char)(t[cur].c + 'a') << " " << t[cur].has << ": " << t[cur].sz << endl;

    int n = words[i].fi;
    
    if(cnt > 25) return;

    int idx = t[cur].c;
    
    if(idx == -1){
        int hs;
        string b, c, d, e;
        tie(hs, b, c, d, e) = words_quads[t[cur].strs[0]];
        if((n & hs) == 0 and __builtin_popcount((n | hs)) == 25){
            seen[n+hs] = 1;
            store.push_back({words[i].se , b,c,d,e});
        }
        return;
    }

    finalMatch(i, t[cur].child[0], cnt, t, words, words_quads, store);

    if(((n>>(idx)) & 1) == 0){
        finalMatch(i, t[cur].child[1], cnt+1, t, words, words_quads, store);
    }
}

int main(){ 
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    vector<string> words = open_file("words_alpha", 5);

    sort(words.begin(), words.end());

    vector<pair<int, string>> words_clean;

    for(int i = 0; i < words.size(); i++){
        int hs = getWordHash(words[i]);
        if(seen[hs]) continue;
        if(__builtin_popcount(hs) < 5) continue;
        seen[hs] = 1;
        words_clean.push_back({hs, words[i]});
    }

    for(int i = 0; i < words_clean.size(); i++){
        int hs = words_clean[i].fi;
        insert(hs, i, sl);
    }

    for(int i = 0; i < words_clean.size(); i++){
        maxMatch(i, 0, 5, sl, words_clean, ps);
    }

    vector<pair<int, string>> words_pair;

    memset(seen, 0, sizeof(seen));
    for(auto[a,b] : ps){
        string c = a + b;
        int hs = getWordHash(c);
        if(seen[hs]) continue;
        seen[hs] = 1;
        insert(hs, words_pair.size(), db);
        words_pair.push_back({hs, a + " " + b});
        // cout << words_pair.back().se << " " << words_pair.back().fi << "\n";
    }

    for(int i = 0; i < words_pair.size(); i++){
        maxMatch(i, 0, 10, db, words_pair, quads);
    }

    vector<tuple<int, string, string, string, string>> words_quads;
    memset(seen, 0, sizeof(seen));
    for(auto[a_,b_] : quads){
        string a, b, c, d;
        for(int i = 0; i < 5; i++) a += a_[i];
        for(int i = 6; i < 11; i++) b += a_[i];
        for(int i = 0; i < 5; i++) c += b_[i];
        for(int i = 6; i < 11; i++) d += b_[i];

        string w = a + b + c + d;
        int hs = getWordHash(w);
        if(seen[hs]) continue;
        seen[hs] = 1;
        insert(hs, words_quads.size(), qd);
        words_quads.push_back({hs, a, b, c, d});
    }

    memset(seen, 0, sizeof(seen));
    for(int i = 0; i < words_clean.size(); i++){
        finalMatch(i, 0, 5, qd, words_clean, words_quads, quints);
    }

    set<set<string>> ans;
    for(auto [a,b,c,d,e] : quints){
        set<string> st;
        st.insert(a);
        st.insert(b);
        st.insert(c);
        st.insert(d);
        st.insert(e);
        ans.insert(st);
    }

    for(auto s : ans){
        for(auto str : s) cout << str << " ";
        cout << endl;
    }

    cout << words_clean.size() << endl;
    cout << sl.size() << endl;
    cout << ps.size() << endl;
    cout << words_pair.size() << endl;
    cout << db.size() << endl;
    cout << quads.size() << endl;
    cout << words_quads.size() << endl;

    return 0;
}