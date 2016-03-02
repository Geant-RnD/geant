//
//  Compare the output of different steppers
// 
//  Based on testStepperFixed.cc
//    was the work of Somnath Banerjee in GSoC 2015
//
#include <iomanip>
#include <ctime>

#include "Units.h"

// using fieldUnits::meter;
using fieldUnits::millimeter;   
using fieldUnits::second;  
using fieldUnits::eplus;  
using fieldUnits::tesla;
using fieldUnits::degree;

#include <Vc/Vc>
#include "base/Vector3D.h"

#include "TUniformMagField.h"
#include "TMagFieldEquation.h"
#include "GUVIntegrationStepper.h"
#include "StepperFactory.h"
#include "GUFieldTrack.h"
#include "GUIntegrationDriver.h"

#include "TemplateTUniformMagField.h"
#include "TemplateTMagFieldEquation.h"
#include "TemplateFieldEquationFactory.h"
#include "TemplateGUVIntegrationStepper.h"
#include "TemplateGUTCashKarpRKF45.h"
#include "TemplateGUIntegrationDriver.h"
#include "FieldTrack.h"

#include <stdlib.h>

// #include "CMSmagField.h"

using namespace std;

int main(int argc, char *args[])
{
  constexpr unsigned int Nposmom= 6; // Position 3-vec + Momentum 3-vec

  using Backend = vecgeom::kVc ;
  typedef vecgeom::Vector3D<double> ThreeVector_d;
  
// #define USECMSFIELD
#ifdef USECMSFIELD
  using Field_Type = CMSmagField;
#else
  using Field_Type = TemplateTUniformMagField<Backend>;
#endif 

  using  GvEquationType=  TemplateTMagFieldEquation<Backend, Field_Type, Nposmom>;
  

  /* -----------------------------SETTINGS-------------------------------- */
  
  /* Parameters of test
   - Modify values  */
  
  int no_of_steps = 20;         // No. of Steps for the stepper
  int stepper_no =  5;         // Choose stepper no., for refernce see above
  double step_len_mm = 200.;    // meant as millimeter;  //Step length 
  double z_field_in = DBL_MAX;
  
  //Checking for command line values :
  if(argc>1)
      stepper_no = atoi(args[1]);
  if(argc > 2)
     step_len_mm = (float)(stof(args[2]));   // *mm);
  if(argc > 3)
      no_of_steps = atoi(args[3]);
  if(argc > 4)
     z_field_in = (float) (stof(args[4]));     // tesla
  double step_len = step_len_mm * fieldUnits::millimeter;
  
  //Set Charge etc.
  double particleCharge = +1.0;      // in e+ units
  
  //Choice of output coordinates
/*  int
  columns[] =
  {
     1 , 1 , 1 ,  // position  x, y, z 
     1 , 1 , 1 ,  // momentum  x, y, z
     0 , 0 , 0 ,  // dydx pos  x, y, z
     1 , 1 , 0    // dydx mom  x, y, z
  }; *///Variables in yOut[] & dydx[] we want to display - 0 for No, 1 for yes

  // const unsigned int nwdf= 12;  // Width for float/double
  
  //Set coordinates here
  double
     x_pos = 0.,                 //pos - position  : input unit = mm
     y_pos = 0.,
     z_pos = 0.;
  double   
     x_mom = 0.,                 //mom - momentum  : input unit = GeV / c
     y_mom = 1.,
     z_mom = 1.;
  double          
     x_field = 0.,               //Uniform Magnetic Field (x,y,z)
     y_field = 0.;               //  Tesla // *tesla ;
  double z_field;
  // vecgeom::MaskedAssign(z_field_in < DBL_MAX, z_field_in, &z_field);

  if( z_field_in < DBL_MAX )
     z_field = z_field_in;
  else
     z_field = -1.0;  //  Tesla // *tesla ;

   #ifdef DEBUGAnanya
    cout<<"----Just before making TemplateTUniformMagField"<<endl;
   #endif 



  // CMSmagField m1("../VecMagFieldRoutine/cms2015.txt");

  // Field
#ifdef USECMSFIELD
  auto gvField = new Field_Type ("../VecMagFieldRoutine/cms2015.txt");
#else 
  auto gvField= new Field_Type( fieldUnits::tesla * ThreeVector_d(x_field, y_field, z_field) );
#endif
  
  #ifdef DEBUGAnanya
   cout<<"----TemplateTUniformMagField Object constructed"<<endl;
  #endif
  cout << "#  Initial  Field strength (GeantV) = "
       << x_field << " , " << y_field << " , " << z_field 
     // << (1.0/fieldUnits::tesla) * gvField->GetValue()->X() << ",  "
       << " Tesla " << endl;
  cout << "#  Initial  momentum * c = " << x_mom << " , " << y_mom << " , " << z_mom << " GeV " << endl;
  //Create an Equation :
  #ifdef DEBUGAnanya
    cout<<"----Just before making EquationFactory"<<endl;
  #endif 
  auto gvEquation =
     TemplateFieldEquationFactory<Backend>::CreateMagEquation< Field_Type >(gvField);
  #ifdef DEBUGAnanya
     cout<<"----EquationFactory made "<<endl;
  #endif 

  // gvEquation->InitializeCharge( particleCharge );  // Send it via Stepper instead    

  /*-------------------------PREPARING STEPPER-----------------------------*/
  
  //Create a stepper :

#ifdef DEBUGAnanya
   cout<<"---- "<<endl;
   cout<<"---- Making TemplateGUTCashKarpRKF45"<<endl;
#endif   
  // TemplateGUTCashKarpRKF45<Backend,GvEquationType,Nposmom> myStepper2(gvEquation);
#ifdef DEBUGAnanya
  cout<<"---- constructed TemplateGUTCashKarpRKF45"<<endl;
#endif
/*    TemplateGUVIntegrationStepper<Backend> *myStepper;
  myStepper = &myStepper2;*/

  TemplateGUVIntegrationStepper<Backend> *myStepper = new TemplateGUTCashKarpRKF45<Backend,GvEquationType,Nposmom>(gvEquation);

  myStepper->InitializeCharge( particleCharge );

  //Initialising coordinates
  const double mmGVf = fieldUnits::millimeter;
  const double ppGVf = fieldUnits::GeV ;  //   it is really  momentum * c_light
                                       //   Else it must be divided by fieldUnits::c_light;
  // const double ppGVf = fieldUnits::GeV / Constants::c_light;     // OLD

/*  double yIn[] = {x_pos * mmGVf, y_pos * mmGVf ,z_pos * mmGVf,
                  x_mom * ppGVf ,y_mom * ppGVf ,z_mom * ppGVf};*/

  std::cout << "# step_len_mm = " << step_len_mm;
  
  
  //Empty buckets for results
/*  Double dydx[8] = {0.,0.,0.,0.,0.,0.,0.,0.},  // 2 extra safety buffer
         yout[8] = {0.,0.,0.,0.,0.,0.,0.,0.},
         yerr[8] = {0.,0.,0.,0.,0.,0.,0.,0.};*/
  
  /*-----------------------END PREPARING STEPPER---------------------------*/


  //=======================Test part for Integration driver====================
  double hminimum = 0.2;
  auto testVectorDriver = new TemplateGUIntegrationDriver<Backend>(hminimum, myStepper);

  // Preparing scalar Integration Driver
  using  GvEquationTypeScalar=  TMagFieldEquation<TUniformMagField, Nposmom>;
  
  auto gvFieldScalar    = new TUniformMagField( fieldUnits::tesla * ThreeVector_d(x_field, y_field, z_field) );
  auto gvEquationScalar = new GvEquationTypeScalar(gvFieldScalar);

  GUVIntegrationStepper *myStepperScalar; 
  myStepperScalar= StepperFactory::CreateStepper<GvEquationTypeScalar>(gvEquationScalar, stepper_no);

  int statisticsVerbosity = 1;
  auto testScalarDriver= new GUIntegrationDriver( hminimum,
                                                  myStepperScalar,
                                                  Nposmom,
                                                  statisticsVerbosity); 
  testScalarDriver->InitializeCharge( particleCharge );



  double total_step = 0.;

  typedef typename Backend::bool_v Bool;
  Bool goodAdvance(true);
  double epsTol = 1.0e-5;

  // goodAdvance = testDriver->AccurateAdvance( yTrackIn, total_step, epsTol, yTrackOut );

  int nTracks = 16;
  FieldTrack yInput[nTracks], yOutput[nTracks];
  // double posMom[] ={0., 0., 0., 0., 1., 1.};

  double hstep[nTracks] = {0}; // = {0, 0, 0, 1, -.3, .4, 20, 178., 920.}; 
  bool   succeeded[nTracks];


#define TIMINGTESTING 
// #define MAINTESTING
#define CALCULATETIME

#ifdef MAINTESTING 
  int nRepititions = 1;
#ifdef CALCULATETIME
  nRepititions = 100;
  std::vector<double> ratioVector;
  long double outputVarForScalar=0, outputVarForVector=0;
  int indOutputVar=1;
  std::vector<GUFieldTrack> v;
#endif 
  for (int step = 0; step < 10; ++step)
  {
    srand(time(NULL));


    x_pos = (float) rand()/(RAND_MAX) ;
    y_pos = (float) rand()/(RAND_MAX) ;
    z_pos = (float) rand()/(RAND_MAX) ;
    x_mom = (float) rand()/(RAND_MAX) ;
    y_mom = (float) rand()/(RAND_MAX) ;
    z_mom = (float) rand()/(RAND_MAX) ;
    double posMom[] = {x_pos * mmGVf, y_pos * mmGVf ,z_pos * mmGVf,
                       x_mom * ppGVf ,y_mom * ppGVf ,z_mom * ppGVf};

    const ThreeVector_d  startPosition( posMom[0], posMom[1], posMom[2]);
    const ThreeVector_d  startMomentum( posMom[3], posMom[4], posMom[5]);
    GUFieldTrack yTrackIn ( startPosition, startMomentum );  // yStart
    GUFieldTrack yTrackOut( startPosition, startMomentum );  // yStart

    for (int j = 0; j < nTracks; ++j)
    {
      yInput [j].LoadFromArray(posMom);
      yOutput[j].LoadFromArray(posMom);
    }
/*
    double X_Pos[nTracks], Y_Pos[nTracks], Z_Pos[nTracks];
    double X_Mom[nTracks], Y_Mom[nTracks], Z_Mom[nTracks];
    double posMomMatrix[nTracks][6];
    for (int i = 0; i < nTracks; ++i)
    {
      X_Pos[i] = (float) rand()/(RAND_MAX) ;
      Y_Pos[i] = (float) rand()/(RAND_MAX) ;
      Z_Pos[i] = (float) rand()/(RAND_MAX) ;
      X_Mom[i] = (float) rand()/(RAND_MAX) ;
      Y_Mom[i] = (float) rand()/(RAND_MAX) ;
      Z_Mom[i] = (float) rand()/(RAND_MAX) ;

      // posMomMatrix[i] = {X_Pos[i] * mmGVf, Y_Pos[i] * mmGVf ,Z_Pos[i] * mmGVf,
      //                    X_Mom[i] * ppGVf ,Y_Mom[i] * ppGVf ,Z_Mom[i] * ppGVf};
      posMomMatrix[i][0] = X_Pos[i] * mmGVf;
      posMomMatrix[i][1] = Y_Pos[i] * mmGVf;
      posMomMatrix[i][2] = Z_Pos[i] * mmGVf;
      posMomMatrix[i][3] = X_Mom[i] * ppGVf;
      posMomMatrix[i][4] = Y_Mom[i] * ppGVf;
      posMomMatrix[i][5] = Z_Mom[i] * ppGVf;
      yInput [i].LoadFromArray(posMomMatrix[i]);
      yOutput[i].LoadFromArray(posMomMatrix[i]);
    }*/

    // Random hstep between 0 and 200 cm (2m)
    // x, y, z values are multiplied with mmRef before being passed to function
    // the value of which is 0.1, so passing 200 directly would be in cm
    for (int i = 0; i < nTracks; ++i)
    {
      hstep[i] = (float) rand()/(RAND_MAX) *200.; 
    }

#ifdef CALCULATETIME
    clock_t biasClock= clock();
    biasClock = clock() - biasClock;
    clock_t biasClock2 = clock();
    biasClock2 = clock() - biasClock2;
    cout<<"biasClock2 is: "<<biasClock2<<" and 1 is : "<<biasClock<<" and diff. in double is : "<< ((float)(biasClock-biasClock2))/CLOCKS_PER_SEC;

    clock_t clock1= clock();
#endif 

    for (int repeat = 0; repeat < nRepititions; ++repeat)
    {
      testVectorDriver->AccurateAdvance( yInput, hstep, epsTol, yOutput, nTracks, succeeded );
      for (int i = 0; i < nTracks; ++i)
      {
        outputVarForVector += yOutput[i].PosMomVector[1];
      }
    }

    // testVectorDriver->AccurateAdvance( yInput, hstep, epsTol, yOutput, nTracks, succeeded );

#ifdef CALCULATETIME
    clock1 = clock() - clock1 - biasClock;
    float clock1InFloat = ((float)clock1)/CLOCKS_PER_SEC;
    cout<<"Vector time is: "<<clock1InFloat<<endl;
#endif 
/*      testScalarDriver->AccurateAdvance( yTrackIn, hstep[0], epsTol, yTrackOut );

    cout<<" yOutput[0] is: "<< yOutput[0]<<" for yInput: "  <<yInput[0]<< endl;
    cout<<" yTrackOut is: " << yTrackOut <<" for yTrackIn: "<<yTrackIn << endl;*/

/*      for (int i = 0; i < nTracks; ++i)
    {
      const ThreeVector_d  startPosition( posMomMatrix[i][0], posMomMatrix[i][1], posMomMatrix[i][2]);
      const ThreeVector_d  startMomentum( posMomMatrix[i][3], posMomMatrix[i][4], posMomMatrix[i][5]);
      GUFieldTrack yTrackIn ( startPosition, startMomentum ); 
      GUFieldTrack yTrackOut( startPosition, startMomentum ); 

      testScalarDriver->AccurateAdvance( yTrackIn, hstep[i], epsTol, yTrackOut );

      cout<<" yOutput["<<i<<"] is: "<< yOutput[i]<<" for yInput: "  <<yInput[i]<< endl;
      cout<<" yTrackOut is: " << yTrackOut <<" for yTrackIn: "<<yTrackIn << endl;
    }
*/
#ifdef CALCULATETIME
    clock_t clock2= clock();
#endif 
    for (int repeat = 0; repeat < nRepititions; ++repeat)
    {
      for (int i = 0; i < nTracks; ++i)
      {
        testScalarDriver->AccurateAdvance( yTrackIn, hstep[i], epsTol, yTrackOut );

    #ifdef CALCULATETIME
        outputXForScalar += yTrackOut .SixVector   [1];
        // outputVarForVector += yOutput[i].PosMomVector[1];
    #else
        cout<<" yOutput["<<i<<"] is: "<< yOutput[i]<<" for yInput: "  <<yInput[i]<< endl;
        cout<<" yTrackOut is : "      << yTrackOut <<" for yTrackIn: "<<yTrackIn <<" for hstep: "<<hstep[i]<< endl;
    #endif 
    

      }
    }

#ifdef CALCULATETIME
    clock2 = clock() - clock2 - biasClock;
    float clock2InFloat = ((float)clock2)/CLOCKS_PER_SEC;
    cout<<"scalar time is: "<<clock2InFloat<<endl;
    // cout<<"ratio is: "<<clock2InFloat/clock1InFloat<<endl;
    ratioVector.push_back(clock2InFloat/clock1InFloat);
#endif 
  }

#ifdef CALCULATETIME
  cout<<"outputXForScalar: "<< outputXForScalar<< endl;
  cout<<"outputVarForVector: "<< outputVarForVector<< endl;
  int sizeOfRatioVector = ratioVector.size(); //no_steps;
  cout<<"Size of ratioVector is: "<<ratioVector.size()<<endl;
  cout<<"Time ratios are: "<<endl;
  for (int i = 0; i < sizeOfRatioVector; ++i)
  {
    cout<< sizeOfRatioVector - i<<" "<<ratioVector.back()<< " " <<endl;
    ratioVector.pop_back();
  }
  cout<<endl;
#endif 

#endif 



// cout<<" Scalar Stepper function calls are: "<< testScalarDriver->fStepperCalls <<" and OneGoodStep calls are "<<testScalarDriver->fNoTotalSteps << endl;
// cout<<" Vector Stepper function calls are: "<< testVectorDriver->fStepperCalls <<" and OneStep calls are "<<testVectorDriver->fNoTotalSteps << endl;

 //=======================Proper timing calculation===========================
 // Storing random input data in array of arrays and 
 // vector of GUFieldTrack for vector and sequential version respectively. 

#ifdef TIMINGTESTING 
  int nRepititions = 1;
  int noOfVectorCalls = 10; // scalarcalls = nTracks*noOfVectorCalls
  nRepititions = 10;
  std::vector<double> ratioVector;
  std::vector<GUFieldTrack> vectorGUFieldTrack;
  long double outputVarForScalar = 0, outputVarForVector = 0;
  int indOutputVar = 2;
    
  // srand(time(NULL));
  srand(34);
  
  for (int step = 0; step < 10; ++step)
  {


    double X_Pos[nTracks], Y_Pos[nTracks], Z_Pos[nTracks];
    double X_Mom[nTracks], Y_Mom[nTracks], Z_Mom[nTracks];
    double posMomMatrix[nTracks][6];
    FieldTrack yInputMatrix[noOfVectorCalls][nTracks]; // [6];

    for (int j = 0; j < noOfVectorCalls; ++j)
    {
      for (int i = 0; i < nTracks; ++i)
      {
        X_Pos[i] = (float) rand()/(RAND_MAX) ;
        Y_Pos[i] = (float) rand()/(RAND_MAX) ;
        Z_Pos[i] = (float) rand()/(RAND_MAX) ;
        X_Mom[i] = (float) rand()/(RAND_MAX) ;
        Y_Mom[i] = (float) rand()/(RAND_MAX) ;
        Z_Mom[i] = (float) rand()/(RAND_MAX) ;

        posMomMatrix[i][0] = X_Pos[i] * mmGVf;
        posMomMatrix[i][1] = Y_Pos[i] * mmGVf;
        posMomMatrix[i][2] = Z_Pos[i] * mmGVf;
        posMomMatrix[i][3] = X_Mom[i] * ppGVf;
        posMomMatrix[i][4] = Y_Mom[i] * ppGVf;
        posMomMatrix[i][5] = Z_Mom[i] * ppGVf;
        yInput [i].LoadFromArray(posMomMatrix[i]);
        yOutput[i].LoadFromArray(posMomMatrix[i]);

        yInputMatrix[j][i] = yInput[i];

        const ThreeVector_d  startPosition( posMomMatrix[i][0], posMomMatrix[i][1], posMomMatrix[i][2]);
        const ThreeVector_d  startMomentum( posMomMatrix[i][3], posMomMatrix[i][4], posMomMatrix[i][5]);
        GUFieldTrack yTrackIn ( startPosition, startMomentum ); 
        vectorGUFieldTrack.push_back(yTrackIn);
      }
    }


    // Random hstep between 0 and 200 cm (2m)
    // x, y, z values are multiplied with mmRef before being passed to function
    // the value of which is 0.1, so passing 200 directly would be in cm
    double hstepMatrix[noOfVectorCalls][nTracks];
    for (int j = 0; j < noOfVectorCalls; ++j)
    {
      for (int i = 0; i < nTracks; ++i)
      {
        hstepMatrix[j][i] = (float) rand()/(RAND_MAX) *200.; 
      }
    }

    for (int i = 0; i < nTracks; ++i)
    {
      hstep[i] = (float) rand()/(RAND_MAX) *200.; 
    }


    clock_t clock1 = clock();
    for (int repeat = 0; repeat < nRepititions; ++repeat)
    {
      for (int j = 0; j < noOfVectorCalls; ++j)
      {
        testVectorDriver->AccurateAdvance( yInputMatrix[j], hstepMatrix[j], epsTol, yOutput, nTracks, succeeded );
        // testVectorDriver->AccurateAdvance( yInputMatrix[j], hstep, epsTol, yOutput, nTracks, succeeded );
        for (int i = 0; i < nTracks; ++i)
        {
          // cout<<" yOutput["<<i<<"] is: "<< yOutput[i]<<" for yInput: "  <<yInput[i]<< endl;
          outputVarForVector += yOutput[i].PosMomVector[indOutputVar];
        }      
      }
    }
    clock1 = clock() - clock1 ;
    float clock1InFloat = ((float)clock1)/CLOCKS_PER_SEC;
    cout<<"Vector time is: "<<clock1InFloat<<" s"<<endl;


    const ThreeVector_d  startPos( x_pos, y_pos, z_pos);
    const ThreeVector_d  startMom( x_mom, y_mom, z_mom);
    GUFieldTrack yTrackOut( startPos, startMom ); 

    clock_t clock2 = clock();
    for (int repeat = 0; repeat < nRepititions; ++repeat)
    {
      int indScalar = 0;
      for (int j = 0; j < noOfVectorCalls; ++j)
      {
        for (int i = 0; i < nTracks; ++i)
        {
          // testScalarDriver->AccurateAdvance( vectorGUFieldTrack[i], hstep[i%nTracks], epsTol, yTrackOut );
          // testScalarDriver->AccurateAdvance( vectorGUFieldTrack[indScalar], hstep[i], epsTol, yTrackOut );
          testScalarDriver->AccurateAdvance( vectorGUFieldTrack[indScalar], hstepMatrix[j][i], epsTol, yTrackOut );
          // cout<<" yTrackOut is : "      << yTrackOut <<" for yTrackIn: "<<vectorGUFieldTrack[indScalar] <<" for hstep: "<<hstep[i]<< endl;

          outputVarForScalar += yTrackOut.SixVector[indOutputVar];
          indScalar++;
        }      
      }
    }
    clock2 = clock() - clock2 ;
    float clock2InFloat = ((float)clock2)/CLOCKS_PER_SEC;
    cout<<"scalar time is: "<<clock2InFloat<<" s"<< endl;
    ratioVector.push_back(clock2InFloat/clock1InFloat);

  }


  cout<<"outputVarForScalar: "<< outputVarForScalar<< endl;
  cout<<"outputVarForVector: "<< outputVarForVector<< endl;
  int sizeOfRatioVector = ratioVector.size(); // no_steps;
  cout<<"Size of ratioVector is: "<<ratioVector.size()<<endl;
  cout<<"Time ratios are: "<<endl;
  for (int i = 0; i < sizeOfRatioVector; ++i)
  {
    cout<< sizeOfRatioVector - i<<" "<<ratioVector.back()<< " " <<endl;
    ratioVector.pop_back();
  }
  cout<<endl;

#endif 

    cout<<" Scalar Stepper function calls are: "<< testScalarDriver->fStepperCalls <<" and OneGoodStep calls are "<<testScalarDriver->fNoTotalSteps << endl;
    cout<<" Vector Stepper function calls are: "<< testVectorDriver->fStepperCalls <<" and OneStep calls are "<<testVectorDriver->fNoTotalSteps << endl;


  //========================End testing IntegrationDriver=======================


  /*------ Clean up ------*/
  myStepper->InformDone(); 
  delete myStepper; 
  delete gvField;

  // deleting IntegrationDriver
  delete testVectorDriver;
  delete testScalarDriver;      
  
  cout<<"\n\n#-------------End of output-----------------\n";
}