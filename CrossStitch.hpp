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

using namespace std;

double get_time(){
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return tv.tv_sec + tv.tv_usec*1e-6;
}

// const
// cross type
const int LTRB = 0; // left top to right bottom
const int LBRT = 1; // left bottom to right top

string to_string(int r, int c){
    ostringstream oss;
    oss << r << " " << c;
    return oss.str();
}

typedef vector<vector<int> > NNGraph;

class State{
    void init(int pos, int score, State *prev){
        this->pos = pos;
        this->score = score;
        this->prev = prev;
    }
public:
    int pos;
    int score;
    State *prev;
    State(int pos){
        this->init(pos, 0, NULL);
    }
    State(int pos, int score, State *prev){
        this->init(pos, score, prev);
    }
    void debug(){
        cerr << "pos: " << pos << ", score: " << score << endl;
    }
};

class StateComparator{
public:
    bool operator()(const State *a, const State *b){
        return a->score > b->score;
    }
};

typedef priority_queue<State*, vector<State*>, StateComparator> StateQueue;

struct P{
    int y, x;
    P(int _y, int _x){
        y = _y;
        x = _x;
    }
    P(){
        P(0, 0);
    }
};

struct Stitch{
    P from, to;
    Stitch(P f, P t){
        from = f;
        to = t;
    }
};

vector<P> extract_cells_with_same_color(const vector<string> &pattern, char c){
    vector<P> cells;

    REP(y, pattern.size())REP(x, pattern.size()){
        if(pattern[y][x]!=c)continue;
        cells.push_back(P(y, x));
    }

    return cells;
}

vector<Stitch> convert_cells_to_stitches(const vector<P> &cells){
    vector<Stitch> stitches;

    REP(i, cells.size()){
        int y = cells[i].y;
        int x = cells[i].x;
        stitches.push_back(Stitch(P(y+0, x+0), P(y+1, x+1)));
        stitches.push_back(Stitch(P(y+0, x+1), P(y+1, x+0)));
    }

    return stitches;
}

int sq_dist(int ay, int ax, int by, int bx){
    int dy = ay - by;
    int dx = ax - bx;
    return dy*dy + dx*dx;
}

bool include(int ay, int ax, int by, int bx, int thres){
    int sd = sq_dist(ay, ax, by, bx);
    return sd <= thres*thres;
}

class CrossStitch {
protected:
    vector<string> pattern;
    int pattern_size;
    void set_pattern(vector<string> pattern){
        this->pattern = pattern;
        this->pattern_size = pattern.size();
    }
    bool is_valid_position(int y, int x){
        int ps = this->pattern_size;
        return y >= 0 && x >=0 && y < ps && x < ps;
    }
    int sq_dist_of_pos(int ap, int bp){
        int ps = this->pattern_size;
        int ay = ap / ps;
        int ax = ap % ps;
        int by = bp / ps;
        int bx = bp % ps;
        return sq_dist(ay, ax, by, bx);
    }
    NNGraph create_nngraph_with_same_color(char c){
        int ps = this->pattern_size;

        NNGraph nngraph(ps*ps, vector<int>());
        REP(y, ps)REP(x, ps){
            if(this->pattern[y][x]!=c)continue;
            int from = y*ps + x;
            vector<pair<int, int> > cand;
            REP(i, 3)REP(j, 3){
                int ty = y - 1 + i;
                int tx = x - 1 + j;
                if(ty==y && tx==x)continue;
                if(!is_valid_position(ty, tx))continue;
                if(pattern[ty][tx]!=c)continue;
                int sd = sq_dist(y, x, ty, tx);

                int to = ty*ps + tx;
                cand.push_back(make_pair(sd, to));
            }
            if(cand.size()<4){ // TODO: parameterize?
                cand.clear();
                REP(ty, ps)REP(tx, ps){
                    if(ty==y && tx==x)continue;
                    if(pattern[ty][tx]!=c)continue;
                    int sd = sq_dist(y, x, ty, tx);
                    int to = ty*ps + tx;
                    cand.push_back(make_pair(sd, to));
                }
            }
            sort(ALL(cand));
            REP(i, min(4, (int)cand.size())){
                int to = cand[i].second;
                nngraph[from].push_back(to);
                nngraph[to].push_back(from);
            }
        }

        // unique
        REP(i, nngraph.size()){
            sort(ALL(nngraph[i]));
            nngraph[i].erase(unique(ALL(nngraph[i])), nngraph[i].end());
        }
        return nngraph;
    }
    vector<State*> extract_next_states(State *current_state, const NNGraph &g){
        // TODO: hash
        int ps = this->pattern_size;
        vector<int> used(ps*ps);

        // recover
        State *ptr = current_state;
        while(ptr!=NULL){
            int pos = ptr->pos;
            used[pos] = 1;
            ptr = ptr->prev;
        }

        // search
        vector<State*> next_states;
        REP(i, g[current_state->pos].size()){
            int to = g[current_state->pos][i];
            if(used[to])continue;
            int score = current_state->score + this->sq_dist_of_pos(current_state->pos, to);
            State *next_state = new State(to, score, current_state);
            next_states.push_back(next_state);
        }

        return next_states;
    }
    vector<State*> create_initial_states(const NNGraph &g){
        vector<pair<int, int> > cand;
        REP(i, g.size()){
            if(g[i].size()==0)continue;
            cand.push_back(make_pair(g[i].size(), i));
        }
        sort(ALL(cand));
        vector<State*> res;
        REP(i, cand.size()){
            int p = cand[i].second;
            res.push_back(new State(p));
        }
        return res;
    }
    vector<int> search_min_path(vector<State*> initial_states, double limit, const NNGraph &g){
        int node_size = 0;
        REP(i, g.size())if(g[i].size()>0)node_size++;

        vector<StateQueue> beam(node_size, StateQueue());
        int init_idx = 0;
        while(get_time() < limit){
            if(init_idx < (int)initial_states.size()){
                beam[0].push(initial_states[init_idx++]);
            }
            REP(b, node_size-1){
                if(beam[b].empty())continue;
                State *s = beam[b].top();
                beam[b].pop();
                // cout << "b: " << b << ", ";
                // s->debug();
                vector<State*> next_states = this->extract_next_states(s, g);
                REP(j, next_states.size()){
                    beam[b+1].push(next_states[j]);
                }
            }
        }

        State *ptr = beam[node_size-1].top();
        vector<int> res;
        while(ptr!=NULL){
            res.push_back(ptr->pos);
            ptr = ptr->prev;
        }
        reverse(ALL(res));
        return res;
    }
public:

    vector<string> embroider(vector<string> pattern) {
        this->set_pattern(pattern);
        vector<string> ret;
        int S = pattern.size();
        // for each color, for each cell (r, c) do two stitches (r+1, c)-(r, c+1)-(r+1, c+1)-(r, c)
        for (char col = 'a'; col <= 'z'; ++col) {
            bool first = true;
            for (int r = 0; r < S; ++r)
            for (int c = 0; c < S; ++c)
                if (pattern[r][c] == col) {
                    if (first) {
                        ret.push_back(string(1, col));
                        first = false;
                    }
                    ret.push_back(to_string(r+1, c));
                    ret.push_back(to_string(r, c+1));
                    ret.push_back(to_string(r+1, c+1));
                    ret.push_back(to_string(r, c));
                }
        }
        return ret;
    }
};

