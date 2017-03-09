#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <queue>
#include <stack>
#include <deque>
#include <complex>
#include <algorithm>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <ctime>
#include <sys/time.h>

#define REP(i,x) for(int i=0 ; i<(int)(x) ; i++)
#define ALL(x) (x).begin(),(x).end()
#define LL long long

#define INFO(x) cerr << "INFO: " x << endl
#define DEBUG(x) cerr << "DEBUG: " x << endl

using namespace std;

double get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec*1e-6;
}

string to_string(int r, int c){
    ostringstream oss;
    oss << r << " " << c;
    return oss.str();
}

struct P{
    int y, x;
    void init(int y, int x){
        this->y = y;
        this->x = x;
    }
    P(){
        init(0, 0);
    }
    P(int y, int x){
        init(y, x);
    }
    string repr(){
        ostringstream oss;
        oss << y << " " << x;
        return oss.str();
    }
};

struct Stitch{
    P from, to;
    void init(P f, P t){
        this->from = f;
        this->to = t;
    }
    Stitch(){
        init(P(), P());
    }
    Stitch(P f, P t){
        init(f, t);
    }
    P* get_from(const int &rev){
        return rev ? &to : &from;
    }
    P* get_to(const int &rev){
        return rev ? &from : &to;
    }
};

struct Flip{
    int idx, rev;
    void init(int i, int r){
        this->idx = i;
        this->rev = r;
    }
    Flip(int i, int r){
        init(i, r);
    }
    Flip(){
        init(0, 0);
    }
};

vector<char> create_alphabet(const vector<string> &pattern){
    vector<char> res;
    int ps = pattern.size();
    REP(y, ps)REP(x, ps){
        if(pattern[y][x]=='.')continue;
        res.push_back(pattern[y][x]);
    }
    sort(ALL(res));
    res.erase(unique(ALL(res)), res.end());
    return res;
}

vector<Stitch> extract_stitches(const vector<string> &pattern, const char c){
    int ps = pattern.size();
    vector<Stitch> res;
    REP(y, ps)REP(x, ps){
        if(pattern[y][x]!=c)continue;
        res.push_back(Stitch(P(y, x), P(y+1, x+1)));
        res.push_back(Stitch(P(y+1, x), P(y, x+1)));
    }
    return res;
}

int sq_dist(int ay, int ax, int by, int bx){
    int dy = ay - by;
    int dx = ax - bx;
    return dy*dy + dx*dx;
}

int sq_dist(const P &a, const P &b){
    return sq_dist(a.y, a.x, b.y, b.x);
}

vector<Flip> search_min_permutation(vector<Stitch> &stitches){
    list<Flip> res; // idx, rev
    res.push_back(Flip(0, 0));
    for(int i=1; i<(int)stitches.size(); i++){
        int best_score = 1<<30;
        list<Flip>::iterator best_it = res.begin();
        int best_rev = 0;

        Stitch &t = stitches[i];
        list<Flip>::iterator prev = res.end();
        for(list<Flip>::iterator it=res.begin(); it != res.end(); it++){
            int ci = it->idx;
            int cr = it->rev;
            if(prev==res.end()){
                // target to first
                Stitch &a = stitches[ci];
                REP(r, 2){
                    P *mid_to = t.get_to(r);
                    P *to = a.get_from(cr);
                    int d = sq_dist(*mid_to, *to);
                    if(d==0)continue;
                    if(best_score > d){
                        best_score = d;
                        best_it = it;
                        best_rev = r;
                    }
                }
            }else{
                // prev to target to current
                int pi = prev->idx;
                int pr = prev->rev;
                Stitch &a = stitches[ci];
                Stitch &b = stitches[pi];
                REP(r, 2){
                    P *from = b.get_to(pr);
                    P *mid_from = t.get_from(r);
                    P *mid_to = t.get_to(r);
                    P *to = a.get_from(cr);
                    int from_d = sq_dist(*from, *mid_from);
                    int to_d = sq_dist(*mid_to, *to);
                    if(from_d == 0 || to_d == 0)continue;
                    int d = from_d + to_d - sq_dist(*from, *to);
                    if(best_score > d){
                        best_score = d;
                        best_it = it;
                        best_rev = r;
                    }
                }
            }
            prev = it;
        }
        // last to target
        int pi = prev->idx;
        int pr = prev->rev;
        Stitch &b = stitches[pi];
        REP(r, 2){
            P *from = b.get_to(pr);
            P *mid_from = t.get_from(r);
            int d = sq_dist(*from, *mid_from);
            if(d == 0)continue;
            if(best_score > d){
                best_score = d;
                best_it = res.end();
                best_rev = r;
            }
        }
        res.insert(best_it, Flip(i, best_rev));
    }

    return vector<Flip>(ALL(res));
}

int evaluate(vector<Stitch> &stitches, const vector<Flip> &min_perm){
    int res = 0;
    P *prev = NULL;
    REP(i, min_perm.size()){
        Stitch &s = stitches[min_perm[i].idx];
        int rev = min_perm[i].rev;
        P *f = s.get_from(rev);
        P *t = s.get_to(rev);
        if(prev!=NULL){
            res += sq_dist(*prev, *f);
        }
        prev = t;
    }
    return res;
}

class CrossStitch {
public:
    vector<string> embroider(vector<string> pattern) {
        double start_time = get_time();
        vector<string> ret;

        vector<char> alphabet = create_alphabet(pattern);
        double each_time = 9.0 / alphabet.size();
        INFO(<< "S=" << pattern.size());
        INFO(<< "C=" << alphabet.size());
        REP(i, alphabet.size()){
            char c = alphabet[i];
            ret.push_back(string(1, c));
            //DEBUG(<< "start search: " << c << endl);
            vector<Stitch> stitches = extract_stitches(pattern, c);

            // search
            int best_score = 1<<30;
            vector<Flip> best_min_perm;
            vector<Stitch> best_stitches;
            double limit = start_time + each_time*(i+1);
            do{
                random_shuffle(ALL(stitches));
                vector<Flip> min_perm = search_min_permutation(stitches);
                int evaluated = evaluate(stitches, min_perm);
                if(evaluated < best_score){
                    best_score = evaluated;
                    best_min_perm = min_perm;
                    best_stitches = stitches;
                }
            }while(get_time() < limit);
            //DEBUG(<< "best: " << best_score);

            REP(j, best_min_perm.size()){
                const Stitch &s = best_stitches[best_min_perm[j].idx];
                int rev = best_min_perm[j].rev;
                const P &f = rev ? s.to : s.from;
                const P &t = rev ? s.from : s.to;
                ret.push_back(to_string(f.y, f.x));
                ret.push_back(to_string(t.y, t.x));
            }
        }
        return ret;
    }
};
