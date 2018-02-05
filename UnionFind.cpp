/* UnionFind.cpp

Copyright 2017-2018 Takeki Sudo and Kazushi Ahara.

This file is part of CubicalRipser_2dim.

CubicalRipser: C++ system for computation of Cubical persistence pairs
Copyright 2017-2018 Takeki Sudo and Kazushi Ahara.
CubicalRipser is free software: you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the
Free Software Foundation, either version 3 of the License, or (at your option)
any later version.

CubicalRipser is deeply depending on 'Ripser', software for Vietoris-Rips 
persitence pairs by Ulrich Bauer, 2015-2016.  We appreciate Ulrich very much.
We rearrange his codes of Ripser and add some new ideas for optimization on it 
and modify it for calculation of a Cubical filtration.

This part of CubicalRiper is a calculator of cubical persistence pairs for 
2 dimensional pixel data. The input data format conforms to that of DIPHA.
 See more descriptions in README.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more details.
You should have received a copy of the GNU Lesser General Public License along
with this program.  If not, see <http://www.gnu.org/licenses/>.
*/


#include <iostream>
#include <algorithm>
#include <queue>

#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"
#include "WritePairs.h"
#include "UnionFind.h"

using namespace std;
	
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
	UnionFind dset(ctr_moi, dcg);
	ctr -> columns_to_reduce.clear();
	ctr -> dim = 1;
	double min_birth = dcg -> threshold;

	if(print == true){
		cout << "persistence intervals in dim " << 0 << ":" << endl;
	}

	for(BirthdayIndex e : dim1_simplex_list){

		int index = e.getIndex();
		int cx = index & 0x07ff;
		int cy = (index >> 11) & 0x03ff;
		int cm = (index >> 21) & 0xff;
		int ce0=0, ce1 =0;

		switch(cm){
		case 0:
			ce0 =  ((cy) << 11) | cx;
			ce1 =  ((cy) << 11) | (cx + 1);
			break;
		default:
			ce0 =  ((cy) << 11) | cx;
			ce1 =  ((cy + 1) << 11) | cx;
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
