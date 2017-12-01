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
   ReportRowOfDoubles( std::string    varName,
                       const Real_v & varValue, 
                       int            charName = -1,
                       int            widthVal = -1 )
{
   using std::cout;
   using std::endl;

   // int argWV= widthVal;
   if( charName <=  0 ) { charName  = sDefaultNameLength; }
   if( widthVal <= 0 ) { widthVal = sDefaultVarSize; }

   int prec = (widthVal - 7);
   prec = std::min( 5, prec );
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
   ReportRowOfSquareRoots( std::string    varName,
                           const Real_v & valueSq,      // Square of interesting value
                           int            charName = -1,
                           int            widthVal = -1 )
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
   ReportManyRowsOfDoubles( std::string    varArrName,
                            const Real_v   varArr[],
                            int            arrLen, 
                            int            charName = -1,
                            int            widthVal = -1 )
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
   Real_v GetMomentumMag( const Real_v varPositionsMomenta[6] )
{
   Real_v px = varPositionsMomenta[3];
   Real_v py = varPositionsMomenta[4];
   Real_v pz = varPositionsMomenta[5];   

   return vecCore::math::Sqrt( px * px + py * py + pz * pz );
}

template< typename Real_v >
   void
   ReportRowsOfPositionsMomenta( std::string    varName, 
                                 const Real_v   varPositionsMomenta[],
                                 int            arrLen,
                                 const Real_v & momentumMagStart, // 
                                 int            widthNm  = -1,
                                 int            widthVal = -1 )
{
   using vecCore::math::Sqrt;
   using vecCore::MaskEmpty;
   using vecCore::MaskFull;   
   if( widthVal < 0 ) { widthVal = sDefaultVarSize; }
   
   ReportManyRowsOfDoubles( varName,
                            varPositionsMomenta,
                            arrLen,
                            widthNm, 
                            widthVal );
   assert( arrLen >= 6 ); 
   
   // Real_v px = varPositionsMomenta[3];
   // Real_v py = varPositionsMomenta[4];
   // Real_v pz = varPositionsMomenta[5];   
   // Real_v momEnd= Sqrt( px * px + py * py + pz * pz );
   
   Real_v momEnd= GetMomentumMag( varPositionsMomenta );
   
   Real_v diffMagP = momEnd - momentumMagStart;
   ReportRowOfDoubles( "diff|p|", diffMagP );

   if( ! MaskFull( momentumMagStart == Real_v( 0.0 ) ) )
   {
      double tinyVal = 1.0e-80;
      Real_v relDiff = diffMagP / ( momentumMagStart + Real_v(tinyVal) ) ;
      ReportRowOfDoubles( "d|p|/|p|",   relDiff  );
      
      double thresholdRelativeDiff = 1.0e-5;   //  Later: 3 * epsilon ??         
      if( ! MaskEmpty( abs(relDiff) > thresholdRelativeDiff ) )
      {
         int extraWidth= widthVal + 12;
         ReportRowOfDoubles( "|momEnd|",   momEnd,           widthNm, extraWidth  );
         ReportRowOfDoubles( "|momStart|", momentumMagStart, widthNm, extraWidth  );
      }else{
         ReportRowOfDoubles( "|momEnd|",   momEnd  );
      }
      std::cout << "##-------------------------------------------------------------------------------"
                << std::endl;
   }
}

// ---------------------------------------------


template< typename Real_v >
   void
   ReportRowOfBools( std::string                     varName,
                     const vecCore::Mask_v<Real_v> & var,
                     int          widthName= -1,
                     int          widthVal= -1)
{
   if( widthName < 0 ) { widthName  =  sDefaultNameLength; }
   if( widthVal < 0 ) { widthVal = sDefaultVarSize; }
   
   std::cout << std::setw( widthName ) << varName << " : ";
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
                      int            widthName = -1,
                      int            widthVal = -1                 
   )
{
   if( widthName < 0 ) { widthName  =  sDefaultNameLength; }
   if( widthVal < 0 )  { widthVal = sDefaultVarSize; }
   
   std::cout << std::setw( widthName ) << varName << " : ";
   for (int i = 0; i < vecgeom::kVectorSize; ++i) {
      if( Get( cond, i ) )
         std::cout << " " << std::setw( widthVal ) << Get(var, i) << " | ";      
   }
   std::cout << std::endl;
}


};  // End of namespace
