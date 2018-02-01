//UnionFind.cpp

#include <iostream>
#include <algorithm>
#include <queue>
// #include <unordered_map>
// #include <string>
// #include <cstdint>

// #include "Coeff.h"
// #include "Vertices.h"
#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"
// #include "SimplexCoboundaryEnumerator.h"
#include "WritePairs.h"
#include "UnionFind.h"


using namespace std;

	// int max_of_index;
	// vector<int> parent;
	// vector<double> birthtime;
	// vector<double> time_max;
	// DenseCubicalGrids* dcg;
	
UnionFind::UnionFind(int moi, DenseCubicalGrids* _dcg) : parent(moi), birthtime(moi), time_max(moi) { // Thie "n" is the number of cubes.
	dcg = _dcg;
	max_of_index = moi;

	for(int i = 0; i < moi; ++i){
		parent[i] = i;
		birthtime[i] = dcg -> getBirthday(i, 0);
		time_max[i] = dcg -> getBirthday(i, 0);
	}
}

int UnionFind::find(int x){ // Thie "x" is Index.
	int y = x, z = parent[y];
	while (z != y) {
		y = z;
		z = parent[y];
	}
	y = parent[x];
	while (z != y) {
		parent[x] = z;
		x = y;
		y = parent[x];
	}
	return z;
}

void UnionFind::link(int x, int y){
	x = find(x);
	y = find(y);
	if (x == y) return;
	if (birthtime[x] > birthtime[y]){
		parent[x] = y; 
		birthtime[y] = min(birthtime[x], birthtime[y]);
		time_max[y] = max(time_max[x], time_max[y]);
	} else if(birthtime[x] < birthtime[y]) {
		parent[y] = x;
		birthtime[x] = min(birthtime[x], birthtime[y]);
		time_max[x] = max(time_max[x], time_max[y]);
	} else { //birthtime[x] == birthtime[y]
		parent[x] = y;
		time_max[y] = max(time_max[x], time_max[y]);
	}
}


// int n; // the number of cubes
// int ctr_moi;
// int ax, ay;
// DenseCubicalGrids* dcg;
// ColumnsToReduce* ctr;
// vector<Writepairs> *wp;
// double u, v;
// vector<int64_t> cubes_edges;
// vector<BirthdayIndex> dim1_simplex_list;

JointPairs::JointPairs(DenseCubicalGrids* _dcg, ColumnsToReduce* _ctr, vector<WritePairs> &_wp, const bool _print){
	dcg = _dcg;
	ax = dcg -> ax;
	ay = dcg -> ay;
	ctr = _ctr; // ctr is "dim0"simplex list.
	ctr_moi = ctr -> max_of_index;
	n = ctr -> columns_to_reduce.size();
	print = _print;

	wp = &_wp;

	for(int x = 1; x <= ax; ++x){
		for(int y = 1; y <= ay; ++y){
			for(int type = 0; type < 2; ++type){
				int index = x | (y << 11) | (type << 21);
				double birthday = dcg -> getBirthday(index, 1);
				if(birthday < dcg -> threshold){
					dim1_simplex_list.push_back(BirthdayIndex(birthday, index, 1));
				}
			}
		}
	}
	sort(dim1_simplex_list.begin(), dim1_simplex_list.end(), BirthdayIndexInverseComparator());
}

void JointPairs::joint_pairs_main(){
	//cubes_edges.reserve(2);
	UnionFind dset(ctr_moi, dcg);
	ctr -> columns_to_reduce.clear();
	ctr -> dim = 1;
	double min_birth = dcg -> threshold;

	if(print == true){
		cout << "persistence intervals in dim " << 0 << ":" << endl;
	}

	for(BirthdayIndex e : dim1_simplex_list){
		//cubes_edges.clear();

		int index = e.getIndex();
		int cx = index & 0x07ff;
		int cy = (index >> 11) & 0x03ff;
		int cm = (index >> 21) & 0xff;
		int ce0=0, ce1 =0;
		switch(cm){
		case 0:
			ce0 =  ((cy) << 11) | (cx);
			ce1 =  ((cy) << 11) | (cx + 1);
			break;
		default:
			ce0 =  ((cy) << 11) | (cx);
			ce1 =  ((cy + 1) << 11) | (cx);
			break;
		}
		u = dset.find(ce0);
		v = dset.find(ce1);
		if(min_birth >= min(dset.birthtime[u], dset.birthtime[v])){
			min_birth = min(dset.birthtime[u], dset.birthtime[v]);
		}

		if(u != v){
			double birth = max(dset.birthtime[u], dset.birthtime[v]);
			double death = max(dset.time_max[u], dset.time_max[v]);
			//cout << "birth : death = ( " << birth << " , " << death << " )" <<endl;
			if(birth == death){
				dset.link(u, v);
			} else {
				if(print == true){
					cout << "[" << birth << "," << death << ")" << endl;
				}
				
				wp->push_back(WritePairs(0, birth, death));
				dset.link(u, v);
			}
		} else { // If two values have same "parent", these are potential edges which make a 2-simplex.
			ctr->columns_to_reduce.push_back(e);
		}
	}

	if(print == true){
		cout << "[" << min_birth << ", )" << endl;
	}

	wp -> push_back(WritePairs(-1, min_birth, dcg -> threshold));
	sort(ctr -> columns_to_reduce.begin(), ctr -> columns_to_reduce.end(), BirthdayIndexComparator());
}
