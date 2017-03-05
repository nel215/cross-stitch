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

        NNGraph nngraph = this->create_nngraph_with_same_color(p, 'a');
        assert(nngraph.size() == 9);
        assert(nngraph[0].size() == 1);
        assert(nngraph[4].size() == 2);
    }
    void test_extract_next_states(){
        vector<string> p;
        p.push_back("aa");
        p.push_back("..");
        this->set_pattern(p);

        NNGraph g(4);
        g[0].push_back(1);
        g[1].push_back(2);
        State s1(0);
        State s2(1, 1, &s1);
        vector<State*> next_states = this->extract_next_states(&s2, g);
        assert(next_states.size() == 1);
        assert(next_states[0]->pos == 2);
        assert(next_states[0]->score == 3);
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
