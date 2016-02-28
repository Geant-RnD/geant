// The following code is an autogenerated and specialized Navigator 
// obtained from the NavigationSpecializerService ( which is part of VecGeom )
// ADD INFORMATION ABOUT INPUT FILES, DATE, MD5 HASH OF INPUTFILES
// ADD INFORMATION ABOUT WHICH PARTS HAVE BEEN SPECIALIZED 
// DO NOT MODIFY THIS FILE UNLESS YOU KNOW WHAT YOU ARE DOING 

#include "navigation/VNavigator.h"
#include "navigation/NavigationState.h"
#include "base/Transformation3D.h"
#include "management/GeoManager.h"
#include <Vc/Vc>
#include "backend/Backend.h"
// more relevant includes to be figures out ... 
#include "volumes/Box.h"
#include "volumes/Trapezoid.h"
#include "volumes/Tube.h"
#include "volumes/Polycone.h"
#include "volumes/Polyhedron.h"
#include "volumes/Trd.h"
#include "volumes/Cone.h"
#include "volumes/BooleanVolume.h"
#include "navigation/SimpleSafetyEstimator.h"


namespace vecgeom {
inline namespace VECGEOM_IMPL_NAMESPACE {
class HVQXNavigator : public VNavigatorHelper<HVQXNavigator,1> {
private:

// automatically generated function to transform a path for HVQX into an array index
static size_t PathToIndex( NavigationState const *path ){
size_t finalindex=0;
{
// it might be better to init to -1 ( to detect possible inconsitencies )
size_t levelindex(0);
switch (path->ValueAt( 2 )){
 case 18872 : { levelindex = 0; break; }
 case 18892 : { levelindex = 1; break; }
}
finalindex += levelindex * 1;
}
{
// it might be better to init to -1 ( to detect possible inconsitencies )
size_t levelindex(0);
switch (path->ValueAt( 4 )){
 case 18874 : { levelindex = 0; break; }
 case 18875 : { levelindex = 1; break; }
 case 18876 : { levelindex = 2; break; }
 case 18877 : { levelindex = 3; break; }
 case 18878 : { levelindex = 4; break; }
 case 18879 : { levelindex = 5; break; }
 case 18880 : { levelindex = 6; break; }
 case 18881 : { levelindex = 7; break; }
 case 18882 : { levelindex = 8; break; }
 case 18883 : { levelindex = 9; break; }
 case 18884 : { levelindex = 10; break; }
 case 18885 : { levelindex = 11; break; }
 case 18886 : { levelindex = 12; break; }
 case 18887 : { levelindex = 13; break; }
 case 18888 : { levelindex = 14; break; }
 case 18889 : { levelindex = 15; break; }
 case 18890 : { levelindex = 16; break; }
 case 18891 : { levelindex = 17; break; }
}
finalindex += levelindex * 2;
}
return finalindex;
}


static constexpr double gTrans0[36] = {84.75, -84.75, 79.638949611605710288131376728415489, -79.638949611605710288131376728415489, 64.922266554333404542376229073852301, -64.922266554333404542376229073852301, 42.375, -42.374999999999992894572642398998141, 14.716683057272300416684629453811795, -14.716683057272289758543593052309006, -14.716683057272399892667635867837816, 14.716683057272410550808672269340605, -42.375, 42.375000000000007105427357601001859, -64.922266554333404542376229073852301, 64.922266554333404542376229073852301, -79.638949611605710288131376728415489, 79.638949611605710288131376728415489, -84.75, 84.75, -79.638949611605710288131376728415489, 79.638949611605710288131376728415489, -64.922266554333404542376229073852301, 64.922266554333404542376229073852301, -42.375, 42.374999999999992894572642398998141, -14.716683057272300416684629453811795, 14.716683057272289758543593052309006, 14.716683057272399892667635867837816, -14.716683057272410550808672269340605, 42.375, -42.375000000000007105427357601001859, 64.922266554333404542376229073852301, -64.922266554333404542376229073852301, 79.638949611605710288131376728415489, -79.638949611605710288131376728415489};
static constexpr double gTrans1[36] = {5.1892694243223204762272626306965423e-15, 1.5568151047096136904057770617631166e-14, 28.98620714685040056224352156277746, 28.986207146850411220384557964280248, 54.47624992093420104311007889918983, 54.476249920934208148537436500191689, 73.395652970731205755328119266778231, 73.395652970731205755328119266778231, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 73.395652970731205755328119266778231, 73.395652970731205755328119266778231, 54.47624992093420104311007889918983, 54.476249920934193937682721298187971, 28.98620714685040056224352156277746, 28.986207146850389904102485161274672, -1.3629010843074100238893209005702581e-14, -2.4007892465847915089001906550613594e-14, -28.98620714685040056224352156277746, -28.986207146850411220384557964280248, -54.47624992093420104311007889918983, -54.476249920934208148537436500191689, -73.395652970731205755328119266778231, -73.395652970731205755328119266778231, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -73.395652970731205755328119266778231, -73.395652970731205755328119266778231, -54.47624992093420104311007889918983, -54.476249920934193937682721298187971, -28.986207146850503590940206777304411, -28.986207146850492932799170375801623};
static constexpr double gTrans2[36] = {1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5};
static constexpr double gRot0[36] = {6.123233995736766035868820147291983e-17, -1.8369701987210296875011296034045003e-16, 0.34202014332566882393038554255326744, -0.34202014332566893495268800506892148, 0.64278760968653936291872241781675257, -0.64278760968653947394102488033240661, 0.86602540378443870761060452423407696, -0.866025403784438818632906986749731, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.86602540378443870761060452423407696, -0.86602540378443859658830206171842292, 0.64278760968653936291872241781675257, -0.64278760968653925189641995530109853, 0.34202014332566882393038554255326744, -0.3420201433256687129080830800376134, 6.123233995736766035868820147291983e-17, 6.123233995736766035868820147291983e-17, -0.3420201433256687129080830800376134, 0.34202014332566882393038554255326744, -0.64278760968653936291872241781675257, 0.64278760968653947394102488033240661, -0.86602540378443870761060452423407696, 0.866025403784438818632906986749731, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.86602540378443870761060452423407696, 0.86602540378443859658830206171842292, -0.64278760968653936291872241781675257, 0.64278760968653925189641995530109853, -0.3420201433256687129080830800376134, 0.34202014332566860188578061752195936};
static constexpr double gRot1[36] = {1, -1, 0.93969262078590831688273965482949279, -0.93969262078590831688273965482949279, 0.7660444431189780134516809084743727, -0.76604444311897790242937844595871866, 0.49999999999999994448884876874217298, -0.49999999999999983346654630622651894, 0.1736481776669303311866343619840336, -0.17364817766693022016433189946837956, -0.1736481776669303311866343619840336, 0.17364817766693044220893682449968765, -0.49999999999999994448884876874217298, 0.5, -0.7660444431189780134516809084743727, 0.76604444311897812447398337099002674, -0.93969262078590831688273965482949279, 0.93969262078590831688273965482949279, -1, 1, -0.93969262078590842790504211734514683, 0.93969262078590842790504211734514683, -0.7660444431189780134516809084743727, 0.76604444311897790242937844595871866, -0.49999999999999994448884876874217298, 0.49999999999999983346654630622651894, -0.17364817766693027567548313072620658, 0.17364817766693016465318066821055254, 0.17364817766693027567548313072620658, -0.17364817766693038669778559324186062, 0.49999999999999994448884876874217298, -0.5, 0.7660444431189780134516809084743727, -0.76604444311897812447398337099002674, 0.93969262078590842790504211734514683, -0.93969262078590842790504211734514683};
static constexpr double gRot3[36] = {-1, -1, -0.93969262078590831688273965482949279, -0.93969262078590831688273965482949279, -0.7660444431189780134516809084743727, -0.76604444311897790242937844595871866, -0.49999999999999994448884876874217298, -0.49999999999999983346654630622651894, -0.1736481776669303311866343619840336, -0.17364817766693022016433189946837956, 0.1736481776669303311866343619840336, 0.17364817766693044220893682449968765, 0.49999999999999994448884876874217298, 0.5, 0.7660444431189780134516809084743727, 0.76604444311897812447398337099002674, 0.93969262078590831688273965482949279, 0.93969262078590831688273965482949279, 1, 1, 0.93969262078590842790504211734514683, 0.93969262078590842790504211734514683, 0.7660444431189780134516809084743727, 0.76604444311897790242937844595871866, 0.49999999999999994448884876874217298, 0.49999999999999983346654630622651894, 0.17364817766693027567548313072620658, 0.17364817766693016465318066821055254, -0.17364817766693027567548313072620658, -0.17364817766693038669778559324186062, -0.49999999999999994448884876874217298, -0.5, -0.7660444431189780134516809084743727, -0.76604444311897812447398337099002674, -0.93969262078590842790504211734514683, -0.93969262078590842790504211734514683};
static constexpr double gRot4[36] = {6.123233995736766035868820147291983e-17, 1.8369701987210296875011296034045003e-16, 0.34202014332566882393038554255326744, 0.34202014332566893495268800506892148, 0.64278760968653936291872241781675257, 0.64278760968653947394102488033240661, 0.86602540378443870761060452423407696, 0.866025403784438818632906986749731, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.86602540378443870761060452423407696, 0.86602540378443859658830206171842292, 0.64278760968653936291872241781675257, 0.64278760968653925189641995530109853, 0.34202014332566882393038554255326744, 0.3420201433256687129080830800376134, 6.123233995736766035868820147291983e-17, -6.1232339957367635706784913316300911e-17, -0.3420201433256687129080830800376134, -0.34202014332566882393038554255326744, -0.64278760968653936291872241781675257, -0.64278760968653947394102488033240661, -0.86602540378443870761060452423407696, -0.866025403784438818632906986749731, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.86602540378443870761060452423407696, -0.86602540378443859658830206171842292, -0.64278760968653936291872241781675257, -0.64278760968653925189641995530109853, -0.3420201433256687129080830800376134, -0.34202014332566860188578061752195936};
static constexpr double gRot8[36] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1};
// ------- generated tables ------
static constexpr double globalTranstrans0[36] = {84.75, -84.75, 79.638949611605710288131376728415489, -79.638949611605710288131376728415489, 64.922266554333404542376229073852301, -64.922266554333404542376229073852301, 42.375, -42.374999999999992894572642398998141, 14.716683057272300416684629453811795, -14.716683057272289758543593052309006, -14.716683057272399892667635867837816, 14.716683057272410550808672269340605, -42.375, 42.375000000000007105427357601001859, -64.922266554333404542376229073852301, 64.922266554333404542376229073852301, -79.638949611605710288131376728415489, 79.638949611605710288131376728415489, -84.75, 84.75, -79.638949611605710288131376728415489, 79.638949611605710288131376728415489, -64.922266554333404542376229073852301, 64.922266554333404542376229073852301, -42.375, 42.374999999999992894572642398998141, -14.716683057272300416684629453811795, 14.716683057272289758543593052309006, 14.716683057272399892667635867837816, -14.716683057272410550808672269340605, 42.375, -42.375000000000007105427357601001859, 64.922266554333404542376229073852301, -64.922266554333404542376229073852301, 79.638949611605710288131376728415489, -79.638949611605710288131376728415489};
static constexpr double globalTranstrans1[36] = {5.1892694243223204762272626306965423e-15, 1.5568151047096136904057770617631166e-14, 28.98620714685040056224352156277746, 28.986207146850411220384557964280248, 54.47624992093420104311007889918983, 54.476249920934208148537436500191689, 73.395652970731205755328119266778231, 73.395652970731205755328119266778231, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 83.462457067784612263494636863470078, 73.395652970731205755328119266778231, 73.395652970731205755328119266778231, 54.47624992093420104311007889918983, 54.476249920934193937682721298187971, 28.98620714685040056224352156277746, 28.986207146850389904102485161274672, -1.3629010843074100238893209005702581e-14, -2.4007892465847915089001906550613594e-14, -28.98620714685040056224352156277746, -28.986207146850411220384557964280248, -54.47624992093420104311007889918983, -54.476249920934208148537436500191689, -73.395652970731205755328119266778231, -73.395652970731205755328119266778231, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -83.462457067784612263494636863470078, -73.395652970731205755328119266778231, -73.395652970731205755328119266778231, -54.47624992093420104311007889918983, -54.476249920934193937682721298187971, -28.986207146850503590940206777304411, -28.986207146850492932799170375801623};
static constexpr double globalTranstrans2[36] = {1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5, 1197.5, -1197.5};
static constexpr double globalTransrot0[36] = {6.123233995736766035868820147291983e-17, -1.8369701987210296875011296034045003e-16, 0.34202014332566882393038554255326744, -0.34202014332566893495268800506892148, 0.64278760968653936291872241781675257, -0.64278760968653947394102488033240661, 0.86602540378443870761060452423407696, -0.866025403784438818632906986749731, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.86602540378443870761060452423407696, -0.86602540378443859658830206171842292, 0.64278760968653936291872241781675257, -0.64278760968653925189641995530109853, 0.34202014332566882393038554255326744, -0.3420201433256687129080830800376134, 6.123233995736766035868820147291983e-17, 6.123233995736766035868820147291983e-17, -0.3420201433256687129080830800376134, 0.34202014332566882393038554255326744, -0.64278760968653936291872241781675257, 0.64278760968653947394102488033240661, -0.86602540378443870761060452423407696, 0.866025403784438818632906986749731, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, -0.86602540378443870761060452423407696, 0.86602540378443859658830206171842292, -0.64278760968653936291872241781675257, 0.64278760968653925189641995530109853, -0.3420201433256687129080830800376134, 0.34202014332566860188578061752195936};
static constexpr double globalTransrot1[36] = {1, -1, 0.93969262078590831688273965482949279, -0.93969262078590831688273965482949279, 0.7660444431189780134516809084743727, -0.76604444311897790242937844595871866, 0.49999999999999994448884876874217298, -0.49999999999999983346654630622651894, 0.1736481776669303311866343619840336, -0.17364817766693022016433189946837956, -0.1736481776669303311866343619840336, 0.17364817766693044220893682449968765, -0.49999999999999994448884876874217298, 0.5, -0.7660444431189780134516809084743727, 0.76604444311897812447398337099002674, -0.93969262078590831688273965482949279, 0.93969262078590831688273965482949279, -1, 1, -0.93969262078590842790504211734514683, 0.93969262078590842790504211734514683, -0.7660444431189780134516809084743727, 0.76604444311897790242937844595871866, -0.49999999999999994448884876874217298, 0.49999999999999983346654630622651894, -0.17364817766693027567548313072620658, 0.17364817766693016465318066821055254, 0.17364817766693027567548313072620658, -0.17364817766693038669778559324186062, 0.49999999999999994448884876874217298, -0.5, 0.7660444431189780134516809084743727, -0.76604444311897812447398337099002674, 0.93969262078590842790504211734514683, -0.93969262078590842790504211734514683};
static constexpr double globalTransrot3[36] = {-1, -1, -0.93969262078590831688273965482949279, -0.93969262078590831688273965482949279, -0.7660444431189780134516809084743727, -0.76604444311897790242937844595871866, -0.49999999999999994448884876874217298, -0.49999999999999983346654630622651894, -0.1736481776669303311866343619840336, -0.17364817766693022016433189946837956, 0.1736481776669303311866343619840336, 0.17364817766693044220893682449968765, 0.49999999999999994448884876874217298, 0.5, 0.7660444431189780134516809084743727, 0.76604444311897812447398337099002674, 0.93969262078590831688273965482949279, 0.93969262078590831688273965482949279, 1, 1, 0.93969262078590842790504211734514683, 0.93969262078590842790504211734514683, 0.7660444431189780134516809084743727, 0.76604444311897790242937844595871866, 0.49999999999999994448884876874217298, 0.49999999999999983346654630622651894, 0.17364817766693027567548313072620658, 0.17364817766693016465318066821055254, -0.17364817766693027567548313072620658, -0.17364817766693038669778559324186062, -0.49999999999999994448884876874217298, -0.5, -0.7660444431189780134516809084743727, -0.76604444311897812447398337099002674, -0.93969262078590842790504211734514683, -0.93969262078590842790504211734514683};
static constexpr double globalTransrot4[36] = {6.123233995736766035868820147291983e-17, 1.8369701987210296875011296034045003e-16, 0.34202014332566882393038554255326744, 0.34202014332566893495268800506892148, 0.64278760968653936291872241781675257, 0.64278760968653947394102488033240661, 0.86602540378443870761060452423407696, 0.866025403784438818632906986749731, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.98480775301220802031565426659653895, 0.86602540378443870761060452423407696, 0.86602540378443859658830206171842292, 0.64278760968653936291872241781675257, 0.64278760968653925189641995530109853, 0.34202014332566882393038554255326744, 0.3420201433256687129080830800376134, 6.123233995736766035868820147291983e-17, -6.1232339957367635706784913316300911e-17, -0.3420201433256687129080830800376134, -0.34202014332566882393038554255326744, -0.64278760968653936291872241781675257, -0.64278760968653947394102488033240661, -0.86602540378443870761060452423407696, -0.866025403784438818632906986749731, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.98480775301220802031565426659653895, -0.86602540378443870761060452423407696, -0.86602540378443859658830206171842292, -0.64278760968653936291872241781675257, -0.64278760968653925189641995530109853, -0.3420201433256687129080830800376134, -0.34202014332566860188578061752195936};
static constexpr double globalTransrot8[36] = {1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1, 1, -1};
static constexpr short deltamatrixmapping[36][5] = {{0,14,-1,2,-1},{0,14,-1,2,-1},{-1,1,0,2,-1},{-1,1,0,2,-1},{-1,5,0,2,-1},{-1,5,0,2,-1},{-1,11,0,2,-1},{-1,11,0,2,-1},{-1,15,0,2,-1},{-1,15,0,2,-1},{-1,12,0,2,-1},{-1,12,0,2,-1},{-1,6,0,2,-1},{-1,6,0,2,-1},{-1,7,0,2,-1},{-1,7,0,2,-1},{-1,3,0,2,-1},{-1,3,0,2,-1},{-1,13,0,2,-1},{-1,13,0,2,-1},{-1,4,0,2,-1},{-1,4,0,2,-1},{-1,8,0,2,-1},{-1,8,0,2,-1},{-1,17,0,2,-1},{-1,17,0,2,-1},{-1,9,0,2,-1},{-1,9,0,2,-1},{-1,19,0,2,-1},{-1,19,0,2,-1},{-1,18,0,2,-1},{-1,18,0,2,-1},{-1,10,0,2,-1},{-1,10,0,2,-1},{-1,16,0,-1,2},{-1,16,0,-1,2}};
static constexpr double gDeltaTrans0[20] = {28.986207146850503590940206777304411, 0, -28.986207146850446747521345969289541, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static constexpr double gDeltaTrans1[20] = {-5.1110503883942897118686232715845108, -84.749999999999985789145284797996283, -5.1110503883943323444327688775956631, -84.749999999999985789145284797996283, -84.749999999999971578290569595992565, -84.749999999999687361196265555918217, -84.75000000000005684341886080801487, -84.750000000000127897692436818033457, -84.750000000000312638803734444081783, -84.750000000000184741111297626048326, -84.749999999999872102307563181966543, -84.750000000000028421709430404007435, -84.750000000000014210854715202003717, -84.75, -84.749999999999701572050980757921934, -84.749999999999786837179271969944239, -84.749999999999772626324556767940521, -84.750000000000028421709430404007435, -84.750000000000028421709430404007435, -84.750000000000014210854715202003717};
static constexpr double gDeltaRot0[20] = {0.93969262078590842790504211734514683, 0.34202014332566887944153677381109446, 0.93969262078590842790504211734514683, 0.34202014332566887944153677381109446, -0.3420201433256687129080830800376134, 0.64278760968653936291872241781675257, 0.86602540378443892965520944926538505, 0.64278760968653936291872241781675257, -0.64278760968653936291872241781675257, -0.98480775301220824236025919162784703, -0.64278760968653936291872241781675257, 0.86602540378443870761060452423407696, 0.98480775301220824236025919162784703, 0, 0, 0.98480775301220824236025919162784703, -0.3420201433256687129080830800376134, -0.86602540378443870761060452423407696, -0.86602540378443870761060452423407696, -0.98480775301220824236025919162784703};
static constexpr double gDeltaRot1[20] = {0.3420201433256687129080830800376134, -0.93969262078590853892734457986080088, -0.34202014332566865739693184877978638, 0.93969262078590853892734457986080088, 0.93969262078590842790504211734514683, -0.7660444431189780134516809084743727, 0.5, 0.7660444431189780134516809084743727, 0.7660444431189780134516809084743727, 0.17364817766693030343105874635512009, -0.7660444431189780134516809084743727, -0.49999999999999994448884876874217298, 0.17364817766693035894220997761294711, 1, -1, -0.17364817766693035894220997761294711, -0.93969262078590842790504211734514683, 0.49999999999999994448884876874217298, -0.49999999999999994448884876874217298, -0.17364817766693030343105874635512009};
static constexpr double gDeltaRot3[20] = {-0.3420201433256687129080830800376134, 0.93969262078590853892734457986080088, 0.34202014332566865739693184877978638, -0.93969262078590853892734457986080088, -0.93969262078590842790504211734514683, 0.7660444431189780134516809084743727, -0.5, -0.7660444431189780134516809084743727, -0.7660444431189780134516809084743727, -0.17364817766693030343105874635512009, 0.7660444431189780134516809084743727, 0.49999999999999994448884876874217298, -0.17364817766693035894220997761294711, -1, 1, 0.17364817766693035894220997761294711, 0.93969262078590842790504211734514683, -0.49999999999999994448884876874217298, 0.49999999999999994448884876874217298, 0.17364817766693030343105874635512009};
static constexpr double gDeltaRot4[20] = {0.93969262078590842790504211734514683, 0.34202014332566887944153677381109446, 0.93969262078590842790504211734514683, 0.34202014332566887944153677381109446, -0.3420201433256687129080830800376134, 0.64278760968653936291872241781675257, 0.86602540378443892965520944926538505, 0.64278760968653936291872241781675257, -0.64278760968653936291872241781675257, -0.98480775301220824236025919162784703, -0.64278760968653936291872241781675257, 0.86602540378443870761060452423407696, 0.98480775301220824236025919162784703, 0, 0, 0.98480775301220824236025919162784703, -0.3420201433256687129080830800376134, -0.86602540378443870761060452423407696, -0.86602540378443870761060452423407696, -0.98480775301220824236025919162784703};
// **** HEY: THESE VALUES ARE ALL THE SAME ---> can safe memory
static constexpr double gDeltaRot8[20] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

public:
static constexpr const char *gClassNameString="HVQXNavigator";
typedef SimpleSafetyEstimator SafetyEstimator_t;

static VNavigator *Instance(){
static HVQXNavigator instance;
return &instance;}

template <typename T>
 VECGEOM_INLINE
 static void DoGlobalToLocalTransformation(NavigationState const &in_state,Vector3D<T> const &globalpoint, Vector3D<T> const &globaldir,Vector3D<T> &localpoint, Vector3D<T> &localdir, NavigationState * internal)  {
auto index = PathToIndex( &in_state );
// caching this index in internal navigationstate for later reuse
// we know that is safe to do this because of static analysis (never do this in user code)
internal->SetCacheValue(index);
Vector3D<Precision> tmp( globalpoint[0]- globalTranstrans0[index]
, globalpoint[1]- globalTranstrans1[index]
, globalpoint[2]- globalTranstrans2[index]
);
localpoint[0]=tmp[0] * globalTransrot0[index];
localpoint[1]=tmp[0] * globalTransrot1[index];
localpoint[0]+=tmp[1] * globalTransrot3[index];
localpoint[1]+=tmp[1] * globalTransrot4[index];
localpoint[2]=tmp[2] * globalTransrot8[index];
localdir[0]=globaldir[0] * globalTransrot0[index];
localdir[1]=globaldir[0] * globalTransrot1[index];
localdir[0]+=globaldir[1] * globalTransrot3[index];
localdir[1]+=globaldir[1] * globalTransrot4[index];
localdir[2]=globaldir[2] * globalTransrot8[index];
}
template <typename T, unsigned int ChunkSize>
VECGEOM_INLINE
 static void DoGlobalToLocalTransformations(NavigationState const ** in_states,SOA3D<Precision> const &globalpoints,SOA3D<Precision> const &globaldirs, unsigned int from_index,Vector3D<T> &localpoint, Vector3D<T> &localdir, NavigationState ** internal) {
T globalTranstrans0_v;
T globalTranstrans1_v;
T globalTranstrans2_v;
T globalTransrot0_v;
T globalTransrot1_v;
T globalTransrot3_v;
T globalTransrot4_v;
T globalTransrot8_v;
// filling the vectors from the tabulated data 
// TODO: index independent data should come first (outside the loop)
for(size_t i=0;i<ChunkSize;++i){
auto trackindex = from_index + i;
auto index = PathToIndex( in_states[trackindex] );
// caching this index in internal navigationstate for later reuse
// we know that is safe to do this because of static analysis (never do this in user code)
internal[trackindex]->SetCacheValue(index);
globalTranstrans0_v[i] = globalTranstrans0[index];
globalTranstrans1_v[i] = globalTranstrans1[index];
globalTranstrans2_v[i] = globalTranstrans2[index];
globalTransrot0_v[i] = globalTransrot0[index];
globalTransrot1_v[i] = globalTransrot1[index];
globalTransrot3_v[i] = globalTransrot3[index];
globalTransrot4_v[i] = globalTransrot4[index];
globalTransrot8_v[i] = globalTransrot8[index];
}
Vector3D<T> gpoint_v(T(globalpoints.x()+from_index),T(globalpoints.y()+from_index),T(globalpoints.z()+from_index));
Vector3D<T> gdir_v(T(globaldirs.x()+from_index),T(globaldirs.y()+from_index),T(globaldirs.z()+from_index));
Vector3D<T> tmp_v( gpoint_v.x()- globalTranstrans0_v
, gpoint_v.y()- globalTranstrans1_v
, gpoint_v.z()- globalTranstrans2_v
);
localpoint[0]=tmp_v[0] * globalTransrot0_v;
localpoint[1]=tmp_v[0] * globalTransrot1_v;
localpoint[0]+=tmp_v[1] * globalTransrot3_v;
localpoint[1]+=tmp_v[1] * globalTransrot4_v;
localpoint[2]=tmp_v[2] * globalTransrot8_v;
localdir[0]=gdir_v[0] * globalTransrot0_v;
localdir[1]=gdir_v[0] * globalTransrot1_v;
localdir[0]+=gdir_v[1] * globalTransrot3_v;
localdir[1]+=gdir_v[1] * globalTransrot4_v;
localdir[2]=gdir_v[2] * globalTransrot8_v;
}
template <typename T> VECGEOM_INLINE static T TreatDistanceToMother(VPlacedVolume const *pvol, Vector3D<T> const &localpoint, Vector3D<T> const &localdir, T step_limit) {
T step;
assert(pvol != nullptr && "currentvolume is null in navigation");
step = ((SpecializedTrapezoid<-1,-1>*)pvol)->SpecializedTrapezoid<-1,-1>::DistanceToOut(localpoint, localdir, step_limit);
MaskedAssign(step < T(0.), kInfinity, &step);
return step;
}
VECGEOM_INLINE static Precision PrepareOutState(NavigationState const &in_state, NavigationState &out_state, Precision geom_step, Precision step_limit, VPlacedVolume const *hitcandidate, bool &done){
// this is the special part ( fast navigation state copying since we know the depth at compile time )
in_state.CopyToFixedSize<NavigationState::SizeOf(4)>(&out_state);
// this is just the first try -- we should factor out the following part which is probably not 
// special code
done = false;
if (geom_step == kInfinity && step_limit > 0.) {geom_step = vecgeom::kTolerance;out_state.SetBoundaryState(true);out_state.Pop();done=true;return geom_step;}
// is geometry further away than physics step?
// this is a physics step
if (geom_step > step_limit) {// don't need to do anything
geom_step = step_limit;out_state.SetBoundaryState(false);return geom_step;}
out_state.SetBoundaryState(true);
if (geom_step < 0.) {// InspectEnvironmentForPointAndDirection( globalpoint, globaldir, currentstate );
geom_step = 0.;}return geom_step;}
virtual Precision ComputeStepAndHittingBoundaryForLocalPoint(Vector3D<Precision> const & localpoint, Vector3D<Precision> const & localdir, Precision pstep, NavigationState const & in_state, NavigationState & out_state) const override {
 assert(false && "reached unimplemented point");
 return -1;
}
virtual bool CheckDaughterIntersections(LogicalVolume const *lvol, Vector3D<Precision> const & localpoint, Vector3D<Precision> const & localdir,                                           NavigationState const & in_state, NavigationState & out_state, Precision &step, VPlacedVolume const *& hitcandidate) const override {
return false;
}
VECGEOM_INLINE
virtual void Relocate(Vector3D<Precision> const &pointafterboundary, NavigationState const &__restrict__ in_state,NavigationState &__restrict__ out_state) const override {
// this means that we are leaving the mother
// alternatively we could use nextvolumeindex like before
// this was probably calculated before 
auto pathindex = out_state.GetCacheValue();
if(pathindex < 0){ pathindex = PathToIndex(&in_state);
 }{
// considering transition /horiz/-1
short index = deltamatrixmapping[pathindex][2];
if(index!=-1){
Vector3D<Precision> tmp( pointafterboundary[0]- gDeltaTrans0[index], pointafterboundary[1]- gDeltaTrans1[index], pointafterboundary[2]);
Vector3D<Precision> localpoint;
localpoint[0]=tmp[0] * gDeltaRot0[index];
localpoint[1]=tmp[0] * gDeltaRot1[index];
localpoint[0]+=tmp[1] * gDeltaRot3[index];
localpoint[1]+=tmp[1] * gDeltaRot4[index];
localpoint[2]=tmp[2];
VPlacedVolume const * pvol = &GeoManager::gCompactPlacedVolBuffer[18875];
bool intarget = ((SpecializedTrapezoid<-1,-1>const *) pvol)->SpecializedTrapezoid<-1,-1>::UnplacedContains(localpoint);
if(intarget){
auto oldvalue = out_state.ValueAt( out_state.GetCurrentLevel()-1 );
out_state.Pop();
out_state.PushIndexType( oldvalue  + -1 );
return;}
}
}
{
// considering transition /horiz/1
short index = deltamatrixmapping[pathindex][3];
if(index!=-1){
Vector3D<Precision> tmp( pointafterboundary[0]- gDeltaTrans0[index], pointafterboundary[1]- gDeltaTrans1[index], pointafterboundary[2]);
Vector3D<Precision> localpoint;
localpoint[0]=tmp[0] * gDeltaRot0[index];
localpoint[1]=tmp[0] * gDeltaRot1[index];
localpoint[0]+=tmp[1] * gDeltaRot3[index];
localpoint[1]+=tmp[1] * gDeltaRot4[index];
localpoint[2]=tmp[2];
VPlacedVolume const * pvol = &GeoManager::gCompactPlacedVolBuffer[18878];
bool intarget = ((SpecializedTrapezoid<-1,-1>const *) pvol)->SpecializedTrapezoid<-1,-1>::UnplacedContains(localpoint);
if(intarget){
auto oldvalue = out_state.ValueAt( out_state.GetCurrentLevel()-1 );
out_state.Pop();
out_state.PushIndexType( oldvalue  + 1 );
return;}
}
}
{
// considering transition /horiz/17
short index = deltamatrixmapping[pathindex][0];
if(index!=-1){
Vector3D<Precision> tmp( pointafterboundary[0]- gDeltaTrans0[index], pointafterboundary[1]- gDeltaTrans1[index], pointafterboundary[2]);
Vector3D<Precision> localpoint;
localpoint[0]=tmp[0] * gDeltaRot0[index];
localpoint[1]=tmp[0] * gDeltaRot1[index];
localpoint[0]+=tmp[1] * gDeltaRot3[index];
localpoint[1]+=tmp[1] * gDeltaRot4[index];
localpoint[2]=tmp[2];
VPlacedVolume const * pvol = &GeoManager::gCompactPlacedVolBuffer[18891];
bool intarget = ((SpecializedTrapezoid<-1,-1>const *) pvol)->SpecializedTrapezoid<-1,-1>::UnplacedContains(localpoint);
if(intarget){
auto oldvalue = out_state.ValueAt( out_state.GetCurrentLevel()-1 );
out_state.Pop();
out_state.PushIndexType( oldvalue  + 17 );
return;}
}
}
{
// considering transition /horiz/-17
short index = deltamatrixmapping[pathindex][4];
if(index!=-1){
Vector3D<Precision> tmp( pointafterboundary[0]- gDeltaTrans0[index], pointafterboundary[1]- gDeltaTrans1[index], pointafterboundary[2]);
Vector3D<Precision> localpoint;
localpoint[0]=tmp[0] * gDeltaRot0[index];
localpoint[1]=tmp[0] * gDeltaRot1[index];
localpoint[0]+=tmp[1] * gDeltaRot3[index];
localpoint[1]+=tmp[1] * gDeltaRot4[index];
localpoint[2]=tmp[2];
VPlacedVolume const * pvol = &GeoManager::gCompactPlacedVolBuffer[18874];
bool intarget = ((SpecializedTrapezoid<-1,-1>const *) pvol)->SpecializedTrapezoid<-1,-1>::UnplacedContains(localpoint);
if(intarget){
auto oldvalue = out_state.ValueAt( out_state.GetCurrentLevel()-1 );
out_state.Pop();
out_state.PushIndexType( oldvalue  + -17 );
return;}
}
}
{
// considering transition /up
short index = deltamatrixmapping[pathindex][1];
if(index!=-1){
Vector3D<Precision> tmp( pointafterboundary[0]- gDeltaTrans0[index], pointafterboundary[1]- gDeltaTrans1[index], pointafterboundary[2]);
Vector3D<Precision> localpoint;
localpoint[0]=tmp[0] * gDeltaRot0[index];
localpoint[1]=tmp[0] * gDeltaRot1[index];
localpoint[0]+=tmp[1] * gDeltaRot3[index];
localpoint[1]+=tmp[1] * gDeltaRot4[index];
localpoint[2]=tmp[2];
VPlacedVolume const * pvol = &GeoManager::gCompactPlacedVolBuffer[18873];
bool intarget = ((SpecializedTube<-1,-1,TubeTypes::UniversalTube>const *) pvol)->SpecializedTube<-1,-1,TubeTypes::UniversalTube>::UnplacedContains(localpoint);
if(intarget){
out_state.Pop();
return;}
}
}
}
}; // end class

constexpr double HVQXNavigator::globalTranstrans0[];
constexpr double HVQXNavigator::globalTranstrans1[];
constexpr double HVQXNavigator::globalTranstrans2[];
constexpr double HVQXNavigator::globalTransrot0[];
constexpr double HVQXNavigator::globalTransrot1[];
constexpr double HVQXNavigator::globalTransrot3[];
constexpr double HVQXNavigator::globalTransrot4[];
constexpr double HVQXNavigator::globalTransrot8[];
constexpr short HVQXNavigator::deltamatrixmapping[36][5];
constexpr double HVQXNavigator::gDeltaTrans0[20];
constexpr double HVQXNavigator::gDeltaTrans1[20];
constexpr double HVQXNavigator::gDeltaRot0[20];
constexpr double HVQXNavigator::gDeltaRot1[20];
constexpr double HVQXNavigator::gDeltaRot3[20];
constexpr double HVQXNavigator::gDeltaRot4[20];
constexpr double HVQXNavigator::gDeltaRot8[20];

}} // end namespace
