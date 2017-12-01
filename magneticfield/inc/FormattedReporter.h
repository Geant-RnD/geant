#include <fstream>
#include <cstdlib>
#include <iomanip>

//  Auxiliary methods - should be encapsulated into a separate helper class

namespace ReportValuesOfVectors
{

const int sDefaultNameLength= 14;
const int sDefaultVarSize =   12;

// using std::setw;
// using std::cout;
// using std::endl;

template< typename Real_v >
   void
   ReportRowOfDoubles( std::string  varName,
                       const Real_v varValue, 
                       int          charName = -1,
                       int          widthVal = -1 )
{
   using std::cout;
   using std::endl;

   // int argWV= widthVal;
   if( charName <=  0 ) { charName  = sDefaultNameLength; }
   if( widthVal <= 0 ) { widthVal = sDefaultVarSize; }

   int prec = (widthVal - 7);
   prec = std::max( 5, prec );
   widthVal = prec + 7;
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {   
      std::cout << " " << std::setw( widthVal )
                << std::setprecision( prec )
                << vecCore::Get( varValue, i ) << " | ";      
   }
   // Auxiliary information about width, precision
   // cout << " withVal: arg= " << argWV << " used= " << widthVal << " ( prec= " << prec << " ) ";
   // cout << " lenName = " << charName; 
   cout << endl;
}

// ---------------------------------------------
template< typename Real_v >
   void
   ReportRowOfSquareRoots( std::string  varName,
                           const Real_v valueSq,      // Square of interesting value
                           int          charName = -1,
                           int          widthVal = -1 )
{
   if( vecCore::MaskEmpty( valueSq < 0.0)  ) {
      Real_v value= vecCore::math::Sqrt( valueSq );
      ReportRowOfDoubles( varName, value, charName, widthVal );
   } else {
      // There is an erroneous value !
      std::string varNameAndWarning= "WARNING> some values are Negative> " + varName;
      ReportRowOfDoubles( varNameAndWarning, valueSq, charName, widthVal );
   }   
}
// ---------------------------------------------

template< typename Real_v >
   void
   ReportManyRowsOfDoubles( std::string  varArrName,
                            const Real_v varArr[],
                            int          arrLen, 
                            int          charName = -1,
                            int          widthVal = -1 )
{
   for( int i=0; i< arrLen; i++ ) {
      // std::ostringstream nameAndIndex;
      // nameAndIndex << varArrName << "[" << i << "]/AF";
      // ReportRowOfDoubles( nameAndIndex.str(), yValues[i] );
      std::string nameAndIndex = varArrName + "[" + std::to_string(i) + "]"; // + "/AF";
      ReportRowOfDoubles<Real_v>( nameAndIndex, varArr[i], charName, widthVal );
   }
   std::cout << "##-------------------------------------------------------------------------------" << std::endl;
}

// ---------------------------------------------

template< typename Real_v >
   void
   ReportRowOfBools( std::string  varName,
                    const vecCore::Mask_v<Real_v> var,
                    int          charName= -1,
                    int          widthVal= -1)
{
   if( charName < 0 ) { charName  =  sDefaultNameLength; }
   if( widthVal < 0 ) { widthVal = sDefaultVarSize; }
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      std::cout << " " << std::setw( widthVal ) << vecCore::Get( var, i ) << " | ";
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
                      int            widthVal = -1                 
   )
{
   if( charName < 0 ) { charName  =  sDefaultNameLength; }
   if( widthVal < 0 ) { widthVal = sDefaultVarSize; }
   
   std::cout << std::setw( charName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      if( Get( cond, i ) )
         std::cout << " " << std::setw( widthVal ) << Get(var, i) << " | ";      
   }
   std::cout << std::endl;
}


};  // End of namespace
