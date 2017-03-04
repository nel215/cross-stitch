#include "CrossStitch.hpp"
#include <cassert>

void test_extract_cells_with_same_color(){
    vector<string> p;
    p.push_back(".a");
    p.push_back("..");

    vector<P> cells = extract_cells_with_same_color(p, 'a');
    assert(cells.size() == 1);
    assert(cells[0].y == 0);
    assert(cells[0].x == 1);
}

void test_extract_stitches(){
    vector<string> p;
    p.push_back("a");

    vector<Stitch> stitches = extract_stitches(p, 'a');
    assert(stitches.size() == 2);
}

int main(){
    test_extract_cells_with_same_color();
    test_extract_stitches();
    cout << "all tests succeeded." << endl;
    return 0;
}
