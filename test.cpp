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

    list<Stitch> min_perm = search_min_permutation(stitches);
    assert(min_perm.size()==stitches.size());

    evaluate(min_perm);
}

int main(){
    test_extract_stitches();
    cout << "all tests succeeded." << endl;
    return 0;
}
