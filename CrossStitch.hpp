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

unsigned int xor128() {
  static unsigned int x = 123456789;
  static unsigned int y = 362436069;
  static unsigned int z = 521288629;
  static unsigned int w = 88675123;

  unsigned int t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

double uniform(){
    const unsigned int UINT_MAX = (unsigned int)(-1);
    return 1.0 * xor128() / UINT_MAX;
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

        REP(r, 2){
            const P *mid_to = stitches[i].get_to(r);
            const P *mid_from = stitches[i].get_from(r);
            list<Flip>::iterator prev = res.end();
            for(list<Flip>::iterator it=res.begin();; it++){
                P *from = NULL;
                P *to = NULL;
                int dist = 0;
                bool valid = true;
                if(prev!=res.end()){
                    // prev to target
                    int pi = prev->idx;
                    int pr = prev->rev;
                    from = stitches[pi].get_to(pr);
                    int from_d = sq_dist(*from, *mid_from);
                    valid &= from_d > 0;
                    dist += from_d;
                }
                if(it!=res.end()){
                    // target to current
                    int ci = it->idx;
                    int cr = it->rev;
                    to = stitches[ci].get_from(cr);
                    int to_d = sq_dist(*mid_to, *to);
                    valid &= to_d > 0;
                    dist += to_d;
                }
                if(from != NULL && to != NULL){
                    dist -= sq_dist(*from, *to);
                }
                if(valid && best_score > dist){
                    best_score = dist;
                    best_it = it;
                    best_rev = r;
                }
                if(it==res.end())break;
                prev = it;
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

int get_dist(int c, vector<Stitch> &stitches, vector<Flip> &perm){
    int n = perm.size();
    int d = 0;
    Stitch &cs = stitches[perm[c].idx];
    if(c-1>=0){
        Stitch &ps = stitches[perm[c-1].idx];
        P *f = ps.get_to(perm[c-1].rev);
        P *t = cs.get_from(perm[c].rev);
        d += sq_dist(*f, *t);
    }
    if(c+1<n){
        Stitch &ns = stitches[perm[c+1].idx];
        P *f = cs.get_to(perm[c].rev);
        P *t = ns.get_from(perm[c+1].rev);
        d += sq_dist(*f, *t);
    }
    return d;
}

bool valid_perm(int c, vector<Stitch> &stitches, vector<Flip> &perm){
    int n = perm.size();
    Stitch &cs = stitches[perm[c].idx];
    if(c-1>=0){
        Stitch &ps = stitches[perm[c-1].idx];
        P *f = ps.get_to(perm[c-1].rev);
        P *t = cs.get_from(perm[c].rev);
        if(sq_dist(*f, *t)==0)return false;
    }
    if(c+1<n){
        Stitch &ns = stitches[perm[c+1].idx];
        P *f = cs.get_to(perm[c].rev);
        P *t = ns.get_from(perm[c+1].rev);
        if(sq_dist(*f, *t)==0)return false;
    }
    return true;
}

void optimize_permutation(vector<Stitch> &stitches, vector<Flip> &perm){
    int n = perm.size();
    vector<pair<int, int> > cand(perm.size());
    REP(i, n){
        int d = get_dist(i, stitches, perm);
        cand[i] = make_pair(-d, i);
    }
    sort(ALL(cand));
    REP(i, n){
        for(int j=i+1; j<n; j++){
            pair<int, int> ci = cand[i];
            pair<int, int> cj = cand[j];
            Flip f1 = perm[ci.second];
            Flip f2 = perm[cj.second];
            int cdist = get_dist(ci.second, stitches, perm) + get_dist(cj.second, stitches, perm);
            int best = 1<<30;
            pair<int, int> best_rev;
            swap(perm[ci.second].idx, perm[cj.second].idx);
            REP(ir, 2)REP(jr, 2){
                perm[ci.second].rev = ir;
                perm[cj.second].rev = jr;
                if(!valid_perm(ci.second, stitches, perm))continue;
                if(!valid_perm(cj.second, stitches, perm))continue;
                int d = get_dist(ci.second, stitches, perm) + get_dist(cj.second, stitches, perm);
                if(d < best){
                    best = d;
                    best_rev = make_pair(ir, jr);
                }
            }
            if(best<cdist){
                perm[ci.second].rev = best_rev.first;
                perm[cj.second].rev = best_rev.second;
                return;
            }else{
                perm[ci.second] = f1;
                perm[cj.second] = f2;
            }
        }
    }
}

vector<Flip> search_by_stitch_swap(vector<Stitch> &stitches, double limit){
    vector<Flip> best_min_perm = search_min_permutation(stitches);
    int best_score = evaluate(stitches, best_min_perm);
    for(;;){
        double now = get_time();
        if(now > limit)break;
        double T = (limit - now) * best_score * 0.01;
        int a = xor128()%stitches.size();
        int b = xor128()%stitches.size();
        if(a==b)continue;
        swap(stitches[a], stitches[b]);
        vector<Flip> min_perm = search_min_permutation(stitches);
        int score = evaluate(stitches, min_perm);
        double p = min(1.0, exp((best_score-score)/T));
        if(p > uniform()){
            //if(score>best_score)DEBUG(<< "p: " << p << ",q:" << q);
            best_score = score;
            best_min_perm = min_perm;
        }else{
            swap(stitches[a], stitches[b]);
        }
    }
    //DEBUG(<< "best: " << best_score);
    return best_min_perm;
}

vector<Flip> search_by_random_shuffle(vector<Stitch> &stitches, double limit){
    int best_score = 1<<30;
    vector<Flip> best_min_perm = search_min_permutation(stitches);
    vector<Stitch> best_stitches = stitches;

    while(get_time() < limit){
        random_shuffle(ALL(stitches));
        vector<Flip> min_perm = search_min_permutation(stitches);
        int score = evaluate(stitches, min_perm);
        if(score < best_score){
            best_score = score;
            best_min_perm = min_perm;
            best_stitches = stitches;
        }
    }
    stitches = best_stitches;
    return best_min_perm;
}

class CrossStitch {
public:
    vector<string> embroider(vector<string> pattern) {
        double start_time = get_time();
        vector<string> ret;

        vector<char> alphabet = create_alphabet(pattern);
        double each_time = 9.0 / alphabet.size();
        int S = pattern.size();
        INFO(<< "S=" << S);
        INFO(<< "C=" << alphabet.size());
        REP(i, alphabet.size()){
            char c = alphabet[i];
            ret.push_back(string(1, c));
            //DEBUG(<< "start search: " << c);
            vector<Stitch> stitches = extract_stitches(pattern, c);

            // search
            double limit = start_time + each_time*(i+1);
            vector<Flip> best_min_perm;
            if(S < 70){
                best_min_perm = search_by_stitch_swap(stitches, limit);
            }else{
                best_min_perm = search_by_random_shuffle(stitches, limit);
            }

            //while(get_time() < limit){
            //    optimize_permutation(best_stitches, best_min_perm);
            //    int score = evaluate(best_stitches, best_min_perm);
            //    if(score==best_score)break;
            //    best_score = score;
            //}

            REP(j, best_min_perm.size()){
                const Stitch &s = stitches[best_min_perm[j].idx];
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
