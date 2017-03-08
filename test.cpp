#include "CrossStitch.hpp"
#include <cassert>


void test_extract_stitches(){
    vector<string> p;
    p.push_back(".a.");
    p.push_back("a..");
    p.push_back("a..");

    vector<Stitch> stitches = extract_stitches(p, 'a');
    assert(stitches.size()==6);

    vector<pair<int, int> > min_perm = search_min_permutation(stitches);
    assert(min_perm.size()==stitches.size());
}

int main(){
    test_extract_stitches();
    cout << "all tests succeeded." << endl;
    return 0;
}
