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


int xor128() {
  static int x = 123456789;
  static int y = 362436069;
  static int z = 521288629;
  static int w = 88675123;

  int t = x ^ (x << 11);
  x = y; y = z; z = w;
  return w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
}

typedef vector<vector<int> > NNGraph;

class State{
    void init(int pos, int score, int hash, State *prev){
        this->pos = pos;
        this->score = score;
        this->hash = hash;
        this->prev = prev;
    }
public:
    int pos;
    int score;
    State *prev;
    int hash;
    State(int pos){
        this->init(pos, 0, 0, NULL);
    }
    State(int pos, int score, int hash, State *prev){
        this->init(pos, score, hash, prev);
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

int sq_dist(int ay, int ax, int by, int bx){
    int dy = ay - by;
    int dx = ax - bx;
    return dy*dy + dx*dx;
}

vector<int> create_hash_table(int s){
    vector<int> res(s*s);
    REP(i, res.size())res[i] = xor128();
    return res;
}

class CrossStitch {
protected:
    vector<string> pattern;
    vector<char> alphabet;
    vector<int> hash_table;
    int pattern_size;
    void set_pattern(vector<string> pattern){
        this->pattern = pattern;
        this->pattern_size = pattern.size();
        this->alphabet = vector<char>();
        int ps = this->pattern_size;
        REP(y, ps)REP(x, ps){
            if(pattern[y][x]=='.')continue;
            alphabet.push_back(pattern[y][x]);
        }
        sort(ALL(alphabet));
        alphabet.erase(unique(ALL(alphabet)), alphabet.end());
        hash_table = create_hash_table(ps);
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
    vector<int> extract_cells_with_same_color(char c){
        int ps = this->pattern_size;
        vector<int> cells;

        REP(y, ps)REP(x, ps){
            if(this->pattern[y][x]!=c)continue;
            cells.push_back(y*ps+x);
        }

        return cells;
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
    vector<State*> extract_next_states(State *current_state, const NNGraph &g, const vector<int> &cells){
        const State *cs = current_state;
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
            int score = cs->score + this->sq_dist_of_pos(current_state->pos, to);
            int hash = cs->hash ^ hash_table[to];
            State *next_state = new State(to, score, hash, current_state);
            next_states.push_back(next_state);
        }
        if(next_states.size()>0)return next_states;

        REP(i, cells.size()){
            int to = cells[i];
            if(used[to])continue;
            int score = current_state->score + this->sq_dist_of_pos(current_state->pos, to);
            int hash = cs->hash ^ hash_table[to];
            State *next_state = new State(to, score, hash, current_state);
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
    vector<int> search_min_path(vector<State*> initial_states, double limit, const NNGraph &g, const vector<int> &cells){
        int node_size = 0;
        REP(i, g.size())if(g[i].size()>0)node_size++;

        vector<StateQueue> beam(node_size, StateQueue());
        int init_idx = 0;
        while(get_time() < limit){
            if(init_idx < (int)initial_states.size()){
                beam[0].push(initial_states[init_idx++]);
            }
            int b=0;
            for(;;){
                if(b==node_size-1)break;
                if(beam[b].empty())break;
                State *s = beam[b].top();
                beam[b].pop();
                //cerr << "b: " << b << ", ";
                //s->debug();
                vector<State*> next_states = this->extract_next_states(s, g, cells);
                if(next_states.size()==0)continue;
                REP(j, next_states.size()){
                    //next_states[j]->debug();
                    beam[b+1].push(next_states[j]);
                }
                b++;
            }
        }
        //cerr << node_size << beam[node_size-1].size() << endl;

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
        double start_time = get_time();
        this->set_pattern(pattern);
        vector<string> ret;
        int ps = this->pattern_size;
        // for each color, for each cell (r, c) do two stitches (r+1, c)-(r, c+1)-(r+1, c+1)-(r, c)
        double each_time = 8.0 / alphabet.size();
        INFO(<< "S=" << this->pattern_size);
        INFO(<< "C=" << alphabet.size());
        REP(i, alphabet.size()){
            char c = alphabet[i];
            DEBUG(<< "start search: " << c << endl);
            NNGraph g = create_nngraph_with_same_color(c);
            vector<int> cells = extract_cells_with_same_color(c);
            vector<State*> initial_states = create_initial_states(g);
            double limit = start_time + each_time*(i+1);
            vector<int> min_path = search_min_path(initial_states, limit, g, cells);
            ret.push_back(string(1, c));
            REP(j, min_path.size()){
                int p = min_path[j];
                int y = p / ps;
                int x = p % ps;
                int last = (int)ret.size()-1;
                if(ret[last]!=to_string(y+1, x)){
                    ret.push_back(to_string(y+1, x));
                    ret.push_back(to_string(y, x+1));
                    ret.push_back(to_string(y+1, x+1));
                    ret.push_back(to_string(y, x));
                }else{
                    ret.push_back(to_string(y+1, x+1));
                    ret.push_back(to_string(y, x));
                    ret.push_back(to_string(y+1, x));
                    ret.push_back(to_string(y, x+1));
                }
            }
        }
        return ret;
    }
};

