// from https://github.com/cvneves/kit-opt/blob/master/GILS-RVND-TSP/leitor-instancias/src

#ifndef DATA_H
#define DATA_H

#define INFINITE 0

#include <iostream>
#include <string>
#include <fstream>
#include <math.h>
#include <memory>

class Data{
public:
	Data( int, const char * );

	void read();
	void printMatrixDist();
	inline int getDimension(){ return dimension; };
	inline double getDistance(int i, int j){return distMatrix[i-1][j-1]; };
	inline double **getMatrixCost(){return distMatrix.get(); }
	inline double getXCoord(int i){return xCoord[i-1];}
	inline double getYCoord(int i){return yCoord[i-1];}
	inline bool getExplicitCoord(){return explicitCoord; };

	std::string getInstanceName();                    //Get instance's name

private:
	std::string instaceName;
	int nbOfPar;

	int dimension;

	std::shared_ptr<double*[]> distMatrix;
	std::shared_ptr<double[]> xCoord, yCoord;

	//Computing Distances
	static double CalcDistEuc ( double *, double *, int , int );
	static double CalcDistAtt ( double *, double *, int , int );
	static double CalcDistGeo ( double *, double *, int , int );
	static void CalcLatLong ( double *, double *, int , double *, double* );

	bool explicitCoord;
};

#endif
