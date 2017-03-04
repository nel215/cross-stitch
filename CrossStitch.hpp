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

const double TIME_LIMIT = 7.5;
double GetTime(){
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
    vector<vector<P> > create_nngraph_with_same_color(const vector<string> &pattern, char c){
        int thres = 2;
        int ps = this->pattern_size;

        vector<vector<P> > nngraph(ps*ps, vector<P>());
        REP(y, ps)REP(x, ps){
            if(pattern[y][x]!=c)continue;
            REP(i, 5)REP(j, 5){
                int ty = y - 2 + i;
                int tx = x - 2 + j;
                if(ty==y && tx==x)continue;
                if(!is_valid_position(ty, tx))continue;
                if(!include(y, x, ty, tx, thres))continue;
                if(pattern[ty][tx]!=c)continue;

                int from = y*ps + x;
                nngraph[from].push_back(P(ty, tx));
            }
        }
        return nngraph;
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

