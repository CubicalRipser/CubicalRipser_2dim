//UnionFind.h

#include <vector>


using namespace std;

class UnionFind{
public:
	int max_of_index;
	vector<int> parent;
	vector<double> birthtime;
	vector<double> time_max;
	DenseCubicalGrids* dcg;
	
	UnionFind(int moi, DenseCubicalGrids* _dcg);
	int find(int x);
	void link(int x, int y);
};

//JointPairs.h


class JointPairs{

	int n; // the number of cubes
	int ctr_moi;
	int ax, ay;
	DenseCubicalGrids* dcg;
	ColumnsToReduce* ctr;
	vector<WritePairs> *wp;
	bool print;
	double u, v;
	vector<int64_t> cubes_edges;
	vector<BirthdayIndex> dim1_simplex_list;

public:
	JointPairs(DenseCubicalGrids* _dcg, ColumnsToReduce* _ctr, vector<WritePairs> &_wp, const bool _print);
	void joint_pairs_main();
};
