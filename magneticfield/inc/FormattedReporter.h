#include <fstream>
#include <cstdlib>
#include <iomanip>

//  Auxiliary methods - should be encapsulated into a separate helper class

namespace ReportValuesOfVectors
{

const int sDefaultNameLength= 10;
const int sDefaultVarSize =   10;

// using std::setw;
// using std::cout;
// using std::endl;

template< typename Real_v >
   void
   ReportRowOfDoubles( std::string  varName,
                       const Real_v varValue, 
                       int          charName = -1,
                       int          charValue = -1 )
{
   if( charName < 0 ) { charName  =  sDefaultNameLength; }
   if( charValue < 0 ) { charValue = sDefaultVarSize; }
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {   
      std::cout << " " << std::setw( charValue ) << vecCore::Get( varValue, i ) << " | ";      
   }
   std::cout << std::endl;   
}

// ---------------------------------------------

template< typename Real_v >
   void
   ReportManyRowsOfDoubles( std::string  varArrName,
                            const Real_v varArr[],
                            int          arrLen, 
                            int          charName = -1,
                            int          charValue = -1 )
{
   for( int i=0; i< arrLen; i++ ) {
      // std::ostringstream nameAndIndex;
      // nameAndIndex << varArrName << "[" << i << "]/AF";
      // ReportRowOfDoubles( nameAndIndex.str(), yValues[i] );
      std::string nameAndIndex = varArrName + "[" + std::to_string(i) + "]/AF";
      ReportRowOfDoubles<Real_v>( nameAndIndex, varArr[i], charName, charValue );
   }
}

// ---------------------------------------------

template< typename Real_v >
   void
   ReportRowOfBools( std::string  varName,
                    const vecCore::Mask_v<Real_v> var,
                    int          charName= -1,
                    int          charValue= -1)
{
   if( charName < 0 ) { charName  =  sDefaultNameLength; }
   if( charValue < 0 ) { charValue = sDefaultVarSize; }
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      std::cout << " " << std::setw( charValue ) << vecCore::Get( var, i ) << " | ";
   }
   std::cout << std::endl;
}

// ---------------------------------------------


// ===============  Selective Reporting / Printing ==================

template< typename Real_v >
void
ReportRowOfDoublesIf( std::string    varName,
                      const Real_v   var, 
                      vecCore::Mask_v<Real_v> cond,
                      int            charName = -1,
                      int            charValue = -1                 
   )
{
   if( charName < 0 ) { charName  =  sDefaultNameLength; }
   if( charValue < 0 ) { charValue = sDefaultVarSize; }
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      if( Get( cond, i ) )
         std::cout << " " << std::setw( charValue ) << Get(var, i) << " | ";      
   }
   std::cout << std::endl;
}


};  // End of namespace
