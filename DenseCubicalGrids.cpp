/* DenseCubicalGrids.cpp

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


#include <cassert>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "DenseCubicalGrids.h"


using namespace std;


DenseCubicalGrids::DenseCubicalGrids(const string& filename, double _threshold, file_format _format)  {
	
	threshold = _threshold;
	format = _format;

	if(format == DIPHA){ // ???.complex, DIPHA format
		ifstream reading_file; 

		ifstream fin( filename, ios::in | ios::binary ); 
		cout << filename << endl;

		int64_t d;
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // magic number
		assert(d == 8067171840);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // type number
		assert(d == 1);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); //data num
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // dim
		dim = d;
		assert(dim == 2);
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ax = d;
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ay = d;
		assert(0 < ax && ax < 2000 && 0 < ay && ay < 1000);
		cout << "ax : ay = " << ax << " : " << ay << endl;


		double dou;
		for (int y = 0; y < ay + 2; ++y) {
			for (int x = 0; x < ax + 2; ++x) {
				if(0 < x && x <= ax && 0 < y && y <= ay){
					if (!fin.eof()) {
						fin.read( ( char * ) &dou, sizeof( double ) );
						dense2[x][y] = dou;
					} else {
						cout << "file endof error " << endl;
					}
				}
				else {
					dense2[x][y] = threshold;
				}
			}
		}
		fin.close();
	}  else if(format == PERSEUS){// PERSEUS format

		ifstream reading_file; 
		reading_file.open(filename.c_str(), ios::in);
		cout << filename << endl;

		string reading_line_buffer; 
		getline(reading_file, reading_line_buffer); 
		dim = atoi(reading_line_buffer.c_str()); 
		getline(reading_file, reading_line_buffer); 
		ax = atoi(reading_line_buffer.c_str()); 
		getline(reading_file, reading_line_buffer); 
		ay = atoi(reading_line_buffer.c_str()); 
		assert(0 < ax && ax < 2000 && 0 < ay && ay < 1000);
		cout << "ax : ay = " << ax << " : " << ay << endl;

		for (int y = 0; y <ay + 2; ++y) { 
			for (int x = 0; x < ax + 2; ++x) { 
				if(0 < x && x <= ax && 0 < y && y <= ay){ 
					if (!reading_file.eof()) { 
						getline(reading_file, reading_line_buffer); 
						dense2[x][y] = atoi(reading_line_buffer.c_str()); 
						if (dense2[x][y] == -1) { 
							dense2[x][y] = threshold;
						} 
					} 
				} 
				else { 
					dense2[x][y] = threshold; 
				} 
			} 
		}
	} 
}


double DenseCubicalGrids::getBirthday(int index, int dim){
	int cx = index & 0x07ff;
	int cy = (index >> 11) & 0x03ff;
	int cm = (index >> 21) & 0xff;

	switch(dim){
	case 0:
		return dense2[cx][cy];
	case 1:
		switch(cm){
		case 0: 
			return max(dense2[cx][cy], dense2[cx + 1][cy]);
		default:
		 	return max(dense2[cx][cy], dense2[cx][cy + 1]);
		}
	case 2:
		return max({dense2[cx][cy], dense2[cx + 1][cy], dense2[cx][cy + 1], dense2[cx + 1][cy + 1]});
	}
	return threshold;
}