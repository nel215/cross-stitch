#include "CrossStitch.hpp"
#include <cassert>

void test_extract_stitches(){
    vector<string> p;
    p.push_back("a");

    vector<Stitch> stitches = extract_stitches(p, 'a');
    assert(stitches.size() == 2);
}

int main(){
    test_extract_stitches();
    cout << "all tests succeeded." << endl;
    return 0;
}
