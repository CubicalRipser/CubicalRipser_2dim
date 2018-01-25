//ComputePairs.h

#include <unordered_map>

template <class Key, class T> class hash_map : public std::unordered_map<Key, T> {};


class ComputePairs
{

public:
	DenseCubicalGrids* dcg;
	ColumnsToReduce* ctr;
	hash_map<int, int> pivot_column_index;
	int ax, ay;
	int dim;
	const int mode=1;
	vector<WritePairs> *wp;

	ComputePairs(DenseCubicalGrids* _dcg, ColumnsToReduce* _ctr, vector<WritePairs> &_wp);
	void compute_pairs_main();
	void outputPP(int _dim, double _birth, double _death);
	BirthdayIndex pop_pivot(priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>& column);
	BirthdayIndex get_pivot(priority_queue<BirthdayIndex, vector<BirthdayIndex>, BirthdayIndexComparator>& column);
	void assemble_columns_to_reduce();
};
