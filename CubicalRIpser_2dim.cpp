/*
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

//#define PRINT_PERSISTENCE_PAIRS
#define FILE_OUTPUT

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

using namespace std;

enum calculation_method { LINKFIND, COMPUTEPAIRS};

void print_usage_and_exit(int exit_code) {
	std::cerr << "Usage: "
	          << "CR2 "
	          << "[options] [input_filename]" << std::endl
	          << std::endl
	          << "Options:" << std::endl
	          << std::endl
	          << "  --help           print this screen" << std::endl
	          << "  --format         use the specified file format for the input. Options are:" << std::endl
	          << "                     dipha          (pixel data in DIPHA file format; default)" << std::endl
	          << "                     perseus        (pixel data in Perseus file format)" << std::endl
	          << "  --threshold <t>  compute cubical complexes up to birth time <t>" << std::endl
	          << "  --method         method to compute the persistent homology of the cubical complexes. Options are" << std::endl
	          << "                     link_find      (calculating the 0-dim PP, use 'link_find' algorithm; default)" << std::endl
	          << "                     compute_pairs  (calculating the 0-dim PP, use 'compute_pairs' algorithm)" << std::endl
	          << "  --output         name of file that will contain the persistence diagram " << std::endl
	          << std::endl;

	exit(exit_code);
}


int main(int argc, char** argv){
	const char* filename = nullptr;
	string output_filename = "answer_2dim.diagram"; //default name
	file_format format = DIPHA;
	calculation_method method = LINKFIND;
	double threshold = 99999;

	for (int i = 1; i < argc; ++i) {
		const std::string arg(argv[i]);
		if (arg == "--help") {
			print_usage_and_exit(0);
		} else if (arg == "--threshold") {
			std::string parameter = std::string(argv[++i]);
			size_t next_pos;
			threshold = std::stod(parameter, &next_pos);
			if (next_pos != parameter.size()) print_usage_and_exit(-1);
		} else if (arg == "--format") {
			std::string parameter = std::string(argv[++i]);
			if (parameter == "dipha") {
				format = DIPHA;
			} else if (parameter == "perseus") {
				format = PERSEUS;
			} else {
				print_usage_and_exit(-1);
			}
		} else if(arg == "--method") {
			std::string parameter = std::string(argv[++i]);
			if (parameter == "link_find") {
				method = LINKFIND;
			} else if (parameter == "compute_pairs") {
				method = COMPUTEPAIRS;
			} else {
				print_usage_and_exit(-1);
			}
		} else if (arg == "--output") {
			output_filename = std::string(argv[++i]);
		} else {
			if (filename) { print_usage_and_exit(-1); }
			filename = argv[i];
		}
	}

    std::ifstream file_stream(filename);
	if (filename && file_stream.fail()) {
		std::cerr << "couldn't open file " << filename << std::endl;
		exit(-1);
	}

	vector<WritePairs> writepairs; // dim birth death
	writepairs.clear();
	

	DenseCubicalGrids* dcg = new DenseCubicalGrids(filename, threshold, format);
	ColumnsToReduce* ctr = new ColumnsToReduce(dcg);
	
	switch(method){
		case LINKFIND:
		{
			JointPairs* jp = new JointPairs(dcg, ctr, writepairs);
			jp->joint_pairs_main(); // dim0

			ComputePairs* cp = new ComputePairs(dcg, ctr, writepairs);
			cp->compute_pairs_main(); // dim1
		
		break;
		}
		
		case COMPUTEPAIRS:
		{
			ComputePairs* cp = new ComputePairs(dcg, ctr, writepairs);
			cp->compute_pairs_main(); // dim0
			cp->assemble_columns_to_reduce();

			cp->compute_pairs_main(); // dim1
	
		break;
		}
	}

	
#ifdef FILE_OUTPUT
	ofstream writing_file;

	string extension = ".csv";
	if(equal(extension.rbegin(), extension.rend(), output_filename.rbegin()) == true){
		string outname = output_filename;
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
	} else {

		writing_file.open(output_filename, ios::out | ios::binary);

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
	}
#endif

	return 0;

}

