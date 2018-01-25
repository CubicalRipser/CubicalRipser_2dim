//
//DenseCubicalGrids.cpp

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>

#include "DenseCubicalGrids.h"


using namespace std;

// double threshold;
// int dim;
// int ax, ay;
// double dense2[2048][1024];
// int flag;

DenseCubicalGrids::DenseCubicalGrids(const std::string& filename, double _threshold, int _flag)  {
	
	threshold = _threshold;
	flag = _flag;

	if(flag == 0){// ???.complex, DIPHA format
		std::ifstream reading_file; 

		ifstream fin( filename, ios::in | ios::binary ); 
		int64_t d;

		fin.read( ( char * ) &d, sizeof( int64_t ) ); // magic number
		//assert(d == 8067171840);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // type number
		//assert(d == 1);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); //data num
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // dim 
		dim = d;
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ax = d;
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ay = d;
		std::cout << "ax : ay = " << ax << " : " << ay <<std::endl;


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
	}  else if(flag == 1){// PERCEUS format

		std::ifstream reading_file; 
		reading_file.open(filename.c_str(), std::ios::in); 

		std::string reading_line_buffer; 
		std::getline(reading_file, reading_line_buffer); 
		dim = std::atoi(reading_line_buffer.c_str()); 
		std::getline(reading_file, reading_line_buffer); 
		ax = std::atoi(reading_line_buffer.c_str()); 
		std::getline(reading_file, reading_line_buffer); 
		ay = std::atoi(reading_line_buffer.c_str()); 

		for (int y = 0; y <ay+2; y++) { 
			for (int x = 0; x < ax+2; x++) { 
				if(0<x && x<=ax && 0<y && y<=ay){ 
					if (!reading_file.eof()) { 
						std::getline(reading_file, reading_line_buffer); 
						dense2[x][y] = std::atoi(reading_line_buffer.c_str()); 
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
	} else if(flag == 2){ // convert "DIPHA format" into "Perseus format"
		std::ifstream reading_file; 

		ifstream fin( filename, ios::in | ios::binary ); 
		int64_t d;

		string outname = "test.txt";
		ofstream writing_file;
		writing_file.open(outname, ios::out);

		fin.read( ( char * ) &d, sizeof( int64_t ) ); // magic number
		//assert(d == 8067171840);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // type number
		//assert(d == 1);
		fin.read( ( char * ) &d, sizeof( int64_t ) ); //data num
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // dim 
		dim = d;
		writing_file << dim << std::endl;
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ax = d;
		writing_file << ax << std::endl;
		fin.read( ( char * ) &d, sizeof( int64_t ) );
		ay = d;
		writing_file << ay << std::endl;
		std::cout << "ax : ay = " << ax << " : " << ay <<std::endl;

		double dou;
		for (int y = 0; y < ay + 2; ++y) {
			for (int x = 0; x < ax + 2; ++x) {
				if(0 < x && x <= ax && 0 < y && y <= ay){
					if (!fin.eof()) {
						fin.read( ( char * ) &dou, sizeof( double ) );
						dense2[x][y] = dou;
						writing_file << dou << std::endl;
					} else {
						cout << "file endof error " << endl;
					}
				}
				else {
					dense2[x][y] = threshold;
				}
			}
		}

		writing_file.close();
		fin.close();
	} else if(flag == 3){ // covert "DIPHA answer" into "list format"
		std::ifstream reading_file; // **.diagram

		ifstream fin( filename, ios::in | ios::binary ); 
		int64_t d;

		string outname = "answer.txt";
		ofstream writing_file;
		writing_file.open(outname, ios::out);

		fin.read( ( char * ) &d, sizeof( int64_t ) ); // magic number
		//assert(d == 8067171840);
		writing_file << d << std::endl;
		fin.read( ( char * ) &d, sizeof( int64_t ) ); // type number
		//assert(d == 2);
		writing_file << d << std::endl;
		fin.read( ( char * ) &d, sizeof( int64_t ) ); //data num
		int64_t p = d;
		writing_file << p << std::endl;

		double dou;
		double birth;
		double death;
		for(int i = 0; i < p; i++){
			// dim
			fin.read( ( char * ) &d, sizeof( int64_t ) );
			dim = d;
			//writing_file << dim << std::endl;

			// birth
			fin.read( ( char * ) &dou, sizeof( double ) );
			birth = dou;
			//writing_file << birth << std::endl;

			// death
			fin.read( ( char * ) &dou, sizeof( double ) );
			death = dou;
			//writing_file << death << std::endl;

			writing_file << dim << "," << birth << "," << death << endl;

			cout << "dim " << dim << " : [" << birth << "," << death << ")" << endl; 
		}

		writing_file.close();
		fin.close();
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
			return max(dense2[cx    ][cy    ], dense2[cx + 1][cy    ]);
		default:
		 	return max(dense2[cx    ][cy    ], dense2[cx    ][cy + 1]);
		}
	case 2:
		return max({dense2[cx][cy], dense2[cx + 1][cy], dense2[cx][cy + 1], dense2[cx + 1][cy + 1]});
	}
	return threshold;
}

// void DenseCubicalGrids::GetSimplexVertices(int index, int dim, Vertices* v){
// 	int cx = index & 0x07ff;
// 	int cy = (index>>11)&0x03ff;
// 	int cm = (index>>21)&0xff;
// 	switch (dim) {
// 	case 0:
// 		v->setVertices(cx, cy);
// 	break;
// 	case 1:
// 		switch(cm){
// 		case 0:
// 			v->setVertices(cx  , cy  , cx+1,cy  );
// 			break;
// 		default:
// 			v->setVertices(cx  , cy  , cx  ,cy+1);
// 			break;
// 		}
// 		break;
// 	case 2:
// 		cout << "error: DenseCubicalGrids::GetSimplexVertices, dim=2" << endl;
// 		exit(0);
// 	}
// }

