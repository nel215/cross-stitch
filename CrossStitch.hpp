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

const int INF = 1<<28;

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

class BIT{
    vector<int> data;
public:
    int size;
    BIT(int size){
        this->size = size;
        this->data.assign(size+1, 0);
    }
    void add(int i, int v){
        while(i<=this->size){
            this->data[i] += v;
            i += (i&-i);
        }
    }
    int sum(int i) const{
        int res = 0;
        while(i>0){
            res += data[i];
            i -= (i&-i);
        }
        return res;
    }
};

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
    int rev, color, hub;
    void init(P f, P t, int c, int h, int r){
        this->from = f;
        this->to = t;
        this->rev = r;
        this->color = c;
        this->hub = h;
    }
    Stitch(){
        init(P(), P(), -1, 0, 0);
    }
    Stitch(P f, P t, int c, int h){
        init(f, t, c, h, 0);
    }
    P* get_from(){
        return rev ? &to : &from;
    }
    P* get_to(){
        return rev ? &from : &to;
    }
};

inline int sq_dist(int ay, int ax, int by, int bx){
    int dy = ay - by;
    int dx = ax - bx;
    return dy*dy + dx*dx;
}

int sq_dist(const P &a, const P &b){
    return sq_dist(a.y, a.x, b.y, b.x);
}

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

    // draw same region with same color
    vector<int> color(ps*ps, -1);
    int color_count = 0;
    REP(y, ps)REP(x, ps){
        if(pattern[y][x]!=c)continue;
        if(color[y*ps+x]!=-1)continue;
        stack<int> st;
        st.push(y*ps + x);
        while(!st.empty()){
            int cp = st.top();st.pop();
            color[cp] = color_count;
            int cy = cp / ps;
            int cx = cp % ps;
            const int DY[4] = {-1, 0, 1, 0};
            const int DX[4] = {0, -1, 0, 1};
            REP(i, 4){
                int ny = cy + DY[i];
                int nx = cx + DX[i];
                if(ny<0 || nx<0 || ny>=ps || nx>=ps)continue;
                if(pattern[ny][nx]!=c)continue;

                int np = ny*ps + nx;
                if(color[np]!=-1)continue;
                st.push(np);
            }
        }
        color_count++;
    }

    vector<vector<P> > color_cells(color_count, vector<P>());
    REP(y, ps)REP(x, ps){
        int p = y*ps+x;
        if(color[p]==-1)continue;
        color_cells[color[p]].push_back(P(y, x));
    }

    vector<int> is_hub(ps*ps, 0);
    REP(c1, color_count){
        for(int c2=c1+1; c2<color_count; c2++){
            int best = INF;
            P a, b;
            REP(i, color_cells[c1].size()){
                REP(j, color_cells[c2].size()){
                    int d = sq_dist(color_cells[c1][i], color_cells[c2][j]);
                    if(d<best){
                        best = d;
                        a = color_cells[c1][i];
                        b = color_cells[c2][j];
                    }
                }
            }
            //cerr << c1 << " to " << c2 << " best " << best << endl;
            is_hub[a.y*ps+a.x] = 1;
            is_hub[b.y*ps+b.x] = 1;
        }
    }


    // extract
    vector<Stitch> res;
    REP(y, ps)REP(x, ps){
        if(pattern[y][x]!=c)continue;
        res.push_back(Stitch(P(y, x), P(y+1, x+1), color[y*ps+x], is_hub[y*ps+x]));
        res.push_back(Stitch(P(y+1, x), P(y, x+1), color[y*ps+x], is_hub[y*ps+x]));
    }
    return res;
}

list<Stitch> search_min_permutation(const vector<Stitch> &stitches){
    int n = stitches.size();
    list<Stitch> res;
    vector<int> used(n, 0);
    REP(hub, 2){
        for(int i=0; i<(int)stitches.size(); i++){
            if(stitches[i].hub==hub)continue;
            if(used[i])continue;
            used[i] = 1;
            if(res.size()==0){
                res.push_back(stitches[i]);
                continue;
            }

            int best_score = 1<<30;
            list<Stitch>::iterator best_it = res.begin();
            int best_rev = 0;

            Stitch s = stitches[i];
            REP(r, 2){
                s.rev = r;
                const P *mid_to = s.get_to();
                const P *mid_from = s.get_from();
                list<Stitch>::iterator prev = res.end();
                for(list<Stitch>::iterator it=res.begin();; it++){
                    P *from = NULL;
                    P *to = NULL;
                    int dist = 0;
                    bool valid = true;
                    if(prev!=res.end()){
                        // prev to target
                        from = prev->get_to();
                        int from_d = sq_dist(*from, *mid_from);
                        valid &= from_d > 0;
                        dist += from_d;
                    }
                    if(it!=res.end()){
                        // target to current
                        to = it->get_from();
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
            s.rev = best_rev;
            res.insert(best_it, s);
        }
    }

    return res;
}

int evaluate(list<Stitch> &stitches){
    int res = 0;
    P *prev = NULL;
    for(list<Stitch>::iterator it = stitches.begin(); it != stitches.end(); it++){
        P *f = it->get_from();
        if(prev!=NULL){
            res += sq_dist(*prev, *f);
        }
        prev = it->get_to();
    }
    return res;
}

vector<Stitch> search_by_stitch_swap(vector<Stitch> stitches, double limit){
    list<Stitch> best_min_perm = search_min_permutation(stitches);
    int best_score = evaluate(best_min_perm);
    int n = stitches.size();
    for(;;){
        double now = get_time();
        if(now > limit)break;
        double T = (limit - now) * best_score * 0.01;
        int a = xor128()%n;
        int b = xor128()%n;
        if(a==b)continue;
        swap(stitches[a], stitches[b]);
        list<Stitch> min_perm = search_min_permutation(stitches);
        int score = evaluate(min_perm);
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
    return vector<Stitch>(ALL(best_min_perm));
}

vector<Stitch> search_by_random_shuffle(vector<Stitch> stitches, double limit){
    int best_score = 1<<30;
    list<Stitch> best_min_perm = search_min_permutation(stitches);

    while(get_time() < limit){
        random_shuffle(ALL(stitches));
        list<Stitch> min_perm = search_min_permutation(stitches);
        int score = evaluate(min_perm);
        if(score < best_score){
            best_score = score;
            best_min_perm = min_perm;
        }
    }
    return vector<Stitch>(ALL(best_min_perm));
}

int get_dist(int idx, vector<Stitch> &stitches){
    const int n = stitches.size();
    const int prev = idx-1;
    const int next = idx+1;
    int res = 0;
    if(prev>=0){
        int d = sq_dist(*stitches[prev].get_to(), *stitches[idx].get_from());
        if(d==0)return INF;
        res += d;
    }
    if(next<n){
        int d = sq_dist(*stitches[idx].get_to(), *stitches[next].get_from());
        if(d==0)return INF;
        res += d;
    }
    return res;
}

int upper_bound(double v, const BIT &bit){
    int lb=0, ub=bit.size;
    while(ub-lb>1){
        int mid = (lb+ub)>>1;
        if(bit.sum(mid+1)<v)lb = mid;
        else ub = mid;
    }
    return lb;
}

void optimize(vector<Stitch> &stitches, const double limit){
    const int n = stitches.size();
    BIT bit(n);
    REP(i, n)bit.add(i+1, get_dist(i, stitches));
    while(get_time() < limit){
        REP(_, 100){
            int sum = bit.sum(n);

            int a = upper_bound(uniform()*sum, bit);
            int b = upper_bound(uniform()*sum, bit);
            while(a==b)b = upper_bound(uniform()*sum, bit);
            //if(a<0 || a>=n)cerr << "error" << endl;
            //if(b<0 || b>=n)cerr << "error" << endl;
            int now_a = get_dist(a, stitches);
            int now_b = get_dist(b, stitches);
            //cerr << now_a << "," << now_b << endl;
            int now = now_a + now_b;
            Stitch as = stitches[a];
            Stitch bs = stitches[b];
            swap(stitches[a], stitches[b]);
            int best_a = 1<<27;
            int best_b = 1<<27;
            int best_ar = 0;
            int best_br = 0;
            REP(ar, 2)REP(br, 2){
                stitches[a].rev = ar;
                stitches[b].rev = br;
                int next_a = get_dist(a, stitches);
                int next_b = get_dist(b, stitches);
                if(best_a+best_b>next_a+next_b){
                    best_a = next_a;
                    best_b = next_b;
                    best_ar = ar;
                    best_br = br;
                }
            }
            if(best_a+best_b >= now){
                stitches[a] = as;
                stitches[b] = bs;
            }else{
                stitches[a].rev = best_ar;
                stitches[b].rev = best_br;
                bit.add(a+1, best_a - now_a);
                bit.add(b+1, best_b - now_b);
            }
        }
    }
}

class CrossStitch {
public:
    vector<string> embroider(vector<string> pattern) {
        double start_time = get_time();
        vector<string> ret;

        vector<char> alphabet = create_alphabet(pattern);
        double each_time = 9.5 / alphabet.size();
        int S = pattern.size();
        int C = alphabet.size();
        INFO(<< "S=" << S);
        INFO(<< "C=" << C);
        REP(i, C){
            char c = alphabet[i];
            //DEBUG(<< "start search: " << c);
            vector<Stitch> stitches = extract_stitches(pattern, c);
            ret.push_back(string(1, c));
            // search
            double limit = start_time + each_time*(i+0.8);
            vector<Stitch> best_min_perm;
            if(S < 70){
                best_min_perm = search_by_stitch_swap(stitches, limit);
            }else{
                best_min_perm = search_by_random_shuffle(stitches, limit);
            }

            limit = start_time + each_time*(i+1);
            int n = stitches.size();

            if(n>4)optimize(best_min_perm, limit);

            REP(j, best_min_perm.size()){
                Stitch &s = best_min_perm[j];
                P *f = s.get_from();
                P *t = s.get_to();
                ret.push_back(to_string(f->y, f->x));
                ret.push_back(to_string(t->y, t->x));
            }
        }
        return ret;
    }
};
