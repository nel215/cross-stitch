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
    vector<P> p;
    p.push_back(P(0, 0));

    vector<Stitch> stitches = convert_cells_to_stitches(p);
    assert(stitches.size() == 2);
}

class TestingCrossStitch : public CrossStitch {
public:
    void test_create_nngraph_with_same_color(){
        vector<string> p;
        p.push_back("a..");
        p.push_back(".a.");
        p.push_back("..a");
        this->set_pattern(p);

        vector<vector<P> > nngraph = this->create_nngraph_with_same_color(p, 'a');
        assert(nngraph.size() == 9);
        assert(nngraph[0].size() == 1);
        assert(nngraph[4].size() == 2);
    }
    void test_extract_next_states(){
        State s;
    }
};

int main(){
    test_extract_cells_with_same_color();
    test_extract_stitches();
    TestingCrossStitch tcs;
    tcs.test_create_nngraph_with_same_color();
    tcs.test_extract_next_states();
    cout << "all tests succeeded." << endl;
    return 0;
}
