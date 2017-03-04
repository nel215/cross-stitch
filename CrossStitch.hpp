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

struct Stitch{
    int from, to;
    Stitch(int f, int t){
        from = f;
        to = t;
    }
};

vector<Stitch> extract_stitches(const vector<string> &pattern, char c){
    int T = pattern.size() + 1;
    vector<Stitch> stitches;

    REP(y, pattern.size())REP(x, pattern.size()){
        if(pattern[y][x]!=c)continue;
        stitches.push_back(Stitch((y+0)*T + (x+0), (y+1)*T + (x+1)));
        stitches.push_back(Stitch((y+0)*T + (x+1), (y+1)*T + (x+0)));
    }

    return stitches;
}

class CrossStitch {
public:
    vector<string> embroider(vector<string> pattern) {
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

