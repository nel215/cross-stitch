#include "CrossStitch.hpp"
#include <cassert>


void test_extract_stitches(){
    srand(215);
    vector<string> p(50, string(50, '.'));
    REP(i, 100){
        int y = rand()%50;
        int x = rand()%50;
        if(p[y][x]=='a')continue;
        p[y][x] = 'a';
    }


    vector<Stitch> stitches = extract_stitches(p, 'a');
    assert(stitches.size()==200);

    vector<Flip> min_perm = search_min_permutation(stitches);
    assert(min_perm.size()==stitches.size());

    int before = evaluate(stitches, min_perm);
    optimize_permutation(stitches, min_perm);
    int after = evaluate(stitches, min_perm);
    assert(before>after);
}

int main(){
    test_extract_stitches();
    cout << "all tests succeeded." << endl;
    return 0;
}
