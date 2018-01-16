
#include <string>
#include <iostream>
#include <vector>
#include <cassert>
#include <ctime>
#include <cmath> //for sqrt
#include <stdlib.h>

#include "base/Vector.h"

//#include "test/unit_tests/ApproxEqual.h"
#include "ApproxEqual.h"

#include "base/Vector3D.h"
#include "base/Global.h"

#include "CMSmagField.h"
#include <Geant/VectorTypes.h>

#undef NDEBUG

using namespace std;


using Double_v = Geant::Double_v;
using ThreeVector = vecgeom::Vector3D<double>;
using ThreeVector_v = vecgeom::Vector3D<Double_v>;

template <typename T>
using vector_t = std::vector<T>;
//constexpr float tesla     = fieldUnits::tesla; 
//constexpr float kilogauss = fieldUnits::kilogauss;
constexpr float millimeter = fieldUnits::millimeter;

const double kRMax=9000 * millimeter;
const double kZMax= 16000 * millimeter;

double RandR(){
    double rnd = (double)rand()/(RAND_MAX) ;
    return rnd * kRMax;
}

double RandZ(){
   double rnd = (double)rand()/(RAND_MAX) ;
   return -kZMax + 2*rnd*kZMax;
}

void GenVecCartSubR(double &x, double &y){
    x = RandR();
    y = RandR();
    if((x*x + y*y)> kRMax*kRMax){
        GenVecCartSubR(x,y);
    }
}

void GenVecCart(ThreeVector &pos){
    double rnd = (double)rand()/(RAND_MAX) ;
    double phi = 2. * M_PI * rnd;
    double r = RandR();
    double x = r * vecCore::math::Cos(phi);
    double y = r * vecCore::math::Sin(phi);
    double z = RandZ();
    pos.Set(x, y, z);
}

void GenVecCart(vector_t<ThreeVector> &posVec, const int &n){
    for (int i = 0; i < n; ++i)
    {       
        ThreeVector pos;
        GenVecCart(pos);
        posVec.push_back(pos);
    }
}

int main()
{   
    CMSmagField m1;
    m1.ReadVectorData("../VecMagFieldRoutine/cmsmagfield2015.txt");
    vector_t<ThreeVector> posVec;
    
    int n = 1e+4;

    srand(time(NULL));
    //srand(2);
    GenVecCart(posVec, n);
    cout<<"Size of posVec is: "<<posVec.size()<<endl;

    ThreeVector sumXYZField(0., 0., 0.), xyzField;
    vector_t<ThreeVector> outputScalar;

    cout<<"Scalar fields start: "<<endl;

    for (int i = 0; i < n; ++i)
    {
        m1.GetFieldValue<double>(posVec[i], xyzField);
        // m1.GetFieldValue(posVec[i], xyzField);        
        sumXYZField += xyzField;
        outputScalar.push_back(xyzField);
    }
    cout<<sumXYZField<<endl;
    for (int i = 0; i < 8; ++i)
    {
        cout<<outputScalar[i]<<endl;
    }
 

    cout<<"\nVector fields start: "<<endl;
    
    size_t inputVcLen = ceil(((double)n)/Geant::kVecLenD);
    ThreeVector_v *inputForVec = new ThreeVector_v[inputVcLen];
    size_t init = 0;
    for (int i = 0; i < n; i+=Geant::kVecLenD){
        for (size_t j = 0; j < Geant::kVecLenD; ++j){
            vecCore::Set(inputForVec[init].x(), j, posVec[i+j].x());
            vecCore::Set(inputForVec[init].y(), j, posVec[i+j].y());
            vecCore::Set(inputForVec[init].z(), j, posVec[i+j].z());
        }
        init++;
    }

    //vector_t<ThreeVector_v> outputVec;
    ThreeVector_v *outputVec = new ThreeVector_v[inputVcLen];
    ThreeVector_v sumXYZField_v, xyzField_v;
    for (size_t i = 0; i < inputVcLen; ++i){
        m1.GetFieldValue<Double_v>(inputForVec[i], outputVec[i]);
        sumXYZField_v += xyzField_v;
    }
    cout<<sumXYZField<<endl;

    cout<<outputVec[0]<<endl;

    for (int i = 0, k=0; i < 256 ; ++i)
    {
        for (size_t j = 0; j < Geant::kVecLenD; ++j)
        {
            //ThreeVector testVec2(xyzField_v[0][j], xyzField_v[1][j], xyzField_v[2][j]);
            cout<<k<<endl;
            ThreeVector testVec(outputVec[i][0][j],outputVec[i][1][j] ,outputVec[i][2][j] );
            cout<<testVec<<" being tested against "<<outputScalar[k]<<endl;
            assert(ApproxEqual(testVec, outputScalar[k] ));
            k++;
        }
       
    }
    


    return 0;

}


