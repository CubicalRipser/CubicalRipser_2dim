/*
CubicalRipser: C++ system for computation of Cubical persistence pairs
Copyright 2017-2018 Takeki Sudou and Kazushi Ahara.
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

//#define PRINT_PERSISTENCE_PAIRS
//#define TIME_MEASURING

//#define FILE_OUTPUT
#define CSV_OUTPUT

#define LINK_FIND
//#define COMPUTEPAIRS

#include <fstream>
#include <iostream>
#include <algorithm>
#include <queue>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>

// #include "Coeff.h"
// #include "Vertices.h"
#include "DenseCubicalGrids.h"
#include "BirthdayIndex.h"
#include "ColumnsToReduce.h"
#include "SimplexCoboundaryEnumerator.h"
#include "WritePairs.h"
#include "UnionFind.h"
#include "ComputePairs.h"

#ifdef TIME_MEASURING
#include <chrono> // time measuring
#endif

using namespace std;



int main(){
#ifdef TIME_MEASURING
	const auto startTime = chrono::system_clock::now();
#endif
	vector<WritePairs> writepairs; // dim birth death
	writepairs.clear();
	//writepairs.reserve();
	
	//string filename("data-1");
	//string filename("noise_2_1000");
	string filename("test_2_100");

	DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/"+filename+".complex", 99999, 0);
	

	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/data-1.complex", 99999, 0);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/noise_2_1000.complex", 99999, 0);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/test_2_10.complex", 99999, 0);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/test_2_100.complex", 99999, 0);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/threeDiamonds.txt", 99999, 1);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/linksample.txt", 99999, 1);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/linksample2.txt", 99999, 1);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/one_square_cubical.txt", 99999, 1);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/sample1_cubical.txt", 99999, 1);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/test_2_10.diagram", 99999, 3);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("test.diagram", 99999, 3);
	//DenseCubicalGrids* dcg = new DenseCubicalGrids("dat/data-1.diagram", 99999, 3);
	ColumnsToReduce* ctr = new ColumnsToReduce(dcg); 

	cout << filename << ".complex" << endl;
#ifdef LINK_FIND
	JointPairs* jp = new JointPairs(dcg, ctr, writepairs);
	jp->joint_pairs_main();
	cout << "dim0 pair : " << writepairs.size() << endl;
	int a = writepairs.size();
	ComputePairs* cp = new ComputePairs(dcg, ctr, writepairs);
	cp->compute_pairs_main(); // dim1
	cout << "dim1 pair : " << writepairs.size() - a << endl;
#endif

#ifdef COMPUTEPAIRS	
	ComputePairs* cp = new ComputePairs(dcg, ctr, writepairs);
	cp->compute_pairs_main(); // dim0
	cp->assemble_columns_to_reduce();
	cout << "dim0 pair : " << writepairs.size() << endl;
	int a = writepairs.size();
	cp->compute_pairs_main(); // dim1
	cout << "dim1 pair : " << writepairs.size() - a << endl;
#endif

#ifdef FILE_OUTPUT
	string outname = "dat/"+filename+".diagram";
	ofstream writing_file;
	writing_file.open(outname, ios::out | ios::binary);

	if(!writing_file.is_open()){
		cout << " error: open file for output failed! " << endl;
	}

	int64_t mn = 8067171840;
	writing_file.write((char *) &mn, sizeof( int64_t )); // magic number
	int64_t type = 2;
	writing_file.write((char *) &type, sizeof( int64_t )); // type number of PERSISTENCE_DIAGRAM
	int64_t p = writepairs.size();
	cout << "the number of pairs : " << p << endl;
	writing_file.write((char *) &p, sizeof( int64_t )); // number of points in the diagram p
	for(int64_t i = 0; i < p; ++i){
		int64_t writedim = writepairs[i].getDimension();
		writing_file.write((char *) &writedim, sizeof( int64_t )); // dim

		double writebirth = writepairs[i].getBirth();
		writing_file.write((char *) &writebirth, sizeof( double )); // birth
		
		double writedeath = writepairs[i].getDeath();
		writing_file.write((char *) &writedeath, sizeof( double )); // death
	}
	writing_file.close();
#endif

#ifdef CSV_OUTPUT
	string outname = "dat/"+filename+".csv";
	ofstream writing_file;
	writing_file.open(outname, ios::out);

	if(!writing_file.is_open()){
		cout << " error: open file for output failed! " << endl;
	}

	int64_t p = writepairs.size();
	for(int64_t i = 0; i < p; ++i){
		writing_file << writepairs[i].getDimension() << ",";

		writing_file << writepairs[i].getBirth() << ",";
		writing_file << writepairs[i].getDeath() << endl;
	}
	writing_file.close();
#endif

#ifdef TIME_MEASURING
	const auto endTime = chrono::system_clock::now();
	const auto timeSpan = endTime - startTime;
	cout << "processing-time : " << chrono::duration_cast<std::chrono::milliseconds>(timeSpan).count() << "[ms]" << std::endl;
#endif

	return 0;

}

