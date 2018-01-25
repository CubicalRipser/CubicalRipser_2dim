//SimplexCoboundaryEnumerator.h

class SimplexCoboundaryEnumerator
{
public:
	BirthdayIndex simplex;
	DenseCubicalGrids* dcg;
	int dim;
	double birthtime;
	int ax, ay;
	int cx, cy, cm;
	int count;
	BirthdayIndex nextCoface;
	double threshold;
	const int mode = 1; // 0 -> eight neighbourhoods 、 1 -> four neighbourhoods 
	//
	// 0 4 2
	// 7 * 6
	// 3 5 1
	//
	// int dx[8] = { -1, 1, 1, -1, 0, 0, 1, -1 };
	// int dy[8] = { 1, -1, 1, -1, 1, -1, 0, 0 };
	// int dcx[8] = { -1, 0, 0, -1, 0, 0, 0, -1 };
	// int dcy[8] = { 0, -1, 0, -1, 0, -1, 0, 0 };
	// int cm1[8] = { 3, 3, 2, 2, 1, 1, 0, 0 };

	SimplexCoboundaryEnumerator();
	void setSimplexCoboundaryEnumerator(BirthdayIndex _s, DenseCubicalGrids* _dcg);
	bool hasNextCoface() ;
	BirthdayIndex getNextCoface();
};
