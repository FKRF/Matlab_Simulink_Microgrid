#include "__cf_MR_CIGRE_RESISTIVO.h"
#include <math.h>
#include "MR_CIGRE_RESISTIVO_acc.h"
#include "MR_CIGRE_RESISTIVO_acc_private.h"
#include <stdio.h>
#include "slexec_vm_simstruct_bridge.h"
#include "slexec_vm_zc_functions.h"
#include "slexec_vm_lookup_functions.h"
#include "slsv_diagnostic_codegen_c_api.h"
#include "simstruc.h"
#include "fixedpoint.h"
#define CodeFormat S-Function
#define AccDefine1 Accelerator_S-Function
#include "simtarget/slAccSfcnBridge.h"
#ifndef __RTW_UTFREE__  
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T *
bufSzPtr , int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T
tMinusDelay , real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr ,
boolean_T isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr )
{ int_T testIdx ; int_T tail = * tailPtr ; int_T bufSz = * bufSzPtr ; real_T
* tBuf = * tBufPtr ; real_T * xBuf = ( NULL ) ; int_T numBuffer = 2 ; if (
istransportdelay ) { numBuffer = 3 ; xBuf = * xBufPtr ; } testIdx = ( tail <
( bufSz - 1 ) ) ? ( tail + 1 ) : 0 ; if ( ( tMinusDelay <= tBuf [ testIdx ] )
&& ! isfixedbuf ) { int_T j ; real_T * tempT ; real_T * tempU ; real_T *
tempX = ( NULL ) ; real_T * uBuf = * uBufPtr ; int_T newBufSz = bufSz + 1024
; if ( newBufSz > * maxNewBufSzPtr ) { * maxNewBufSzPtr = newBufSz ; } tempU
= ( real_T * ) utMalloc ( numBuffer * newBufSz * sizeof ( real_T ) ) ; if (
tempU == ( NULL ) ) { return ( false ) ; } tempT = tempU + newBufSz ; if (
istransportdelay ) tempX = tempT + newBufSz ; for ( j = tail ; j < bufSz ; j
++ ) { tempT [ j - tail ] = tBuf [ j ] ; tempU [ j - tail ] = uBuf [ j ] ; if
( istransportdelay ) tempX [ j - tail ] = xBuf [ j ] ; } for ( j = 0 ; j <
tail ; j ++ ) { tempT [ j + bufSz - tail ] = tBuf [ j ] ; tempU [ j + bufSz -
tail ] = uBuf [ j ] ; if ( istransportdelay ) tempX [ j + bufSz - tail ] =
xBuf [ j ] ; } if ( * lastPtr > tail ) { * lastPtr -= tail ; } else { *
lastPtr += ( bufSz - tail ) ; } * tailPtr = 0 ; * headPtr = bufSz ; utFree (
uBuf ) ; * bufSzPtr = newBufSz ; * tBufPtr = tempT ; * uBufPtr = tempU ; if (
istransportdelay ) * xBufPtr = tempX ; } else { * tailPtr = testIdx ; }
return ( true ) ; } real_T MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
real_T tMinusDelay , real_T tStart , real_T * tBuf , real_T * uBuf , int_T
bufSz , int_T * lastIdx , int_T oldestIdx , int_T newIdx , real_T initOutput
, boolean_T discrete , boolean_T minorStepAndTAtLastMajorOutput ) { int_T i ;
real_T yout , t1 , t2 , u1 , u2 ; if ( ( newIdx == 0 ) && ( oldestIdx == 0 )
&& ( tMinusDelay > tStart ) ) return initOutput ; if ( tMinusDelay <= tStart
) return initOutput ; if ( ( tMinusDelay <= tBuf [ oldestIdx ] ) ) { if (
discrete ) { return ( uBuf [ oldestIdx ] ) ; } else { int_T tempIdx =
oldestIdx + 1 ; if ( oldestIdx == bufSz - 1 ) tempIdx = 0 ; t1 = tBuf [
oldestIdx ] ; t2 = tBuf [ tempIdx ] ; u1 = uBuf [ oldestIdx ] ; u2 = uBuf [
tempIdx ] ; if ( t2 == t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else {
yout = u1 ; } } else { real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ;
real_T f2 = 1.0 - f1 ; yout = f1 * u1 + f2 * u2 ; } return yout ; } } if (
minorStepAndTAtLastMajorOutput ) { if ( newIdx != 0 ) { if ( * lastIdx ==
newIdx ) { ( * lastIdx ) -- ; } newIdx -- ; } else { if ( * lastIdx == newIdx
) { * lastIdx = bufSz - 1 ; } newIdx = bufSz - 1 ; } } i = * lastIdx ; if (
tBuf [ i ] < tMinusDelay ) { while ( tBuf [ i ] < tMinusDelay ) { if ( i ==
newIdx ) break ; i = ( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } } else { while
( tBuf [ i ] >= tMinusDelay ) { i = ( i > 0 ) ? i - 1 : ( bufSz - 1 ) ; } i =
( i < ( bufSz - 1 ) ) ? ( i + 1 ) : 0 ; } * lastIdx = i ; if ( discrete ) {
double tempEps = ( DBL_EPSILON ) * 128.0 ; double localEps = tempEps *
muDoubleScalarAbs ( tBuf [ i ] ) ; if ( tempEps > localEps ) { localEps =
tempEps ; } localEps = localEps / 2.0 ; if ( tMinusDelay >= ( tBuf [ i ] -
localEps ) ) { yout = uBuf [ i ] ; } else { if ( i == 0 ) { yout = uBuf [
bufSz - 1 ] ; } else { yout = uBuf [ i - 1 ] ; } } } else { if ( i == 0 ) {
t1 = tBuf [ bufSz - 1 ] ; u1 = uBuf [ bufSz - 1 ] ; } else { t1 = tBuf [ i -
1 ] ; u1 = uBuf [ i - 1 ] ; } t2 = tBuf [ i ] ; u2 = uBuf [ i ] ; if ( t2 ==
t1 ) { if ( tMinusDelay >= t2 ) { yout = u2 ; } else { yout = u1 ; } } else {
real_T f1 = ( t2 - tMinusDelay ) / ( t2 - t1 ) ; real_T f2 = 1.0 - f1 ; yout
= f1 * u1 + f2 * u2 ; } } return ( yout ) ; } void rt_ssGetBlockPath (
SimStruct * S , int_T sysIdx , int_T blkIdx , char_T * * path ) {
_ssGetBlockPath ( S , sysIdx , blkIdx , path ) ; } void rt_ssSet_slErrMsg (
SimStruct * S , void * diag ) { _ssSet_slErrMsg ( S , diag ) ; } void
rt_ssReportDiagnosticAsWarning ( SimStruct * S , void * diag ) {
_ssReportDiagnosticAsWarning ( S , diag ) ; } void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Init (
SimStruct * S ) { B_MR_CIGRE_RESISTIVO_T * _rtB ; P_MR_CIGRE_RESISTIVO_T *
_rtP ; DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( ( DW_MR_CIGRE_RESISTIVO_T
* ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_MR_CIGRE_RESISTIVO_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_MR_CIGRE_RESISTIVO_T * )
_ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 0 , 14 ,
SS_CALL_MDL_INITIALIZE_CONDITIONS ) ; } void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1 ( SimStruct *
S ) { DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( ( DW_MR_CIGRE_RESISTIVO_T *
) ssGetRootDWork ( S ) ) ; ssCallAccelRunBlock ( S , 390 , _rtDW ->
Synthesized_Atomic_Subsystem_For_Alg_Loop_1_blkIdxToRun , SS_CALL_MDL_OUTPUTS
) ; } void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Update (
SimStruct * S ) { B_MR_CIGRE_RESISTIVO_T * _rtB ; P_MR_CIGRE_RESISTIVO_T *
_rtP ; DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( ( DW_MR_CIGRE_RESISTIVO_T
* ) ssGetRootDWork ( S ) ) ; _rtP = ( ( P_MR_CIGRE_RESISTIVO_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_MR_CIGRE_RESISTIVO_T * )
_ssGetModelBlockIO ( S ) ) ; ssCallAccelRunBlock ( S , 0 , 14 ,
SS_CALL_MDL_UPDATE ) ; } void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Term (
SimStruct * const S ) { } void MR_CIGRE_RESISTIVO_Subsystempi2delay_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localP ) { localB -> B_1_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_1_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_Disable (
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localDW ) { localDW ->
Subsystempi2delay_MODE = false ; } void MR_CIGRE_RESISTIVO_Subsystempi2delay
( SimStruct * S , uint8_T rtu_Enable , const real_T rtu_alpha_beta [ 2 ] ,
real_T rtu_wt , B_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localDW ) { int32_T isHit ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep (
S ) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW ->
Subsystempi2delay_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystempi2delay_MODE = true ; } } else { if ( localDW ->
Subsystempi2delay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_Disable ( localDW ) ; } } } if ( localDW
-> Subsystempi2delay_MODE ) { localB -> B_1_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarSin ( rtu_wt ) - rtu_alpha_beta [ 1 ] * muDoubleScalarCos (
rtu_wt ) ; localB -> B_1_1_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarCos (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_Subsystem1_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_T * localP ) { localB -> B_2_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_2_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystem1_Disable ( DW_Subsystem1_MR_CIGRE_RESISTIVO_T *
localDW ) { localDW -> Subsystem1_MODE = false ; } void
MR_CIGRE_RESISTIVO_Subsystem1 ( SimStruct * S , uint8_T rtu_Enable , const
real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_RESISTIVO_T * localB ,
DW_Subsystem1_MR_CIGRE_RESISTIVO_T * localDW ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW -> Subsystem1_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystem1_MODE = true ; } } else { if ( localDW -> Subsystem1_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_RESISTIVO_Subsystem1_Disable ( localDW ) ; } } } if ( localDW ->
Subsystem1_MODE ) { localB -> B_2_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarCos ( rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; localB -> B_2_1_0 = - rtu_alpha_beta [ 0 ] * muDoubleScalarSin (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void MR_CIGRE_RESISTIVO_Subsystem1_Term (
SimStruct * const S ) { } void MR_CIGRE_RESISTIVO_Subsystempi2delay_n_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_j_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_h_T * localP ) { localB -> B_9_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_9_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_j_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_k_T * localDW ) { if ( rtu_Enable > 0
) { localB -> B_9_0_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarSin ( rtu_wt ) -
rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; localB -> B_9_1_0 =
rtu_alpha_beta [ 0 ] * muDoubleScalarCos ( rtu_wt ) + rtu_alpha_beta [ 1 ] *
muDoubleScalarSin ( rtu_wt ) ; srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_e_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_Subsystem1_a_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_n_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_l_T * localP ) { localB -> B_10_0_0
= localP -> P_0 [ 0 ] ; localB -> B_10_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystem1_b ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt , B_Subsystem1_MR_CIGRE_RESISTIVO_n_T *
localB , DW_Subsystem1_MR_CIGRE_RESISTIVO_i_T * localDW ) { if ( rtu_Enable >
0 ) { localB -> B_10_0_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarCos ( rtu_wt
) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin ( rtu_wt ) ; localB -> B_10_1_0
= - rtu_alpha_beta [ 0 ] * muDoubleScalarSin ( rtu_wt ) + rtu_alpha_beta [ 1
] * muDoubleScalarCos ( rtu_wt ) ; srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } void MR_CIGRE_RESISTIVO_Subsystem1_o_Term (
SimStruct * const S ) { } void MR_CIGRE_RESISTIVO_NegSeqComputation_Disable (
DW_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ) { localDW ->
NegSeqComputation_MODE = false ; } void MR_CIGRE_RESISTIVO_NegSeqComputation
( SimStruct * S , real_T rtu_Enable , creal_T rtu_In , creal_T rtu_In_p ,
creal_T rtu_In_d , B_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localB ,
DW_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ,
P_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localP ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW ->
NegSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
NegSeqComputation_MODE = true ; } } else { if ( localDW ->
NegSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_RESISTIVO_NegSeqComputation_Disable ( localDW ) ; } } } if ( localDW
-> NegSeqComputation_MODE ) { localB -> B_22_0_0 [ 0 ] . re = localP -> P_1 [
0 ] . re * rtu_In . re - localP -> P_1 [ 0 ] . im * rtu_In . im ; localB ->
B_22_0_0 [ 0 ] . im = localP -> P_1 [ 0 ] . re * rtu_In . im + localP -> P_1
[ 0 ] . im * rtu_In . re ; localB -> B_22_0_0 [ 1 ] . re = localP -> P_1 [ 1
] . re * rtu_In_p . re - localP -> P_1 [ 1 ] . im * rtu_In_p . im ; localB ->
B_22_0_0 [ 1 ] . im = localP -> P_1 [ 1 ] . re * rtu_In_p . im + localP ->
P_1 [ 1 ] . im * rtu_In_p . re ; localB -> B_22_0_0 [ 2 ] . re = localP ->
P_1 [ 2 ] . re * rtu_In_d . re - localP -> P_1 [ 2 ] . im * rtu_In_d . im ;
localB -> B_22_0_0 [ 2 ] . im = localP -> P_1 [ 2 ] . re * rtu_In_d . im +
localP -> P_1 [ 2 ] . im * rtu_In_d . re ; localB -> B_22_1_0 . re = ( localB
-> B_22_0_0 [ 0 ] . re + localB -> B_22_0_0 [ 1 ] . re ) + localB -> B_22_0_0
[ 2 ] . re ; localB -> B_22_1_0 . im = ( localB -> B_22_0_0 [ 0 ] . im +
localB -> B_22_0_0 [ 1 ] . im ) + localB -> B_22_0_0 [ 2 ] . im ; localB ->
B_22_2_0 . re = localP -> P_0 * localB -> B_22_1_0 . re ; localB -> B_22_2_0
. im = localP -> P_0 * localB -> B_22_1_0 . im ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { srUpdateBC ( localDW -> NegSeqComputation_SubsysRanBC ) ; } } } void
MR_CIGRE_RESISTIVO_NegSeqComputation_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_ZeroSeqComputation_Disable (
DW_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ) { localDW ->
ZeroSeqComputation_MODE = false ; } void
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( SimStruct * S , real_T rtu_Enable ,
creal_T rtu_In , creal_T rtu_In_l , creal_T rtu_In_n ,
B_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localB ,
DW_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ,
P_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localP ) { int32_T isHit ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep (
S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW ->
ZeroSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
ZeroSeqComputation_MODE = true ; } } else { if ( localDW ->
ZeroSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; MR_CIGRE_RESISTIVO_ZeroSeqComputation_Disable ( localDW ) ; } } } if (
localDW -> ZeroSeqComputation_MODE ) { localB -> B_24_0_0 . re = ( rtu_In .
re + rtu_In_l . re ) + rtu_In_n . re ; localB -> B_24_0_0 . im = ( rtu_In .
im + rtu_In_l . im ) + rtu_In_n . im ; localB -> B_24_1_0 . re = localP ->
P_0 * localB -> B_24_0_0 . re ; localB -> B_24_1_0 . im = localP -> P_0 *
localB -> B_24_0_0 . im ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> ZeroSeqComputation_SubsysRanBC ) ; } } } void
MR_CIGRE_RESISTIVO_ZeroSeqComputation_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_g_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_f_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_p_T * localP ) { localB -> B_34_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_34_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_l_Disable (
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_o_T * localDW ) { localDW ->
Subsystempi2delay_MODE = false ; } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_o ( SimStruct * S , uint8_T rtu_Enable ,
real_T rtu_dq , real_T rtu_dq_j , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_f_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_o_T * localDW ) { int32_T isHit ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW ->
Subsystempi2delay_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystempi2delay_MODE = true ; } } else { if ( localDW ->
Subsystempi2delay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_l_Disable ( localDW ) ; } } } if (
localDW -> Subsystempi2delay_MODE ) { localB -> B_34_0_0 = rtu_dq *
muDoubleScalarSin ( rtu_wt ) + rtu_dq_j * muDoubleScalarCos ( rtu_wt ) ;
localB -> B_34_1_0 = - rtu_dq * muDoubleScalarCos ( rtu_wt ) + rtu_dq_j *
muDoubleScalarSin ( rtu_wt ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( localDW -> Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_p_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_Subsystem1_j_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_nf_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_n_T * localP ) { localB -> B_35_0_0
= localP -> P_0 [ 0 ] ; localB -> B_35_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystem1_c_Disable (
DW_Subsystem1_MR_CIGRE_RESISTIVO_g_T * localDW ) { localDW -> Subsystem1_MODE
= false ; } void MR_CIGRE_RESISTIVO_Subsystem1_e ( SimStruct * S , uint8_T
rtu_Enable , real_T rtu_dq , real_T rtu_dq_m , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_RESISTIVO_nf_T * localB ,
DW_Subsystem1_MR_CIGRE_RESISTIVO_g_T * localDW ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW -> Subsystem1_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystem1_MODE = true ; } } else { if ( localDW -> Subsystem1_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_RESISTIVO_Subsystem1_c_Disable ( localDW ) ; } } } if ( localDW ->
Subsystem1_MODE ) { localB -> B_35_0_0 = rtu_dq * muDoubleScalarCos ( rtu_wt
) - rtu_dq_m * muDoubleScalarSin ( rtu_wt ) ; localB -> B_35_1_0 = rtu_dq *
muDoubleScalarSin ( rtu_wt ) + rtu_dq_m * muDoubleScalarCos ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void MR_CIGRE_RESISTIVO_Subsystem1_oy_Term (
SimStruct * const S ) { } void MR_CIGRE_RESISTIVO_Subsystempi2delay_h_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_c_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_e_T * localP ) { localB -> B_351_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_351_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_l ( SimStruct * S , uint8_T rtu_Enable ,
const real_T rtu_dq [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_c_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_g_T * localDW ) { if ( rtu_Enable > 0
) { localB -> B_351_0_0 = rtu_dq [ 0 ] * muDoubleScalarSin ( rtu_wt ) +
rtu_dq [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; localB -> B_351_1_0 = - rtu_dq
[ 0 ] * muDoubleScalarCos ( rtu_wt ) + rtu_dq [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_RESISTIVO_Subsystempi2delay_n_Term ( SimStruct * const S ) { } void
MR_CIGRE_RESISTIVO_Subsystem1_e_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_g_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_lv_T * localP ) { localB ->
B_352_0_0 = localP -> P_0 [ 0 ] ; localB -> B_352_1_0 = localP -> P_0 [ 1 ] ;
} void MR_CIGRE_RESISTIVO_Subsystem1_l ( SimStruct * S , uint8_T rtu_Enable ,
const real_T rtu_dq [ 2 ] , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_RESISTIVO_g_T * localB ,
DW_Subsystem1_MR_CIGRE_RESISTIVO_m_T * localDW ) { if ( rtu_Enable > 0 ) {
localB -> B_352_0_0 = rtu_dq [ 0 ] * muDoubleScalarCos ( rtu_wt ) - rtu_dq [
1 ] * muDoubleScalarSin ( rtu_wt ) ; localB -> B_352_1_0 = rtu_dq [ 0 ] *
muDoubleScalarSin ( rtu_wt ) + rtu_dq [ 1 ] * muDoubleScalarCos ( rtu_wt ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void MR_CIGRE_RESISTIVO_Subsystem1_m_Term (
SimStruct * const S ) { } static void mdlOutputs ( SimStruct * S , int_T tid
) { creal_T B_390_275_0 ; creal_T B_390_296_0 ; creal_T B_390_317_0 ; creal_T
B_390_349_0 ; creal_T B_390_370_0 ; creal_T B_390_391_0 ; creal_T B_390_444_0
; creal_T B_390_465_0 ; creal_T B_390_486_0 ; creal_T B_390_518_0 ; creal_T
B_390_539_0 ; creal_T B_390_560_0 ; creal_T B_390_1298_0 ; creal_T
B_390_1319_0 ; creal_T B_390_1340_0 ; creal_T B_390_1371_0 ; creal_T
B_390_1392_0 ; creal_T B_390_1413_0 ; creal_T B_390_1455_0 ; creal_T
B_390_1476_0 ; creal_T B_390_1497_0 ; creal_T B_390_1735_0 ; creal_T
B_390_1756_0 ; creal_T B_390_1777_0 ; creal_T B_390_1808_0 ; creal_T
B_390_1829_0 ; creal_T B_390_1850_0 ; creal_T B_390_1889_0 ; creal_T
B_390_1910_0 ; creal_T B_390_1931_0 ; creal_T B_390_1964_0 ; creal_T
B_390_1985_0 ; creal_T B_390_2006_0 ; creal_T B_390_2187_0 ; creal_T
B_390_2208_0 ; creal_T B_390_2229_0 ; creal_T B_390_2260_0 ; creal_T
B_390_2281_0 ; creal_T B_390_2302_0 ; creal_T B_390_2341_0 ; creal_T
B_390_2362_0 ; creal_T B_390_2383_0 ; creal_T B_390_2416_0 ; creal_T
B_390_2437_0 ; creal_T B_390_2458_0 ; creal_T B_390_2587_0 ; creal_T
B_390_2608_0 ; creal_T B_390_2629_0 ; creal_T B_390_2660_0 ; creal_T
B_390_2681_0 ; creal_T B_390_2702_0 ; creal_T B_390_2741_0 ; creal_T
B_390_2762_0 ; creal_T B_390_2783_0 ; creal_T B_390_2816_0 ; creal_T
B_390_2837_0 ; creal_T B_390_2858_0 ; creal_T B_390_3039_0 ; creal_T
B_390_3060_0 ; creal_T B_390_3081_0 ; creal_T B_390_3112_0 ; creal_T
B_390_3133_0 ; creal_T B_390_3154_0 ; creal_T B_390_3193_0 ; creal_T
B_390_3214_0 ; creal_T B_390_3235_0 ; creal_T B_390_3268_0 ; creal_T
B_390_3289_0 ; creal_T B_390_3310_0 ; creal_T B_390_3491_0 ; creal_T
B_390_3512_0 ; creal_T B_390_3533_0 ; creal_T B_390_3564_0 ; creal_T
B_390_3585_0 ; creal_T B_390_3606_0 ; creal_T B_390_3645_0 ; creal_T
B_390_3666_0 ; creal_T B_390_3687_0 ; creal_T B_390_3720_0 ; creal_T
B_390_3741_0 ; creal_T B_390_3762_0 ; creal_T B_390_3945_0 ; creal_T
B_390_3966_0 ; creal_T B_390_3987_0 ; creal_T B_390_4019_0 ; creal_T
B_390_4040_0 ; creal_T B_390_4061_0 ; creal_T B_390_4479_0 ; creal_T
B_390_4500_0 ; creal_T B_390_4521_0 ; creal_T B_390_4553_0 ; creal_T
B_390_4574_0 ; creal_T B_390_4595_0 ; creal_T B_390_5011_0 ; creal_T
B_390_5032_0 ; creal_T B_390_5053_0 ; creal_T B_390_5084_0 ; creal_T
B_390_5105_0 ; creal_T B_390_5126_0 ; creal_T B_390_5165_0 ; creal_T
B_390_5186_0 ; creal_T B_390_5207_0 ; creal_T B_390_5240_0 ; creal_T
B_390_5261_0 ; creal_T B_390_5282_0 ; real_T B_390_74_0 [ 3 ] ; real_T
B_390_98_0 [ 3 ] ; real_T B_390_256_0 ; real_T B_390_264_0 ; real_T
B_390_277_0 ; real_T B_390_285_0 ; real_T B_390_298_0 ; real_T B_390_306_0 ;
real_T B_390_330_0 ; real_T B_390_338_0 ; real_T B_390_351_0 ; real_T
B_390_359_0 ; real_T B_390_372_0 ; real_T B_390_380_0 ; real_T B_390_425_0 ;
real_T B_390_433_0 ; real_T B_390_446_0 ; real_T B_390_454_0 ; real_T
B_390_467_0 ; real_T B_390_475_0 ; real_T B_390_499_0 ; real_T B_390_507_0 ;
real_T B_390_520_0 ; real_T B_390_528_0 ; real_T B_390_541_0 ; real_T
B_390_549_0 ; real_T B_390_746_0 [ 3 ] ; real_T B_390_817_0 [ 3 ] ; real_T
B_390_883_0 ; real_T B_390_896_0 ; real_T B_390_1086_0 [ 3 ] ; real_T
B_390_1156_0 [ 3 ] ; real_T B_390_1223_0 ; real_T B_390_1236_0 ; real_T
B_390_1279_0 ; real_T B_390_1287_0 ; real_T B_390_1300_0 ; real_T
B_390_1308_0 ; real_T B_390_1321_0 ; real_T B_390_1329_0 ; real_T
B_390_1352_0 ; real_T B_390_1360_0 ; real_T B_390_1373_0 ; real_T
B_390_1381_0 ; real_T B_390_1394_0 ; real_T B_390_1402_0 ; real_T
B_390_1436_0 ; real_T B_390_1444_0 ; real_T B_390_1457_0 ; real_T
B_390_1465_0 ; real_T B_390_1478_0 ; real_T B_390_1486_0 ; real_T
B_390_1588_0 ; real_T B_390_1660_0 [ 3 ] ; real_T B_390_1661_0 [ 3 ] ; real_T
B_390_1689_0 [ 3 ] ; real_T B_390_1690_0 [ 3 ] ; real_T B_390_1716_0 ; real_T
B_390_1724_0 ; real_T B_390_1737_0 ; real_T B_390_1745_0 ; real_T
B_390_1758_0 ; real_T B_390_1766_0 ; real_T B_390_1789_0 ; real_T
B_390_1797_0 ; real_T B_390_1810_0 ; real_T B_390_1818_0 ; real_T
B_390_1831_0 ; real_T B_390_1839_0 ; real_T B_390_1870_0 ; real_T
B_390_1878_0 ; real_T B_390_1891_0 ; real_T B_390_1899_0 ; real_T
B_390_1912_0 ; real_T B_390_1920_0 ; real_T B_390_1945_0 ; real_T
B_390_1953_0 ; real_T B_390_1966_0 ; real_T B_390_1974_0 ; real_T
B_390_1987_0 ; real_T B_390_1995_0 ; real_T B_390_2075_0 ; real_T
B_390_2112_0 ; real_T B_390_2168_0 ; real_T B_390_2176_0 ; real_T
B_390_2189_0 ; real_T B_390_2197_0 ; real_T B_390_2210_0 ; real_T
B_390_2218_0 ; real_T B_390_2241_0 ; real_T B_390_2249_0 ; real_T
B_390_2262_0 ; real_T B_390_2270_0 ; real_T B_390_2283_0 ; real_T
B_390_2291_0 ; real_T B_390_2322_0 ; real_T B_390_2330_0 ; real_T
B_390_2343_0 ; real_T B_390_2351_0 ; real_T B_390_2364_0 ; real_T
B_390_2372_0 ; real_T B_390_2397_0 ; real_T B_390_2405_0 ; real_T
B_390_2418_0 ; real_T B_390_2426_0 ; real_T B_390_2439_0 ; real_T
B_390_2447_0 ; real_T B_390_2525_0 ; real_T B_390_2562_0 ; real_T
B_390_2568_0 ; real_T B_390_2576_0 ; real_T B_390_2589_0 ; real_T
B_390_2597_0 ; real_T B_390_2610_0 ; real_T B_390_2618_0 ; real_T
B_390_2641_0 ; real_T B_390_2649_0 ; real_T B_390_2662_0 ; real_T
B_390_2670_0 ; real_T B_390_2683_0 ; real_T B_390_2691_0 ; real_T
B_390_2722_0 ; real_T B_390_2730_0 ; real_T B_390_2743_0 ; real_T
B_390_2751_0 ; real_T B_390_2764_0 ; real_T B_390_2772_0 ; real_T
B_390_2797_0 ; real_T B_390_2805_0 ; real_T B_390_2818_0 ; real_T
B_390_2826_0 ; real_T B_390_2839_0 ; real_T B_390_2847_0 ; real_T
B_390_2927_0 ; real_T B_390_2964_0 ; real_T B_390_3020_0 ; real_T
B_390_3028_0 ; real_T B_390_3041_0 ; real_T B_390_3049_0 ; real_T
B_390_3062_0 ; real_T B_390_3070_0 ; real_T B_390_3093_0 ; real_T
B_390_3101_0 ; real_T B_390_3114_0 ; real_T B_390_3122_0 ; real_T
B_390_3135_0 ; real_T B_390_3143_0 ; real_T B_390_3174_0 ; real_T
B_390_3182_0 ; real_T B_390_3195_0 ; real_T B_390_3203_0 ; real_T
B_390_3216_0 ; real_T B_390_3224_0 ; real_T B_390_3249_0 ; real_T
B_390_3257_0 ; real_T B_390_3270_0 ; real_T B_390_3278_0 ; real_T
B_390_3291_0 ; real_T B_390_3299_0 ; real_T B_390_3379_0 ; real_T
B_390_3416_0 ; real_T B_390_3472_0 ; real_T B_390_3480_0 ; real_T
B_390_3493_0 ; real_T B_390_3501_0 ; real_T B_390_3514_0 ; real_T
B_390_3522_0 ; real_T B_390_3545_0 ; real_T B_390_3553_0 ; real_T
B_390_3566_0 ; real_T B_390_3574_0 ; real_T B_390_3587_0 ; real_T
B_390_3595_0 ; real_T B_390_3626_0 ; real_T B_390_3634_0 ; real_T
B_390_3647_0 ; real_T B_390_3655_0 ; real_T B_390_3668_0 ; real_T
B_390_3676_0 ; real_T B_390_3701_0 ; real_T B_390_3709_0 ; real_T
B_390_3722_0 ; real_T B_390_3730_0 ; real_T B_390_3743_0 ; real_T
B_390_3751_0 ; real_T B_390_3831_0 ; real_T B_390_3868_0 ; real_T
B_390_3926_0 ; real_T B_390_3934_0 ; real_T B_390_3947_0 ; real_T
B_390_3955_0 ; real_T B_390_3968_0 ; real_T B_390_3976_0 ; real_T
B_390_4000_0 ; real_T B_390_4008_0 ; real_T B_390_4021_0 ; real_T
B_390_4029_0 ; real_T B_390_4042_0 ; real_T B_390_4050_0 ; real_T
B_390_4084_0 [ 3 ] ; real_T B_390_4121_0 [ 3 ] ; real_T B_390_4158_0 ; real_T
B_390_4170_0 [ 3 ] ; real_T B_390_4248_0 [ 3 ] ; real_T B_390_4251_0 [ 2 ] ;
real_T B_390_4256_0 [ 3 ] ; real_T B_390_4317_0 ; real_T B_390_4330_0 ;
real_T B_390_4371_0 ; real_T B_390_4423_0 ; real_T B_390_4424_0 ; real_T
B_390_4431_0 ; real_T B_390_4432_0 ; real_T B_390_4460_0 ; real_T
B_390_4468_0 ; real_T B_390_4481_0 ; real_T B_390_4489_0 ; real_T
B_390_4502_0 ; real_T B_390_4510_0 ; real_T B_390_4534_0 ; real_T
B_390_4542_0 ; real_T B_390_4555_0 ; real_T B_390_4563_0 ; real_T
B_390_4576_0 ; real_T B_390_4584_0 ; real_T B_390_4618_0 [ 3 ] ; real_T
B_390_4655_0 [ 3 ] ; real_T B_390_4692_0 ; real_T B_390_4704_0 [ 3 ] ; real_T
B_390_4782_0 [ 3 ] ; real_T B_390_4785_0 [ 2 ] ; real_T B_390_4790_0 [ 3 ] ;
real_T B_390_4851_0 ; real_T B_390_4864_0 ; real_T B_390_4905_0 ; real_T
B_390_4957_0 ; real_T B_390_4958_0 ; real_T B_390_4965_0 ; real_T
B_390_4966_0 ; real_T B_390_4992_0 ; real_T B_390_5000_0 ; real_T
B_390_5013_0 ; real_T B_390_5021_0 ; real_T B_390_5034_0 ; real_T
B_390_5042_0 ; real_T B_390_5065_0 ; real_T B_390_5073_0 ; real_T
B_390_5086_0 ; real_T B_390_5094_0 ; real_T B_390_5107_0 ; real_T
B_390_5115_0 ; real_T B_390_5146_0 ; real_T B_390_5154_0 ; real_T
B_390_5167_0 ; real_T B_390_5175_0 ; real_T B_390_5188_0 ; real_T
B_390_5196_0 ; real_T B_390_5221_0 ; real_T B_390_5229_0 ; real_T
B_390_5242_0 ; real_T B_390_5250_0 ; real_T B_390_5263_0 ; real_T
B_390_5271_0 ; real_T B_390_5350_0 ; real_T B_390_5387_0 ; real_T
B_390_5494_0 [ 3 ] ; real_T B_390_5495_0 [ 3 ] ; real_T B_366_1_0 [ 3 ] ;
real_T B_366_42_0 ; real_T B_329_1_0 [ 3 ] ; real_T B_329_42_0 ; real_T
B_46_1_0 [ 3 ] ; real_T B_46_42_0 ; real_T B_11_1_0 [ 3 ] ; real_T B_11_42_0
; real_T * lastU ; real_T rtb_B_390_1124_0 ; real_T rtb_B_390_784_0 ; real_T
rtb_B_390_4666_0 ; real_T rtb_B_390_4132_0 ; real_T rtb_B_390_422_0 ; real_T
rtb_B_390_253_0 ; real_T rtb_B_390_5447_0 ; real_T rtb_B_390_5393_0 ; real_T
rtb_B_390_32_0 [ 3 ] ; real_T rtb_B_390_3874_0 ; real_T rtb_B_390_3422_0 ;
real_T rtb_B_390_2970_0 ; real_T rtb_B_390_2118_0 ; real_T rtb_B_390_1596_0 ;
boolean_T rtb_B_390_259_0 ; real_T rtb_B_390_239_0 ; real_T rtb_B_390_222_0 ;
real_T rtb_B_390_185_0 ; real_T rtb_B_390_183_0 ; real_T rtb_B_390_16_0 ;
real_T rtb_B_390_4_1 ; real_T rtb_B_390_117_0 ; real_T tmp [ 3 ] ; real_T
B_390_401_1_idx_0 ; real_T tmp_0 ; real_T tmp_1 ; int32_T isHit ;
B_MR_CIGRE_RESISTIVO_T * _rtB ; P_MR_CIGRE_RESISTIVO_T * _rtP ;
X_MR_CIGRE_RESISTIVO_T * _rtX ; DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( (
DW_MR_CIGRE_RESISTIVO_T * ) ssGetRootDWork ( S ) ) ; _rtX = ( (
X_MR_CIGRE_RESISTIVO_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_RESISTIVO_T * ) _ssGetModelBlockIO ( S ) ) ; _rtB -> B_390_1_0 =
0.0 ; _rtB -> B_390_1_0 += _rtP -> P_412 * _rtX -> LPF2_CSTATE ; _rtB ->
B_390_2_0 = 0.0 ; _rtB -> B_390_2_0 += _rtP -> P_414 * _rtX -> LPF1_CSTATE ;
_rtB -> B_390_3_0 . re = _rtB -> B_390_1_0 ; _rtB -> B_390_3_0 . im = _rtB ->
B_390_2_0 ; _rtB -> B_390_4_0 = muDoubleScalarHypot ( _rtB -> B_390_3_0 . re
, _rtB -> B_390_3_0 . im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
B_390_3_0 . im , _rtB -> B_390_3_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Saturation_MODE = _rtB -> B_390_4_0 >= _rtP -> P_415 ? 1 : _rtB
-> B_390_4_0 > _rtP -> P_416 ? 0 : - 1 ; } _rtB -> B_390_5_0 = _rtDW ->
Saturation_MODE == 1 ? _rtP -> P_415 : _rtDW -> Saturation_MODE == - 1 ? _rtP
-> P_416 : _rtB -> B_390_4_0 ; _rtB -> B_390_7_0 = _rtB -> B_390_5_0 *
muDoubleScalarCos ( rtb_B_390_4_1 ) ; _rtB -> B_390_7_1 = _rtB -> B_390_5_0 *
muDoubleScalarSin ( rtb_B_390_4_1 ) ; isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_390_9_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE > _rtP -> P_417 ) ; if ( _rtDW ->
Initial_FirstOutputTime ) { _rtDW -> Initial_FirstOutputTime = false ; _rtB
-> B_390_11_0 = _rtP -> P_418 ; } else { _rtB -> B_390_11_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE - _rtP -> P_417 ; } if ( _rtB -> B_390_9_0 && (
_rtDW -> DiscreteTimeIntegrator_PrevResetState <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE = _rtB -> B_390_11_0 ; } _rtB -> B_390_12_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE ; } isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { rtb_B_390_16_0 = ssGetTaskTime ( S , 1 ) ; _rtDW ->
PQparaVf_MODE = ( rtb_B_390_16_0 >= _rtP -> P_422 ) ; if ( 0.0 > _rtP ->
P_425 ) { rtb_B_390_16_0 = _rtB -> B_390_13_0 ; } else if ( _rtDW ->
PQparaVf_MODE == 1 ) { rtb_B_390_16_0 = _rtP -> P_424 ; } else {
rtb_B_390_16_0 = _rtP -> P_423 ; } if ( _rtB -> B_390_14_0 > _rtP -> P_426 )
{ _rtB -> B_390_17_0 = _rtB -> B_390_13_0 ; } else { _rtB -> B_390_17_0 =
rtb_B_390_16_0 ; } } _rtB -> B_390_18_0 = _rtX -> Integrator1_CSTATE ; if (
_rtB -> B_390_17_0 > _rtP -> P_430 ) { _rtB -> B_390_19_0 = _rtB ->
B_390_12_0 ; } else { _rtB -> B_390_19_0 = _rtB -> B_390_18_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_23_0 = _rtB
-> B_390_22_0 ; } MR_CIGRE_RESISTIVO_Subsystem1_e ( S , _rtB -> B_390_23_0 ,
_rtB -> B_390_7_0 , _rtB -> B_390_7_1 , _rtB -> B_390_19_0 , & _rtB ->
Subsystem1 , & _rtDW -> Subsystem1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_390_27_0 = _rtB -> B_390_26_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay_o ( S , _rtB -> B_390_27_0 , _rtB ->
B_390_7_0 , _rtB -> B_390_7_1 , _rtB -> B_390_19_0 , & _rtB ->
Subsystempi2delay , & _rtDW -> Subsystempi2delay ) ; if ( _rtB -> B_390_22_0
!= 0 ) { tmp_0 = _rtB -> Subsystem1 . B_35_0_0 ; tmp_1 = _rtB -> Subsystem1 .
B_35_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay . B_34_0_0 ; tmp_1 =
_rtB -> Subsystempi2delay . B_34_1_0 ; } for ( isHit = 0 ; isHit < 3 ; isHit
++ ) { rtb_B_390_32_0 [ isHit ] = _rtP -> P_435 [ isHit + 6 ] * _rtB ->
B_390_30_0 + ( _rtP -> P_435 [ isHit + 3 ] * tmp_1 + _rtP -> P_435 [ isHit ]
* tmp_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_390_33_0 = _rtDW -> Vdc_DSTATE ; _rtB -> B_390_34_0 = _rtP -> P_437 *
_rtB -> B_390_33_0 ; } rtb_B_390_32_0 [ 0 ] *= _rtB -> B_390_34_0 ;
rtb_B_390_32_0 [ 1 ] *= _rtB -> B_390_34_0 ; _rtB -> B_390_36_0 =
rtb_B_390_32_0 [ 0 ] - rtb_B_390_32_0 [ 1 ] ; _rtB -> B_390_37_0 =
rtb_B_390_32_0 [ 1 ] - rtb_B_390_32_0 [ 2 ] * _rtB -> B_390_34_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_38_0 = _rtDW
-> Idc_DSTATE ; } _rtB -> B_390_39_0 = 0.0 ; _rtB -> B_390_39_0 += _rtP ->
P_440 * _rtX -> LPF2_CSTATE_e ; _rtB -> B_390_40_0 = 0.0 ; _rtB -> B_390_40_0
+= _rtP -> P_442 * _rtX -> LPF1_CSTATE_g ; _rtB -> B_390_41_0 . re = _rtB ->
B_390_39_0 ; _rtB -> B_390_41_0 . im = _rtB -> B_390_40_0 ; _rtB ->
B_390_42_0 = muDoubleScalarHypot ( _rtB -> B_390_41_0 . re , _rtB ->
B_390_41_0 . im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB -> B_390_41_0
. im , _rtB -> B_390_41_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Saturation_MODE_p = _rtB -> B_390_42_0 >= _rtP -> P_443 ? 1 : _rtB
-> B_390_42_0 > _rtP -> P_444 ? 0 : - 1 ; } _rtB -> B_390_43_0 = _rtDW ->
Saturation_MODE_p == 1 ? _rtP -> P_443 : _rtDW -> Saturation_MODE_p == - 1 ?
_rtP -> P_444 : _rtB -> B_390_42_0 ; _rtB -> B_390_45_0 = _rtB -> B_390_43_0
* muDoubleScalarCos ( rtb_B_390_4_1 ) ; _rtB -> B_390_45_1 = _rtB ->
B_390_43_0 * muDoubleScalarSin ( rtb_B_390_4_1 ) ; isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_47_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_k > _rtP -> P_445 ) ; if ( _rtDW ->
Initial_FirstOutputTime_f ) { _rtDW -> Initial_FirstOutputTime_f = false ;
_rtB -> B_390_49_0 = _rtP -> P_446 ; } else { _rtB -> B_390_49_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_k - _rtP -> P_445 ; } if ( _rtB -> B_390_47_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_n <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_k = _rtB -> B_390_49_0 ; } _rtB -> B_390_50_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE_k ; } isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_51_0 > _rtP -> P_449 ) { _rtB ->
B_390_52_0 = _rtB -> B_390_13_0 ; } else { _rtB -> B_390_52_0 =
rtb_B_390_16_0 ; } } _rtB -> B_390_53_0 = _rtX -> Integrator1_CSTATE_o ; if (
_rtB -> B_390_52_0 > _rtP -> P_453 ) { _rtB -> B_390_54_0 = _rtB ->
B_390_50_0 ; } else { _rtB -> B_390_54_0 = _rtB -> B_390_53_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_58_0 = _rtB
-> B_390_57_0 ; } MR_CIGRE_RESISTIVO_Subsystem1_e ( S , _rtB -> B_390_58_0 ,
_rtB -> B_390_45_0 , _rtB -> B_390_45_1 , _rtB -> B_390_54_0 , & _rtB ->
Subsystem1_e , & _rtDW -> Subsystem1_e ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_390_62_0 = _rtB -> B_390_61_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay_o ( S , _rtB -> B_390_62_0 , _rtB ->
B_390_45_0 , _rtB -> B_390_45_1 , _rtB -> B_390_54_0 , & _rtB ->
Subsystempi2delay_a , & _rtDW -> Subsystempi2delay_a ) ; if ( _rtB ->
B_390_57_0 != 0 ) { tmp_0 = _rtB -> Subsystem1_e . B_35_0_0 ; tmp_1 = _rtB ->
Subsystem1_e . B_35_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay_a .
B_34_0_0 ; tmp_1 = _rtB -> Subsystempi2delay_a . B_34_1_0 ; } for ( isHit = 0
; isHit < 3 ; isHit ++ ) { rtb_B_390_32_0 [ isHit ] = _rtP -> P_458 [ isHit +
6 ] * _rtB -> B_390_65_0 + ( _rtP -> P_458 [ isHit + 3 ] * tmp_1 + _rtP ->
P_458 [ isHit ] * tmp_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_390_68_0 = _rtDW -> Vdc_DSTATE_p ; _rtB ->
B_390_69_0 = _rtP -> P_460 * _rtB -> B_390_68_0 ; } rtb_B_390_32_0 [ 0 ] *=
_rtB -> B_390_69_0 ; rtb_B_390_32_0 [ 1 ] *= _rtB -> B_390_69_0 ; _rtB ->
B_390_71_0 = rtb_B_390_32_0 [ 0 ] - rtb_B_390_32_0 [ 1 ] ; _rtB -> B_390_72_0
= rtb_B_390_32_0 [ 1 ] - rtb_B_390_32_0 [ 2 ] * _rtB -> B_390_69_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_73_0 = _rtDW
-> Idc_DSTATE_b ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
B_390_74_0 [ 0 ] = _rtDW -> UnitDelay1_DSTATE [ 0 ] ; B_390_74_0 [ 1 ] =
_rtDW -> UnitDelay1_DSTATE [ 1 ] ; B_390_74_0 [ 2 ] = _rtDW ->
UnitDelay1_DSTATE [ 2 ] ; _rtB -> B_390_76_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_a > _rtP -> P_463 ) ; if ( _rtDW ->
Initial_FirstOutputTime_g ) { _rtDW -> Initial_FirstOutputTime_g = false ;
_rtB -> B_390_78_0 = _rtP -> P_464 ; } else { _rtB -> B_390_78_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_a - _rtP -> P_463 ; } if ( _rtB -> B_390_76_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_p <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_a = _rtB -> B_390_78_0 ; } _rtB -> B_390_79_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE_a ; _rtB -> B_390_83_0 = _rtB ->
B_390_82_0 ; MR_CIGRE_RESISTIVO_Subsystem1_l ( S , _rtB -> B_390_83_0 , &
B_390_74_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB -> Subsystem1_l , & _rtDW ->
Subsystem1_l ) ; _rtB -> B_390_87_0 = _rtB -> B_390_86_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_l ( S , _rtB -> B_390_87_0 , &
B_390_74_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB -> Subsystempi2delay_l , &
_rtDW -> Subsystempi2delay_l ) ; if ( _rtB -> B_390_82_0 != 0 ) { tmp_0 =
_rtB -> Subsystem1_l . B_352_0_0 ; tmp_1 = _rtB -> Subsystem1_l . B_352_1_0 ;
} else { tmp_0 = _rtB -> Subsystempi2delay_l . B_351_0_0 ; tmp_1 = _rtB ->
Subsystempi2delay_l . B_351_1_0 ; } for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ _rtB -> B_390_91_0 [ isHit ] = 0.0 ; _rtB -> B_390_91_0 [ isHit ] += _rtP
-> P_469 [ isHit ] * tmp_0 ; _rtB -> B_390_91_0 [ isHit ] += _rtP -> P_469 [
isHit + 3 ] * tmp_1 ; _rtB -> B_390_91_0 [ isHit ] += _rtP -> P_469 [ isHit +
6 ] * B_390_74_0 [ 2 ] ; } } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_390_92_0 = _rtDW -> Vdc_DSTATE_g ; rtb_B_390_16_0 = _rtP
-> P_471 * _rtB -> B_390_92_0 ; rtb_B_390_32_0 [ 1 ] = _rtB -> B_390_91_0 [ 1
] * rtb_B_390_16_0 ; _rtB -> B_390_95_0 = _rtB -> B_390_91_0 [ 0 ] *
rtb_B_390_16_0 - rtb_B_390_32_0 [ 1 ] ; _rtB -> B_390_96_0 = rtb_B_390_32_0 [
1 ] - _rtB -> B_390_91_0 [ 2 ] * rtb_B_390_16_0 ; _rtB -> B_390_97_0 = _rtDW
-> Idc_DSTATE_h ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
B_390_98_0 [ 0 ] = _rtDW -> UnitDelay1_DSTATE_e [ 0 ] ; B_390_98_0 [ 1 ] =
_rtDW -> UnitDelay1_DSTATE_e [ 1 ] ; B_390_98_0 [ 2 ] = _rtDW ->
UnitDelay1_DSTATE_e [ 2 ] ; _rtB -> B_390_100_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq > _rtP -> P_474 ) ; if ( _rtDW ->
Initial_FirstOutputTime_o ) { _rtDW -> Initial_FirstOutputTime_o = false ;
_rtB -> B_390_102_0 = _rtP -> P_475 ; } else { _rtB -> B_390_102_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq - _rtP -> P_474 ; } if ( _rtB -> B_390_100_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_d <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq = _rtB -> B_390_102_0 ; } _rtB ->
B_390_103_0 = _rtDW -> DiscreteTimeIntegrator_DSTATE_aq ; _rtB -> B_390_107_0
= _rtB -> B_390_106_0 ; MR_CIGRE_RESISTIVO_Subsystem1_l ( S , _rtB ->
B_390_107_0 , & B_390_98_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB ->
Subsystem1_m , & _rtDW -> Subsystem1_m ) ; _rtB -> B_390_111_0 = _rtB ->
B_390_110_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_l ( S , _rtB ->
B_390_111_0 , & B_390_98_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB ->
Subsystempi2delay_l4 , & _rtDW -> Subsystempi2delay_l4 ) ; if ( _rtB ->
B_390_106_0 != 0 ) { tmp_0 = _rtB -> Subsystem1_m . B_352_0_0 ; tmp_1 = _rtB
-> Subsystem1_m . B_352_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay_l4 .
B_351_0_0 ; tmp_1 = _rtB -> Subsystempi2delay_l4 . B_351_1_0 ; } for ( isHit
= 0 ; isHit < 3 ; isHit ++ ) { _rtB -> B_390_115_0 [ isHit ] = 0.0 ; _rtB ->
B_390_115_0 [ isHit ] += _rtP -> P_480 [ isHit ] * tmp_0 ; _rtB ->
B_390_115_0 [ isHit ] += _rtP -> P_480 [ isHit + 3 ] * tmp_1 ; _rtB ->
B_390_115_0 [ isHit ] += _rtP -> P_480 [ isHit + 6 ] * B_390_98_0 [ 2 ] ; } }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_116_0
= _rtDW -> Vdc_DSTATE_b ; rtb_B_390_117_0 = _rtP -> P_482 * _rtB ->
B_390_116_0 ; rtb_B_390_32_0 [ 1 ] = _rtB -> B_390_115_0 [ 1 ] *
rtb_B_390_117_0 ; _rtB -> B_390_119_0 = _rtB -> B_390_115_0 [ 0 ] *
rtb_B_390_117_0 - rtb_B_390_32_0 [ 1 ] ; _rtB -> B_390_120_0 = rtb_B_390_32_0
[ 1 ] - _rtB -> B_390_115_0 [ 2 ] * rtb_B_390_117_0 ; _rtB -> B_390_121_0 =
_rtDW -> Idc_DSTATE_k ; if ( _rtB -> B_390_124_0 > _rtP -> P_487 ) {
rtb_B_390_117_0 = _rtP -> P_487 ; } else if ( _rtB -> B_390_124_0 < _rtP ->
P_488 ) { rtb_B_390_117_0 = _rtP -> P_488 ; } else { rtb_B_390_117_0 = _rtB
-> B_390_124_0 ; } _rtB -> B_390_132_0 [ 0 ] = rtb_B_390_117_0 * _rtB ->
B_390_131_0 ; _rtB -> B_390_132_0 [ 1 ] = ( _rtB -> B_390_127_0 -
rtb_B_390_117_0 ) * _rtB -> B_390_131_0 ; _rtB -> B_390_133_0 = _rtDW ->
UnitDelay5_DSTATE ; _rtB -> B_390_136_0 = _rtP -> P_492 * _rtB -> B_390_133_0
- _rtDW -> UnitDelay6_DSTATE ; _rtB -> B_390_137_0 [ 0 ] = _rtDW ->
UnitDelay1_DSTATE_p [ 0 ] ; _rtB -> B_390_137_0 [ 1 ] = _rtDW ->
UnitDelay1_DSTATE_p [ 1 ] ; ssCallAccelRunBlock ( S , 316 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; if ( _rtB -> B_390_139_0 > _rtP -> P_496 ) {
rtb_B_390_117_0 = _rtP -> P_496 ; } else if ( _rtB -> B_390_139_0 < _rtP ->
P_497 ) { rtb_B_390_117_0 = _rtP -> P_497 ; } else { rtb_B_390_117_0 = _rtB
-> B_390_139_0 ; } _rtB -> B_390_147_0 [ 0 ] = rtb_B_390_117_0 * _rtB ->
B_390_146_0 ; _rtB -> B_390_147_0 [ 1 ] = ( _rtB -> B_390_142_0 -
rtb_B_390_117_0 ) * _rtB -> B_390_146_0 ; _rtB -> B_390_148_0 = _rtDW ->
UnitDelay5_DSTATE_e ; _rtB -> B_390_151_0 = _rtP -> P_501 * _rtB ->
B_390_148_0 - _rtDW -> UnitDelay6_DSTATE_l ; _rtB -> B_390_152_0 [ 0 ] =
_rtDW -> UnitDelay1_DSTATE_m [ 0 ] ; _rtB -> B_390_152_0 [ 1 ] = _rtDW ->
UnitDelay1_DSTATE_m [ 1 ] ; ssCallAccelRunBlock ( S , 353 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; rtb_B_390_183_0 = _rtDW -> Delay_x_DSTATE ; _rtB ->
B_390_184_0 = _rtP -> P_522 * _rtDW -> Delay_x_DSTATE ; rtb_B_390_185_0 =
_rtDW -> Delay_x_DSTATE_h ; _rtB -> B_390_186_0 = _rtP -> P_524 * _rtDW ->
Delay_x_DSTATE_h ; rtb_B_390_117_0 = _rtDW -> Delay_x_DSTATE_e ; _rtB ->
B_390_221_0 = _rtP -> P_547 * _rtDW -> Delay_x_DSTATE_e ; rtb_B_390_222_0 =
_rtDW -> Delay_x_DSTATE_b ; _rtB -> B_390_223_0 = _rtP -> P_549 * _rtDW ->
Delay_x_DSTATE_b ; if ( _rtDW -> systemEnable != 0 ) { _rtDW -> lastSin =
muDoubleScalarSin ( _rtP -> P_556 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
lastCos = muDoubleScalarCos ( _rtP -> P_556 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> systemEnable = 0 ; } _rtB -> B_390_228_0 = ( ( _rtDW -> lastSin *
_rtP -> P_560 + _rtDW -> lastCos * _rtP -> P_559 ) * _rtP -> P_558 + ( _rtDW
-> lastCos * _rtP -> P_560 - _rtDW -> lastSin * _rtP -> P_559 ) * _rtP ->
P_557 ) * _rtP -> P_554 + _rtP -> P_555 ; if ( _rtDW -> systemEnable_f != 0 )
{ _rtDW -> lastSin_i = muDoubleScalarSin ( _rtP -> P_563 * ssGetTaskTime ( S
, 2 ) ) ; _rtDW -> lastCos_p = muDoubleScalarCos ( _rtP -> P_563 *
ssGetTaskTime ( S , 2 ) ) ; _rtDW -> systemEnable_f = 0 ; } _rtB ->
B_390_229_0 = ( ( _rtDW -> lastSin_i * _rtP -> P_567 + _rtDW -> lastCos_p *
_rtP -> P_566 ) * _rtP -> P_565 + ( _rtDW -> lastCos_p * _rtP -> P_567 -
_rtDW -> lastSin_i * _rtP -> P_566 ) * _rtP -> P_564 ) * _rtP -> P_561 + _rtP
-> P_562 ; if ( _rtDW -> systemEnable_n != 0 ) { _rtDW -> lastSin_p =
muDoubleScalarSin ( _rtP -> P_570 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
lastCos_c = muDoubleScalarCos ( _rtP -> P_570 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> systemEnable_n = 0 ; } _rtB -> B_390_230_0 = ( ( _rtDW -> lastSin_p
* _rtP -> P_574 + _rtDW -> lastCos_c * _rtP -> P_573 ) * _rtP -> P_572 + (
_rtDW -> lastCos_c * _rtP -> P_574 - _rtDW -> lastSin_p * _rtP -> P_573 ) *
_rtP -> P_571 ) * _rtP -> P_568 + _rtP -> P_569 ;
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1 ( S ) ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_390_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> ref_MODE = ( rtb_B_390_16_0 >= _rtP ->
P_579 ) ; if ( _rtDW -> ref_MODE == 1 ) { _rtB -> B_390_237_0 = _rtP -> P_581
; } else { _rtB -> B_390_237_0 = _rtP -> P_580 ; } rtb_B_390_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> Time_init_ref_MODE = ( rtb_B_390_16_0 >=
_rtP -> P_583 ) ; if ( _rtDW -> Time_init_ref_MODE == 1 ) { rtb_B_390_239_0 =
_rtP -> P_585 ; } else { rtb_B_390_239_0 = _rtP -> P_584 ; } _rtB ->
B_390_240_0 = _rtB -> B_390_238_0 * rtb_B_390_239_0 ; if ( ( _rtDW ->
LastMajorTimeA >= ssGetTaskTime ( S , 1 ) ) && ( _rtDW -> LastMajorTimeB >=
ssGetTaskTime ( S , 1 ) ) ) { _rtB -> B_390_241_0 = _rtB -> B_390_240_0 ; }
else { if ( ( ( _rtDW -> LastMajorTimeA < _rtDW -> LastMajorTimeB ) && (
_rtDW -> LastMajorTimeB < ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW ->
LastMajorTimeA >= _rtDW -> LastMajorTimeB ) && ( _rtDW -> LastMajorTimeA >=
ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW ->
LastMajorTimeB ; rtb_B_390_4_1 = _rtDW -> PrevYB ; } else { tmp_0 =
ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA ; rtb_B_390_4_1 = _rtDW ->
PrevYA ; } tmp_1 = tmp_0 * _rtP -> P_586 ; rtb_B_390_16_0 = _rtB ->
B_390_240_0 - rtb_B_390_4_1 ; if ( rtb_B_390_16_0 > tmp_1 ) { _rtB ->
B_390_241_0 = rtb_B_390_4_1 + tmp_1 ; } else { tmp_0 *= _rtP -> P_587 ; if (
rtb_B_390_16_0 < tmp_0 ) { _rtB -> B_390_241_0 = rtb_B_390_4_1 + tmp_0 ; }
else { _rtB -> B_390_241_0 = _rtB -> B_390_240_0 ; } } } _rtB -> B_390_243_0
= rtb_B_390_239_0 * _rtB -> B_390_242_0 ; if ( ( _rtDW -> LastMajorTimeA_o >=
ssGetTaskTime ( S , 1 ) ) && ( _rtDW -> LastMajorTimeB_h >= ssGetTaskTime ( S
, 1 ) ) ) { _rtB -> B_390_244_0 = _rtB -> B_390_243_0 ; } else { if ( ( (
_rtDW -> LastMajorTimeA_o < _rtDW -> LastMajorTimeB_h ) && ( _rtDW ->
LastMajorTimeB_h < ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW ->
LastMajorTimeA_o >= _rtDW -> LastMajorTimeB_h ) && ( _rtDW ->
LastMajorTimeA_o >= ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime ( S
, 1 ) - _rtDW -> LastMajorTimeB_h ; rtb_B_390_4_1 = _rtDW -> PrevYB_l ; }
else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA_o ;
rtb_B_390_4_1 = _rtDW -> PrevYA_e ; } tmp_1 = tmp_0 * _rtP -> P_589 ;
rtb_B_390_16_0 = _rtB -> B_390_243_0 - rtb_B_390_4_1 ; if ( rtb_B_390_16_0 >
tmp_1 ) { _rtB -> B_390_244_0 = rtb_B_390_4_1 + tmp_1 ; } else { tmp_0 *=
_rtP -> P_590 ; if ( rtb_B_390_16_0 < tmp_0 ) { _rtB -> B_390_244_0 =
rtb_B_390_4_1 + tmp_0 ; } else { _rtB -> B_390_244_0 = _rtB -> B_390_243_0 ;
} } } } _rtB -> B_390_245_0 = 0.0 ; _rtB -> B_390_245_0 += _rtP -> P_592 *
_rtX -> LPF1_CSTATE_o ; _rtB -> B_390_246_0 = 0.0 ; _rtB -> B_390_246_0 +=
_rtP -> P_594 * _rtX -> LPF2_CSTATE_c ; isHit = ssIsSampleHit ( S , 2 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 15 , 0 , SS_CALL_MDL_OUTPUTS )
; } _rtB -> B_390_248_0 = _rtB -> B_390_246_0 - _rtB -> B_15_0_2 ; _rtB ->
B_390_249_0 = _rtP -> P_595 * _rtB -> B_390_248_0 ; _rtB -> B_390_250_0 =
_rtP -> P_596 * _rtB -> B_390_249_0 ; _rtB -> B_390_251_0 = _rtB ->
B_390_237_0 + _rtB -> B_390_250_0 ; _rtB -> B_390_252_0 = _rtP -> P_597 *
_rtB -> B_390_251_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 )
{ rtb_B_390_253_0 = _rtDW -> UnitDelay_DSTATE ; _rtB -> B_390_254_0 = _rtP ->
P_599 * _rtDW -> UnitDelay_DSTATE ; } _rtB -> B_390_255_0 = _rtX ->
integrator_CSTATE ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * *
) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_601 ; B_390_256_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail , _rtDW ->
TransportDelay_IWORK . Head , _rtP -> P_602 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_258_0 = _rtP -> P_603 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_258_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_260_0 = _rtDW -> Memory_PreviousInput ; } if ( rtb_B_390_259_0
) { _rtB -> B_390_262_0 = ( _rtB -> B_390_255_0 - B_390_256_0 ) * _rtP ->
P_63 ; } else { _rtB -> B_390_262_0 = _rtB -> B_390_260_0 ; } _rtB ->
B_390_263_0 = _rtX -> integrator_CSTATE_n ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_606 ; B_390_264_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_h . Last , _rtDW ->
TransportDelay_IWORK_h . Tail , _rtDW -> TransportDelay_IWORK_h . Head , _rtP
-> P_607 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_266_0 = _rtP -> P_608 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_266_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_268_0 = _rtDW
-> Memory_PreviousInput_a ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_270_0 =
( _rtB -> B_390_263_0 - B_390_264_0 ) * _rtP -> P_62 ; } else { _rtB ->
B_390_270_0 = _rtB -> B_390_268_0 ; } rtb_B_390_16_0 = _rtP -> P_610 *
muDoubleScalarAtan2 ( _rtB -> B_390_270_0 , _rtB -> B_390_262_0 ) * _rtP ->
P_611 ; B_390_275_0 . re = muDoubleScalarHypot ( _rtB -> B_390_262_0 , _rtB
-> B_390_270_0 ) * muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_275_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_262_0 , _rtB -> B_390_270_0 ) *
muDoubleScalarSin ( rtb_B_390_16_0 ) ; _rtB -> B_390_276_0 = _rtX ->
integrator_CSTATE_g ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_613 ; B_390_277_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_l . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l . Last , _rtDW -> TransportDelay_IWORK_l . Tail ,
_rtDW -> TransportDelay_IWORK_l . Head , _rtP -> P_614 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_279_0 = _rtP -> P_615 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_279_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_281_0 = _rtDW -> Memory_PreviousInput_av ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_283_0 = ( _rtB -> B_390_276_0 - B_390_277_0
) * _rtP -> P_65 ; } else { _rtB -> B_390_283_0 = _rtB -> B_390_281_0 ; }
_rtB -> B_390_284_0 = _rtX -> integrator_CSTATE_e ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_618 ; B_390_285_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_k .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_k . Last , _rtDW ->
TransportDelay_IWORK_k . Tail , _rtDW -> TransportDelay_IWORK_k . Head , _rtP
-> P_619 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_287_0 = _rtP -> P_620 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_287_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_289_0 = _rtDW
-> Memory_PreviousInput_e ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_291_0 =
( _rtB -> B_390_284_0 - B_390_285_0 ) * _rtP -> P_64 ; } else { _rtB ->
B_390_291_0 = _rtB -> B_390_289_0 ; } rtb_B_390_16_0 = _rtP -> P_622 *
muDoubleScalarAtan2 ( _rtB -> B_390_291_0 , _rtB -> B_390_283_0 ) * _rtP ->
P_623 ; B_390_296_0 . re = muDoubleScalarHypot ( _rtB -> B_390_283_0 , _rtB
-> B_390_291_0 ) * muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_296_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_283_0 , _rtB -> B_390_291_0 ) *
muDoubleScalarSin ( rtb_B_390_16_0 ) ; _rtB -> B_390_297_0 = _rtX ->
integrator_CSTATE_l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_o0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_625 ;
B_390_298_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_c . Last , _rtDW -> TransportDelay_IWORK_c .
Tail , _rtDW -> TransportDelay_IWORK_c . Head , _rtP -> P_626 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_300_0 = _rtP -> P_627 ; } rtb_B_390_259_0 = ( ssGetT ( S )
>= _rtB -> B_390_300_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_390_302_0 = _rtDW -> Memory_PreviousInput_d ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_304_0 = ( _rtB -> B_390_297_0 - B_390_298_0
) * _rtP -> P_67 ; } else { _rtB -> B_390_304_0 = _rtB -> B_390_302_0 ; }
_rtB -> B_390_305_0 = _rtX -> integrator_CSTATE_p ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_630 ; B_390_306_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_m . Last , _rtDW ->
TransportDelay_IWORK_m . Tail , _rtDW -> TransportDelay_IWORK_m . Head , _rtP
-> P_631 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_308_0 = _rtP -> P_632 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_308_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_310_0 = _rtDW
-> Memory_PreviousInput_ac ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_312_0 =
( _rtB -> B_390_305_0 - B_390_306_0 ) * _rtP -> P_66 ; } else { _rtB ->
B_390_312_0 = _rtB -> B_390_310_0 ; } rtb_B_390_16_0 = _rtP -> P_634 *
muDoubleScalarAtan2 ( _rtB -> B_390_312_0 , _rtB -> B_390_304_0 ) * _rtP ->
P_635 ; B_390_317_0 . re = muDoubleScalarHypot ( _rtB -> B_390_304_0 , _rtB
-> B_390_312_0 ) * muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_317_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_304_0 , _rtB -> B_390_312_0 ) *
muDoubleScalarSin ( rtb_B_390_16_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_390_319_0 = _rtB -> B_390_318_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_319_0 , B_390_275_0
, B_390_296_0 , B_390_317_0 , & _rtB -> PosSeqComputation , & _rtDW ->
PosSeqComputation , & _rtP -> PosSeqComputation ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_322_0 = _rtB -> B_390_321_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_322_0 , B_390_275_0
, B_390_296_0 , B_390_317_0 , & _rtB -> NegSeqComputation , & _rtDW ->
NegSeqComputation , & _rtP -> NegSeqComputation ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_325_0 = _rtB -> B_390_324_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_325_0 , B_390_275_0
, B_390_296_0 , B_390_317_0 , & _rtB -> ZeroSeqComputation , & _rtDW ->
ZeroSeqComputation , & _rtP -> ZeroSeqComputation ) ; rtb_B_390_32_0 [ 0 ] =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation . B_22_2_0 . im , _rtB ->
PosSeqComputation . B_22_2_0 . re ) ; _rtB -> B_390_328_0 = _rtP -> P_639 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation . B_22_2_0 . re , _rtB ->
PosSeqComputation . B_22_2_0 . im ) ; _rtB -> B_390_329_0 = _rtX ->
integrator_CSTATE_f ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_641 ; B_390_330_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_a . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a . Last , _rtDW -> TransportDelay_IWORK_a . Tail ,
_rtDW -> TransportDelay_IWORK_a . Head , _rtP -> P_642 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_332_0 = _rtP -> P_643 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_332_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_334_0 = _rtDW -> Memory_PreviousInput_am ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_336_0 = ( _rtB -> B_390_329_0 - B_390_330_0
) * _rtP -> P_69 ; } else { _rtB -> B_390_336_0 = _rtB -> B_390_334_0 ; }
_rtB -> B_390_337_0 = _rtX -> integrator_CSTATE_k ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_me . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_646 ; B_390_338_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_p . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p . Last , _rtDW -> TransportDelay_IWORK_p . Tail ,
_rtDW -> TransportDelay_IWORK_p . Head , _rtP -> P_647 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_340_0 = _rtP -> P_648 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_340_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_342_0 = _rtDW -> Memory_PreviousInput_f ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_344_0 = ( _rtB -> B_390_337_0 - B_390_338_0
) * _rtP -> P_68 ; } else { _rtB -> B_390_344_0 = _rtB -> B_390_342_0 ; }
rtb_B_390_16_0 = _rtP -> P_650 * muDoubleScalarAtan2 ( _rtB -> B_390_344_0 ,
_rtB -> B_390_336_0 ) * _rtP -> P_651 ; B_390_349_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_336_0 , _rtB -> B_390_344_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_349_0 . im = muDoubleScalarHypot
( _rtB -> B_390_336_0 , _rtB -> B_390_344_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; _rtB -> B_390_350_0 = _rtX -> integrator_CSTATE_lb ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_653 ; B_390_351_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_d . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_d . Last , _rtDW -> TransportDelay_IWORK_d . Tail ,
_rtDW -> TransportDelay_IWORK_d . Head , _rtP -> P_654 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_353_0 = _rtP -> P_655 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_353_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_355_0 = _rtDW -> Memory_PreviousInput_o ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_357_0 = ( _rtB -> B_390_350_0 - B_390_351_0
) * _rtP -> P_71 ; } else { _rtB -> B_390_357_0 = _rtB -> B_390_355_0 ; }
_rtB -> B_390_358_0 = _rtX -> integrator_CSTATE_gs ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_658 ; B_390_359_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dg
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_dg . Last , _rtDW ->
TransportDelay_IWORK_dg . Tail , _rtDW -> TransportDelay_IWORK_dg . Head ,
_rtP -> P_659 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_361_0 = _rtP -> P_660 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_361_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_363_0 = _rtDW
-> Memory_PreviousInput_l ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_365_0 =
( _rtB -> B_390_358_0 - B_390_359_0 ) * _rtP -> P_70 ; } else { _rtB ->
B_390_365_0 = _rtB -> B_390_363_0 ; } rtb_B_390_16_0 = _rtP -> P_662 *
muDoubleScalarAtan2 ( _rtB -> B_390_365_0 , _rtB -> B_390_357_0 ) * _rtP ->
P_663 ; B_390_370_0 . re = muDoubleScalarHypot ( _rtB -> B_390_357_0 , _rtB
-> B_390_365_0 ) * muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_370_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_357_0 , _rtB -> B_390_365_0 ) *
muDoubleScalarSin ( rtb_B_390_16_0 ) ; _rtB -> B_390_371_0 = _rtX ->
integrator_CSTATE_o ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_665 ; B_390_372_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_o . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o . Last , _rtDW -> TransportDelay_IWORK_o . Tail ,
_rtDW -> TransportDelay_IWORK_o . Head , _rtP -> P_666 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_374_0 = _rtP -> P_667 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_374_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_376_0 = _rtDW -> Memory_PreviousInput_j ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_378_0 = ( _rtB -> B_390_371_0 - B_390_372_0
) * _rtP -> P_73 ; } else { _rtB -> B_390_378_0 = _rtB -> B_390_376_0 ; }
_rtB -> B_390_379_0 = _rtX -> integrator_CSTATE_h ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_670 ; B_390_380_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ph . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ph . Last , _rtDW -> TransportDelay_IWORK_ph . Tail ,
_rtDW -> TransportDelay_IWORK_ph . Head , _rtP -> P_671 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_382_0 = _rtP -> P_672 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_382_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_384_0 = _rtDW -> Memory_PreviousInput_b ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_386_0 = ( _rtB -> B_390_379_0 - B_390_380_0
) * _rtP -> P_72 ; } else { _rtB -> B_390_386_0 = _rtB -> B_390_384_0 ; }
rtb_B_390_16_0 = _rtP -> P_674 * muDoubleScalarAtan2 ( _rtB -> B_390_386_0 ,
_rtB -> B_390_378_0 ) * _rtP -> P_675 ; B_390_391_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_378_0 , _rtB -> B_390_386_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_391_0 . im = muDoubleScalarHypot
( _rtB -> B_390_378_0 , _rtB -> B_390_386_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_393_0 = _rtB -> B_390_392_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_393_0 , B_390_349_0
, B_390_370_0 , B_390_391_0 , & _rtB -> PosSeqComputation_d , & _rtDW ->
PosSeqComputation_d , & _rtP -> PosSeqComputation_d ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_396_0 = _rtB -> B_390_395_0
; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_396_0 ,
B_390_349_0 , B_390_370_0 , B_390_391_0 , & _rtB -> NegSeqComputation_e , &
_rtDW -> NegSeqComputation_e , & _rtP -> NegSeqComputation_e ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_399_0 = _rtB
-> B_390_398_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_399_0 , B_390_349_0 , B_390_370_0 , B_390_391_0 , & _rtB ->
ZeroSeqComputation_p , & _rtDW -> ZeroSeqComputation_p , & _rtP ->
ZeroSeqComputation_p ) ; B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_d . B_22_2_0 . im , _rtB -> PosSeqComputation_d . B_22_2_0
. re ) ; _rtB -> B_390_402_0 = _rtP -> P_679 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_d . B_22_2_0 . re , _rtB -> PosSeqComputation_d . B_22_2_0
. im ) ; ssCallAccelRunBlock ( S , 390 , 404 , SS_CALL_MDL_OUTPUTS ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_390_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> ref_MODE_f = ( rtb_B_390_16_0 >= _rtP ->
P_681 ) ; if ( _rtDW -> ref_MODE_f == 1 ) { _rtB -> B_390_406_0 = _rtP ->
P_683 ; } else { _rtB -> B_390_406_0 = _rtP -> P_682 ; } rtb_B_390_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> Time_init_ref_MODE_k = ( rtb_B_390_16_0 >=
_rtP -> P_685 ) ; if ( _rtDW -> Time_init_ref_MODE_k == 1 ) { rtb_B_390_239_0
= _rtP -> P_687 ; } else { rtb_B_390_239_0 = _rtP -> P_686 ; } _rtB ->
B_390_409_0 = _rtB -> B_390_407_0 * rtb_B_390_239_0 ; if ( ( _rtDW ->
LastMajorTimeA_l >= ssGetTaskTime ( S , 1 ) ) && ( _rtDW -> LastMajorTimeB_g
>= ssGetTaskTime ( S , 1 ) ) ) { _rtB -> B_390_410_0 = _rtB -> B_390_409_0 ;
} else { if ( ( ( _rtDW -> LastMajorTimeA_l < _rtDW -> LastMajorTimeB_g ) &&
( _rtDW -> LastMajorTimeB_g < ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW ->
LastMajorTimeA_l >= _rtDW -> LastMajorTimeB_g ) && ( _rtDW ->
LastMajorTimeA_l >= ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime ( S
, 1 ) - _rtDW -> LastMajorTimeB_g ; rtb_B_390_4_1 = _rtDW -> PrevYB_c ; }
else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA_l ;
rtb_B_390_4_1 = _rtDW -> PrevYA_ep ; } tmp_1 = tmp_0 * _rtP -> P_688 ;
rtb_B_390_16_0 = _rtB -> B_390_409_0 - rtb_B_390_4_1 ; if ( rtb_B_390_16_0 >
tmp_1 ) { _rtB -> B_390_410_0 = rtb_B_390_4_1 + tmp_1 ; } else { tmp_0 *=
_rtP -> P_689 ; if ( rtb_B_390_16_0 < tmp_0 ) { _rtB -> B_390_410_0 =
rtb_B_390_4_1 + tmp_0 ; } else { _rtB -> B_390_410_0 = _rtB -> B_390_409_0 ;
} } } _rtB -> B_390_412_0 = rtb_B_390_239_0 * _rtB -> B_390_411_0 ; if ( (
_rtDW -> LastMajorTimeA_f >= ssGetTaskTime ( S , 1 ) ) && ( _rtDW ->
LastMajorTimeB_c >= ssGetTaskTime ( S , 1 ) ) ) { _rtB -> B_390_413_0 = _rtB
-> B_390_412_0 ; } else { if ( ( ( _rtDW -> LastMajorTimeA_f < _rtDW ->
LastMajorTimeB_c ) && ( _rtDW -> LastMajorTimeB_c < ssGetTaskTime ( S , 1 ) )
) || ( ( _rtDW -> LastMajorTimeA_f >= _rtDW -> LastMajorTimeB_c ) && ( _rtDW
-> LastMajorTimeA_f >= ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime
( S , 1 ) - _rtDW -> LastMajorTimeB_c ; rtb_B_390_4_1 = _rtDW -> PrevYB_e ; }
else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA_f ;
rtb_B_390_4_1 = _rtDW -> PrevYA_f ; } tmp_1 = tmp_0 * _rtP -> P_691 ;
rtb_B_390_16_0 = _rtB -> B_390_412_0 - rtb_B_390_4_1 ; if ( rtb_B_390_16_0 >
tmp_1 ) { _rtB -> B_390_413_0 = rtb_B_390_4_1 + tmp_1 ; } else { tmp_0 *=
_rtP -> P_692 ; if ( rtb_B_390_16_0 < tmp_0 ) { _rtB -> B_390_413_0 =
rtb_B_390_4_1 + tmp_0 ; } else { _rtB -> B_390_413_0 = _rtB -> B_390_412_0 ;
} } } } _rtB -> B_390_414_0 = 0.0 ; _rtB -> B_390_414_0 += _rtP -> P_694 *
_rtX -> LPF1_CSTATE_e ; _rtB -> B_390_415_0 = 0.0 ; _rtB -> B_390_415_0 +=
_rtP -> P_696 * _rtX -> LPF2_CSTATE_m ; isHit = ssIsSampleHit ( S , 2 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 52 , 0 , SS_CALL_MDL_OUTPUTS )
; } _rtB -> B_390_417_0 = _rtB -> B_390_415_0 - _rtB -> B_52_0_2 ; _rtB ->
B_390_418_0 = _rtP -> P_697 * _rtB -> B_390_417_0 ; _rtB -> B_390_419_0 =
_rtP -> P_698 * _rtB -> B_390_418_0 ; _rtB -> B_390_420_0 = _rtB ->
B_390_406_0 + _rtB -> B_390_419_0 ; _rtB -> B_390_421_0 = _rtP -> P_699 *
_rtB -> B_390_420_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 )
{ rtb_B_390_422_0 = _rtDW -> UnitDelay_DSTATE_o ; _rtB -> B_390_423_0 = _rtP
-> P_701 * _rtDW -> UnitDelay_DSTATE_o ; } _rtB -> B_390_424_0 = _rtX ->
integrator_CSTATE_ok ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_703 ;
B_390_425_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m5 . CircularBufSize
, & _rtDW -> TransportDelay_IWORK_m5 . Last , _rtDW ->
TransportDelay_IWORK_m5 . Tail , _rtDW -> TransportDelay_IWORK_m5 . Head ,
_rtP -> P_704 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_427_0 = _rtP -> P_705 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_427_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_429_0 = _rtDW
-> Memory_PreviousInput_lv ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_431_0 =
( _rtB -> B_390_424_0 - B_390_425_0 ) * _rtP -> P_121 ; } else { _rtB ->
B_390_431_0 = _rtB -> B_390_429_0 ; } _rtB -> B_390_432_0 = _rtX ->
integrator_CSTATE_hr ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_708 ;
B_390_433_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0
, * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_i . Last , _rtDW -> TransportDelay_IWORK_i .
Tail , _rtDW -> TransportDelay_IWORK_i . Head , _rtP -> P_709 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_435_0 = _rtP -> P_710 ; } rtb_B_390_259_0 = ( ssGetT ( S )
>= _rtB -> B_390_435_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_390_437_0 = _rtDW -> Memory_PreviousInput_ff ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_439_0 = ( _rtB -> B_390_432_0 - B_390_433_0
) * _rtP -> P_120 ; } else { _rtB -> B_390_439_0 = _rtB -> B_390_437_0 ; }
rtb_B_390_16_0 = _rtP -> P_712 * muDoubleScalarAtan2 ( _rtB -> B_390_439_0 ,
_rtB -> B_390_431_0 ) * _rtP -> P_713 ; B_390_444_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_431_0 , _rtB -> B_390_439_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_444_0 . im = muDoubleScalarHypot
( _rtB -> B_390_431_0 , _rtB -> B_390_439_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; _rtB -> B_390_445_0 = _rtX -> integrator_CSTATE_e5 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_p . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_715 ; B_390_446_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b . Last , _rtDW -> TransportDelay_IWORK_b . Tail ,
_rtDW -> TransportDelay_IWORK_b . Head , _rtP -> P_716 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_448_0 = _rtP -> P_717 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_448_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_450_0 = _rtDW -> Memory_PreviousInput_of ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_452_0 = ( _rtB -> B_390_445_0 - B_390_446_0
) * _rtP -> P_123 ; } else { _rtB -> B_390_452_0 = _rtB -> B_390_450_0 ; }
_rtB -> B_390_453_0 = _rtX -> integrator_CSTATE_j ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_720 ; B_390_454_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_an . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_an . Last , _rtDW -> TransportDelay_IWORK_an . Tail ,
_rtDW -> TransportDelay_IWORK_an . Head , _rtP -> P_721 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_456_0 = _rtP -> P_722 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_456_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_458_0 = _rtDW -> Memory_PreviousInput_m ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_460_0 = ( _rtB -> B_390_453_0 - B_390_454_0
) * _rtP -> P_122 ; } else { _rtB -> B_390_460_0 = _rtB -> B_390_458_0 ; }
rtb_B_390_16_0 = _rtP -> P_724 * muDoubleScalarAtan2 ( _rtB -> B_390_460_0 ,
_rtB -> B_390_452_0 ) * _rtP -> P_725 ; B_390_465_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_452_0 , _rtB -> B_390_460_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_465_0 . im = muDoubleScalarHypot
( _rtB -> B_390_452_0 , _rtB -> B_390_460_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; _rtB -> B_390_466_0 = _rtX -> integrator_CSTATE_n1 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oo .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oo . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_727 ; B_390_467_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_e . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e . Last , _rtDW -> TransportDelay_IWORK_e . Tail ,
_rtDW -> TransportDelay_IWORK_e . Head , _rtP -> P_728 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_469_0 = _rtP -> P_729 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_469_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_471_0 = _rtDW -> Memory_PreviousInput_a3 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_473_0 = ( _rtB -> B_390_466_0 - B_390_467_0
) * _rtP -> P_125 ; } else { _rtB -> B_390_473_0 = _rtB -> B_390_471_0 ; }
_rtB -> B_390_474_0 = _rtX -> integrator_CSTATE_fv ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_732 ; B_390_475_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_j . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j . Last , _rtDW -> TransportDelay_IWORK_j . Tail ,
_rtDW -> TransportDelay_IWORK_j . Head , _rtP -> P_733 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_477_0 = _rtP -> P_734 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_477_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_479_0 = _rtDW -> Memory_PreviousInput_ll ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_481_0 = ( _rtB -> B_390_474_0 - B_390_475_0
) * _rtP -> P_124 ; } else { _rtB -> B_390_481_0 = _rtB -> B_390_479_0 ; }
rtb_B_390_16_0 = _rtP -> P_736 * muDoubleScalarAtan2 ( _rtB -> B_390_481_0 ,
_rtB -> B_390_473_0 ) * _rtP -> P_737 ; B_390_486_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_473_0 , _rtB -> B_390_481_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_486_0 . im = muDoubleScalarHypot
( _rtB -> B_390_473_0 , _rtB -> B_390_481_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_488_0 = _rtB -> B_390_487_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_488_0 , B_390_444_0
, B_390_465_0 , B_390_486_0 , & _rtB -> PosSeqComputation_p , & _rtDW ->
PosSeqComputation_p , & _rtP -> PosSeqComputation_p ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_491_0 = _rtB -> B_390_490_0
; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_491_0 ,
B_390_444_0 , B_390_465_0 , B_390_486_0 , & _rtB -> NegSeqComputation_m , &
_rtDW -> NegSeqComputation_m , & _rtP -> NegSeqComputation_m ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_494_0 = _rtB
-> B_390_493_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_494_0 , B_390_444_0 , B_390_465_0 , B_390_486_0 , & _rtB ->
ZeroSeqComputation_n , & _rtDW -> ZeroSeqComputation_n , & _rtP ->
ZeroSeqComputation_n ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_p . B_22_2_0 . im , _rtB -> PosSeqComputation_p . B_22_2_0
. re ) ; _rtB -> B_390_497_0 = _rtP -> P_741 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_p . B_22_2_0 . re , _rtB -> PosSeqComputation_p . B_22_2_0
. im ) ; _rtB -> B_390_498_0 = _rtX -> integrator_CSTATE_b ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_743 ; B_390_499_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_lg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lg . Last , _rtDW -> TransportDelay_IWORK_lg . Tail ,
_rtDW -> TransportDelay_IWORK_lg . Head , _rtP -> P_744 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_501_0 = _rtP -> P_745 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_501_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_503_0 = _rtDW -> Memory_PreviousInput_e0 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_505_0 = ( _rtB -> B_390_498_0 - B_390_499_0
) * _rtP -> P_127 ; } else { _rtB -> B_390_505_0 = _rtB -> B_390_503_0 ; }
_rtB -> B_390_506_0 = _rtX -> integrator_CSTATE_kx ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_748 ; B_390_507_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ho . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ho . Last , _rtDW -> TransportDelay_IWORK_ho . Tail ,
_rtDW -> TransportDelay_IWORK_ho . Head , _rtP -> P_749 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_509_0 = _rtP -> P_750 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_509_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_511_0 = _rtDW -> Memory_PreviousInput_h ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_513_0 = ( _rtB -> B_390_506_0 - B_390_507_0
) * _rtP -> P_126 ; } else { _rtB -> B_390_513_0 = _rtB -> B_390_511_0 ; }
rtb_B_390_16_0 = _rtP -> P_752 * muDoubleScalarAtan2 ( _rtB -> B_390_513_0 ,
_rtB -> B_390_505_0 ) * _rtP -> P_753 ; B_390_518_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_505_0 , _rtB -> B_390_513_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_518_0 . im = muDoubleScalarHypot
( _rtB -> B_390_505_0 , _rtB -> B_390_513_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; _rtB -> B_390_519_0 = _rtX -> integrator_CSTATE_m ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_755 ; B_390_520_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ay . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ay . Last , _rtDW -> TransportDelay_IWORK_ay . Tail ,
_rtDW -> TransportDelay_IWORK_ay . Head , _rtP -> P_756 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_522_0 = _rtP -> P_757 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_522_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_524_0 = _rtDW -> Memory_PreviousInput_i ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_526_0 = ( _rtB -> B_390_519_0 - B_390_520_0
) * _rtP -> P_129 ; } else { _rtB -> B_390_526_0 = _rtB -> B_390_524_0 ; }
_rtB -> B_390_527_0 = _rtX -> integrator_CSTATE_c ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_760 ; B_390_528_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_g . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g . Last , _rtDW -> TransportDelay_IWORK_g . Tail ,
_rtDW -> TransportDelay_IWORK_g . Head , _rtP -> P_761 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_530_0 = _rtP -> P_762 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_530_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_532_0 = _rtDW -> Memory_PreviousInput_k ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_534_0 = ( _rtB -> B_390_527_0 - B_390_528_0
) * _rtP -> P_128 ; } else { _rtB -> B_390_534_0 = _rtB -> B_390_532_0 ; }
rtb_B_390_16_0 = _rtP -> P_764 * muDoubleScalarAtan2 ( _rtB -> B_390_534_0 ,
_rtB -> B_390_526_0 ) * _rtP -> P_765 ; B_390_539_0 . re =
muDoubleScalarHypot ( _rtB -> B_390_526_0 , _rtB -> B_390_534_0 ) *
muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_539_0 . im = muDoubleScalarHypot
( _rtB -> B_390_526_0 , _rtB -> B_390_534_0 ) * muDoubleScalarSin (
rtb_B_390_16_0 ) ; _rtB -> B_390_540_0 = _rtX -> integrator_CSTATE_cw ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b3 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b3 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_767 ; B_390_541_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_mq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mq . Last , _rtDW -> TransportDelay_IWORK_mq . Tail ,
_rtDW -> TransportDelay_IWORK_mq . Head , _rtP -> P_768 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_543_0 = _rtP -> P_769 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_543_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_545_0 = _rtDW -> Memory_PreviousInput_im ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_547_0 = ( _rtB -> B_390_540_0 - B_390_541_0
) * _rtP -> P_131 ; } else { _rtB -> B_390_547_0 = _rtB -> B_390_545_0 ; }
_rtB -> B_390_548_0 = _rtX -> integrator_CSTATE_d ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_772 ; B_390_549_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_od
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_od . Last , _rtDW ->
TransportDelay_IWORK_od . Tail , _rtDW -> TransportDelay_IWORK_od . Head ,
_rtP -> P_773 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_551_0 = _rtP -> P_774 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_551_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_553_0 = _rtDW
-> Memory_PreviousInput_jc ; } if ( rtb_B_390_259_0 ) { _rtB -> B_390_555_0 =
( _rtB -> B_390_548_0 - B_390_549_0 ) * _rtP -> P_130 ; } else { _rtB ->
B_390_555_0 = _rtB -> B_390_553_0 ; } rtb_B_390_16_0 = _rtP -> P_776 *
muDoubleScalarAtan2 ( _rtB -> B_390_555_0 , _rtB -> B_390_547_0 ) * _rtP ->
P_777 ; B_390_560_0 . re = muDoubleScalarHypot ( _rtB -> B_390_547_0 , _rtB
-> B_390_555_0 ) * muDoubleScalarCos ( rtb_B_390_16_0 ) ; B_390_560_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_547_0 , _rtB -> B_390_555_0 ) *
muDoubleScalarSin ( rtb_B_390_16_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_390_562_0 = _rtB -> B_390_561_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_562_0 , B_390_518_0
, B_390_539_0 , B_390_560_0 , & _rtB -> PosSeqComputation_a , & _rtDW ->
PosSeqComputation_a , & _rtP -> PosSeqComputation_a ) ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_565_0 = _rtB -> B_390_564_0
; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_565_0 ,
B_390_518_0 , B_390_539_0 , B_390_560_0 , & _rtB -> NegSeqComputation_j , &
_rtDW -> NegSeqComputation_j , & _rtP -> NegSeqComputation_j ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_568_0 = _rtB
-> B_390_567_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_568_0 , B_390_518_0 , B_390_539_0 , B_390_560_0 , & _rtB ->
ZeroSeqComputation_o , & _rtDW -> ZeroSeqComputation_o , & _rtP ->
ZeroSeqComputation_o ) ; rtb_B_390_239_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_a . B_22_2_0 . im , _rtB -> PosSeqComputation_a . B_22_2_0
. re ) ; _rtB -> B_390_571_0 = _rtP -> P_781 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_a . B_22_2_0 . re , _rtB -> PosSeqComputation_a . B_22_2_0
. im ) ; ssCallAccelRunBlock ( S , 390 , 573 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 574 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 575 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_390_576_0 = _rtP -> P_783 * _rtB -> B_390_328_0 ; _rtB -> B_390_577_0 =
_rtP -> P_784 * _rtB -> B_390_497_0 ; ssCallAccelRunBlock ( S , 390 , 578 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { ssCallAccelRunBlock ( S , 390 , 579 , SS_CALL_MDL_OUTPUTS ) ; } isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_580_0 = _rtP
-> P_785 * _rtB -> B_390_254_0 ; _rtB -> B_390_581_0 = _rtP -> P_786 * _rtB
-> B_390_423_0 ; ssCallAccelRunBlock ( S , 390 , 582 , SS_CALL_MDL_OUTPUTS )
; } _rtB -> B_390_583_0 [ 0 ] = _rtB -> B_390_245_0 ; _rtB -> B_390_583_0 [ 1
] = _rtB -> B_390_254_0 ; _rtB -> B_390_583_0 [ 2 ] = _rtB -> B_390_246_0 ;
_rtB -> B_390_583_0 [ 3 ] = _rtB -> B_390_328_0 ; _rtB -> B_390_583_0 [ 4 ] =
_rtB -> B_390_402_0 ; _rtB -> B_390_583_0 [ 5 ] = _rtB -> B_390_252_0 ; _rtB
-> B_390_583_0 [ 6 ] = _rtB -> B_15_0_1 ; _rtB -> B_390_583_0 [ 7 ] = _rtB ->
B_15_0_2 ; _rtB -> B_390_583_0 [ 8 ] = 0.0 ; _rtB -> B_390_583_0 [ 9 ] = 0.0
; _rtB -> B_390_583_0 [ 10 ] = 0.0 ; _rtB -> B_390_583_0 [ 11 ] = 0.0 ;
ssCallAccelRunBlock ( S , 390 , 584 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 585 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 586 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_591_0 [ 0 ] =
_rtP -> P_788 * _rtB -> B_0_14_0 [ 41 ] * _rtP -> P_791 ; _rtB -> B_390_591_0
[ 1 ] = _rtP -> P_789 * _rtB -> B_0_14_0 [ 42 ] * _rtP -> P_791 ; _rtB ->
B_390_591_0 [ 2 ] = _rtP -> P_790 * _rtB -> B_0_14_0 [ 43 ] * _rtP -> P_791 ;
for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] = _rtP -> P_792 [
isHit + 6 ] * _rtB -> B_390_591_0 [ 2 ] + ( _rtP -> P_792 [ isHit + 3 ] *
_rtB -> B_390_591_0 [ 1 ] + _rtP -> P_792 [ isHit ] * _rtB -> B_390_591_0 [ 0
] ) ; } _rtB -> B_390_593_0 [ 0 ] = _rtP -> P_793 * tmp [ 0 ] ; _rtB ->
B_390_593_0 [ 1 ] = _rtP -> P_793 * tmp [ 1 ] ; _rtB -> B_390_593_0 [ 2 ] =
_rtP -> P_793 * tmp [ 2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_390_597_0 = _rtB -> B_390_596_0 ; }
MR_CIGRE_RESISTIVO_Subsystem1 ( S , _rtB -> B_390_597_0 , & _rtB ->
B_390_593_0 [ 0 ] , _rtB -> B_390_19_0 , & _rtB -> Subsystem1_b , & _rtDW ->
Subsystem1_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_601_0 = _rtB -> B_390_600_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_601_0 , & _rtB ->
B_390_593_0 [ 0 ] , _rtB -> B_390_19_0 , & _rtB -> Subsystempi2delay_ls , &
_rtDW -> Subsystempi2delay_ls ) ; if ( _rtB -> B_390_596_0 != 0 ) { _rtB ->
B_390_603_0 [ 0 ] = _rtB -> Subsystem1_b . B_2_0_0 ; _rtB -> B_390_603_0 [ 1
] = _rtB -> Subsystem1_b . B_2_1_0 ; } else { _rtB -> B_390_603_0 [ 0 ] =
_rtB -> Subsystempi2delay_ls . B_1_0_0 ; _rtB -> B_390_603_0 [ 1 ] = _rtB ->
Subsystempi2delay_ls . B_1_1_0 ; } _rtB -> B_390_604_0 = 0.0 ; _rtB ->
B_390_604_0 += _rtP -> P_798 * _rtX -> Integradord_CSTATE ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_799 *
_rtB -> B_0_14_0 [ 93 ] * _rtP -> P_802 ; tmp_1 = _rtP -> P_800 * _rtB ->
B_0_14_0 [ 94 ] * _rtP -> P_802 ; rtb_B_390_16_0 = _rtP -> P_801 * _rtB ->
B_0_14_0 [ 95 ] * _rtP -> P_802 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_803 [ isHit + 6 ] * rtb_B_390_16_0 + ( _rtP ->
P_803 [ isHit + 3 ] * tmp_1 + _rtP -> P_803 [ isHit ] * tmp_0 ) ; } _rtB ->
B_390_610_0 [ 0 ] = _rtP -> P_804 * tmp [ 0 ] ; _rtB -> B_390_610_0 [ 1 ] =
_rtP -> P_804 * tmp [ 1 ] ; _rtB -> B_390_610_0 [ 2 ] = _rtP -> P_804 * tmp [
2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_614_0 = _rtB -> B_390_613_0 ; } MR_CIGRE_RESISTIVO_Subsystem1 ( S ,
_rtB -> B_390_614_0 , & _rtB -> B_390_610_0 [ 0 ] , _rtB -> B_390_19_0 , &
_rtB -> Subsystem1_d , & _rtDW -> Subsystem1_d ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_618_0 = _rtB -> B_390_617_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_618_0 , & _rtB ->
B_390_610_0 [ 0 ] , _rtB -> B_390_19_0 , & _rtB -> Subsystempi2delay_lc , &
_rtDW -> Subsystempi2delay_lc ) ; if ( _rtB -> B_390_613_0 != 0 ) { _rtB ->
B_390_620_0 [ 0 ] = _rtB -> Subsystem1_d . B_2_0_0 ; _rtB -> B_390_620_0 [ 1
] = _rtB -> Subsystem1_d . B_2_1_0 ; } else { _rtB -> B_390_620_0 [ 0 ] =
_rtB -> Subsystempi2delay_lc . B_1_0_0 ; _rtB -> B_390_620_0 [ 1 ] = _rtB ->
Subsystempi2delay_lc . B_1_1_0 ; } _rtB -> B_390_621_0 = 0.0 ; _rtB ->
B_390_621_0 += _rtP -> P_809 * _rtX -> LPF4_CSTATE ; _rtB -> B_390_622_0 =
_rtB -> B_390_621_0 - _rtB -> B_390_620_0 [ 0 ] ; _rtB -> B_390_623_0 = _rtP
-> P_810 * _rtB -> B_390_622_0 ; _rtB -> B_390_624_0 = _rtB -> B_390_604_0 +
_rtB -> B_390_623_0 ; _rtB -> B_390_625_0 = _rtB -> B_390_254_0 * _rtB ->
B_390_620_0 [ 1 ] ; _rtB -> B_390_626_0 = _rtP -> P_811 * _rtB -> B_390_625_0
; _rtB -> B_390_627_0 = ( _rtB -> B_390_603_0 [ 0 ] + _rtB -> B_390_624_0 ) -
_rtB -> B_390_626_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_390_629_0 = _rtDW -> UnitDelay1_DSTATE_pe / _rtB -> B_390_587_0 ;
} _rtB -> B_390_630_0 = _rtB -> B_390_627_0 / _rtB -> B_390_629_0 ; _rtB ->
B_390_631_0 = _rtB -> B_390_620_0 [ 0 ] * _rtB -> B_390_254_0 ; _rtB ->
B_390_632_0 = _rtP -> P_813 * _rtB -> B_390_631_0 ; _rtB -> B_390_633_0 = 0.0
; _rtB -> B_390_633_0 += _rtP -> P_815 * _rtX -> LPF2_CSTATE_p ; _rtB ->
B_390_634_0 = _rtB -> B_390_633_0 - _rtB -> B_390_620_0 [ 1 ] ; _rtB ->
B_390_635_0 = _rtP -> P_816 * _rtB -> B_390_634_0 ; _rtB -> B_390_636_0 = 0.0
; _rtB -> B_390_636_0 += _rtP -> P_818 * _rtX -> Integradord1_CSTATE ; _rtB
-> B_390_637_0 = _rtB -> B_390_635_0 + _rtB -> B_390_636_0 ; _rtB ->
B_390_638_0 = ( _rtB -> B_390_632_0 + _rtB -> B_390_637_0 ) + _rtB ->
B_390_603_0 [ 1 ] ; _rtB -> B_390_639_0 = 1.0 / _rtB -> B_390_629_0 * _rtB ->
B_390_638_0 ; _rtB -> B_390_640_0 = _rtB -> B_390_1_0 - _rtB -> B_390_7_0 ;
_rtB -> B_390_641_0 = _rtP -> P_819 * _rtB -> B_390_640_0 ; _rtB ->
B_390_642_0 = _rtB -> B_390_641_0 * _rtB -> B_390_629_0 ; _rtB -> B_390_643_0
= _rtB -> B_390_2_0 - _rtB -> B_390_7_1 ; _rtB -> B_390_644_0 = _rtP -> P_820
* _rtB -> B_390_643_0 ; _rtB -> B_390_645_0 = _rtB -> B_390_629_0 * _rtB ->
B_390_644_0 ; _rtB -> B_390_646_0 = _rtB -> B_390_622_0 - _rtB -> B_390_642_0
; _rtB -> B_390_647_0 = _rtB -> B_390_634_0 - _rtB -> B_390_645_0 ; _rtB ->
B_390_648_0 = _rtB -> B_390_245_0 - _rtB -> B_15_0_1 ; _rtB -> B_390_649_0 =
_rtP -> P_821 * _rtB -> B_390_648_0 ; _rtB -> B_390_650_0 = _rtP -> P_822 *
_rtB -> B_390_649_0 ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ rtb_B_390_16_0 = ssGetTaskTime ( S , 1 ) ; _rtDW -> ref2_MODE = (
rtb_B_390_16_0 >= _rtP -> P_823 ) ; if ( _rtDW -> ref2_MODE == 1 ) { _rtB ->
B_390_652_0 = _rtP -> P_825 ; } else { _rtB -> B_390_652_0 = _rtP -> P_824 ;
} } _rtB -> B_390_653_0 = _rtB -> B_390_652_0 - _rtB -> B_390_650_0 ; _rtB ->
B_390_654_0 = _rtP -> P_826 * _rtB -> B_390_653_0 ; isHit = ssIsSampleHit ( S
, 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_658_0 [ 0 ] = _rtP -> P_827 *
_rtB -> B_0_14_0 [ 96 ] * _rtP -> P_830 ; _rtB -> B_390_658_0 [ 1 ] = _rtP ->
P_828 * _rtB -> B_0_14_0 [ 97 ] * _rtP -> P_830 ; _rtB -> B_390_658_0 [ 2 ] =
_rtP -> P_829 * _rtB -> B_0_14_0 [ 98 ] * _rtP -> P_830 ; for ( isHit = 0 ;
isHit < 3 ; isHit ++ ) { tmp [ isHit ] = _rtP -> P_831 [ isHit + 6 ] * _rtB
-> B_390_658_0 [ 2 ] + ( _rtP -> P_831 [ isHit + 3 ] * _rtB -> B_390_658_0 [
1 ] + _rtP -> P_831 [ isHit ] * _rtB -> B_390_658_0 [ 0 ] ) ; } _rtB ->
B_390_660_0 [ 0 ] = _rtP -> P_832 * tmp [ 0 ] ; _rtB -> B_390_660_0 [ 1 ] =
_rtP -> P_832 * tmp [ 1 ] ; _rtB -> B_390_660_0 [ 2 ] = _rtP -> P_832 * tmp [
2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_664_0 = _rtB -> B_390_663_0 ; } MR_CIGRE_RESISTIVO_Subsystem1 ( S ,
_rtB -> B_390_664_0 , & _rtB -> B_390_660_0 [ 0 ] , _rtB -> B_390_19_0 , &
_rtB -> Subsystem1_lc , & _rtDW -> Subsystem1_lc ) ; isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_668_0 = _rtB -> B_390_667_0 ;
} MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_668_0 , & _rtB ->
B_390_660_0 [ 0 ] , _rtB -> B_390_19_0 , & _rtB -> Subsystempi2delay_e , &
_rtDW -> Subsystempi2delay_e ) ; if ( _rtB -> B_390_663_0 != 0 ) { _rtB ->
B_390_670_0 [ 0 ] = _rtB -> Subsystem1_lc . B_2_0_0 ; _rtB -> B_390_670_0 [ 1
] = _rtB -> Subsystem1_lc . B_2_1_0 ; } else { _rtB -> B_390_670_0 [ 0 ] =
_rtB -> Subsystempi2delay_e . B_1_0_0 ; _rtB -> B_390_670_0 [ 1 ] = _rtB ->
Subsystempi2delay_e . B_1_1_0 ; } _rtB -> B_390_671_0 = _rtP -> P_836 * _rtB
-> B_390_670_0 [ 0 ] ; _rtB -> B_390_672_0 = 0.0 ; _rtB -> B_390_672_0 +=
_rtP -> P_838 * _rtX -> Integradord_CSTATE_l ; _rtB -> B_390_673_0 = _rtP ->
P_839 * _rtB -> B_390_670_0 [ 0 ] ; _rtB -> B_390_674_0 = 0.0 ; _rtB ->
B_390_674_0 += _rtP -> P_841 * _rtX -> HPF1_CSTATE ; _rtB -> B_390_674_0 +=
_rtP -> P_842 * _rtB -> B_390_673_0 ; _rtB -> B_390_675_0 = _rtP -> P_843 *
_rtB -> B_390_670_0 [ 0 ] ; _rtB -> B_390_676_0 = _rtB -> B_390_674_0 + _rtB
-> B_390_675_0 ; _rtB -> B_390_677_0 = _rtP -> P_844 * _rtB -> B_390_670_0 [
1 ] ; _rtB -> B_390_678_0 = _rtB -> B_390_677_0 * _rtB -> B_390_254_0 ; _rtB
-> B_390_679_0 = _rtB -> B_390_676_0 + _rtB -> B_390_678_0 ; _rtB ->
B_390_680_0 = _rtB -> B_390_654_0 - _rtB -> B_390_679_0 ; _rtB -> B_390_681_0
= _rtB -> B_390_680_0 - _rtB -> B_390_603_0 [ 0 ] ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_17_0 > _rtP -> P_847 ) {
_rtB -> B_390_684_0 = _rtB -> B_390_682_0 ; } else { _rtB -> B_390_684_0 =
_rtB -> B_390_683_0 ; } } _rtB -> B_390_685_0 = _rtB -> B_390_681_0 * _rtB ->
B_390_684_0 ; _rtB -> B_390_686_0 = _rtP -> P_848 * _rtB -> B_390_685_0 ;
_rtB -> B_390_687_0 = _rtB -> B_390_672_0 + _rtB -> B_390_686_0 ; _rtB ->
B_390_688_0 = _rtB -> B_390_254_0 * _rtB -> B_390_603_0 [ 1 ] ; _rtB ->
B_390_689_0 = _rtP -> P_849 * _rtB -> B_390_688_0 ; _rtB -> B_390_690_0 = (
_rtB -> B_390_671_0 + _rtB -> B_390_687_0 ) - _rtB -> B_390_689_0 ; _rtB ->
B_390_691_0 = _rtB -> B_390_690_0 * _rtB -> B_390_684_0 ; _rtB -> B_390_692_0
= _rtB -> B_390_603_0 [ 0 ] * _rtB -> B_390_254_0 ; _rtB -> B_390_693_0 =
_rtP -> P_850 * _rtB -> B_390_692_0 ; _rtB -> B_390_695_0 = _rtP -> P_852 *
_rtB -> B_390_670_0 [ 0 ] ; _rtB -> B_390_696_0 = _rtB -> B_390_254_0 * _rtB
-> B_390_695_0 ; _rtB -> B_390_697_0 = _rtP -> P_853 * _rtB -> B_390_670_0 [
1 ] ; _rtB -> B_390_698_0 = 0.0 ; _rtB -> B_390_698_0 += _rtP -> P_855 * _rtX
-> HPF2_CSTATE ; _rtB -> B_390_698_0 += _rtP -> P_856 * _rtB -> B_390_697_0 ;
_rtB -> B_390_699_0 = _rtP -> P_857 * _rtB -> B_390_670_0 [ 1 ] ; _rtB ->
B_390_700_0 = _rtB -> B_390_698_0 + _rtB -> B_390_699_0 ; _rtB -> B_390_701_0
= _rtB -> B_390_696_0 + _rtB -> B_390_700_0 ; _rtB -> B_390_702_0 = _rtB ->
B_390_694_0 - _rtB -> B_390_701_0 ; _rtB -> B_390_703_0 = _rtB -> B_390_702_0
- _rtB -> B_390_603_0 [ 1 ] ; _rtB -> B_390_704_0 = _rtB -> B_390_684_0 *
_rtB -> B_390_703_0 ; _rtB -> B_390_705_0 = _rtP -> P_858 * _rtB ->
B_390_704_0 ; _rtB -> B_390_706_0 = 0.0 ; _rtB -> B_390_706_0 += _rtP ->
P_860 * _rtX -> Integradorq_CSTATE ; _rtB -> B_390_707_0 = _rtB ->
B_390_705_0 + _rtB -> B_390_706_0 ; _rtB -> B_390_708_0 = _rtP -> P_861 *
_rtB -> B_390_670_0 [ 1 ] ; _rtB -> B_390_709_0 = ( _rtB -> B_390_693_0 +
_rtB -> B_390_707_0 ) + _rtB -> B_390_708_0 ; _rtB -> B_390_710_0 = _rtB ->
B_390_684_0 * _rtB -> B_390_709_0 ; _rtB -> B_390_711_0 . re = _rtB ->
B_390_691_0 ; _rtB -> B_390_711_0 . im = _rtB -> B_390_710_0 ; _rtB ->
B_390_712_0 = muDoubleScalarHypot ( _rtB -> B_390_711_0 . re , _rtB ->
B_390_711_0 . im ) ; rtb_B_390_16_0 = muDoubleScalarAtan2 ( _rtB ->
B_390_711_0 . im , _rtB -> B_390_711_0 . re ) ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { _rtDW -> Saturation_MODE_k = _rtB -> B_390_712_0 >= _rtP -> P_862 ?
1 : _rtB -> B_390_712_0 > _rtP -> P_863 ? 0 : - 1 ; } _rtB -> B_390_713_0 =
_rtDW -> Saturation_MODE_k == 1 ? _rtP -> P_862 : _rtDW -> Saturation_MODE_k
== - 1 ? _rtP -> P_863 : _rtB -> B_390_712_0 ; _rtB -> B_390_715_0 = _rtB ->
B_390_713_0 * muDoubleScalarCos ( rtb_B_390_16_0 ) ; _rtB -> B_390_715_1 =
_rtB -> B_390_713_0 * muDoubleScalarSin ( rtb_B_390_16_0 ) ;
ssCallAccelRunBlock ( S , 390 , 716 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 717 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_390_718_0 = _rtB -> B_390_691_0 - _rtB -> B_390_715_0 ; _rtB -> B_390_719_0
= _rtP -> P_864 * _rtB -> B_390_718_0 ; _rtB -> B_390_720_0 = _rtB ->
B_390_685_0 - _rtB -> B_390_719_0 ; _rtB -> B_390_721_0 = _rtP -> P_865 *
_rtB -> B_390_720_0 ; _rtB -> B_390_722_0 = _rtB -> B_390_710_0 - _rtB ->
B_390_715_1 ; _rtB -> B_390_723_0 = _rtP -> P_866 * _rtB -> B_390_722_0 ;
_rtB -> B_390_724_0 = _rtB -> B_390_704_0 - _rtB -> B_390_723_0 ; _rtB ->
B_390_725_0 = _rtP -> P_867 * _rtB -> B_390_724_0 ; _rtB -> B_390_726_0 =
_rtB -> B_15_0_2 * _rtB -> B_390_603_0 [ 0 ] ; _rtB -> B_390_727_0 = _rtB ->
B_15_0_1 * _rtB -> B_390_603_0 [ 1 ] ; _rtB -> B_390_728_0 = _rtB ->
B_390_727_0 - _rtB -> B_390_726_0 ; _rtB -> B_390_729_0 = _rtB -> B_390_603_0
[ 0 ] * _rtB -> B_390_603_0 [ 0 ] ; _rtB -> B_390_730_0 = _rtB -> B_390_603_0
[ 1 ] * _rtB -> B_390_603_0 [ 1 ] ; _rtB -> B_390_731_0 = _rtB -> B_390_729_0
- _rtB -> B_390_730_0 ; _rtB -> B_390_732_0 = _rtB -> B_390_728_0 / _rtB ->
B_390_731_0 ; _rtB -> B_390_733_0 = _rtP -> P_868 * _rtB -> B_390_732_0 ;
ssCallAccelRunBlock ( S , 390 , 734 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_390_735_0 = _rtB -> B_15_0_1 * _rtB -> B_390_603_0 [ 0 ] ; _rtB ->
B_390_736_0 = _rtB -> B_15_0_2 * _rtB -> B_390_603_0 [ 1 ] ; _rtB ->
B_390_737_0 = _rtB -> B_390_735_0 - _rtB -> B_390_736_0 ; _rtB -> B_390_738_0
= _rtB -> B_390_730_0 + _rtB -> B_390_729_0 ; _rtB -> B_390_739_0 = _rtB ->
B_390_737_0 / _rtB -> B_390_738_0 ; _rtB -> B_390_740_0 = _rtP -> P_869 *
_rtB -> B_390_739_0 ; ssCallAccelRunBlock ( S , 390 , 741 ,
SS_CALL_MDL_OUTPUTS ) ; if ( _rtB -> B_390_17_0 > _rtP -> P_870 ) { _rtB ->
B_390_742_0 = _rtB -> B_390_254_0 ; } else { _rtB -> B_390_742_0 = _rtB ->
B_390_252_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
B_390_746_0 [ 0 ] = _rtP -> P_871 * _rtB -> B_0_14_0 [ 38 ] * _rtP -> P_874 ;
B_390_746_0 [ 1 ] = _rtP -> P_872 * _rtB -> B_0_14_0 [ 39 ] * _rtP -> P_874 ;
B_390_746_0 [ 2 ] = _rtP -> P_873 * _rtB -> B_0_14_0 [ 40 ] * _rtP -> P_874 ;
rtb_B_390_784_0 = _rtP -> P_876 * _rtB -> B_390_33_0 - _rtDW ->
prediction_DSTATE ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_390_788_0 = rtb_B_390_784_0 + _rtB -> B_390_781_0 ; } _rtB ->
B_390_789_0 = ( _rtB -> B_390_36_0 * _rtB -> B_0_14_0 [ 148 ] - _rtB ->
B_390_37_0 * _rtB -> B_0_14_0 [ 149 ] ) / _rtB -> B_390_788_0 ; _rtB ->
B_390_804_0 = _rtB -> B_390_603_0 [ 0 ] * _rtB -> B_390_620_0 [ 0 ] ; _rtB ->
B_390_805_0 = _rtB -> B_390_603_0 [ 1 ] * _rtB -> B_390_620_0 [ 1 ] ; _rtB ->
B_390_806_0 = _rtB -> B_390_804_0 + _rtB -> B_390_805_0 ; _rtB -> B_390_807_0
= _rtP -> P_878 * _rtB -> B_390_806_0 ; _rtB -> B_390_808_0 = _rtB ->
B_390_603_0 [ 0 ] * _rtB -> B_390_620_0 [ 1 ] ; _rtB -> B_390_809_0 = _rtB ->
B_390_620_0 [ 0 ] * _rtB -> B_390_603_0 [ 1 ] ; _rtB -> B_390_810_0 = _rtB ->
B_390_809_0 - _rtB -> B_390_808_0 ; _rtB -> B_390_811_0 = _rtP -> P_879 *
_rtB -> B_390_810_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 )
{ ssCallAccelRunBlock ( S , 390 , 812 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_390_814_0 = _rtP -> P_880 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( (
isHit != 0 ) && ( ssIsMajorTimeStep ( S ) != 0 ) ) { if ( _rtB -> B_390_814_0
> 0.0 ) { if ( ! _rtDW -> AutomaticGainControl_MODE_l ) { if ( ssGetTaskTime
( S , 3 ) != ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep
( S ) ; } _rtDW -> Integ4_SYSTEM_ENABLE_a = 1U ; _rtDW ->
Integ4_SYSTEM_ENABLE_ll = 1U ; _rtDW -> AutomaticGainControl_MODE_l = true ;
} } else { if ( _rtDW -> AutomaticGainControl_MODE_l ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; _rtDW ->
AutomaticGainControl_MODE_l = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_l ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_21 [ isHit + 6 ] * _rtB -> B_390_591_0 [ 2 ] + ( _rtP -> P_21 [
isHit + 3 ] * _rtB -> B_390_591_0 [ 1 ] + _rtP -> P_21 [ isHit ] * _rtB ->
B_390_591_0 [ 0 ] ) ; } B_11_1_0 [ 0 ] = _rtP -> P_22 * tmp [ 0 ] ; B_11_1_0
[ 1 ] = _rtP -> P_22 * tmp [ 1 ] ; B_11_1_0 [ 2 ] = _rtP -> P_22 * tmp [ 2 ]
; _rtB -> B_11_5_0 = _rtB -> B_11_4_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b (
_rtB -> B_11_5_0 , & B_11_1_0 [ 0 ] , _rtB -> B_390_12_0 , & _rtB ->
Subsystem1_b2 , & _rtDW -> Subsystem1_b2 ) ; _rtB -> B_11_9_0 = _rtB ->
B_11_8_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_11_9_0 , &
B_11_1_0 [ 0 ] , _rtB -> B_390_12_0 , & _rtB -> Subsystempi2delay_m5 , &
_rtDW -> Subsystempi2delay_m5 ) ; if ( _rtB -> B_11_4_0 != 0 ) { _rtB ->
B_11_11_0 [ 0 ] = _rtB -> Subsystem1_b2 . B_10_0_0 ; _rtB -> B_11_11_0 [ 1 ]
= _rtB -> Subsystem1_b2 . B_10_1_0 ; } else { _rtB -> B_11_11_0 [ 0 ] = _rtB
-> Subsystempi2delay_m5 . B_9_0_0 ; _rtB -> B_11_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_m5 . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_a != 0
) { _rtB -> B_11_12_0 = _rtDW -> Integ4_DSTATE_e ; } else { _rtB -> B_11_12_0
= _rtP -> P_26 * _rtB -> B_11_11_0 [ 0 ] + _rtDW -> Integ4_DSTATE_e ; } if (
rtb_B_390_253_0 > _rtP -> P_28 ) { rtb_B_390_784_0 = _rtP -> P_28 ; } else if
( rtb_B_390_253_0 < _rtP -> P_29 ) { rtb_B_390_784_0 = _rtP -> P_29 ; } else
{ rtb_B_390_784_0 = rtb_B_390_253_0 ; } rtb_B_390_784_0 = 1.0 /
rtb_B_390_784_0 / 5.0e-5 ; rtb_B_390_16_0 = muDoubleScalarCeil (
rtb_B_390_784_0 ) ; _rtB -> B_11_16_0 = _rtP -> P_30 * rtb_B_390_16_0 ;
ssCallAccelRunBlock ( S , 11 , 17 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_11_20_0 ) { rtb_B_390_16_0 =
rtb_B_390_784_0 - rtb_B_390_16_0 ; _rtB -> B_11_24_0 = ( ( _rtB -> B_11_11_0
[ 0 ] - _rtDW -> UnitDelay_DSTATE_n ) * rtb_B_390_16_0 * _rtP -> P_18 + _rtB
-> B_11_11_0 [ 0 ] ) * ( rtb_B_390_16_0 / rtb_B_390_784_0 ) + ( _rtB ->
B_11_12_0 - _rtB -> B_11_17_0 ) * rtb_B_390_253_0 ; } else { _rtB ->
B_11_24_0 = _rtDW -> UnitDelay1_DSTATE_fp ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_ll != 0 ) { _rtB -> B_11_25_0 = _rtDW -> Integ4_DSTATE_c
; } else { _rtB -> B_11_25_0 = _rtP -> P_42 * _rtB -> B_11_11_0 [ 1 ] + _rtDW
-> Integ4_DSTATE_c ; } if ( rtb_B_390_253_0 > _rtP -> P_44 ) {
rtb_B_390_784_0 = _rtP -> P_44 ; } else if ( rtb_B_390_253_0 < _rtP -> P_45 )
{ rtb_B_390_784_0 = _rtP -> P_45 ; } else { rtb_B_390_784_0 = rtb_B_390_253_0
; } rtb_B_390_784_0 = 1.0 / rtb_B_390_784_0 / 5.0e-5 ; rtb_B_390_16_0 =
muDoubleScalarCeil ( rtb_B_390_784_0 ) ; _rtB -> B_11_29_0 = _rtP -> P_46 *
rtb_B_390_16_0 ; ssCallAccelRunBlock ( S , 11 , 30 , SS_CALL_MDL_OUTPUTS ) ;
if ( ssGetTaskTime ( S , 3 ) >= _rtB -> B_11_33_0 ) { rtb_B_390_16_0 =
rtb_B_390_784_0 - rtb_B_390_16_0 ; _rtB -> B_11_37_0 = ( ( _rtB -> B_11_11_0
[ 1 ] - _rtDW -> UnitDelay_DSTATE_k ) * rtb_B_390_16_0 * _rtP -> P_19 + _rtB
-> B_11_11_0 [ 1 ] ) * ( rtb_B_390_16_0 / rtb_B_390_784_0 ) + ( _rtB ->
B_11_25_0 - _rtB -> B_11_30_0 ) * rtb_B_390_253_0 ; } else { _rtB ->
B_11_37_0 = _rtDW -> UnitDelay1_DSTATE_b ; } rtb_B_390_784_0 =
muDoubleScalarHypot ( _rtB -> B_11_24_0 , _rtB -> B_11_37_0 ) ; if (
rtb_B_390_784_0 > _rtP -> P_58 ) { rtb_B_390_784_0 = _rtP -> P_58 ; } else {
if ( rtb_B_390_784_0 < _rtP -> P_59 ) { rtb_B_390_784_0 = _rtP -> P_59 ; } }
_rtB -> B_11_41_0 = 1.0 / rtb_B_390_784_0 ; B_11_42_0 = _rtP -> P_60 *
muDoubleScalarAtan2 ( _rtB -> B_11_37_0 , _rtB -> B_11_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_c ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_881 [ isHit + 6 ] * _rtB ->
B_390_591_0 [ 2 ] + ( _rtP -> P_881 [ isHit + 3 ] * _rtB -> B_390_591_0 [ 1 ]
+ _rtP -> P_881 [ isHit ] * _rtB -> B_390_591_0 [ 0 ] ) ; } B_390_817_0 [ 0 ]
= _rtP -> P_882 * tmp [ 0 ] ; B_390_817_0 [ 1 ] = _rtP -> P_882 * tmp [ 1 ] ;
B_390_817_0 [ 2 ] = _rtP -> P_882 * tmp [ 2 ] ; _rtB -> B_390_821_0 = _rtB ->
B_390_820_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_390_821_0 , &
B_390_817_0 [ 0 ] , _rtB -> B_390_12_0 , & _rtB -> Subsystem1_p , & _rtDW ->
Subsystem1_p ) ; _rtB -> B_390_825_0 = _rtB -> B_390_824_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_825_0 , & B_390_817_0
[ 0 ] , _rtB -> B_390_12_0 , & _rtB -> Subsystempi2delay_d , & _rtDW ->
Subsystempi2delay_d ) ; if ( _rtB -> B_390_820_0 != 0 ) { _rtB -> B_390_827_0
[ 0 ] = _rtB -> Subsystem1_p . B_10_0_0 ; _rtB -> B_390_827_0 [ 1 ] = _rtB ->
Subsystem1_p . B_10_1_0 ; } else { _rtB -> B_390_827_0 [ 0 ] = _rtB ->
Subsystempi2delay_d . B_9_0_0 ; _rtB -> B_390_827_0 [ 1 ] = _rtB ->
Subsystempi2delay_d . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE != 0 ) {
_rtB -> B_390_828_0 = _rtDW -> Integ4_DSTATE ; } else { _rtB -> B_390_828_0 =
_rtP -> P_886 * _rtB -> B_390_827_0 [ 1 ] + _rtDW -> Integ4_DSTATE ; } if (
rtb_B_390_253_0 > _rtP -> P_888 ) { rtb_B_390_784_0 = _rtP -> P_888 ; } else
if ( rtb_B_390_253_0 < _rtP -> P_889 ) { rtb_B_390_784_0 = _rtP -> P_889 ; }
else { rtb_B_390_784_0 = rtb_B_390_253_0 ; } rtb_B_390_784_0 = 1.0 /
rtb_B_390_784_0 / 5.0e-5 ; rtb_B_390_16_0 = muDoubleScalarCeil (
rtb_B_390_784_0 ) ; _rtB -> B_390_832_0 = _rtP -> P_890 * rtb_B_390_16_0 ;
ssCallAccelRunBlock ( S , 390 , 833 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_390_836_0 ) { rtb_B_390_16_0 =
rtb_B_390_784_0 - rtb_B_390_16_0 ; _rtB -> B_390_840_0 = ( ( _rtB ->
B_390_827_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_i ) * rtb_B_390_16_0 * _rtP ->
P_61 + _rtB -> B_390_827_0 [ 1 ] ) * ( rtb_B_390_16_0 / rtb_B_390_784_0 ) + (
_rtB -> B_390_828_0 - _rtB -> B_390_833_0 ) * rtb_B_390_253_0 ; } else { _rtB
-> B_390_840_0 = _rtDW -> UnitDelay1_DSTATE_i ; } _rtB -> B_390_841_0 = _rtB
-> B_390_840_0 * _rtB -> B_11_41_0 ; _rtDW -> DiscreteDerivative_tmp = ( _rtB
-> B_390_841_0 - _rtP -> P_903 [ 1 ] * _rtDW -> DiscreteDerivative_states ) /
_rtP -> P_903 [ 0 ] ; rtb_B_390_784_0 = ( _rtP -> P_909 * _rtB -> B_390_841_0
+ _rtDW -> DiscreteTimeIntegrator_DSTATE_c ) + ( _rtP -> P_902 [ 0 ] * _rtDW
-> DiscreteDerivative_tmp + _rtP -> P_902 [ 1 ] * _rtDW ->
DiscreteDerivative_states ) ; if ( rtb_B_390_784_0 > _rtP -> P_910 ) { _rtB
-> B_390_846_0 = _rtP -> P_910 ; } else if ( rtb_B_390_784_0 < _rtP -> P_911
) { _rtB -> B_390_846_0 = _rtP -> P_911 ; } else { _rtB -> B_390_846_0 =
rtb_B_390_784_0 ; } rtb_B_390_253_0 = _rtP -> P_912 * _rtB -> B_390_846_0 ;
rtb_B_390_16_0 = rtb_B_390_253_0 - _rtDW -> PrevY ; if ( rtb_B_390_16_0 >
_rtP -> P_913 ) { rtb_B_390_253_0 = _rtDW -> PrevY + _rtP -> P_913 ; } else {
if ( rtb_B_390_16_0 < _rtP -> P_914 ) { rtb_B_390_253_0 = _rtDW -> PrevY +
_rtP -> P_914 ; } } _rtDW -> PrevY = rtb_B_390_253_0 ; _rtB -> B_390_858_0 =
( _rtP -> P_917 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_919 * _rtDW ->
Delay_x2_DSTATE ) + _rtP -> P_922 * rtb_B_390_253_0 ; _rtB -> B_390_860_0 = (
_rtP -> P_920 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_921 * _rtDW ->
Delay_x2_DSTATE ) + _rtP -> P_923 * rtb_B_390_253_0 ; _rtB -> B_390_865_0 = (
_rtP -> P_925 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_926 * _rtDW ->
Delay_x2_DSTATE ) + _rtP -> P_924 * rtb_B_390_253_0 ; } if ( _rtB ->
B_390_17_0 > _rtP -> P_928 ) { _rtB -> B_390_867_0 = _rtB -> B_390_740_0 ; }
else { _rtB -> B_390_867_0 = _rtB -> B_390_715_0 ; } if ( _rtB -> B_390_17_0
> _rtP -> P_929 ) { _rtB -> B_390_868_0 = _rtB -> B_390_733_0 ; } else { _rtB
-> B_390_868_0 = _rtB -> B_390_715_1 ; } _rtB -> B_390_872_0 = (
muDoubleScalarSin ( _rtP -> P_934 * ssGetTaskTime ( S , 0 ) + _rtP -> P_935 )
* _rtP -> P_932 + _rtP -> P_933 ) * _rtB -> B_390_591_0 [ 0 ] ; _rtB ->
B_390_874_0 = ( muDoubleScalarSin ( _rtP -> P_938 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_939 ) * _rtP -> P_936 + _rtP -> P_937 ) * _rtB -> B_390_591_0 [ 0 ]
; _rtB -> B_390_876_0 = ( muDoubleScalarSin ( _rtP -> P_942 * ssGetTaskTime (
S , 0 ) + _rtP -> P_943 ) * _rtP -> P_940 + _rtP -> P_941 ) * _rtB ->
B_390_591_0 [ 1 ] ; _rtB -> B_390_878_0 = ( muDoubleScalarSin ( _rtP -> P_946
* ssGetTaskTime ( S , 0 ) + _rtP -> P_947 ) * _rtP -> P_944 + _rtP -> P_945 )
* _rtB -> B_390_591_0 [ 1 ] ; _rtB -> B_390_880_0 = ( muDoubleScalarSin (
_rtP -> P_950 * ssGetTaskTime ( S , 0 ) + _rtP -> P_951 ) * _rtP -> P_948 +
_rtP -> P_949 ) * _rtB -> B_390_591_0 [ 2 ] ; _rtB -> B_390_882_0 = (
muDoubleScalarSin ( _rtP -> P_954 * ssGetTaskTime ( S , 0 ) + _rtP -> P_955 )
* _rtP -> P_952 + _rtP -> P_953 ) * _rtB -> B_390_591_0 [ 2 ] ; B_390_883_0 =
_rtP -> P_956 * rtb_B_390_32_0 [ 0 ] ; _rtB -> B_390_885_0 = (
muDoubleScalarSin ( _rtP -> P_959 * ssGetTaskTime ( S , 0 ) + _rtP -> P_960 )
* _rtP -> P_957 + _rtP -> P_958 ) * _rtB -> B_390_658_0 [ 0 ] ; _rtB ->
B_390_887_0 = ( muDoubleScalarSin ( _rtP -> P_963 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_964 ) * _rtP -> P_961 + _rtP -> P_962 ) * _rtB -> B_390_658_0 [ 0 ]
; _rtB -> B_390_889_0 = ( muDoubleScalarSin ( _rtP -> P_967 * ssGetTaskTime (
S , 0 ) + _rtP -> P_968 ) * _rtP -> P_965 + _rtP -> P_966 ) * _rtB ->
B_390_658_0 [ 1 ] ; _rtB -> B_390_891_0 = ( muDoubleScalarSin ( _rtP -> P_971
* ssGetTaskTime ( S , 0 ) + _rtP -> P_972 ) * _rtP -> P_969 + _rtP -> P_970 )
* _rtB -> B_390_658_0 [ 1 ] ; _rtB -> B_390_893_0 = ( muDoubleScalarSin (
_rtP -> P_975 * ssGetTaskTime ( S , 0 ) + _rtP -> P_976 ) * _rtP -> P_973 +
_rtP -> P_974 ) * _rtB -> B_390_658_0 [ 2 ] ; _rtB -> B_390_895_0 = (
muDoubleScalarSin ( _rtP -> P_979 * ssGetTaskTime ( S , 0 ) + _rtP -> P_980 )
* _rtP -> P_977 + _rtP -> P_978 ) * _rtB -> B_390_658_0 [ 2 ] ; B_390_896_0 =
_rtP -> P_981 * B_390_401_1_idx_0 ; _rtB -> B_390_583_0 [ 0 ] = _rtB ->
B_390_414_0 ; _rtB -> B_390_583_0 [ 1 ] = _rtB -> B_390_423_0 ; _rtB ->
B_390_583_0 [ 2 ] = _rtB -> B_390_415_0 ; _rtB -> B_390_583_0 [ 3 ] = _rtB ->
B_390_497_0 ; _rtB -> B_390_583_0 [ 4 ] = _rtB -> B_390_571_0 ; _rtB ->
B_390_583_0 [ 5 ] = _rtB -> B_390_421_0 ; _rtB -> B_390_583_0 [ 6 ] = _rtB ->
B_52_0_1 ; _rtB -> B_390_583_0 [ 7 ] = _rtB -> B_52_0_2 ; _rtB -> B_390_583_0
[ 8 ] = 0.0 ; _rtB -> B_390_583_0 [ 9 ] = 0.0 ; _rtB -> B_390_583_0 [ 10 ] =
0.0 ; _rtB -> B_390_583_0 [ 11 ] = 0.0 ; ssCallAccelRunBlock ( S , 390 , 939
, SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 390 , 941 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 390 , 942 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_947_0 [ 0 ] = _rtP -> P_984 * _rtB -> B_0_14_0 [ 47 ] *
_rtP -> P_987 ; _rtB -> B_390_947_0 [ 1 ] = _rtP -> P_985 * _rtB -> B_0_14_0
[ 48 ] * _rtP -> P_987 ; _rtB -> B_390_947_0 [ 2 ] = _rtP -> P_986 * _rtB ->
B_0_14_0 [ 49 ] * _rtP -> P_987 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_988 [ isHit + 6 ] * _rtB -> B_390_947_0 [ 2 ] + (
_rtP -> P_988 [ isHit + 3 ] * _rtB -> B_390_947_0 [ 1 ] + _rtP -> P_988 [
isHit ] * _rtB -> B_390_947_0 [ 0 ] ) ; } _rtB -> B_390_949_0 [ 0 ] = _rtP ->
P_989 * tmp [ 0 ] ; _rtB -> B_390_949_0 [ 1 ] = _rtP -> P_989 * tmp [ 1 ] ;
_rtB -> B_390_949_0 [ 2 ] = _rtP -> P_989 * tmp [ 2 ] ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_953_0 = _rtB
-> B_390_952_0 ; } MR_CIGRE_RESISTIVO_Subsystem1 ( S , _rtB -> B_390_953_0 ,
& _rtB -> B_390_949_0 [ 0 ] , _rtB -> B_390_54_0 , & _rtB -> Subsystem1_k , &
_rtDW -> Subsystem1_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_957_0 = _rtB -> B_390_956_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_957_0 , & _rtB ->
B_390_949_0 [ 0 ] , _rtB -> B_390_54_0 , & _rtB -> Subsystempi2delay_ac , &
_rtDW -> Subsystempi2delay_ac ) ; if ( _rtB -> B_390_952_0 != 0 ) { _rtB ->
B_390_959_0 [ 0 ] = _rtB -> Subsystem1_k . B_2_0_0 ; _rtB -> B_390_959_0 [ 1
] = _rtB -> Subsystem1_k . B_2_1_0 ; } else { _rtB -> B_390_959_0 [ 0 ] =
_rtB -> Subsystempi2delay_ac . B_1_0_0 ; _rtB -> B_390_959_0 [ 1 ] = _rtB ->
Subsystempi2delay_ac . B_1_1_0 ; } _rtB -> B_390_960_0 = 0.0 ; _rtB ->
B_390_960_0 += _rtP -> P_994 * _rtX -> Integradord_CSTATE_a ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_995 *
_rtB -> B_0_14_0 [ 99 ] * _rtP -> P_998 ; tmp_1 = _rtP -> P_996 * _rtB ->
B_0_14_0 [ 100 ] * _rtP -> P_998 ; rtb_B_390_16_0 = _rtP -> P_997 * _rtB ->
B_0_14_0 [ 101 ] * _rtP -> P_998 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_999 [ isHit + 6 ] * rtb_B_390_16_0 + ( _rtP ->
P_999 [ isHit + 3 ] * tmp_1 + _rtP -> P_999 [ isHit ] * tmp_0 ) ; } _rtB ->
B_390_966_0 [ 0 ] = _rtP -> P_1000 * tmp [ 0 ] ; _rtB -> B_390_966_0 [ 1 ] =
_rtP -> P_1000 * tmp [ 1 ] ; _rtB -> B_390_966_0 [ 2 ] = _rtP -> P_1000 * tmp
[ 2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_970_0 = _rtB -> B_390_969_0 ; } MR_CIGRE_RESISTIVO_Subsystem1 ( S ,
_rtB -> B_390_970_0 , & _rtB -> B_390_966_0 [ 0 ] , _rtB -> B_390_54_0 , &
_rtB -> Subsystem1_h , & _rtDW -> Subsystem1_h ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_974_0 = _rtB -> B_390_973_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_974_0 , & _rtB ->
B_390_966_0 [ 0 ] , _rtB -> B_390_54_0 , & _rtB -> Subsystempi2delay_k , &
_rtDW -> Subsystempi2delay_k ) ; if ( _rtB -> B_390_969_0 != 0 ) { _rtB ->
B_390_976_0 [ 0 ] = _rtB -> Subsystem1_h . B_2_0_0 ; _rtB -> B_390_976_0 [ 1
] = _rtB -> Subsystem1_h . B_2_1_0 ; } else { _rtB -> B_390_976_0 [ 0 ] =
_rtB -> Subsystempi2delay_k . B_1_0_0 ; _rtB -> B_390_976_0 [ 1 ] = _rtB ->
Subsystempi2delay_k . B_1_1_0 ; } _rtB -> B_390_977_0 = 0.0 ; _rtB ->
B_390_977_0 += _rtP -> P_1005 * _rtX -> LPF4_CSTATE_f ; _rtB -> B_390_978_0 =
_rtB -> B_390_977_0 - _rtB -> B_390_976_0 [ 0 ] ; _rtB -> B_390_979_0 = _rtP
-> P_1006 * _rtB -> B_390_978_0 ; _rtB -> B_390_980_0 = _rtB -> B_390_960_0 +
_rtB -> B_390_979_0 ; _rtB -> B_390_981_0 = _rtB -> B_390_423_0 * _rtB ->
B_390_976_0 [ 1 ] ; _rtB -> B_390_982_0 = _rtP -> P_1007 * _rtB ->
B_390_981_0 ; _rtB -> B_390_983_0 = ( _rtB -> B_390_959_0 [ 0 ] + _rtB ->
B_390_980_0 ) - _rtB -> B_390_982_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_390_985_0 = _rtDW -> UnitDelay1_DSTATE_a / _rtB
-> B_390_943_0 ; } _rtB -> B_390_986_0 = _rtB -> B_390_983_0 / _rtB ->
B_390_985_0 ; _rtB -> B_390_987_0 = _rtB -> B_390_976_0 [ 0 ] * _rtB ->
B_390_423_0 ; _rtB -> B_390_988_0 = _rtP -> P_1009 * _rtB -> B_390_987_0 ;
_rtB -> B_390_989_0 = 0.0 ; _rtB -> B_390_989_0 += _rtP -> P_1011 * _rtX ->
LPF2_CSTATE_a ; _rtB -> B_390_990_0 = _rtB -> B_390_989_0 - _rtB ->
B_390_976_0 [ 1 ] ; _rtB -> B_390_991_0 = _rtP -> P_1012 * _rtB ->
B_390_990_0 ; _rtB -> B_390_992_0 = 0.0 ; _rtB -> B_390_992_0 += _rtP ->
P_1014 * _rtX -> Integradord1_CSTATE_n ; _rtB -> B_390_993_0 = _rtB ->
B_390_991_0 + _rtB -> B_390_992_0 ; _rtB -> B_390_994_0 = ( _rtB ->
B_390_988_0 + _rtB -> B_390_993_0 ) + _rtB -> B_390_959_0 [ 1 ] ; _rtB ->
B_390_995_0 = 1.0 / _rtB -> B_390_985_0 * _rtB -> B_390_994_0 ; _rtB ->
B_390_996_0 = _rtB -> B_390_39_0 - _rtB -> B_390_45_0 ; _rtB -> B_390_997_0 =
_rtP -> P_1015 * _rtB -> B_390_996_0 ; _rtB -> B_390_998_0 = _rtB ->
B_390_997_0 * _rtB -> B_390_985_0 ; _rtB -> B_390_999_0 = _rtB -> B_390_40_0
- _rtB -> B_390_45_1 ; _rtB -> B_390_1000_0 = _rtP -> P_1016 * _rtB ->
B_390_999_0 ; _rtB -> B_390_1001_0 = _rtB -> B_390_985_0 * _rtB ->
B_390_1000_0 ; _rtB -> B_390_1002_0 = _rtB -> B_390_978_0 - _rtB ->
B_390_998_0 ; _rtB -> B_390_1003_0 = _rtB -> B_390_990_0 - _rtB ->
B_390_1001_0 ; _rtB -> B_390_1004_0 = _rtB -> B_390_414_0 - _rtB -> B_52_0_1
; _rtB -> B_390_1005_0 = _rtP -> P_1017 * _rtB -> B_390_1004_0 ; _rtB ->
B_390_1006_0 = _rtP -> P_1018 * _rtB -> B_390_1005_0 ; isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_390_16_0 = ssGetTaskTime ( S , 1 )
; _rtDW -> ref2_MODE_l = ( rtb_B_390_16_0 >= _rtP -> P_1019 ) ; if ( _rtDW ->
ref2_MODE_l == 1 ) { _rtB -> B_390_1008_0 = _rtP -> P_1021 ; } else { _rtB ->
B_390_1008_0 = _rtP -> P_1020 ; } } _rtB -> B_390_1009_0 = _rtB ->
B_390_1008_0 - _rtB -> B_390_1006_0 ; _rtB -> B_390_1010_0 = _rtP -> P_1022 *
_rtB -> B_390_1009_0 ; _rtB -> B_390_1011_0 = _rtB -> B_390_959_0 [ 0 ] *
_rtB -> B_390_423_0 ; _rtB -> B_390_1012_0 = _rtP -> P_1023 * _rtB ->
B_390_1011_0 ; _rtB -> B_390_1013_0 = _rtB -> B_390_423_0 * _rtB ->
B_390_959_0 [ 1 ] ; _rtB -> B_390_1014_0 = _rtP -> P_1024 * _rtB ->
B_390_1013_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_390_1018_0 [ 0 ] = _rtP -> P_1025 * _rtB -> B_0_14_0 [ 102 ] * _rtP ->
P_1028 ; _rtB -> B_390_1018_0 [ 1 ] = _rtP -> P_1026 * _rtB -> B_0_14_0 [ 103
] * _rtP -> P_1028 ; _rtB -> B_390_1018_0 [ 2 ] = _rtP -> P_1027 * _rtB ->
B_0_14_0 [ 104 ] * _rtP -> P_1028 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_1029 [ isHit + 6 ] * _rtB -> B_390_1018_0 [ 2 ] +
( _rtP -> P_1029 [ isHit + 3 ] * _rtB -> B_390_1018_0 [ 1 ] + _rtP -> P_1029
[ isHit ] * _rtB -> B_390_1018_0 [ 0 ] ) ; } _rtB -> B_390_1020_0 [ 0 ] =
_rtP -> P_1030 * tmp [ 0 ] ; _rtB -> B_390_1020_0 [ 1 ] = _rtP -> P_1030 *
tmp [ 1 ] ; _rtB -> B_390_1020_0 [ 2 ] = _rtP -> P_1030 * tmp [ 2 ] ; } isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1024_0 =
_rtB -> B_390_1023_0 ; } MR_CIGRE_RESISTIVO_Subsystem1 ( S , _rtB ->
B_390_1024_0 , & _rtB -> B_390_1020_0 [ 0 ] , _rtB -> B_390_54_0 , & _rtB ->
Subsystem1_bc , & _rtDW -> Subsystem1_bc ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1028_0 = _rtB -> B_390_1027_0 ; }
MR_CIGRE_RESISTIVO_Subsystempi2delay ( S , _rtB -> B_390_1028_0 , & _rtB ->
B_390_1020_0 [ 0 ] , _rtB -> B_390_54_0 , & _rtB -> Subsystempi2delay_g , &
_rtDW -> Subsystempi2delay_g ) ; if ( _rtB -> B_390_1023_0 != 0 ) { _rtB ->
B_390_1030_0 [ 0 ] = _rtB -> Subsystem1_bc . B_2_0_0 ; _rtB -> B_390_1030_0 [
1 ] = _rtB -> Subsystem1_bc . B_2_1_0 ; } else { _rtB -> B_390_1030_0 [ 0 ] =
_rtB -> Subsystempi2delay_g . B_1_0_0 ; _rtB -> B_390_1030_0 [ 1 ] = _rtB ->
Subsystempi2delay_g . B_1_1_0 ; } _rtB -> B_390_1031_0 = _rtP -> P_1034 *
_rtB -> B_390_1030_0 [ 0 ] ; _rtB -> B_390_1032_0 = 0.0 ; _rtB ->
B_390_1032_0 += _rtP -> P_1036 * _rtX -> Integradord_CSTATE_n ; _rtB ->
B_390_1033_0 = _rtP -> P_1037 * _rtB -> B_390_1030_0 [ 0 ] ; _rtB ->
B_390_1034_0 = 0.0 ; _rtB -> B_390_1034_0 += _rtP -> P_1039 * _rtX ->
HPF1_CSTATE_k ; _rtB -> B_390_1034_0 += _rtP -> P_1040 * _rtB -> B_390_1033_0
; _rtB -> B_390_1035_0 = _rtP -> P_1041 * _rtB -> B_390_1030_0 [ 0 ] ; _rtB
-> B_390_1036_0 = _rtB -> B_390_1034_0 + _rtB -> B_390_1035_0 ; _rtB ->
B_390_1037_0 = _rtP -> P_1042 * _rtB -> B_390_1030_0 [ 1 ] ; _rtB ->
B_390_1038_0 = _rtB -> B_390_1037_0 * _rtB -> B_390_423_0 ; _rtB ->
B_390_1039_0 = _rtB -> B_390_1036_0 + _rtB -> B_390_1038_0 ; _rtB ->
B_390_1040_0 = _rtB -> B_390_1010_0 - _rtB -> B_390_1039_0 ; _rtB ->
B_390_1041_0 = _rtB -> B_390_1040_0 - _rtB -> B_390_959_0 [ 0 ] ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_52_0 >
_rtP -> P_1045 ) { _rtB -> B_390_1044_0 = _rtB -> B_390_1042_0 ; } else {
_rtB -> B_390_1044_0 = _rtB -> B_390_1043_0 ; } } _rtB -> B_390_1045_0 = _rtB
-> B_390_1041_0 * _rtB -> B_390_1044_0 ; _rtB -> B_390_1046_0 = _rtP ->
P_1046 * _rtB -> B_390_1045_0 ; _rtB -> B_390_1047_0 = _rtB -> B_390_1032_0 +
_rtB -> B_390_1046_0 ; _rtB -> B_390_1048_0 = ( _rtB -> B_390_1031_0 + _rtB
-> B_390_1047_0 ) - _rtB -> B_390_1014_0 ; _rtB -> B_390_1049_0 = _rtB ->
B_390_1048_0 * _rtB -> B_390_1044_0 ; _rtB -> B_390_1051_0 = _rtP -> P_1048 *
_rtB -> B_390_1030_0 [ 0 ] ; _rtB -> B_390_1052_0 = _rtB -> B_390_423_0 *
_rtB -> B_390_1051_0 ; _rtB -> B_390_1053_0 = _rtP -> P_1049 * _rtB ->
B_390_1030_0 [ 1 ] ; _rtB -> B_390_1054_0 = 0.0 ; _rtB -> B_390_1054_0 +=
_rtP -> P_1051 * _rtX -> HPF2_CSTATE_g ; _rtB -> B_390_1054_0 += _rtP ->
P_1052 * _rtB -> B_390_1053_0 ; _rtB -> B_390_1055_0 = _rtP -> P_1053 * _rtB
-> B_390_1030_0 [ 1 ] ; _rtB -> B_390_1056_0 = _rtB -> B_390_1054_0 + _rtB ->
B_390_1055_0 ; _rtB -> B_390_1057_0 = _rtB -> B_390_1052_0 + _rtB ->
B_390_1056_0 ; _rtB -> B_390_1058_0 = _rtB -> B_390_1050_0 - _rtB ->
B_390_1057_0 ; _rtB -> B_390_1059_0 = _rtB -> B_390_1058_0 - _rtB ->
B_390_959_0 [ 1 ] ; _rtB -> B_390_1060_0 = _rtB -> B_390_1044_0 * _rtB ->
B_390_1059_0 ; _rtB -> B_390_1061_0 = _rtP -> P_1054 * _rtB -> B_390_1060_0 ;
_rtB -> B_390_1062_0 = 0.0 ; _rtB -> B_390_1062_0 += _rtP -> P_1056 * _rtX ->
Integradorq_CSTATE_a ; _rtB -> B_390_1063_0 = _rtB -> B_390_1061_0 + _rtB ->
B_390_1062_0 ; _rtB -> B_390_1064_0 = _rtP -> P_1057 * _rtB -> B_390_1030_0 [
1 ] ; _rtB -> B_390_1065_0 = ( _rtB -> B_390_1012_0 + _rtB -> B_390_1063_0 )
+ _rtB -> B_390_1064_0 ; _rtB -> B_390_1066_0 = _rtB -> B_390_1044_0 * _rtB
-> B_390_1065_0 ; _rtB -> B_390_1067_0 . re = _rtB -> B_390_1049_0 ; _rtB ->
B_390_1067_0 . im = _rtB -> B_390_1066_0 ; _rtB -> B_390_1068_0 =
muDoubleScalarHypot ( _rtB -> B_390_1067_0 . re , _rtB -> B_390_1067_0 . im )
; rtb_B_390_253_0 = muDoubleScalarAtan2 ( _rtB -> B_390_1067_0 . im , _rtB ->
B_390_1067_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_n = _rtB -> B_390_1068_0 >= _rtP -> P_1058 ? 1 : _rtB ->
B_390_1068_0 > _rtP -> P_1059 ? 0 : - 1 ; } _rtB -> B_390_1069_0 = _rtDW ->
Saturation_MODE_n == 1 ? _rtP -> P_1058 : _rtDW -> Saturation_MODE_n == - 1 ?
_rtP -> P_1059 : _rtB -> B_390_1068_0 ; _rtB -> B_390_1071_0 = _rtB ->
B_390_1069_0 * muDoubleScalarCos ( rtb_B_390_253_0 ) ; _rtB -> B_390_1071_1 =
_rtB -> B_390_1069_0 * muDoubleScalarSin ( rtb_B_390_253_0 ) ; _rtB ->
B_390_1072_0 = _rtB -> B_390_1049_0 - _rtB -> B_390_1071_0 ; _rtB ->
B_390_1073_0 = _rtP -> P_1060 * _rtB -> B_390_1072_0 ; _rtB -> B_390_1074_0 =
_rtB -> B_390_1045_0 - _rtB -> B_390_1073_0 ; _rtB -> B_390_1075_0 = _rtP ->
P_1061 * _rtB -> B_390_1074_0 ; _rtB -> B_390_1076_0 = _rtB -> B_390_1066_0 -
_rtB -> B_390_1071_1 ; _rtB -> B_390_1077_0 = _rtP -> P_1062 * _rtB ->
B_390_1076_0 ; _rtB -> B_390_1078_0 = _rtB -> B_390_1060_0 - _rtB ->
B_390_1077_0 ; _rtB -> B_390_1079_0 = _rtP -> P_1063 * _rtB -> B_390_1078_0 ;
_rtB -> B_390_1080_0 = _rtB -> B_390_959_0 [ 1 ] * _rtB -> B_390_959_0 [ 1 ]
; _rtB -> B_390_1081_0 = _rtB -> B_390_959_0 [ 0 ] * _rtB -> B_390_959_0 [ 0
] ; if ( _rtB -> B_390_52_0 > _rtP -> P_1064 ) { _rtB -> B_390_1082_0 = _rtB
-> B_390_423_0 ; } else { _rtB -> B_390_1082_0 = _rtB -> B_390_421_0 ; }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_390_1086_0 [ 0 ]
= _rtP -> P_1065 * _rtB -> B_0_14_0 [ 44 ] * _rtP -> P_1068 ; B_390_1086_0 [
1 ] = _rtP -> P_1066 * _rtB -> B_0_14_0 [ 45 ] * _rtP -> P_1068 ;
B_390_1086_0 [ 2 ] = _rtP -> P_1067 * _rtB -> B_0_14_0 [ 46 ] * _rtP ->
P_1068 ; rtb_B_390_1124_0 = _rtP -> P_1070 * _rtB -> B_390_68_0 - _rtDW ->
prediction_DSTATE_l ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1128_0 = rtb_B_390_1124_0 + _rtB -> B_390_1121_0 ; } _rtB
-> B_390_1129_0 = ( _rtB -> B_390_71_0 * _rtB -> B_0_14_0 [ 150 ] - _rtB ->
B_390_72_0 * _rtB -> B_0_14_0 [ 151 ] ) / _rtB -> B_390_1128_0 ; _rtB ->
B_390_1144_0 = _rtB -> B_390_959_0 [ 0 ] * _rtB -> B_390_976_0 [ 0 ] ; _rtB
-> B_390_1145_0 = _rtB -> B_390_959_0 [ 1 ] * _rtB -> B_390_976_0 [ 1 ] ;
_rtB -> B_390_1146_0 = _rtB -> B_390_1144_0 + _rtB -> B_390_1145_0 ; _rtB ->
B_390_1147_0 = _rtP -> P_1072 * _rtB -> B_390_1146_0 ; _rtB -> B_390_1148_0 =
_rtB -> B_390_959_0 [ 0 ] * _rtB -> B_390_976_0 [ 1 ] ; _rtB -> B_390_1149_0
= _rtB -> B_390_976_0 [ 0 ] * _rtB -> B_390_959_0 [ 1 ] ; _rtB ->
B_390_1150_0 = _rtB -> B_390_1149_0 - _rtB -> B_390_1148_0 ; _rtB ->
B_390_1151_0 = _rtP -> P_1073 * _rtB -> B_390_1150_0 ; isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1153_0 = _rtP -> P_1074 ;
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( _rtB -> B_390_1153_0 > 0.0 ) { if ( !
_rtDW -> AutomaticGainControl_MODE_i ) { if ( ssGetTaskTime ( S , 3 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
_rtDW -> Integ4_SYSTEM_ENABLE_lr = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_c = 1U
; _rtDW -> AutomaticGainControl_MODE_i = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE_i ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> AutomaticGainControl_MODE_i = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_i ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_77 [ isHit + 6 ] * _rtB -> B_390_947_0 [ 2 ] + ( _rtP -> P_77 [
isHit + 3 ] * _rtB -> B_390_947_0 [ 1 ] + _rtP -> P_77 [ isHit ] * _rtB ->
B_390_947_0 [ 0 ] ) ; } B_46_1_0 [ 0 ] = _rtP -> P_78 * tmp [ 0 ] ; B_46_1_0
[ 1 ] = _rtP -> P_78 * tmp [ 1 ] ; B_46_1_0 [ 2 ] = _rtP -> P_78 * tmp [ 2 ]
; _rtB -> B_46_5_0 = _rtB -> B_46_4_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b (
_rtB -> B_46_5_0 , & B_46_1_0 [ 0 ] , _rtB -> B_390_50_0 , & _rtB ->
Subsystem1_hy , & _rtDW -> Subsystem1_hy ) ; _rtB -> B_46_9_0 = _rtB ->
B_46_8_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_46_9_0 , &
B_46_1_0 [ 0 ] , _rtB -> B_390_50_0 , & _rtB -> Subsystempi2delay_ck , &
_rtDW -> Subsystempi2delay_ck ) ; if ( _rtB -> B_46_4_0 != 0 ) { _rtB ->
B_46_11_0 [ 0 ] = _rtB -> Subsystem1_hy . B_10_0_0 ; _rtB -> B_46_11_0 [ 1 ]
= _rtB -> Subsystem1_hy . B_10_1_0 ; } else { _rtB -> B_46_11_0 [ 0 ] = _rtB
-> Subsystempi2delay_ck . B_9_0_0 ; _rtB -> B_46_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_ck . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_lr != 0
) { _rtB -> B_46_12_0 = _rtDW -> Integ4_DSTATE_a ; } else { _rtB -> B_46_12_0
= _rtP -> P_82 * _rtB -> B_46_11_0 [ 0 ] + _rtDW -> Integ4_DSTATE_a ; } if (
rtb_B_390_422_0 > _rtP -> P_84 ) { B_390_401_1_idx_0 = _rtP -> P_84 ; } else
if ( rtb_B_390_422_0 < _rtP -> P_85 ) { B_390_401_1_idx_0 = _rtP -> P_85 ; }
else { B_390_401_1_idx_0 = rtb_B_390_422_0 ; } rtb_B_390_1124_0 = 1.0 /
B_390_401_1_idx_0 / 5.0e-5 ; rtb_B_390_253_0 = muDoubleScalarCeil (
rtb_B_390_1124_0 ) ; _rtB -> B_46_16_0 = _rtP -> P_86 * rtb_B_390_253_0 ;
ssCallAccelRunBlock ( S , 46 , 17 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_46_20_0 ) { rtb_B_390_253_0 =
rtb_B_390_1124_0 - rtb_B_390_253_0 ; _rtB -> B_46_24_0 = ( ( _rtB ->
B_46_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_oc ) * rtb_B_390_253_0 * _rtP ->
P_74 + _rtB -> B_46_11_0 [ 0 ] ) * ( rtb_B_390_253_0 / rtb_B_390_1124_0 ) + (
_rtB -> B_46_12_0 - _rtB -> B_46_17_0 ) * rtb_B_390_422_0 ; } else { _rtB ->
B_46_24_0 = _rtDW -> UnitDelay1_DSTATE_c ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_c != 0 ) { _rtB -> B_46_25_0 = _rtDW -> Integ4_DSTATE_am
; } else { _rtB -> B_46_25_0 = _rtP -> P_98 * _rtB -> B_46_11_0 [ 1 ] + _rtDW
-> Integ4_DSTATE_am ; } if ( rtb_B_390_422_0 > _rtP -> P_100 ) {
B_390_401_1_idx_0 = _rtP -> P_100 ; } else if ( rtb_B_390_422_0 < _rtP ->
P_101 ) { B_390_401_1_idx_0 = _rtP -> P_101 ; } else { B_390_401_1_idx_0 =
rtb_B_390_422_0 ; } rtb_B_390_1124_0 = 1.0 / B_390_401_1_idx_0 / 5.0e-5 ;
rtb_B_390_253_0 = muDoubleScalarCeil ( rtb_B_390_1124_0 ) ; _rtB -> B_46_29_0
= _rtP -> P_102 * rtb_B_390_253_0 ; ssCallAccelRunBlock ( S , 46 , 30 ,
SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB -> B_46_33_0 ) {
rtb_B_390_253_0 = rtb_B_390_1124_0 - rtb_B_390_253_0 ; _rtB -> B_46_37_0 = (
( _rtB -> B_46_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_bg ) * rtb_B_390_253_0
* _rtP -> P_75 + _rtB -> B_46_11_0 [ 1 ] ) * ( rtb_B_390_253_0 /
rtb_B_390_1124_0 ) + ( _rtB -> B_46_25_0 - _rtB -> B_46_30_0 ) *
rtb_B_390_422_0 ; } else { _rtB -> B_46_37_0 = _rtDW -> UnitDelay1_DSTATE_o0
; } rtb_B_390_784_0 = muDoubleScalarHypot ( _rtB -> B_46_24_0 , _rtB ->
B_46_37_0 ) ; if ( rtb_B_390_784_0 > _rtP -> P_114 ) { rtb_B_390_784_0 = _rtP
-> P_114 ; } else { if ( rtb_B_390_784_0 < _rtP -> P_115 ) { rtb_B_390_784_0
= _rtP -> P_115 ; } } _rtB -> B_46_41_0 = 1.0 / rtb_B_390_784_0 ; B_46_42_0 =
_rtP -> P_116 * muDoubleScalarAtan2 ( _rtB -> B_46_37_0 , _rtB -> B_46_24_0 )
; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_l ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_1075 [ isHit + 6 ] * _rtB ->
B_390_947_0 [ 2 ] + ( _rtP -> P_1075 [ isHit + 3 ] * _rtB -> B_390_947_0 [ 1
] + _rtP -> P_1075 [ isHit ] * _rtB -> B_390_947_0 [ 0 ] ) ; } B_390_1156_0 [
0 ] = _rtP -> P_1076 * tmp [ 0 ] ; B_390_1156_0 [ 1 ] = _rtP -> P_1076 * tmp
[ 1 ] ; B_390_1156_0 [ 2 ] = _rtP -> P_1076 * tmp [ 2 ] ; _rtB ->
B_390_1160_0 = _rtB -> B_390_1159_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB
-> B_390_1160_0 , & B_390_1156_0 [ 0 ] , _rtB -> B_390_50_0 , & _rtB ->
Subsystem1_j , & _rtDW -> Subsystem1_j ) ; _rtB -> B_390_1164_0 = _rtB ->
B_390_1163_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_1164_0
, & B_390_1156_0 [ 0 ] , _rtB -> B_390_50_0 , & _rtB -> Subsystempi2delay_c ,
& _rtDW -> Subsystempi2delay_c ) ; if ( _rtB -> B_390_1159_0 != 0 ) { _rtB ->
B_390_1166_0 [ 0 ] = _rtB -> Subsystem1_j . B_10_0_0 ; _rtB -> B_390_1166_0 [
1 ] = _rtB -> Subsystem1_j . B_10_1_0 ; } else { _rtB -> B_390_1166_0 [ 0 ] =
_rtB -> Subsystempi2delay_c . B_9_0_0 ; _rtB -> B_390_1166_0 [ 1 ] = _rtB ->
Subsystempi2delay_c . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_m != 0 )
{ _rtB -> B_390_1167_0 = _rtDW -> Integ4_DSTATE_b ; } else { _rtB ->
B_390_1167_0 = _rtP -> P_1080 * _rtB -> B_390_1166_0 [ 1 ] + _rtDW ->
Integ4_DSTATE_b ; } if ( rtb_B_390_422_0 > _rtP -> P_1082 ) {
B_390_401_1_idx_0 = _rtP -> P_1082 ; } else if ( rtb_B_390_422_0 < _rtP ->
P_1083 ) { B_390_401_1_idx_0 = _rtP -> P_1083 ; } else { B_390_401_1_idx_0 =
rtb_B_390_422_0 ; } rtb_B_390_1124_0 = 1.0 / B_390_401_1_idx_0 / 5.0e-5 ;
rtb_B_390_253_0 = muDoubleScalarCeil ( rtb_B_390_1124_0 ) ; _rtB ->
B_390_1171_0 = _rtP -> P_1084 * rtb_B_390_253_0 ; ssCallAccelRunBlock ( S ,
390 , 1172 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_390_1175_0 ) { rtb_B_390_253_0 = rtb_B_390_1124_0 - rtb_B_390_253_0 ; _rtB
-> B_390_1179_0 = ( ( _rtB -> B_390_1166_0 [ 1 ] - _rtDW ->
UnitDelay_DSTATE_ob ) * rtb_B_390_253_0 * _rtP -> P_117 + _rtB ->
B_390_1166_0 [ 1 ] ) * ( rtb_B_390_253_0 / rtb_B_390_1124_0 ) + ( _rtB ->
B_390_1167_0 - _rtB -> B_390_1172_0 ) * rtb_B_390_422_0 ; } else { _rtB ->
B_390_1179_0 = _rtDW -> UnitDelay1_DSTATE_k ; } _rtB -> B_390_1180_0 = _rtB
-> B_390_1179_0 * _rtB -> B_46_41_0 ; _rtDW -> DiscreteDerivative_tmp_h = (
_rtB -> B_390_1180_0 - _rtP -> P_1097 [ 1 ] * _rtDW ->
DiscreteDerivative_states_b ) / _rtP -> P_1097 [ 0 ] ; rtb_B_390_784_0 = (
_rtP -> P_1103 * _rtB -> B_390_1180_0 + _rtDW ->
DiscreteTimeIntegrator_DSTATE_ce ) + ( _rtP -> P_1096 [ 0 ] * _rtDW ->
DiscreteDerivative_tmp_h + _rtP -> P_1096 [ 1 ] * _rtDW ->
DiscreteDerivative_states_b ) ; if ( rtb_B_390_784_0 > _rtP -> P_1104 ) {
_rtB -> B_390_1185_0 = _rtP -> P_1104 ; } else if ( rtb_B_390_784_0 < _rtP ->
P_1105 ) { _rtB -> B_390_1185_0 = _rtP -> P_1105 ; } else { _rtB ->
B_390_1185_0 = rtb_B_390_784_0 ; } rtb_B_390_422_0 = _rtP -> P_1106 * _rtB ->
B_390_1185_0 ; rtb_B_390_16_0 = rtb_B_390_422_0 - _rtDW -> PrevY_o ; if (
rtb_B_390_16_0 > _rtP -> P_1107 ) { rtb_B_390_422_0 = _rtDW -> PrevY_o + _rtP
-> P_1107 ; } else { if ( rtb_B_390_16_0 < _rtP -> P_1108 ) { rtb_B_390_422_0
= _rtDW -> PrevY_o + _rtP -> P_1108 ; } } _rtDW -> PrevY_o = rtb_B_390_422_0
; _rtB -> B_390_1197_0 = ( _rtP -> P_1111 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1113 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1116 * rtb_B_390_422_0 ;
_rtB -> B_390_1199_0 = ( _rtP -> P_1114 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1115 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1117 * rtb_B_390_422_0 ;
_rtB -> B_390_1204_0 = ( _rtP -> P_1119 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1120 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1118 * rtb_B_390_422_0 ;
} if ( _rtB -> B_390_52_0 > _rtP -> P_1121 ) { _rtB -> B_51_0_0 = _rtB ->
B_390_1080_0 + _rtB -> B_390_1081_0 ; _rtB -> B_51_1_0 = _rtB -> B_52_0_2 *
_rtB -> B_390_959_0 [ 1 ] ; _rtB -> B_51_2_0 = _rtB -> B_52_0_1 * _rtB ->
B_390_959_0 [ 0 ] ; _rtB -> B_51_3_0 = _rtB -> B_51_2_0 - _rtB -> B_51_1_0 ;
_rtB -> B_51_4_0 = _rtB -> B_51_3_0 / _rtB -> B_51_0_0 ; _rtB -> B_51_5_0 =
_rtP -> P_119 * _rtB -> B_51_4_0 ; _rtB -> B_390_1206_0 = _rtB -> B_51_5_0 ;
} else { _rtB -> B_390_1206_0 = _rtB -> B_390_1071_0 ; } if ( _rtB ->
B_390_52_0 > _rtP -> P_1122 ) { _rtB -> B_50_0_0 = _rtB -> B_390_1081_0 -
_rtB -> B_390_1080_0 ; _rtB -> B_50_1_0 = _rtB -> B_52_0_1 * _rtB ->
B_390_959_0 [ 1 ] ; _rtB -> B_50_2_0 = _rtB -> B_52_0_2 * _rtB -> B_390_959_0
[ 0 ] ; _rtB -> B_50_3_0 = _rtB -> B_50_1_0 - _rtB -> B_50_2_0 ; _rtB ->
B_50_4_0 = _rtB -> B_50_3_0 / _rtB -> B_50_0_0 ; _rtB -> B_50_5_0 = _rtP ->
P_118 * _rtB -> B_50_4_0 ; _rtB -> B_390_1208_0 = _rtB -> B_50_5_0 ; } else {
_rtB -> B_390_1208_0 = _rtB -> B_390_1071_1 ; } _rtB -> B_390_1212_0 = (
muDoubleScalarSin ( _rtP -> P_1127 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1128
) * _rtP -> P_1125 + _rtP -> P_1126 ) * _rtB -> B_390_947_0 [ 0 ] ; _rtB ->
B_390_1214_0 = ( muDoubleScalarSin ( _rtP -> P_1131 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1132 ) * _rtP -> P_1129 + _rtP -> P_1130 ) * _rtB -> B_390_947_0
[ 0 ] ; _rtB -> B_390_1216_0 = ( muDoubleScalarSin ( _rtP -> P_1135 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1136 ) * _rtP -> P_1133 + _rtP -> P_1134
) * _rtB -> B_390_947_0 [ 1 ] ; _rtB -> B_390_1218_0 = ( muDoubleScalarSin (
_rtP -> P_1139 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1140 ) * _rtP -> P_1137
+ _rtP -> P_1138 ) * _rtB -> B_390_947_0 [ 1 ] ; _rtB -> B_390_1220_0 = (
muDoubleScalarSin ( _rtP -> P_1143 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1144
) * _rtP -> P_1141 + _rtP -> P_1142 ) * _rtB -> B_390_947_0 [ 2 ] ; _rtB ->
B_390_1222_0 = ( muDoubleScalarSin ( _rtP -> P_1147 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1148 ) * _rtP -> P_1145 + _rtP -> P_1146 ) * _rtB -> B_390_947_0
[ 2 ] ; B_390_1223_0 = _rtP -> P_1149 * rtb_B_390_4_1 ; _rtB -> B_390_1225_0
= ( muDoubleScalarSin ( _rtP -> P_1152 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_1153 ) * _rtP -> P_1150 + _rtP -> P_1151 ) * _rtB -> B_390_1018_0 [ 0 ] ;
_rtB -> B_390_1227_0 = ( muDoubleScalarSin ( _rtP -> P_1156 * ssGetTaskTime (
S , 0 ) + _rtP -> P_1157 ) * _rtP -> P_1154 + _rtP -> P_1155 ) * _rtB ->
B_390_1018_0 [ 0 ] ; _rtB -> B_390_1229_0 = ( muDoubleScalarSin ( _rtP ->
P_1160 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1161 ) * _rtP -> P_1158 + _rtP
-> P_1159 ) * _rtB -> B_390_1018_0 [ 1 ] ; _rtB -> B_390_1231_0 = (
muDoubleScalarSin ( _rtP -> P_1164 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1165
) * _rtP -> P_1162 + _rtP -> P_1163 ) * _rtB -> B_390_1018_0 [ 1 ] ; _rtB ->
B_390_1233_0 = ( muDoubleScalarSin ( _rtP -> P_1168 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1169 ) * _rtP -> P_1166 + _rtP -> P_1167 ) * _rtB -> B_390_1018_0
[ 2 ] ; _rtB -> B_390_1235_0 = ( muDoubleScalarSin ( _rtP -> P_1172 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1173 ) * _rtP -> P_1170 + _rtP -> P_1171
) * _rtB -> B_390_1018_0 [ 2 ] ; B_390_1236_0 = _rtP -> P_1174 *
rtb_B_390_239_0 ; _rtB -> B_390_1278_0 = _rtX -> integrator_CSTATE_ot ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ke .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ke . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1176 ; B_390_1279_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_mp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mp . Last , _rtDW -> TransportDelay_IWORK_mp . Tail ,
_rtDW -> TransportDelay_IWORK_mp . Head , _rtP -> P_1177 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1281_0 = _rtP -> P_1178 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1281_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1283_0 = _rtDW -> Memory_PreviousInput_fp ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1285_0 = ( _rtB -> B_390_1278_0 -
B_390_1279_0 ) * _rtP -> P_133 ; } else { _rtB -> B_390_1285_0 = _rtB ->
B_390_1283_0 ; } _rtB -> B_390_1286_0 = _rtX -> integrator_CSTATE_a ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1181 ; B_390_1287_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_k0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k0 . Last , _rtDW -> TransportDelay_IWORK_k0 . Tail ,
_rtDW -> TransportDelay_IWORK_k0 . Head , _rtP -> P_1182 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1289_0 = _rtP -> P_1183 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1289_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1291_0 = _rtDW -> Memory_PreviousInput_lu ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1293_0 = ( _rtB -> B_390_1286_0 -
B_390_1287_0 ) * _rtP -> P_132 ; } else { _rtB -> B_390_1293_0 = _rtB ->
B_390_1291_0 ; } rtb_B_390_422_0 = _rtP -> P_1185 * muDoubleScalarAtan2 (
_rtB -> B_390_1293_0 , _rtB -> B_390_1285_0 ) * _rtP -> P_1186 ; B_390_1298_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1285_0 , _rtB -> B_390_1293_0 ) *
muDoubleScalarCos ( rtb_B_390_422_0 ) ; B_390_1298_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1285_0 , _rtB -> B_390_1293_0 ) *
muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_1299_0 = _rtX ->
integrator_CSTATE_ob ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1188 ; B_390_1300_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_l2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l2 . Last , _rtDW -> TransportDelay_IWORK_l2 . Tail ,
_rtDW -> TransportDelay_IWORK_l2 . Head , _rtP -> P_1189 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1302_0 = _rtP -> P_1190 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1302_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1304_0 = _rtDW -> Memory_PreviousInput_p ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1306_0 = ( _rtB -> B_390_1299_0 -
B_390_1300_0 ) * _rtP -> P_135 ; } else { _rtB -> B_390_1306_0 = _rtB ->
B_390_1304_0 ; } _rtB -> B_390_1307_0 = _rtX -> integrator_CSTATE_pp ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ha .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ha . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1193 ; B_390_1308_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_a1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a1 . Last , _rtDW -> TransportDelay_IWORK_a1 . Tail ,
_rtDW -> TransportDelay_IWORK_a1 . Head , _rtP -> P_1194 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1310_0 = _rtP -> P_1195 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1310_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1312_0 = _rtDW -> Memory_PreviousInput_bn ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1314_0 = ( _rtB -> B_390_1307_0 -
B_390_1308_0 ) * _rtP -> P_134 ; } else { _rtB -> B_390_1314_0 = _rtB ->
B_390_1312_0 ; } rtb_B_390_422_0 = _rtP -> P_1197 * muDoubleScalarAtan2 (
_rtB -> B_390_1314_0 , _rtB -> B_390_1306_0 ) * _rtP -> P_1198 ; B_390_1319_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1306_0 , _rtB -> B_390_1314_0 ) *
muDoubleScalarCos ( rtb_B_390_422_0 ) ; B_390_1319_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1306_0 , _rtB -> B_390_1314_0 ) *
muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_1320_0 = _rtX ->
integrator_CSTATE_fq ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_hu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1200 ;
B_390_1321_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_eh . Last , _rtDW ->
TransportDelay_IWORK_eh . Tail , _rtDW -> TransportDelay_IWORK_eh . Head ,
_rtP -> P_1201 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1323_0 = _rtP -> P_1202 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1323_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1325_0 =
_rtDW -> Memory_PreviousInput_pm ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1327_0 = ( _rtB -> B_390_1320_0 - B_390_1321_0 ) * _rtP -> P_137 ; }
else { _rtB -> B_390_1327_0 = _rtB -> B_390_1325_0 ; } _rtB -> B_390_1328_0 =
_rtX -> integrator_CSTATE_eu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1205 ;
B_390_1329_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_f . Last , _rtDW ->
TransportDelay_IWORK_f . Tail , _rtDW -> TransportDelay_IWORK_f . Head , _rtP
-> P_1206 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1331_0 = _rtP -> P_1207 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1331_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1333_0 =
_rtDW -> Memory_PreviousInput_an ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1335_0 = ( _rtB -> B_390_1328_0 - B_390_1329_0 ) * _rtP -> P_136 ; }
else { _rtB -> B_390_1335_0 = _rtB -> B_390_1333_0 ; } rtb_B_390_422_0 = _rtP
-> P_1209 * muDoubleScalarAtan2 ( _rtB -> B_390_1335_0 , _rtB -> B_390_1327_0
) * _rtP -> P_1210 ; B_390_1340_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_1327_0 , _rtB -> B_390_1335_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_1340_0 . im = muDoubleScalarHypot ( _rtB -> B_390_1327_0 , _rtB ->
B_390_1335_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1342_0 = _rtB
-> B_390_1341_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_1342_0 , B_390_1298_0 , B_390_1319_0 , B_390_1340_0 , & _rtB ->
PosSeqComputation_l , & _rtDW -> PosSeqComputation_l , & _rtP ->
PosSeqComputation_l ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1345_0 = _rtB -> B_390_1344_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1345_0 ,
B_390_1298_0 , B_390_1319_0 , B_390_1340_0 , & _rtB -> NegSeqComputation_f ,
& _rtDW -> NegSeqComputation_f , & _rtP -> NegSeqComputation_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1348_0 = _rtB
-> B_390_1347_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_1348_0 , B_390_1298_0 , B_390_1319_0 , B_390_1340_0 , & _rtB ->
ZeroSeqComputation_k , & _rtDW -> ZeroSeqComputation_k , & _rtP ->
ZeroSeqComputation_k ) ; rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_l . B_22_2_0 . re , _rtB -> PosSeqComputation_l . B_22_2_0
. im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_l .
B_22_2_0 . im , _rtB -> PosSeqComputation_l . B_22_2_0 . re ) ; _rtB ->
B_390_1351_0 = _rtX -> integrator_CSTATE_mz ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_1215 ; B_390_1352_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ij
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_ij . Last , _rtDW ->
TransportDelay_IWORK_ij . Tail , _rtDW -> TransportDelay_IWORK_ij . Head ,
_rtP -> P_1216 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1354_0 = _rtP -> P_1217 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1354_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1356_0 =
_rtDW -> Memory_PreviousInput_hj ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1358_0 = ( _rtB -> B_390_1351_0 - B_390_1352_0 ) * _rtP -> P_139 ; }
else { _rtB -> B_390_1358_0 = _rtB -> B_390_1356_0 ; } _rtB -> B_390_1359_0 =
_rtX -> integrator_CSTATE_fa ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_cf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cf . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1220 ;
B_390_1360_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ck .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ck . Last , _rtDW ->
TransportDelay_IWORK_ck . Tail , _rtDW -> TransportDelay_IWORK_ck . Head ,
_rtP -> P_1221 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1362_0 = _rtP -> P_1222 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1362_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1364_0 =
_rtDW -> Memory_PreviousInput_eg ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1366_0 = ( _rtB -> B_390_1359_0 - B_390_1360_0 ) * _rtP -> P_138 ; }
else { _rtB -> B_390_1366_0 = _rtB -> B_390_1364_0 ; } rtb_B_390_422_0 = _rtP
-> P_1224 * muDoubleScalarAtan2 ( _rtB -> B_390_1366_0 , _rtB -> B_390_1358_0
) * _rtP -> P_1225 ; B_390_1371_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_1358_0 , _rtB -> B_390_1366_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_1371_0 . im = muDoubleScalarHypot ( _rtB -> B_390_1358_0 , _rtB ->
B_390_1366_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_1372_0
= _rtX -> integrator_CSTATE_bq ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1227
; B_390_1373_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ok .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ok . Last , _rtDW ->
TransportDelay_IWORK_ok . Tail , _rtDW -> TransportDelay_IWORK_ok . Head ,
_rtP -> P_1228 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1375_0 = _rtP -> P_1229 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1375_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1377_0 =
_rtDW -> Memory_PreviousInput_dh ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1379_0 = ( _rtB -> B_390_1372_0 - B_390_1373_0 ) * _rtP -> P_141 ; }
else { _rtB -> B_390_1379_0 = _rtB -> B_390_1377_0 ; } _rtB -> B_390_1380_0 =
_rtX -> integrator_CSTATE_fc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1232 ;
B_390_1381_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fi .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_fi . Last , _rtDW ->
TransportDelay_IWORK_fi . Tail , _rtDW -> TransportDelay_IWORK_fi . Head ,
_rtP -> P_1233 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1383_0 = _rtP -> P_1234 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1383_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1385_0 =
_rtDW -> Memory_PreviousInput_mp ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1387_0 = ( _rtB -> B_390_1380_0 - B_390_1381_0 ) * _rtP -> P_140 ; }
else { _rtB -> B_390_1387_0 = _rtB -> B_390_1385_0 ; } rtb_B_390_422_0 = _rtP
-> P_1236 * muDoubleScalarAtan2 ( _rtB -> B_390_1387_0 , _rtB -> B_390_1379_0
) * _rtP -> P_1237 ; B_390_1392_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_1379_0 , _rtB -> B_390_1387_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_1392_0 . im = muDoubleScalarHypot ( _rtB -> B_390_1379_0 , _rtB ->
B_390_1387_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_1393_0
= _rtX -> integrator_CSTATE_nm ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1239
; B_390_1394_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ea .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ea . Last , _rtDW ->
TransportDelay_IWORK_ea . Tail , _rtDW -> TransportDelay_IWORK_ea . Head ,
_rtP -> P_1240 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1396_0 = _rtP -> P_1241 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1396_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1398_0 =
_rtDW -> Memory_PreviousInput_g ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1400_0 = ( _rtB -> B_390_1393_0 - B_390_1394_0 ) * _rtP -> P_143 ; }
else { _rtB -> B_390_1400_0 = _rtB -> B_390_1398_0 ; } _rtB -> B_390_1401_0 =
_rtX -> integrator_CSTATE_c1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1244 ;
B_390_1402_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_me .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_me . Last , _rtDW ->
TransportDelay_IWORK_me . Tail , _rtDW -> TransportDelay_IWORK_me . Head ,
_rtP -> P_1245 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1404_0 = _rtP -> P_1246 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1404_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1406_0 =
_rtDW -> Memory_PreviousInput_av3 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1408_0 = ( _rtB -> B_390_1401_0 - B_390_1402_0 ) * _rtP -> P_142 ; }
else { _rtB -> B_390_1408_0 = _rtB -> B_390_1406_0 ; } rtb_B_390_422_0 = _rtP
-> P_1248 * muDoubleScalarAtan2 ( _rtB -> B_390_1408_0 , _rtB -> B_390_1400_0
) * _rtP -> P_1249 ; B_390_1413_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_1400_0 , _rtB -> B_390_1408_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_1413_0 . im = muDoubleScalarHypot ( _rtB -> B_390_1400_0 , _rtB ->
B_390_1408_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1415_0 = _rtB
-> B_390_1414_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_1415_0 , B_390_1371_0 , B_390_1392_0 , B_390_1413_0 , & _rtB ->
PosSeqComputation_m , & _rtDW -> PosSeqComputation_m , & _rtP ->
PosSeqComputation_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1418_0 = _rtB -> B_390_1417_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1418_0 ,
B_390_1371_0 , B_390_1392_0 , B_390_1413_0 , & _rtB -> NegSeqComputation_c ,
& _rtDW -> NegSeqComputation_c , & _rtP -> NegSeqComputation_c ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1421_0 = _rtB
-> B_390_1420_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_1421_0 , B_390_1371_0 , B_390_1392_0 , B_390_1413_0 , & _rtB ->
ZeroSeqComputation_f , & _rtDW -> ZeroSeqComputation_f , & _rtP ->
ZeroSeqComputation_f ) ; B_390_401_1_idx_0 = rtb_B_390_239_0 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation_m . B_22_2_0 . re , _rtB ->
PosSeqComputation_m . B_22_2_0 . im ) * _rtP -> P_1253 ; muDoubleScalarSinCos
( _rtP -> P_1256 * ( _rtP -> P_1254 * rtb_B_390_4_1 - _rtP -> P_1255 *
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_m . B_22_2_0 . im , _rtB ->
PosSeqComputation_m . B_22_2_0 . re ) ) , & rtb_B_390_1124_0 , &
rtb_B_390_422_0 ) ; rtb_B_390_422_0 = B_390_401_1_idx_0 * rtb_B_390_422_0 *
_rtP -> P_1257 ; B_390_401_1_idx_0 = B_390_401_1_idx_0 * rtb_B_390_1124_0 *
_rtP -> P_1258 ; _rtB -> B_390_1435_0 = _rtX -> integrator_CSTATE_g2 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h45 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1260 ; B_390_1436_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_di . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_di . Last , _rtDW -> TransportDelay_IWORK_di . Tail ,
_rtDW -> TransportDelay_IWORK_di . Head , _rtP -> P_1261 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1438_0 = _rtP -> P_1262 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1438_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1440_0 = _rtDW -> Memory_PreviousInput_fq ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1442_0 = ( _rtB -> B_390_1435_0 -
B_390_1436_0 ) * _rtP -> P_145 ; } else { _rtB -> B_390_1442_0 = _rtB ->
B_390_1440_0 ; } _rtB -> B_390_1443_0 = _rtX -> integrator_CSTATE_lbk ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_a . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_1265 ; B_390_1444_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_n . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n . Last , _rtDW -> TransportDelay_IWORK_n . Tail ,
_rtDW -> TransportDelay_IWORK_n . Head , _rtP -> P_1266 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1446_0 = _rtP -> P_1267 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1446_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1448_0 = _rtDW -> Memory_PreviousInput_ffa ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1450_0 = ( _rtB -> B_390_1443_0 -
B_390_1444_0 ) * _rtP -> P_144 ; } else { _rtB -> B_390_1450_0 = _rtB ->
B_390_1448_0 ; } rtb_B_390_1124_0 = _rtP -> P_1269 * muDoubleScalarAtan2 (
_rtB -> B_390_1450_0 , _rtB -> B_390_1442_0 ) * _rtP -> P_1270 ; B_390_1455_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1442_0 , _rtB -> B_390_1450_0 ) *
muDoubleScalarCos ( rtb_B_390_1124_0 ) ; B_390_1455_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1442_0 , _rtB -> B_390_1450_0 ) *
muDoubleScalarSin ( rtb_B_390_1124_0 ) ; _rtB -> B_390_1456_0 = _rtX ->
integrator_CSTATE_gw ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_om . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1272 ;
B_390_1457_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hj .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hj . Last , _rtDW ->
TransportDelay_IWORK_hj . Tail , _rtDW -> TransportDelay_IWORK_hj . Head ,
_rtP -> P_1273 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1459_0 = _rtP -> P_1274 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1459_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1461_0 =
_rtDW -> Memory_PreviousInput_fm ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1463_0 = ( _rtB -> B_390_1456_0 - B_390_1457_0 ) * _rtP -> P_147 ; }
else { _rtB -> B_390_1463_0 = _rtB -> B_390_1461_0 ; } _rtB -> B_390_1464_0 =
_rtX -> integrator_CSTATE_k1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_o5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1277 ;
B_390_1465_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oz .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_oz . Last , _rtDW ->
TransportDelay_IWORK_oz . Tail , _rtDW -> TransportDelay_IWORK_oz . Head ,
_rtP -> P_1278 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1467_0 = _rtP -> P_1279 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1467_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1469_0 =
_rtDW -> Memory_PreviousInput_fv ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1471_0 = ( _rtB -> B_390_1464_0 - B_390_1465_0 ) * _rtP -> P_146 ; }
else { _rtB -> B_390_1471_0 = _rtB -> B_390_1469_0 ; } rtb_B_390_1124_0 =
_rtP -> P_1281 * muDoubleScalarAtan2 ( _rtB -> B_390_1471_0 , _rtB ->
B_390_1463_0 ) * _rtP -> P_1282 ; B_390_1476_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1463_0 , _rtB -> B_390_1471_0 ) * muDoubleScalarCos (
rtb_B_390_1124_0 ) ; B_390_1476_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1463_0 , _rtB -> B_390_1471_0 ) * muDoubleScalarSin ( rtb_B_390_1124_0
) ; _rtB -> B_390_1477_0 = _rtX -> integrator_CSTATE_obo ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1284 ; B_390_1478_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_fq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fq . Last , _rtDW -> TransportDelay_IWORK_fq . Tail ,
_rtDW -> TransportDelay_IWORK_fq . Head , _rtP -> P_1285 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1480_0 = _rtP -> P_1286 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1480_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1482_0 = _rtDW -> Memory_PreviousInput_gx ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1484_0 = ( _rtB -> B_390_1477_0 -
B_390_1478_0 ) * _rtP -> P_149 ; } else { _rtB -> B_390_1484_0 = _rtB ->
B_390_1482_0 ; } _rtB -> B_390_1485_0 = _rtX -> integrator_CSTATE_md ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iz .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_iz . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1289 ; B_390_1486_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g1 . Last , _rtDW -> TransportDelay_IWORK_g1 . Tail ,
_rtDW -> TransportDelay_IWORK_g1 . Head , _rtP -> P_1290 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1488_0 = _rtP -> P_1291 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1488_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1490_0 = _rtDW -> Memory_PreviousInput_pc ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1492_0 = ( _rtB -> B_390_1485_0 -
B_390_1486_0 ) * _rtP -> P_148 ; } else { _rtB -> B_390_1492_0 = _rtB ->
B_390_1490_0 ; } rtb_B_390_1124_0 = _rtP -> P_1293 * muDoubleScalarAtan2 (
_rtB -> B_390_1492_0 , _rtB -> B_390_1484_0 ) * _rtP -> P_1294 ; B_390_1497_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1484_0 , _rtB -> B_390_1492_0 ) *
muDoubleScalarCos ( rtb_B_390_1124_0 ) ; B_390_1497_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1484_0 , _rtB -> B_390_1492_0 ) *
muDoubleScalarSin ( rtb_B_390_1124_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_1499_0 = _rtB -> B_390_1498_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1499_0 ,
B_390_1455_0 , B_390_1476_0 , B_390_1497_0 , & _rtB -> PosSeqComputation_i ,
& _rtDW -> PosSeqComputation_i , & _rtP -> PosSeqComputation_i ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1502_0 = _rtB
-> B_390_1501_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_1502_0 , B_390_1455_0 , B_390_1476_0 , B_390_1497_0 , & _rtB ->
NegSeqComputation_mj , & _rtDW -> NegSeqComputation_mj , & _rtP ->
NegSeqComputation_mj ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_1505_0 = _rtB -> B_390_1504_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_1505_0 ,
B_390_1455_0 , B_390_1476_0 , B_390_1497_0 , & _rtB -> ZeroSeqComputation_d ,
& _rtDW -> ZeroSeqComputation_d , & _rtP -> ZeroSeqComputation_d ) ;
rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_i . B_22_2_0
. im , _rtB -> PosSeqComputation_i . B_22_2_0 . re ) ; _rtB -> B_390_1508_0 [
0 ] = rtb_B_390_422_0 ; _rtB -> B_390_1508_0 [ 1 ] = B_390_401_1_idx_0 ; _rtB
-> B_390_1508_0 [ 2 ] = muDoubleScalarHypot ( _rtB -> PosSeqComputation_i .
B_22_2_0 . re , _rtB -> PosSeqComputation_i . B_22_2_0 . im ) ; _rtB ->
B_390_1508_0 [ 3 ] = 0.0 ; ssCallAccelRunBlock ( S , 390 , 1509 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1513_0 [ 0 ] = _rtP -> P_1298 * _rtB -> B_0_14_0 [ 105 ] *
_rtP -> P_1301 ; _rtB -> B_390_1513_0 [ 1 ] = _rtP -> P_1299 * _rtB ->
B_0_14_0 [ 106 ] * _rtP -> P_1301 ; _rtB -> B_390_1513_0 [ 2 ] = _rtP ->
P_1300 * _rtB -> B_0_14_0 [ 107 ] * _rtP -> P_1301 ; _rtB -> B_390_1517_0 [ 0
] = _rtP -> P_1302 * _rtB -> B_0_14_0 [ 50 ] * _rtP -> P_1305 ; _rtB ->
B_390_1517_0 [ 1 ] = _rtP -> P_1303 * _rtB -> B_0_14_0 [ 51 ] * _rtP ->
P_1305 ; _rtB -> B_390_1517_0 [ 2 ] = _rtP -> P_1304 * _rtB -> B_0_14_0 [ 52
] * _rtP -> P_1305 ; } _rtB -> B_390_1553_0 = ( muDoubleScalarSin ( _rtP ->
P_1308 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1309 ) * _rtP -> P_1306 + _rtP
-> P_1307 ) * _rtB -> B_390_1517_0 [ 0 ] ; _rtB -> B_390_1555_0 = (
muDoubleScalarSin ( _rtP -> P_1312 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1313
) * _rtP -> P_1310 + _rtP -> P_1311 ) * _rtB -> B_390_1517_0 [ 0 ] ; _rtB ->
B_390_1557_0 = ( muDoubleScalarSin ( _rtP -> P_1316 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1317 ) * _rtP -> P_1314 + _rtP -> P_1315 ) * _rtB -> B_390_1517_0
[ 1 ] ; _rtB -> B_390_1559_0 = ( muDoubleScalarSin ( _rtP -> P_1320 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1321 ) * _rtP -> P_1318 + _rtP -> P_1319
) * _rtB -> B_390_1517_0 [ 1 ] ; _rtB -> B_390_1561_0 = ( muDoubleScalarSin (
_rtP -> P_1324 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1325 ) * _rtP -> P_1322
+ _rtP -> P_1323 ) * _rtB -> B_390_1517_0 [ 2 ] ; _rtB -> B_390_1563_0 = (
muDoubleScalarSin ( _rtP -> P_1328 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1329
) * _rtP -> P_1326 + _rtP -> P_1327 ) * _rtB -> B_390_1517_0 [ 2 ] ; _rtB ->
B_390_1565_0 = ( muDoubleScalarSin ( _rtP -> P_1332 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1333 ) * _rtP -> P_1330 + _rtP -> P_1331 ) * _rtB -> B_390_1513_0
[ 0 ] ; _rtB -> B_390_1567_0 = ( muDoubleScalarSin ( _rtP -> P_1336 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1337 ) * _rtP -> P_1334 + _rtP -> P_1335
) * _rtB -> B_390_1513_0 [ 0 ] ; _rtB -> B_390_1569_0 = ( muDoubleScalarSin (
_rtP -> P_1340 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1341 ) * _rtP -> P_1338
+ _rtP -> P_1339 ) * _rtB -> B_390_1513_0 [ 1 ] ; _rtB -> B_390_1571_0 = (
muDoubleScalarSin ( _rtP -> P_1344 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1345
) * _rtP -> P_1342 + _rtP -> P_1343 ) * _rtB -> B_390_1513_0 [ 1 ] ; _rtB ->
B_390_1573_0 = ( muDoubleScalarSin ( _rtP -> P_1348 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1349 ) * _rtP -> P_1346 + _rtP -> P_1347 ) * _rtB -> B_390_1513_0
[ 2 ] ; _rtB -> B_390_1575_0 = ( muDoubleScalarSin ( _rtP -> P_1352 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1353 ) * _rtP -> P_1350 + _rtP -> P_1351
) * _rtB -> B_390_1513_0 [ 2 ] ; _rtB -> B_390_1577_0 = ( muDoubleScalarSin (
_rtP -> P_1356 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1357 ) * _rtP -> P_1354
+ _rtP -> P_1355 ) * _rtB -> B_390_1517_0 [ 0 ] ; _rtB -> B_390_1579_0 = (
muDoubleScalarSin ( _rtP -> P_1360 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1361
) * _rtP -> P_1358 + _rtP -> P_1359 ) * _rtB -> B_390_1517_0 [ 0 ] ; _rtB ->
B_390_1581_0 = ( muDoubleScalarSin ( _rtP -> P_1364 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1365 ) * _rtP -> P_1362 + _rtP -> P_1363 ) * _rtB -> B_390_1517_0
[ 1 ] ; _rtB -> B_390_1583_0 = ( muDoubleScalarSin ( _rtP -> P_1368 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1369 ) * _rtP -> P_1366 + _rtP -> P_1367
) * _rtB -> B_390_1517_0 [ 1 ] ; _rtB -> B_390_1585_0 = ( muDoubleScalarSin (
_rtP -> P_1372 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1373 ) * _rtP -> P_1370
+ _rtP -> P_1371 ) * _rtB -> B_390_1517_0 [ 2 ] ; _rtB -> B_390_1587_0 = (
muDoubleScalarSin ( _rtP -> P_1376 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1377
) * _rtP -> P_1374 + _rtP -> P_1375 ) * _rtB -> B_390_1517_0 [ 2 ] ;
B_390_1588_0 = _rtP -> P_1378 * rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 1
, 0 ) ; if ( isHit != 0 ) { rtb_B_390_16_0 = ssGetTaskTime ( S , 1 ) ; _rtDW
-> IlhamentoChavefechadaparachaveabertaemtilha_MODE = ( rtb_B_390_16_0 >=
_rtP -> P_1381 ) ; if ( _rtDW ->
IlhamentoChavefechadaparachaveabertaemtilha_MODE == 1 ) { _rtB ->
B_390_1591_0 = _rtP -> P_1383 ; } else { _rtB -> B_390_1591_0 = _rtP ->
P_1382 ; } if ( 0.0 > _rtP -> P_1384 ) { _rtB -> B_390_1592_0 = _rtB ->
B_390_1590_0 ; } else { _rtB -> B_390_1592_0 = _rtB -> B_390_1591_0 ; } }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1595_0 = rt_Lookup ( _rtP -> P_1386 , 4 , ssGetTaskTime ( S , 2 ) ,
_rtP -> P_1387 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_1593_0 >= _rtP -> P_1388 ) { rtb_B_390_1596_0 = _rtB ->
B_390_1592_0 ; } else { rtb_B_390_1596_0 = _rtB -> B_390_1595_0 ; } if ( _rtB
-> B_390_1597_0 >= _rtP -> P_1391 ) { _rtB -> B_390_1600_0 = rtb_B_390_1596_0
; } else { _rtB -> B_390_1600_0 = _rtB -> B_390_1598_0 ; } } _rtB ->
B_390_1602_0 = rt_Lookup ( _rtP -> P_1392 , 5 , ssGetT ( S ) , _rtP -> P_1393
) ; if ( _rtB -> B_390_1589_0 >= _rtP -> P_1394 ) { _rtB -> B_390_1603_0 =
_rtB -> B_390_1600_0 ; } else { _rtB -> B_390_1603_0 = _rtB -> B_390_1602_0 ;
} if ( ( _rtDW -> TimeStampA >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB >=
ssGetT ( S ) ) ) { _rtB -> B_390_1604_0 = 0.0 ; } else { rtb_B_390_422_0 =
_rtDW -> TimeStampA ; lastU = & _rtDW -> LastUAtTimeA ; if ( _rtDW ->
TimeStampA < _rtDW -> TimeStampB ) { if ( _rtDW -> TimeStampB < ssGetT ( S )
) { rtb_B_390_422_0 = _rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ;
} } else { if ( _rtDW -> TimeStampA >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ; } } _rtB ->
B_390_1604_0 = ( _rtB -> B_390_1602_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_1610_0 >= _rtP -> P_1398 ) { _rtB -> B_390_1612_0 =
rtb_B_390_1596_0 ; } else { _rtB -> B_390_1612_0 = _rtB -> B_390_1598_0 ; } }
_rtB -> B_390_1614_0 = rt_Lookup ( _rtP -> P_1399 , 5 , ssGetT ( S ) , _rtP
-> P_1400 ) ; if ( _rtB -> B_390_1609_0 >= _rtP -> P_1401 ) { _rtB ->
B_390_1615_0 = _rtB -> B_390_1612_0 ; } else { _rtB -> B_390_1615_0 = _rtB ->
B_390_1614_0 ; } if ( ( _rtDW -> TimeStampA_n >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_i >= ssGetT ( S ) ) ) { _rtB -> B_390_1616_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_n ; lastU = & _rtDW -> LastUAtTimeA_j ;
if ( _rtDW -> TimeStampA_n < _rtDW -> TimeStampB_i ) { if ( _rtDW ->
TimeStampB_i < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_i ;
lastU = & _rtDW -> LastUAtTimeB_d ; } } else { if ( _rtDW -> TimeStampA_n >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_i ; lastU = & _rtDW ->
LastUAtTimeB_d ; } } _rtB -> B_390_1616_0 = ( _rtB -> B_390_1614_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_390_1622_0 >= _rtP -> P_1405 ) { _rtB
-> B_390_1624_0 = rtb_B_390_1596_0 ; } else { _rtB -> B_390_1624_0 = _rtB ->
B_390_1598_0 ; } } _rtB -> B_390_1626_0 = rt_Lookup ( _rtP -> P_1406 , 5 ,
ssGetT ( S ) , _rtP -> P_1407 ) ; if ( _rtB -> B_390_1621_0 >= _rtP -> P_1408
) { _rtB -> B_390_1627_0 = _rtB -> B_390_1624_0 ; } else { _rtB ->
B_390_1627_0 = _rtB -> B_390_1626_0 ; } if ( ( _rtDW -> TimeStampA_nq >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_k >= ssGetT ( S ) ) ) { _rtB ->
B_390_1628_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_nq ;
lastU = & _rtDW -> LastUAtTimeA_c ; if ( _rtDW -> TimeStampA_nq < _rtDW ->
TimeStampB_k ) { if ( _rtDW -> TimeStampB_k < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_k ; lastU = & _rtDW -> LastUAtTimeB_m ;
} } else { if ( _rtDW -> TimeStampA_nq >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_k ; lastU = & _rtDW -> LastUAtTimeB_m ; } } _rtB ->
B_390_1628_0 = ( _rtB -> B_390_1626_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ ssCallAccelRunBlock ( S , 390 , 1641 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 390 , 1642 , SS_CALL_MDL_OUTPUTS ) ; rtb_B_390_16_0
= ssGetTaskTime ( S , 1 ) ; _rtDW -> Step_MODE = ( rtb_B_390_16_0 >= _rtP ->
P_1410 ) ; if ( _rtDW -> Step_MODE == 1 ) { _rtB -> B_390_1649_0 = _rtP ->
P_1412 ; } else { _rtB -> B_390_1649_0 = _rtP -> P_1411 ; } } _rtB ->
B_390_1655_0 = ( ssGetT ( S ) - _rtB -> B_390_1651_0 ) * _rtB -> B_390_1649_0
+ _rtB -> B_390_1654_0 ; ssCallAccelRunBlock ( S , 390 , 1656 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { rtb_B_390_1596_0 = _rtP -> P_1415 * _rtB -> B_0_14_0 [ 35 ] ;
rtb_B_390_422_0 = _rtP -> P_1416 * _rtB -> B_0_14_0 [ 36 ] ;
B_390_401_1_idx_0 = _rtP -> P_1417 * _rtB -> B_0_14_0 [ 37 ] ; B_390_1660_0 [
0 ] = _rtP -> P_1418 * rtb_B_390_1596_0 ; B_390_1660_0 [ 1 ] = _rtP -> P_1418
* rtb_B_390_422_0 ; B_390_1660_0 [ 2 ] = _rtP -> P_1418 * B_390_401_1_idx_0 ;
B_390_1661_0 [ 0 ] = _rtP -> P_1419 * rtb_B_390_1596_0 ; B_390_1661_0 [ 1 ] =
_rtP -> P_1419 * rtb_B_390_422_0 ; B_390_1661_0 [ 2 ] = _rtP -> P_1419 *
B_390_401_1_idx_0 ; rtb_B_390_1596_0 = _rtP -> P_1420 * _rtB -> B_0_14_0 [ 32
] ; rtb_B_390_422_0 = _rtP -> P_1421 * _rtB -> B_0_14_0 [ 33 ] ;
B_390_401_1_idx_0 = _rtP -> P_1422 * _rtB -> B_0_14_0 [ 34 ] ; B_390_1689_0 [
0 ] = _rtP -> P_1423 * rtb_B_390_1596_0 ; B_390_1689_0 [ 1 ] = _rtP -> P_1423
* rtb_B_390_422_0 ; B_390_1689_0 [ 2 ] = _rtP -> P_1423 * B_390_401_1_idx_0 ;
B_390_1690_0 [ 0 ] = _rtP -> P_1424 * rtb_B_390_1596_0 ; B_390_1690_0 [ 1 ] =
_rtP -> P_1424 * rtb_B_390_422_0 ; B_390_1690_0 [ 2 ] = _rtP -> P_1424 *
B_390_401_1_idx_0 ; } _rtB -> B_390_1715_0 = _rtX -> integrator_CSTATE_nq ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1426 ; B_390_1716_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_hog . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hog . Last , _rtDW -> TransportDelay_IWORK_hog . Tail ,
_rtDW -> TransportDelay_IWORK_hog . Head , _rtP -> P_1427 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1718_0 = _rtP -> P_1428 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1718_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1720_0 = _rtDW -> Memory_PreviousInput_pz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1722_0 = ( _rtB -> B_390_1715_0 -
B_390_1716_0 ) * _rtP -> P_181 ; } else { _rtB -> B_390_1722_0 = _rtB ->
B_390_1720_0 ; } _rtB -> B_390_1723_0 = _rtX -> integrator_CSTATE_fo ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ak .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ak . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1431 ; B_390_1724_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ju . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ju . Last , _rtDW -> TransportDelay_IWORK_ju . Tail ,
_rtDW -> TransportDelay_IWORK_ju . Head , _rtP -> P_1432 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1726_0 = _rtP -> P_1433 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1726_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1728_0 = _rtDW -> Memory_PreviousInput_ft ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1730_0 = ( _rtB -> B_390_1723_0 -
B_390_1724_0 ) * _rtP -> P_180 ; } else { _rtB -> B_390_1730_0 = _rtB ->
B_390_1728_0 ; } rtb_B_390_1596_0 = _rtP -> P_1435 * muDoubleScalarAtan2 (
_rtB -> B_390_1730_0 , _rtB -> B_390_1722_0 ) * _rtP -> P_1436 ; B_390_1735_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1722_0 , _rtB -> B_390_1730_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_1735_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1722_0 , _rtB -> B_390_1730_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; _rtB -> B_390_1736_0 = _rtX ->
integrator_CSTATE_p5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_js . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_js . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1438 ;
B_390_1737_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a3 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_a3 . Last , _rtDW ->
TransportDelay_IWORK_a3 . Tail , _rtDW -> TransportDelay_IWORK_a3 . Head ,
_rtP -> P_1439 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1739_0 = _rtP -> P_1440 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1739_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1741_0 =
_rtDW -> Memory_PreviousInput_gi ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1743_0 = ( _rtB -> B_390_1736_0 - B_390_1737_0 ) * _rtP -> P_183 ; }
else { _rtB -> B_390_1743_0 = _rtB -> B_390_1741_0 ; } _rtB -> B_390_1744_0 =
_rtX -> integrator_CSTATE_ad ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_pp . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pp . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1443 ;
B_390_1745_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_at .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_at . Last , _rtDW ->
TransportDelay_IWORK_at . Tail , _rtDW -> TransportDelay_IWORK_at . Head ,
_rtP -> P_1444 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1747_0 = _rtP -> P_1445 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1747_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1749_0 =
_rtDW -> Memory_PreviousInput_hg ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1751_0 = ( _rtB -> B_390_1744_0 - B_390_1745_0 ) * _rtP -> P_182 ; }
else { _rtB -> B_390_1751_0 = _rtB -> B_390_1749_0 ; } rtb_B_390_1596_0 =
_rtP -> P_1447 * muDoubleScalarAtan2 ( _rtB -> B_390_1751_0 , _rtB ->
B_390_1743_0 ) * _rtP -> P_1448 ; B_390_1756_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1743_0 , _rtB -> B_390_1751_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_1756_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1743_0 , _rtB -> B_390_1751_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; _rtB -> B_390_1757_0 = _rtX -> integrator_CSTATE_cd ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1450 ; B_390_1758_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_h5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_h5 . Last , _rtDW -> TransportDelay_IWORK_h5 . Tail ,
_rtDW -> TransportDelay_IWORK_h5 . Head , _rtP -> P_1451 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1760_0 = _rtP -> P_1452 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1760_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1762_0 = _rtDW -> Memory_PreviousInput_dg ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1764_0 = ( _rtB -> B_390_1757_0 -
B_390_1758_0 ) * _rtP -> P_185 ; } else { _rtB -> B_390_1764_0 = _rtB ->
B_390_1762_0 ; } _rtB -> B_390_1765_0 = _rtX -> integrator_CSTATE_ds ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fp .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fp . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1455 ; B_390_1766_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_cr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cr . Last , _rtDW -> TransportDelay_IWORK_cr . Tail ,
_rtDW -> TransportDelay_IWORK_cr . Head , _rtP -> P_1456 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1768_0 = _rtP -> P_1457 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1768_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1770_0 = _rtDW -> Memory_PreviousInput_kp ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1772_0 = ( _rtB -> B_390_1765_0 -
B_390_1766_0 ) * _rtP -> P_184 ; } else { _rtB -> B_390_1772_0 = _rtB ->
B_390_1770_0 ; } rtb_B_390_1596_0 = _rtP -> P_1459 * muDoubleScalarAtan2 (
_rtB -> B_390_1772_0 , _rtB -> B_390_1764_0 ) * _rtP -> P_1460 ; B_390_1777_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1764_0 , _rtB -> B_390_1772_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_1777_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1764_0 , _rtB -> B_390_1772_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_1779_0 = _rtB -> B_390_1778_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1779_0 ,
B_390_1735_0 , B_390_1756_0 , B_390_1777_0 , & _rtB -> PosSeqComputation_aq ,
& _rtDW -> PosSeqComputation_aq , & _rtP -> PosSeqComputation_aq ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1782_0 = _rtB
-> B_390_1781_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_1782_0 , B_390_1735_0 , B_390_1756_0 , B_390_1777_0 , & _rtB ->
NegSeqComputation_k , & _rtDW -> NegSeqComputation_k , & _rtP ->
NegSeqComputation_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1785_0 = _rtB -> B_390_1784_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_1785_0 ,
B_390_1735_0 , B_390_1756_0 , B_390_1777_0 , & _rtB -> ZeroSeqComputation_e ,
& _rtDW -> ZeroSeqComputation_e , & _rtP -> ZeroSeqComputation_e ) ;
rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_aq .
B_22_2_0 . re , _rtB -> PosSeqComputation_aq . B_22_2_0 . im ) ;
rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_aq . B_22_2_0
. im , _rtB -> PosSeqComputation_aq . B_22_2_0 . re ) ; _rtB -> B_390_1788_0
= _rtX -> integrator_CSTATE_ja ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1465
; B_390_1789_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dv . Last , _rtDW ->
TransportDelay_IWORK_dv . Tail , _rtDW -> TransportDelay_IWORK_dv . Head ,
_rtP -> P_1466 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1791_0 = _rtP -> P_1467 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1791_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1793_0 =
_rtDW -> Memory_PreviousInput_fg ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1795_0 = ( _rtB -> B_390_1788_0 - B_390_1789_0 ) * _rtP -> P_187 ; }
else { _rtB -> B_390_1795_0 = _rtB -> B_390_1793_0 ; } _rtB -> B_390_1796_0 =
_rtX -> integrator_CSTATE_an ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_o1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1470 ;
B_390_1797_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lv . Last , _rtDW ->
TransportDelay_IWORK_lv . Tail , _rtDW -> TransportDelay_IWORK_lv . Head ,
_rtP -> P_1471 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1799_0 = _rtP -> P_1472 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1799_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1801_0 =
_rtDW -> Memory_PreviousInput_bj ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1803_0 = ( _rtB -> B_390_1796_0 - B_390_1797_0 ) * _rtP -> P_186 ; }
else { _rtB -> B_390_1803_0 = _rtB -> B_390_1801_0 ; } rtb_B_390_1596_0 =
_rtP -> P_1474 * muDoubleScalarAtan2 ( _rtB -> B_390_1803_0 , _rtB ->
B_390_1795_0 ) * _rtP -> P_1475 ; B_390_1808_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1795_0 , _rtB -> B_390_1803_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_1808_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1795_0 , _rtB -> B_390_1803_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; _rtB -> B_390_1809_0 = _rtX -> integrator_CSTATE_d3 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1477 ; B_390_1810_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_lu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lu . Last , _rtDW -> TransportDelay_IWORK_lu . Tail ,
_rtDW -> TransportDelay_IWORK_lu . Head , _rtP -> P_1478 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1812_0 = _rtP -> P_1479 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1812_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1814_0 = _rtDW -> Memory_PreviousInput_df ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1816_0 = ( _rtB -> B_390_1809_0 -
B_390_1810_0 ) * _rtP -> P_189 ; } else { _rtB -> B_390_1816_0 = _rtB ->
B_390_1814_0 ; } _rtB -> B_390_1817_0 = _rtX -> integrator_CSTATE_fl ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1482 ; B_390_1818_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kz . Last , _rtDW -> TransportDelay_IWORK_kz . Tail ,
_rtDW -> TransportDelay_IWORK_kz . Head , _rtP -> P_1483 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1820_0 = _rtP -> P_1484 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1820_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1822_0 = _rtDW -> Memory_PreviousInput_bv ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1824_0 = ( _rtB -> B_390_1817_0 -
B_390_1818_0 ) * _rtP -> P_188 ; } else { _rtB -> B_390_1824_0 = _rtB ->
B_390_1822_0 ; } rtb_B_390_1596_0 = _rtP -> P_1486 * muDoubleScalarAtan2 (
_rtB -> B_390_1824_0 , _rtB -> B_390_1816_0 ) * _rtP -> P_1487 ; B_390_1829_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1816_0 , _rtB -> B_390_1824_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_1829_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1816_0 , _rtB -> B_390_1824_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; _rtB -> B_390_1830_0 = _rtX ->
integrator_CSTATE_m3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oa . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oa . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1489 ;
B_390_1831_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ft .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ft . Last , _rtDW ->
TransportDelay_IWORK_ft . Tail , _rtDW -> TransportDelay_IWORK_ft . Head ,
_rtP -> P_1490 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1833_0 = _rtP -> P_1491 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1833_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1835_0 =
_rtDW -> Memory_PreviousInput_eq ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1837_0 = ( _rtB -> B_390_1830_0 - B_390_1831_0 ) * _rtP -> P_191 ; }
else { _rtB -> B_390_1837_0 = _rtB -> B_390_1835_0 ; } _rtB -> B_390_1838_0 =
_rtX -> integrator_CSTATE_oo ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fd . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1494 ;
B_390_1839_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bc .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bc . Last , _rtDW ->
TransportDelay_IWORK_bc . Tail , _rtDW -> TransportDelay_IWORK_bc . Head ,
_rtP -> P_1495 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1841_0 = _rtP -> P_1496 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1841_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1843_0 =
_rtDW -> Memory_PreviousInput_iw ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1845_0 = ( _rtB -> B_390_1838_0 - B_390_1839_0 ) * _rtP -> P_190 ; }
else { _rtB -> B_390_1845_0 = _rtB -> B_390_1843_0 ; } rtb_B_390_1596_0 =
_rtP -> P_1498 * muDoubleScalarAtan2 ( _rtB -> B_390_1845_0 , _rtB ->
B_390_1837_0 ) * _rtP -> P_1499 ; B_390_1850_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1837_0 , _rtB -> B_390_1845_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_1850_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1837_0 , _rtB -> B_390_1845_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1852_0 = _rtB -> B_390_1851_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_1852_0 , B_390_1808_0 , B_390_1829_0 , B_390_1850_0 , &
_rtB -> PosSeqComputation_e , & _rtDW -> PosSeqComputation_e , & _rtP ->
PosSeqComputation_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1855_0 = _rtB -> B_390_1854_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1855_0 ,
B_390_1808_0 , B_390_1829_0 , B_390_1850_0 , & _rtB -> NegSeqComputation_ed ,
& _rtDW -> NegSeqComputation_ed , & _rtP -> NegSeqComputation_ed ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1858_0 = _rtB
-> B_390_1857_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_1858_0 , B_390_1808_0 , B_390_1829_0 , B_390_1850_0 , & _rtB ->
ZeroSeqComputation_l , & _rtDW -> ZeroSeqComputation_l , & _rtP ->
ZeroSeqComputation_l ) ; rtb_B_390_1596_0 = rtb_B_390_239_0 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation_e . B_22_2_0 . re , _rtB ->
PosSeqComputation_e . B_22_2_0 . im ) * _rtP -> P_1503 ; muDoubleScalarSinCos
( _rtP -> P_1506 * ( _rtP -> P_1504 * rtb_B_390_4_1 - _rtP -> P_1505 *
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_e . B_22_2_0 . im , _rtB ->
PosSeqComputation_e . B_22_2_0 . re ) ) , & rtb_B_390_422_0 , &
B_390_401_1_idx_0 ) ; _rtB -> B_390_1868_0 = rtb_B_390_1596_0 *
B_390_401_1_idx_0 ; _rtB -> B_390_1869_0 = _rtX -> integrator_CSTATE_cf ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ej .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ej . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1508 ; B_390_1870_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_by . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_by . Last , _rtDW -> TransportDelay_IWORK_by . Tail ,
_rtDW -> TransportDelay_IWORK_by . Head , _rtP -> P_1509 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1872_0 = _rtP -> P_1510 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1872_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1874_0 = _rtDW -> Memory_PreviousInput_fqc ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1876_0 = ( _rtB -> B_390_1869_0 -
B_390_1870_0 ) * _rtP -> P_193 ; } else { _rtB -> B_390_1876_0 = _rtB ->
B_390_1874_0 ; } _rtB -> B_390_1877_0 = _rtX -> integrator_CSTATE_jq ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fb .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fb . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1513 ; B_390_1878_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_nm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nm . Last , _rtDW -> TransportDelay_IWORK_nm . Tail ,
_rtDW -> TransportDelay_IWORK_nm . Head , _rtP -> P_1514 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1880_0 = _rtP -> P_1515 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1880_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1882_0 = _rtDW -> Memory_PreviousInput_bf ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1884_0 = ( _rtB -> B_390_1877_0 -
B_390_1878_0 ) * _rtP -> P_192 ; } else { _rtB -> B_390_1884_0 = _rtB ->
B_390_1882_0 ; } B_390_401_1_idx_0 = _rtP -> P_1517 * muDoubleScalarAtan2 (
_rtB -> B_390_1884_0 , _rtB -> B_390_1876_0 ) * _rtP -> P_1518 ; B_390_1889_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1876_0 , _rtB -> B_390_1884_0 ) *
muDoubleScalarCos ( B_390_401_1_idx_0 ) ; B_390_1889_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1876_0 , _rtB -> B_390_1884_0 ) *
muDoubleScalarSin ( B_390_401_1_idx_0 ) ; _rtB -> B_390_1890_0 = _rtX ->
integrator_CSTATE_i ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1520 ; B_390_1891_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_og . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_og . Last , _rtDW -> TransportDelay_IWORK_og . Tail ,
_rtDW -> TransportDelay_IWORK_og . Head , _rtP -> P_1521 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1893_0 = _rtP -> P_1522 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1893_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1895_0 = _rtDW -> Memory_PreviousInput_ib ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1897_0 = ( _rtB -> B_390_1890_0 -
B_390_1891_0 ) * _rtP -> P_195 ; } else { _rtB -> B_390_1897_0 = _rtB ->
B_390_1895_0 ; } _rtB -> B_390_1898_0 = _rtX -> integrator_CSTATE_l1 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_op .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_op . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1525 ; B_390_1899_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_he . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he . Last , _rtDW -> TransportDelay_IWORK_he . Tail ,
_rtDW -> TransportDelay_IWORK_he . Head , _rtP -> P_1526 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1901_0 = _rtP -> P_1527 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1901_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1903_0 = _rtDW -> Memory_PreviousInput_ak ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1905_0 = ( _rtB -> B_390_1898_0 -
B_390_1899_0 ) * _rtP -> P_194 ; } else { _rtB -> B_390_1905_0 = _rtB ->
B_390_1903_0 ; } B_390_401_1_idx_0 = _rtP -> P_1529 * muDoubleScalarAtan2 (
_rtB -> B_390_1905_0 , _rtB -> B_390_1897_0 ) * _rtP -> P_1530 ; B_390_1910_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1897_0 , _rtB -> B_390_1905_0 ) *
muDoubleScalarCos ( B_390_401_1_idx_0 ) ; B_390_1910_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1897_0 , _rtB -> B_390_1905_0 ) *
muDoubleScalarSin ( B_390_401_1_idx_0 ) ; _rtB -> B_390_1911_0 = _rtX ->
integrator_CSTATE_k1l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_hk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1532 ;
B_390_1912_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hl . Last , _rtDW ->
TransportDelay_IWORK_hl . Tail , _rtDW -> TransportDelay_IWORK_hl . Head ,
_rtP -> P_1533 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1914_0 = _rtP -> P_1534 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1914_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1916_0 =
_rtDW -> Memory_PreviousInput_bu ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1918_0 = ( _rtB -> B_390_1911_0 - B_390_1912_0 ) * _rtP -> P_197 ; }
else { _rtB -> B_390_1918_0 = _rtB -> B_390_1916_0 ; } _rtB -> B_390_1919_0 =
_rtX -> integrator_CSTATE_hs ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1537 ;
B_390_1920_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j1 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j1 . Last , _rtDW ->
TransportDelay_IWORK_j1 . Tail , _rtDW -> TransportDelay_IWORK_j1 . Head ,
_rtP -> P_1538 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1922_0 = _rtP -> P_1539 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1922_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1924_0 =
_rtDW -> Memory_PreviousInput_ih ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1926_0 = ( _rtB -> B_390_1919_0 - B_390_1920_0 ) * _rtP -> P_196 ; }
else { _rtB -> B_390_1926_0 = _rtB -> B_390_1924_0 ; } B_390_401_1_idx_0 =
_rtP -> P_1541 * muDoubleScalarAtan2 ( _rtB -> B_390_1926_0 , _rtB ->
B_390_1918_0 ) * _rtP -> P_1542 ; B_390_1931_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1918_0 , _rtB -> B_390_1926_0 ) * muDoubleScalarCos (
B_390_401_1_idx_0 ) ; B_390_1931_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1918_0 , _rtB -> B_390_1926_0 ) * muDoubleScalarSin ( B_390_401_1_idx_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1933_0 = _rtB -> B_390_1932_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_1933_0 , B_390_1889_0 , B_390_1910_0 , B_390_1931_0 , &
_rtB -> PosSeqComputation_h , & _rtDW -> PosSeqComputation_h , & _rtP ->
PosSeqComputation_h ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_1936_0 = _rtB -> B_390_1935_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_1936_0 ,
B_390_1889_0 , B_390_1910_0 , B_390_1931_0 , & _rtB -> NegSeqComputation_o ,
& _rtDW -> NegSeqComputation_o , & _rtP -> NegSeqComputation_o ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1939_0 = _rtB
-> B_390_1938_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_1939_0 , B_390_1889_0 , B_390_1910_0 , B_390_1931_0 , & _rtB ->
ZeroSeqComputation_g , & _rtDW -> ZeroSeqComputation_g , & _rtP ->
ZeroSeqComputation_g ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_h . B_22_2_0 . im , _rtB -> PosSeqComputation_h . B_22_2_0
. re ) ; _rtB -> B_390_1942_0 = _rtP -> P_1546 * muDoubleScalarHypot ( _rtB
-> PosSeqComputation_h . B_22_2_0 . re , _rtB -> PosSeqComputation_h .
B_22_2_0 . im ) ; _rtB -> B_390_1943_0 = rtb_B_390_1596_0 * rtb_B_390_422_0 ;
_rtB -> B_390_1944_0 = _rtX -> integrator_CSTATE_en ; { real_T * * uBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1548 ; B_390_1945_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_e2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2 . Last , _rtDW -> TransportDelay_IWORK_e2 . Tail ,
_rtDW -> TransportDelay_IWORK_e2 . Head , _rtP -> P_1549 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1947_0 = _rtP -> P_1550 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1947_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1949_0 = _rtDW -> Memory_PreviousInput_de ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1951_0 = ( _rtB -> B_390_1944_0 -
B_390_1945_0 ) * _rtP -> P_175 ; } else { _rtB -> B_390_1951_0 = _rtB ->
B_390_1949_0 ; } _rtB -> B_390_1952_0 = _rtX -> integrator_CSTATE_iv ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fdx . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1553 ; B_390_1953_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_id . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_id . Last , _rtDW -> TransportDelay_IWORK_id . Tail ,
_rtDW -> TransportDelay_IWORK_id . Head , _rtP -> P_1554 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1955_0 = _rtP -> P_1555 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1955_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1957_0 = _rtDW -> Memory_PreviousInput_h2 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1959_0 = ( _rtB -> B_390_1952_0 -
B_390_1953_0 ) * _rtP -> P_174 ; } else { _rtB -> B_390_1959_0 = _rtB ->
B_390_1957_0 ; } rtb_B_390_1596_0 = _rtP -> P_1557 * muDoubleScalarAtan2 (
_rtB -> B_390_1959_0 , _rtB -> B_390_1951_0 ) * _rtP -> P_1558 ; B_390_1964_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1951_0 , _rtB -> B_390_1959_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_1964_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1951_0 , _rtB -> B_390_1959_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; _rtB -> B_390_1965_0 = _rtX ->
integrator_CSTATE_hf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_lh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1560 ;
B_390_1966_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gr . Last , _rtDW ->
TransportDelay_IWORK_gr . Tail , _rtDW -> TransportDelay_IWORK_gr . Head ,
_rtP -> P_1561 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1968_0 = _rtP -> P_1562 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1968_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1970_0 =
_rtDW -> Memory_PreviousInput_mb ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1972_0 = ( _rtB -> B_390_1965_0 - B_390_1966_0 ) * _rtP -> P_177 ; }
else { _rtB -> B_390_1972_0 = _rtB -> B_390_1970_0 ; } _rtB -> B_390_1973_0 =
_rtX -> integrator_CSTATE_fj ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nd . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1565 ;
B_390_1974_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dl . Last , _rtDW ->
TransportDelay_IWORK_dl . Tail , _rtDW -> TransportDelay_IWORK_dl . Head ,
_rtP -> P_1566 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1976_0 = _rtP -> P_1567 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_1976_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_1978_0 =
_rtDW -> Memory_PreviousInput_bz ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_1980_0 = ( _rtB -> B_390_1973_0 - B_390_1974_0 ) * _rtP -> P_176 ; }
else { _rtB -> B_390_1980_0 = _rtB -> B_390_1978_0 ; } rtb_B_390_1596_0 =
_rtP -> P_1569 * muDoubleScalarAtan2 ( _rtB -> B_390_1980_0 , _rtB ->
B_390_1972_0 ) * _rtP -> P_1570 ; B_390_1985_0 . re = muDoubleScalarHypot (
_rtB -> B_390_1972_0 , _rtB -> B_390_1980_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_1985_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_1972_0 , _rtB -> B_390_1980_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; _rtB -> B_390_1986_0 = _rtX -> integrator_CSTATE_ff ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1572 ; B_390_1987_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_hx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hx . Last , _rtDW -> TransportDelay_IWORK_hx . Tail ,
_rtDW -> TransportDelay_IWORK_hx . Head , _rtP -> P_1573 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1989_0 = _rtP -> P_1574 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1989_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1991_0 = _rtDW -> Memory_PreviousInput_eh ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_1993_0 = ( _rtB -> B_390_1986_0 -
B_390_1987_0 ) * _rtP -> P_179 ; } else { _rtB -> B_390_1993_0 = _rtB ->
B_390_1991_0 ; } _rtB -> B_390_1994_0 = _rtX -> integrator_CSTATE_er ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jf .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_jf . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1577 ; B_390_1995_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_fw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fw . Last , _rtDW -> TransportDelay_IWORK_fw . Tail ,
_rtDW -> TransportDelay_IWORK_fw . Head , _rtP -> P_1578 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_1997_0 = _rtP -> P_1579 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_1997_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_1999_0 = _rtDW -> Memory_PreviousInput_n ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2001_0 = ( _rtB -> B_390_1994_0 -
B_390_1995_0 ) * _rtP -> P_178 ; } else { _rtB -> B_390_2001_0 = _rtB ->
B_390_1999_0 ; } rtb_B_390_1596_0 = _rtP -> P_1581 * muDoubleScalarAtan2 (
_rtB -> B_390_2001_0 , _rtB -> B_390_1993_0 ) * _rtP -> P_1582 ; B_390_2006_0
. re = muDoubleScalarHypot ( _rtB -> B_390_1993_0 , _rtB -> B_390_2001_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_2006_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_1993_0 , _rtB -> B_390_2001_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_2008_0 = _rtB -> B_390_2007_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2008_0 ,
B_390_1964_0 , B_390_1985_0 , B_390_2006_0 , & _rtB -> PosSeqComputation_ep ,
& _rtDW -> PosSeqComputation_ep , & _rtP -> PosSeqComputation_ep ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2011_0 = _rtB
-> B_390_2010_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2011_0 , B_390_1964_0 , B_390_1985_0 , B_390_2006_0 , & _rtB ->
NegSeqComputation_ms , & _rtDW -> NegSeqComputation_ms , & _rtP ->
NegSeqComputation_ms ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_2014_0 = _rtB -> B_390_2013_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_2014_0 ,
B_390_1964_0 , B_390_1985_0 , B_390_2006_0 , & _rtB -> ZeroSeqComputation_dq
, & _rtDW -> ZeroSeqComputation_dq , & _rtP -> ZeroSeqComputation_dq ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ep .
B_22_2_0 . im , _rtB -> PosSeqComputation_ep . B_22_2_0 . re ) ; _rtB ->
B_390_2017_0 = _rtP -> P_1586 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_ep . B_22_2_0 . re , _rtB -> PosSeqComputation_ep .
B_22_2_0 . im ) ; ssCallAccelRunBlock ( S , 390 , 2018 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_390_1508_0 [ 0 ] = _rtB -> B_390_1868_0 ; _rtB -> B_390_1508_0
[ 1 ] = _rtB -> B_390_1942_0 ; _rtB -> B_390_1508_0 [ 2 ] = _rtB ->
B_390_1943_0 ; _rtB -> B_390_1508_0 [ 3 ] = _rtB -> B_390_2017_0 ;
ssCallAccelRunBlock ( S , 390 , 2020 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2024_0 [ 0 ]
= _rtP -> P_1587 * _rtB -> B_0_14_0 [ 111 ] * _rtP -> P_1590 ; _rtB ->
B_390_2024_0 [ 1 ] = _rtP -> P_1588 * _rtB -> B_0_14_0 [ 112 ] * _rtP ->
P_1590 ; _rtB -> B_390_2024_0 [ 2 ] = _rtP -> P_1589 * _rtB -> B_0_14_0 [ 113
] * _rtP -> P_1590 ; _rtB -> B_390_2028_0 [ 0 ] = _rtP -> P_1591 * _rtB ->
B_0_14_0 [ 56 ] * _rtP -> P_1594 ; _rtB -> B_390_2028_0 [ 1 ] = _rtP ->
P_1592 * _rtB -> B_0_14_0 [ 57 ] * _rtP -> P_1594 ; _rtB -> B_390_2028_0 [ 2
] = _rtP -> P_1593 * _rtB -> B_0_14_0 [ 58 ] * _rtP -> P_1594 ; } _rtB ->
B_390_2064_0 = ( muDoubleScalarSin ( _rtP -> P_1597 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1598 ) * _rtP -> P_1595 + _rtP -> P_1596 ) * _rtB -> B_390_2024_0
[ 0 ] ; _rtB -> B_390_2066_0 = ( muDoubleScalarSin ( _rtP -> P_1601 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1602 ) * _rtP -> P_1599 + _rtP -> P_1600
) * _rtB -> B_390_2024_0 [ 0 ] ; _rtB -> B_390_2068_0 = ( muDoubleScalarSin (
_rtP -> P_1605 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1606 ) * _rtP -> P_1603
+ _rtP -> P_1604 ) * _rtB -> B_390_2024_0 [ 1 ] ; _rtB -> B_390_2070_0 = (
muDoubleScalarSin ( _rtP -> P_1609 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1610
) * _rtP -> P_1607 + _rtP -> P_1608 ) * _rtB -> B_390_2024_0 [ 1 ] ; _rtB ->
B_390_2072_0 = ( muDoubleScalarSin ( _rtP -> P_1613 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1614 ) * _rtP -> P_1611 + _rtP -> P_1612 ) * _rtB -> B_390_2024_0
[ 2 ] ; _rtB -> B_390_2074_0 = ( muDoubleScalarSin ( _rtP -> P_1617 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1618 ) * _rtP -> P_1615 + _rtP -> P_1616
) * _rtB -> B_390_2024_0 [ 2 ] ; B_390_2075_0 = _rtP -> P_1619 *
B_390_401_1_idx_0 ; _rtB -> B_390_2077_0 = ( muDoubleScalarSin ( _rtP ->
P_1622 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1623 ) * _rtP -> P_1620 + _rtP
-> P_1621 ) * _rtB -> B_390_2028_0 [ 0 ] ; _rtB -> B_390_2079_0 = (
muDoubleScalarSin ( _rtP -> P_1626 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1627
) * _rtP -> P_1624 + _rtP -> P_1625 ) * _rtB -> B_390_2028_0 [ 0 ] ; _rtB ->
B_390_2081_0 = ( muDoubleScalarSin ( _rtP -> P_1630 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1631 ) * _rtP -> P_1628 + _rtP -> P_1629 ) * _rtB -> B_390_2028_0
[ 1 ] ; _rtB -> B_390_2083_0 = ( muDoubleScalarSin ( _rtP -> P_1634 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1635 ) * _rtP -> P_1632 + _rtP -> P_1633
) * _rtB -> B_390_2028_0 [ 1 ] ; _rtB -> B_390_2085_0 = ( muDoubleScalarSin (
_rtP -> P_1638 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1639 ) * _rtP -> P_1636
+ _rtP -> P_1637 ) * _rtB -> B_390_2028_0 [ 2 ] ; _rtB -> B_390_2087_0 = (
muDoubleScalarSin ( _rtP -> P_1642 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1643
) * _rtP -> P_1640 + _rtP -> P_1641 ) * _rtB -> B_390_2028_0 [ 2 ] ; _rtB ->
B_390_2089_0 = ( muDoubleScalarSin ( _rtP -> P_1646 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1647 ) * _rtP -> P_1644 + _rtP -> P_1645 ) * _rtB -> B_390_2024_0
[ 0 ] ; _rtB -> B_390_2091_0 = ( muDoubleScalarSin ( _rtP -> P_1650 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1651 ) * _rtP -> P_1648 + _rtP -> P_1649
) * _rtB -> B_390_2024_0 [ 0 ] ; _rtB -> B_390_2093_0 = ( muDoubleScalarSin (
_rtP -> P_1654 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1655 ) * _rtP -> P_1652
+ _rtP -> P_1653 ) * _rtB -> B_390_2024_0 [ 1 ] ; _rtB -> B_390_2095_0 = (
muDoubleScalarSin ( _rtP -> P_1658 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1659
) * _rtP -> P_1656 + _rtP -> P_1657 ) * _rtB -> B_390_2024_0 [ 1 ] ; _rtB ->
B_390_2097_0 = ( muDoubleScalarSin ( _rtP -> P_1662 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1663 ) * _rtP -> P_1660 + _rtP -> P_1661 ) * _rtB -> B_390_2024_0
[ 2 ] ; _rtB -> B_390_2099_0 = ( muDoubleScalarSin ( _rtP -> P_1666 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1667 ) * _rtP -> P_1664 + _rtP -> P_1665
) * _rtB -> B_390_2024_0 [ 2 ] ; _rtB -> B_390_2101_0 = ( muDoubleScalarSin (
_rtP -> P_1670 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1671 ) * _rtP -> P_1668
+ _rtP -> P_1669 ) * _rtB -> B_390_2028_0 [ 0 ] ; _rtB -> B_390_2103_0 = (
muDoubleScalarSin ( _rtP -> P_1674 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1675
) * _rtP -> P_1672 + _rtP -> P_1673 ) * _rtB -> B_390_2028_0 [ 0 ] ; _rtB ->
B_390_2105_0 = ( muDoubleScalarSin ( _rtP -> P_1678 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1679 ) * _rtP -> P_1676 + _rtP -> P_1677 ) * _rtB -> B_390_2028_0
[ 1 ] ; _rtB -> B_390_2107_0 = ( muDoubleScalarSin ( _rtP -> P_1682 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1683 ) * _rtP -> P_1680 + _rtP -> P_1681
) * _rtB -> B_390_2028_0 [ 1 ] ; _rtB -> B_390_2109_0 = ( muDoubleScalarSin (
_rtP -> P_1686 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1687 ) * _rtP -> P_1684
+ _rtP -> P_1685 ) * _rtB -> B_390_2028_0 [ 2 ] ; _rtB -> B_390_2111_0 = (
muDoubleScalarSin ( _rtP -> P_1690 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1691
) * _rtP -> P_1688 + _rtP -> P_1689 ) * _rtB -> B_390_2028_0 [ 2 ] ;
B_390_2112_0 = _rtP -> P_1692 * rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 2
, 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_2115_0 >= _rtP -> P_1698 ) {
rtb_B_390_2118_0 = _rtB -> B_390_2114_0 ; } else { rtb_B_390_2118_0 =
rt_Lookup ( _rtP -> P_1696 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_1697 ) ;
} if ( _rtB -> B_390_2119_0 >= _rtP -> P_1701 ) { _rtB -> B_390_2122_0 =
rtb_B_390_2118_0 ; } else { _rtB -> B_390_2122_0 = _rtB -> B_390_2120_0 ; } }
_rtB -> B_390_2124_0 = rt_Lookup ( _rtP -> P_1702 , 5 , ssGetT ( S ) , _rtP
-> P_1703 ) ; if ( _rtB -> B_390_2113_0 >= _rtP -> P_1704 ) { _rtB ->
B_390_2125_0 = _rtB -> B_390_2122_0 ; } else { _rtB -> B_390_2125_0 = _rtB ->
B_390_2124_0 ; } if ( ( _rtDW -> TimeStampA_b >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_e >= ssGetT ( S ) ) ) { _rtB -> B_390_2126_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_b ; lastU = & _rtDW -> LastUAtTimeA_h ;
if ( _rtDW -> TimeStampA_b < _rtDW -> TimeStampB_e ) { if ( _rtDW ->
TimeStampB_e < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_e ;
lastU = & _rtDW -> LastUAtTimeB_p ; } } else { if ( _rtDW -> TimeStampA_b >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_e ; lastU = & _rtDW ->
LastUAtTimeB_p ; } } _rtB -> B_390_2126_0 = ( _rtB -> B_390_2124_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_390_2132_0 >= _rtP -> P_1708 ) { _rtB
-> B_390_2134_0 = rtb_B_390_2118_0 ; } else { _rtB -> B_390_2134_0 = _rtB ->
B_390_2120_0 ; } } _rtB -> B_390_2136_0 = rt_Lookup ( _rtP -> P_1709 , 5 ,
ssGetT ( S ) , _rtP -> P_1710 ) ; if ( _rtB -> B_390_2131_0 >= _rtP -> P_1711
) { _rtB -> B_390_2137_0 = _rtB -> B_390_2134_0 ; } else { _rtB ->
B_390_2137_0 = _rtB -> B_390_2136_0 ; } if ( ( _rtDW -> TimeStampA_i >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_m >= ssGetT ( S ) ) ) { _rtB ->
B_390_2138_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_i ; lastU
= & _rtDW -> LastUAtTimeA_p ; if ( _rtDW -> TimeStampA_i < _rtDW ->
TimeStampB_m ) { if ( _rtDW -> TimeStampB_m < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_m ; lastU = & _rtDW -> LastUAtTimeB_h ;
} } else { if ( _rtDW -> TimeStampA_i >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_m ; lastU = & _rtDW -> LastUAtTimeB_h ; } } _rtB ->
B_390_2138_0 = ( _rtB -> B_390_2136_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_2144_0 >= _rtP -> P_1715 ) { _rtB -> B_390_2146_0 =
rtb_B_390_2118_0 ; } else { _rtB -> B_390_2146_0 = _rtB -> B_390_2120_0 ; } }
_rtB -> B_390_2148_0 = rt_Lookup ( _rtP -> P_1716 , 5 , ssGetT ( S ) , _rtP
-> P_1717 ) ; if ( _rtB -> B_390_2143_0 >= _rtP -> P_1718 ) { _rtB ->
B_390_2149_0 = _rtB -> B_390_2146_0 ; } else { _rtB -> B_390_2149_0 = _rtB ->
B_390_2148_0 ; } if ( ( _rtDW -> TimeStampA_h >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_id >= ssGetT ( S ) ) ) { _rtB -> B_390_2150_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_h ; lastU = & _rtDW -> LastUAtTimeA_cg
; if ( _rtDW -> TimeStampA_h < _rtDW -> TimeStampB_id ) { if ( _rtDW ->
TimeStampB_id < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_id ;
lastU = & _rtDW -> LastUAtTimeB_e ; } } else { if ( _rtDW -> TimeStampA_h >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_id ; lastU = & _rtDW
-> LastUAtTimeB_e ; } } _rtB -> B_390_2150_0 = ( _rtB -> B_390_2148_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } _rtB -> B_390_2167_0 = _rtX
-> integrator_CSTATE_pb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ph . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1721 ;
B_390_2168_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_be .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_be . Last , _rtDW ->
TransportDelay_IWORK_be . Tail , _rtDW -> TransportDelay_IWORK_be . Head ,
_rtP -> P_1722 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2170_0 = _rtP -> P_1723 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2170_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2172_0 =
_rtDW -> Memory_PreviousInput_kb ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2174_0 = ( _rtB -> B_390_2167_0 - B_390_2168_0 ) * _rtP -> P_157 ; }
else { _rtB -> B_390_2174_0 = _rtB -> B_390_2172_0 ; } _rtB -> B_390_2175_0 =
_rtX -> integrator_CSTATE_os ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_eq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1726 ;
B_390_2176_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_op .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_op . Last , _rtDW ->
TransportDelay_IWORK_op . Tail , _rtDW -> TransportDelay_IWORK_op . Head ,
_rtP -> P_1727 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2178_0 = _rtP -> P_1728 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2178_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2180_0 =
_rtDW -> Memory_PreviousInput_hs ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2182_0 = ( _rtB -> B_390_2175_0 - B_390_2176_0 ) * _rtP -> P_156 ; }
else { _rtB -> B_390_2182_0 = _rtB -> B_390_2180_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1730 * muDoubleScalarAtan2 ( _rtB -> B_390_2182_0 , _rtB ->
B_390_2174_0 ) * _rtP -> P_1731 ; B_390_2187_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2174_0 , _rtB -> B_390_2182_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2187_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2174_0 , _rtB -> B_390_2182_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2188_0 = _rtX -> integrator_CSTATE_ju ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1733 ; B_390_2189_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ma . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ma . Last , _rtDW -> TransportDelay_IWORK_ma . Tail ,
_rtDW -> TransportDelay_IWORK_ma . Head , _rtP -> P_1734 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2191_0 = _rtP -> P_1735 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2191_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2193_0 = _rtDW -> Memory_PreviousInput_dv ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2195_0 = ( _rtB -> B_390_2188_0 -
B_390_2189_0 ) * _rtP -> P_159 ; } else { _rtB -> B_390_2195_0 = _rtB ->
B_390_2193_0 ; } _rtB -> B_390_2196_0 = _rtX -> integrator_CSTATE_a5 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m0c . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1738 ; B_390_2197_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ov . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ov . Last , _rtDW -> TransportDelay_IWORK_ov . Tail ,
_rtDW -> TransportDelay_IWORK_ov . Head , _rtP -> P_1739 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2199_0 = _rtP -> P_1740 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2199_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2201_0 = _rtDW -> Memory_PreviousInput_gg ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2203_0 = ( _rtB -> B_390_2196_0 -
B_390_2197_0 ) * _rtP -> P_158 ; } else { _rtB -> B_390_2203_0 = _rtB ->
B_390_2201_0 ; } rtb_B_390_2118_0 = _rtP -> P_1742 * muDoubleScalarAtan2 (
_rtB -> B_390_2203_0 , _rtB -> B_390_2195_0 ) * _rtP -> P_1743 ; B_390_2208_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2195_0 , _rtB -> B_390_2203_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2208_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2195_0 , _rtB -> B_390_2203_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2209_0 = _rtX ->
integrator_CSTATE_pbj ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_do . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_do . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1745 ;
B_390_2210_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hr . Last , _rtDW ->
TransportDelay_IWORK_hr . Tail , _rtDW -> TransportDelay_IWORK_hr . Head ,
_rtP -> P_1746 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2212_0 = _rtP -> P_1747 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2212_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2214_0 =
_rtDW -> Memory_PreviousInput_nb ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2216_0 = ( _rtB -> B_390_2209_0 - B_390_2210_0 ) * _rtP -> P_161 ; }
else { _rtB -> B_390_2216_0 = _rtB -> B_390_2214_0 ; } _rtB -> B_390_2217_0 =
_rtX -> integrator_CSTATE_mu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_hi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1750 ;
B_390_2218_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_as .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_as . Last , _rtDW ->
TransportDelay_IWORK_as . Tail , _rtDW -> TransportDelay_IWORK_as . Head ,
_rtP -> P_1751 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2220_0 = _rtP -> P_1752 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2220_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2222_0 =
_rtDW -> Memory_PreviousInput_l1 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2224_0 = ( _rtB -> B_390_2217_0 - B_390_2218_0 ) * _rtP -> P_160 ; }
else { _rtB -> B_390_2224_0 = _rtB -> B_390_2222_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1754 * muDoubleScalarAtan2 ( _rtB -> B_390_2224_0 , _rtB ->
B_390_2216_0 ) * _rtP -> P_1755 ; B_390_2229_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2216_0 , _rtB -> B_390_2224_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2229_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2216_0 , _rtB -> B_390_2224_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2231_0 = _rtB -> B_390_2230_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_2231_0 , B_390_2187_0 , B_390_2208_0 , B_390_2229_0 , &
_rtB -> PosSeqComputation_k , & _rtDW -> PosSeqComputation_k , & _rtP ->
PosSeqComputation_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_2234_0 = _rtB -> B_390_2233_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2234_0 ,
B_390_2187_0 , B_390_2208_0 , B_390_2229_0 , & _rtB -> NegSeqComputation_l ,
& _rtDW -> NegSeqComputation_l , & _rtP -> NegSeqComputation_l ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2237_0 = _rtB
-> B_390_2236_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_2237_0 , B_390_2187_0 , B_390_2208_0 , B_390_2229_0 , & _rtB ->
ZeroSeqComputation_m , & _rtDW -> ZeroSeqComputation_m , & _rtP ->
ZeroSeqComputation_m ) ; rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_k . B_22_2_0 . re , _rtB -> PosSeqComputation_k . B_22_2_0
. im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_k .
B_22_2_0 . im , _rtB -> PosSeqComputation_k . B_22_2_0 . re ) ; _rtB ->
B_390_2240_0 = _rtX -> integrator_CSTATE_gd ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cv . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cv . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_1760 ; B_390_2241_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_iw
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_iw . Last , _rtDW ->
TransportDelay_IWORK_iw . Tail , _rtDW -> TransportDelay_IWORK_iw . Head ,
_rtP -> P_1761 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2243_0 = _rtP -> P_1762 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2243_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2245_0 =
_rtDW -> Memory_PreviousInput_ja ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2247_0 = ( _rtB -> B_390_2240_0 - B_390_2241_0 ) * _rtP -> P_163 ; }
else { _rtB -> B_390_2247_0 = _rtB -> B_390_2245_0 ; } _rtB -> B_390_2248_0 =
_rtX -> integrator_CSTATE_l5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ai . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ai . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1765 ;
B_390_2249_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dq . Last , _rtDW ->
TransportDelay_IWORK_dq . Tail , _rtDW -> TransportDelay_IWORK_dq . Head ,
_rtP -> P_1766 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2251_0 = _rtP -> P_1767 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2251_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2253_0 =
_rtDW -> Memory_PreviousInput_gr ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2255_0 = ( _rtB -> B_390_2248_0 - B_390_2249_0 ) * _rtP -> P_162 ; }
else { _rtB -> B_390_2255_0 = _rtB -> B_390_2253_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1769 * muDoubleScalarAtan2 ( _rtB -> B_390_2255_0 , _rtB ->
B_390_2247_0 ) * _rtP -> P_1770 ; B_390_2260_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2247_0 , _rtB -> B_390_2255_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2260_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2247_0 , _rtB -> B_390_2255_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2261_0 = _rtX -> integrator_CSTATE_hq ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1772 ; B_390_2262_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_os . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_os . Last , _rtDW -> TransportDelay_IWORK_os . Tail ,
_rtDW -> TransportDelay_IWORK_os . Head , _rtP -> P_1773 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2264_0 = _rtP -> P_1774 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2264_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2266_0 = _rtDW -> Memory_PreviousInput_c ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2268_0 = ( _rtB -> B_390_2261_0 -
B_390_2262_0 ) * _rtP -> P_165 ; } else { _rtB -> B_390_2268_0 = _rtB ->
B_390_2266_0 ; } _rtB -> B_390_2269_0 = _rtX -> integrator_CSTATE_cb ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nvu . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1777 ; B_390_2270_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_pi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pi . Last , _rtDW -> TransportDelay_IWORK_pi . Tail ,
_rtDW -> TransportDelay_IWORK_pi . Head , _rtP -> P_1778 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2272_0 = _rtP -> P_1779 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2272_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2274_0 = _rtDW -> Memory_PreviousInput_mt ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2276_0 = ( _rtB -> B_390_2269_0 -
B_390_2270_0 ) * _rtP -> P_164 ; } else { _rtB -> B_390_2276_0 = _rtB ->
B_390_2274_0 ; } rtb_B_390_2118_0 = _rtP -> P_1781 * muDoubleScalarAtan2 (
_rtB -> B_390_2276_0 , _rtB -> B_390_2268_0 ) * _rtP -> P_1782 ; B_390_2281_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2268_0 , _rtB -> B_390_2276_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2281_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2268_0 , _rtB -> B_390_2276_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2282_0 = _rtX ->
integrator_CSTATE_po ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_mh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_mh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1784 ;
B_390_2283_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jo .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jo . Last , _rtDW ->
TransportDelay_IWORK_jo . Tail , _rtDW -> TransportDelay_IWORK_jo . Head ,
_rtP -> P_1785 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2285_0 = _rtP -> P_1786 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2285_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2287_0 =
_rtDW -> Memory_PreviousInput_nr ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2289_0 = ( _rtB -> B_390_2282_0 - B_390_2283_0 ) * _rtP -> P_167 ; }
else { _rtB -> B_390_2289_0 = _rtB -> B_390_2287_0 ; } _rtB -> B_390_2290_0 =
_rtX -> integrator_CSTATE_ag ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_my . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_my . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1789 ;
B_390_2291_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_i2 . Last , _rtDW ->
TransportDelay_IWORK_i2 . Tail , _rtDW -> TransportDelay_IWORK_i2 . Head ,
_rtP -> P_1790 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2293_0 = _rtP -> P_1791 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2293_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2295_0 =
_rtDW -> Memory_PreviousInput_jr ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2297_0 = ( _rtB -> B_390_2290_0 - B_390_2291_0 ) * _rtP -> P_166 ; }
else { _rtB -> B_390_2297_0 = _rtB -> B_390_2295_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1793 * muDoubleScalarAtan2 ( _rtB -> B_390_2297_0 , _rtB ->
B_390_2289_0 ) * _rtP -> P_1794 ; B_390_2302_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2289_0 , _rtB -> B_390_2297_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2302_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2289_0 , _rtB -> B_390_2297_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2304_0 = _rtB -> B_390_2303_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_2304_0 , B_390_2260_0 , B_390_2281_0 , B_390_2302_0 , &
_rtB -> PosSeqComputation_c , & _rtDW -> PosSeqComputation_c , & _rtP ->
PosSeqComputation_c ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_2307_0 = _rtB -> B_390_2306_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2307_0 ,
B_390_2260_0 , B_390_2281_0 , B_390_2302_0 , & _rtB -> NegSeqComputation_lv ,
& _rtDW -> NegSeqComputation_lv , & _rtP -> NegSeqComputation_lv ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2310_0 = _rtB
-> B_390_2309_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_2310_0 , B_390_2260_0 , B_390_2281_0 , B_390_2302_0 , & _rtB ->
ZeroSeqComputation_mr , & _rtDW -> ZeroSeqComputation_mr , & _rtP ->
ZeroSeqComputation_mr ) ; rtb_B_390_2118_0 = rtb_B_390_239_0 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation_c . B_22_2_0 . re , _rtB ->
PosSeqComputation_c . B_22_2_0 . im ) * _rtP -> P_1798 ; muDoubleScalarSinCos
( _rtP -> P_1801 * ( _rtP -> P_1799 * rtb_B_390_4_1 - _rtP -> P_1800 *
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_c . B_22_2_0 . im , _rtB ->
PosSeqComputation_c . B_22_2_0 . re ) ) , & rtb_B_390_1596_0 , &
rtb_B_390_422_0 ) ; _rtB -> B_390_2320_0 = rtb_B_390_2118_0 * rtb_B_390_422_0
; _rtB -> B_390_2321_0 = _rtX -> integrator_CSTATE_ba ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cct . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cct .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1803 ; B_390_2322_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_pd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pd . Last , _rtDW -> TransportDelay_IWORK_pd . Tail ,
_rtDW -> TransportDelay_IWORK_pd . Head , _rtP -> P_1804 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2324_0 = _rtP -> P_1805 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2324_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2326_0 = _rtDW -> Memory_PreviousInput_ex ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2328_0 = ( _rtB -> B_390_2321_0 -
B_390_2322_0 ) * _rtP -> P_169 ; } else { _rtB -> B_390_2328_0 = _rtB ->
B_390_2326_0 ; } _rtB -> B_390_2329_0 = _rtX -> integrator_CSTATE_j4 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fk .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fk . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1808 ; B_390_2330_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_n2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n2 . Last , _rtDW -> TransportDelay_IWORK_n2 . Tail ,
_rtDW -> TransportDelay_IWORK_n2 . Head , _rtP -> P_1809 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2332_0 = _rtP -> P_1810 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2332_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2334_0 = _rtDW -> Memory_PreviousInput_aa ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2336_0 = ( _rtB -> B_390_2329_0 -
B_390_2330_0 ) * _rtP -> P_168 ; } else { _rtB -> B_390_2336_0 = _rtB ->
B_390_2334_0 ; } rtb_B_390_422_0 = _rtP -> P_1812 * muDoubleScalarAtan2 (
_rtB -> B_390_2336_0 , _rtB -> B_390_2328_0 ) * _rtP -> P_1813 ; B_390_2341_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2328_0 , _rtB -> B_390_2336_0 ) *
muDoubleScalarCos ( rtb_B_390_422_0 ) ; B_390_2341_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2328_0 , _rtB -> B_390_2336_0 ) *
muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_2342_0 = _rtX ->
integrator_CSTATE_oi ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_du . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_du . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1815 ;
B_390_2343_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lc .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lc . Last , _rtDW ->
TransportDelay_IWORK_lc . Tail , _rtDW -> TransportDelay_IWORK_lc . Head ,
_rtP -> P_1816 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2345_0 = _rtP -> P_1817 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2345_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2347_0 =
_rtDW -> Memory_PreviousInput_bc ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2349_0 = ( _rtB -> B_390_2342_0 - B_390_2343_0 ) * _rtP -> P_171 ; }
else { _rtB -> B_390_2349_0 = _rtB -> B_390_2347_0 ; } _rtB -> B_390_2350_0 =
_rtX -> integrator_CSTATE_gb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_oj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1820 ;
B_390_2351_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_el .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_el . Last , _rtDW ->
TransportDelay_IWORK_el . Tail , _rtDW -> TransportDelay_IWORK_el . Head ,
_rtP -> P_1821 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2353_0 = _rtP -> P_1822 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2353_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2355_0 =
_rtDW -> Memory_PreviousInput_mo ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2357_0 = ( _rtB -> B_390_2350_0 - B_390_2351_0 ) * _rtP -> P_170 ; }
else { _rtB -> B_390_2357_0 = _rtB -> B_390_2355_0 ; } rtb_B_390_422_0 = _rtP
-> P_1824 * muDoubleScalarAtan2 ( _rtB -> B_390_2357_0 , _rtB -> B_390_2349_0
) * _rtP -> P_1825 ; B_390_2362_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_2349_0 , _rtB -> B_390_2357_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_2362_0 . im = muDoubleScalarHypot ( _rtB -> B_390_2349_0 , _rtB ->
B_390_2357_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_2363_0
= _rtX -> integrator_CSTATE_kn ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1827
; B_390_2364_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jd . Last , _rtDW ->
TransportDelay_IWORK_jd . Tail , _rtDW -> TransportDelay_IWORK_jd . Head ,
_rtP -> P_1828 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2366_0 = _rtP -> P_1829 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2366_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2368_0 =
_rtDW -> Memory_PreviousInput_ct ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2370_0 = ( _rtB -> B_390_2363_0 - B_390_2364_0 ) * _rtP -> P_173 ; }
else { _rtB -> B_390_2370_0 = _rtB -> B_390_2368_0 ; } _rtB -> B_390_2371_0 =
_rtX -> integrator_CSTATE_dn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_na . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_na . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1832 ;
B_390_2372_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_md .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_md . Last , _rtDW ->
TransportDelay_IWORK_md . Tail , _rtDW -> TransportDelay_IWORK_md . Head ,
_rtP -> P_1833 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2374_0 = _rtP -> P_1834 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2374_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2376_0 =
_rtDW -> Memory_PreviousInput_ad ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2378_0 = ( _rtB -> B_390_2371_0 - B_390_2372_0 ) * _rtP -> P_172 ; }
else { _rtB -> B_390_2378_0 = _rtB -> B_390_2376_0 ; } rtb_B_390_422_0 = _rtP
-> P_1836 * muDoubleScalarAtan2 ( _rtB -> B_390_2378_0 , _rtB -> B_390_2370_0
) * _rtP -> P_1837 ; B_390_2383_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_2370_0 , _rtB -> B_390_2378_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_2383_0 . im = muDoubleScalarHypot ( _rtB -> B_390_2370_0 , _rtB ->
B_390_2378_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2385_0 = _rtB
-> B_390_2384_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2385_0 , B_390_2341_0 , B_390_2362_0 , B_390_2383_0 , & _rtB ->
PosSeqComputation_n , & _rtDW -> PosSeqComputation_n , & _rtP ->
PosSeqComputation_n ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_2388_0 = _rtB -> B_390_2387_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2388_0 ,
B_390_2341_0 , B_390_2362_0 , B_390_2383_0 , & _rtB -> NegSeqComputation_a ,
& _rtDW -> NegSeqComputation_a , & _rtP -> NegSeqComputation_a ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2391_0 = _rtB
-> B_390_2390_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_2391_0 , B_390_2341_0 , B_390_2362_0 , B_390_2383_0 , & _rtB ->
ZeroSeqComputation_c , & _rtDW -> ZeroSeqComputation_c , & _rtP ->
ZeroSeqComputation_c ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_n . B_22_2_0 . im , _rtB -> PosSeqComputation_n . B_22_2_0
. re ) ; _rtB -> B_390_2394_0 = _rtP -> P_1841 * muDoubleScalarHypot ( _rtB
-> PosSeqComputation_n . B_22_2_0 . re , _rtB -> PosSeqComputation_n .
B_22_2_0 . im ) ; _rtB -> B_390_2395_0 = rtb_B_390_2118_0 * rtb_B_390_1596_0
; _rtB -> B_390_2396_0 = _rtX -> integrator_CSTATE_bd ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1843 ; B_390_2397_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_fqe . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fqe . Last , _rtDW -> TransportDelay_IWORK_fqe . Tail ,
_rtDW -> TransportDelay_IWORK_fqe . Head , _rtP -> P_1844 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2399_0 = _rtP -> P_1845 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2399_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2401_0 = _rtDW -> Memory_PreviousInput_ow ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2403_0 = ( _rtB -> B_390_2396_0 -
B_390_2397_0 ) * _rtP -> P_151 ; } else { _rtB -> B_390_2403_0 = _rtB ->
B_390_2401_0 ; } _rtB -> B_390_2404_0 = _rtX -> integrator_CSTATE_mf ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fv .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fv . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1848 ; B_390_2405_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_m2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m2 . Last , _rtDW -> TransportDelay_IWORK_m2 . Tail ,
_rtDW -> TransportDelay_IWORK_m2 . Head , _rtP -> P_1849 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2407_0 = _rtP -> P_1850 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2407_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2409_0 = _rtDW -> Memory_PreviousInput_dp ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2411_0 = ( _rtB -> B_390_2404_0 -
B_390_2405_0 ) * _rtP -> P_150 ; } else { _rtB -> B_390_2411_0 = _rtB ->
B_390_2409_0 ; } rtb_B_390_2118_0 = _rtP -> P_1852 * muDoubleScalarAtan2 (
_rtB -> B_390_2411_0 , _rtB -> B_390_2403_0 ) * _rtP -> P_1853 ; B_390_2416_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2403_0 , _rtB -> B_390_2411_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2416_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2403_0 , _rtB -> B_390_2411_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2417_0 = _rtX ->
integrator_CSTATE_mt ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_nq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1855 ;
B_390_2418_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ko .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ko . Last , _rtDW ->
TransportDelay_IWORK_ko . Tail , _rtDW -> TransportDelay_IWORK_ko . Head ,
_rtP -> P_1856 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2420_0 = _rtP -> P_1857 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2420_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2422_0 =
_rtDW -> Memory_PreviousInput_a0 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2424_0 = ( _rtB -> B_390_2417_0 - B_390_2418_0 ) * _rtP -> P_153 ; }
else { _rtB -> B_390_2424_0 = _rtB -> B_390_2422_0 ; } _rtB -> B_390_2425_0 =
_rtX -> integrator_CSTATE_jq4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_cw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1860 ;
B_390_2426_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ld .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ld . Last , _rtDW ->
TransportDelay_IWORK_ld . Tail , _rtDW -> TransportDelay_IWORK_ld . Head ,
_rtP -> P_1861 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2428_0 = _rtP -> P_1862 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2428_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2430_0 =
_rtDW -> Memory_PreviousInput_lf ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2432_0 = ( _rtB -> B_390_2425_0 - B_390_2426_0 ) * _rtP -> P_152 ; }
else { _rtB -> B_390_2432_0 = _rtB -> B_390_2430_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1864 * muDoubleScalarAtan2 ( _rtB -> B_390_2432_0 , _rtB ->
B_390_2424_0 ) * _rtP -> P_1865 ; B_390_2437_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2424_0 , _rtB -> B_390_2432_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2437_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2424_0 , _rtB -> B_390_2432_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2438_0 = _rtX -> integrator_CSTATE_ez ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1867 ; B_390_2439_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_al . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_al . Last , _rtDW -> TransportDelay_IWORK_al . Tail ,
_rtDW -> TransportDelay_IWORK_al . Head , _rtP -> P_1868 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2441_0 = _rtP -> P_1869 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2441_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2443_0 = _rtDW -> Memory_PreviousInput_ce ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2445_0 = ( _rtB -> B_390_2438_0 -
B_390_2439_0 ) * _rtP -> P_155 ; } else { _rtB -> B_390_2445_0 = _rtB ->
B_390_2443_0 ; } _rtB -> B_390_2446_0 = _rtX -> integrator_CSTATE_ct ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1872 ; B_390_2447_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_jdp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jdp . Last , _rtDW -> TransportDelay_IWORK_jdp . Tail ,
_rtDW -> TransportDelay_IWORK_jdp . Head , _rtP -> P_1873 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2449_0 = _rtP -> P_1874 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2449_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2451_0 = _rtDW -> Memory_PreviousInput_ie ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2453_0 = ( _rtB -> B_390_2446_0 -
B_390_2447_0 ) * _rtP -> P_154 ; } else { _rtB -> B_390_2453_0 = _rtB ->
B_390_2451_0 ; } rtb_B_390_2118_0 = _rtP -> P_1876 * muDoubleScalarAtan2 (
_rtB -> B_390_2453_0 , _rtB -> B_390_2445_0 ) * _rtP -> P_1877 ; B_390_2458_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2445_0 , _rtB -> B_390_2453_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2458_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2445_0 , _rtB -> B_390_2453_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_2460_0 = _rtB -> B_390_2459_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2460_0 ,
B_390_2416_0 , B_390_2437_0 , B_390_2458_0 , & _rtB -> PosSeqComputation_em ,
& _rtDW -> PosSeqComputation_em , & _rtP -> PosSeqComputation_em ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2463_0 = _rtB
-> B_390_2462_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2463_0 , B_390_2416_0 , B_390_2437_0 , B_390_2458_0 , & _rtB ->
NegSeqComputation_g , & _rtDW -> NegSeqComputation_g , & _rtP ->
NegSeqComputation_g ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_2466_0 = _rtB -> B_390_2465_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_2466_0 ,
B_390_2416_0 , B_390_2437_0 , B_390_2458_0 , & _rtB -> ZeroSeqComputation_cx
, & _rtDW -> ZeroSeqComputation_cx , & _rtP -> ZeroSeqComputation_cx ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_em .
B_22_2_0 . im , _rtB -> PosSeqComputation_em . B_22_2_0 . re ) ; _rtB ->
B_390_2469_0 = _rtP -> P_1881 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_em . B_22_2_0 . re , _rtB -> PosSeqComputation_em .
B_22_2_0 . im ) ; ssCallAccelRunBlock ( S , 390 , 2470 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2474_0 [ 0 ] = _rtP -> P_1882 * _rtB -> B_0_14_0 [ 108 ] * _rtP ->
P_1885 ; _rtB -> B_390_2474_0 [ 1 ] = _rtP -> P_1883 * _rtB -> B_0_14_0 [ 109
] * _rtP -> P_1885 ; _rtB -> B_390_2474_0 [ 2 ] = _rtP -> P_1884 * _rtB ->
B_0_14_0 [ 110 ] * _rtP -> P_1885 ; _rtB -> B_390_2478_0 [ 0 ] = _rtP ->
P_1886 * _rtB -> B_0_14_0 [ 53 ] * _rtP -> P_1889 ; _rtB -> B_390_2478_0 [ 1
] = _rtP -> P_1887 * _rtB -> B_0_14_0 [ 54 ] * _rtP -> P_1889 ; _rtB ->
B_390_2478_0 [ 2 ] = _rtP -> P_1888 * _rtB -> B_0_14_0 [ 55 ] * _rtP ->
P_1889 ; } _rtB -> B_390_2514_0 = ( muDoubleScalarSin ( _rtP -> P_1892 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1893 ) * _rtP -> P_1890 + _rtP -> P_1891
) * _rtB -> B_390_2474_0 [ 0 ] ; _rtB -> B_390_2516_0 = ( muDoubleScalarSin (
_rtP -> P_1896 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1897 ) * _rtP -> P_1894
+ _rtP -> P_1895 ) * _rtB -> B_390_2474_0 [ 0 ] ; _rtB -> B_390_2518_0 = (
muDoubleScalarSin ( _rtP -> P_1900 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1901
) * _rtP -> P_1898 + _rtP -> P_1899 ) * _rtB -> B_390_2474_0 [ 1 ] ; _rtB ->
B_390_2520_0 = ( muDoubleScalarSin ( _rtP -> P_1904 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1905 ) * _rtP -> P_1902 + _rtP -> P_1903 ) * _rtB -> B_390_2474_0
[ 1 ] ; _rtB -> B_390_2522_0 = ( muDoubleScalarSin ( _rtP -> P_1908 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1909 ) * _rtP -> P_1906 + _rtP -> P_1907
) * _rtB -> B_390_2474_0 [ 2 ] ; _rtB -> B_390_2524_0 = ( muDoubleScalarSin (
_rtP -> P_1912 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1913 ) * _rtP -> P_1910
+ _rtP -> P_1911 ) * _rtB -> B_390_2474_0 [ 2 ] ; B_390_2525_0 = _rtP ->
P_1914 * B_390_401_1_idx_0 ; _rtB -> B_390_2527_0 = ( muDoubleScalarSin (
_rtP -> P_1917 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1918 ) * _rtP -> P_1915
+ _rtP -> P_1916 ) * _rtB -> B_390_2478_0 [ 0 ] ; _rtB -> B_390_2529_0 = (
muDoubleScalarSin ( _rtP -> P_1921 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1922
) * _rtP -> P_1919 + _rtP -> P_1920 ) * _rtB -> B_390_2478_0 [ 0 ] ; _rtB ->
B_390_2531_0 = ( muDoubleScalarSin ( _rtP -> P_1925 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1926 ) * _rtP -> P_1923 + _rtP -> P_1924 ) * _rtB -> B_390_2478_0
[ 1 ] ; _rtB -> B_390_2533_0 = ( muDoubleScalarSin ( _rtP -> P_1929 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1930 ) * _rtP -> P_1927 + _rtP -> P_1928
) * _rtB -> B_390_2478_0 [ 1 ] ; _rtB -> B_390_2535_0 = ( muDoubleScalarSin (
_rtP -> P_1933 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1934 ) * _rtP -> P_1931
+ _rtP -> P_1932 ) * _rtB -> B_390_2478_0 [ 2 ] ; _rtB -> B_390_2537_0 = (
muDoubleScalarSin ( _rtP -> P_1937 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1938
) * _rtP -> P_1935 + _rtP -> P_1936 ) * _rtB -> B_390_2478_0 [ 2 ] ; _rtB ->
B_390_2539_0 = ( muDoubleScalarSin ( _rtP -> P_1941 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1942 ) * _rtP -> P_1939 + _rtP -> P_1940 ) * _rtB -> B_390_2474_0
[ 0 ] ; _rtB -> B_390_2541_0 = ( muDoubleScalarSin ( _rtP -> P_1945 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1946 ) * _rtP -> P_1943 + _rtP -> P_1944
) * _rtB -> B_390_2474_0 [ 0 ] ; _rtB -> B_390_2543_0 = ( muDoubleScalarSin (
_rtP -> P_1949 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1950 ) * _rtP -> P_1947
+ _rtP -> P_1948 ) * _rtB -> B_390_2474_0 [ 1 ] ; _rtB -> B_390_2545_0 = (
muDoubleScalarSin ( _rtP -> P_1953 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1954
) * _rtP -> P_1951 + _rtP -> P_1952 ) * _rtB -> B_390_2474_0 [ 1 ] ; _rtB ->
B_390_2547_0 = ( muDoubleScalarSin ( _rtP -> P_1957 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1958 ) * _rtP -> P_1955 + _rtP -> P_1956 ) * _rtB -> B_390_2474_0
[ 2 ] ; _rtB -> B_390_2549_0 = ( muDoubleScalarSin ( _rtP -> P_1961 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1962 ) * _rtP -> P_1959 + _rtP -> P_1960
) * _rtB -> B_390_2474_0 [ 2 ] ; _rtB -> B_390_2551_0 = ( muDoubleScalarSin (
_rtP -> P_1965 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1966 ) * _rtP -> P_1963
+ _rtP -> P_1964 ) * _rtB -> B_390_2478_0 [ 0 ] ; _rtB -> B_390_2553_0 = (
muDoubleScalarSin ( _rtP -> P_1969 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1970
) * _rtP -> P_1967 + _rtP -> P_1968 ) * _rtB -> B_390_2478_0 [ 0 ] ; _rtB ->
B_390_2555_0 = ( muDoubleScalarSin ( _rtP -> P_1973 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1974 ) * _rtP -> P_1971 + _rtP -> P_1972 ) * _rtB -> B_390_2478_0
[ 1 ] ; _rtB -> B_390_2557_0 = ( muDoubleScalarSin ( _rtP -> P_1977 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1978 ) * _rtP -> P_1975 + _rtP -> P_1976
) * _rtB -> B_390_2478_0 [ 1 ] ; _rtB -> B_390_2559_0 = ( muDoubleScalarSin (
_rtP -> P_1981 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1982 ) * _rtP -> P_1979
+ _rtP -> P_1980 ) * _rtB -> B_390_2478_0 [ 2 ] ; _rtB -> B_390_2561_0 = (
muDoubleScalarSin ( _rtP -> P_1985 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1986
) * _rtP -> P_1983 + _rtP -> P_1984 ) * _rtB -> B_390_2478_0 [ 2 ] ;
B_390_2562_0 = _rtP -> P_1987 * rtb_B_390_4_1 ; _rtB -> B_390_2567_0 = _rtX
-> integrator_CSTATE_bm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_mg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_mg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1989 ;
B_390_2568_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_grh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_grh . Last , _rtDW ->
TransportDelay_IWORK_grh . Tail , _rtDW -> TransportDelay_IWORK_grh . Head ,
_rtP -> P_1990 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2570_0 = _rtP -> P_1991 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2570_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2572_0 =
_rtDW -> Memory_PreviousInput_fmc ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2574_0 = ( _rtB -> B_390_2567_0 - B_390_2568_0 ) * _rtP -> P_229 ; }
else { _rtB -> B_390_2574_0 = _rtB -> B_390_2572_0 ; } _rtB -> B_390_2575_0 =
_rtX -> integrator_CSTATE_hk ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1994 ;
B_390_2576_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_b2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_b2 . Last , _rtDW ->
TransportDelay_IWORK_b2 . Tail , _rtDW -> TransportDelay_IWORK_b2 . Head ,
_rtP -> P_1995 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2578_0 = _rtP -> P_1996 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2578_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2580_0 =
_rtDW -> Memory_PreviousInput_dm ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2582_0 = ( _rtB -> B_390_2575_0 - B_390_2576_0 ) * _rtP -> P_228 ; }
else { _rtB -> B_390_2582_0 = _rtB -> B_390_2580_0 ; } rtb_B_390_2118_0 =
_rtP -> P_1998 * muDoubleScalarAtan2 ( _rtB -> B_390_2582_0 , _rtB ->
B_390_2574_0 ) * _rtP -> P_1999 ; B_390_2587_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2574_0 , _rtB -> B_390_2582_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2587_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2574_0 , _rtB -> B_390_2582_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2588_0 = _rtX -> integrator_CSTATE_ia ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2001 ; B_390_2589_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_e2b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2b . Last , _rtDW -> TransportDelay_IWORK_e2b . Tail ,
_rtDW -> TransportDelay_IWORK_e2b . Head , _rtP -> P_2002 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2591_0 = _rtP -> P_2003 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2591_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2593_0 = _rtDW -> Memory_PreviousInput_js ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2595_0 = ( _rtB -> B_390_2588_0 -
B_390_2589_0 ) * _rtP -> P_231 ; } else { _rtB -> B_390_2595_0 = _rtB ->
B_390_2593_0 ; } _rtB -> B_390_2596_0 = _rtX -> integrator_CSTATE_eq ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ns .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ns . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2006 ; B_390_2597_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_mz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mz . Last , _rtDW -> TransportDelay_IWORK_mz . Tail ,
_rtDW -> TransportDelay_IWORK_mz . Head , _rtP -> P_2007 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2599_0 = _rtP -> P_2008 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2599_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2601_0 = _rtDW -> Memory_PreviousInput_o2 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2603_0 = ( _rtB -> B_390_2596_0 -
B_390_2597_0 ) * _rtP -> P_230 ; } else { _rtB -> B_390_2603_0 = _rtB ->
B_390_2601_0 ; } rtb_B_390_2118_0 = _rtP -> P_2010 * muDoubleScalarAtan2 (
_rtB -> B_390_2603_0 , _rtB -> B_390_2595_0 ) * _rtP -> P_2011 ; B_390_2608_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2595_0 , _rtB -> B_390_2603_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2608_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2595_0 , _rtB -> B_390_2603_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2609_0 = _rtX ->
integrator_CSTATE_af ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2013 ;
B_390_2610_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fg .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_fg . Last , _rtDW ->
TransportDelay_IWORK_fg . Tail , _rtDW -> TransportDelay_IWORK_fg . Head ,
_rtP -> P_2014 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2612_0 = _rtP -> P_2015 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2612_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2614_0 =
_rtDW -> Memory_PreviousInput_eq5 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2616_0 = ( _rtB -> B_390_2609_0 - B_390_2610_0 ) * _rtP -> P_233 ; }
else { _rtB -> B_390_2616_0 = _rtB -> B_390_2614_0 ; } _rtB -> B_390_2617_0 =
_rtX -> integrator_CSTATE_kxp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_da . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_da . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2018 ;
B_390_2618_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ji .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ji . Last , _rtDW ->
TransportDelay_IWORK_ji . Tail , _rtDW -> TransportDelay_IWORK_ji . Head ,
_rtP -> P_2019 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2620_0 = _rtP -> P_2020 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2620_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2622_0 =
_rtDW -> Memory_PreviousInput_b1 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2624_0 = ( _rtB -> B_390_2617_0 - B_390_2618_0 ) * _rtP -> P_232 ; }
else { _rtB -> B_390_2624_0 = _rtB -> B_390_2622_0 ; } rtb_B_390_2118_0 =
_rtP -> P_2022 * muDoubleScalarAtan2 ( _rtB -> B_390_2624_0 , _rtB ->
B_390_2616_0 ) * _rtP -> P_2023 ; B_390_2629_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2616_0 , _rtB -> B_390_2624_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2629_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2616_0 , _rtB -> B_390_2624_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2631_0 = _rtB -> B_390_2630_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_2631_0 , B_390_2587_0 , B_390_2608_0 , B_390_2629_0 , &
_rtB -> PosSeqComputation_du , & _rtDW -> PosSeqComputation_du , & _rtP ->
PosSeqComputation_du ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_2634_0 = _rtB -> B_390_2633_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2634_0 ,
B_390_2587_0 , B_390_2608_0 , B_390_2629_0 , & _rtB -> NegSeqComputation_le ,
& _rtDW -> NegSeqComputation_le , & _rtP -> NegSeqComputation_le ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2637_0 = _rtB
-> B_390_2636_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_2637_0 , B_390_2587_0 , B_390_2608_0 , B_390_2629_0 , & _rtB ->
ZeroSeqComputation_ow , & _rtDW -> ZeroSeqComputation_ow , & _rtP ->
ZeroSeqComputation_ow ) ; rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_du . B_22_2_0 . re , _rtB -> PosSeqComputation_du .
B_22_2_0 . im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_du . B_22_2_0 . im , _rtB -> PosSeqComputation_du .
B_22_2_0 . re ) ; _rtB -> B_390_2640_0 = _rtX -> integrator_CSTATE_hrr ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2028 ; B_390_2641_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ln . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ln . Last , _rtDW -> TransportDelay_IWORK_ln . Tail ,
_rtDW -> TransportDelay_IWORK_ln . Head , _rtP -> P_2029 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2643_0 = _rtP -> P_2030 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2643_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2645_0 = _rtDW -> Memory_PreviousInput_pzf ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2647_0 = ( _rtB -> B_390_2640_0 -
B_390_2641_0 ) * _rtP -> P_235 ; } else { _rtB -> B_390_2647_0 = _rtB ->
B_390_2645_0 ; } _rtB -> B_390_2648_0 = _rtX -> integrator_CSTATE_e4 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iu .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_iu . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2033 ; B_390_2649_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_po . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_po . Last , _rtDW -> TransportDelay_IWORK_po . Tail ,
_rtDW -> TransportDelay_IWORK_po . Head , _rtP -> P_2034 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2651_0 = _rtP -> P_2035 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2651_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2653_0 = _rtDW -> Memory_PreviousInput_fc ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2655_0 = ( _rtB -> B_390_2648_0 -
B_390_2649_0 ) * _rtP -> P_234 ; } else { _rtB -> B_390_2655_0 = _rtB ->
B_390_2653_0 ; } rtb_B_390_2118_0 = _rtP -> P_2037 * muDoubleScalarAtan2 (
_rtB -> B_390_2655_0 , _rtB -> B_390_2647_0 ) * _rtP -> P_2038 ; B_390_2660_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2647_0 , _rtB -> B_390_2655_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2660_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2647_0 , _rtB -> B_390_2655_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2661_0 = _rtX ->
integrator_CSTATE_otl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_nw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2040 ;
B_390_2662_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nl . Last , _rtDW ->
TransportDelay_IWORK_nl . Tail , _rtDW -> TransportDelay_IWORK_nl . Head ,
_rtP -> P_2041 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2664_0 = _rtP -> P_2042 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2664_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2666_0 =
_rtDW -> Memory_PreviousInput_jd ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2668_0 = ( _rtB -> B_390_2661_0 - B_390_2662_0 ) * _rtP -> P_237 ; }
else { _rtB -> B_390_2668_0 = _rtB -> B_390_2666_0 ; } _rtB -> B_390_2669_0 =
_rtX -> integrator_CSTATE_pv ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_f0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2045 ;
B_390_2670_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_px .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_px . Last , _rtDW ->
TransportDelay_IWORK_px . Tail , _rtDW -> TransportDelay_IWORK_px . Head ,
_rtP -> P_2046 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2672_0 = _rtP -> P_2047 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2672_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2674_0 =
_rtDW -> Memory_PreviousInput_mw ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2676_0 = ( _rtB -> B_390_2669_0 - B_390_2670_0 ) * _rtP -> P_236 ; }
else { _rtB -> B_390_2676_0 = _rtB -> B_390_2674_0 ; } rtb_B_390_2118_0 =
_rtP -> P_2049 * muDoubleScalarAtan2 ( _rtB -> B_390_2676_0 , _rtB ->
B_390_2668_0 ) * _rtP -> P_2050 ; B_390_2681_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2668_0 , _rtB -> B_390_2676_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2681_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2668_0 , _rtB -> B_390_2676_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2682_0 = _rtX -> integrator_CSTATE_i5 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_io . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_io
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2052 ; B_390_2683_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kx . Last , _rtDW -> TransportDelay_IWORK_kx . Tail ,
_rtDW -> TransportDelay_IWORK_kx . Head , _rtP -> P_2053 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2685_0 = _rtP -> P_2054 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2685_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2687_0 = _rtDW -> Memory_PreviousInput_aa3 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2689_0 = ( _rtB -> B_390_2682_0 -
B_390_2683_0 ) * _rtP -> P_239 ; } else { _rtB -> B_390_2689_0 = _rtB ->
B_390_2687_0 ; } _rtB -> B_390_2690_0 = _rtX -> integrator_CSTATE_dsl ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2057 ; B_390_2691_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ff . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ff . Last , _rtDW -> TransportDelay_IWORK_ff . Tail ,
_rtDW -> TransportDelay_IWORK_ff . Head , _rtP -> P_2058 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2693_0 = _rtP -> P_2059 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2693_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2695_0 = _rtDW -> Memory_PreviousInput_dj ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2697_0 = ( _rtB -> B_390_2690_0 -
B_390_2691_0 ) * _rtP -> P_238 ; } else { _rtB -> B_390_2697_0 = _rtB ->
B_390_2695_0 ; } rtb_B_390_2118_0 = _rtP -> P_2061 * muDoubleScalarAtan2 (
_rtB -> B_390_2697_0 , _rtB -> B_390_2689_0 ) * _rtP -> P_2062 ; B_390_2702_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2689_0 , _rtB -> B_390_2697_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2702_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2689_0 , _rtB -> B_390_2697_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_2704_0 = _rtB -> B_390_2703_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2704_0 ,
B_390_2660_0 , B_390_2681_0 , B_390_2702_0 , & _rtB -> PosSeqComputation_pf ,
& _rtDW -> PosSeqComputation_pf , & _rtP -> PosSeqComputation_pf ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2707_0 = _rtB
-> B_390_2706_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2707_0 , B_390_2660_0 , B_390_2681_0 , B_390_2702_0 , & _rtB ->
NegSeqComputation_kv , & _rtDW -> NegSeqComputation_kv , & _rtP ->
NegSeqComputation_kv ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_2710_0 = _rtB -> B_390_2709_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_2710_0 ,
B_390_2660_0 , B_390_2681_0 , B_390_2702_0 , & _rtB -> ZeroSeqComputation_fm
, & _rtDW -> ZeroSeqComputation_fm , & _rtP -> ZeroSeqComputation_fm ) ;
rtb_B_390_2118_0 = rtb_B_390_239_0 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_pf . B_22_2_0 . re , _rtB -> PosSeqComputation_pf .
B_22_2_0 . im ) * _rtP -> P_2066 ; muDoubleScalarSinCos ( _rtP -> P_2069 * (
_rtP -> P_2067 * rtb_B_390_4_1 - _rtP -> P_2068 * muDoubleScalarAtan2 ( _rtB
-> PosSeqComputation_pf . B_22_2_0 . im , _rtB -> PosSeqComputation_pf .
B_22_2_0 . re ) ) , & rtb_B_390_1596_0 , & rtb_B_390_422_0 ) ; _rtB ->
B_390_2720_0 = rtb_B_390_2118_0 * rtb_B_390_422_0 ; _rtB -> B_390_2721_0 =
_rtX -> integrator_CSTATE_if ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mo . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mo . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2071 ;
B_390_2722_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lh . Last , _rtDW ->
TransportDelay_IWORK_lh . Tail , _rtDW -> TransportDelay_IWORK_lh . Head ,
_rtP -> P_2072 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2724_0 = _rtP -> P_2073 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2724_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2726_0 =
_rtDW -> Memory_PreviousInput_jw ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2728_0 = ( _rtB -> B_390_2721_0 - B_390_2722_0 ) * _rtP -> P_241 ; }
else { _rtB -> B_390_2728_0 = _rtB -> B_390_2726_0 ; } _rtB -> B_390_2729_0 =
_rtX -> integrator_CSTATE_il ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_kt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kt . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2076 ;
B_390_2730_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ip .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ip . Last , _rtDW ->
TransportDelay_IWORK_ip . Tail , _rtDW -> TransportDelay_IWORK_ip . Head ,
_rtP -> P_2077 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2732_0 = _rtP -> P_2078 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2732_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2734_0 =
_rtDW -> Memory_PreviousInput_b2 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2736_0 = ( _rtB -> B_390_2729_0 - B_390_2730_0 ) * _rtP -> P_240 ; }
else { _rtB -> B_390_2736_0 = _rtB -> B_390_2734_0 ; } rtb_B_390_422_0 = _rtP
-> P_2080 * muDoubleScalarAtan2 ( _rtB -> B_390_2736_0 , _rtB -> B_390_2728_0
) * _rtP -> P_2081 ; B_390_2741_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_2728_0 , _rtB -> B_390_2736_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_2741_0 . im = muDoubleScalarHypot ( _rtB -> B_390_2728_0 , _rtB ->
B_390_2736_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_2742_0
= _rtX -> integrator_CSTATE_cv ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2083
; B_390_2743_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_of .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_of . Last , _rtDW ->
TransportDelay_IWORK_of . Tail , _rtDW -> TransportDelay_IWORK_of . Head ,
_rtP -> P_2084 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2745_0 = _rtP -> P_2085 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2745_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2747_0 =
_rtDW -> Memory_PreviousInput_fw ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2749_0 = ( _rtB -> B_390_2742_0 - B_390_2743_0 ) * _rtP -> P_243 ; }
else { _rtB -> B_390_2749_0 = _rtB -> B_390_2747_0 ; } _rtB -> B_390_2750_0 =
_rtX -> integrator_CSTATE_cl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_d5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2088 ;
B_390_2751_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lk .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lk . Last , _rtDW ->
TransportDelay_IWORK_lk . Tail , _rtDW -> TransportDelay_IWORK_lk . Head ,
_rtP -> P_2089 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2753_0 = _rtP -> P_2090 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2753_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2755_0 =
_rtDW -> Memory_PreviousInput_ml ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2757_0 = ( _rtB -> B_390_2750_0 - B_390_2751_0 ) * _rtP -> P_242 ; }
else { _rtB -> B_390_2757_0 = _rtB -> B_390_2755_0 ; } rtb_B_390_422_0 = _rtP
-> P_2092 * muDoubleScalarAtan2 ( _rtB -> B_390_2757_0 , _rtB -> B_390_2749_0
) * _rtP -> P_2093 ; B_390_2762_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_2749_0 , _rtB -> B_390_2757_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_2762_0 . im = muDoubleScalarHypot ( _rtB -> B_390_2749_0 , _rtB ->
B_390_2757_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; _rtB -> B_390_2763_0
= _rtX -> integrator_CSTATE_ge ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2095
; B_390_2764_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pj .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pj . Last , _rtDW ->
TransportDelay_IWORK_pj . Tail , _rtDW -> TransportDelay_IWORK_pj . Head ,
_rtP -> P_2096 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2766_0 = _rtP -> P_2097 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2766_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2768_0 =
_rtDW -> Memory_PreviousInput_n1 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2770_0 = ( _rtB -> B_390_2763_0 - B_390_2764_0 ) * _rtP -> P_245 ; }
else { _rtB -> B_390_2770_0 = _rtB -> B_390_2768_0 ; } _rtB -> B_390_2771_0 =
_rtX -> integrator_CSTATE_ak ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_em . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_em . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2100 ;
B_390_2772_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_l5 . Last , _rtDW ->
TransportDelay_IWORK_l5 . Tail , _rtDW -> TransportDelay_IWORK_l5 . Head ,
_rtP -> P_2101 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2774_0 = _rtP -> P_2102 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2774_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2776_0 =
_rtDW -> Memory_PreviousInput_i5 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2778_0 = ( _rtB -> B_390_2771_0 - B_390_2772_0 ) * _rtP -> P_244 ; }
else { _rtB -> B_390_2778_0 = _rtB -> B_390_2776_0 ; } rtb_B_390_422_0 = _rtP
-> P_2104 * muDoubleScalarAtan2 ( _rtB -> B_390_2778_0 , _rtB -> B_390_2770_0
) * _rtP -> P_2105 ; B_390_2783_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_2770_0 , _rtB -> B_390_2778_0 ) * muDoubleScalarCos ( rtb_B_390_422_0 )
; B_390_2783_0 . im = muDoubleScalarHypot ( _rtB -> B_390_2770_0 , _rtB ->
B_390_2778_0 ) * muDoubleScalarSin ( rtb_B_390_422_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2785_0 = _rtB
-> B_390_2784_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2785_0 , B_390_2741_0 , B_390_2762_0 , B_390_2783_0 , & _rtB ->
PosSeqComputation_ko , & _rtDW -> PosSeqComputation_ko , & _rtP ->
PosSeqComputation_ko ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_2788_0 = _rtB -> B_390_2787_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2788_0 ,
B_390_2741_0 , B_390_2762_0 , B_390_2783_0 , & _rtB -> NegSeqComputation_n ,
& _rtDW -> NegSeqComputation_n , & _rtP -> NegSeqComputation_n ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2791_0 = _rtB
-> B_390_2790_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_2791_0 , B_390_2741_0 , B_390_2762_0 , B_390_2783_0 , & _rtB ->
ZeroSeqComputation_j , & _rtDW -> ZeroSeqComputation_j , & _rtP ->
ZeroSeqComputation_j ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ko . B_22_2_0 . im , _rtB -> PosSeqComputation_ko .
B_22_2_0 . re ) ; _rtB -> B_390_2794_0 = _rtP -> P_2109 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_ko . B_22_2_0 . re , _rtB -> PosSeqComputation_ko
. B_22_2_0 . im ) ; _rtB -> B_390_2795_0 = rtb_B_390_2118_0 *
rtb_B_390_1596_0 ; _rtB -> B_390_2796_0 = _rtX -> integrator_CSTATE_mz2 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ki .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ki . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2111 ; B_390_2797_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_jy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jy . Last , _rtDW -> TransportDelay_IWORK_jy . Tail ,
_rtDW -> TransportDelay_IWORK_jy . Head , _rtP -> P_2112 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2799_0 = _rtP -> P_2113 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2799_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2801_0 = _rtDW -> Memory_PreviousInput_hz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2803_0 = ( _rtB -> B_390_2796_0 -
B_390_2797_0 ) * _rtP -> P_223 ; } else { _rtB -> B_390_2803_0 = _rtB ->
B_390_2801_0 ; } _rtB -> B_390_2804_0 = _rtX -> integrator_CSTATE_b5 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ev .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ev . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2116 ; B_390_2805_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ly . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ly . Last , _rtDW -> TransportDelay_IWORK_ly . Tail ,
_rtDW -> TransportDelay_IWORK_ly . Head , _rtP -> P_2117 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2807_0 = _rtP -> P_2118 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2807_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2809_0 = _rtDW -> Memory_PreviousInput_lk ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2811_0 = ( _rtB -> B_390_2804_0 -
B_390_2805_0 ) * _rtP -> P_222 ; } else { _rtB -> B_390_2811_0 = _rtB ->
B_390_2809_0 ; } rtb_B_390_2118_0 = _rtP -> P_2120 * muDoubleScalarAtan2 (
_rtB -> B_390_2811_0 , _rtB -> B_390_2803_0 ) * _rtP -> P_2121 ; B_390_2816_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2803_0 , _rtB -> B_390_2811_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2816_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2803_0 , _rtB -> B_390_2811_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_2817_0 = _rtX ->
integrator_CSTATE_hc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oeh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oeh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2123 ;
B_390_2818_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_oq . Last , _rtDW ->
TransportDelay_IWORK_oq . Tail , _rtDW -> TransportDelay_IWORK_oq . Head ,
_rtP -> P_2124 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2820_0 = _rtP -> P_2125 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2820_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2822_0 =
_rtDW -> Memory_PreviousInput_gj ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2824_0 = ( _rtB -> B_390_2817_0 - B_390_2818_0 ) * _rtP -> P_225 ; }
else { _rtB -> B_390_2824_0 = _rtB -> B_390_2822_0 ; } _rtB -> B_390_2825_0 =
_rtX -> integrator_CSTATE_bn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2128 ;
B_390_2826_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pr . Last , _rtDW ->
TransportDelay_IWORK_pr . Tail , _rtDW -> TransportDelay_IWORK_pr . Head ,
_rtP -> P_2129 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2828_0 = _rtP -> P_2130 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_2828_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2830_0 =
_rtDW -> Memory_PreviousInput_gl ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_2832_0 = ( _rtB -> B_390_2825_0 - B_390_2826_0 ) * _rtP -> P_224 ; }
else { _rtB -> B_390_2832_0 = _rtB -> B_390_2830_0 ; } rtb_B_390_2118_0 =
_rtP -> P_2132 * muDoubleScalarAtan2 ( _rtB -> B_390_2832_0 , _rtB ->
B_390_2824_0 ) * _rtP -> P_2133 ; B_390_2837_0 . re = muDoubleScalarHypot (
_rtB -> B_390_2824_0 , _rtB -> B_390_2832_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_2837_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_2824_0 , _rtB -> B_390_2832_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_2838_0 = _rtX -> integrator_CSTATE_jqz ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2135 ; B_390_2839_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kj . Last , _rtDW -> TransportDelay_IWORK_kj . Tail ,
_rtDW -> TransportDelay_IWORK_kj . Head , _rtP -> P_2136 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2841_0 = _rtP -> P_2137 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2841_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2843_0 = _rtDW -> Memory_PreviousInput_pd ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2845_0 = ( _rtB -> B_390_2838_0 -
B_390_2839_0 ) * _rtP -> P_227 ; } else { _rtB -> B_390_2845_0 = _rtB ->
B_390_2843_0 ; } _rtB -> B_390_2846_0 = _rtX -> integrator_CSTATE_mn ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oq .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oq . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2140 ; B_390_2847_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_pn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pn . Last , _rtDW -> TransportDelay_IWORK_pn . Tail ,
_rtDW -> TransportDelay_IWORK_pn . Head , _rtP -> P_2141 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_2849_0 = _rtP -> P_2142 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_2849_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_2851_0 = _rtDW -> Memory_PreviousInput_ey ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_2853_0 = ( _rtB -> B_390_2846_0 -
B_390_2847_0 ) * _rtP -> P_226 ; } else { _rtB -> B_390_2853_0 = _rtB ->
B_390_2851_0 ; } rtb_B_390_2118_0 = _rtP -> P_2144 * muDoubleScalarAtan2 (
_rtB -> B_390_2853_0 , _rtB -> B_390_2845_0 ) * _rtP -> P_2145 ; B_390_2858_0
. re = muDoubleScalarHypot ( _rtB -> B_390_2845_0 , _rtB -> B_390_2853_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_2858_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_2845_0 , _rtB -> B_390_2853_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_2860_0 = _rtB -> B_390_2859_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_2860_0 ,
B_390_2816_0 , B_390_2837_0 , B_390_2858_0 , & _rtB -> PosSeqComputation_c4 ,
& _rtDW -> PosSeqComputation_c4 , & _rtP -> PosSeqComputation_c4 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2863_0 = _rtB
-> B_390_2862_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_2863_0 , B_390_2816_0 , B_390_2837_0 , B_390_2858_0 , & _rtB ->
NegSeqComputation_gw , & _rtDW -> NegSeqComputation_gw , & _rtP ->
NegSeqComputation_gw ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_2866_0 = _rtB -> B_390_2865_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_2866_0 ,
B_390_2816_0 , B_390_2837_0 , B_390_2858_0 , & _rtB -> ZeroSeqComputation_mx
, & _rtDW -> ZeroSeqComputation_mx , & _rtP -> ZeroSeqComputation_mx ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_c4 .
B_22_2_0 . im , _rtB -> PosSeqComputation_c4 . B_22_2_0 . re ) ; _rtB ->
B_390_2869_0 = _rtP -> P_2149 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_c4 . B_22_2_0 . re , _rtB -> PosSeqComputation_c4 .
B_22_2_0 . im ) ; ssCallAccelRunBlock ( S , 390 , 2870 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_390_1508_0 [ 0 ] = _rtB -> B_390_2720_0 ; _rtB -> B_390_1508_0
[ 1 ] = _rtB -> B_390_2794_0 ; _rtB -> B_390_1508_0 [ 2 ] = _rtB ->
B_390_2795_0 ; _rtB -> B_390_1508_0 [ 3 ] = _rtB -> B_390_2869_0 ;
ssCallAccelRunBlock ( S , 390 , 2872 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_2876_0 [ 0 ]
= _rtP -> P_2150 * _rtB -> B_0_14_0 [ 117 ] * _rtP -> P_2153 ; _rtB ->
B_390_2876_0 [ 1 ] = _rtP -> P_2151 * _rtB -> B_0_14_0 [ 118 ] * _rtP ->
P_2153 ; _rtB -> B_390_2876_0 [ 2 ] = _rtP -> P_2152 * _rtB -> B_0_14_0 [ 119
] * _rtP -> P_2153 ; _rtB -> B_390_2880_0 [ 0 ] = _rtP -> P_2154 * _rtB ->
B_0_14_0 [ 62 ] * _rtP -> P_2157 ; _rtB -> B_390_2880_0 [ 1 ] = _rtP ->
P_2155 * _rtB -> B_0_14_0 [ 63 ] * _rtP -> P_2157 ; _rtB -> B_390_2880_0 [ 2
] = _rtP -> P_2156 * _rtB -> B_0_14_0 [ 64 ] * _rtP -> P_2157 ; } _rtB ->
B_390_2916_0 = ( muDoubleScalarSin ( _rtP -> P_2160 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2161 ) * _rtP -> P_2158 + _rtP -> P_2159 ) * _rtB -> B_390_2876_0
[ 0 ] ; _rtB -> B_390_2918_0 = ( muDoubleScalarSin ( _rtP -> P_2164 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2165 ) * _rtP -> P_2162 + _rtP -> P_2163
) * _rtB -> B_390_2876_0 [ 0 ] ; _rtB -> B_390_2920_0 = ( muDoubleScalarSin (
_rtP -> P_2168 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2169 ) * _rtP -> P_2166
+ _rtP -> P_2167 ) * _rtB -> B_390_2876_0 [ 1 ] ; _rtB -> B_390_2922_0 = (
muDoubleScalarSin ( _rtP -> P_2172 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2173
) * _rtP -> P_2170 + _rtP -> P_2171 ) * _rtB -> B_390_2876_0 [ 1 ] ; _rtB ->
B_390_2924_0 = ( muDoubleScalarSin ( _rtP -> P_2176 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2177 ) * _rtP -> P_2174 + _rtP -> P_2175 ) * _rtB -> B_390_2876_0
[ 2 ] ; _rtB -> B_390_2926_0 = ( muDoubleScalarSin ( _rtP -> P_2180 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2181 ) * _rtP -> P_2178 + _rtP -> P_2179
) * _rtB -> B_390_2876_0 [ 2 ] ; B_390_2927_0 = _rtP -> P_2182 *
B_390_401_1_idx_0 ; _rtB -> B_390_2929_0 = ( muDoubleScalarSin ( _rtP ->
P_2185 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2186 ) * _rtP -> P_2183 + _rtP
-> P_2184 ) * _rtB -> B_390_2880_0 [ 0 ] ; _rtB -> B_390_2931_0 = (
muDoubleScalarSin ( _rtP -> P_2189 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2190
) * _rtP -> P_2187 + _rtP -> P_2188 ) * _rtB -> B_390_2880_0 [ 0 ] ; _rtB ->
B_390_2933_0 = ( muDoubleScalarSin ( _rtP -> P_2193 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2194 ) * _rtP -> P_2191 + _rtP -> P_2192 ) * _rtB -> B_390_2880_0
[ 1 ] ; _rtB -> B_390_2935_0 = ( muDoubleScalarSin ( _rtP -> P_2197 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2198 ) * _rtP -> P_2195 + _rtP -> P_2196
) * _rtB -> B_390_2880_0 [ 1 ] ; _rtB -> B_390_2937_0 = ( muDoubleScalarSin (
_rtP -> P_2201 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2202 ) * _rtP -> P_2199
+ _rtP -> P_2200 ) * _rtB -> B_390_2880_0 [ 2 ] ; _rtB -> B_390_2939_0 = (
muDoubleScalarSin ( _rtP -> P_2205 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2206
) * _rtP -> P_2203 + _rtP -> P_2204 ) * _rtB -> B_390_2880_0 [ 2 ] ; _rtB ->
B_390_2941_0 = ( muDoubleScalarSin ( _rtP -> P_2209 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2210 ) * _rtP -> P_2207 + _rtP -> P_2208 ) * _rtB -> B_390_2876_0
[ 0 ] ; _rtB -> B_390_2943_0 = ( muDoubleScalarSin ( _rtP -> P_2213 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2214 ) * _rtP -> P_2211 + _rtP -> P_2212
) * _rtB -> B_390_2876_0 [ 0 ] ; _rtB -> B_390_2945_0 = ( muDoubleScalarSin (
_rtP -> P_2217 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2218 ) * _rtP -> P_2215
+ _rtP -> P_2216 ) * _rtB -> B_390_2876_0 [ 1 ] ; _rtB -> B_390_2947_0 = (
muDoubleScalarSin ( _rtP -> P_2221 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2222
) * _rtP -> P_2219 + _rtP -> P_2220 ) * _rtB -> B_390_2876_0 [ 1 ] ; _rtB ->
B_390_2949_0 = ( muDoubleScalarSin ( _rtP -> P_2225 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2226 ) * _rtP -> P_2223 + _rtP -> P_2224 ) * _rtB -> B_390_2876_0
[ 2 ] ; _rtB -> B_390_2951_0 = ( muDoubleScalarSin ( _rtP -> P_2229 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2230 ) * _rtP -> P_2227 + _rtP -> P_2228
) * _rtB -> B_390_2876_0 [ 2 ] ; _rtB -> B_390_2953_0 = ( muDoubleScalarSin (
_rtP -> P_2233 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2234 ) * _rtP -> P_2231
+ _rtP -> P_2232 ) * _rtB -> B_390_2880_0 [ 0 ] ; _rtB -> B_390_2955_0 = (
muDoubleScalarSin ( _rtP -> P_2237 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2238
) * _rtP -> P_2235 + _rtP -> P_2236 ) * _rtB -> B_390_2880_0 [ 0 ] ; _rtB ->
B_390_2957_0 = ( muDoubleScalarSin ( _rtP -> P_2241 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2242 ) * _rtP -> P_2239 + _rtP -> P_2240 ) * _rtB -> B_390_2880_0
[ 1 ] ; _rtB -> B_390_2959_0 = ( muDoubleScalarSin ( _rtP -> P_2245 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2246 ) * _rtP -> P_2243 + _rtP -> P_2244
) * _rtB -> B_390_2880_0 [ 1 ] ; _rtB -> B_390_2961_0 = ( muDoubleScalarSin (
_rtP -> P_2249 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2250 ) * _rtP -> P_2247
+ _rtP -> P_2248 ) * _rtB -> B_390_2880_0 [ 2 ] ; _rtB -> B_390_2963_0 = (
muDoubleScalarSin ( _rtP -> P_2253 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2254
) * _rtP -> P_2251 + _rtP -> P_2252 ) * _rtB -> B_390_2880_0 [ 2 ] ;
B_390_2964_0 = _rtP -> P_2255 * rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 2
, 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_2967_0 >= _rtP -> P_2261 ) {
rtb_B_390_2970_0 = _rtB -> B_390_2966_0 ; } else { rtb_B_390_2970_0 =
rt_Lookup ( _rtP -> P_2259 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_2260 ) ;
} if ( _rtB -> B_390_2971_0 >= _rtP -> P_2264 ) { _rtB -> B_390_2974_0 =
rtb_B_390_2970_0 ; } else { _rtB -> B_390_2974_0 = _rtB -> B_390_2972_0 ; } }
_rtB -> B_390_2976_0 = rt_Lookup ( _rtP -> P_2265 , 5 , ssGetT ( S ) , _rtP
-> P_2266 ) ; if ( _rtB -> B_390_2965_0 >= _rtP -> P_2267 ) { _rtB ->
B_390_2977_0 = _rtB -> B_390_2974_0 ; } else { _rtB -> B_390_2977_0 = _rtB ->
B_390_2976_0 ; } if ( ( _rtDW -> TimeStampA_nv >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_g >= ssGetT ( S ) ) ) { _rtB -> B_390_2978_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_nv ; lastU = & _rtDW -> LastUAtTimeA_d
; if ( _rtDW -> TimeStampA_nv < _rtDW -> TimeStampB_g ) { if ( _rtDW ->
TimeStampB_g < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_g ;
lastU = & _rtDW -> LastUAtTimeB_dr ; } } else { if ( _rtDW -> TimeStampA_nv
>= ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_g ; lastU = & _rtDW
-> LastUAtTimeB_dr ; } } _rtB -> B_390_2978_0 = ( _rtB -> B_390_2976_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_2984_0 >= _rtP -> P_2271 ) {
_rtB -> B_390_2986_0 = rtb_B_390_2970_0 ; } else { _rtB -> B_390_2986_0 =
_rtB -> B_390_2972_0 ; } } _rtB -> B_390_2988_0 = rt_Lookup ( _rtP -> P_2272
, 5 , ssGetT ( S ) , _rtP -> P_2273 ) ; if ( _rtB -> B_390_2983_0 >= _rtP ->
P_2274 ) { _rtB -> B_390_2989_0 = _rtB -> B_390_2986_0 ; } else { _rtB ->
B_390_2989_0 = _rtB -> B_390_2988_0 ; } if ( ( _rtDW -> TimeStampA_l >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ic >= ssGetT ( S ) ) ) { _rtB ->
B_390_2990_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_l ; lastU
= & _rtDW -> LastUAtTimeA_ca ; if ( _rtDW -> TimeStampA_l < _rtDW ->
TimeStampB_ic ) { if ( _rtDW -> TimeStampB_ic < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_ic ; lastU = & _rtDW -> LastUAtTimeB_b
; } } else { if ( _rtDW -> TimeStampA_l >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_ic ; lastU = & _rtDW -> LastUAtTimeB_b ; } } _rtB ->
B_390_2990_0 = ( _rtB -> B_390_2988_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_2996_0 >= _rtP -> P_2278 ) { _rtB -> B_390_2998_0 =
rtb_B_390_2970_0 ; } else { _rtB -> B_390_2998_0 = _rtB -> B_390_2972_0 ; } }
_rtB -> B_390_3000_0 = rt_Lookup ( _rtP -> P_2279 , 5 , ssGetT ( S ) , _rtP
-> P_2280 ) ; if ( _rtB -> B_390_2995_0 >= _rtP -> P_2281 ) { _rtB ->
B_390_3001_0 = _rtB -> B_390_2998_0 ; } else { _rtB -> B_390_3001_0 = _rtB ->
B_390_3000_0 ; } if ( ( _rtDW -> TimeStampA_m >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_ka >= ssGetT ( S ) ) ) { _rtB -> B_390_3002_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_m ; lastU = & _rtDW -> LastUAtTimeA_e ;
if ( _rtDW -> TimeStampA_m < _rtDW -> TimeStampB_ka ) { if ( _rtDW ->
TimeStampB_ka < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_ka ;
lastU = & _rtDW -> LastUAtTimeB_c ; } } else { if ( _rtDW -> TimeStampA_m >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_ka ; lastU = & _rtDW
-> LastUAtTimeB_c ; } } _rtB -> B_390_3002_0 = ( _rtB -> B_390_3000_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } _rtB -> B_390_3019_0 = _rtX
-> integrator_CSTATE_np ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dt . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2284 ;
B_390_3020_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ea5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ea5 . Last , _rtDW ->
TransportDelay_IWORK_ea5 . Tail , _rtDW -> TransportDelay_IWORK_ea5 . Head ,
_rtP -> P_2285 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3022_0 = _rtP -> P_2286 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3022_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3024_0 =
_rtDW -> Memory_PreviousInput_o4 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3026_0 = ( _rtB -> B_390_3019_0 - B_390_3020_0 ) * _rtP -> P_253 ; }
else { _rtB -> B_390_3026_0 = _rtB -> B_390_3024_0 ; } _rtB -> B_390_3027_0 =
_rtX -> integrator_CSTATE_a5a ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mwl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2289
; B_390_3028_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eg .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_eg . Last , _rtDW ->
TransportDelay_IWORK_eg . Tail , _rtDW -> TransportDelay_IWORK_eg . Head ,
_rtP -> P_2290 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3030_0 = _rtP -> P_2291 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3030_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3032_0 =
_rtDW -> Memory_PreviousInput_nx ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3034_0 = ( _rtB -> B_390_3027_0 - B_390_3028_0 ) * _rtP -> P_252 ; }
else { _rtB -> B_390_3034_0 = _rtB -> B_390_3032_0 ; } rtb_B_390_2970_0 =
_rtP -> P_2293 * muDoubleScalarAtan2 ( _rtB -> B_390_3034_0 , _rtB ->
B_390_3026_0 ) * _rtP -> P_2294 ; B_390_3039_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3026_0 , _rtB -> B_390_3034_0 ) * muDoubleScalarCos (
rtb_B_390_2970_0 ) ; B_390_3039_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3026_0 , _rtB -> B_390_3034_0 ) * muDoubleScalarSin ( rtb_B_390_2970_0
) ; _rtB -> B_390_3040_0 = _rtX -> integrator_CSTATE_jo ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2296 ; B_390_3041_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kk . Last , _rtDW -> TransportDelay_IWORK_kk . Tail ,
_rtDW -> TransportDelay_IWORK_kk . Head , _rtP -> P_2297 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3043_0 = _rtP -> P_2298 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3043_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3045_0 = _rtDW -> Memory_PreviousInput_mq ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3047_0 = ( _rtB -> B_390_3040_0 -
B_390_3041_0 ) * _rtP -> P_255 ; } else { _rtB -> B_390_3047_0 = _rtB ->
B_390_3045_0 ; } _rtB -> B_390_3048_0 = _rtX -> integrator_CSTATE_bj ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jp .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_jp . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2301 ; B_390_3049_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_he5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he5 . Last , _rtDW -> TransportDelay_IWORK_he5 . Tail ,
_rtDW -> TransportDelay_IWORK_he5 . Head , _rtP -> P_2302 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3051_0 = _rtP -> P_2303 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3051_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3053_0 = _rtDW -> Memory_PreviousInput_bq ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3055_0 = ( _rtB -> B_390_3048_0 -
B_390_3049_0 ) * _rtP -> P_254 ; } else { _rtB -> B_390_3055_0 = _rtB ->
B_390_3053_0 ; } rtb_B_390_2970_0 = _rtP -> P_2305 * muDoubleScalarAtan2 (
_rtB -> B_390_3055_0 , _rtB -> B_390_3047_0 ) * _rtP -> P_2306 ; B_390_3060_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3047_0 , _rtB -> B_390_3055_0 ) *
muDoubleScalarCos ( rtb_B_390_2970_0 ) ; B_390_3060_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3047_0 , _rtB -> B_390_3055_0 ) *
muDoubleScalarSin ( rtb_B_390_2970_0 ) ; _rtB -> B_390_3061_0 = _rtX ->
integrator_CSTATE_bu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_gi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2308 ;
B_390_3062_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dp .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dp . Last , _rtDW ->
TransportDelay_IWORK_dp . Tail , _rtDW -> TransportDelay_IWORK_dp . Head ,
_rtP -> P_2309 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3064_0 = _rtP -> P_2310 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3064_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3066_0 =
_rtDW -> Memory_PreviousInput_ep ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3068_0 = ( _rtB -> B_390_3061_0 - B_390_3062_0 ) * _rtP -> P_257 ; }
else { _rtB -> B_390_3068_0 = _rtB -> B_390_3066_0 ; } _rtB -> B_390_3069_0 =
_rtX -> integrator_CSTATE_df ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ac . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ac . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2313 ;
B_390_3070_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_la .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_la . Last , _rtDW ->
TransportDelay_IWORK_la . Tail , _rtDW -> TransportDelay_IWORK_la . Head ,
_rtP -> P_2314 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3072_0 = _rtP -> P_2315 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3072_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3074_0 =
_rtDW -> Memory_PreviousInput_ms ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3076_0 = ( _rtB -> B_390_3069_0 - B_390_3070_0 ) * _rtP -> P_256 ; }
else { _rtB -> B_390_3076_0 = _rtB -> B_390_3074_0 ; } rtb_B_390_2970_0 =
_rtP -> P_2317 * muDoubleScalarAtan2 ( _rtB -> B_390_3076_0 , _rtB ->
B_390_3068_0 ) * _rtP -> P_2318 ; B_390_3081_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3068_0 , _rtB -> B_390_3076_0 ) * muDoubleScalarCos (
rtb_B_390_2970_0 ) ; B_390_3081_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3068_0 , _rtB -> B_390_3076_0 ) * muDoubleScalarSin ( rtb_B_390_2970_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3083_0 = _rtB -> B_390_3082_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_3083_0 , B_390_3039_0 , B_390_3060_0 , B_390_3081_0 , &
_rtB -> PosSeqComputation_l3 , & _rtDW -> PosSeqComputation_l3 , & _rtP ->
PosSeqComputation_l3 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3086_0 = _rtB -> B_390_3085_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3086_0 ,
B_390_3039_0 , B_390_3060_0 , B_390_3081_0 , & _rtB -> NegSeqComputation_b ,
& _rtDW -> NegSeqComputation_b , & _rtP -> NegSeqComputation_b ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3089_0 = _rtB
-> B_390_3088_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_3089_0 , B_390_3039_0 , B_390_3060_0 , B_390_3081_0 , & _rtB ->
ZeroSeqComputation_oh , & _rtDW -> ZeroSeqComputation_oh , & _rtP ->
ZeroSeqComputation_oh ) ; rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_l3 . B_22_2_0 . re , _rtB -> PosSeqComputation_l3 .
B_22_2_0 . im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_l3 . B_22_2_0 . im , _rtB -> PosSeqComputation_l3 .
B_22_2_0 . re ) ; _rtB -> B_390_3092_0 = _rtX -> integrator_CSTATE_pe ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2323 ; B_390_3093_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ev . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ev . Last , _rtDW -> TransportDelay_IWORK_ev . Tail ,
_rtDW -> TransportDelay_IWORK_ev . Head , _rtP -> P_2324 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3095_0 = _rtP -> P_2325 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3095_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3097_0 = _rtDW -> Memory_PreviousInput_dw ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3099_0 = ( _rtB -> B_390_3092_0 -
B_390_3093_0 ) * _rtP -> P_259 ; } else { _rtB -> B_390_3099_0 = _rtB ->
B_390_3097_0 ; } _rtB -> B_390_3100_0 = _rtX -> integrator_CSTATE_di ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gl .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gl . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2328 ; B_390_3101_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_b1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b1 . Last , _rtDW -> TransportDelay_IWORK_b1 . Tail ,
_rtDW -> TransportDelay_IWORK_b1 . Head , _rtP -> P_2329 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3103_0 = _rtP -> P_2330 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3103_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3105_0 = _rtDW -> Memory_PreviousInput_jh ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3107_0 = ( _rtB -> B_390_3100_0 -
B_390_3101_0 ) * _rtP -> P_258 ; } else { _rtB -> B_390_3107_0 = _rtB ->
B_390_3105_0 ; } rtb_B_390_2970_0 = _rtP -> P_2332 * muDoubleScalarAtan2 (
_rtB -> B_390_3107_0 , _rtB -> B_390_3099_0 ) * _rtP -> P_2333 ; B_390_3112_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3099_0 , _rtB -> B_390_3107_0 ) *
muDoubleScalarCos ( rtb_B_390_2970_0 ) ; B_390_3112_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3099_0 , _rtB -> B_390_3107_0 ) *
muDoubleScalarSin ( rtb_B_390_2970_0 ) ; _rtB -> B_390_3113_0 = _rtX ->
integrator_CSTATE_gi ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_mf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_mf . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2335 ;
B_390_3114_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nq . Last , _rtDW ->
TransportDelay_IWORK_nq . Tail , _rtDW -> TransportDelay_IWORK_nq . Head ,
_rtP -> P_2336 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3116_0 = _rtP -> P_2337 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3116_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3118_0 =
_rtDW -> Memory_PreviousInput_fx ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3120_0 = ( _rtB -> B_390_3113_0 - B_390_3114_0 ) * _rtP -> P_261 ; }
else { _rtB -> B_390_3120_0 = _rtB -> B_390_3118_0 ; } _rtB -> B_390_3121_0 =
_rtX -> integrator_CSTATE_fd ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ag . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ag . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2340 ;
B_390_3122_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_he5l .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_he5l . Last , _rtDW ->
TransportDelay_IWORK_he5l . Tail , _rtDW -> TransportDelay_IWORK_he5l . Head
, _rtP -> P_2341 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3124_0 = _rtP -> P_2342 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3124_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3126_0 =
_rtDW -> Memory_PreviousInput_a2 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3128_0 = ( _rtB -> B_390_3121_0 - B_390_3122_0 ) * _rtP -> P_260 ; }
else { _rtB -> B_390_3128_0 = _rtB -> B_390_3126_0 ; } rtb_B_390_2970_0 =
_rtP -> P_2344 * muDoubleScalarAtan2 ( _rtB -> B_390_3128_0 , _rtB ->
B_390_3120_0 ) * _rtP -> P_2345 ; B_390_3133_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3120_0 , _rtB -> B_390_3128_0 ) * muDoubleScalarCos (
rtb_B_390_2970_0 ) ; B_390_3133_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3120_0 , _rtB -> B_390_3128_0 ) * muDoubleScalarSin ( rtb_B_390_2970_0
) ; _rtB -> B_390_3134_0 = _rtX -> integrator_CSTATE_i2 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2347 ; B_390_3135_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4 . Last , _rtDW -> TransportDelay_IWORK_p4 . Tail ,
_rtDW -> TransportDelay_IWORK_p4 . Head , _rtP -> P_2348 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3137_0 = _rtP -> P_2349 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3137_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3139_0 = _rtDW -> Memory_PreviousInput_cs ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3141_0 = ( _rtB -> B_390_3134_0 -
B_390_3135_0 ) * _rtP -> P_263 ; } else { _rtB -> B_390_3141_0 = _rtB ->
B_390_3139_0 ; } _rtB -> B_390_3142_0 = _rtX -> integrator_CSTATE_kz ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_og .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_og . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2352 ; B_390_3143_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kjd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kjd . Last , _rtDW -> TransportDelay_IWORK_kjd . Tail ,
_rtDW -> TransportDelay_IWORK_kjd . Head , _rtP -> P_2353 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3145_0 = _rtP -> P_2354 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3145_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3147_0 = _rtDW -> Memory_PreviousInput_fi ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3149_0 = ( _rtB -> B_390_3142_0 -
B_390_3143_0 ) * _rtP -> P_262 ; } else { _rtB -> B_390_3149_0 = _rtB ->
B_390_3147_0 ; } rtb_B_390_2970_0 = _rtP -> P_2356 * muDoubleScalarAtan2 (
_rtB -> B_390_3149_0 , _rtB -> B_390_3141_0 ) * _rtP -> P_2357 ; B_390_3154_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3141_0 , _rtB -> B_390_3149_0 ) *
muDoubleScalarCos ( rtb_B_390_2970_0 ) ; B_390_3154_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3141_0 , _rtB -> B_390_3149_0 ) *
muDoubleScalarSin ( rtb_B_390_2970_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_3156_0 = _rtB -> B_390_3155_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3156_0 ,
B_390_3112_0 , B_390_3133_0 , B_390_3154_0 , & _rtB -> PosSeqComputation_f ,
& _rtDW -> PosSeqComputation_f , & _rtP -> PosSeqComputation_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3159_0 = _rtB
-> B_390_3158_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_3159_0 , B_390_3112_0 , B_390_3133_0 , B_390_3154_0 , & _rtB ->
NegSeqComputation_jy , & _rtDW -> NegSeqComputation_jy , & _rtP ->
NegSeqComputation_jy ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3162_0 = _rtB -> B_390_3161_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_3162_0 ,
B_390_3112_0 , B_390_3133_0 , B_390_3154_0 , & _rtB -> ZeroSeqComputation_mj
, & _rtDW -> ZeroSeqComputation_mj , & _rtP -> ZeroSeqComputation_mj ) ;
rtb_B_390_2970_0 = rtb_B_390_239_0 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_f . B_22_2_0 . re , _rtB -> PosSeqComputation_f . B_22_2_0
. im ) * _rtP -> P_2361 ; muDoubleScalarSinCos ( _rtP -> P_2364 * ( _rtP ->
P_2362 * rtb_B_390_4_1 - _rtP -> P_2363 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_f . B_22_2_0 . im , _rtB -> PosSeqComputation_f . B_22_2_0
. re ) ) , & rtb_B_390_2118_0 , & rtb_B_390_1596_0 ) ; _rtB -> B_390_3172_0 =
rtb_B_390_2970_0 * rtb_B_390_1596_0 ; _rtB -> B_390_3173_0 = _rtX ->
integrator_CSTATE_nf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_e0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2366 ;
B_390_3174_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_cv . Last , _rtDW ->
TransportDelay_IWORK_cv . Tail , _rtDW -> TransportDelay_IWORK_cv . Head ,
_rtP -> P_2367 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3176_0 = _rtP -> P_2368 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3176_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3178_0 =
_rtDW -> Memory_PreviousInput_m3 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3180_0 = ( _rtB -> B_390_3173_0 - B_390_3174_0 ) * _rtP -> P_265 ; }
else { _rtB -> B_390_3180_0 = _rtB -> B_390_3178_0 ; } _rtB -> B_390_3181_0 =
_rtX -> integrator_CSTATE_dl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2371 ;
B_390_3182_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p4s .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_p4s . Last , _rtDW ->
TransportDelay_IWORK_p4s . Tail , _rtDW -> TransportDelay_IWORK_p4s . Head ,
_rtP -> P_2372 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3184_0 = _rtP -> P_2373 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3184_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3186_0 =
_rtDW -> Memory_PreviousInput_pu ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3188_0 = ( _rtB -> B_390_3181_0 - B_390_3182_0 ) * _rtP -> P_264 ; }
else { _rtB -> B_390_3188_0 = _rtB -> B_390_3186_0 ; } rtb_B_390_1596_0 =
_rtP -> P_2375 * muDoubleScalarAtan2 ( _rtB -> B_390_3188_0 , _rtB ->
B_390_3180_0 ) * _rtP -> P_2376 ; B_390_3193_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3180_0 , _rtB -> B_390_3188_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_3193_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3180_0 , _rtB -> B_390_3188_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; _rtB -> B_390_3194_0 = _rtX -> integrator_CSTATE_kf ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2378 ; B_390_3195_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ce . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ce . Last , _rtDW -> TransportDelay_IWORK_ce . Tail ,
_rtDW -> TransportDelay_IWORK_ce . Head , _rtP -> P_2379 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3197_0 = _rtP -> P_2380 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3197_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3199_0 = _rtDW -> Memory_PreviousInput_oz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3201_0 = ( _rtB -> B_390_3194_0 -
B_390_3195_0 ) * _rtP -> P_267 ; } else { _rtB -> B_390_3201_0 = _rtB ->
B_390_3199_0 ; } _rtB -> B_390_3202_0 = _rtX -> integrator_CSTATE_k4 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ck .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ck . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2383 ; B_390_3203_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_cc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cc . Last , _rtDW -> TransportDelay_IWORK_cc . Tail ,
_rtDW -> TransportDelay_IWORK_cc . Head , _rtP -> P_2384 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3205_0 = _rtP -> P_2385 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3205_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3207_0 = _rtDW -> Memory_PreviousInput_hjp ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3209_0 = ( _rtB -> B_390_3202_0 -
B_390_3203_0 ) * _rtP -> P_266 ; } else { _rtB -> B_390_3209_0 = _rtB ->
B_390_3207_0 ; } rtb_B_390_1596_0 = _rtP -> P_2387 * muDoubleScalarAtan2 (
_rtB -> B_390_3209_0 , _rtB -> B_390_3201_0 ) * _rtP -> P_2388 ; B_390_3214_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3201_0 , _rtB -> B_390_3209_0 ) *
muDoubleScalarCos ( rtb_B_390_1596_0 ) ; B_390_3214_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3201_0 , _rtB -> B_390_3209_0 ) *
muDoubleScalarSin ( rtb_B_390_1596_0 ) ; _rtB -> B_390_3215_0 = _rtX ->
integrator_CSTATE_h4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ni . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ni . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2390 ;
B_390_3216_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e2t .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_e2t . Last , _rtDW ->
TransportDelay_IWORK_e2t . Tail , _rtDW -> TransportDelay_IWORK_e2t . Head ,
_rtP -> P_2391 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3218_0 = _rtP -> P_2392 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3218_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3220_0 =
_rtDW -> Memory_PreviousInput_kt ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3222_0 = ( _rtB -> B_390_3215_0 - B_390_3216_0 ) * _rtP -> P_269 ; }
else { _rtB -> B_390_3222_0 = _rtB -> B_390_3220_0 ; } _rtB -> B_390_3223_0 =
_rtX -> integrator_CSTATE_a2 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_lu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2395 ;
B_390_3224_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ex .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ex . Last , _rtDW ->
TransportDelay_IWORK_ex . Tail , _rtDW -> TransportDelay_IWORK_ex . Head ,
_rtP -> P_2396 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3226_0 = _rtP -> P_2397 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3226_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3228_0 =
_rtDW -> Memory_PreviousInput_kk ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3230_0 = ( _rtB -> B_390_3223_0 - B_390_3224_0 ) * _rtP -> P_268 ; }
else { _rtB -> B_390_3230_0 = _rtB -> B_390_3228_0 ; } rtb_B_390_1596_0 =
_rtP -> P_2399 * muDoubleScalarAtan2 ( _rtB -> B_390_3230_0 , _rtB ->
B_390_3222_0 ) * _rtP -> P_2400 ; B_390_3235_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3222_0 , _rtB -> B_390_3230_0 ) * muDoubleScalarCos (
rtb_B_390_1596_0 ) ; B_390_3235_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3222_0 , _rtB -> B_390_3230_0 ) * muDoubleScalarSin ( rtb_B_390_1596_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3237_0 = _rtB -> B_390_3236_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_3237_0 , B_390_3193_0 , B_390_3214_0 , B_390_3235_0 , &
_rtB -> PosSeqComputation_ic , & _rtDW -> PosSeqComputation_ic , & _rtP ->
PosSeqComputation_ic ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3240_0 = _rtB -> B_390_3239_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3240_0 ,
B_390_3193_0 , B_390_3214_0 , B_390_3235_0 , & _rtB -> NegSeqComputation_lx ,
& _rtDW -> NegSeqComputation_lx , & _rtP -> NegSeqComputation_lx ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3243_0 = _rtB
-> B_390_3242_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_3243_0 , B_390_3193_0 , B_390_3214_0 , B_390_3235_0 , & _rtB ->
ZeroSeqComputation_lw , & _rtDW -> ZeroSeqComputation_lw , & _rtP ->
ZeroSeqComputation_lw ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ic . B_22_2_0 . im , _rtB -> PosSeqComputation_ic .
B_22_2_0 . re ) ; _rtB -> B_390_3246_0 = _rtP -> P_2404 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_ic . B_22_2_0 . re , _rtB -> PosSeqComputation_ic
. B_22_2_0 . im ) ; _rtB -> B_390_3247_0 = rtb_B_390_2970_0 *
rtb_B_390_2118_0 ; _rtB -> B_390_3248_0 = _rtX -> integrator_CSTATE_mh ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_pn . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2406 ; B_390_3249_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_p5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p5 . Last , _rtDW -> TransportDelay_IWORK_p5 . Tail ,
_rtDW -> TransportDelay_IWORK_p5 . Head , _rtP -> P_2407 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3251_0 = _rtP -> P_2408 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3251_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3253_0 = _rtDW -> Memory_PreviousInput_g3 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3255_0 = ( _rtB -> B_390_3248_0 -
B_390_3249_0 ) * _rtP -> P_247 ; } else { _rtB -> B_390_3255_0 = _rtB ->
B_390_3253_0 ; } _rtB -> B_390_3256_0 = _rtX -> integrator_CSTATE_fs ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ld .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ld . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2411 ; B_390_3257_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_eu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eu . Last , _rtDW -> TransportDelay_IWORK_eu . Tail ,
_rtDW -> TransportDelay_IWORK_eu . Head , _rtP -> P_2412 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3259_0 = _rtP -> P_2413 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3259_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3261_0 = _rtDW -> Memory_PreviousInput_o3 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3263_0 = ( _rtB -> B_390_3256_0 -
B_390_3257_0 ) * _rtP -> P_246 ; } else { _rtB -> B_390_3263_0 = _rtB ->
B_390_3261_0 ; } rtb_B_390_2970_0 = _rtP -> P_2415 * muDoubleScalarAtan2 (
_rtB -> B_390_3263_0 , _rtB -> B_390_3255_0 ) * _rtP -> P_2416 ; B_390_3268_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3255_0 , _rtB -> B_390_3263_0 ) *
muDoubleScalarCos ( rtb_B_390_2970_0 ) ; B_390_3268_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3255_0 , _rtB -> B_390_3263_0 ) *
muDoubleScalarSin ( rtb_B_390_2970_0 ) ; _rtB -> B_390_3269_0 = _rtX ->
integrator_CSTATE_bdm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kv . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kv . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2418 ;
B_390_3270_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i2y .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_i2y . Last , _rtDW ->
TransportDelay_IWORK_i2y . Tail , _rtDW -> TransportDelay_IWORK_i2y . Head ,
_rtP -> P_2419 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3272_0 = _rtP -> P_2420 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3272_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3274_0 =
_rtDW -> Memory_PreviousInput_dl ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3276_0 = ( _rtB -> B_390_3269_0 - B_390_3270_0 ) * _rtP -> P_249 ; }
else { _rtB -> B_390_3276_0 = _rtB -> B_390_3274_0 ; } _rtB -> B_390_3277_0 =
_rtX -> integrator_CSTATE_hn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ep . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ep . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2423 ;
B_390_3278_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lna .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lna . Last , _rtDW ->
TransportDelay_IWORK_lna . Tail , _rtDW -> TransportDelay_IWORK_lna . Head ,
_rtP -> P_2424 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3280_0 = _rtP -> P_2425 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3280_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3282_0 =
_rtDW -> Memory_PreviousInput_by ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3284_0 = ( _rtB -> B_390_3277_0 - B_390_3278_0 ) * _rtP -> P_248 ; }
else { _rtB -> B_390_3284_0 = _rtB -> B_390_3282_0 ; } rtb_B_390_2970_0 =
_rtP -> P_2427 * muDoubleScalarAtan2 ( _rtB -> B_390_3284_0 , _rtB ->
B_390_3276_0 ) * _rtP -> P_2428 ; B_390_3289_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3276_0 , _rtB -> B_390_3284_0 ) * muDoubleScalarCos (
rtb_B_390_2970_0 ) ; B_390_3289_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3276_0 , _rtB -> B_390_3284_0 ) * muDoubleScalarSin ( rtb_B_390_2970_0
) ; _rtB -> B_390_3290_0 = _rtX -> integrator_CSTATE_nm2 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2430 ; B_390_3291_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kw . Last , _rtDW -> TransportDelay_IWORK_kw . Tail ,
_rtDW -> TransportDelay_IWORK_kw . Head , _rtP -> P_2431 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3293_0 = _rtP -> P_2432 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3293_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3295_0 = _rtDW -> Memory_PreviousInput_g3o ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3297_0 = ( _rtB -> B_390_3290_0 -
B_390_3291_0 ) * _rtP -> P_251 ; } else { _rtB -> B_390_3297_0 = _rtB ->
B_390_3295_0 ; } _rtB -> B_390_3298_0 = _rtX -> integrator_CSTATE_f3 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eo .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_eo . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2435 ; B_390_3299_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_bf . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bf . Last , _rtDW -> TransportDelay_IWORK_bf . Tail ,
_rtDW -> TransportDelay_IWORK_bf . Head , _rtP -> P_2436 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3301_0 = _rtP -> P_2437 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3301_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3303_0 = _rtDW -> Memory_PreviousInput_m1 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3305_0 = ( _rtB -> B_390_3298_0 -
B_390_3299_0 ) * _rtP -> P_250 ; } else { _rtB -> B_390_3305_0 = _rtB ->
B_390_3303_0 ; } rtb_B_390_2970_0 = _rtP -> P_2439 * muDoubleScalarAtan2 (
_rtB -> B_390_3305_0 , _rtB -> B_390_3297_0 ) * _rtP -> P_2440 ; B_390_3310_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3297_0 , _rtB -> B_390_3305_0 ) *
muDoubleScalarCos ( rtb_B_390_2970_0 ) ; B_390_3310_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3297_0 , _rtB -> B_390_3305_0 ) *
muDoubleScalarSin ( rtb_B_390_2970_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_3312_0 = _rtB -> B_390_3311_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3312_0 ,
B_390_3268_0 , B_390_3289_0 , B_390_3310_0 , & _rtB -> PosSeqComputation_nf ,
& _rtDW -> PosSeqComputation_nf , & _rtP -> PosSeqComputation_nf ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3315_0 = _rtB
-> B_390_3314_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_3315_0 , B_390_3268_0 , B_390_3289_0 , B_390_3310_0 , & _rtB ->
NegSeqComputation_oh , & _rtDW -> NegSeqComputation_oh , & _rtP ->
NegSeqComputation_oh ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3318_0 = _rtB -> B_390_3317_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_3318_0 ,
B_390_3268_0 , B_390_3289_0 , B_390_3310_0 , & _rtB -> ZeroSeqComputation_k0
, & _rtDW -> ZeroSeqComputation_k0 , & _rtP -> ZeroSeqComputation_k0 ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_nf .
B_22_2_0 . im , _rtB -> PosSeqComputation_nf . B_22_2_0 . re ) ; _rtB ->
B_390_3321_0 = _rtP -> P_2444 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_nf . B_22_2_0 . re , _rtB -> PosSeqComputation_nf .
B_22_2_0 . im ) ; ssCallAccelRunBlock ( S , 390 , 3322 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_390_1508_0 [ 0 ] = _rtB -> B_390_3172_0 ; _rtB -> B_390_1508_0
[ 1 ] = _rtB -> B_390_3246_0 ; _rtB -> B_390_1508_0 [ 2 ] = _rtB ->
B_390_3247_0 ; _rtB -> B_390_1508_0 [ 3 ] = _rtB -> B_390_3321_0 ;
ssCallAccelRunBlock ( S , 390 , 3324 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3328_0 [ 0 ]
= _rtP -> P_2445 * _rtB -> B_0_14_0 [ 120 ] * _rtP -> P_2448 ; _rtB ->
B_390_3328_0 [ 1 ] = _rtP -> P_2446 * _rtB -> B_0_14_0 [ 121 ] * _rtP ->
P_2448 ; _rtB -> B_390_3328_0 [ 2 ] = _rtP -> P_2447 * _rtB -> B_0_14_0 [ 122
] * _rtP -> P_2448 ; _rtB -> B_390_3332_0 [ 0 ] = _rtP -> P_2449 * _rtB ->
B_0_14_0 [ 65 ] * _rtP -> P_2452 ; _rtB -> B_390_3332_0 [ 1 ] = _rtP ->
P_2450 * _rtB -> B_0_14_0 [ 66 ] * _rtP -> P_2452 ; _rtB -> B_390_3332_0 [ 2
] = _rtP -> P_2451 * _rtB -> B_0_14_0 [ 67 ] * _rtP -> P_2452 ; } _rtB ->
B_390_3368_0 = ( muDoubleScalarSin ( _rtP -> P_2455 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2456 ) * _rtP -> P_2453 + _rtP -> P_2454 ) * _rtB -> B_390_3328_0
[ 0 ] ; _rtB -> B_390_3370_0 = ( muDoubleScalarSin ( _rtP -> P_2459 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2460 ) * _rtP -> P_2457 + _rtP -> P_2458
) * _rtB -> B_390_3328_0 [ 0 ] ; _rtB -> B_390_3372_0 = ( muDoubleScalarSin (
_rtP -> P_2463 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2464 ) * _rtP -> P_2461
+ _rtP -> P_2462 ) * _rtB -> B_390_3328_0 [ 1 ] ; _rtB -> B_390_3374_0 = (
muDoubleScalarSin ( _rtP -> P_2467 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2468
) * _rtP -> P_2465 + _rtP -> P_2466 ) * _rtB -> B_390_3328_0 [ 1 ] ; _rtB ->
B_390_3376_0 = ( muDoubleScalarSin ( _rtP -> P_2471 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2472 ) * _rtP -> P_2469 + _rtP -> P_2470 ) * _rtB -> B_390_3328_0
[ 2 ] ; _rtB -> B_390_3378_0 = ( muDoubleScalarSin ( _rtP -> P_2475 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2476 ) * _rtP -> P_2473 + _rtP -> P_2474
) * _rtB -> B_390_3328_0 [ 2 ] ; B_390_3379_0 = _rtP -> P_2477 *
B_390_401_1_idx_0 ; _rtB -> B_390_3381_0 = ( muDoubleScalarSin ( _rtP ->
P_2480 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2481 ) * _rtP -> P_2478 + _rtP
-> P_2479 ) * _rtB -> B_390_3332_0 [ 0 ] ; _rtB -> B_390_3383_0 = (
muDoubleScalarSin ( _rtP -> P_2484 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2485
) * _rtP -> P_2482 + _rtP -> P_2483 ) * _rtB -> B_390_3332_0 [ 0 ] ; _rtB ->
B_390_3385_0 = ( muDoubleScalarSin ( _rtP -> P_2488 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2489 ) * _rtP -> P_2486 + _rtP -> P_2487 ) * _rtB -> B_390_3332_0
[ 1 ] ; _rtB -> B_390_3387_0 = ( muDoubleScalarSin ( _rtP -> P_2492 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2493 ) * _rtP -> P_2490 + _rtP -> P_2491
) * _rtB -> B_390_3332_0 [ 1 ] ; _rtB -> B_390_3389_0 = ( muDoubleScalarSin (
_rtP -> P_2496 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2497 ) * _rtP -> P_2494
+ _rtP -> P_2495 ) * _rtB -> B_390_3332_0 [ 2 ] ; _rtB -> B_390_3391_0 = (
muDoubleScalarSin ( _rtP -> P_2500 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2501
) * _rtP -> P_2498 + _rtP -> P_2499 ) * _rtB -> B_390_3332_0 [ 2 ] ; _rtB ->
B_390_3393_0 = ( muDoubleScalarSin ( _rtP -> P_2504 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2505 ) * _rtP -> P_2502 + _rtP -> P_2503 ) * _rtB -> B_390_3328_0
[ 0 ] ; _rtB -> B_390_3395_0 = ( muDoubleScalarSin ( _rtP -> P_2508 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2509 ) * _rtP -> P_2506 + _rtP -> P_2507
) * _rtB -> B_390_3328_0 [ 0 ] ; _rtB -> B_390_3397_0 = ( muDoubleScalarSin (
_rtP -> P_2512 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2513 ) * _rtP -> P_2510
+ _rtP -> P_2511 ) * _rtB -> B_390_3328_0 [ 1 ] ; _rtB -> B_390_3399_0 = (
muDoubleScalarSin ( _rtP -> P_2516 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2517
) * _rtP -> P_2514 + _rtP -> P_2515 ) * _rtB -> B_390_3328_0 [ 1 ] ; _rtB ->
B_390_3401_0 = ( muDoubleScalarSin ( _rtP -> P_2520 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2521 ) * _rtP -> P_2518 + _rtP -> P_2519 ) * _rtB -> B_390_3328_0
[ 2 ] ; _rtB -> B_390_3403_0 = ( muDoubleScalarSin ( _rtP -> P_2524 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2525 ) * _rtP -> P_2522 + _rtP -> P_2523
) * _rtB -> B_390_3328_0 [ 2 ] ; _rtB -> B_390_3405_0 = ( muDoubleScalarSin (
_rtP -> P_2528 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2529 ) * _rtP -> P_2526
+ _rtP -> P_2527 ) * _rtB -> B_390_3332_0 [ 0 ] ; _rtB -> B_390_3407_0 = (
muDoubleScalarSin ( _rtP -> P_2532 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2533
) * _rtP -> P_2530 + _rtP -> P_2531 ) * _rtB -> B_390_3332_0 [ 0 ] ; _rtB ->
B_390_3409_0 = ( muDoubleScalarSin ( _rtP -> P_2536 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2537 ) * _rtP -> P_2534 + _rtP -> P_2535 ) * _rtB -> B_390_3332_0
[ 1 ] ; _rtB -> B_390_3411_0 = ( muDoubleScalarSin ( _rtP -> P_2540 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2541 ) * _rtP -> P_2538 + _rtP -> P_2539
) * _rtB -> B_390_3332_0 [ 1 ] ; _rtB -> B_390_3413_0 = ( muDoubleScalarSin (
_rtP -> P_2544 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2545 ) * _rtP -> P_2542
+ _rtP -> P_2543 ) * _rtB -> B_390_3332_0 [ 2 ] ; _rtB -> B_390_3415_0 = (
muDoubleScalarSin ( _rtP -> P_2548 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2549
) * _rtP -> P_2546 + _rtP -> P_2547 ) * _rtB -> B_390_3332_0 [ 2 ] ;
B_390_3416_0 = _rtP -> P_2550 * rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 2
, 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_3419_0 >= _rtP -> P_2556 ) {
rtb_B_390_3422_0 = _rtB -> B_390_3418_0 ; } else { rtb_B_390_3422_0 =
rt_Lookup ( _rtP -> P_2554 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_2555 ) ;
} if ( _rtB -> B_390_3423_0 >= _rtP -> P_2559 ) { _rtB -> B_390_3426_0 =
rtb_B_390_3422_0 ; } else { _rtB -> B_390_3426_0 = _rtB -> B_390_3424_0 ; } }
_rtB -> B_390_3428_0 = rt_Lookup ( _rtP -> P_2560 , 5 , ssGetT ( S ) , _rtP
-> P_2561 ) ; if ( _rtB -> B_390_3417_0 >= _rtP -> P_2562 ) { _rtB ->
B_390_3429_0 = _rtB -> B_390_3426_0 ; } else { _rtB -> B_390_3429_0 = _rtB ->
B_390_3428_0 ; } if ( ( _rtDW -> TimeStampA_mm >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_eo >= ssGetT ( S ) ) ) { _rtB -> B_390_3430_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_mm ; lastU = & _rtDW -> LastUAtTimeA_ed
; if ( _rtDW -> TimeStampA_mm < _rtDW -> TimeStampB_eo ) { if ( _rtDW ->
TimeStampB_eo < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_eo ;
lastU = & _rtDW -> LastUAtTimeB_i ; } } else { if ( _rtDW -> TimeStampA_mm >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_eo ; lastU = & _rtDW
-> LastUAtTimeB_i ; } } _rtB -> B_390_3430_0 = ( _rtB -> B_390_3428_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_3436_0 >= _rtP -> P_2566 ) {
_rtB -> B_390_3438_0 = rtb_B_390_3422_0 ; } else { _rtB -> B_390_3438_0 =
_rtB -> B_390_3424_0 ; } } _rtB -> B_390_3440_0 = rt_Lookup ( _rtP -> P_2567
, 5 , ssGetT ( S ) , _rtP -> P_2568 ) ; if ( _rtB -> B_390_3435_0 >= _rtP ->
P_2569 ) { _rtB -> B_390_3441_0 = _rtB -> B_390_3438_0 ; } else { _rtB ->
B_390_3441_0 = _rtB -> B_390_3440_0 ; } if ( ( _rtDW -> TimeStampA_j >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_n >= ssGetT ( S ) ) ) { _rtB ->
B_390_3442_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_j ; lastU
= & _rtDW -> LastUAtTimeA_dt ; if ( _rtDW -> TimeStampA_j < _rtDW ->
TimeStampB_n ) { if ( _rtDW -> TimeStampB_n < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_n ; lastU = & _rtDW -> LastUAtTimeB_po
; } } else { if ( _rtDW -> TimeStampA_j >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_n ; lastU = & _rtDW -> LastUAtTimeB_po ; } } _rtB ->
B_390_3442_0 = ( _rtB -> B_390_3440_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_3448_0 >= _rtP -> P_2573 ) { _rtB -> B_390_3450_0 =
rtb_B_390_3422_0 ; } else { _rtB -> B_390_3450_0 = _rtB -> B_390_3424_0 ; } }
_rtB -> B_390_3452_0 = rt_Lookup ( _rtP -> P_2574 , 5 , ssGetT ( S ) , _rtP
-> P_2575 ) ; if ( _rtB -> B_390_3447_0 >= _rtP -> P_2576 ) { _rtB ->
B_390_3453_0 = _rtB -> B_390_3450_0 ; } else { _rtB -> B_390_3453_0 = _rtB ->
B_390_3452_0 ; } if ( ( _rtDW -> TimeStampA_g >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_d >= ssGetT ( S ) ) ) { _rtB -> B_390_3454_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_g ; lastU = & _rtDW -> LastUAtTimeA_g ;
if ( _rtDW -> TimeStampA_g < _rtDW -> TimeStampB_d ) { if ( _rtDW ->
TimeStampB_d < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_d ;
lastU = & _rtDW -> LastUAtTimeB_mm ; } } else { if ( _rtDW -> TimeStampA_g >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_d ; lastU = & _rtDW ->
LastUAtTimeB_mm ; } } _rtB -> B_390_3454_0 = ( _rtB -> B_390_3452_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } _rtB -> B_390_3471_0 = _rtX ->
integrator_CSTATE_dp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ca . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ca . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2579 ;
B_390_3472_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kd . Last , _rtDW ->
TransportDelay_IWORK_kd . Tail , _rtDW -> TransportDelay_IWORK_kd . Head ,
_rtP -> P_2580 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3474_0 = _rtP -> P_2581 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3474_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3476_0 =
_rtDW -> Memory_PreviousInput_ev ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3478_0 = ( _rtB -> B_390_3471_0 - B_390_3472_0 ) * _rtP -> P_277 ; }
else { _rtB -> B_390_3478_0 = _rtB -> B_390_3476_0 ; } _rtB -> B_390_3479_0 =
_rtX -> integrator_CSTATE_hz ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_in . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_in . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2584 ;
B_390_3480_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_mm . Last , _rtDW ->
TransportDelay_IWORK_mm . Tail , _rtDW -> TransportDelay_IWORK_mm . Head ,
_rtP -> P_2585 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3482_0 = _rtP -> P_2586 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3482_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3484_0 =
_rtDW -> Memory_PreviousInput_fis ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3486_0 = ( _rtB -> B_390_3479_0 - B_390_3480_0 ) * _rtP -> P_276 ; }
else { _rtB -> B_390_3486_0 = _rtB -> B_390_3484_0 ; } rtb_B_390_3422_0 =
_rtP -> P_2588 * muDoubleScalarAtan2 ( _rtB -> B_390_3486_0 , _rtB ->
B_390_3478_0 ) * _rtP -> P_2589 ; B_390_3491_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3478_0 , _rtB -> B_390_3486_0 ) * muDoubleScalarCos (
rtb_B_390_3422_0 ) ; B_390_3491_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3478_0 , _rtB -> B_390_3486_0 ) * muDoubleScalarSin ( rtb_B_390_3422_0
) ; _rtB -> B_390_3492_0 = _rtX -> integrator_CSTATE_oa ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2591 ; B_390_3493_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_gn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gn . Last , _rtDW -> TransportDelay_IWORK_gn . Tail ,
_rtDW -> TransportDelay_IWORK_gn . Head , _rtP -> P_2592 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3495_0 = _rtP -> P_2593 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3495_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3497_0 = _rtDW -> Memory_PreviousInput_ij ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3499_0 = ( _rtB -> B_390_3492_0 -
B_390_3493_0 ) * _rtP -> P_279 ; } else { _rtB -> B_390_3499_0 = _rtB ->
B_390_3497_0 ; } _rtB -> B_390_3500_0 = _rtX -> integrator_CSTATE_ow ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2596 ; B_390_3501_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_fd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fd . Last , _rtDW -> TransportDelay_IWORK_fd . Tail ,
_rtDW -> TransportDelay_IWORK_fd . Head , _rtP -> P_2597 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3503_0 = _rtP -> P_2598 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3503_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3505_0 = _rtDW -> Memory_PreviousInput_jy ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3507_0 = ( _rtB -> B_390_3500_0 -
B_390_3501_0 ) * _rtP -> P_278 ; } else { _rtB -> B_390_3507_0 = _rtB ->
B_390_3505_0 ; } rtb_B_390_3422_0 = _rtP -> P_2600 * muDoubleScalarAtan2 (
_rtB -> B_390_3507_0 , _rtB -> B_390_3499_0 ) * _rtP -> P_2601 ; B_390_3512_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3499_0 , _rtB -> B_390_3507_0 ) *
muDoubleScalarCos ( rtb_B_390_3422_0 ) ; B_390_3512_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3499_0 , _rtB -> B_390_3507_0 ) *
muDoubleScalarSin ( rtb_B_390_3422_0 ) ; _rtB -> B_390_3513_0 = _rtX ->
integrator_CSTATE_n3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d53 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_d53 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2603 ;
B_390_3514_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gz .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gz . Last , _rtDW ->
TransportDelay_IWORK_gz . Tail , _rtDW -> TransportDelay_IWORK_gz . Head ,
_rtP -> P_2604 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3516_0 = _rtP -> P_2605 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3516_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3518_0 =
_rtDW -> Memory_PreviousInput_grf ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3520_0 = ( _rtB -> B_390_3513_0 - B_390_3514_0 ) * _rtP -> P_281 ; }
else { _rtB -> B_390_3520_0 = _rtB -> B_390_3518_0 ; } _rtB -> B_390_3521_0 =
_rtX -> integrator_CSTATE_dn3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ee . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ee . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2608 ;
B_390_3522_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ma4 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ma4 . Last , _rtDW ->
TransportDelay_IWORK_ma4 . Tail , _rtDW -> TransportDelay_IWORK_ma4 . Head ,
_rtP -> P_2609 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3524_0 = _rtP -> P_2610 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3524_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3526_0 =
_rtDW -> Memory_PreviousInput_pe ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3528_0 = ( _rtB -> B_390_3521_0 - B_390_3522_0 ) * _rtP -> P_280 ; }
else { _rtB -> B_390_3528_0 = _rtB -> B_390_3526_0 ; } rtb_B_390_3422_0 =
_rtP -> P_2612 * muDoubleScalarAtan2 ( _rtB -> B_390_3528_0 , _rtB ->
B_390_3520_0 ) * _rtP -> P_2613 ; B_390_3533_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3520_0 , _rtB -> B_390_3528_0 ) * muDoubleScalarCos (
rtb_B_390_3422_0 ) ; B_390_3533_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3520_0 , _rtB -> B_390_3528_0 ) * muDoubleScalarSin ( rtb_B_390_3422_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3535_0 = _rtB -> B_390_3534_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_3535_0 , B_390_3491_0 , B_390_3512_0 , B_390_3533_0 , &
_rtB -> PosSeqComputation_pa , & _rtDW -> PosSeqComputation_pa , & _rtP ->
PosSeqComputation_pa ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3538_0 = _rtB -> B_390_3537_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3538_0 ,
B_390_3491_0 , B_390_3512_0 , B_390_3533_0 , & _rtB -> NegSeqComputation_i ,
& _rtDW -> NegSeqComputation_i , & _rtP -> NegSeqComputation_i ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3541_0 = _rtB
-> B_390_3540_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_3541_0 , B_390_3491_0 , B_390_3512_0 , B_390_3533_0 , & _rtB ->
ZeroSeqComputation_mp , & _rtDW -> ZeroSeqComputation_mp , & _rtP ->
ZeroSeqComputation_mp ) ; rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_pa . B_22_2_0 . re , _rtB -> PosSeqComputation_pa .
B_22_2_0 . im ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_pa . B_22_2_0 . im , _rtB -> PosSeqComputation_pa .
B_22_2_0 . re ) ; _rtB -> B_390_3544_0 = _rtX -> integrator_CSTATE_lx ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gn .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gn . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2618 ; B_390_3545_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kl . Last , _rtDW -> TransportDelay_IWORK_kl . Tail ,
_rtDW -> TransportDelay_IWORK_kl . Head , _rtP -> P_2619 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3547_0 = _rtP -> P_2620 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3547_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3549_0 = _rtDW -> Memory_PreviousInput_ic ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3551_0 = ( _rtB -> B_390_3544_0 -
B_390_3545_0 ) * _rtP -> P_283 ; } else { _rtB -> B_390_3551_0 = _rtB ->
B_390_3549_0 ; } _rtB -> B_390_3552_0 = _rtX -> integrator_CSTATE_am ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hq .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hq . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2623 ; B_390_3553_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_er . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_er . Last , _rtDW -> TransportDelay_IWORK_er . Tail ,
_rtDW -> TransportDelay_IWORK_er . Head , _rtP -> P_2624 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3555_0 = _rtP -> P_2625 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3555_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3557_0 = _rtDW -> Memory_PreviousInput_k0 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3559_0 = ( _rtB -> B_390_3552_0 -
B_390_3553_0 ) * _rtP -> P_282 ; } else { _rtB -> B_390_3559_0 = _rtB ->
B_390_3557_0 ; } rtb_B_390_3422_0 = _rtP -> P_2627 * muDoubleScalarAtan2 (
_rtB -> B_390_3559_0 , _rtB -> B_390_3551_0 ) * _rtP -> P_2628 ; B_390_3564_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3551_0 , _rtB -> B_390_3559_0 ) *
muDoubleScalarCos ( rtb_B_390_3422_0 ) ; B_390_3564_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3551_0 , _rtB -> B_390_3559_0 ) *
muDoubleScalarSin ( rtb_B_390_3422_0 ) ; _rtB -> B_390_3565_0 = _rtX ->
integrator_CSTATE_gew ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2630 ;
B_390_3566_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hq . Last , _rtDW ->
TransportDelay_IWORK_hq . Tail , _rtDW -> TransportDelay_IWORK_hq . Head ,
_rtP -> P_2631 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3568_0 = _rtP -> P_2632 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3568_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3570_0 =
_rtDW -> Memory_PreviousInput_ofm ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3572_0 = ( _rtB -> B_390_3565_0 - B_390_3566_0 ) * _rtP -> P_285 ; }
else { _rtB -> B_390_3572_0 = _rtB -> B_390_3570_0 ; } _rtB -> B_390_3573_0 =
_rtX -> integrator_CSTATE_br ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_dy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dy . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2635 ;
B_390_3574_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hj2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hj2 . Last , _rtDW ->
TransportDelay_IWORK_hj2 . Tail , _rtDW -> TransportDelay_IWORK_hj2 . Head ,
_rtP -> P_2636 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3576_0 = _rtP -> P_2637 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3576_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3578_0 =
_rtDW -> Memory_PreviousInput_ico ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3580_0 = ( _rtB -> B_390_3573_0 - B_390_3574_0 ) * _rtP -> P_284 ; }
else { _rtB -> B_390_3580_0 = _rtB -> B_390_3578_0 ; } rtb_B_390_3422_0 =
_rtP -> P_2639 * muDoubleScalarAtan2 ( _rtB -> B_390_3580_0 , _rtB ->
B_390_3572_0 ) * _rtP -> P_2640 ; B_390_3585_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3572_0 , _rtB -> B_390_3580_0 ) * muDoubleScalarCos (
rtb_B_390_3422_0 ) ; B_390_3585_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3572_0 , _rtB -> B_390_3580_0 ) * muDoubleScalarSin ( rtb_B_390_3422_0
) ; _rtB -> B_390_3586_0 = _rtX -> integrator_CSTATE_bl ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l5 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l5
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2642 ; B_390_3587_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_atb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_atb . Last , _rtDW -> TransportDelay_IWORK_atb . Tail ,
_rtDW -> TransportDelay_IWORK_atb . Head , _rtP -> P_2643 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3589_0 = _rtP -> P_2644 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3589_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3591_0 = _rtDW -> Memory_PreviousInput_ji ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3593_0 = ( _rtB -> B_390_3586_0 -
B_390_3587_0 ) * _rtP -> P_287 ; } else { _rtB -> B_390_3593_0 = _rtB ->
B_390_3591_0 ; } _rtB -> B_390_3594_0 = _rtX -> integrator_CSTATE_oe ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oy .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oy . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2647 ; B_390_3595_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_or . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_or . Last , _rtDW -> TransportDelay_IWORK_or . Tail ,
_rtDW -> TransportDelay_IWORK_or . Head , _rtP -> P_2648 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3597_0 = _rtP -> P_2649 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3597_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3599_0 = _rtDW -> Memory_PreviousInput_nm ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3601_0 = ( _rtB -> B_390_3594_0 -
B_390_3595_0 ) * _rtP -> P_286 ; } else { _rtB -> B_390_3601_0 = _rtB ->
B_390_3599_0 ; } rtb_B_390_3422_0 = _rtP -> P_2651 * muDoubleScalarAtan2 (
_rtB -> B_390_3601_0 , _rtB -> B_390_3593_0 ) * _rtP -> P_2652 ; B_390_3606_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3593_0 , _rtB -> B_390_3601_0 ) *
muDoubleScalarCos ( rtb_B_390_3422_0 ) ; B_390_3606_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3593_0 , _rtB -> B_390_3601_0 ) *
muDoubleScalarSin ( rtb_B_390_3422_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_3608_0 = _rtB -> B_390_3607_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3608_0 ,
B_390_3564_0 , B_390_3585_0 , B_390_3606_0 , & _rtB -> PosSeqComputation_g ,
& _rtDW -> PosSeqComputation_g , & _rtP -> PosSeqComputation_g ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3611_0 = _rtB
-> B_390_3610_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_3611_0 , B_390_3564_0 , B_390_3585_0 , B_390_3606_0 , & _rtB ->
NegSeqComputation_lk , & _rtDW -> NegSeqComputation_lk , & _rtP ->
NegSeqComputation_lk ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3614_0 = _rtB -> B_390_3613_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_3614_0 ,
B_390_3564_0 , B_390_3585_0 , B_390_3606_0 , & _rtB -> ZeroSeqComputation_od
, & _rtDW -> ZeroSeqComputation_od , & _rtP -> ZeroSeqComputation_od ) ;
rtb_B_390_3422_0 = rtb_B_390_239_0 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_g . B_22_2_0 . re , _rtB -> PosSeqComputation_g . B_22_2_0
. im ) * _rtP -> P_2656 ; muDoubleScalarSinCos ( _rtP -> P_2659 * ( _rtP ->
P_2657 * rtb_B_390_4_1 - _rtP -> P_2658 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_g . B_22_2_0 . im , _rtB -> PosSeqComputation_g . B_22_2_0
. re ) ) , & rtb_B_390_2970_0 , & rtb_B_390_2118_0 ) ; _rtB -> B_390_3624_0 =
rtb_B_390_3422_0 * rtb_B_390_2118_0 ; _rtB -> B_390_3625_0 = _rtX ->
integrator_CSTATE_k3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cmj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cmj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2661 ;
B_390_3626_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dw .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dw . Last , _rtDW ->
TransportDelay_IWORK_dw . Tail , _rtDW -> TransportDelay_IWORK_dw . Head ,
_rtP -> P_2662 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3628_0 = _rtP -> P_2663 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3628_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3630_0 =
_rtDW -> Memory_PreviousInput_jwz ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3632_0 = ( _rtB -> B_390_3625_0 - B_390_3626_0 ) * _rtP -> P_289 ; }
else { _rtB -> B_390_3632_0 = _rtB -> B_390_3630_0 ; } _rtB -> B_390_3633_0 =
_rtX -> integrator_CSTATE_ay ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ojz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2666
; B_390_3634_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_c5 . Last , _rtDW ->
TransportDelay_IWORK_c5 . Tail , _rtDW -> TransportDelay_IWORK_c5 . Head ,
_rtP -> P_2667 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3636_0 = _rtP -> P_2668 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3636_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3638_0 =
_rtDW -> Memory_PreviousInput_lb ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3640_0 = ( _rtB -> B_390_3633_0 - B_390_3634_0 ) * _rtP -> P_288 ; }
else { _rtB -> B_390_3640_0 = _rtB -> B_390_3638_0 ; } rtb_B_390_2118_0 =
_rtP -> P_2670 * muDoubleScalarAtan2 ( _rtB -> B_390_3640_0 , _rtB ->
B_390_3632_0 ) * _rtP -> P_2671 ; B_390_3645_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3632_0 , _rtB -> B_390_3640_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_3645_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3632_0 , _rtB -> B_390_3640_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; _rtB -> B_390_3646_0 = _rtX -> integrator_CSTATE_hx ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2673 ; B_390_3647_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_hj0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj0 . Last , _rtDW -> TransportDelay_IWORK_hj0 . Tail ,
_rtDW -> TransportDelay_IWORK_hj0 . Head , _rtP -> P_2674 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3649_0 = _rtP -> P_2675 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3649_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3651_0 = _rtDW -> Memory_PreviousInput_ao ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3653_0 = ( _rtB -> B_390_3646_0 -
B_390_3647_0 ) * _rtP -> P_291 ; } else { _rtB -> B_390_3653_0 = _rtB ->
B_390_3651_0 ; } _rtB -> B_390_3654_0 = _rtX -> integrator_CSTATE_nc ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cvh . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2678 ; B_390_3655_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_da . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_da . Last , _rtDW -> TransportDelay_IWORK_da . Tail ,
_rtDW -> TransportDelay_IWORK_da . Head , _rtP -> P_2679 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3657_0 = _rtP -> P_2680 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3657_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3659_0 = _rtDW -> Memory_PreviousInput_kf ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3661_0 = ( _rtB -> B_390_3654_0 -
B_390_3655_0 ) * _rtP -> P_290 ; } else { _rtB -> B_390_3661_0 = _rtB ->
B_390_3659_0 ; } rtb_B_390_2118_0 = _rtP -> P_2682 * muDoubleScalarAtan2 (
_rtB -> B_390_3661_0 , _rtB -> B_390_3653_0 ) * _rtP -> P_2683 ; B_390_3666_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3653_0 , _rtB -> B_390_3661_0 ) *
muDoubleScalarCos ( rtb_B_390_2118_0 ) ; B_390_3666_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3653_0 , _rtB -> B_390_3661_0 ) *
muDoubleScalarSin ( rtb_B_390_2118_0 ) ; _rtB -> B_390_3667_0 = _rtX ->
integrator_CSTATE_jf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ig . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ig . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2685 ;
B_390_3668_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j0 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j0 . Last , _rtDW ->
TransportDelay_IWORK_j0 . Tail , _rtDW -> TransportDelay_IWORK_j0 . Head ,
_rtP -> P_2686 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3670_0 = _rtP -> P_2687 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3670_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3672_0 =
_rtDW -> Memory_PreviousInput_pzv ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3674_0 = ( _rtB -> B_390_3667_0 - B_390_3668_0 ) * _rtP -> P_293 ; }
else { _rtB -> B_390_3674_0 = _rtB -> B_390_3672_0 ; } _rtB -> B_390_3675_0 =
_rtX -> integrator_CSTATE_mc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_gf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gf . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2690 ;
B_390_3676_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nd . Last , _rtDW ->
TransportDelay_IWORK_nd . Tail , _rtDW -> TransportDelay_IWORK_nd . Head ,
_rtP -> P_2691 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3678_0 = _rtP -> P_2692 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3678_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3680_0 =
_rtDW -> Memory_PreviousInput_kv ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3682_0 = ( _rtB -> B_390_3675_0 - B_390_3676_0 ) * _rtP -> P_292 ; }
else { _rtB -> B_390_3682_0 = _rtB -> B_390_3680_0 ; } rtb_B_390_2118_0 =
_rtP -> P_2694 * muDoubleScalarAtan2 ( _rtB -> B_390_3682_0 , _rtB ->
B_390_3674_0 ) * _rtP -> P_2695 ; B_390_3687_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3674_0 , _rtB -> B_390_3682_0 ) * muDoubleScalarCos (
rtb_B_390_2118_0 ) ; B_390_3687_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3674_0 , _rtB -> B_390_3682_0 ) * muDoubleScalarSin ( rtb_B_390_2118_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3689_0 = _rtB -> B_390_3688_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_3689_0 , B_390_3645_0 , B_390_3666_0 , B_390_3687_0 , &
_rtB -> PosSeqComputation_ni , & _rtDW -> PosSeqComputation_ni , & _rtP ->
PosSeqComputation_ni ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3692_0 = _rtB -> B_390_3691_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3692_0 ,
B_390_3645_0 , B_390_3666_0 , B_390_3687_0 , & _rtB -> NegSeqComputation_n5 ,
& _rtDW -> NegSeqComputation_n5 , & _rtP -> NegSeqComputation_n5 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3695_0 = _rtB
-> B_390_3694_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_3695_0 , B_390_3645_0 , B_390_3666_0 , B_390_3687_0 , & _rtB ->
ZeroSeqComputation_fl , & _rtDW -> ZeroSeqComputation_fl , & _rtP ->
ZeroSeqComputation_fl ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ni . B_22_2_0 . im , _rtB -> PosSeqComputation_ni .
B_22_2_0 . re ) ; _rtB -> B_390_3698_0 = _rtP -> P_2699 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_ni . B_22_2_0 . re , _rtB -> PosSeqComputation_ni
. B_22_2_0 . im ) ; _rtB -> B_390_3699_0 = rtb_B_390_3422_0 *
rtb_B_390_2970_0 ; _rtB -> B_390_3700_0 = _rtX -> integrator_CSTATE_av ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hs .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hs . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2701 ; B_390_3701_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ou . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ou . Last , _rtDW -> TransportDelay_IWORK_ou . Tail ,
_rtDW -> TransportDelay_IWORK_ou . Head , _rtP -> P_2702 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3703_0 = _rtP -> P_2703 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3703_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3705_0 = _rtDW -> Memory_PreviousInput_m2 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3707_0 = ( _rtB -> B_390_3700_0 -
B_390_3701_0 ) * _rtP -> P_271 ; } else { _rtB -> B_390_3707_0 = _rtB ->
B_390_3705_0 ; } _rtB -> B_390_3708_0 = _rtX -> integrator_CSTATE_gy ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o0x . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2706 ; B_390_3709_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_my . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_my . Last , _rtDW -> TransportDelay_IWORK_my . Tail ,
_rtDW -> TransportDelay_IWORK_my . Head , _rtP -> P_2707 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3711_0 = _rtP -> P_2708 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3711_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3713_0 = _rtDW -> Memory_PreviousInput_il ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3715_0 = ( _rtB -> B_390_3708_0 -
B_390_3709_0 ) * _rtP -> P_270 ; } else { _rtB -> B_390_3715_0 = _rtB ->
B_390_3713_0 ; } rtb_B_390_3422_0 = _rtP -> P_2710 * muDoubleScalarAtan2 (
_rtB -> B_390_3715_0 , _rtB -> B_390_3707_0 ) * _rtP -> P_2711 ; B_390_3720_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3707_0 , _rtB -> B_390_3715_0 ) *
muDoubleScalarCos ( rtb_B_390_3422_0 ) ; B_390_3720_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3707_0 , _rtB -> B_390_3715_0 ) *
muDoubleScalarSin ( rtb_B_390_3422_0 ) ; _rtB -> B_390_3721_0 = _rtX ->
integrator_CSTATE_i3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fy . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2713 ;
B_390_3722_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ayh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ayh . Last , _rtDW ->
TransportDelay_IWORK_ayh . Tail , _rtDW -> TransportDelay_IWORK_ayh . Head ,
_rtP -> P_2714 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3724_0 = _rtP -> P_2715 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3724_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3726_0 =
_rtDW -> Memory_PreviousInput_pi ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3728_0 = ( _rtB -> B_390_3721_0 - B_390_3722_0 ) * _rtP -> P_273 ; }
else { _rtB -> B_390_3728_0 = _rtB -> B_390_3726_0 ; } _rtB -> B_390_3729_0 =
_rtX -> integrator_CSTATE_kg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_n0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2718 ;
B_390_3730_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jyu .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jyu . Last , _rtDW ->
TransportDelay_IWORK_jyu . Tail , _rtDW -> TransportDelay_IWORK_jyu . Head ,
_rtP -> P_2719 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3732_0 = _rtP -> P_2720 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3732_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3734_0 =
_rtDW -> Memory_PreviousInput_ol ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3736_0 = ( _rtB -> B_390_3729_0 - B_390_3730_0 ) * _rtP -> P_272 ; }
else { _rtB -> B_390_3736_0 = _rtB -> B_390_3734_0 ; } rtb_B_390_3422_0 =
_rtP -> P_2722 * muDoubleScalarAtan2 ( _rtB -> B_390_3736_0 , _rtB ->
B_390_3728_0 ) * _rtP -> P_2723 ; B_390_3741_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3728_0 , _rtB -> B_390_3736_0 ) * muDoubleScalarCos (
rtb_B_390_3422_0 ) ; B_390_3741_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3728_0 , _rtB -> B_390_3736_0 ) * muDoubleScalarSin ( rtb_B_390_3422_0
) ; _rtB -> B_390_3742_0 = _rtX -> integrator_CSTATE_fqj ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2725 ; B_390_3743_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ps . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ps . Last , _rtDW -> TransportDelay_IWORK_ps . Tail ,
_rtDW -> TransportDelay_IWORK_ps . Head , _rtP -> P_2726 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3745_0 = _rtP -> P_2727 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3745_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3747_0 = _rtDW -> Memory_PreviousInput_cj ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3749_0 = ( _rtB -> B_390_3742_0 -
B_390_3743_0 ) * _rtP -> P_275 ; } else { _rtB -> B_390_3749_0 = _rtB ->
B_390_3747_0 ; } _rtB -> B_390_3750_0 = _rtX -> integrator_CSTATE_gii ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2730 ; B_390_3751_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ab . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ab . Last , _rtDW -> TransportDelay_IWORK_ab . Tail ,
_rtDW -> TransportDelay_IWORK_ab . Head , _rtP -> P_2731 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3753_0 = _rtP -> P_2732 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3753_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3755_0 = _rtDW -> Memory_PreviousInput_imb ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3757_0 = ( _rtB -> B_390_3750_0 -
B_390_3751_0 ) * _rtP -> P_274 ; } else { _rtB -> B_390_3757_0 = _rtB ->
B_390_3755_0 ; } rtb_B_390_3422_0 = _rtP -> P_2734 * muDoubleScalarAtan2 (
_rtB -> B_390_3757_0 , _rtB -> B_390_3749_0 ) * _rtP -> P_2735 ; B_390_3762_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3749_0 , _rtB -> B_390_3757_0 ) *
muDoubleScalarCos ( rtb_B_390_3422_0 ) ; B_390_3762_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3749_0 , _rtB -> B_390_3757_0 ) *
muDoubleScalarSin ( rtb_B_390_3422_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_3764_0 = _rtB -> B_390_3763_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3764_0 ,
B_390_3720_0 , B_390_3741_0 , B_390_3762_0 , & _rtB -> PosSeqComputation_j ,
& _rtDW -> PosSeqComputation_j , & _rtP -> PosSeqComputation_j ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3767_0 = _rtB
-> B_390_3766_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_3767_0 , B_390_3720_0 , B_390_3741_0 , B_390_3762_0 , & _rtB ->
NegSeqComputation_oj , & _rtDW -> NegSeqComputation_oj , & _rtP ->
NegSeqComputation_oj ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3770_0 = _rtB -> B_390_3769_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_3770_0 ,
B_390_3720_0 , B_390_3741_0 , B_390_3762_0 , & _rtB -> ZeroSeqComputation_of
, & _rtDW -> ZeroSeqComputation_of , & _rtP -> ZeroSeqComputation_of ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_j .
B_22_2_0 . im , _rtB -> PosSeqComputation_j . B_22_2_0 . re ) ; _rtB ->
B_390_3773_0 = _rtP -> P_2739 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_j . B_22_2_0 . re , _rtB -> PosSeqComputation_j . B_22_2_0
. im ) ; ssCallAccelRunBlock ( S , 390 , 3774 , SS_CALL_MDL_OUTPUTS ) ; _rtB
-> B_390_1508_0 [ 0 ] = _rtB -> B_390_3624_0 ; _rtB -> B_390_1508_0 [ 1 ] =
_rtB -> B_390_3698_0 ; _rtB -> B_390_1508_0 [ 2 ] = _rtB -> B_390_3699_0 ;
_rtB -> B_390_1508_0 [ 3 ] = _rtB -> B_390_3773_0 ; ssCallAccelRunBlock ( S ,
390 , 3776 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_390_3780_0 [ 0 ] = _rtP -> P_2740 * _rtB ->
B_0_14_0 [ 123 ] * _rtP -> P_2743 ; _rtB -> B_390_3780_0 [ 1 ] = _rtP ->
P_2741 * _rtB -> B_0_14_0 [ 124 ] * _rtP -> P_2743 ; _rtB -> B_390_3780_0 [ 2
] = _rtP -> P_2742 * _rtB -> B_0_14_0 [ 125 ] * _rtP -> P_2743 ; _rtB ->
B_390_3784_0 [ 0 ] = _rtP -> P_2744 * _rtB -> B_0_14_0 [ 68 ] * _rtP ->
P_2747 ; _rtB -> B_390_3784_0 [ 1 ] = _rtP -> P_2745 * _rtB -> B_0_14_0 [ 69
] * _rtP -> P_2747 ; _rtB -> B_390_3784_0 [ 2 ] = _rtP -> P_2746 * _rtB ->
B_0_14_0 [ 70 ] * _rtP -> P_2747 ; } _rtB -> B_390_3820_0 = (
muDoubleScalarSin ( _rtP -> P_2750 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2751
) * _rtP -> P_2748 + _rtP -> P_2749 ) * _rtB -> B_390_3780_0 [ 0 ] ; _rtB ->
B_390_3822_0 = ( muDoubleScalarSin ( _rtP -> P_2754 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2755 ) * _rtP -> P_2752 + _rtP -> P_2753 ) * _rtB -> B_390_3780_0
[ 0 ] ; _rtB -> B_390_3824_0 = ( muDoubleScalarSin ( _rtP -> P_2758 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2759 ) * _rtP -> P_2756 + _rtP -> P_2757
) * _rtB -> B_390_3780_0 [ 1 ] ; _rtB -> B_390_3826_0 = ( muDoubleScalarSin (
_rtP -> P_2762 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2763 ) * _rtP -> P_2760
+ _rtP -> P_2761 ) * _rtB -> B_390_3780_0 [ 1 ] ; _rtB -> B_390_3828_0 = (
muDoubleScalarSin ( _rtP -> P_2766 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2767
) * _rtP -> P_2764 + _rtP -> P_2765 ) * _rtB -> B_390_3780_0 [ 2 ] ; _rtB ->
B_390_3830_0 = ( muDoubleScalarSin ( _rtP -> P_2770 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2771 ) * _rtP -> P_2768 + _rtP -> P_2769 ) * _rtB -> B_390_3780_0
[ 2 ] ; B_390_3831_0 = _rtP -> P_2772 * B_390_401_1_idx_0 ; _rtB ->
B_390_3833_0 = ( muDoubleScalarSin ( _rtP -> P_2775 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2776 ) * _rtP -> P_2773 + _rtP -> P_2774 ) * _rtB -> B_390_3784_0
[ 0 ] ; _rtB -> B_390_3835_0 = ( muDoubleScalarSin ( _rtP -> P_2779 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2780 ) * _rtP -> P_2777 + _rtP -> P_2778
) * _rtB -> B_390_3784_0 [ 0 ] ; _rtB -> B_390_3837_0 = ( muDoubleScalarSin (
_rtP -> P_2783 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2784 ) * _rtP -> P_2781
+ _rtP -> P_2782 ) * _rtB -> B_390_3784_0 [ 1 ] ; _rtB -> B_390_3839_0 = (
muDoubleScalarSin ( _rtP -> P_2787 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2788
) * _rtP -> P_2785 + _rtP -> P_2786 ) * _rtB -> B_390_3784_0 [ 1 ] ; _rtB ->
B_390_3841_0 = ( muDoubleScalarSin ( _rtP -> P_2791 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2792 ) * _rtP -> P_2789 + _rtP -> P_2790 ) * _rtB -> B_390_3784_0
[ 2 ] ; _rtB -> B_390_3843_0 = ( muDoubleScalarSin ( _rtP -> P_2795 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2796 ) * _rtP -> P_2793 + _rtP -> P_2794
) * _rtB -> B_390_3784_0 [ 2 ] ; _rtB -> B_390_3845_0 = ( muDoubleScalarSin (
_rtP -> P_2799 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2800 ) * _rtP -> P_2797
+ _rtP -> P_2798 ) * _rtB -> B_390_3780_0 [ 0 ] ; _rtB -> B_390_3847_0 = (
muDoubleScalarSin ( _rtP -> P_2803 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2804
) * _rtP -> P_2801 + _rtP -> P_2802 ) * _rtB -> B_390_3780_0 [ 0 ] ; _rtB ->
B_390_3849_0 = ( muDoubleScalarSin ( _rtP -> P_2807 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2808 ) * _rtP -> P_2805 + _rtP -> P_2806 ) * _rtB -> B_390_3780_0
[ 1 ] ; _rtB -> B_390_3851_0 = ( muDoubleScalarSin ( _rtP -> P_2811 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2812 ) * _rtP -> P_2809 + _rtP -> P_2810
) * _rtB -> B_390_3780_0 [ 1 ] ; _rtB -> B_390_3853_0 = ( muDoubleScalarSin (
_rtP -> P_2815 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2816 ) * _rtP -> P_2813
+ _rtP -> P_2814 ) * _rtB -> B_390_3780_0 [ 2 ] ; _rtB -> B_390_3855_0 = (
muDoubleScalarSin ( _rtP -> P_2819 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2820
) * _rtP -> P_2817 + _rtP -> P_2818 ) * _rtB -> B_390_3780_0 [ 2 ] ; _rtB ->
B_390_3857_0 = ( muDoubleScalarSin ( _rtP -> P_2823 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2824 ) * _rtP -> P_2821 + _rtP -> P_2822 ) * _rtB -> B_390_3784_0
[ 0 ] ; _rtB -> B_390_3859_0 = ( muDoubleScalarSin ( _rtP -> P_2827 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2828 ) * _rtP -> P_2825 + _rtP -> P_2826
) * _rtB -> B_390_3784_0 [ 0 ] ; _rtB -> B_390_3861_0 = ( muDoubleScalarSin (
_rtP -> P_2831 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2832 ) * _rtP -> P_2829
+ _rtP -> P_2830 ) * _rtB -> B_390_3784_0 [ 1 ] ; _rtB -> B_390_3863_0 = (
muDoubleScalarSin ( _rtP -> P_2835 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2836
) * _rtP -> P_2833 + _rtP -> P_2834 ) * _rtB -> B_390_3784_0 [ 1 ] ; _rtB ->
B_390_3865_0 = ( muDoubleScalarSin ( _rtP -> P_2839 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2840 ) * _rtP -> P_2837 + _rtP -> P_2838 ) * _rtB -> B_390_3784_0
[ 2 ] ; _rtB -> B_390_3867_0 = ( muDoubleScalarSin ( _rtP -> P_2843 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2844 ) * _rtP -> P_2841 + _rtP -> P_2842
) * _rtB -> B_390_3784_0 [ 2 ] ; B_390_3868_0 = _rtP -> P_2845 *
rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { if
( _rtB -> B_390_3871_0 >= _rtP -> P_2851 ) { rtb_B_390_3874_0 = _rtB ->
B_390_3870_0 ; } else { rtb_B_390_3874_0 = rt_Lookup ( _rtP -> P_2849 , 6 ,
ssGetTaskTime ( S , 2 ) , _rtP -> P_2850 ) ; } if ( _rtB -> B_390_3875_0 >=
_rtP -> P_2854 ) { _rtB -> B_390_3878_0 = rtb_B_390_3874_0 ; } else { _rtB ->
B_390_3878_0 = _rtB -> B_390_3876_0 ; } } _rtB -> B_390_3880_0 = rt_Lookup (
_rtP -> P_2855 , 5 , ssGetT ( S ) , _rtP -> P_2856 ) ; if ( _rtB ->
B_390_3869_0 >= _rtP -> P_2857 ) { _rtB -> B_390_3881_0 = _rtB ->
B_390_3878_0 ; } else { _rtB -> B_390_3881_0 = _rtB -> B_390_3880_0 ; } if (
( _rtDW -> TimeStampA_mu >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB_c >=
ssGetT ( S ) ) ) { _rtB -> B_390_3882_0 = 0.0 ; } else { rtb_B_390_422_0 =
_rtDW -> TimeStampA_mu ; lastU = & _rtDW -> LastUAtTimeA_g4 ; if ( _rtDW ->
TimeStampA_mu < _rtDW -> TimeStampB_c ) { if ( _rtDW -> TimeStampB_c < ssGetT
( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_c ; lastU = & _rtDW ->
LastUAtTimeB_e5 ; } } else { if ( _rtDW -> TimeStampA_mu >= ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_c ; lastU = & _rtDW -> LastUAtTimeB_e5
; } } _rtB -> B_390_3882_0 = ( _rtB -> B_390_3880_0 - * lastU ) / ( ssGetT (
S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit
!= 0 ) { if ( _rtB -> B_390_3888_0 >= _rtP -> P_2861 ) { _rtB -> B_390_3890_0
= rtb_B_390_3874_0 ; } else { _rtB -> B_390_3890_0 = _rtB -> B_390_3876_0 ; }
} _rtB -> B_390_3892_0 = rt_Lookup ( _rtP -> P_2862 , 5 , ssGetT ( S ) , _rtP
-> P_2863 ) ; if ( _rtB -> B_390_3887_0 >= _rtP -> P_2864 ) { _rtB ->
B_390_3893_0 = _rtB -> B_390_3890_0 ; } else { _rtB -> B_390_3893_0 = _rtB ->
B_390_3892_0 ; } if ( ( _rtDW -> TimeStampA_k >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_p >= ssGetT ( S ) ) ) { _rtB -> B_390_3894_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_k ; lastU = & _rtDW -> LastUAtTimeA_pj
; if ( _rtDW -> TimeStampA_k < _rtDW -> TimeStampB_p ) { if ( _rtDW ->
TimeStampB_p < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_p ;
lastU = & _rtDW -> LastUAtTimeB_o ; } } else { if ( _rtDW -> TimeStampA_k >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_p ; lastU = & _rtDW ->
LastUAtTimeB_o ; } } _rtB -> B_390_3894_0 = ( _rtB -> B_390_3892_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_390_3900_0 >= _rtP -> P_2868 ) { _rtB
-> B_390_3902_0 = rtb_B_390_3874_0 ; } else { _rtB -> B_390_3902_0 = _rtB ->
B_390_3876_0 ; } } _rtB -> B_390_3904_0 = rt_Lookup ( _rtP -> P_2869 , 5 ,
ssGetT ( S ) , _rtP -> P_2870 ) ; if ( _rtB -> B_390_3899_0 >= _rtP -> P_2871
) { _rtB -> B_390_3905_0 = _rtB -> B_390_3902_0 ; } else { _rtB ->
B_390_3905_0 = _rtB -> B_390_3904_0 ; } if ( ( _rtDW -> TimeStampA_p >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ne >= ssGetT ( S ) ) ) { _rtB ->
B_390_3906_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_p ; lastU
= & _rtDW -> LastUAtTimeA_o ; if ( _rtDW -> TimeStampA_p < _rtDW ->
TimeStampB_ne ) { if ( _rtDW -> TimeStampB_ne < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_ne ; lastU = & _rtDW -> LastUAtTimeB_ou
; } } else { if ( _rtDW -> TimeStampA_p >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_ne ; lastU = & _rtDW -> LastUAtTimeB_ou ; } } _rtB ->
B_390_3906_0 = ( _rtB -> B_390_3904_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } _rtB -> B_390_3923_0 = 0.0 ; _rtB -> B_390_3923_0 +=
_rtP -> P_2874 * _rtX -> LPF1_CSTATE_j ; _rtB -> B_390_3924_0 = 0.0 ; _rtB ->
B_390_3924_0 += _rtP -> P_2876 * _rtX -> LPF2_CSTATE_m5 ; _rtB ->
B_390_3925_0 = _rtX -> integrator_CSTATE_fn ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_j4 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j4 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_2878 ; B_390_3926_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ogq
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_ogq . Last , _rtDW ->
TransportDelay_IWORK_ogq . Tail , _rtDW -> TransportDelay_IWORK_ogq . Head ,
_rtP -> P_2879 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3928_0 = _rtP -> P_2880 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3928_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3930_0 =
_rtDW -> Memory_PreviousInput_csp ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3932_0 = ( _rtB -> B_390_3925_0 - B_390_3926_0 ) * _rtP -> P_341 ; }
else { _rtB -> B_390_3932_0 = _rtB -> B_390_3930_0 ; } _rtB -> B_390_3933_0 =
_rtX -> integrator_CSTATE_ox ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_hl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hl . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2883 ;
B_390_3934_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nn .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nn . Last , _rtDW ->
TransportDelay_IWORK_nn . Tail , _rtDW -> TransportDelay_IWORK_nn . Head ,
_rtP -> P_2884 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3936_0 = _rtP -> P_2885 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3936_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3938_0 =
_rtDW -> Memory_PreviousInput_i1 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3940_0 = ( _rtB -> B_390_3933_0 - B_390_3934_0 ) * _rtP -> P_340 ; }
else { _rtB -> B_390_3940_0 = _rtB -> B_390_3938_0 ; } rtb_B_390_3874_0 =
_rtP -> P_2887 * muDoubleScalarAtan2 ( _rtB -> B_390_3940_0 , _rtB ->
B_390_3932_0 ) * _rtP -> P_2888 ; B_390_3945_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3932_0 , _rtB -> B_390_3940_0 ) * muDoubleScalarCos (
rtb_B_390_3874_0 ) ; B_390_3945_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3932_0 , _rtB -> B_390_3940_0 ) * muDoubleScalarSin ( rtb_B_390_3874_0
) ; _rtB -> B_390_3946_0 = _rtX -> integrator_CSTATE_clt ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2890 ; B_390_3947_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_pik . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pik . Last , _rtDW -> TransportDelay_IWORK_pik . Tail ,
_rtDW -> TransportDelay_IWORK_pik . Head , _rtP -> P_2891 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3949_0 = _rtP -> P_2892 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3949_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3951_0 = _rtDW -> Memory_PreviousInput_a0q ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3953_0 = ( _rtB -> B_390_3946_0 -
B_390_3947_0 ) * _rtP -> P_343 ; } else { _rtB -> B_390_3953_0 = _rtB ->
B_390_3951_0 ; } _rtB -> B_390_3954_0 = _rtX -> integrator_CSTATE_bo ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ndm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2895 ; B_390_3955_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_k1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k1 . Last , _rtDW -> TransportDelay_IWORK_k1 . Tail ,
_rtDW -> TransportDelay_IWORK_k1 . Head , _rtP -> P_2896 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3957_0 = _rtP -> P_2897 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_3957_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_3959_0 = _rtDW -> Memory_PreviousInput_dz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_3961_0 = ( _rtB -> B_390_3954_0 -
B_390_3955_0 ) * _rtP -> P_342 ; } else { _rtB -> B_390_3961_0 = _rtB ->
B_390_3959_0 ; } rtb_B_390_3874_0 = _rtP -> P_2899 * muDoubleScalarAtan2 (
_rtB -> B_390_3961_0 , _rtB -> B_390_3953_0 ) * _rtP -> P_2900 ; B_390_3966_0
. re = muDoubleScalarHypot ( _rtB -> B_390_3953_0 , _rtB -> B_390_3961_0 ) *
muDoubleScalarCos ( rtb_B_390_3874_0 ) ; B_390_3966_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_3953_0 , _rtB -> B_390_3961_0 ) *
muDoubleScalarSin ( rtb_B_390_3874_0 ) ; _rtB -> B_390_3967_0 = _rtX ->
integrator_CSTATE_jm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f0i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_f0i . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2902 ;
B_390_3968_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mn .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_mn . Last , _rtDW ->
TransportDelay_IWORK_mn . Tail , _rtDW -> TransportDelay_IWORK_mn . Head ,
_rtP -> P_2903 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3970_0 = _rtP -> P_2904 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3970_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3972_0 =
_rtDW -> Memory_PreviousInput_kn ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3974_0 = ( _rtB -> B_390_3967_0 - B_390_3968_0 ) * _rtP -> P_345 ; }
else { _rtB -> B_390_3974_0 = _rtB -> B_390_3972_0 ; } _rtB -> B_390_3975_0 =
_rtX -> integrator_CSTATE_gv ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ge . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ge . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2907 ;
B_390_3976_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pl . Last , _rtDW ->
TransportDelay_IWORK_pl . Tail , _rtDW -> TransportDelay_IWORK_pl . Head ,
_rtP -> P_2908 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3978_0 = _rtP -> P_2909 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_3978_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3980_0 =
_rtDW -> Memory_PreviousInput_iq ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_3982_0 = ( _rtB -> B_390_3975_0 - B_390_3976_0 ) * _rtP -> P_344 ; }
else { _rtB -> B_390_3982_0 = _rtB -> B_390_3980_0 ; } rtb_B_390_3874_0 =
_rtP -> P_2911 * muDoubleScalarAtan2 ( _rtB -> B_390_3982_0 , _rtB ->
B_390_3974_0 ) * _rtP -> P_2912 ; B_390_3987_0 . re = muDoubleScalarHypot (
_rtB -> B_390_3974_0 , _rtB -> B_390_3982_0 ) * muDoubleScalarCos (
rtb_B_390_3874_0 ) ; B_390_3987_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_3974_0 , _rtB -> B_390_3982_0 ) * muDoubleScalarSin ( rtb_B_390_3874_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_3989_0 = _rtB -> B_390_3988_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_3989_0 , B_390_3945_0 , B_390_3966_0 , B_390_3987_0 , &
_rtB -> PosSeqComputation_n1 , & _rtDW -> PosSeqComputation_n1 , & _rtP ->
PosSeqComputation_n1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_3992_0 = _rtB -> B_390_3991_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_3992_0 ,
B_390_3945_0 , B_390_3966_0 , B_390_3987_0 , & _rtB -> NegSeqComputation_fo ,
& _rtDW -> NegSeqComputation_fo , & _rtP -> NegSeqComputation_fo ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_3995_0 = _rtB
-> B_390_3994_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_3995_0 , B_390_3945_0 , B_390_3966_0 , B_390_3987_0 , & _rtB ->
ZeroSeqComputation_jr , & _rtDW -> ZeroSeqComputation_jr , & _rtP ->
ZeroSeqComputation_jr ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_n1 . B_22_2_0 . im , _rtB -> PosSeqComputation_n1 .
B_22_2_0 . re ) ; _rtB -> B_390_3998_0 = _rtP -> P_2916 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_n1 . B_22_2_0 . re , _rtB -> PosSeqComputation_n1
. B_22_2_0 . im ) ; _rtB -> B_390_3999_0 = _rtX -> integrator_CSTATE_cr ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e1q . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2918 ; B_390_4000_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_jj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jj . Last , _rtDW -> TransportDelay_IWORK_jj . Tail ,
_rtDW -> TransportDelay_IWORK_jj . Head , _rtP -> P_2919 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4002_0 = _rtP -> P_2920 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4002_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4004_0 = _rtDW -> Memory_PreviousInput_kz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4006_0 = ( _rtB -> B_390_3999_0 -
B_390_4000_0 ) * _rtP -> P_347 ; } else { _rtB -> B_390_4006_0 = _rtB ->
B_390_4004_0 ; } _rtB -> B_390_4007_0 = _rtX -> integrator_CSTATE_gm ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b2 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2923 ; B_390_4008_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_dr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dr . Last , _rtDW -> TransportDelay_IWORK_dr . Tail ,
_rtDW -> TransportDelay_IWORK_dr . Head , _rtP -> P_2924 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4010_0 = _rtP -> P_2925 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4010_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4012_0 = _rtDW -> Memory_PreviousInput_lp ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4014_0 = ( _rtB -> B_390_4007_0 -
B_390_4008_0 ) * _rtP -> P_346 ; } else { _rtB -> B_390_4014_0 = _rtB ->
B_390_4012_0 ; } rtb_B_390_3874_0 = _rtP -> P_2927 * muDoubleScalarAtan2 (
_rtB -> B_390_4014_0 , _rtB -> B_390_4006_0 ) * _rtP -> P_2928 ; B_390_4019_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4006_0 , _rtB -> B_390_4014_0 ) *
muDoubleScalarCos ( rtb_B_390_3874_0 ) ; B_390_4019_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4006_0 , _rtB -> B_390_4014_0 ) *
muDoubleScalarSin ( rtb_B_390_3874_0 ) ; _rtB -> B_390_4020_0 = _rtX ->
integrator_CSTATE_l3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ma . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ma . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2930 ;
B_390_4021_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a3y .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_a3y . Last , _rtDW ->
TransportDelay_IWORK_a3y . Tail , _rtDW -> TransportDelay_IWORK_a3y . Head ,
_rtP -> P_2931 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4023_0 = _rtP -> P_2932 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4023_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4025_0 =
_rtDW -> Memory_PreviousInput_ae ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4027_0 = ( _rtB -> B_390_4020_0 - B_390_4021_0 ) * _rtP -> P_349 ; }
else { _rtB -> B_390_4027_0 = _rtB -> B_390_4025_0 ; } _rtB -> B_390_4028_0 =
_rtX -> integrator_CSTATE_hm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_b5s . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2935
; B_390_4029_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dm . Last , _rtDW ->
TransportDelay_IWORK_dm . Tail , _rtDW -> TransportDelay_IWORK_dm . Head ,
_rtP -> P_2936 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4031_0 = _rtP -> P_2937 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4031_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4033_0 =
_rtDW -> Memory_PreviousInput_nrz ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4035_0 = ( _rtB -> B_390_4028_0 - B_390_4029_0 ) * _rtP -> P_348 ; }
else { _rtB -> B_390_4035_0 = _rtB -> B_390_4033_0 ; } rtb_B_390_3874_0 =
_rtP -> P_2939 * muDoubleScalarAtan2 ( _rtB -> B_390_4035_0 , _rtB ->
B_390_4027_0 ) * _rtP -> P_2940 ; B_390_4040_0 . re = muDoubleScalarHypot (
_rtB -> B_390_4027_0 , _rtB -> B_390_4035_0 ) * muDoubleScalarCos (
rtb_B_390_3874_0 ) ; B_390_4040_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_4027_0 , _rtB -> B_390_4035_0 ) * muDoubleScalarSin ( rtb_B_390_3874_0
) ; _rtB -> B_390_4041_0 = _rtX -> integrator_CSTATE_co ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2942 ; B_390_4042_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_pdg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pdg . Last , _rtDW -> TransportDelay_IWORK_pdg . Tail ,
_rtDW -> TransportDelay_IWORK_pdg . Head , _rtP -> P_2943 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4044_0 = _rtP -> P_2944 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4044_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4046_0 = _rtDW -> Memory_PreviousInput_jd4 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4048_0 = ( _rtB -> B_390_4041_0 -
B_390_4042_0 ) * _rtP -> P_351 ; } else { _rtB -> B_390_4048_0 = _rtB ->
B_390_4046_0 ; } _rtB -> B_390_4049_0 = _rtX -> integrator_CSTATE_knp ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2947 ; B_390_4050_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ny . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ny . Last , _rtDW -> TransportDelay_IWORK_ny . Tail ,
_rtDW -> TransportDelay_IWORK_ny . Head , _rtP -> P_2948 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4052_0 = _rtP -> P_2949 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4052_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4054_0 = _rtDW -> Memory_PreviousInput_o2n ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4056_0 = ( _rtB -> B_390_4049_0 -
B_390_4050_0 ) * _rtP -> P_350 ; } else { _rtB -> B_390_4056_0 = _rtB ->
B_390_4054_0 ; } rtb_B_390_3874_0 = _rtP -> P_2951 * muDoubleScalarAtan2 (
_rtB -> B_390_4056_0 , _rtB -> B_390_4048_0 ) * _rtP -> P_2952 ; B_390_4061_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4048_0 , _rtB -> B_390_4056_0 ) *
muDoubleScalarCos ( rtb_B_390_3874_0 ) ; B_390_4061_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4048_0 , _rtB -> B_390_4056_0 ) *
muDoubleScalarSin ( rtb_B_390_3874_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_4063_0 = _rtB -> B_390_4062_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_4063_0 ,
B_390_4019_0 , B_390_4040_0 , B_390_4061_0 , & _rtB -> PosSeqComputation_b ,
& _rtDW -> PosSeqComputation_b , & _rtP -> PosSeqComputation_b ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4066_0 = _rtB
-> B_390_4065_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_4066_0 , B_390_4019_0 , B_390_4040_0 , B_390_4061_0 , & _rtB ->
NegSeqComputation_f5 , & _rtDW -> NegSeqComputation_f5 , & _rtP ->
NegSeqComputation_f5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_4069_0 = _rtB -> B_390_4068_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_4069_0 ,
B_390_4019_0 , B_390_4040_0 , B_390_4061_0 , & _rtB -> ZeroSeqComputation_dy
, & _rtDW -> ZeroSeqComputation_dy , & _rtP -> ZeroSeqComputation_dy ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_b .
B_22_2_0 . im , _rtB -> PosSeqComputation_b . B_22_2_0 . re ) ; _rtB ->
B_390_4072_0 = _rtP -> P_2956 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_b . B_22_2_0 . re , _rtB -> PosSeqComputation_b . B_22_2_0
. im ) ; ssCallAccelRunBlock ( S , 390 , 4074 , SS_CALL_MDL_OUTPUTS ) ; isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4075_0 =
_rtP -> P_2958 * _rtB -> B_0_14_0 [ 71 ] ; } _rtB -> B_390_4076_0 [ 0 ] =
_rtB -> B_390_3923_0 ; _rtB -> B_390_4076_0 [ 1 ] = _rtB -> B_390_3924_0 ;
_rtB -> B_390_4076_0 [ 2 ] = _rtB -> B_390_3998_0 ; _rtB -> B_390_4076_0 [ 3
] = _rtB -> B_390_4072_0 ; _rtB -> B_390_4076_0 [ 4 ] = _rtB -> B_390_4075_0
; ssCallAccelRunBlock ( S , 390 , 4077 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_2960 *
_rtB -> B_0_14_0 [ 131 ] * _rtP -> P_2963 ; tmp_1 = _rtP -> P_2961 * _rtB ->
B_0_14_0 [ 132 ] * _rtP -> P_2963 ; rtb_B_390_16_0 = _rtP -> P_2962 * _rtB ->
B_0_14_0 [ 133 ] * _rtP -> P_2963 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_2964 [ isHit + 6 ] * rtb_B_390_16_0 + ( _rtP ->
P_2964 [ isHit + 3 ] * tmp_1 + _rtP -> P_2964 [ isHit ] * tmp_0 ) ; }
B_390_4084_0 [ 0 ] = _rtP -> P_2965 * tmp [ 0 ] ; B_390_4084_0 [ 1 ] = _rtP
-> P_2965 * tmp [ 1 ] ; B_390_4084_0 [ 2 ] = _rtP -> P_2965 * tmp [ 2 ] ;
_rtB -> B_390_4088_0 = _rtB -> B_390_4087_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b
( _rtB -> B_390_4088_0 , & B_390_4084_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB
-> Subsystem1_k0 , & _rtDW -> Subsystem1_k0 ) ; _rtB -> B_390_4092_0 = _rtB
-> B_390_4091_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB ->
B_390_4092_0 , & B_390_4084_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB ->
Subsystempi2delay_n , & _rtDW -> Subsystempi2delay_n ) ; if ( _rtB ->
B_390_4087_0 != 0 ) { _rtB -> B_390_4094_0 [ 0 ] = _rtB -> Subsystem1_k0 .
B_10_0_0 ; _rtB -> B_390_4094_0 [ 1 ] = _rtB -> Subsystem1_k0 . B_10_1_0 ; }
else { _rtB -> B_390_4094_0 [ 0 ] = _rtB -> Subsystempi2delay_n . B_9_0_0 ;
_rtB -> B_390_4094_0 [ 1 ] = _rtB -> Subsystempi2delay_n . B_9_1_0 ; } } _rtB
-> B_390_4095_0 = 0.0 ; _rtB -> B_390_4095_0 += _rtP -> P_2970 * _rtX ->
Integradord_CSTATE_g ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_4098_0 = ( _rtB -> B_390_4096_0 - _rtB -> B_390_4075_0 ) *
_rtP -> P_2972 ; _rtB -> B_390_4099_0 = _rtP -> P_2973 * _rtB -> B_390_4098_0
; } _rtB -> B_390_4100_0 = _rtB -> B_390_4095_0 + _rtB -> B_390_4099_0 ; _rtB
-> B_390_4102_0 = _rtB -> B_390_4101_0 - _rtB -> B_390_3924_0 ; _rtB ->
B_390_4103_0 = _rtP -> P_2975 * _rtB -> B_390_4102_0 ; _rtB -> B_390_4104_0 =
_rtP -> P_2976 * _rtB -> B_390_4103_0 ; _rtB -> B_390_4105_0 = 0.0 ; _rtB ->
B_390_4105_0 += _rtP -> P_2978 * _rtX -> Integradord1_CSTATE_m ; _rtB ->
B_390_4106_0 = _rtB -> B_390_4104_0 + _rtB -> B_390_4105_0 ; _rtB ->
B_390_4107_0 . re = _rtB -> B_390_4100_0 ; _rtB -> B_390_4107_0 . im = _rtB
-> B_390_4106_0 ; _rtB -> B_390_4108_0 = muDoubleScalarHypot ( _rtB ->
B_390_4107_0 . re , _rtB -> B_390_4107_0 . im ) ; rtb_B_390_3874_0 =
muDoubleScalarAtan2 ( _rtB -> B_390_4107_0 . im , _rtB -> B_390_4107_0 . re )
; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Saturation_MODE_p0 = _rtB ->
B_390_4108_0 >= _rtP -> P_2979 ? 1 : _rtB -> B_390_4108_0 > _rtP -> P_2980 ?
0 : - 1 ; } _rtB -> B_390_4109_0 = _rtDW -> Saturation_MODE_p0 == 1 ? _rtP ->
P_2979 : _rtDW -> Saturation_MODE_p0 == - 1 ? _rtP -> P_2980 : _rtB ->
B_390_4108_0 ; _rtB -> B_390_4111_0 = _rtB -> B_390_4109_0 *
muDoubleScalarCos ( rtb_B_390_3874_0 ) ; _rtB -> B_390_4111_1 = _rtB ->
B_390_4109_0 * muDoubleScalarSin ( rtb_B_390_3874_0 ) ; _rtB -> B_390_4112_0
= _rtB -> B_390_4111_0 - _rtB -> B_390_4094_0 [ 0 ] ; _rtB -> B_390_4113_0 =
0.0 ; _rtB -> B_390_4113_0 += _rtP -> P_2982 * _rtX -> Controled_CSTATE ;
_rtB -> B_390_4113_0 += _rtP -> P_2983 * _rtB -> B_390_4112_0 ; _rtB ->
B_390_4114_0 = _rtB -> B_390_4111_1 - _rtB -> B_390_4094_0 [ 1 ] ; _rtB ->
B_390_4115_0 = 0.0 ; _rtB -> B_390_4115_0 += _rtP -> P_2985 * _rtX ->
Controleq_CSTATE ; _rtB -> B_390_4115_0 += _rtP -> P_2986 * _rtB ->
B_390_4114_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_390_4119_0 [ 0 ] = _rtP -> P_2987 * _rtB -> B_0_14_0 [ 79 ] * _rtP ->
P_2990 ; _rtB -> B_390_4119_0 [ 1 ] = _rtP -> P_2988 * _rtB -> B_0_14_0 [ 80
] * _rtP -> P_2990 ; _rtB -> B_390_4119_0 [ 2 ] = _rtP -> P_2989 * _rtB ->
B_0_14_0 [ 81 ] * _rtP -> P_2990 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_2991 [ isHit + 6 ] * _rtB -> B_390_4119_0 [ 2 ] + (
_rtP -> P_2991 [ isHit + 3 ] * _rtB -> B_390_4119_0 [ 1 ] + _rtP -> P_2991 [
isHit ] * _rtB -> B_390_4119_0 [ 0 ] ) ; } B_390_4121_0 [ 0 ] = _rtP ->
P_2992 * tmp [ 0 ] ; B_390_4121_0 [ 1 ] = _rtP -> P_2992 * tmp [ 1 ] ;
B_390_4121_0 [ 2 ] = _rtP -> P_2992 * tmp [ 2 ] ; _rtB -> B_390_4125_0 = _rtB
-> B_390_4124_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_390_4125_0 , &
B_390_4121_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB -> Subsystem1_m2 , & _rtDW
-> Subsystem1_m2 ) ; _rtB -> B_390_4129_0 = _rtB -> B_390_4128_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4129_0 , &
B_390_4121_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB -> Subsystempi2delay_cj , &
_rtDW -> Subsystempi2delay_cj ) ; if ( _rtB -> B_390_4124_0 != 0 ) { _rtB ->
B_390_4131_0 [ 0 ] = _rtB -> Subsystem1_m2 . B_10_0_0 ; _rtB -> B_390_4131_0
[ 1 ] = _rtB -> Subsystem1_m2 . B_10_1_0 ; } else { _rtB -> B_390_4131_0 [ 0
] = _rtB -> Subsystempi2delay_cj . B_9_0_0 ; _rtB -> B_390_4131_0 [ 1 ] =
_rtB -> Subsystempi2delay_cj . B_9_1_0 ; } } isHit = ssIsSampleHit ( S , 3 ,
0 ) ; if ( isHit != 0 ) { rtb_B_390_4132_0 = _rtDW -> UnitDelay_DSTATE_c ;
_rtB -> B_390_4133_0 = _rtP -> P_2997 * _rtDW -> UnitDelay_DSTATE_c ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4135_0 =
_rtB -> B_390_4133_0 * _rtB -> B_390_4094_0 [ 1 ] * _rtP -> P_2998 ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4137_0 =
_rtB -> B_390_4075_0 * _rtB -> B_390_4078_0 ; } _rtB -> B_390_4138_0 = ( (
_rtB -> B_390_4131_0 [ 0 ] + _rtB -> B_390_4113_0 ) - _rtB -> B_390_4135_0 )
/ _rtB -> B_390_4137_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_4140_0 = _rtB -> B_390_4094_0 [ 0 ] * _rtB ->
B_390_4133_0 * _rtP -> P_2999 ; } _rtB -> B_390_4142_0 = ( ( _rtB ->
B_390_4140_0 + _rtB -> B_390_4115_0 ) + _rtB -> B_390_4131_0 [ 1 ] ) * ( 1.0
/ _rtB -> B_390_4137_0 ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_k5 = _rtB -> B_390_4142_0 >= _rtP -> P_3000 ? 1 : _rtB ->
B_390_4142_0 > _rtP -> P_3001 ? 0 : - 1 ; _rtDW -> Saturation1_MODE = _rtB ->
B_390_4138_0 >= _rtP -> P_3002 ? 1 : _rtB -> B_390_4138_0 > _rtP -> P_3003 ?
0 : - 1 ; } _rtB -> B_390_4143_0 = _rtDW -> Saturation_MODE_k5 == 1 ? _rtP ->
P_3000 : _rtDW -> Saturation_MODE_k5 == - 1 ? _rtP -> P_3001 : _rtB ->
B_390_4142_0 ; _rtB -> B_390_4144_0 = _rtDW -> Saturation1_MODE == 1 ? _rtP
-> P_3002 : _rtDW -> Saturation1_MODE == - 1 ? _rtP -> P_3003 : _rtB ->
B_390_4138_0 ; _rtB -> B_390_4145_0 = _rtB -> B_390_4100_0 - _rtB ->
B_390_4111_0 ; _rtB -> B_390_4146_0 = _rtP -> P_3004 * _rtB -> B_390_4145_0 ;
_rtB -> B_390_4147_0 = _rtB -> B_390_4098_0 - _rtB -> B_390_4146_0 ; _rtB ->
B_390_4148_0 = _rtP -> P_3005 * _rtB -> B_390_4147_0 ; _rtB -> B_390_4149_0 =
_rtB -> B_390_4106_0 - _rtB -> B_390_4111_1 ; _rtB -> B_390_4150_0 = _rtP ->
P_3006 * _rtB -> B_390_4149_0 ; _rtB -> B_390_4151_0 = _rtB -> B_390_4103_0 -
_rtB -> B_390_4150_0 ; _rtB -> B_390_4152_0 = _rtP -> P_3007 * _rtB ->
B_390_4151_0 ; ssCallAccelRunBlock ( S , 390 , 4153 , SS_CALL_MDL_OUTPUTS ) ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_390_4158_0 = _rtP
-> P_3008 * _rtB -> B_0_14_0 [ 126 ] * _rtB -> B_390_4075_0 ; B_390_4170_0 [
0 ] = _rtP -> P_3009 * _rtB -> B_0_14_0 [ 76 ] * _rtP -> P_3012 ;
B_390_4170_0 [ 1 ] = _rtP -> P_3010 * _rtB -> B_0_14_0 [ 77 ] * _rtP ->
P_3012 ; B_390_4170_0 [ 2 ] = _rtP -> P_3011 * _rtB -> B_0_14_0 [ 78 ] * _rtP
-> P_3012 ; _rtB -> B_390_4213_0 = ( _rtB -> B_390_95_0 * _rtB -> B_0_14_0 [
152 ] - _rtB -> B_390_96_0 * _rtB -> B_0_14_0 [ 153 ] ) / ( ( _rtP -> P_3014
* _rtB -> B_390_92_0 - _rtDW -> prediction_DSTATE_la ) + _rtB -> B_390_4205_0
) ; _rtB -> B_390_4231_0 = ( _rtB -> B_390_4131_0 [ 0 ] * _rtB ->
B_390_4094_0 [ 0 ] + _rtB -> B_390_4131_0 [ 1 ] * _rtB -> B_390_4094_0 [ 1 ]
) * _rtP -> P_3016 ; _rtB -> B_390_4235_0 = ( _rtB -> B_390_4094_0 [ 0 ] *
_rtB -> B_390_4131_0 [ 1 ] - _rtB -> B_390_4131_0 [ 0 ] * _rtB ->
B_390_4094_0 [ 1 ] ) * _rtP -> P_3017 ; _rtB -> B_390_4241_0 = _rtB ->
B_390_4240_0 ; _rtB -> B_390_4242_0 = _rtB -> B_390_4238_0 ; _rtB ->
B_390_4246_0 [ 0 ] = _rtP -> P_3021 * _rtB -> B_0_14_0 [ 134 ] * _rtP ->
P_3024 ; _rtB -> B_390_4246_0 [ 1 ] = _rtP -> P_3022 * _rtB -> B_0_14_0 [ 135
] * _rtP -> P_3024 ; _rtB -> B_390_4246_0 [ 2 ] = _rtP -> P_3023 * _rtB ->
B_0_14_0 [ 136 ] * _rtP -> P_3024 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_3025 [ isHit + 6 ] * _rtB -> B_390_4246_0 [ 2 ] +
( _rtP -> P_3025 [ isHit + 3 ] * _rtB -> B_390_4246_0 [ 1 ] + _rtP -> P_3025
[ isHit ] * _rtB -> B_390_4246_0 [ 0 ] ) ; } B_390_4248_0 [ 0 ] = _rtP ->
P_3026 * tmp [ 0 ] ; B_390_4248_0 [ 1 ] = _rtP -> P_3026 * tmp [ 1 ] ;
B_390_4248_0 [ 2 ] = _rtP -> P_3026 * tmp [ 2 ] ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4241_0 , &
B_390_4248_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB -> Subsystempi2delay_j , &
_rtDW -> Subsystempi2delay_j ) ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB ->
B_390_4242_0 , & B_390_4248_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB ->
Subsystem1_i , & _rtDW -> Subsystem1_i ) ; if ( _rtB -> B_390_4238_0 != 0 ) {
B_390_4251_0 [ 0 ] = _rtB -> Subsystem1_i . B_10_0_0 ; B_390_4251_0 [ 1 ] =
_rtB -> Subsystem1_i . B_10_1_0 ; } else { B_390_4251_0 [ 0 ] = _rtB ->
Subsystempi2delay_j . B_9_0_0 ; B_390_4251_0 [ 1 ] = _rtB ->
Subsystempi2delay_j . B_9_1_0 ; } } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_390_4253_0 = _rtP -> P_3027 ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( _rtB -> B_390_4253_0 > 0.0 ) { if ( ! _rtDW ->
AutomaticGainControl_MODE_p ) { if ( ssGetTaskTime ( S , 3 ) != ssGetTStart (
S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
Integ4_SYSTEM_ENABLE_f = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_n = 1U ; _rtDW ->
AutomaticGainControl_MODE_p = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE_p ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> AutomaticGainControl_MODE_p = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_p ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_299 [ isHit + 6 ] * _rtB -> B_390_4119_0 [ 2 ] + ( _rtP -> P_299 [
isHit + 3 ] * _rtB -> B_390_4119_0 [ 1 ] + _rtP -> P_299 [ isHit ] * _rtB ->
B_390_4119_0 [ 0 ] ) ; } B_329_1_0 [ 0 ] = _rtP -> P_300 * tmp [ 0 ] ;
B_329_1_0 [ 1 ] = _rtP -> P_300 * tmp [ 1 ] ; B_329_1_0 [ 2 ] = _rtP -> P_300
* tmp [ 2 ] ; _rtB -> B_329_5_0 = _rtB -> B_329_4_0 ;
MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_329_5_0 , & B_329_1_0 [ 0 ] ,
_rtB -> B_390_79_0 , & _rtB -> Subsystem1_eh , & _rtDW -> Subsystem1_eh ) ;
_rtB -> B_329_9_0 = _rtB -> B_329_8_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_329_9_0 , & B_329_1_0 [ 0
] , _rtB -> B_390_79_0 , & _rtB -> Subsystempi2delay_a1 , & _rtDW ->
Subsystempi2delay_a1 ) ; if ( _rtB -> B_329_4_0 != 0 ) { _rtB -> B_329_11_0 [
0 ] = _rtB -> Subsystem1_eh . B_10_0_0 ; _rtB -> B_329_11_0 [ 1 ] = _rtB ->
Subsystem1_eh . B_10_1_0 ; } else { _rtB -> B_329_11_0 [ 0 ] = _rtB ->
Subsystempi2delay_a1 . B_9_0_0 ; _rtB -> B_329_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_a1 . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_f != 0
) { _rtB -> B_329_12_0 = _rtDW -> Integ4_DSTATE_kt ; } else { _rtB ->
B_329_12_0 = _rtP -> P_304 * _rtB -> B_329_11_0 [ 0 ] + _rtDW ->
Integ4_DSTATE_kt ; } if ( rtb_B_390_4132_0 > _rtP -> P_306 ) {
rtb_B_390_422_0 = _rtP -> P_306 ; } else if ( rtb_B_390_4132_0 < _rtP ->
P_307 ) { rtb_B_390_422_0 = _rtP -> P_307 ; } else { rtb_B_390_422_0 =
rtb_B_390_4132_0 ; } rtb_B_390_3874_0 = 1.0 / rtb_B_390_422_0 / 5.0e-5 ;
rtb_B_390_3422_0 = muDoubleScalarCeil ( rtb_B_390_3874_0 ) ; _rtB ->
B_329_16_0 = _rtP -> P_308 * rtb_B_390_3422_0 ; ssCallAccelRunBlock ( S , 329
, 17 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_329_20_0 ) { rtb_B_390_3422_0 = rtb_B_390_3874_0 - rtb_B_390_3422_0 ; _rtB
-> B_329_24_0 = ( ( _rtB -> B_329_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_da )
* rtb_B_390_3422_0 * _rtP -> P_296 + _rtB -> B_329_11_0 [ 0 ] ) * (
rtb_B_390_3422_0 / rtb_B_390_3874_0 ) + ( _rtB -> B_329_12_0 - _rtB ->
B_329_17_0 ) * rtb_B_390_4132_0 ; } else { _rtB -> B_329_24_0 = _rtDW ->
UnitDelay1_DSTATE_mz ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_n != 0 ) { _rtB
-> B_329_25_0 = _rtDW -> Integ4_DSTATE_h ; } else { _rtB -> B_329_25_0 = _rtP
-> P_320 * _rtB -> B_329_11_0 [ 1 ] + _rtDW -> Integ4_DSTATE_h ; } if (
rtb_B_390_4132_0 > _rtP -> P_322 ) { rtb_B_390_422_0 = _rtP -> P_322 ; } else
if ( rtb_B_390_4132_0 < _rtP -> P_323 ) { rtb_B_390_422_0 = _rtP -> P_323 ; }
else { rtb_B_390_422_0 = rtb_B_390_4132_0 ; } rtb_B_390_3874_0 = 1.0 /
rtb_B_390_422_0 / 5.0e-5 ; rtb_B_390_3422_0 = muDoubleScalarCeil (
rtb_B_390_3874_0 ) ; _rtB -> B_329_29_0 = _rtP -> P_324 * rtb_B_390_3422_0 ;
ssCallAccelRunBlock ( S , 329 , 30 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_329_33_0 ) { rtb_B_390_3422_0 =
rtb_B_390_3874_0 - rtb_B_390_3422_0 ; _rtB -> B_329_37_0 = ( ( _rtB ->
B_329_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_b ) * rtb_B_390_3422_0 * _rtP ->
P_297 + _rtB -> B_329_11_0 [ 1 ] ) * ( rtb_B_390_3422_0 / rtb_B_390_3874_0 )
+ ( _rtB -> B_329_25_0 - _rtB -> B_329_30_0 ) * rtb_B_390_4132_0 ; } else {
_rtB -> B_329_37_0 = _rtDW -> UnitDelay1_DSTATE_o ; } rtb_B_390_784_0 =
muDoubleScalarHypot ( _rtB -> B_329_24_0 , _rtB -> B_329_37_0 ) ; if (
rtb_B_390_784_0 > _rtP -> P_336 ) { rtb_B_390_784_0 = _rtP -> P_336 ; } else
{ if ( rtb_B_390_784_0 < _rtP -> P_337 ) { rtb_B_390_784_0 = _rtP -> P_337 ;
} } _rtB -> B_329_41_0 = 1.0 / rtb_B_390_784_0 ; B_329_42_0 = _rtP -> P_338 *
muDoubleScalarAtan2 ( _rtB -> B_329_37_0 , _rtB -> B_329_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_n ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_3028 [ isHit + 6 ] * _rtB ->
B_390_4119_0 [ 2 ] + ( _rtP -> P_3028 [ isHit + 3 ] * _rtB -> B_390_4119_0 [
1 ] + _rtP -> P_3028 [ isHit ] * _rtB -> B_390_4119_0 [ 0 ] ) ; }
B_390_4256_0 [ 0 ] = _rtP -> P_3029 * tmp [ 0 ] ; B_390_4256_0 [ 1 ] = _rtP
-> P_3029 * tmp [ 1 ] ; B_390_4256_0 [ 2 ] = _rtP -> P_3029 * tmp [ 2 ] ;
_rtB -> B_390_4260_0 = _rtB -> B_390_4259_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b
( _rtB -> B_390_4260_0 , & B_390_4256_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB
-> Subsystem1_hr , & _rtDW -> Subsystem1_hr ) ; _rtB -> B_390_4264_0 = _rtB
-> B_390_4263_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB ->
B_390_4264_0 , & B_390_4256_0 [ 0 ] , _rtB -> B_390_79_0 , & _rtB ->
Subsystempi2delay_o , & _rtDW -> Subsystempi2delay_o ) ; if ( _rtB ->
B_390_4259_0 != 0 ) { _rtB -> B_390_4266_0 [ 0 ] = _rtB -> Subsystem1_hr .
B_10_0_0 ; _rtB -> B_390_4266_0 [ 1 ] = _rtB -> Subsystem1_hr . B_10_1_0 ; }
else { _rtB -> B_390_4266_0 [ 0 ] = _rtB -> Subsystempi2delay_o . B_9_0_0 ;
_rtB -> B_390_4266_0 [ 1 ] = _rtB -> Subsystempi2delay_o . B_9_1_0 ; } if (
_rtDW -> Integ4_SYSTEM_ENABLE_l != 0 ) { _rtB -> B_390_4267_0 = _rtDW ->
Integ4_DSTATE_d ; } else { _rtB -> B_390_4267_0 = _rtP -> P_3033 * _rtB ->
B_390_4266_0 [ 1 ] + _rtDW -> Integ4_DSTATE_d ; } if ( rtb_B_390_4132_0 >
_rtP -> P_3035 ) { rtb_B_390_422_0 = _rtP -> P_3035 ; } else if (
rtb_B_390_4132_0 < _rtP -> P_3036 ) { rtb_B_390_422_0 = _rtP -> P_3036 ; }
else { rtb_B_390_422_0 = rtb_B_390_4132_0 ; } rtb_B_390_3874_0 = 1.0 /
rtb_B_390_422_0 / 5.0e-5 ; rtb_B_390_3422_0 = muDoubleScalarCeil (
rtb_B_390_3874_0 ) ; _rtB -> B_390_4271_0 = _rtP -> P_3037 * rtb_B_390_3422_0
; ssCallAccelRunBlock ( S , 390 , 4272 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_390_4275_0 ) { rtb_B_390_3422_0 =
rtb_B_390_3874_0 - rtb_B_390_3422_0 ; _rtB -> B_390_4279_0 = ( ( _rtB ->
B_390_4266_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_f ) * rtb_B_390_3422_0 * _rtP
-> P_339 + _rtB -> B_390_4266_0 [ 1 ] ) * ( rtb_B_390_3422_0 /
rtb_B_390_3874_0 ) + ( _rtB -> B_390_4267_0 - _rtB -> B_390_4272_0 ) *
rtb_B_390_4132_0 ; } else { _rtB -> B_390_4279_0 = _rtDW ->
UnitDelay1_DSTATE_at ; } _rtB -> B_390_4280_0 = _rtB -> B_390_4279_0 * _rtB
-> B_329_41_0 ; _rtDW -> DiscreteDerivative_tmp_p = ( _rtB -> B_390_4280_0 -
_rtP -> P_3050 [ 1 ] * _rtDW -> DiscreteDerivative_states_c ) / _rtP ->
P_3050 [ 0 ] ; rtb_B_390_784_0 = ( _rtP -> P_3056 * _rtB -> B_390_4280_0 +
_rtDW -> DiscreteTimeIntegrator_DSTATE_f ) + ( _rtP -> P_3049 [ 0 ] * _rtDW
-> DiscreteDerivative_tmp_p + _rtP -> P_3049 [ 1 ] * _rtDW ->
DiscreteDerivative_states_c ) ; if ( rtb_B_390_784_0 > _rtP -> P_3057 ) {
_rtB -> B_390_4285_0 = _rtP -> P_3057 ; } else if ( rtb_B_390_784_0 < _rtP ->
P_3058 ) { _rtB -> B_390_4285_0 = _rtP -> P_3058 ; } else { _rtB ->
B_390_4285_0 = rtb_B_390_784_0 ; } rtb_B_390_4132_0 = _rtP -> P_3059 * _rtB
-> B_390_4285_0 ; rtb_B_390_16_0 = rtb_B_390_4132_0 - _rtDW -> PrevY_i ; if (
rtb_B_390_16_0 > _rtP -> P_3060 ) { rtb_B_390_4132_0 = _rtDW -> PrevY_i +
_rtP -> P_3060 ; } else { if ( rtb_B_390_16_0 < _rtP -> P_3061 ) {
rtb_B_390_4132_0 = _rtDW -> PrevY_i + _rtP -> P_3061 ; } } _rtDW -> PrevY_i =
rtb_B_390_4132_0 ; _rtB -> B_390_4297_0 = ( _rtP -> P_3064 * _rtDW ->
Delay_x1_DSTATE_h + _rtP -> P_3066 * _rtDW -> Delay_x2_DSTATE_c ) + _rtP ->
P_3069 * rtb_B_390_4132_0 ; _rtB -> B_390_4299_0 = ( _rtP -> P_3067 * _rtDW
-> Delay_x1_DSTATE_h + _rtP -> P_3068 * _rtDW -> Delay_x2_DSTATE_c ) + _rtP
-> P_3070 * rtb_B_390_4132_0 ; _rtB -> B_390_4304_0 = ( _rtP -> P_3072 *
_rtDW -> Delay_x1_DSTATE_h + _rtP -> P_3073 * _rtDW -> Delay_x2_DSTATE_c ) +
_rtP -> P_3071 * rtb_B_390_4132_0 ; } _rtB -> B_390_4306_0 = (
muDoubleScalarSin ( _rtP -> P_3076 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3077
) * _rtP -> P_3074 + _rtP -> P_3075 ) * _rtB -> B_390_4119_0 [ 0 ] ; _rtB ->
B_390_4308_0 = ( muDoubleScalarSin ( _rtP -> P_3080 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3081 ) * _rtP -> P_3078 + _rtP -> P_3079 ) * _rtB -> B_390_4119_0
[ 0 ] ; _rtB -> B_390_4310_0 = ( muDoubleScalarSin ( _rtP -> P_3084 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3085 ) * _rtP -> P_3082 + _rtP -> P_3083
) * _rtB -> B_390_4119_0 [ 1 ] ; _rtB -> B_390_4312_0 = ( muDoubleScalarSin (
_rtP -> P_3088 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3089 ) * _rtP -> P_3086
+ _rtP -> P_3087 ) * _rtB -> B_390_4119_0 [ 1 ] ; _rtB -> B_390_4314_0 = (
muDoubleScalarSin ( _rtP -> P_3092 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3093
) * _rtP -> P_3090 + _rtP -> P_3091 ) * _rtB -> B_390_4119_0 [ 2 ] ; _rtB ->
B_390_4316_0 = ( muDoubleScalarSin ( _rtP -> P_3096 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3097 ) * _rtP -> P_3094 + _rtP -> P_3095 ) * _rtB -> B_390_4119_0
[ 2 ] ; B_390_4317_0 = _rtP -> P_3098 * rtb_B_390_4_1 ; _rtB -> B_390_4319_0
= ( muDoubleScalarSin ( _rtP -> P_3101 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_3102 ) * _rtP -> P_3099 + _rtP -> P_3100 ) * _rtB -> B_390_4246_0 [ 0 ] ;
_rtB -> B_390_4321_0 = ( muDoubleScalarSin ( _rtP -> P_3105 * ssGetTaskTime (
S , 0 ) + _rtP -> P_3106 ) * _rtP -> P_3103 + _rtP -> P_3104 ) * _rtB ->
B_390_4246_0 [ 0 ] ; _rtB -> B_390_4323_0 = ( muDoubleScalarSin ( _rtP ->
P_3109 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3110 ) * _rtP -> P_3107 + _rtP
-> P_3108 ) * _rtB -> B_390_4246_0 [ 1 ] ; _rtB -> B_390_4325_0 = (
muDoubleScalarSin ( _rtP -> P_3113 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3114
) * _rtP -> P_3111 + _rtP -> P_3112 ) * _rtB -> B_390_4246_0 [ 1 ] ; _rtB ->
B_390_4327_0 = ( muDoubleScalarSin ( _rtP -> P_3117 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3118 ) * _rtP -> P_3115 + _rtP -> P_3116 ) * _rtB -> B_390_4246_0
[ 2 ] ; _rtB -> B_390_4329_0 = ( muDoubleScalarSin ( _rtP -> P_3121 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3122 ) * _rtP -> P_3119 + _rtP -> P_3120
) * _rtB -> B_390_4246_0 [ 2 ] ; B_390_4330_0 = _rtP -> P_3123 *
B_390_401_1_idx_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
B_390_4371_0 = _rtP -> P_3125 * _rtB -> B_0_14_0 [ 127 ] * ( _rtP -> P_3126 *
_rtB -> B_0_14_0 [ 72 ] ) ; _rtB -> B_390_4385_0 = _rtP -> P_3127 *
rtb_B_390_185_0 + _rtP -> P_3128 * _rtB -> B_0_23_0 ; _rtB -> B_390_4391_0 =
_rtP -> P_3129 * rtb_B_390_183_0 + _rtP -> P_3130 * _rtB -> B_0_27_0 ; _rtB
-> B_390_4399_0 = _rtP -> P_3131 * _rtB -> B_0_14_0 [ 75 ] ; _rtB ->
B_390_4403_0 = _rtP -> P_3132 * _rtB -> B_0_14_0 [ 129 ] ; _rtB ->
B_390_4407_0 = _rtP -> P_3133 * _rtB -> B_0_14_0 [ 130 ] ; if ( _rtB ->
B_0_14_1 [ 21 ] >= _rtP -> P_3135 ) { rtb_B_390_4132_0 = _rtP -> P_294 * _rtB
-> B_0_14_0 [ 21 ] ; } else { rtb_B_390_4132_0 = _rtB -> B_390_4420_0 ; } if
( rtb_B_390_4132_0 > _rtP -> P_3136 ) { B_390_4423_0 = _rtP -> P_3136 ; }
else if ( rtb_B_390_4132_0 < _rtP -> P_3137 ) { B_390_4423_0 = _rtP -> P_3137
; } else { B_390_4423_0 = rtb_B_390_4132_0 ; } B_390_4424_0 = _rtB ->
B_0_14_0 [ 21 ] + _rtB -> B_390_231_0 ; if ( _rtB -> B_0_14_1 [ 22 ] >= _rtP
-> P_3139 ) { rtb_B_390_4132_0 = _rtP -> P_295 * _rtB -> B_0_14_0 [ 22 ] ; }
else { rtb_B_390_4132_0 = _rtB -> B_390_4428_0 ; } if ( rtb_B_390_4132_0 >
_rtP -> P_3140 ) { B_390_4431_0 = _rtP -> P_3140 ; } else if (
rtb_B_390_4132_0 < _rtP -> P_3141 ) { B_390_4431_0 = _rtP -> P_3141 ; } else
{ B_390_4431_0 = rtb_B_390_4132_0 ; } B_390_4432_0 = _rtB -> B_0_14_0 [ 22 ]
+ _rtB -> B_390_232_0 ; _rtB -> B_390_4446_0 = _rtP -> P_3142 * _rtB ->
B_0_14_0 [ 73 ] ; ssCallAccelRunBlock ( S , 390 , 4447 , SS_CALL_MDL_OUTPUTS
) ; } _rtB -> B_390_4457_0 = 0.0 ; _rtB -> B_390_4457_0 += _rtP -> P_3144 *
_rtX -> LPF1_CSTATE_a ; _rtB -> B_390_4458_0 = 0.0 ; _rtB -> B_390_4458_0 +=
_rtP -> P_3146 * _rtX -> LPF2_CSTATE_d ; _rtB -> B_390_4459_0 = _rtX ->
integrator_CSTATE_ib ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3148 ;
B_390_4460_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ml .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ml . Last , _rtDW ->
TransportDelay_IWORK_ml . Tail , _rtDW -> TransportDelay_IWORK_ml . Head ,
_rtP -> P_3149 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4462_0 = _rtP -> P_3150 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4462_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4464_0 =
_rtDW -> Memory_PreviousInput_on ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4466_0 = ( _rtB -> B_390_4459_0 - B_390_4460_0 ) * _rtP -> P_399 ; }
else { _rtB -> B_390_4466_0 = _rtB -> B_390_4464_0 ; } _rtB -> B_390_4467_0 =
_rtX -> integrator_CSTATE_j3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ie . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ie . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3153 ;
B_390_4468_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_av .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_av . Last , _rtDW ->
TransportDelay_IWORK_av . Tail , _rtDW -> TransportDelay_IWORK_av . Head ,
_rtP -> P_3154 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4470_0 = _rtP -> P_3155 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4470_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4472_0 =
_rtDW -> Memory_PreviousInput_nh ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4474_0 = ( _rtB -> B_390_4467_0 - B_390_4468_0 ) * _rtP -> P_398 ; }
else { _rtB -> B_390_4474_0 = _rtB -> B_390_4472_0 ; } rtb_B_390_4132_0 =
_rtP -> P_3157 * muDoubleScalarAtan2 ( _rtB -> B_390_4474_0 , _rtB ->
B_390_4466_0 ) * _rtP -> P_3158 ; B_390_4479_0 . re = muDoubleScalarHypot (
_rtB -> B_390_4466_0 , _rtB -> B_390_4474_0 ) * muDoubleScalarCos (
rtb_B_390_4132_0 ) ; B_390_4479_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_4466_0 , _rtB -> B_390_4474_0 ) * muDoubleScalarSin ( rtb_B_390_4132_0
) ; _rtB -> B_390_4480_0 = _rtX -> integrator_CSTATE_bx ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3160 ; B_390_4481_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_bcn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bcn . Last , _rtDW -> TransportDelay_IWORK_bcn . Tail ,
_rtDW -> TransportDelay_IWORK_bcn . Head , _rtP -> P_3161 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4483_0 = _rtP -> P_3162 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4483_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4485_0 = _rtDW -> Memory_PreviousInput_gw ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4487_0 = ( _rtB -> B_390_4480_0 -
B_390_4481_0 ) * _rtP -> P_401 ; } else { _rtB -> B_390_4487_0 = _rtB ->
B_390_4485_0 ; } _rtB -> B_390_4488_0 = _rtX -> integrator_CSTATE_h0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3165 ; B_390_4489_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ge . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ge . Last , _rtDW -> TransportDelay_IWORK_ge . Tail ,
_rtDW -> TransportDelay_IWORK_ge . Head , _rtP -> P_3166 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4491_0 = _rtP -> P_3167 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4491_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4493_0 = _rtDW -> Memory_PreviousInput_ln ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4495_0 = ( _rtB -> B_390_4488_0 -
B_390_4489_0 ) * _rtP -> P_400 ; } else { _rtB -> B_390_4495_0 = _rtB ->
B_390_4493_0 ; } rtb_B_390_4132_0 = _rtP -> P_3169 * muDoubleScalarAtan2 (
_rtB -> B_390_4495_0 , _rtB -> B_390_4487_0 ) * _rtP -> P_3170 ; B_390_4500_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4487_0 , _rtB -> B_390_4495_0 ) *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; B_390_4500_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4487_0 , _rtB -> B_390_4495_0 ) *
muDoubleScalarSin ( rtb_B_390_4132_0 ) ; _rtB -> B_390_4501_0 = _rtX ->
integrator_CSTATE_pa ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_agi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_agi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3172 ;
B_390_4502_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_np .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_np . Last , _rtDW ->
TransportDelay_IWORK_np . Tail , _rtDW -> TransportDelay_IWORK_np . Head ,
_rtP -> P_3173 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4504_0 = _rtP -> P_3174 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4504_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4506_0 =
_rtDW -> Memory_PreviousInput_cy ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4508_0 = ( _rtB -> B_390_4501_0 - B_390_4502_0 ) * _rtP -> P_403 ; }
else { _rtB -> B_390_4508_0 = _rtB -> B_390_4506_0 ; } _rtB -> B_390_4509_0 =
_rtX -> integrator_CSTATE_ew ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_iyk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3177
; B_390_4510_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_p2 . Last , _rtDW ->
TransportDelay_IWORK_p2 . Tail , _rtDW -> TransportDelay_IWORK_p2 . Head ,
_rtP -> P_3178 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4512_0 = _rtP -> P_3179 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4512_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4514_0 =
_rtDW -> Memory_PreviousInput_lj ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4516_0 = ( _rtB -> B_390_4509_0 - B_390_4510_0 ) * _rtP -> P_402 ; }
else { _rtB -> B_390_4516_0 = _rtB -> B_390_4514_0 ; } rtb_B_390_4132_0 =
_rtP -> P_3181 * muDoubleScalarAtan2 ( _rtB -> B_390_4516_0 , _rtB ->
B_390_4508_0 ) * _rtP -> P_3182 ; B_390_4521_0 . re = muDoubleScalarHypot (
_rtB -> B_390_4508_0 , _rtB -> B_390_4516_0 ) * muDoubleScalarCos (
rtb_B_390_4132_0 ) ; B_390_4521_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_4508_0 , _rtB -> B_390_4516_0 ) * muDoubleScalarSin ( rtb_B_390_4132_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4523_0 = _rtB -> B_390_4522_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_4523_0 , B_390_4479_0 , B_390_4500_0 , B_390_4521_0 , &
_rtB -> PosSeqComputation_jw , & _rtDW -> PosSeqComputation_jw , & _rtP ->
PosSeqComputation_jw ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_4526_0 = _rtB -> B_390_4525_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_4526_0 ,
B_390_4479_0 , B_390_4500_0 , B_390_4521_0 , & _rtB -> NegSeqComputation_p ,
& _rtDW -> NegSeqComputation_p , & _rtP -> NegSeqComputation_p ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4529_0 = _rtB
-> B_390_4528_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_4529_0 , B_390_4479_0 , B_390_4500_0 , B_390_4521_0 , & _rtB ->
ZeroSeqComputation_jv , & _rtDW -> ZeroSeqComputation_jv , & _rtP ->
ZeroSeqComputation_jv ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_jw . B_22_2_0 . im , _rtB -> PosSeqComputation_jw .
B_22_2_0 . re ) ; _rtB -> B_390_4532_0 = _rtP -> P_3186 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_jw . B_22_2_0 . re , _rtB -> PosSeqComputation_jw
. B_22_2_0 . im ) ; _rtB -> B_390_4533_0 = _rtX -> integrator_CSTATE_hh ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ais .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ais . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3188 ; B_390_4534_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_e0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e0 . Last , _rtDW -> TransportDelay_IWORK_e0 . Tail ,
_rtDW -> TransportDelay_IWORK_e0 . Head , _rtP -> P_3189 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4536_0 = _rtP -> P_3190 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4536_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4538_0 = _rtDW -> Memory_PreviousInput_ko ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4540_0 = ( _rtB -> B_390_4533_0 -
B_390_4534_0 ) * _rtP -> P_405 ; } else { _rtB -> B_390_4540_0 = _rtB ->
B_390_4538_0 ; } _rtB -> B_390_4541_0 = _rtX -> integrator_CSTATE_fcw ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fh .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fh . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3193 ; B_390_4542_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_jr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jr . Last , _rtDW -> TransportDelay_IWORK_jr . Tail ,
_rtDW -> TransportDelay_IWORK_jr . Head , _rtP -> P_3194 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4544_0 = _rtP -> P_3195 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4544_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4546_0 = _rtDW -> Memory_PreviousInput_dc ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4548_0 = ( _rtB -> B_390_4541_0 -
B_390_4542_0 ) * _rtP -> P_404 ; } else { _rtB -> B_390_4548_0 = _rtB ->
B_390_4546_0 ; } rtb_B_390_4132_0 = _rtP -> P_3197 * muDoubleScalarAtan2 (
_rtB -> B_390_4548_0 , _rtB -> B_390_4540_0 ) * _rtP -> P_3198 ; B_390_4553_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4540_0 , _rtB -> B_390_4548_0 ) *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; B_390_4553_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4540_0 , _rtB -> B_390_4548_0 ) *
muDoubleScalarSin ( rtb_B_390_4132_0 ) ; _rtB -> B_390_4554_0 = _rtX ->
integrator_CSTATE_kg1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3200 ;
B_390_4555_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_g2 . Last , _rtDW ->
TransportDelay_IWORK_g2 . Tail , _rtDW -> TransportDelay_IWORK_g2 . Head ,
_rtP -> P_3201 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4557_0 = _rtP -> P_3202 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4557_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4559_0 =
_rtDW -> Memory_PreviousInput_jl ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4561_0 = ( _rtB -> B_390_4554_0 - B_390_4555_0 ) * _rtP -> P_407 ; }
else { _rtB -> B_390_4561_0 = _rtB -> B_390_4559_0 ; } _rtB -> B_390_4562_0 =
_rtX -> integrator_CSTATE_it ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ejf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3205
; B_390_4563_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kq . Last , _rtDW ->
TransportDelay_IWORK_kq . Tail , _rtDW -> TransportDelay_IWORK_kq . Head ,
_rtP -> P_3206 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4565_0 = _rtP -> P_3207 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_4565_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4567_0 =
_rtDW -> Memory_PreviousInput_gk ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_4569_0 = ( _rtB -> B_390_4562_0 - B_390_4563_0 ) * _rtP -> P_406 ; }
else { _rtB -> B_390_4569_0 = _rtB -> B_390_4567_0 ; } rtb_B_390_4132_0 =
_rtP -> P_3209 * muDoubleScalarAtan2 ( _rtB -> B_390_4569_0 , _rtB ->
B_390_4561_0 ) * _rtP -> P_3210 ; B_390_4574_0 . re = muDoubleScalarHypot (
_rtB -> B_390_4561_0 , _rtB -> B_390_4569_0 ) * muDoubleScalarCos (
rtb_B_390_4132_0 ) ; B_390_4574_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_4561_0 , _rtB -> B_390_4569_0 ) * muDoubleScalarSin ( rtb_B_390_4132_0
) ; _rtB -> B_390_4575_0 = _rtX -> integrator_CSTATE_fg ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3212 ; B_390_4576_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_oh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oh . Last , _rtDW -> TransportDelay_IWORK_oh . Tail ,
_rtDW -> TransportDelay_IWORK_oh . Head , _rtP -> P_3213 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4578_0 = _rtP -> P_3214 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4578_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4580_0 = _rtDW -> Memory_PreviousInput_kh ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4582_0 = ( _rtB -> B_390_4575_0 -
B_390_4576_0 ) * _rtP -> P_409 ; } else { _rtB -> B_390_4582_0 = _rtB ->
B_390_4580_0 ; } _rtB -> B_390_4583_0 = _rtX -> integrator_CSTATE_jb ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bz .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bz . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3217 ; B_390_4584_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_eq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eq . Last , _rtDW -> TransportDelay_IWORK_eq . Tail ,
_rtDW -> TransportDelay_IWORK_eq . Head , _rtP -> P_3218 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4586_0 = _rtP -> P_3219 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4586_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4588_0 = _rtDW -> Memory_PreviousInput_kl ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4590_0 = ( _rtB -> B_390_4583_0 -
B_390_4584_0 ) * _rtP -> P_408 ; } else { _rtB -> B_390_4590_0 = _rtB ->
B_390_4588_0 ; } rtb_B_390_4132_0 = _rtP -> P_3221 * muDoubleScalarAtan2 (
_rtB -> B_390_4590_0 , _rtB -> B_390_4582_0 ) * _rtP -> P_3222 ; B_390_4595_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4582_0 , _rtB -> B_390_4590_0 ) *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; B_390_4595_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4582_0 , _rtB -> B_390_4590_0 ) *
muDoubleScalarSin ( rtb_B_390_4132_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_4597_0 = _rtB -> B_390_4596_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_4597_0 ,
B_390_4553_0 , B_390_4574_0 , B_390_4595_0 , & _rtB -> PosSeqComputation_bv ,
& _rtDW -> PosSeqComputation_bv , & _rtP -> PosSeqComputation_bv ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4600_0 = _rtB
-> B_390_4599_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_4600_0 , B_390_4553_0 , B_390_4574_0 , B_390_4595_0 , & _rtB ->
NegSeqComputation_ih , & _rtDW -> NegSeqComputation_ih , & _rtP ->
NegSeqComputation_ih ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_4603_0 = _rtB -> B_390_4602_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_4603_0 ,
B_390_4553_0 , B_390_4574_0 , B_390_4595_0 , & _rtB -> ZeroSeqComputation_i ,
& _rtDW -> ZeroSeqComputation_i , & _rtP -> ZeroSeqComputation_i ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_bv .
B_22_2_0 . im , _rtB -> PosSeqComputation_bv . B_22_2_0 . re ) ; _rtB ->
B_390_4606_0 = _rtP -> P_3226 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_bv . B_22_2_0 . re , _rtB -> PosSeqComputation_bv .
B_22_2_0 . im ) ; ssCallAccelRunBlock ( S , 390 , 4608 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4609_0 = _rtP -> P_3228 * _rtB -> B_0_14_0 [ 82 ] ; } _rtB ->
B_390_4076_0 [ 0 ] = _rtB -> B_390_4457_0 ; _rtB -> B_390_4076_0 [ 1 ] = _rtB
-> B_390_4458_0 ; _rtB -> B_390_4076_0 [ 2 ] = _rtB -> B_390_4532_0 ; _rtB ->
B_390_4076_0 [ 3 ] = _rtB -> B_390_4606_0 ; _rtB -> B_390_4076_0 [ 4 ] = _rtB
-> B_390_4609_0 ; ssCallAccelRunBlock ( S , 390 , 4611 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP ->
P_3230 * _rtB -> B_0_14_0 [ 142 ] * _rtP -> P_3233 ; tmp_1 = _rtP -> P_3231 *
_rtB -> B_0_14_0 [ 143 ] * _rtP -> P_3233 ; rtb_B_390_16_0 = _rtP -> P_3232 *
_rtB -> B_0_14_0 [ 144 ] * _rtP -> P_3233 ; for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_3234 [ isHit + 6 ] * rtb_B_390_16_0 +
( _rtP -> P_3234 [ isHit + 3 ] * tmp_1 + _rtP -> P_3234 [ isHit ] * tmp_0 ) ;
} B_390_4618_0 [ 0 ] = _rtP -> P_3235 * tmp [ 0 ] ; B_390_4618_0 [ 1 ] = _rtP
-> P_3235 * tmp [ 1 ] ; B_390_4618_0 [ 2 ] = _rtP -> P_3235 * tmp [ 2 ] ;
_rtB -> B_390_4622_0 = _rtB -> B_390_4621_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b
( _rtB -> B_390_4622_0 , & B_390_4618_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB
-> Subsystem1_n , & _rtDW -> Subsystem1_n ) ; _rtB -> B_390_4626_0 = _rtB ->
B_390_4625_0 ; MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4626_0
, & B_390_4618_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystempi2delay_m
, & _rtDW -> Subsystempi2delay_m ) ; if ( _rtB -> B_390_4621_0 != 0 ) { _rtB
-> B_390_4628_0 [ 0 ] = _rtB -> Subsystem1_n . B_10_0_0 ; _rtB ->
B_390_4628_0 [ 1 ] = _rtB -> Subsystem1_n . B_10_1_0 ; } else { _rtB ->
B_390_4628_0 [ 0 ] = _rtB -> Subsystempi2delay_m . B_9_0_0 ; _rtB ->
B_390_4628_0 [ 1 ] = _rtB -> Subsystempi2delay_m . B_9_1_0 ; } } _rtB ->
B_390_4629_0 = 0.0 ; _rtB -> B_390_4629_0 += _rtP -> P_3240 * _rtX ->
Integradord_CSTATE_an ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_4632_0 = ( _rtB -> B_390_4630_0 - _rtB -> B_390_4609_0 ) *
_rtP -> P_3242 ; _rtB -> B_390_4633_0 = _rtP -> P_3243 * _rtB -> B_390_4632_0
; } _rtB -> B_390_4634_0 = _rtB -> B_390_4629_0 + _rtB -> B_390_4633_0 ; _rtB
-> B_390_4636_0 = _rtB -> B_390_4635_0 - _rtB -> B_390_4458_0 ; _rtB ->
B_390_4637_0 = _rtP -> P_3245 * _rtB -> B_390_4636_0 ; _rtB -> B_390_4638_0 =
_rtP -> P_3246 * _rtB -> B_390_4637_0 ; _rtB -> B_390_4639_0 = 0.0 ; _rtB ->
B_390_4639_0 += _rtP -> P_3248 * _rtX -> Integradord1_CSTATE_c ; _rtB ->
B_390_4640_0 = _rtB -> B_390_4638_0 + _rtB -> B_390_4639_0 ; _rtB ->
B_390_4641_0 . re = _rtB -> B_390_4634_0 ; _rtB -> B_390_4641_0 . im = _rtB
-> B_390_4640_0 ; _rtB -> B_390_4642_0 = muDoubleScalarHypot ( _rtB ->
B_390_4641_0 . re , _rtB -> B_390_4641_0 . im ) ; rtb_B_390_4132_0 =
muDoubleScalarAtan2 ( _rtB -> B_390_4641_0 . im , _rtB -> B_390_4641_0 . re )
; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW -> Saturation_MODE_i = _rtB ->
B_390_4642_0 >= _rtP -> P_3249 ? 1 : _rtB -> B_390_4642_0 > _rtP -> P_3250 ?
0 : - 1 ; } _rtB -> B_390_4643_0 = _rtDW -> Saturation_MODE_i == 1 ? _rtP ->
P_3249 : _rtDW -> Saturation_MODE_i == - 1 ? _rtP -> P_3250 : _rtB ->
B_390_4642_0 ; _rtB -> B_390_4645_0 = _rtB -> B_390_4643_0 *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; _rtB -> B_390_4645_1 = _rtB ->
B_390_4643_0 * muDoubleScalarSin ( rtb_B_390_4132_0 ) ; _rtB -> B_390_4646_0
= _rtB -> B_390_4645_0 - _rtB -> B_390_4628_0 [ 0 ] ; _rtB -> B_390_4647_0 =
0.0 ; _rtB -> B_390_4647_0 += _rtP -> P_3252 * _rtX -> Controled_CSTATE_o ;
_rtB -> B_390_4647_0 += _rtP -> P_3253 * _rtB -> B_390_4646_0 ; _rtB ->
B_390_4648_0 = _rtB -> B_390_4645_1 - _rtB -> B_390_4628_0 [ 1 ] ; _rtB ->
B_390_4649_0 = 0.0 ; _rtB -> B_390_4649_0 += _rtP -> P_3255 * _rtX ->
Controleq_CSTATE_j ; _rtB -> B_390_4649_0 += _rtP -> P_3256 * _rtB ->
B_390_4648_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_390_4653_0 [ 0 ] = _rtP -> P_3257 * _rtB -> B_0_14_0 [ 90 ] * _rtP ->
P_3260 ; _rtB -> B_390_4653_0 [ 1 ] = _rtP -> P_3258 * _rtB -> B_0_14_0 [ 91
] * _rtP -> P_3260 ; _rtB -> B_390_4653_0 [ 2 ] = _rtP -> P_3259 * _rtB ->
B_0_14_0 [ 92 ] * _rtP -> P_3260 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_3261 [ isHit + 6 ] * _rtB -> B_390_4653_0 [ 2 ] + (
_rtP -> P_3261 [ isHit + 3 ] * _rtB -> B_390_4653_0 [ 1 ] + _rtP -> P_3261 [
isHit ] * _rtB -> B_390_4653_0 [ 0 ] ) ; } B_390_4655_0 [ 0 ] = _rtP ->
P_3262 * tmp [ 0 ] ; B_390_4655_0 [ 1 ] = _rtP -> P_3262 * tmp [ 1 ] ;
B_390_4655_0 [ 2 ] = _rtP -> P_3262 * tmp [ 2 ] ; _rtB -> B_390_4659_0 = _rtB
-> B_390_4658_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_390_4659_0 , &
B_390_4655_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystem1_i4 , & _rtDW
-> Subsystem1_i4 ) ; _rtB -> B_390_4663_0 = _rtB -> B_390_4662_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4663_0 , &
B_390_4655_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystempi2delay_cjn ,
& _rtDW -> Subsystempi2delay_cjn ) ; if ( _rtB -> B_390_4658_0 != 0 ) { _rtB
-> B_390_4665_0 [ 0 ] = _rtB -> Subsystem1_i4 . B_10_0_0 ; _rtB ->
B_390_4665_0 [ 1 ] = _rtB -> Subsystem1_i4 . B_10_1_0 ; } else { _rtB ->
B_390_4665_0 [ 0 ] = _rtB -> Subsystempi2delay_cjn . B_9_0_0 ; _rtB ->
B_390_4665_0 [ 1 ] = _rtB -> Subsystempi2delay_cjn . B_9_1_0 ; } } isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { rtb_B_390_4666_0 = _rtDW ->
UnitDelay_DSTATE_j ; _rtB -> B_390_4667_0 = _rtP -> P_3267 * _rtDW ->
UnitDelay_DSTATE_j ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_390_4669_0 = _rtB -> B_390_4667_0 * _rtB -> B_390_4628_0 [ 1 ]
* _rtP -> P_3268 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_390_4671_0 = _rtB -> B_390_4609_0 * _rtB -> B_390_4612_0 ; } _rtB
-> B_390_4672_0 = ( ( _rtB -> B_390_4665_0 [ 0 ] + _rtB -> B_390_4647_0 ) -
_rtB -> B_390_4669_0 ) / _rtB -> B_390_4671_0 ; isHit = ssIsSampleHit ( S , 2
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_4674_0 = _rtB -> B_390_4628_0 [ 0 ]
* _rtB -> B_390_4667_0 * _rtP -> P_3269 ; } _rtB -> B_390_4676_0 = ( ( _rtB
-> B_390_4674_0 + _rtB -> B_390_4649_0 ) + _rtB -> B_390_4665_0 [ 1 ] ) * (
1.0 / _rtB -> B_390_4671_0 ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_e = _rtB -> B_390_4676_0 >= _rtP -> P_3270 ? 1 : _rtB ->
B_390_4676_0 > _rtP -> P_3271 ? 0 : - 1 ; _rtDW -> Saturation1_MODE_l = _rtB
-> B_390_4672_0 >= _rtP -> P_3272 ? 1 : _rtB -> B_390_4672_0 > _rtP -> P_3273
? 0 : - 1 ; } _rtB -> B_390_4677_0 = _rtDW -> Saturation_MODE_e == 1 ? _rtP
-> P_3270 : _rtDW -> Saturation_MODE_e == - 1 ? _rtP -> P_3271 : _rtB ->
B_390_4676_0 ; _rtB -> B_390_4678_0 = _rtDW -> Saturation1_MODE_l == 1 ? _rtP
-> P_3272 : _rtDW -> Saturation1_MODE_l == - 1 ? _rtP -> P_3273 : _rtB ->
B_390_4672_0 ; _rtB -> B_390_4679_0 = _rtB -> B_390_4634_0 - _rtB ->
B_390_4645_0 ; _rtB -> B_390_4680_0 = _rtP -> P_3274 * _rtB -> B_390_4679_0 ;
_rtB -> B_390_4681_0 = _rtB -> B_390_4632_0 - _rtB -> B_390_4680_0 ; _rtB ->
B_390_4682_0 = _rtP -> P_3275 * _rtB -> B_390_4681_0 ; _rtB -> B_390_4683_0 =
_rtB -> B_390_4640_0 - _rtB -> B_390_4645_1 ; _rtB -> B_390_4684_0 = _rtP ->
P_3276 * _rtB -> B_390_4683_0 ; _rtB -> B_390_4685_0 = _rtB -> B_390_4637_0 -
_rtB -> B_390_4684_0 ; _rtB -> B_390_4686_0 = _rtP -> P_3277 * _rtB ->
B_390_4685_0 ; ssCallAccelRunBlock ( S , 390 , 4687 , SS_CALL_MDL_OUTPUTS ) ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_390_4692_0 = _rtP
-> P_3278 * _rtB -> B_0_14_0 [ 137 ] * _rtB -> B_390_4609_0 ; B_390_4704_0 [
0 ] = _rtP -> P_3279 * _rtB -> B_0_14_0 [ 87 ] * _rtP -> P_3282 ;
B_390_4704_0 [ 1 ] = _rtP -> P_3280 * _rtB -> B_0_14_0 [ 88 ] * _rtP ->
P_3282 ; B_390_4704_0 [ 2 ] = _rtP -> P_3281 * _rtB -> B_0_14_0 [ 89 ] * _rtP
-> P_3282 ; _rtB -> B_390_4747_0 = ( _rtB -> B_390_119_0 * _rtB -> B_0_14_0 [
154 ] - _rtB -> B_390_120_0 * _rtB -> B_0_14_0 [ 155 ] ) / ( ( _rtP -> P_3284
* _rtB -> B_390_116_0 - _rtDW -> prediction_DSTATE_j ) + _rtB -> B_390_4739_0
) ; _rtB -> B_390_4765_0 = ( _rtB -> B_390_4665_0 [ 0 ] * _rtB ->
B_390_4628_0 [ 0 ] + _rtB -> B_390_4665_0 [ 1 ] * _rtB -> B_390_4628_0 [ 1 ]
) * _rtP -> P_3286 ; _rtB -> B_390_4769_0 = ( _rtB -> B_390_4628_0 [ 0 ] *
_rtB -> B_390_4665_0 [ 1 ] - _rtB -> B_390_4665_0 [ 0 ] * _rtB ->
B_390_4628_0 [ 1 ] ) * _rtP -> P_3287 ; _rtB -> B_390_4775_0 = _rtB ->
B_390_4774_0 ; _rtB -> B_390_4776_0 = _rtB -> B_390_4772_0 ; _rtB ->
B_390_4780_0 [ 0 ] = _rtP -> P_3291 * _rtB -> B_0_14_0 [ 145 ] * _rtP ->
P_3294 ; _rtB -> B_390_4780_0 [ 1 ] = _rtP -> P_3292 * _rtB -> B_0_14_0 [ 146
] * _rtP -> P_3294 ; _rtB -> B_390_4780_0 [ 2 ] = _rtP -> P_3293 * _rtB ->
B_0_14_0 [ 147 ] * _rtP -> P_3294 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_3295 [ isHit + 6 ] * _rtB -> B_390_4780_0 [ 2 ] +
( _rtP -> P_3295 [ isHit + 3 ] * _rtB -> B_390_4780_0 [ 1 ] + _rtP -> P_3295
[ isHit ] * _rtB -> B_390_4780_0 [ 0 ] ) ; } B_390_4782_0 [ 0 ] = _rtP ->
P_3296 * tmp [ 0 ] ; B_390_4782_0 [ 1 ] = _rtP -> P_3296 * tmp [ 1 ] ;
B_390_4782_0 [ 2 ] = _rtP -> P_3296 * tmp [ 2 ] ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4775_0 , &
B_390_4782_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystempi2delay_kg , &
_rtDW -> Subsystempi2delay_kg ) ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB ->
B_390_4776_0 , & B_390_4782_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB ->
Subsystem1_o , & _rtDW -> Subsystem1_o ) ; if ( _rtB -> B_390_4772_0 != 0 ) {
B_390_4785_0 [ 0 ] = _rtB -> Subsystem1_o . B_10_0_0 ; B_390_4785_0 [ 1 ] =
_rtB -> Subsystem1_o . B_10_1_0 ; } else { B_390_4785_0 [ 0 ] = _rtB ->
Subsystempi2delay_kg . B_9_0_0 ; B_390_4785_0 [ 1 ] = _rtB ->
Subsystempi2delay_kg . B_9_1_0 ; } } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_390_4787_0 = _rtP -> P_3297 ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( _rtB -> B_390_4787_0 > 0.0 ) { if ( ! _rtDW ->
AutomaticGainControl_MODE ) { if ( ssGetTaskTime ( S , 3 ) != ssGetTStart ( S
) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
Integ4_SYSTEM_ENABLE_k = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_b = 1U ; _rtDW ->
AutomaticGainControl_MODE = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S
) ; _rtDW -> AutomaticGainControl_MODE = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_357 [ isHit + 6 ] * _rtB -> B_390_4653_0 [ 2 ] + ( _rtP -> P_357 [
isHit + 3 ] * _rtB -> B_390_4653_0 [ 1 ] + _rtP -> P_357 [ isHit ] * _rtB ->
B_390_4653_0 [ 0 ] ) ; } B_366_1_0 [ 0 ] = _rtP -> P_358 * tmp [ 0 ] ;
B_366_1_0 [ 1 ] = _rtP -> P_358 * tmp [ 1 ] ; B_366_1_0 [ 2 ] = _rtP -> P_358
* tmp [ 2 ] ; _rtB -> B_366_5_0 = _rtB -> B_366_4_0 ;
MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_366_5_0 , & B_366_1_0 [ 0 ] ,
_rtB -> B_390_103_0 , & _rtB -> Subsystem1_lu , & _rtDW -> Subsystem1_lu ) ;
_rtB -> B_366_9_0 = _rtB -> B_366_8_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_366_9_0 , & B_366_1_0 [ 0
] , _rtB -> B_390_103_0 , & _rtB -> Subsystempi2delay_b , & _rtDW ->
Subsystempi2delay_b ) ; if ( _rtB -> B_366_4_0 != 0 ) { _rtB -> B_366_11_0 [
0 ] = _rtB -> Subsystem1_lu . B_10_0_0 ; _rtB -> B_366_11_0 [ 1 ] = _rtB ->
Subsystem1_lu . B_10_1_0 ; } else { _rtB -> B_366_11_0 [ 0 ] = _rtB ->
Subsystempi2delay_b . B_9_0_0 ; _rtB -> B_366_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_b . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_k != 0 )
{ _rtB -> B_366_12_0 = _rtDW -> Integ4_DSTATE_j ; } else { _rtB -> B_366_12_0
= _rtP -> P_362 * _rtB -> B_366_11_0 [ 0 ] + _rtDW -> Integ4_DSTATE_j ; } if
( rtb_B_390_4666_0 > _rtP -> P_364 ) { rtb_B_390_4132_0 = _rtP -> P_364 ; }
else if ( rtb_B_390_4666_0 < _rtP -> P_365 ) { rtb_B_390_4132_0 = _rtP ->
P_365 ; } else { rtb_B_390_4132_0 = rtb_B_390_4666_0 ; } rtb_B_390_4132_0 =
1.0 / rtb_B_390_4132_0 / 5.0e-5 ; rtb_B_390_183_0 = muDoubleScalarCeil (
rtb_B_390_4132_0 ) ; _rtB -> B_366_16_0 = _rtP -> P_366 * rtb_B_390_183_0 ;
ssCallAccelRunBlock ( S , 366 , 17 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_366_20_0 ) { rtb_B_390_183_0 =
rtb_B_390_4132_0 - rtb_B_390_183_0 ; _rtB -> B_366_24_0 = ( ( _rtB ->
B_366_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_d ) * rtb_B_390_183_0 * _rtP ->
P_354 + _rtB -> B_366_11_0 [ 0 ] ) * ( rtb_B_390_183_0 / rtb_B_390_4132_0 ) +
( _rtB -> B_366_12_0 - _rtB -> B_366_17_0 ) * rtb_B_390_4666_0 ; } else {
_rtB -> B_366_24_0 = _rtDW -> UnitDelay1_DSTATE_g ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_b != 0 ) { _rtB -> B_366_25_0 = _rtDW ->
Integ4_DSTATE_bx ; } else { _rtB -> B_366_25_0 = _rtP -> P_378 * _rtB ->
B_366_11_0 [ 1 ] + _rtDW -> Integ4_DSTATE_bx ; } if ( rtb_B_390_4666_0 > _rtP
-> P_380 ) { rtb_B_390_4132_0 = _rtP -> P_380 ; } else if ( rtb_B_390_4666_0
< _rtP -> P_381 ) { rtb_B_390_4132_0 = _rtP -> P_381 ; } else {
rtb_B_390_4132_0 = rtb_B_390_4666_0 ; } rtb_B_390_4132_0 = 1.0 /
rtb_B_390_4132_0 / 5.0e-5 ; rtb_B_390_183_0 = muDoubleScalarCeil (
rtb_B_390_4132_0 ) ; _rtB -> B_366_29_0 = _rtP -> P_382 * rtb_B_390_183_0 ;
ssCallAccelRunBlock ( S , 366 , 30 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_366_33_0 ) { rtb_B_390_183_0 =
rtb_B_390_4132_0 - rtb_B_390_183_0 ; _rtB -> B_366_37_0 = ( ( _rtB ->
B_366_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_e ) * rtb_B_390_183_0 * _rtP ->
P_355 + _rtB -> B_366_11_0 [ 1 ] ) * ( rtb_B_390_183_0 / rtb_B_390_4132_0 ) +
( _rtB -> B_366_25_0 - _rtB -> B_366_30_0 ) * rtb_B_390_4666_0 ; } else {
_rtB -> B_366_37_0 = _rtDW -> UnitDelay1_DSTATE_d ; } rtb_B_390_784_0 =
muDoubleScalarHypot ( _rtB -> B_366_24_0 , _rtB -> B_366_37_0 ) ; if (
rtb_B_390_784_0 > _rtP -> P_394 ) { rtb_B_390_784_0 = _rtP -> P_394 ; } else
{ if ( rtb_B_390_784_0 < _rtP -> P_395 ) { rtb_B_390_784_0 = _rtP -> P_395 ;
} } _rtB -> B_366_41_0 = 1.0 / rtb_B_390_784_0 ; B_366_42_0 = _rtP -> P_396 *
muDoubleScalarAtan2 ( _rtB -> B_366_37_0 , _rtB -> B_366_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC ) ; } } for ( isHit = 0 ; isHit < 3 ; isHit
++ ) { tmp [ isHit ] = _rtP -> P_3298 [ isHit + 6 ] * _rtB -> B_390_4653_0 [
2 ] + ( _rtP -> P_3298 [ isHit + 3 ] * _rtB -> B_390_4653_0 [ 1 ] + _rtP ->
P_3298 [ isHit ] * _rtB -> B_390_4653_0 [ 0 ] ) ; } B_390_4790_0 [ 0 ] = _rtP
-> P_3299 * tmp [ 0 ] ; B_390_4790_0 [ 1 ] = _rtP -> P_3299 * tmp [ 1 ] ;
B_390_4790_0 [ 2 ] = _rtP -> P_3299 * tmp [ 2 ] ; _rtB -> B_390_4794_0 = _rtB
-> B_390_4793_0 ; MR_CIGRE_RESISTIVO_Subsystem1_b ( _rtB -> B_390_4794_0 , &
B_390_4790_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystem1_c , & _rtDW
-> Subsystem1_c ) ; _rtB -> B_390_4798_0 = _rtB -> B_390_4797_0 ;
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( _rtB -> B_390_4798_0 , &
B_390_4790_0 [ 0 ] , _rtB -> B_390_103_0 , & _rtB -> Subsystempi2delay_p , &
_rtDW -> Subsystempi2delay_p ) ; if ( _rtB -> B_390_4793_0 != 0 ) { _rtB ->
B_390_4800_0 [ 0 ] = _rtB -> Subsystem1_c . B_10_0_0 ; _rtB -> B_390_4800_0 [
1 ] = _rtB -> Subsystem1_c . B_10_1_0 ; } else { _rtB -> B_390_4800_0 [ 0 ] =
_rtB -> Subsystempi2delay_p . B_9_0_0 ; _rtB -> B_390_4800_0 [ 1 ] = _rtB ->
Subsystempi2delay_p . B_9_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_e != 0 )
{ _rtB -> B_390_4801_0 = _rtDW -> Integ4_DSTATE_k ; } else { _rtB ->
B_390_4801_0 = _rtP -> P_3303 * _rtB -> B_390_4800_0 [ 1 ] + _rtDW ->
Integ4_DSTATE_k ; } if ( rtb_B_390_4666_0 > _rtP -> P_3305 ) {
rtb_B_390_4132_0 = _rtP -> P_3305 ; } else if ( rtb_B_390_4666_0 < _rtP ->
P_3306 ) { rtb_B_390_4132_0 = _rtP -> P_3306 ; } else { rtb_B_390_4132_0 =
rtb_B_390_4666_0 ; } rtb_B_390_4132_0 = 1.0 / rtb_B_390_4132_0 / 5.0e-5 ;
rtb_B_390_183_0 = muDoubleScalarCeil ( rtb_B_390_4132_0 ) ; _rtB ->
B_390_4805_0 = _rtP -> P_3307 * rtb_B_390_183_0 ; ssCallAccelRunBlock ( S ,
390 , 4806 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_390_4809_0 ) { rtb_B_390_183_0 = rtb_B_390_4132_0 - rtb_B_390_183_0 ; _rtB
-> B_390_4813_0 = ( ( _rtB -> B_390_4800_0 [ 1 ] - _rtDW ->
UnitDelay_DSTATE_h ) * rtb_B_390_183_0 * _rtP -> P_397 + _rtB -> B_390_4800_0
[ 1 ] ) * ( rtb_B_390_183_0 / rtb_B_390_4132_0 ) + ( _rtB -> B_390_4801_0 -
_rtB -> B_390_4806_0 ) * rtb_B_390_4666_0 ; } else { _rtB -> B_390_4813_0 =
_rtDW -> UnitDelay1_DSTATE_f ; } _rtB -> B_390_4814_0 = _rtB -> B_390_4813_0
* _rtB -> B_366_41_0 ; _rtDW -> DiscreteDerivative_tmp_m = ( _rtB ->
B_390_4814_0 - _rtP -> P_3320 [ 1 ] * _rtDW -> DiscreteDerivative_states_m )
/ _rtP -> P_3320 [ 0 ] ; rtb_B_390_784_0 = ( _rtP -> P_3326 * _rtB ->
B_390_4814_0 + _rtDW -> DiscreteTimeIntegrator_DSTATE_p ) + ( _rtP -> P_3319
[ 0 ] * _rtDW -> DiscreteDerivative_tmp_m + _rtP -> P_3319 [ 1 ] * _rtDW ->
DiscreteDerivative_states_m ) ; if ( rtb_B_390_784_0 > _rtP -> P_3327 ) {
_rtB -> B_390_4819_0 = _rtP -> P_3327 ; } else if ( rtb_B_390_784_0 < _rtP ->
P_3328 ) { _rtB -> B_390_4819_0 = _rtP -> P_3328 ; } else { _rtB ->
B_390_4819_0 = rtb_B_390_784_0 ; } rtb_B_390_4666_0 = _rtP -> P_3329 * _rtB
-> B_390_4819_0 ; rtb_B_390_16_0 = rtb_B_390_4666_0 - _rtDW -> PrevY_c ; if (
rtb_B_390_16_0 > _rtP -> P_3330 ) { rtb_B_390_4666_0 = _rtDW -> PrevY_c +
_rtP -> P_3330 ; } else { if ( rtb_B_390_16_0 < _rtP -> P_3331 ) {
rtb_B_390_4666_0 = _rtDW -> PrevY_c + _rtP -> P_3331 ; } } _rtDW -> PrevY_c =
rtb_B_390_4666_0 ; _rtB -> B_390_4831_0 = ( _rtP -> P_3334 * _rtDW ->
Delay_x1_DSTATE_c + _rtP -> P_3336 * _rtDW -> Delay_x2_DSTATE_ch ) + _rtP ->
P_3339 * rtb_B_390_4666_0 ; _rtB -> B_390_4833_0 = ( _rtP -> P_3337 * _rtDW
-> Delay_x1_DSTATE_c + _rtP -> P_3338 * _rtDW -> Delay_x2_DSTATE_ch ) + _rtP
-> P_3340 * rtb_B_390_4666_0 ; _rtB -> B_390_4838_0 = ( _rtP -> P_3342 *
_rtDW -> Delay_x1_DSTATE_c + _rtP -> P_3343 * _rtDW -> Delay_x2_DSTATE_ch ) +
_rtP -> P_3341 * rtb_B_390_4666_0 ; } _rtB -> B_390_4840_0 = (
muDoubleScalarSin ( _rtP -> P_3346 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3347
) * _rtP -> P_3344 + _rtP -> P_3345 ) * _rtB -> B_390_4653_0 [ 0 ] ; _rtB ->
B_390_4842_0 = ( muDoubleScalarSin ( _rtP -> P_3350 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3351 ) * _rtP -> P_3348 + _rtP -> P_3349 ) * _rtB -> B_390_4653_0
[ 0 ] ; _rtB -> B_390_4844_0 = ( muDoubleScalarSin ( _rtP -> P_3354 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3355 ) * _rtP -> P_3352 + _rtP -> P_3353
) * _rtB -> B_390_4653_0 [ 1 ] ; _rtB -> B_390_4846_0 = ( muDoubleScalarSin (
_rtP -> P_3358 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3359 ) * _rtP -> P_3356
+ _rtP -> P_3357 ) * _rtB -> B_390_4653_0 [ 1 ] ; _rtB -> B_390_4848_0 = (
muDoubleScalarSin ( _rtP -> P_3362 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3363
) * _rtP -> P_3360 + _rtP -> P_3361 ) * _rtB -> B_390_4653_0 [ 2 ] ; _rtB ->
B_390_4850_0 = ( muDoubleScalarSin ( _rtP -> P_3366 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3367 ) * _rtP -> P_3364 + _rtP -> P_3365 ) * _rtB -> B_390_4653_0
[ 2 ] ; B_390_4851_0 = _rtP -> P_3368 * rtb_B_390_4_1 ; _rtB -> B_390_4853_0
= ( muDoubleScalarSin ( _rtP -> P_3371 * ssGetTaskTime ( S , 0 ) + _rtP ->
P_3372 ) * _rtP -> P_3369 + _rtP -> P_3370 ) * _rtB -> B_390_4780_0 [ 0 ] ;
_rtB -> B_390_4855_0 = ( muDoubleScalarSin ( _rtP -> P_3375 * ssGetTaskTime (
S , 0 ) + _rtP -> P_3376 ) * _rtP -> P_3373 + _rtP -> P_3374 ) * _rtB ->
B_390_4780_0 [ 0 ] ; _rtB -> B_390_4857_0 = ( muDoubleScalarSin ( _rtP ->
P_3379 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3380 ) * _rtP -> P_3377 + _rtP
-> P_3378 ) * _rtB -> B_390_4780_0 [ 1 ] ; _rtB -> B_390_4859_0 = (
muDoubleScalarSin ( _rtP -> P_3383 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3384
) * _rtP -> P_3381 + _rtP -> P_3382 ) * _rtB -> B_390_4780_0 [ 1 ] ; _rtB ->
B_390_4861_0 = ( muDoubleScalarSin ( _rtP -> P_3387 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3388 ) * _rtP -> P_3385 + _rtP -> P_3386 ) * _rtB -> B_390_4780_0
[ 2 ] ; _rtB -> B_390_4863_0 = ( muDoubleScalarSin ( _rtP -> P_3391 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3392 ) * _rtP -> P_3389 + _rtP -> P_3390
) * _rtB -> B_390_4780_0 [ 2 ] ; B_390_4864_0 = _rtP -> P_3393 *
B_390_401_1_idx_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
B_390_4905_0 = _rtP -> P_3395 * _rtB -> B_0_14_0 [ 138 ] * ( _rtP -> P_3396 *
_rtB -> B_0_14_0 [ 83 ] ) ; _rtB -> B_390_4919_0 = _rtP -> P_3397 *
rtb_B_390_222_0 + _rtP -> P_3398 * _rtB -> B_0_15_0 ; _rtB -> B_390_4925_0 =
_rtP -> P_3399 * rtb_B_390_117_0 + _rtP -> P_3400 * _rtB -> B_0_19_0 ; _rtB
-> B_390_4933_0 = _rtP -> P_3401 * _rtB -> B_0_14_0 [ 86 ] ; _rtB ->
B_390_4937_0 = _rtP -> P_3402 * _rtB -> B_0_14_0 [ 140 ] ; _rtB ->
B_390_4941_0 = _rtP -> P_3403 * _rtB -> B_0_14_0 [ 141 ] ; if ( _rtB ->
B_0_14_1 [ 23 ] >= _rtP -> P_3405 ) { rtb_B_390_4666_0 = _rtP -> P_352 * _rtB
-> B_0_14_0 [ 23 ] ; } else { rtb_B_390_4666_0 = _rtB -> B_390_4954_0 ; } if
( rtb_B_390_4666_0 > _rtP -> P_3406 ) { B_390_4957_0 = _rtP -> P_3406 ; }
else if ( rtb_B_390_4666_0 < _rtP -> P_3407 ) { B_390_4957_0 = _rtP -> P_3407
; } else { B_390_4957_0 = rtb_B_390_4666_0 ; } B_390_4958_0 = _rtB ->
B_0_14_0 [ 23 ] + _rtB -> B_390_233_0 ; if ( _rtB -> B_0_14_1 [ 24 ] >= _rtP
-> P_3409 ) { rtb_B_390_4666_0 = _rtP -> P_353 * _rtB -> B_0_14_0 [ 24 ] ; }
else { rtb_B_390_4666_0 = _rtB -> B_390_4962_0 ; } if ( rtb_B_390_4666_0 >
_rtP -> P_3410 ) { B_390_4965_0 = _rtP -> P_3410 ; } else if (
rtb_B_390_4666_0 < _rtP -> P_3411 ) { B_390_4965_0 = _rtP -> P_3411 ; } else
{ B_390_4965_0 = rtb_B_390_4666_0 ; } B_390_4966_0 = _rtB -> B_0_14_0 [ 24 ]
+ _rtB -> B_390_234_0 ; _rtB -> B_390_4980_0 = _rtP -> P_3412 * _rtB ->
B_0_14_0 [ 84 ] ; ssCallAccelRunBlock ( S , 390 , 4981 , SS_CALL_MDL_OUTPUTS
) ; } _rtB -> B_390_4991_0 = _rtX -> integrator_CSTATE_el ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_emg . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_emg
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3414 ; B_390_4992_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_fc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fc . Last , _rtDW -> TransportDelay_IWORK_fc . Tail ,
_rtDW -> TransportDelay_IWORK_fc . Head , _rtP -> P_3415 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_4994_0 = _rtP -> P_3416 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_4994_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_4996_0 = _rtDW -> Memory_PreviousInput_jz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_4998_0 = ( _rtB -> B_390_4991_0 -
B_390_4992_0 ) * _rtP -> P_205 ; } else { _rtB -> B_390_4998_0 = _rtB ->
B_390_4996_0 ; } _rtB -> B_390_4999_0 = _rtX -> integrator_CSTATE_ax ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_db .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_db . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3419 ; B_390_5000_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_f0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f0 . Last , _rtDW -> TransportDelay_IWORK_f0 . Tail ,
_rtDW -> TransportDelay_IWORK_f0 . Head , _rtP -> P_3420 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5002_0 = _rtP -> P_3421 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5002_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5004_0 = _rtDW -> Memory_PreviousInput_cv ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5006_0 = ( _rtB -> B_390_4999_0 -
B_390_5000_0 ) * _rtP -> P_204 ; } else { _rtB -> B_390_5006_0 = _rtB ->
B_390_5004_0 ; } rtb_B_390_4666_0 = _rtP -> P_3423 * muDoubleScalarAtan2 (
_rtB -> B_390_5006_0 , _rtB -> B_390_4998_0 ) * _rtP -> P_3424 ; B_390_5011_0
. re = muDoubleScalarHypot ( _rtB -> B_390_4998_0 , _rtB -> B_390_5006_0 ) *
muDoubleScalarCos ( rtb_B_390_4666_0 ) ; B_390_5011_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_4998_0 , _rtB -> B_390_5006_0 ) *
muDoubleScalarSin ( rtb_B_390_4666_0 ) ; _rtB -> B_390_5012_0 = _rtX ->
integrator_CSTATE_pm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ip . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ip . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3426 ;
B_390_5013_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lt .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lt . Last , _rtDW ->
TransportDelay_IWORK_lt . Tail , _rtDW -> TransportDelay_IWORK_lt . Head ,
_rtP -> P_3427 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5015_0 = _rtP -> P_3428 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5015_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5017_0 =
_rtDW -> Memory_PreviousInput_lva ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5019_0 = ( _rtB -> B_390_5012_0 - B_390_5013_0 ) * _rtP -> P_207 ; }
else { _rtB -> B_390_5019_0 = _rtB -> B_390_5017_0 ; } _rtB -> B_390_5020_0 =
_rtX -> integrator_CSTATE_dy ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_es . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_es . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3431 ;
B_390_5021_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lr . Last , _rtDW ->
TransportDelay_IWORK_lr . Tail , _rtDW -> TransportDelay_IWORK_lr . Head ,
_rtP -> P_3432 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5023_0 = _rtP -> P_3433 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5023_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5025_0 =
_rtDW -> Memory_PreviousInput_el ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5027_0 = ( _rtB -> B_390_5020_0 - B_390_5021_0 ) * _rtP -> P_206 ; }
else { _rtB -> B_390_5027_0 = _rtB -> B_390_5025_0 ; } rtb_B_390_4666_0 =
_rtP -> P_3435 * muDoubleScalarAtan2 ( _rtB -> B_390_5027_0 , _rtB ->
B_390_5019_0 ) * _rtP -> P_3436 ; B_390_5032_0 . re = muDoubleScalarHypot (
_rtB -> B_390_5019_0 , _rtB -> B_390_5027_0 ) * muDoubleScalarCos (
rtb_B_390_4666_0 ) ; B_390_5032_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_5019_0 , _rtB -> B_390_5027_0 ) * muDoubleScalarSin ( rtb_B_390_4666_0
) ; _rtB -> B_390_5033_0 = _rtX -> integrator_CSTATE_kk ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3438 ; B_390_5034_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ohq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ohq . Last , _rtDW -> TransportDelay_IWORK_ohq . Tail ,
_rtDW -> TransportDelay_IWORK_ohq . Head , _rtP -> P_3439 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5036_0 = _rtP -> P_3440 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5036_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5038_0 = _rtDW -> Memory_PreviousInput_ds ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5040_0 = ( _rtB -> B_390_5033_0 -
B_390_5034_0 ) * _rtP -> P_209 ; } else { _rtB -> B_390_5040_0 = _rtB ->
B_390_5038_0 ; } _rtB -> B_390_5041_0 = _rtX -> integrator_CSTATE_pg ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kf .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kf . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3443 ; B_390_5042_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_phw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_phw . Last , _rtDW -> TransportDelay_IWORK_phw . Tail ,
_rtDW -> TransportDelay_IWORK_phw . Head , _rtP -> P_3444 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5044_0 = _rtP -> P_3445 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5044_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5046_0 = _rtDW -> Memory_PreviousInput_dgf ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5048_0 = ( _rtB -> B_390_5041_0 -
B_390_5042_0 ) * _rtP -> P_208 ; } else { _rtB -> B_390_5048_0 = _rtB ->
B_390_5046_0 ; } rtb_B_390_4666_0 = _rtP -> P_3447 * muDoubleScalarAtan2 (
_rtB -> B_390_5048_0 , _rtB -> B_390_5040_0 ) * _rtP -> P_3448 ; B_390_5053_0
. re = muDoubleScalarHypot ( _rtB -> B_390_5040_0 , _rtB -> B_390_5048_0 ) *
muDoubleScalarCos ( rtb_B_390_4666_0 ) ; B_390_5053_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_5040_0 , _rtB -> B_390_5048_0 ) *
muDoubleScalarSin ( rtb_B_390_4666_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_5055_0 = _rtB -> B_390_5054_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_5055_0 ,
B_390_5011_0 , B_390_5032_0 , B_390_5053_0 , & _rtB -> PosSeqComputation_o ,
& _rtDW -> PosSeqComputation_o , & _rtP -> PosSeqComputation_o ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5058_0 = _rtB
-> B_390_5057_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_5058_0 , B_390_5011_0 , B_390_5032_0 , B_390_5053_0 , & _rtB ->
NegSeqComputation_m1 , & _rtDW -> NegSeqComputation_m1 , & _rtP ->
NegSeqComputation_m1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_5061_0 = _rtB -> B_390_5060_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_5061_0 ,
B_390_5011_0 , B_390_5032_0 , B_390_5053_0 , & _rtB -> ZeroSeqComputation_lj
, & _rtDW -> ZeroSeqComputation_lj , & _rtP -> ZeroSeqComputation_lj ) ;
rtb_B_390_239_0 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_o .
B_22_2_0 . re , _rtB -> PosSeqComputation_o . B_22_2_0 . im ) ; rtb_B_390_4_1
= muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_o . B_22_2_0 . im , _rtB ->
PosSeqComputation_o . B_22_2_0 . re ) ; _rtB -> B_390_5064_0 = _rtX ->
integrator_CSTATE_hrp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3453 ;
B_390_5065_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_okh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_okh . Last , _rtDW ->
TransportDelay_IWORK_okh . Tail , _rtDW -> TransportDelay_IWORK_okh . Head ,
_rtP -> P_3454 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5067_0 = _rtP -> P_3455 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5067_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5069_0 =
_rtDW -> Memory_PreviousInput_peu ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5071_0 = ( _rtB -> B_390_5064_0 - B_390_5065_0 ) * _rtP -> P_211 ; }
else { _rtB -> B_390_5071_0 = _rtB -> B_390_5069_0 ; } _rtB -> B_390_5072_0 =
_rtX -> integrator_CSTATE_bg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ia . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ia . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3458 ;
B_390_5073_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ne .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ne . Last , _rtDW ->
TransportDelay_IWORK_ne . Tail , _rtDW -> TransportDelay_IWORK_ne . Head ,
_rtP -> P_3459 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5075_0 = _rtP -> P_3460 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5075_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5077_0 =
_rtDW -> Memory_PreviousInput_cu ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5079_0 = ( _rtB -> B_390_5072_0 - B_390_5073_0 ) * _rtP -> P_210 ; }
else { _rtB -> B_390_5079_0 = _rtB -> B_390_5077_0 ; } rtb_B_390_4666_0 =
_rtP -> P_3462 * muDoubleScalarAtan2 ( _rtB -> B_390_5079_0 , _rtB ->
B_390_5071_0 ) * _rtP -> P_3463 ; B_390_5084_0 . re = muDoubleScalarHypot (
_rtB -> B_390_5071_0 , _rtB -> B_390_5079_0 ) * muDoubleScalarCos (
rtb_B_390_4666_0 ) ; B_390_5084_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_5071_0 , _rtB -> B_390_5079_0 ) * muDoubleScalarSin ( rtb_B_390_4666_0
) ; _rtB -> B_390_5085_0 = _rtX -> integrator_CSTATE_dh ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3465 ; B_390_5086_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_dpr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dpr . Last , _rtDW -> TransportDelay_IWORK_dpr . Tail ,
_rtDW -> TransportDelay_IWORK_dpr . Head , _rtP -> P_3466 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5088_0 = _rtP -> P_3467 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5088_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5090_0 = _rtDW -> Memory_PreviousInput_pn ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5092_0 = ( _rtB -> B_390_5085_0 -
B_390_5086_0 ) * _rtP -> P_213 ; } else { _rtB -> B_390_5092_0 = _rtB ->
B_390_5090_0 ; } _rtB -> B_390_5093_0 = _rtX -> integrator_CSTATE_fof ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_on .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_on . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3470 ; B_390_5094_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_m1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m1 . Last , _rtDW -> TransportDelay_IWORK_m1 . Tail ,
_rtDW -> TransportDelay_IWORK_m1 . Head , _rtP -> P_3471 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5096_0 = _rtP -> P_3472 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5096_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5098_0 = _rtDW -> Memory_PreviousInput_du ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5100_0 = ( _rtB -> B_390_5093_0 -
B_390_5094_0 ) * _rtP -> P_212 ; } else { _rtB -> B_390_5100_0 = _rtB ->
B_390_5098_0 ; } rtb_B_390_4666_0 = _rtP -> P_3474 * muDoubleScalarAtan2 (
_rtB -> B_390_5100_0 , _rtB -> B_390_5092_0 ) * _rtP -> P_3475 ; B_390_5105_0
. re = muDoubleScalarHypot ( _rtB -> B_390_5092_0 , _rtB -> B_390_5100_0 ) *
muDoubleScalarCos ( rtb_B_390_4666_0 ) ; B_390_5105_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_5092_0 , _rtB -> B_390_5100_0 ) *
muDoubleScalarSin ( rtb_B_390_4666_0 ) ; _rtB -> B_390_5106_0 = _rtX ->
integrator_CSTATE_l5v ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dii . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dii . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3477 ;
B_390_5107_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hlb .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hlb . Last , _rtDW ->
TransportDelay_IWORK_hlb . Tail , _rtDW -> TransportDelay_IWORK_hlb . Head ,
_rtP -> P_3478 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5109_0 = _rtP -> P_3479 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5109_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5111_0 =
_rtDW -> Memory_PreviousInput_oc ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5113_0 = ( _rtB -> B_390_5106_0 - B_390_5107_0 ) * _rtP -> P_215 ; }
else { _rtB -> B_390_5113_0 = _rtB -> B_390_5111_0 ; } _rtB -> B_390_5114_0 =
_rtX -> integrator_CSTATE_jw ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_pi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3482 ;
B_390_5115_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ka .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ka . Last , _rtDW ->
TransportDelay_IWORK_ka . Tail , _rtDW -> TransportDelay_IWORK_ka . Head ,
_rtP -> P_3483 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5117_0 = _rtP -> P_3484 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5117_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5119_0 =
_rtDW -> Memory_PreviousInput_fr ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5121_0 = ( _rtB -> B_390_5114_0 - B_390_5115_0 ) * _rtP -> P_214 ; }
else { _rtB -> B_390_5121_0 = _rtB -> B_390_5119_0 ; } rtb_B_390_4666_0 =
_rtP -> P_3486 * muDoubleScalarAtan2 ( _rtB -> B_390_5121_0 , _rtB ->
B_390_5113_0 ) * _rtP -> P_3487 ; B_390_5126_0 . re = muDoubleScalarHypot (
_rtB -> B_390_5113_0 , _rtB -> B_390_5121_0 ) * muDoubleScalarCos (
rtb_B_390_4666_0 ) ; B_390_5126_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_5113_0 , _rtB -> B_390_5121_0 ) * muDoubleScalarSin ( rtb_B_390_4666_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5128_0 = _rtB -> B_390_5127_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation
( S , _rtB -> B_390_5128_0 , B_390_5084_0 , B_390_5105_0 , B_390_5126_0 , &
_rtB -> PosSeqComputation_g4 , & _rtDW -> PosSeqComputation_g4 , & _rtP ->
PosSeqComputation_g4 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_5131_0 = _rtB -> B_390_5130_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_5131_0 ,
B_390_5084_0 , B_390_5105_0 , B_390_5126_0 , & _rtB -> NegSeqComputation_fm ,
& _rtDW -> NegSeqComputation_fm , & _rtP -> NegSeqComputation_fm ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5134_0 = _rtB
-> B_390_5133_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_5134_0 , B_390_5084_0 , B_390_5105_0 , B_390_5126_0 , & _rtB ->
ZeroSeqComputation_cn , & _rtDW -> ZeroSeqComputation_cn , & _rtP ->
ZeroSeqComputation_cn ) ; rtb_B_390_4666_0 = rtb_B_390_239_0 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation_g4 . B_22_2_0 . re , _rtB ->
PosSeqComputation_g4 . B_22_2_0 . im ) * _rtP -> P_3491 ;
muDoubleScalarSinCos ( _rtP -> P_3494 * ( _rtP -> P_3492 * rtb_B_390_4_1 -
_rtP -> P_3493 * muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_g4 .
B_22_2_0 . im , _rtB -> PosSeqComputation_g4 . B_22_2_0 . re ) ) , &
rtb_B_390_183_0 , & rtb_B_390_117_0 ) ; _rtB -> B_390_5145_0 = _rtX ->
integrator_CSTATE_cfo ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cco . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cco . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3496 ;
B_390_5146_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lb .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lb . Last , _rtDW ->
TransportDelay_IWORK_lb . Tail , _rtDW -> TransportDelay_IWORK_lb . Head ,
_rtP -> P_3497 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5148_0 = _rtP -> P_3498 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5148_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5150_0 =
_rtDW -> Memory_PreviousInput_l0 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5152_0 = ( _rtB -> B_390_5145_0 - B_390_5146_0 ) * _rtP -> P_217 ; }
else { _rtB -> B_390_5152_0 = _rtB -> B_390_5150_0 ; } _rtB -> B_390_5153_0 =
_rtX -> integrator_CSTATE_hqe ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_bw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3501 ;
B_390_5154_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l3 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_l3 . Last , _rtDW ->
TransportDelay_IWORK_l3 . Tail , _rtDW -> TransportDelay_IWORK_l3 . Head ,
_rtP -> P_3502 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5156_0 = _rtP -> P_3503 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5156_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5158_0 =
_rtDW -> Memory_PreviousInput_he ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5160_0 = ( _rtB -> B_390_5153_0 - B_390_5154_0 ) * _rtP -> P_216 ; }
else { _rtB -> B_390_5160_0 = _rtB -> B_390_5158_0 ; } rtb_B_390_222_0 = _rtP
-> P_3505 * muDoubleScalarAtan2 ( _rtB -> B_390_5160_0 , _rtB -> B_390_5152_0
) * _rtP -> P_3506 ; B_390_5165_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_5152_0 , _rtB -> B_390_5160_0 ) * muDoubleScalarCos ( rtb_B_390_222_0 )
; B_390_5165_0 . im = muDoubleScalarHypot ( _rtB -> B_390_5152_0 , _rtB ->
B_390_5160_0 ) * muDoubleScalarSin ( rtb_B_390_222_0 ) ; _rtB -> B_390_5166_0
= _rtX -> integrator_CSTATE_nt ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ab . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ab . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3508
; B_390_5167_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gp .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gp . Last , _rtDW ->
TransportDelay_IWORK_gp . Tail , _rtDW -> TransportDelay_IWORK_gp . Head ,
_rtP -> P_3509 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5169_0 = _rtP -> P_3510 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5169_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5171_0 =
_rtDW -> Memory_PreviousInput_ph ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5173_0 = ( _rtB -> B_390_5166_0 - B_390_5167_0 ) * _rtP -> P_219 ; }
else { _rtB -> B_390_5173_0 = _rtB -> B_390_5171_0 ; } _rtB -> B_390_5174_0 =
_rtX -> integrator_CSTATE_hqx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mwe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3513
; B_390_5175_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_f5 . Last , _rtDW ->
TransportDelay_IWORK_f5 . Tail , _rtDW -> TransportDelay_IWORK_f5 . Head ,
_rtP -> P_3514 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5177_0 = _rtP -> P_3515 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5177_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5179_0 =
_rtDW -> Memory_PreviousInput_gy ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5181_0 = ( _rtB -> B_390_5174_0 - B_390_5175_0 ) * _rtP -> P_218 ; }
else { _rtB -> B_390_5181_0 = _rtB -> B_390_5179_0 ; } rtb_B_390_222_0 = _rtP
-> P_3517 * muDoubleScalarAtan2 ( _rtB -> B_390_5181_0 , _rtB -> B_390_5173_0
) * _rtP -> P_3518 ; B_390_5186_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_5173_0 , _rtB -> B_390_5181_0 ) * muDoubleScalarCos ( rtb_B_390_222_0 )
; B_390_5186_0 . im = muDoubleScalarHypot ( _rtB -> B_390_5173_0 , _rtB ->
B_390_5181_0 ) * muDoubleScalarSin ( rtb_B_390_222_0 ) ; _rtB -> B_390_5187_0
= _rtX -> integrator_CSTATE_gq ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3520
; B_390_5188_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ha .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ha . Last , _rtDW ->
TransportDelay_IWORK_ha . Tail , _rtDW -> TransportDelay_IWORK_ha . Head ,
_rtP -> P_3521 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5190_0 = _rtP -> P_3522 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5190_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5192_0 =
_rtDW -> Memory_PreviousInput_jg ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5194_0 = ( _rtB -> B_390_5187_0 - B_390_5188_0 ) * _rtP -> P_221 ; }
else { _rtB -> B_390_5194_0 = _rtB -> B_390_5192_0 ; } _rtB -> B_390_5195_0 =
_rtX -> integrator_CSTATE_cvp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ky . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ky . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3525 ;
B_390_5196_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bm . Last , _rtDW ->
TransportDelay_IWORK_bm . Tail , _rtDW -> TransportDelay_IWORK_bm . Head ,
_rtP -> P_3526 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5198_0 = _rtP -> P_3527 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5198_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5200_0 =
_rtDW -> Memory_PreviousInput_ei ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5202_0 = ( _rtB -> B_390_5195_0 - B_390_5196_0 ) * _rtP -> P_220 ; }
else { _rtB -> B_390_5202_0 = _rtB -> B_390_5200_0 ; } rtb_B_390_222_0 = _rtP
-> P_3529 * muDoubleScalarAtan2 ( _rtB -> B_390_5202_0 , _rtB -> B_390_5194_0
) * _rtP -> P_3530 ; B_390_5207_0 . re = muDoubleScalarHypot ( _rtB ->
B_390_5194_0 , _rtB -> B_390_5202_0 ) * muDoubleScalarCos ( rtb_B_390_222_0 )
; B_390_5207_0 . im = muDoubleScalarHypot ( _rtB -> B_390_5194_0 , _rtB ->
B_390_5202_0 ) * muDoubleScalarSin ( rtb_B_390_222_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5209_0 = _rtB
-> B_390_5208_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_5209_0 , B_390_5165_0 , B_390_5186_0 , B_390_5207_0 , & _rtB ->
PosSeqComputation_ao , & _rtDW -> PosSeqComputation_ao , & _rtP ->
PosSeqComputation_ao ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_5212_0 = _rtB -> B_390_5211_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_5212_0 ,
B_390_5165_0 , B_390_5186_0 , B_390_5207_0 , & _rtB -> NegSeqComputation_kc ,
& _rtDW -> NegSeqComputation_kc , & _rtP -> NegSeqComputation_kc ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5215_0 = _rtB
-> B_390_5214_0 ; } MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB ->
B_390_5215_0 , B_390_5165_0 , B_390_5186_0 , B_390_5207_0 , & _rtB ->
ZeroSeqComputation_oz , & _rtDW -> ZeroSeqComputation_oz , & _rtP ->
ZeroSeqComputation_oz ) ; rtb_B_390_4_1 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ao . B_22_2_0 . im , _rtB -> PosSeqComputation_ao .
B_22_2_0 . re ) ; rtb_B_390_222_0 = _rtP -> P_3534 * muDoubleScalarHypot (
_rtB -> PosSeqComputation_ao . B_22_2_0 . re , _rtB -> PosSeqComputation_ao .
B_22_2_0 . im ) ; _rtB -> B_390_5220_0 = _rtX -> integrator_CSTATE_cs ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bro .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bro . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3536 ; B_390_5221_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_f4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f4 . Last , _rtDW -> TransportDelay_IWORK_f4 . Tail ,
_rtDW -> TransportDelay_IWORK_f4 . Head , _rtP -> P_3537 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5223_0 = _rtP -> P_3538 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5223_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5225_0 = _rtDW -> Memory_PreviousInput_phq ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5227_0 = ( _rtB -> B_390_5220_0 -
B_390_5221_0 ) * _rtP -> P_199 ; } else { _rtB -> B_390_5227_0 = _rtB ->
B_390_5225_0 ; } _rtB -> B_390_5228_0 = _rtX -> integrator_CSTATE_oh ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fn .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fn . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3541 ; B_390_5229_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_kg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kg . Last , _rtDW -> TransportDelay_IWORK_kg . Tail ,
_rtDW -> TransportDelay_IWORK_kg . Head , _rtP -> P_3542 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5231_0 = _rtP -> P_3543 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5231_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5233_0 = _rtDW -> Memory_PreviousInput_lkz ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5235_0 = ( _rtB -> B_390_5228_0 -
B_390_5229_0 ) * _rtP -> P_198 ; } else { _rtB -> B_390_5235_0 = _rtB ->
B_390_5233_0 ; } rtb_B_390_4132_0 = _rtP -> P_3545 * muDoubleScalarAtan2 (
_rtB -> B_390_5235_0 , _rtB -> B_390_5227_0 ) * _rtP -> P_3546 ; B_390_5240_0
. re = muDoubleScalarHypot ( _rtB -> B_390_5227_0 , _rtB -> B_390_5235_0 ) *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; B_390_5240_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_5227_0 , _rtB -> B_390_5235_0 ) *
muDoubleScalarSin ( rtb_B_390_4132_0 ) ; _rtB -> B_390_5241_0 = _rtX ->
integrator_CSTATE_mi ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dy4 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dy4 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3548 ;
B_390_5242_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_o4 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_o4 . Last , _rtDW ->
TransportDelay_IWORK_o4 . Tail , _rtDW -> TransportDelay_IWORK_o4 . Head ,
_rtP -> P_3549 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5244_0 = _rtP -> P_3550 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5244_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5246_0 =
_rtDW -> Memory_PreviousInput_po ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5248_0 = ( _rtB -> B_390_5241_0 - B_390_5242_0 ) * _rtP -> P_201 ; }
else { _rtB -> B_390_5248_0 = _rtB -> B_390_5246_0 ; } _rtB -> B_390_5249_0 =
_rtX -> integrator_CSTATE_m4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_m5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3553 ;
B_390_5250_0 = MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bck .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bck . Last , _rtDW ->
TransportDelay_IWORK_bck . Tail , _rtDW -> TransportDelay_IWORK_bck . Head ,
_rtP -> P_3554 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5252_0 = _rtP -> P_3555 ; }
rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB -> B_390_5252_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5254_0 =
_rtDW -> Memory_PreviousInput_o1 ; } if ( rtb_B_390_259_0 ) { _rtB ->
B_390_5256_0 = ( _rtB -> B_390_5249_0 - B_390_5250_0 ) * _rtP -> P_200 ; }
else { _rtB -> B_390_5256_0 = _rtB -> B_390_5254_0 ; } rtb_B_390_4132_0 =
_rtP -> P_3557 * muDoubleScalarAtan2 ( _rtB -> B_390_5256_0 , _rtB ->
B_390_5248_0 ) * _rtP -> P_3558 ; B_390_5261_0 . re = muDoubleScalarHypot (
_rtB -> B_390_5248_0 , _rtB -> B_390_5256_0 ) * muDoubleScalarCos (
rtb_B_390_4132_0 ) ; B_390_5261_0 . im = muDoubleScalarHypot ( _rtB ->
B_390_5248_0 , _rtB -> B_390_5256_0 ) * muDoubleScalarSin ( rtb_B_390_4132_0
) ; _rtB -> B_390_5262_0 = _rtX -> integrator_CSTATE_g2y ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3560 ; B_390_5263_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_prl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_prl . Last , _rtDW -> TransportDelay_IWORK_prl . Tail ,
_rtDW -> TransportDelay_IWORK_prl . Head , _rtP -> P_3561 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5265_0 = _rtP -> P_3562 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5265_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5267_0 = _rtDW -> Memory_PreviousInput_bt ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5269_0 = ( _rtB -> B_390_5262_0 -
B_390_5263_0 ) * _rtP -> P_203 ; } else { _rtB -> B_390_5269_0 = _rtB ->
B_390_5267_0 ; } _rtB -> B_390_5270_0 = _rtX -> integrator_CSTATE_kf0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_acd .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_acd . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3565 ; B_390_5271_0 =
MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , * tBuffer ,
* uBuffer , _rtDW -> TransportDelay_IWORK_ca . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ca . Last , _rtDW -> TransportDelay_IWORK_ca . Tail ,
_rtDW -> TransportDelay_IWORK_ca . Head , _rtP -> P_3566 , 0 , ( boolean_T )
( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) )
) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_390_5273_0 = _rtP -> P_3567 ; } rtb_B_390_259_0 = ( ssGetT ( S ) >= _rtB ->
B_390_5273_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_390_5275_0 = _rtDW -> Memory_PreviousInput_h4 ; } if (
rtb_B_390_259_0 ) { _rtB -> B_390_5277_0 = ( _rtB -> B_390_5270_0 -
B_390_5271_0 ) * _rtP -> P_202 ; } else { _rtB -> B_390_5277_0 = _rtB ->
B_390_5275_0 ; } rtb_B_390_4132_0 = _rtP -> P_3569 * muDoubleScalarAtan2 (
_rtB -> B_390_5277_0 , _rtB -> B_390_5269_0 ) * _rtP -> P_3570 ; B_390_5282_0
. re = muDoubleScalarHypot ( _rtB -> B_390_5269_0 , _rtB -> B_390_5277_0 ) *
muDoubleScalarCos ( rtb_B_390_4132_0 ) ; B_390_5282_0 . im =
muDoubleScalarHypot ( _rtB -> B_390_5269_0 , _rtB -> B_390_5277_0 ) *
muDoubleScalarSin ( rtb_B_390_4132_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_390_5284_0 = _rtB -> B_390_5283_0 ; }
MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB -> B_390_5284_0 ,
B_390_5240_0 , B_390_5261_0 , B_390_5282_0 , & _rtB -> PosSeqComputation_ow ,
& _rtDW -> PosSeqComputation_ow , & _rtP -> PosSeqComputation_ow ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_390_5287_0 = _rtB
-> B_390_5286_0 ; } MR_CIGRE_RESISTIVO_NegSeqComputation ( S , _rtB ->
B_390_5287_0 , B_390_5240_0 , B_390_5261_0 , B_390_5282_0 , & _rtB ->
NegSeqComputation_g5 , & _rtDW -> NegSeqComputation_g5 , & _rtP ->
NegSeqComputation_g5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_390_5290_0 = _rtB -> B_390_5289_0 ; }
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( S , _rtB -> B_390_5290_0 ,
B_390_5240_0 , B_390_5261_0 , B_390_5282_0 , & _rtB -> ZeroSeqComputation_ko
, & _rtDW -> ZeroSeqComputation_ko , & _rtP -> ZeroSeqComputation_ko ) ;
B_390_401_1_idx_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ow .
B_22_2_0 . im , _rtB -> PosSeqComputation_ow . B_22_2_0 . re ) ; _rtB ->
B_390_1508_0 [ 0 ] = rtb_B_390_4666_0 * rtb_B_390_117_0 ; _rtB ->
B_390_1508_0 [ 1 ] = rtb_B_390_222_0 ; _rtB -> B_390_1508_0 [ 2 ] =
rtb_B_390_4666_0 * rtb_B_390_183_0 ; _rtB -> B_390_1508_0 [ 3 ] = _rtP ->
P_3574 * muDoubleScalarHypot ( _rtB -> PosSeqComputation_ow . B_22_2_0 . re ,
_rtB -> PosSeqComputation_ow . B_22_2_0 . im ) ; ssCallAccelRunBlock ( S ,
390 , 5295 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_390_5299_0 [ 0 ] = _rtP -> P_3575 * _rtB ->
B_0_14_0 [ 114 ] * _rtP -> P_3578 ; _rtB -> B_390_5299_0 [ 1 ] = _rtP ->
P_3576 * _rtB -> B_0_14_0 [ 115 ] * _rtP -> P_3578 ; _rtB -> B_390_5299_0 [ 2
] = _rtP -> P_3577 * _rtB -> B_0_14_0 [ 116 ] * _rtP -> P_3578 ; _rtB ->
B_390_5303_0 [ 0 ] = _rtP -> P_3579 * _rtB -> B_0_14_0 [ 59 ] * _rtP ->
P_3582 ; _rtB -> B_390_5303_0 [ 1 ] = _rtP -> P_3580 * _rtB -> B_0_14_0 [ 60
] * _rtP -> P_3582 ; _rtB -> B_390_5303_0 [ 2 ] = _rtP -> P_3581 * _rtB ->
B_0_14_0 [ 61 ] * _rtP -> P_3582 ; } _rtB -> B_390_5339_0 = (
muDoubleScalarSin ( _rtP -> P_3585 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3586
) * _rtP -> P_3583 + _rtP -> P_3584 ) * _rtB -> B_390_5299_0 [ 0 ] ; _rtB ->
B_390_5341_0 = ( muDoubleScalarSin ( _rtP -> P_3589 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3590 ) * _rtP -> P_3587 + _rtP -> P_3588 ) * _rtB -> B_390_5299_0
[ 0 ] ; _rtB -> B_390_5343_0 = ( muDoubleScalarSin ( _rtP -> P_3593 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3594 ) * _rtP -> P_3591 + _rtP -> P_3592
) * _rtB -> B_390_5299_0 [ 1 ] ; _rtB -> B_390_5345_0 = ( muDoubleScalarSin (
_rtP -> P_3597 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3598 ) * _rtP -> P_3595
+ _rtP -> P_3596 ) * _rtB -> B_390_5299_0 [ 1 ] ; _rtB -> B_390_5347_0 = (
muDoubleScalarSin ( _rtP -> P_3601 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3602
) * _rtP -> P_3599 + _rtP -> P_3600 ) * _rtB -> B_390_5299_0 [ 2 ] ; _rtB ->
B_390_5349_0 = ( muDoubleScalarSin ( _rtP -> P_3605 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3606 ) * _rtP -> P_3603 + _rtP -> P_3604 ) * _rtB -> B_390_5299_0
[ 2 ] ; B_390_5350_0 = _rtP -> P_3607 * B_390_401_1_idx_0 ; _rtB ->
B_390_5352_0 = ( muDoubleScalarSin ( _rtP -> P_3610 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3611 ) * _rtP -> P_3608 + _rtP -> P_3609 ) * _rtB -> B_390_5303_0
[ 0 ] ; _rtB -> B_390_5354_0 = ( muDoubleScalarSin ( _rtP -> P_3614 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3615 ) * _rtP -> P_3612 + _rtP -> P_3613
) * _rtB -> B_390_5303_0 [ 0 ] ; _rtB -> B_390_5356_0 = ( muDoubleScalarSin (
_rtP -> P_3618 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3619 ) * _rtP -> P_3616
+ _rtP -> P_3617 ) * _rtB -> B_390_5303_0 [ 1 ] ; _rtB -> B_390_5358_0 = (
muDoubleScalarSin ( _rtP -> P_3622 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3623
) * _rtP -> P_3620 + _rtP -> P_3621 ) * _rtB -> B_390_5303_0 [ 1 ] ; _rtB ->
B_390_5360_0 = ( muDoubleScalarSin ( _rtP -> P_3626 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3627 ) * _rtP -> P_3624 + _rtP -> P_3625 ) * _rtB -> B_390_5303_0
[ 2 ] ; _rtB -> B_390_5362_0 = ( muDoubleScalarSin ( _rtP -> P_3630 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3631 ) * _rtP -> P_3628 + _rtP -> P_3629
) * _rtB -> B_390_5303_0 [ 2 ] ; _rtB -> B_390_5364_0 = ( muDoubleScalarSin (
_rtP -> P_3634 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3635 ) * _rtP -> P_3632
+ _rtP -> P_3633 ) * _rtB -> B_390_5299_0 [ 0 ] ; _rtB -> B_390_5366_0 = (
muDoubleScalarSin ( _rtP -> P_3638 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3639
) * _rtP -> P_3636 + _rtP -> P_3637 ) * _rtB -> B_390_5299_0 [ 0 ] ; _rtB ->
B_390_5368_0 = ( muDoubleScalarSin ( _rtP -> P_3642 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3643 ) * _rtP -> P_3640 + _rtP -> P_3641 ) * _rtB -> B_390_5299_0
[ 1 ] ; _rtB -> B_390_5370_0 = ( muDoubleScalarSin ( _rtP -> P_3646 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3647 ) * _rtP -> P_3644 + _rtP -> P_3645
) * _rtB -> B_390_5299_0 [ 1 ] ; _rtB -> B_390_5372_0 = ( muDoubleScalarSin (
_rtP -> P_3650 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3651 ) * _rtP -> P_3648
+ _rtP -> P_3649 ) * _rtB -> B_390_5299_0 [ 2 ] ; _rtB -> B_390_5374_0 = (
muDoubleScalarSin ( _rtP -> P_3654 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3655
) * _rtP -> P_3652 + _rtP -> P_3653 ) * _rtB -> B_390_5299_0 [ 2 ] ; _rtB ->
B_390_5376_0 = ( muDoubleScalarSin ( _rtP -> P_3658 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3659 ) * _rtP -> P_3656 + _rtP -> P_3657 ) * _rtB -> B_390_5303_0
[ 0 ] ; _rtB -> B_390_5378_0 = ( muDoubleScalarSin ( _rtP -> P_3662 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3663 ) * _rtP -> P_3660 + _rtP -> P_3661
) * _rtB -> B_390_5303_0 [ 0 ] ; _rtB -> B_390_5380_0 = ( muDoubleScalarSin (
_rtP -> P_3666 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3667 ) * _rtP -> P_3664
+ _rtP -> P_3665 ) * _rtB -> B_390_5303_0 [ 1 ] ; _rtB -> B_390_5382_0 = (
muDoubleScalarSin ( _rtP -> P_3670 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3671
) * _rtP -> P_3668 + _rtP -> P_3669 ) * _rtB -> B_390_5303_0 [ 1 ] ; _rtB ->
B_390_5384_0 = ( muDoubleScalarSin ( _rtP -> P_3674 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3675 ) * _rtP -> P_3672 + _rtP -> P_3673 ) * _rtB -> B_390_5303_0
[ 2 ] ; _rtB -> B_390_5386_0 = ( muDoubleScalarSin ( _rtP -> P_3678 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3679 ) * _rtP -> P_3676 + _rtP -> P_3677
) * _rtB -> B_390_5303_0 [ 2 ] ; B_390_5387_0 = _rtP -> P_3680 *
rtb_B_390_4_1 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { if
( _rtB -> B_390_5390_0 >= _rtP -> P_3686 ) { rtb_B_390_5393_0 = _rtB ->
B_390_5389_0 ; } else { rtb_B_390_5393_0 = rt_Lookup ( _rtP -> P_3684 , 6 ,
ssGetTaskTime ( S , 2 ) , _rtP -> P_3685 ) ; } if ( _rtB -> B_390_5394_0 >=
_rtP -> P_3689 ) { _rtB -> B_390_5397_0 = rtb_B_390_5393_0 ; } else { _rtB ->
B_390_5397_0 = _rtB -> B_390_5395_0 ; } } _rtB -> B_390_5399_0 = rt_Lookup (
_rtP -> P_3690 , 5 , ssGetT ( S ) , _rtP -> P_3691 ) ; if ( _rtB ->
B_390_5388_0 >= _rtP -> P_3692 ) { _rtB -> B_390_5400_0 = _rtB ->
B_390_5397_0 ; } else { _rtB -> B_390_5400_0 = _rtB -> B_390_5399_0 ; } if (
( _rtDW -> TimeStampA_lh >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ns >=
ssGetT ( S ) ) ) { _rtB -> B_390_5401_0 = 0.0 ; } else { rtb_B_390_422_0 =
_rtDW -> TimeStampA_lh ; lastU = & _rtDW -> LastUAtTimeA_n ; if ( _rtDW ->
TimeStampA_lh < _rtDW -> TimeStampB_ns ) { if ( _rtDW -> TimeStampB_ns <
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_ns ; lastU = & _rtDW
-> LastUAtTimeB_k ; } } else { if ( _rtDW -> TimeStampA_lh >= ssGetT ( S ) )
{ rtb_B_390_422_0 = _rtDW -> TimeStampB_ns ; lastU = & _rtDW ->
LastUAtTimeB_k ; } } _rtB -> B_390_5401_0 = ( _rtB -> B_390_5399_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_390_5407_0 >= _rtP -> P_3696 ) { _rtB
-> B_390_5409_0 = rtb_B_390_5393_0 ; } else { _rtB -> B_390_5409_0 = _rtB ->
B_390_5395_0 ; } } _rtB -> B_390_5411_0 = rt_Lookup ( _rtP -> P_3697 , 5 ,
ssGetT ( S ) , _rtP -> P_3698 ) ; if ( _rtB -> B_390_5406_0 >= _rtP -> P_3699
) { _rtB -> B_390_5412_0 = _rtB -> B_390_5409_0 ; } else { _rtB ->
B_390_5412_0 = _rtB -> B_390_5411_0 ; } if ( ( _rtDW -> TimeStampA_f >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_o >= ssGetT ( S ) ) ) { _rtB ->
B_390_5413_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_f ; lastU
= & _rtDW -> LastUAtTimeA_i ; if ( _rtDW -> TimeStampA_f < _rtDW ->
TimeStampB_o ) { if ( _rtDW -> TimeStampB_o < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_o ; lastU = & _rtDW -> LastUAtTimeB_ok
; } } else { if ( _rtDW -> TimeStampA_f >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_o ; lastU = & _rtDW -> LastUAtTimeB_ok ; } } _rtB ->
B_390_5413_0 = ( _rtB -> B_390_5411_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_5419_0 >= _rtP -> P_3703 ) { _rtB -> B_390_5421_0 =
rtb_B_390_5393_0 ; } else { _rtB -> B_390_5421_0 = _rtB -> B_390_5395_0 ; } }
_rtB -> B_390_5423_0 = rt_Lookup ( _rtP -> P_3704 , 5 , ssGetT ( S ) , _rtP
-> P_3705 ) ; if ( _rtB -> B_390_5418_0 >= _rtP -> P_3706 ) { _rtB ->
B_390_5424_0 = _rtB -> B_390_5421_0 ; } else { _rtB -> B_390_5424_0 = _rtB ->
B_390_5423_0 ; } if ( ( _rtDW -> TimeStampA_c >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_kd >= ssGetT ( S ) ) ) { _rtB -> B_390_5425_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_c ; lastU = & _rtDW -> LastUAtTimeA_ej
; if ( _rtDW -> TimeStampA_c < _rtDW -> TimeStampB_kd ) { if ( _rtDW ->
TimeStampB_kd < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_kd ;
lastU = & _rtDW -> LastUAtTimeB_j ; } } else { if ( _rtDW -> TimeStampA_c >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_kd ; lastU = & _rtDW
-> LastUAtTimeB_j ; } } _rtB -> B_390_5425_0 = ( _rtB -> B_390_5423_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_5444_0 >= _rtP -> P_3713 ) {
rtb_B_390_5447_0 = _rtB -> B_390_5443_0 ; } else { rtb_B_390_5447_0 =
rt_Lookup ( _rtP -> P_3711 , 4 , ssGetTaskTime ( S , 2 ) , _rtP -> P_3712 ) ;
} if ( _rtB -> B_390_5448_0 >= _rtP -> P_3716 ) { _rtB -> B_390_5451_0 =
rtb_B_390_5447_0 ; } else { _rtB -> B_390_5451_0 = _rtB -> B_390_5449_0 ; } }
_rtB -> B_390_5453_0 = rt_Lookup ( _rtP -> P_3717 , 5 , ssGetT ( S ) , _rtP
-> P_3718 ) ; if ( _rtB -> B_390_5442_0 >= _rtP -> P_3719 ) { _rtB ->
B_390_5454_0 = _rtB -> B_390_5451_0 ; } else { _rtB -> B_390_5454_0 = _rtB ->
B_390_5453_0 ; } if ( ( _rtDW -> TimeStampA_d >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_p0 >= ssGetT ( S ) ) ) { _rtB -> B_390_5455_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_d ; lastU = & _rtDW -> LastUAtTimeA_cs
; if ( _rtDW -> TimeStampA_d < _rtDW -> TimeStampB_p0 ) { if ( _rtDW ->
TimeStampB_p0 < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_p0 ;
lastU = & _rtDW -> LastUAtTimeB_ec ; } } else { if ( _rtDW -> TimeStampA_d >=
ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_p0 ; lastU = & _rtDW
-> LastUAtTimeB_ec ; } } _rtB -> B_390_5455_0 = ( _rtB -> B_390_5453_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_390_5461_0 >= _rtP -> P_3723 ) {
_rtB -> B_390_5463_0 = rtb_B_390_5447_0 ; } else { _rtB -> B_390_5463_0 =
_rtB -> B_390_5449_0 ; } } _rtB -> B_390_5465_0 = rt_Lookup ( _rtP -> P_3724
, 5 , ssGetT ( S ) , _rtP -> P_3725 ) ; if ( _rtB -> B_390_5460_0 >= _rtP ->
P_3726 ) { _rtB -> B_390_5466_0 = _rtB -> B_390_5463_0 ; } else { _rtB ->
B_390_5466_0 = _rtB -> B_390_5465_0 ; } if ( ( _rtDW -> TimeStampA_hs >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_h >= ssGetT ( S ) ) ) { _rtB ->
B_390_5467_0 = 0.0 ; } else { rtb_B_390_422_0 = _rtDW -> TimeStampA_hs ;
lastU = & _rtDW -> LastUAtTimeA_cq ; if ( _rtDW -> TimeStampA_hs < _rtDW ->
TimeStampB_h ) { if ( _rtDW -> TimeStampB_h < ssGetT ( S ) ) {
rtb_B_390_422_0 = _rtDW -> TimeStampB_h ; lastU = & _rtDW -> LastUAtTimeB_f ;
} } else { if ( _rtDW -> TimeStampA_hs >= ssGetT ( S ) ) { rtb_B_390_422_0 =
_rtDW -> TimeStampB_h ; lastU = & _rtDW -> LastUAtTimeB_f ; } } _rtB ->
B_390_5467_0 = ( _rtB -> B_390_5465_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_390_5473_0 >= _rtP -> P_3730 ) { _rtB -> B_390_5475_0 =
rtb_B_390_5447_0 ; } else { _rtB -> B_390_5475_0 = _rtB -> B_390_5449_0 ; } }
_rtB -> B_390_5477_0 = rt_Lookup ( _rtP -> P_3731 , 5 , ssGetT ( S ) , _rtP
-> P_3732 ) ; if ( _rtB -> B_390_5472_0 >= _rtP -> P_3733 ) { _rtB ->
B_390_5478_0 = _rtB -> B_390_5475_0 ; } else { _rtB -> B_390_5478_0 = _rtB ->
B_390_5477_0 ; } if ( ( _rtDW -> TimeStampA_if >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_cy >= ssGetT ( S ) ) ) { _rtB -> B_390_5479_0 = 0.0 ; } else {
rtb_B_390_422_0 = _rtDW -> TimeStampA_if ; lastU = & _rtDW -> LastUAtTimeA_a
; if ( _rtDW -> TimeStampA_if < _rtDW -> TimeStampB_cy ) { if ( _rtDW ->
TimeStampB_cy < ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_cy ;
lastU = & _rtDW -> LastUAtTimeB_hd ; } } else { if ( _rtDW -> TimeStampA_if
>= ssGetT ( S ) ) { rtb_B_390_422_0 = _rtDW -> TimeStampB_cy ; lastU = &
_rtDW -> LastUAtTimeB_hd ; } } _rtB -> B_390_5479_0 = ( _rtB -> B_390_5477_0
- * lastU ) / ( ssGetT ( S ) - rtb_B_390_422_0 ) ; } isHit = ssIsSampleHit (
S , 2 , 0 ) ; if ( isHit != 0 ) { rtb_B_390_5393_0 = _rtP -> P_3735 * _rtB ->
B_0_14_0 [ 29 ] ; rtb_B_390_5447_0 = _rtP -> P_3736 * _rtB -> B_0_14_0 [ 30 ]
; rtb_B_390_4666_0 = _rtP -> P_3737 * _rtB -> B_0_14_0 [ 31 ] ; B_390_5494_0
[ 0 ] = _rtP -> P_3738 * rtb_B_390_5393_0 ; B_390_5494_0 [ 1 ] = _rtP ->
P_3738 * rtb_B_390_5447_0 ; B_390_5494_0 [ 2 ] = _rtP -> P_3738 *
rtb_B_390_4666_0 ; B_390_5495_0 [ 0 ] = _rtP -> P_3739 * rtb_B_390_5393_0 ;
B_390_5495_0 [ 1 ] = _rtP -> P_3739 * rtb_B_390_5447_0 ; B_390_5495_0 [ 2 ] =
_rtP -> P_3739 * rtb_B_390_4666_0 ; } UNUSED_PARAMETER ( tid ) ; } static
void mdlOutputsTID4 ( SimStruct * S , int_T tid ) { real_T rtb_B_390_156_0 ;
real_T rtb_B_390_178_0 ; B_MR_CIGRE_RESISTIVO_T * _rtB ;
P_MR_CIGRE_RESISTIVO_T * _rtP ; DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( (
DW_MR_CIGRE_RESISTIVO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_RESISTIVO_T * ) _ssGetModelBlockIO ( S ) ) ; memcpy ( & _rtB ->
B_390_0_0 [ 0 ] , & _rtP -> P_410 [ 0 ] , 25U * sizeof ( real_T ) ) ; _rtB ->
B_390_13_0 = _rtP -> P_420 ; _rtB -> B_390_14_0 = _rtP -> P_421 ; _rtB ->
B_390_22_0 = ( uint8_T ) ( _rtP -> P_431 == _rtP -> P_432 ) ; _rtB ->
B_390_26_0 = ( uint8_T ) ( _rtP -> P_431 == _rtP -> P_433 ) ; _rtB ->
B_390_30_0 = _rtP -> P_434 ; _rtB -> B_390_51_0 = _rtP -> P_448 ; _rtB ->
B_390_57_0 = ( uint8_T ) ( _rtP -> P_454 == _rtP -> P_455 ) ; _rtB ->
B_390_61_0 = ( uint8_T ) ( _rtP -> P_454 == _rtP -> P_456 ) ; _rtB ->
B_390_65_0 = _rtP -> P_457 ; _rtB -> B_390_82_0 = ( uint8_T ) ( _rtP -> P_466
== _rtP -> P_467 ) ; _rtB -> B_390_86_0 = ( uint8_T ) ( _rtP -> P_466 == _rtP
-> P_468 ) ; _rtB -> B_390_106_0 = ( uint8_T ) ( _rtP -> P_477 == _rtP ->
P_478 ) ; _rtB -> B_390_110_0 = ( uint8_T ) ( _rtP -> P_477 == _rtP -> P_479
) ; _rtB -> B_390_122_0 = _rtP -> P_484 ; _rtB -> B_390_123_0 = _rtP -> P_485
; _rtB -> B_390_124_0 = _rtP -> P_486 ; _rtB -> B_390_127_0 = _rtP -> P_489 ;
_rtB -> B_390_131_0 = ( 0.0 <= _rtP -> P_490 ) ; _rtB -> B_390_139_0 = _rtP
-> P_495 ; _rtB -> B_390_142_0 = _rtP -> P_498 ; _rtB -> B_390_146_0 = ( 0.0
<= _rtP -> P_499 ) ; rtb_B_390_156_0 = _rtP -> P_504 + _rtP -> P_505 ; _rtB
-> B_390_164_0 = _rtP -> P_511 * _rtP -> P_510 ; _rtB -> B_390_165_0 = ( (
rtb_B_390_156_0 - _rtP -> P_506 ) * _rtP -> P_507 + _rtP -> P_508 ) * _rtP ->
P_509 * _rtB -> B_390_164_0 ; rtb_B_390_178_0 = rtb_B_390_156_0 / _rtP ->
P_518 ; _rtB -> B_390_181_0 = muDoubleScalarExp ( _rtP -> P_512 - ( (
rtb_B_390_156_0 - _rtP -> P_514 ) * _rtP -> P_515 + _rtP -> P_513 ) * _rtP ->
P_516 / ( _rtP -> P_517 * rtb_B_390_156_0 ) ) * ( _rtP -> P_519 *
muDoubleScalarPower ( rtb_B_390_178_0 , 3.0 ) ) ; _rtB -> B_390_182_0 = _rtP
-> P_520 * rtb_B_390_178_0 ; _rtB -> B_390_187_0 = _rtP -> P_525 ; _rtB ->
B_390_188_0 = _rtP -> P_526 ; _rtB -> B_390_189_0 = _rtP -> P_527 ; _rtB ->
B_390_190_0 = _rtP -> P_528 ; rtb_B_390_156_0 = _rtP -> P_529 + _rtP -> P_530
; _rtB -> B_390_201_0 = _rtP -> P_536 * _rtP -> P_535 ; _rtB -> B_390_202_0 =
( ( rtb_B_390_156_0 - _rtP -> P_531 ) * _rtP -> P_532 + _rtP -> P_533 ) *
_rtP -> P_534 * _rtB -> B_390_201_0 ; rtb_B_390_178_0 = rtb_B_390_156_0 /
_rtP -> P_543 ; _rtB -> B_390_218_0 = muDoubleScalarExp ( _rtP -> P_537 - ( (
rtb_B_390_156_0 - _rtP -> P_539 ) * _rtP -> P_540 + _rtP -> P_538 ) * _rtP ->
P_541 / ( _rtP -> P_542 * rtb_B_390_156_0 ) ) * ( _rtP -> P_544 *
muDoubleScalarPower ( rtb_B_390_178_0 , 3.0 ) ) ; _rtB -> B_390_219_0 = _rtP
-> P_545 * rtb_B_390_178_0 ; _rtB -> B_390_224_0 = _rtP -> P_550 ; _rtB ->
B_390_225_0 = _rtP -> P_551 ; _rtB -> B_390_226_0 = _rtP -> P_552 ; _rtB ->
B_390_227_0 = _rtP -> P_553 ; _rtB -> B_390_231_0 = _rtP -> P_575 ; _rtB ->
B_390_232_0 = _rtP -> P_576 ; _rtB -> B_390_233_0 = _rtP -> P_577 ; _rtB ->
B_390_234_0 = _rtP -> P_578 ; _rtB -> B_390_238_0 = _rtP -> P_582 ; _rtB ->
B_390_242_0 = _rtP -> P_588 ; _rtB -> B_390_318_0 = _rtP -> P_636 ; _rtB ->
B_390_321_0 = _rtP -> P_637 ; _rtB -> B_390_324_0 = _rtP -> P_638 ; _rtB ->
B_390_392_0 = _rtP -> P_676 ; _rtB -> B_390_395_0 = _rtP -> P_677 ; _rtB ->
B_390_398_0 = _rtP -> P_678 ; _rtB -> B_390_403_0 = _rtP -> P_680 ; _rtB ->
B_390_407_0 = _rtP -> P_684 ; _rtB -> B_390_411_0 = _rtP -> P_690 ; _rtB ->
B_390_487_0 = _rtP -> P_738 ; _rtB -> B_390_490_0 = _rtP -> P_739 ; _rtB ->
B_390_493_0 = _rtP -> P_740 ; _rtB -> B_390_561_0 = _rtP -> P_778 ; _rtB ->
B_390_564_0 = _rtP -> P_779 ; _rtB -> B_390_567_0 = _rtP -> P_780 ; _rtB ->
B_390_572_0 = _rtP -> P_782 ; _rtB -> B_390_587_0 = _rtP -> P_787 ; _rtB ->
B_390_596_0 = ( uint8_T ) ( _rtP -> P_794 == _rtP -> P_795 ) ; _rtB ->
B_390_600_0 = ( uint8_T ) ( _rtP -> P_794 == _rtP -> P_796 ) ; _rtB ->
B_390_613_0 = ( uint8_T ) ( _rtP -> P_805 == _rtP -> P_806 ) ; _rtB ->
B_390_617_0 = ( uint8_T ) ( _rtP -> P_805 == _rtP -> P_807 ) ; _rtB ->
B_390_663_0 = ( uint8_T ) ( _rtP -> P_833 == _rtP -> P_834 ) ; _rtB ->
B_390_667_0 = ( uint8_T ) ( _rtP -> P_833 == _rtP -> P_835 ) ; _rtB ->
B_390_682_0 = _rtP -> P_845 ; _rtB -> B_390_683_0 = _rtP -> P_846 ; _rtB ->
B_390_694_0 = _rtP -> P_851 ; _rtB -> B_390_781_0 = _rtP -> P_875 ; _rtB ->
B_11_4_0 = ( uint8_T ) ( _rtP -> P_23 == _rtP -> P_24 ) ; _rtB -> B_11_8_0 =
( uint8_T ) ( _rtP -> P_23 == _rtP -> P_25 ) ; _rtB -> B_11_20_0 = _rtP ->
P_40 ; _rtB -> B_11_33_0 = _rtP -> P_56 ; if ( ssIsMajorTimeStep ( S ) != 0 )
{ srUpdateBC ( _rtDW -> AutomaticGainControl_SubsysRanBC_c ) ; } _rtB ->
B_390_820_0 = ( uint8_T ) ( _rtP -> P_883 == _rtP -> P_884 ) ; _rtB ->
B_390_824_0 = ( uint8_T ) ( _rtP -> P_883 == _rtP -> P_885 ) ; _rtB ->
B_390_836_0 = _rtP -> P_900 ; _rtB -> B_390_866_0 = _rtP -> P_927 ; _rtB ->
B_390_940_0 = _rtP -> P_982 ; _rtB -> B_390_943_0 = _rtP -> P_983 ; _rtB ->
B_390_952_0 = ( uint8_T ) ( _rtP -> P_990 == _rtP -> P_991 ) ; _rtB ->
B_390_956_0 = ( uint8_T ) ( _rtP -> P_990 == _rtP -> P_992 ) ; _rtB ->
B_390_969_0 = ( uint8_T ) ( _rtP -> P_1001 == _rtP -> P_1002 ) ; _rtB ->
B_390_973_0 = ( uint8_T ) ( _rtP -> P_1001 == _rtP -> P_1003 ) ; _rtB ->
B_390_1023_0 = ( uint8_T ) ( _rtP -> P_1031 == _rtP -> P_1032 ) ; _rtB ->
B_390_1027_0 = ( uint8_T ) ( _rtP -> P_1031 == _rtP -> P_1033 ) ; _rtB ->
B_390_1042_0 = _rtP -> P_1043 ; _rtB -> B_390_1043_0 = _rtP -> P_1044 ; _rtB
-> B_390_1050_0 = _rtP -> P_1047 ; _rtB -> B_390_1121_0 = _rtP -> P_1069 ;
_rtB -> B_46_4_0 = ( uint8_T ) ( _rtP -> P_79 == _rtP -> P_80 ) ; _rtB ->
B_46_8_0 = ( uint8_T ) ( _rtP -> P_79 == _rtP -> P_81 ) ; _rtB -> B_46_20_0 =
_rtP -> P_96 ; _rtB -> B_46_33_0 = _rtP -> P_112 ; if ( ssIsMajorTimeStep ( S
) != 0 ) { srUpdateBC ( _rtDW -> AutomaticGainControl_SubsysRanBC_l ) ; }
_rtB -> B_390_1159_0 = ( uint8_T ) ( _rtP -> P_1077 == _rtP -> P_1078 ) ;
_rtB -> B_390_1163_0 = ( uint8_T ) ( _rtP -> P_1077 == _rtP -> P_1079 ) ;
_rtB -> B_390_1175_0 = _rtP -> P_1094 ; _rtB -> B_390_1341_0 = _rtP -> P_1211
; _rtB -> B_390_1344_0 = _rtP -> P_1212 ; _rtB -> B_390_1347_0 = _rtP ->
P_1213 ; _rtB -> B_390_1414_0 = _rtP -> P_1250 ; _rtB -> B_390_1417_0 = _rtP
-> P_1251 ; _rtB -> B_390_1420_0 = _rtP -> P_1252 ; _rtB -> B_390_1498_0 =
_rtP -> P_1295 ; _rtB -> B_390_1501_0 = _rtP -> P_1296 ; _rtB -> B_390_1504_0
= _rtP -> P_1297 ; _rtB -> B_390_1589_0 = _rtP -> P_1379 ; _rtB ->
B_390_1590_0 = _rtP -> P_1380 ; _rtB -> B_390_1593_0 = _rtP -> P_1385 ; _rtB
-> B_390_1597_0 = _rtP -> P_1389 ; _rtB -> B_390_1598_0 = _rtP -> P_1390 ;
_rtB -> B_390_1609_0 = _rtP -> P_1396 ; _rtB -> B_390_1610_0 = _rtP -> P_1397
; _rtB -> B_390_1621_0 = _rtP -> P_1403 ; _rtB -> B_390_1622_0 = _rtP ->
P_1404 ; ssCallAccelRunBlock ( S , 390 , 1640 , SS_CALL_MDL_OUTPUTS ) ; _rtB
-> B_390_1651_0 = _rtP -> P_1413 ; _rtB -> B_390_1654_0 = _rtP -> P_1414 ;
_rtB -> B_390_1778_0 = _rtP -> P_1461 ; _rtB -> B_390_1781_0 = _rtP -> P_1462
; _rtB -> B_390_1784_0 = _rtP -> P_1463 ; _rtB -> B_390_1851_0 = _rtP ->
P_1500 ; _rtB -> B_390_1854_0 = _rtP -> P_1501 ; _rtB -> B_390_1857_0 = _rtP
-> P_1502 ; _rtB -> B_390_1932_0 = _rtP -> P_1543 ; _rtB -> B_390_1935_0 =
_rtP -> P_1544 ; _rtB -> B_390_1938_0 = _rtP -> P_1545 ; _rtB -> B_390_2007_0
= _rtP -> P_1583 ; _rtB -> B_390_2010_0 = _rtP -> P_1584 ; _rtB ->
B_390_2013_0 = _rtP -> P_1585 ; _rtB -> B_390_2113_0 = _rtP -> P_1693 ; _rtB
-> B_390_2114_0 = _rtP -> P_1694 ; _rtB -> B_390_2115_0 = _rtP -> P_1695 ;
_rtB -> B_390_2119_0 = _rtP -> P_1699 ; _rtB -> B_390_2120_0 = _rtP -> P_1700
; _rtB -> B_390_2131_0 = _rtP -> P_1706 ; _rtB -> B_390_2132_0 = _rtP ->
P_1707 ; _rtB -> B_390_2143_0 = _rtP -> P_1713 ; _rtB -> B_390_2144_0 = _rtP
-> P_1714 ; _rtB -> B_390_2230_0 = _rtP -> P_1756 ; _rtB -> B_390_2233_0 =
_rtP -> P_1757 ; _rtB -> B_390_2236_0 = _rtP -> P_1758 ; _rtB -> B_390_2303_0
= _rtP -> P_1795 ; _rtB -> B_390_2306_0 = _rtP -> P_1796 ; _rtB ->
B_390_2309_0 = _rtP -> P_1797 ; _rtB -> B_390_2384_0 = _rtP -> P_1838 ; _rtB
-> B_390_2387_0 = _rtP -> P_1839 ; _rtB -> B_390_2390_0 = _rtP -> P_1840 ;
_rtB -> B_390_2459_0 = _rtP -> P_1878 ; _rtB -> B_390_2462_0 = _rtP -> P_1879
; _rtB -> B_390_2465_0 = _rtP -> P_1880 ; _rtB -> B_390_2630_0 = _rtP ->
P_2024 ; _rtB -> B_390_2633_0 = _rtP -> P_2025 ; _rtB -> B_390_2636_0 = _rtP
-> P_2026 ; _rtB -> B_390_2703_0 = _rtP -> P_2063 ; _rtB -> B_390_2706_0 =
_rtP -> P_2064 ; _rtB -> B_390_2709_0 = _rtP -> P_2065 ; _rtB -> B_390_2784_0
= _rtP -> P_2106 ; _rtB -> B_390_2787_0 = _rtP -> P_2107 ; _rtB ->
B_390_2790_0 = _rtP -> P_2108 ; _rtB -> B_390_2859_0 = _rtP -> P_2146 ; _rtB
-> B_390_2862_0 = _rtP -> P_2147 ; _rtB -> B_390_2865_0 = _rtP -> P_2148 ;
_rtB -> B_390_2965_0 = _rtP -> P_2256 ; _rtB -> B_390_2966_0 = _rtP -> P_2257
; _rtB -> B_390_2967_0 = _rtP -> P_2258 ; _rtB -> B_390_2971_0 = _rtP ->
P_2262 ; _rtB -> B_390_2972_0 = _rtP -> P_2263 ; _rtB -> B_390_2983_0 = _rtP
-> P_2269 ; _rtB -> B_390_2984_0 = _rtP -> P_2270 ; _rtB -> B_390_2995_0 =
_rtP -> P_2276 ; _rtB -> B_390_2996_0 = _rtP -> P_2277 ; _rtB -> B_390_3082_0
= _rtP -> P_2319 ; _rtB -> B_390_3085_0 = _rtP -> P_2320 ; _rtB ->
B_390_3088_0 = _rtP -> P_2321 ; _rtB -> B_390_3155_0 = _rtP -> P_2358 ; _rtB
-> B_390_3158_0 = _rtP -> P_2359 ; _rtB -> B_390_3161_0 = _rtP -> P_2360 ;
_rtB -> B_390_3236_0 = _rtP -> P_2401 ; _rtB -> B_390_3239_0 = _rtP -> P_2402
; _rtB -> B_390_3242_0 = _rtP -> P_2403 ; _rtB -> B_390_3311_0 = _rtP ->
P_2441 ; _rtB -> B_390_3314_0 = _rtP -> P_2442 ; _rtB -> B_390_3317_0 = _rtP
-> P_2443 ; _rtB -> B_390_3417_0 = _rtP -> P_2551 ; _rtB -> B_390_3418_0 =
_rtP -> P_2552 ; _rtB -> B_390_3419_0 = _rtP -> P_2553 ; _rtB -> B_390_3423_0
= _rtP -> P_2557 ; _rtB -> B_390_3424_0 = _rtP -> P_2558 ; _rtB ->
B_390_3435_0 = _rtP -> P_2564 ; _rtB -> B_390_3436_0 = _rtP -> P_2565 ; _rtB
-> B_390_3447_0 = _rtP -> P_2571 ; _rtB -> B_390_3448_0 = _rtP -> P_2572 ;
_rtB -> B_390_3534_0 = _rtP -> P_2614 ; _rtB -> B_390_3537_0 = _rtP -> P_2615
; _rtB -> B_390_3540_0 = _rtP -> P_2616 ; _rtB -> B_390_3607_0 = _rtP ->
P_2653 ; _rtB -> B_390_3610_0 = _rtP -> P_2654 ; _rtB -> B_390_3613_0 = _rtP
-> P_2655 ; _rtB -> B_390_3688_0 = _rtP -> P_2696 ; _rtB -> B_390_3691_0 =
_rtP -> P_2697 ; _rtB -> B_390_3694_0 = _rtP -> P_2698 ; _rtB -> B_390_3763_0
= _rtP -> P_2736 ; _rtB -> B_390_3766_0 = _rtP -> P_2737 ; _rtB ->
B_390_3769_0 = _rtP -> P_2738 ; _rtB -> B_390_3869_0 = _rtP -> P_2846 ; _rtB
-> B_390_3870_0 = _rtP -> P_2847 ; _rtB -> B_390_3871_0 = _rtP -> P_2848 ;
_rtB -> B_390_3875_0 = _rtP -> P_2852 ; _rtB -> B_390_3876_0 = _rtP -> P_2853
; _rtB -> B_390_3887_0 = _rtP -> P_2859 ; _rtB -> B_390_3888_0 = _rtP ->
P_2860 ; _rtB -> B_390_3899_0 = _rtP -> P_2866 ; _rtB -> B_390_3900_0 = _rtP
-> P_2867 ; _rtB -> B_390_3988_0 = _rtP -> P_2913 ; _rtB -> B_390_3991_0 =
_rtP -> P_2914 ; _rtB -> B_390_3994_0 = _rtP -> P_2915 ; _rtB -> B_390_4062_0
= _rtP -> P_2953 ; _rtB -> B_390_4065_0 = _rtP -> P_2954 ; _rtB ->
B_390_4068_0 = _rtP -> P_2955 ; _rtB -> B_390_4073_0 = _rtP -> P_2957 ; _rtB
-> B_390_4078_0 = _rtP -> P_2959 ; _rtB -> B_390_4087_0 = ( uint8_T ) ( _rtP
-> P_2966 == _rtP -> P_2967 ) ; _rtB -> B_390_4091_0 = ( uint8_T ) ( _rtP ->
P_2966 == _rtP -> P_2968 ) ; _rtB -> B_390_4096_0 = _rtP -> P_2971 ; _rtB ->
B_390_4101_0 = _rtP -> P_2974 ; _rtB -> B_390_4124_0 = ( uint8_T ) ( _rtP ->
P_2993 == _rtP -> P_2994 ) ; _rtB -> B_390_4128_0 = ( uint8_T ) ( _rtP ->
P_2993 == _rtP -> P_2995 ) ; _rtB -> B_390_4205_0 = _rtP -> P_3013 ; _rtB ->
B_390_4238_0 = ( uint8_T ) ( _rtP -> P_3018 == _rtP -> P_3019 ) ; _rtB ->
B_390_4240_0 = ( uint8_T ) ( _rtP -> P_3018 == _rtP -> P_3020 ) ; _rtB ->
B_329_4_0 = ( uint8_T ) ( _rtP -> P_301 == _rtP -> P_302 ) ; _rtB ->
B_329_8_0 = ( uint8_T ) ( _rtP -> P_301 == _rtP -> P_303 ) ; _rtB ->
B_329_20_0 = _rtP -> P_318 ; _rtB -> B_329_33_0 = _rtP -> P_334 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_n ) ; } _rtB -> B_390_4259_0 = ( uint8_T ) (
_rtP -> P_3030 == _rtP -> P_3031 ) ; _rtB -> B_390_4263_0 = ( uint8_T ) (
_rtP -> P_3030 == _rtP -> P_3032 ) ; _rtB -> B_390_4275_0 = _rtP -> P_3047 ;
_rtB -> B_390_4365_0 = _rtP -> P_3124 ; _rtB -> B_390_4420_0 = _rtP -> P_3134
; _rtB -> B_390_4428_0 = _rtP -> P_3138 ; _rtB -> B_390_4522_0 = _rtP ->
P_3183 ; _rtB -> B_390_4525_0 = _rtP -> P_3184 ; _rtB -> B_390_4528_0 = _rtP
-> P_3185 ; _rtB -> B_390_4596_0 = _rtP -> P_3223 ; _rtB -> B_390_4599_0 =
_rtP -> P_3224 ; _rtB -> B_390_4602_0 = _rtP -> P_3225 ; _rtB -> B_390_4607_0
= _rtP -> P_3227 ; _rtB -> B_390_4612_0 = _rtP -> P_3229 ; _rtB ->
B_390_4621_0 = ( uint8_T ) ( _rtP -> P_3236 == _rtP -> P_3237 ) ; _rtB ->
B_390_4625_0 = ( uint8_T ) ( _rtP -> P_3236 == _rtP -> P_3238 ) ; _rtB ->
B_390_4630_0 = _rtP -> P_3241 ; _rtB -> B_390_4635_0 = _rtP -> P_3244 ; _rtB
-> B_390_4658_0 = ( uint8_T ) ( _rtP -> P_3263 == _rtP -> P_3264 ) ; _rtB ->
B_390_4662_0 = ( uint8_T ) ( _rtP -> P_3263 == _rtP -> P_3265 ) ; _rtB ->
B_390_4739_0 = _rtP -> P_3283 ; _rtB -> B_390_4772_0 = ( uint8_T ) ( _rtP ->
P_3288 == _rtP -> P_3289 ) ; _rtB -> B_390_4774_0 = ( uint8_T ) ( _rtP ->
P_3288 == _rtP -> P_3290 ) ; _rtB -> B_366_4_0 = ( uint8_T ) ( _rtP -> P_359
== _rtP -> P_360 ) ; _rtB -> B_366_8_0 = ( uint8_T ) ( _rtP -> P_359 == _rtP
-> P_361 ) ; _rtB -> B_366_20_0 = _rtP -> P_376 ; _rtB -> B_366_33_0 = _rtP
-> P_392 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC ) ; } _rtB -> B_390_4793_0 = ( uint8_T ) (
_rtP -> P_3300 == _rtP -> P_3301 ) ; _rtB -> B_390_4797_0 = ( uint8_T ) (
_rtP -> P_3300 == _rtP -> P_3302 ) ; _rtB -> B_390_4809_0 = _rtP -> P_3317 ;
_rtB -> B_390_4899_0 = _rtP -> P_3394 ; _rtB -> B_390_4954_0 = _rtP -> P_3404
; _rtB -> B_390_4962_0 = _rtP -> P_3408 ; _rtB -> B_390_5054_0 = _rtP ->
P_3449 ; _rtB -> B_390_5057_0 = _rtP -> P_3450 ; _rtB -> B_390_5060_0 = _rtP
-> P_3451 ; _rtB -> B_390_5127_0 = _rtP -> P_3488 ; _rtB -> B_390_5130_0 =
_rtP -> P_3489 ; _rtB -> B_390_5133_0 = _rtP -> P_3490 ; _rtB -> B_390_5208_0
= _rtP -> P_3531 ; _rtB -> B_390_5211_0 = _rtP -> P_3532 ; _rtB ->
B_390_5214_0 = _rtP -> P_3533 ; _rtB -> B_390_5283_0 = _rtP -> P_3571 ; _rtB
-> B_390_5286_0 = _rtP -> P_3572 ; _rtB -> B_390_5289_0 = _rtP -> P_3573 ;
_rtB -> B_390_5388_0 = _rtP -> P_3681 ; _rtB -> B_390_5389_0 = _rtP -> P_3682
; _rtB -> B_390_5390_0 = _rtP -> P_3683 ; _rtB -> B_390_5394_0 = _rtP ->
P_3687 ; _rtB -> B_390_5395_0 = _rtP -> P_3688 ; _rtB -> B_390_5406_0 = _rtP
-> P_3694 ; _rtB -> B_390_5407_0 = _rtP -> P_3695 ; _rtB -> B_390_5418_0 =
_rtP -> P_3701 ; _rtB -> B_390_5419_0 = _rtP -> P_3702 ; _rtB -> B_390_5442_0
= _rtP -> P_3708 ; _rtB -> B_390_5443_0 = _rtP -> P_3709 ; _rtB ->
B_390_5444_0 = _rtP -> P_3710 ; _rtB -> B_390_5448_0 = _rtP -> P_3714 ; _rtB
-> B_390_5449_0 = _rtP -> P_3715 ; _rtB -> B_390_5460_0 = _rtP -> P_3721 ;
_rtB -> B_390_5461_0 = _rtP -> P_3722 ; _rtB -> B_390_5472_0 = _rtP -> P_3728
; _rtB -> B_390_5473_0 = _rtP -> P_3729 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T HoldSine ;
real_T * lastU ; int32_T isHit ; B_MR_CIGRE_RESISTIVO_T * _rtB ;
P_MR_CIGRE_RESISTIVO_T * _rtP ; DW_MR_CIGRE_RESISTIVO_T * _rtDW ; _rtDW = ( (
DW_MR_CIGRE_RESISTIVO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_RESISTIVO_T * ) _ssGetModelBlockIO ( S ) ) ; isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> DiscreteTimeIntegrator_DSTATE +=
_rtP -> P_419 * _rtB -> B_390_846_0 ; _rtDW ->
DiscreteTimeIntegrator_PrevResetState = ( int8_T ) _rtB -> B_390_9_0 ; }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Vdc_DSTATE
= _rtB -> B_0_14_0 [ 25 ] ; _rtDW -> Idc_DSTATE = _rtB -> B_390_789_0 ; }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_k += _rtP -> P_447 * _rtB -> B_390_1185_0 ;
_rtDW -> DiscreteTimeIntegrator_PrevResetState_n = ( int8_T ) _rtB ->
B_390_47_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Vdc_DSTATE_p = _rtB -> B_0_14_0 [ 26 ] ; _rtDW -> Idc_DSTATE_b =
_rtB -> B_390_1129_0 ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit !=
0 ) { _rtDW -> UnitDelay1_DSTATE [ 0 ] = _rtB -> B_390_4144_0 ; _rtDW ->
UnitDelay1_DSTATE [ 1 ] = _rtB -> B_390_4143_0 ; _rtDW -> UnitDelay1_DSTATE [
2 ] = _rtB -> B_390_4365_0 ; _rtDW -> DiscreteTimeIntegrator_DSTATE_a += _rtP
-> P_465 * _rtB -> B_390_4285_0 ; _rtDW ->
DiscreteTimeIntegrator_PrevResetState_p = ( int8_T ) _rtB -> B_390_76_0 ; }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Vdc_DSTATE_g = _rtB -> B_0_14_0 [ 27 ] ; _rtDW -> Idc_DSTATE_h = _rtB ->
B_390_4213_0 ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> UnitDelay1_DSTATE_e [ 0 ] = _rtB -> B_390_4678_0 ; _rtDW ->
UnitDelay1_DSTATE_e [ 1 ] = _rtB -> B_390_4677_0 ; _rtDW ->
UnitDelay1_DSTATE_e [ 2 ] = _rtB -> B_390_4899_0 ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq += _rtP -> P_476 * _rtB -> B_390_4819_0 ;
_rtDW -> DiscreteTimeIntegrator_PrevResetState_d = ( int8_T ) _rtB ->
B_390_100_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Vdc_DSTATE_b = _rtB -> B_0_14_0 [ 28 ] ; _rtDW -> Idc_DSTATE_k =
_rtB -> B_390_4747_0 ; _rtDW -> UnitDelay5_DSTATE = _rtB -> B_390_4399_0 ;
_rtDW -> UnitDelay6_DSTATE = _rtB -> B_390_133_0 ; _rtDW ->
UnitDelay1_DSTATE_p [ 0 ] = _rtB -> B_390_4407_0 ; _rtDW ->
UnitDelay1_DSTATE_p [ 1 ] = _rtB -> B_390_4403_0 ; _rtDW ->
UnitDelay5_DSTATE_e = _rtB -> B_390_4933_0 ; _rtDW -> UnitDelay6_DSTATE_l =
_rtB -> B_390_148_0 ; _rtDW -> UnitDelay1_DSTATE_m [ 0 ] = _rtB ->
B_390_4941_0 ; _rtDW -> UnitDelay1_DSTATE_m [ 1 ] = _rtB -> B_390_4937_0 ;
_rtDW -> Delay_x_DSTATE = _rtB -> B_390_4391_0 ; _rtDW -> Delay_x_DSTATE_h =
_rtB -> B_390_4385_0 ; _rtDW -> Delay_x_DSTATE_e = _rtB -> B_390_4925_0 ;
_rtDW -> Delay_x_DSTATE_b = _rtB -> B_390_4919_0 ; HoldSine = _rtDW ->
lastSin ; _rtDW -> lastSin = _rtDW -> lastSin * _rtP -> P_558 + _rtDW ->
lastCos * _rtP -> P_557 ; _rtDW -> lastCos = _rtDW -> lastCos * _rtP -> P_558
- HoldSine * _rtP -> P_557 ; HoldSine = _rtDW -> lastSin_i ; _rtDW ->
lastSin_i = _rtDW -> lastSin_i * _rtP -> P_565 + _rtDW -> lastCos_p * _rtP ->
P_564 ; _rtDW -> lastCos_p = _rtDW -> lastCos_p * _rtP -> P_565 - HoldSine *
_rtP -> P_564 ; HoldSine = _rtDW -> lastSin_p ; _rtDW -> lastSin_p = _rtDW ->
lastSin_p * _rtP -> P_572 + _rtDW -> lastCos_c * _rtP -> P_571 ; _rtDW ->
lastCos_c = _rtDW -> lastCos_c * _rtP -> P_572 - HoldSine * _rtP -> P_571 ;
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Update ( S ) ;
} isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW ->
LastMajorTimeA == ( rtInf ) ) { _rtDW -> LastMajorTimeA = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYA = _rtB -> B_390_241_0 ; } else if ( _rtDW ->
LastMajorTimeB == ( rtInf ) ) { _rtDW -> LastMajorTimeB = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYB = _rtB -> B_390_241_0 ; } else if ( _rtDW ->
LastMajorTimeA < _rtDW -> LastMajorTimeB ) { _rtDW -> LastMajorTimeA =
ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA = _rtB -> B_390_241_0 ; } else {
_rtDW -> LastMajorTimeB = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB = _rtB ->
B_390_241_0 ; } if ( _rtDW -> LastMajorTimeA_o == ( rtInf ) ) { _rtDW ->
LastMajorTimeA_o = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA_e = _rtB ->
B_390_244_0 ; } else if ( _rtDW -> LastMajorTimeB_h == ( rtInf ) ) { _rtDW ->
LastMajorTimeB_h = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_l = _rtB ->
B_390_244_0 ; } else if ( _rtDW -> LastMajorTimeA_o < _rtDW ->
LastMajorTimeB_h ) { _rtDW -> LastMajorTimeA_o = ssGetTaskTime ( S , 1 ) ;
_rtDW -> PrevYA_e = _rtB -> B_390_244_0 ; } else { _rtDW -> LastMajorTimeB_h
= ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_l = _rtB -> B_390_244_0 ; } }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
UnitDelay_DSTATE = _rtB -> B_390_865_0 ; } { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( (
_rtDW -> TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( &
_rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Tail , & _rtDW -> TransportDelay_IWORK . Head , &
_rtDW -> TransportDelay_IWORK . Last , simTime - _rtP -> P_601 , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head ] = _rtB -> B_390_255_0 ; } isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput = _rtB ->
B_390_262_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_h . Head = ( ( _rtDW ->
TransportDelay_IWORK_h . Head < ( _rtDW -> TransportDelay_IWORK_h .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_h . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_h . Head == _rtDW ->
TransportDelay_IWORK_h . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h . CircularBufSize , & _rtDW -> TransportDelay_IWORK_h
. Tail , & _rtDW -> TransportDelay_IWORK_h . Head , & _rtDW ->
TransportDelay_IWORK_h . Last , simTime - _rtP -> P_606 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_h .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] = _rtB ->
B_390_263_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_a = _rtB -> B_390_270_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l . Head = ( ( _rtDW -> TransportDelay_IWORK_l . Head <
( _rtDW -> TransportDelay_IWORK_l . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l . Head == _rtDW -> TransportDelay_IWORK_l . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l . CircularBufSize , & _rtDW -> TransportDelay_IWORK_l
. Tail , & _rtDW -> TransportDelay_IWORK_l . Head , & _rtDW ->
TransportDelay_IWORK_l . Last , simTime - _rtP -> P_613 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_l .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] = _rtB ->
B_390_276_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_av = _rtB -> B_390_283_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k . Head = ( ( _rtDW -> TransportDelay_IWORK_k . Head <
( _rtDW -> TransportDelay_IWORK_k . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k . Head == _rtDW -> TransportDelay_IWORK_k . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k . CircularBufSize , & _rtDW -> TransportDelay_IWORK_k
. Tail , & _rtDW -> TransportDelay_IWORK_k . Head , & _rtDW ->
TransportDelay_IWORK_k . Last , simTime - _rtP -> P_618 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_k .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] = _rtB ->
B_390_284_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_e = _rtB -> B_390_291_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c . Head = ( ( _rtDW -> TransportDelay_IWORK_c . Head <
( _rtDW -> TransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c . Head == _rtDW -> TransportDelay_IWORK_c . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c . CircularBufSize , & _rtDW -> TransportDelay_IWORK_c
. Tail , & _rtDW -> TransportDelay_IWORK_c . Head , & _rtDW ->
TransportDelay_IWORK_c . Last , simTime - _rtP -> P_625 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_c .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] = _rtB ->
B_390_297_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_d = _rtB -> B_390_304_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m . Head = ( ( _rtDW -> TransportDelay_IWORK_m . Head <
( _rtDW -> TransportDelay_IWORK_m . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m . Head == _rtDW -> TransportDelay_IWORK_m . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m . CircularBufSize , & _rtDW -> TransportDelay_IWORK_m
. Tail , & _rtDW -> TransportDelay_IWORK_m . Head , & _rtDW ->
TransportDelay_IWORK_m . Last , simTime - _rtP -> P_630 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_m .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] = _rtB ->
B_390_305_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ac = _rtB -> B_390_312_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a . Head = ( ( _rtDW -> TransportDelay_IWORK_a . Head <
( _rtDW -> TransportDelay_IWORK_a . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a . Head == _rtDW -> TransportDelay_IWORK_a . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a . CircularBufSize , & _rtDW -> TransportDelay_IWORK_a
. Tail , & _rtDW -> TransportDelay_IWORK_a . Head , & _rtDW ->
TransportDelay_IWORK_a . Last , simTime - _rtP -> P_641 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_a .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] = _rtB ->
B_390_329_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_am = _rtB -> B_390_336_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p . Head = ( ( _rtDW -> TransportDelay_IWORK_p . Head <
( _rtDW -> TransportDelay_IWORK_p . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p . Head == _rtDW -> TransportDelay_IWORK_p . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p . CircularBufSize , & _rtDW -> TransportDelay_IWORK_p
. Tail , & _rtDW -> TransportDelay_IWORK_p . Head , & _rtDW ->
TransportDelay_IWORK_p . Last , simTime - _rtP -> P_646 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_p .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] = _rtB ->
B_390_337_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_f = _rtB -> B_390_344_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_d . Head = ( ( _rtDW -> TransportDelay_IWORK_d . Head <
( _rtDW -> TransportDelay_IWORK_d . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_d . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_d . Head == _rtDW -> TransportDelay_IWORK_d . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_d . CircularBufSize , & _rtDW -> TransportDelay_IWORK_d
. Tail , & _rtDW -> TransportDelay_IWORK_d . Head , & _rtDW ->
TransportDelay_IWORK_d . Last , simTime - _rtP -> P_653 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_d .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] = _rtB ->
B_390_350_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_o = _rtB -> B_390_357_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dg . Head = ( ( _rtDW -> TransportDelay_IWORK_dg . Head
< ( _rtDW -> TransportDelay_IWORK_dg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dg . Head == _rtDW -> TransportDelay_IWORK_dg . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dg . Tail , & _rtDW -> TransportDelay_IWORK_dg . Head ,
& _rtDW -> TransportDelay_IWORK_dg . Last , simTime - _rtP -> P_658 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dg . Head ] = _rtB -> B_390_358_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l = _rtB -> B_390_365_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_o .
Head = ( ( _rtDW -> TransportDelay_IWORK_o . Head < ( _rtDW ->
TransportDelay_IWORK_o . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o . Head == _rtDW -> TransportDelay_IWORK_o . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o . CircularBufSize , & _rtDW -> TransportDelay_IWORK_o
. Tail , & _rtDW -> TransportDelay_IWORK_o . Head , & _rtDW ->
TransportDelay_IWORK_o . Last , simTime - _rtP -> P_665 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_o .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] = _rtB ->
B_390_371_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_j = _rtB -> B_390_378_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ph . Head = ( ( _rtDW -> TransportDelay_IWORK_ph . Head
< ( _rtDW -> TransportDelay_IWORK_ph . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ph . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ph . Head == _rtDW -> TransportDelay_IWORK_ph . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ph . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ph . Tail , & _rtDW -> TransportDelay_IWORK_ph . Head ,
& _rtDW -> TransportDelay_IWORK_ph . Last , simTime - _rtP -> P_670 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ph . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ph . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ph . Head ] = _rtB -> B_390_379_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b = _rtB -> B_390_386_0 ; if ( _rtDW -> LastMajorTimeA_l
== ( rtInf ) ) { _rtDW -> LastMajorTimeA_l = ssGetTaskTime ( S , 1 ) ; _rtDW
-> PrevYA_ep = _rtB -> B_390_410_0 ; } else if ( _rtDW -> LastMajorTimeB_g ==
( rtInf ) ) { _rtDW -> LastMajorTimeB_g = ssGetTaskTime ( S , 1 ) ; _rtDW ->
PrevYB_c = _rtB -> B_390_410_0 ; } else if ( _rtDW -> LastMajorTimeA_l <
_rtDW -> LastMajorTimeB_g ) { _rtDW -> LastMajorTimeA_l = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYA_ep = _rtB -> B_390_410_0 ; } else { _rtDW ->
LastMajorTimeB_g = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_c = _rtB ->
B_390_410_0 ; } if ( _rtDW -> LastMajorTimeA_f == ( rtInf ) ) { _rtDW ->
LastMajorTimeA_f = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA_f = _rtB ->
B_390_413_0 ; } else if ( _rtDW -> LastMajorTimeB_c == ( rtInf ) ) { _rtDW ->
LastMajorTimeB_c = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_e = _rtB ->
B_390_413_0 ; } else if ( _rtDW -> LastMajorTimeA_f < _rtDW ->
LastMajorTimeB_c ) { _rtDW -> LastMajorTimeA_f = ssGetTaskTime ( S , 1 ) ;
_rtDW -> PrevYA_f = _rtB -> B_390_413_0 ; } else { _rtDW -> LastMajorTimeB_c
= ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_e = _rtB -> B_390_413_0 ; } }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
UnitDelay_DSTATE_o = _rtB -> B_390_1204_0 ; } { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_m5 . Head
= ( ( _rtDW -> TransportDelay_IWORK_m5 . Head < ( _rtDW ->
TransportDelay_IWORK_m5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m5 . Head == _rtDW -> TransportDelay_IWORK_m5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m5 . Tail , & _rtDW -> TransportDelay_IWORK_m5 . Head ,
& _rtDW -> TransportDelay_IWORK_m5 . Last , simTime - _rtP -> P_703 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m5 . Head ] = _rtB -> B_390_424_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lv = _rtB -> B_390_431_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i . Head = ( ( _rtDW -> TransportDelay_IWORK_i . Head <
( _rtDW -> TransportDelay_IWORK_i . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_i . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i . Head == _rtDW -> TransportDelay_IWORK_i . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i . CircularBufSize , & _rtDW -> TransportDelay_IWORK_i
. Tail , & _rtDW -> TransportDelay_IWORK_i . Head , & _rtDW ->
TransportDelay_IWORK_i . Last , simTime - _rtP -> P_708 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_i .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] = _rtB ->
B_390_432_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ff = _rtB -> B_390_439_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b . Head = ( ( _rtDW -> TransportDelay_IWORK_b . Head <
( _rtDW -> TransportDelay_IWORK_b . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b . Head == _rtDW -> TransportDelay_IWORK_b . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b . CircularBufSize , & _rtDW -> TransportDelay_IWORK_b
. Tail , & _rtDW -> TransportDelay_IWORK_b . Head , & _rtDW ->
TransportDelay_IWORK_b . Last , simTime - _rtP -> P_715 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_b .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] = _rtB ->
B_390_445_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_of = _rtB -> B_390_452_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_an . Head = ( ( _rtDW -> TransportDelay_IWORK_an . Head
< ( _rtDW -> TransportDelay_IWORK_an . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_an . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_an . Head == _rtDW -> TransportDelay_IWORK_an . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_an . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_an . Tail , & _rtDW -> TransportDelay_IWORK_an . Head ,
& _rtDW -> TransportDelay_IWORK_an . Last , simTime - _rtP -> P_720 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_an . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_an . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_an . Head ] = _rtB -> B_390_453_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m = _rtB -> B_390_460_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e . Head = ( ( _rtDW -> TransportDelay_IWORK_e . Head <
( _rtDW -> TransportDelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e . Head == _rtDW -> TransportDelay_IWORK_e . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e . CircularBufSize , & _rtDW -> TransportDelay_IWORK_e
. Tail , & _rtDW -> TransportDelay_IWORK_e . Head , & _rtDW ->
TransportDelay_IWORK_e . Last , simTime - _rtP -> P_727 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_e .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] = _rtB ->
B_390_466_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_a3 = _rtB -> B_390_473_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pk . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pk
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j . Head = ( ( _rtDW -> TransportDelay_IWORK_j . Head <
( _rtDW -> TransportDelay_IWORK_j . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j . Head == _rtDW -> TransportDelay_IWORK_j . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j . CircularBufSize , & _rtDW -> TransportDelay_IWORK_j
. Tail , & _rtDW -> TransportDelay_IWORK_j . Head , & _rtDW ->
TransportDelay_IWORK_j . Last , simTime - _rtP -> P_732 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_j .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] = _rtB ->
B_390_474_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ll = _rtB -> B_390_481_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lg . Head = ( ( _rtDW -> TransportDelay_IWORK_lg . Head
< ( _rtDW -> TransportDelay_IWORK_lg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lg . Head == _rtDW -> TransportDelay_IWORK_lg . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lg . Tail , & _rtDW -> TransportDelay_IWORK_lg . Head ,
& _rtDW -> TransportDelay_IWORK_lg . Last , simTime - _rtP -> P_743 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lg . Head ] = _rtB -> B_390_498_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_e0 = _rtB -> B_390_505_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ho . Head = ( ( _rtDW -> TransportDelay_IWORK_ho . Head
< ( _rtDW -> TransportDelay_IWORK_ho . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ho . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ho . Head == _rtDW -> TransportDelay_IWORK_ho . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ho . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ho . Tail , & _rtDW -> TransportDelay_IWORK_ho . Head ,
& _rtDW -> TransportDelay_IWORK_ho . Last , simTime - _rtP -> P_748 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ho . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ho . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ho . Head ] = _rtB -> B_390_506_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h = _rtB -> B_390_513_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ay .
Head = ( ( _rtDW -> TransportDelay_IWORK_ay . Head < ( _rtDW ->
TransportDelay_IWORK_ay . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ay . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ay . Head == _rtDW -> TransportDelay_IWORK_ay . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ay . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ay . Tail , & _rtDW -> TransportDelay_IWORK_ay . Head ,
& _rtDW -> TransportDelay_IWORK_ay . Last , simTime - _rtP -> P_755 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ay . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = _rtB -> B_390_519_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i = _rtB -> B_390_526_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g . Head = ( ( _rtDW -> TransportDelay_IWORK_g . Head <
( _rtDW -> TransportDelay_IWORK_g . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g . Head == _rtDW -> TransportDelay_IWORK_g . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g . CircularBufSize , & _rtDW -> TransportDelay_IWORK_g
. Tail , & _rtDW -> TransportDelay_IWORK_g . Head , & _rtDW ->
TransportDelay_IWORK_g . Last , simTime - _rtP -> P_760 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_g .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] = _rtB ->
B_390_527_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_k = _rtB -> B_390_534_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b3 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b3
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mq . Head = ( ( _rtDW -> TransportDelay_IWORK_mq . Head
< ( _rtDW -> TransportDelay_IWORK_mq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mq . Head == _rtDW -> TransportDelay_IWORK_mq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mq . Tail , & _rtDW -> TransportDelay_IWORK_mq . Head ,
& _rtDW -> TransportDelay_IWORK_mq . Last , simTime - _rtP -> P_767 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mq . Head ] = _rtB -> B_390_540_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_im = _rtB -> B_390_547_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_od .
Head = ( ( _rtDW -> TransportDelay_IWORK_od . Head < ( _rtDW ->
TransportDelay_IWORK_od . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_od . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_od . Head == _rtDW -> TransportDelay_IWORK_od . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_od . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_od . Tail , & _rtDW -> TransportDelay_IWORK_od . Head ,
& _rtDW -> TransportDelay_IWORK_od . Last , simTime - _rtP -> P_772 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_od . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = _rtB -> B_390_548_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jc = _rtB -> B_390_555_0 ; } isHit = ssIsSampleHit ( S ,
3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay1_DSTATE_pe = _rtB ->
B_390_866_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE = _rtB -> B_390_33_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_l ) {
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_a = 0U ; _rtDW -> Integ4_DSTATE_e = _rtP -> P_26 * _rtB
-> B_11_11_0 [ 0 ] + _rtB -> B_11_12_0 ; ssCallAccelRunBlock ( S , 11 , 17 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_n = _rtB -> B_11_11_0 [ 0 ]
; _rtDW -> UnitDelay1_DSTATE_fp = _rtB -> B_11_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_ll = 0U ; _rtDW -> Integ4_DSTATE_c = _rtP -> P_42 * _rtB
-> B_11_11_0 [ 1 ] + _rtB -> B_11_25_0 ; ssCallAccelRunBlock ( S , 11 , 30 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_k = _rtB -> B_11_11_0 [ 1 ]
; _rtDW -> UnitDelay1_DSTATE_b = _rtB -> B_11_37_0 ; } } _rtDW ->
Integ4_SYSTEM_ENABLE = 0U ; _rtDW -> Integ4_DSTATE = _rtP -> P_886 * _rtB ->
B_390_827_0 [ 1 ] + _rtB -> B_390_828_0 ; ssCallAccelRunBlock ( S , 390 , 833
, SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_i = _rtB -> B_390_827_0 [
1 ] ; _rtDW -> UnitDelay1_DSTATE_i = _rtB -> B_390_840_0 ; _rtDW ->
DiscreteDerivative_states = _rtDW -> DiscreteDerivative_tmp ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_c += _rtP -> P_905 * _rtB -> B_390_841_0 ; if (
_rtDW -> DiscreteTimeIntegrator_DSTATE_c >= _rtP -> P_907 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_c = _rtP -> P_907 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_c <= _rtP -> P_908 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_c = _rtP -> P_908 ; } } _rtDW ->
Delay_x1_DSTATE = _rtB -> B_390_858_0 ; _rtDW -> Delay_x2_DSTATE = _rtB ->
B_390_860_0 ; _rtDW -> UnitDelay1_DSTATE_a = _rtB -> B_390_940_0 ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
prediction_DSTATE_l = _rtB -> B_390_68_0 ; } isHit = ssIsSampleHit ( S , 3 ,
0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_i ) { isHit
= ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_lr = 0U ; _rtDW -> Integ4_DSTATE_a = _rtP -> P_82 * _rtB
-> B_46_11_0 [ 0 ] + _rtB -> B_46_12_0 ; ssCallAccelRunBlock ( S , 46 , 17 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_oc = _rtB -> B_46_11_0 [ 0 ]
; _rtDW -> UnitDelay1_DSTATE_c = _rtB -> B_46_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_c = 0U ; _rtDW -> Integ4_DSTATE_am = _rtP -> P_98 * _rtB
-> B_46_11_0 [ 1 ] + _rtB -> B_46_25_0 ; ssCallAccelRunBlock ( S , 46 , 30 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_bg = _rtB -> B_46_11_0 [ 1 ]
; _rtDW -> UnitDelay1_DSTATE_o0 = _rtB -> B_46_37_0 ; } } _rtDW ->
Integ4_SYSTEM_ENABLE_m = 0U ; _rtDW -> Integ4_DSTATE_b = _rtP -> P_1080 *
_rtB -> B_390_1166_0 [ 1 ] + _rtB -> B_390_1167_0 ; ssCallAccelRunBlock ( S ,
390 , 1172 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_ob = _rtB ->
B_390_1166_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_k = _rtB -> B_390_1179_0 ;
_rtDW -> DiscreteDerivative_states_b = _rtDW -> DiscreteDerivative_tmp_h ;
_rtDW -> DiscreteTimeIntegrator_DSTATE_ce += _rtP -> P_1099 * _rtB ->
B_390_1180_0 ; if ( _rtDW -> DiscreteTimeIntegrator_DSTATE_ce >= _rtP ->
P_1101 ) { _rtDW -> DiscreteTimeIntegrator_DSTATE_ce = _rtP -> P_1101 ; }
else { if ( _rtDW -> DiscreteTimeIntegrator_DSTATE_ce <= _rtP -> P_1102 ) {
_rtDW -> DiscreteTimeIntegrator_DSTATE_ce = _rtP -> P_1102 ; } } _rtDW ->
Delay_x1_DSTATE_f = _rtB -> B_390_1197_0 ; _rtDW -> Delay_x2_DSTATE_k = _rtB
-> B_390_1199_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ke . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ke . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_mp . Head = ( ( _rtDW
-> TransportDelay_IWORK_mp . Head < ( _rtDW -> TransportDelay_IWORK_mp .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_mp . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_mp . Head == _rtDW ->
TransportDelay_IWORK_mp . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mp . Tail , & _rtDW -> TransportDelay_IWORK_mp . Head ,
& _rtDW -> TransportDelay_IWORK_mp . Last , simTime - _rtP -> P_1176 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mp . Head ] = _rtB -> B_390_1278_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fp = _rtB -> B_390_1285_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k0 . Head = ( ( _rtDW -> TransportDelay_IWORK_k0 . Head
< ( _rtDW -> TransportDelay_IWORK_k0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k0 . Head == _rtDW -> TransportDelay_IWORK_k0 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k0 . Tail , & _rtDW -> TransportDelay_IWORK_k0 . Head ,
& _rtDW -> TransportDelay_IWORK_k0 . Last , simTime - _rtP -> P_1181 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k0 . Head ] = _rtB -> B_390_1286_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lu = _rtB -> B_390_1293_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_l2 .
Head = ( ( _rtDW -> TransportDelay_IWORK_l2 . Head < ( _rtDW ->
TransportDelay_IWORK_l2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l2 . Head == _rtDW -> TransportDelay_IWORK_l2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l2 . Tail , & _rtDW -> TransportDelay_IWORK_l2 . Head ,
& _rtDW -> TransportDelay_IWORK_l2 . Last , simTime - _rtP -> P_1188 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l2 . Head ] = _rtB -> B_390_1299_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_p = _rtB -> B_390_1306_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ha . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ha .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a1 . Head = ( ( _rtDW -> TransportDelay_IWORK_a1 . Head
< ( _rtDW -> TransportDelay_IWORK_a1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a1 . Head == _rtDW -> TransportDelay_IWORK_a1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a1 . Tail , & _rtDW -> TransportDelay_IWORK_a1 . Head ,
& _rtDW -> TransportDelay_IWORK_a1 . Last , simTime - _rtP -> P_1193 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a1 . Head ] = _rtB -> B_390_1307_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bn = _rtB -> B_390_1314_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eh . Head = ( ( _rtDW -> TransportDelay_IWORK_eh . Head
< ( _rtDW -> TransportDelay_IWORK_eh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eh . Head == _rtDW -> TransportDelay_IWORK_eh . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eh . Tail , & _rtDW -> TransportDelay_IWORK_eh . Head ,
& _rtDW -> TransportDelay_IWORK_eh . Last , simTime - _rtP -> P_1200 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eh . Head ] = _rtB -> B_390_1320_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pm = _rtB -> B_390_1327_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f . Head = ( ( _rtDW -> TransportDelay_IWORK_f . Head <
( _rtDW -> TransportDelay_IWORK_f . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f . Head == _rtDW -> TransportDelay_IWORK_f . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f . CircularBufSize , & _rtDW -> TransportDelay_IWORK_f
. Tail , & _rtDW -> TransportDelay_IWORK_f . Head , & _rtDW ->
TransportDelay_IWORK_f . Last , simTime - _rtP -> P_1205 , tBuffer , uBuffer
, ( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_f .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] = _rtB ->
B_390_1328_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_an = _rtB -> B_390_1335_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ij . Head = ( ( _rtDW -> TransportDelay_IWORK_ij . Head
< ( _rtDW -> TransportDelay_IWORK_ij . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ij . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ij . Head == _rtDW -> TransportDelay_IWORK_ij . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ij . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ij . Tail , & _rtDW -> TransportDelay_IWORK_ij . Head ,
& _rtDW -> TransportDelay_IWORK_ij . Last , simTime - _rtP -> P_1215 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ij . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ij . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ij . Head ] = _rtB -> B_390_1351_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hj = _rtB -> B_390_1358_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ck . Head = ( ( _rtDW -> TransportDelay_IWORK_ck . Head
< ( _rtDW -> TransportDelay_IWORK_ck . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ck . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ck . Head == _rtDW -> TransportDelay_IWORK_ck . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ck . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ck . Tail , & _rtDW -> TransportDelay_IWORK_ck . Head ,
& _rtDW -> TransportDelay_IWORK_ck . Last , simTime - _rtP -> P_1220 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ck . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ck . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ck . Head ] = _rtB -> B_390_1359_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eg = _rtB -> B_390_1366_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ok . Head = ( ( _rtDW -> TransportDelay_IWORK_ok . Head
< ( _rtDW -> TransportDelay_IWORK_ok . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ok . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ok . Head == _rtDW -> TransportDelay_IWORK_ok . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ok . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ok . Tail , & _rtDW -> TransportDelay_IWORK_ok . Head ,
& _rtDW -> TransportDelay_IWORK_ok . Last , simTime - _rtP -> P_1227 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ok . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ok . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ok . Head ] = _rtB -> B_390_1372_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dh = _rtB -> B_390_1379_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_fi .
Head = ( ( _rtDW -> TransportDelay_IWORK_fi . Head < ( _rtDW ->
TransportDelay_IWORK_fi . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fi . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fi . Head == _rtDW -> TransportDelay_IWORK_fi . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fi . Tail , & _rtDW -> TransportDelay_IWORK_fi . Head ,
& _rtDW -> TransportDelay_IWORK_fi . Last , simTime - _rtP -> P_1232 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fi . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fi . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fi . Head ] = _rtB -> B_390_1380_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mp = _rtB -> B_390_1387_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ea . Head = ( ( _rtDW -> TransportDelay_IWORK_ea . Head
< ( _rtDW -> TransportDelay_IWORK_ea . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ea . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ea . Head == _rtDW -> TransportDelay_IWORK_ea . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ea . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ea . Tail , & _rtDW -> TransportDelay_IWORK_ea . Head ,
& _rtDW -> TransportDelay_IWORK_ea . Last , simTime - _rtP -> P_1239 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ea . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea . Head ] = _rtB -> B_390_1393_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g = _rtB -> B_390_1400_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_me .
Head = ( ( _rtDW -> TransportDelay_IWORK_me . Head < ( _rtDW ->
TransportDelay_IWORK_me . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_me . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_me . Head == _rtDW -> TransportDelay_IWORK_me . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_me . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_me . Tail , & _rtDW -> TransportDelay_IWORK_me . Head ,
& _rtDW -> TransportDelay_IWORK_me . Last , simTime - _rtP -> P_1244 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_me . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_me . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_me . Head ] = _rtB -> B_390_1401_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_av3 = _rtB -> B_390_1408_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_di . Head = ( ( _rtDW -> TransportDelay_IWORK_di . Head
< ( _rtDW -> TransportDelay_IWORK_di . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_di . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_di . Head == _rtDW -> TransportDelay_IWORK_di . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_di . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_di . Tail , & _rtDW -> TransportDelay_IWORK_di . Head ,
& _rtDW -> TransportDelay_IWORK_di . Last , simTime - _rtP -> P_1260 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_di . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_di . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_di . Head ] = _rtB -> B_390_1435_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fq = _rtB -> B_390_1442_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_n .
Head = ( ( _rtDW -> TransportDelay_IWORK_n . Head < ( _rtDW ->
TransportDelay_IWORK_n . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n . Head == _rtDW -> TransportDelay_IWORK_n . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n . CircularBufSize , & _rtDW -> TransportDelay_IWORK_n
. Tail , & _rtDW -> TransportDelay_IWORK_n . Head , & _rtDW ->
TransportDelay_IWORK_n . Last , simTime - _rtP -> P_1265 , tBuffer , uBuffer
, ( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_n .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] = _rtB ->
B_390_1443_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ffa = _rtB -> B_390_1450_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj . Head = ( ( _rtDW -> TransportDelay_IWORK_hj . Head
< ( _rtDW -> TransportDelay_IWORK_hj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj . Head == _rtDW -> TransportDelay_IWORK_hj . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj . Tail , & _rtDW -> TransportDelay_IWORK_hj . Head ,
& _rtDW -> TransportDelay_IWORK_hj . Last , simTime - _rtP -> P_1272 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj . Head ] = _rtB -> B_390_1456_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fm = _rtB -> B_390_1463_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oz . Head = ( ( _rtDW -> TransportDelay_IWORK_oz . Head
< ( _rtDW -> TransportDelay_IWORK_oz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oz . Head == _rtDW -> TransportDelay_IWORK_oz . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oz . Tail , & _rtDW -> TransportDelay_IWORK_oz . Head ,
& _rtDW -> TransportDelay_IWORK_oz . Last , simTime - _rtP -> P_1277 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oz . Head ] = _rtB -> B_390_1464_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fv = _rtB -> B_390_1471_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ek . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fq . Head = ( ( _rtDW -> TransportDelay_IWORK_fq . Head
< ( _rtDW -> TransportDelay_IWORK_fq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fq . Head == _rtDW -> TransportDelay_IWORK_fq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fq . Tail , & _rtDW -> TransportDelay_IWORK_fq . Head ,
& _rtDW -> TransportDelay_IWORK_fq . Last , simTime - _rtP -> P_1284 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = _rtB -> B_390_1477_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gx = _rtB -> B_390_1484_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g1 . Head = ( ( _rtDW -> TransportDelay_IWORK_g1 . Head
< ( _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g1 . Head == _rtDW -> TransportDelay_IWORK_g1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g1 . Tail , & _rtDW -> TransportDelay_IWORK_g1 . Head ,
& _rtDW -> TransportDelay_IWORK_g1 . Last , simTime - _rtP -> P_1289 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = _rtB -> B_390_1485_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pc = _rtB -> B_390_1492_0 ; } if ( _rtDW -> TimeStampA
== ( rtInf ) ) { _rtDW -> TimeStampA = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA ; } else if ( _rtDW -> TimeStampB == ( rtInf ) ) { _rtDW ->
TimeStampB = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB ; } else if (
_rtDW -> TimeStampA < _rtDW -> TimeStampB ) { _rtDW -> TimeStampA = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA ; } else { _rtDW -> TimeStampB = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB ; } * lastU = _rtB -> B_390_1602_0 ;
if ( _rtDW -> TimeStampA_n == ( rtInf ) ) { _rtDW -> TimeStampA_n = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA_j ; } else if ( _rtDW -> TimeStampB_i
== ( rtInf ) ) { _rtDW -> TimeStampB_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_d ; } else if ( _rtDW -> TimeStampA_n < _rtDW -> TimeStampB_i )
{ _rtDW -> TimeStampA_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_j ;
} else { _rtDW -> TimeStampB_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_d ; } * lastU = _rtB -> B_390_1614_0 ; if ( _rtDW ->
TimeStampA_nq == ( rtInf ) ) { _rtDW -> TimeStampA_nq = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_c ; } else if ( _rtDW -> TimeStampB_k == ( rtInf )
) { _rtDW -> TimeStampB_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_m
; } else if ( _rtDW -> TimeStampA_nq < _rtDW -> TimeStampB_k ) { _rtDW ->
TimeStampA_nq = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_c ; } else {
_rtDW -> TimeStampB_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_m ; }
* lastU = _rtB -> B_390_1626_0 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_nm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_nm . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_hog . Head = (
( _rtDW -> TransportDelay_IWORK_hog . Head < ( _rtDW ->
TransportDelay_IWORK_hog . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hog . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hog . Head == _rtDW -> TransportDelay_IWORK_hog . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hog . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hog . Tail , & _rtDW -> TransportDelay_IWORK_hog . Head
, & _rtDW -> TransportDelay_IWORK_hog . Last , simTime - _rtP -> P_1426 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hog . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hog . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hog . Head ] = _rtB -> B_390_1715_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pz = _rtB -> B_390_1722_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ak . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ak .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ju . Head = ( ( _rtDW -> TransportDelay_IWORK_ju . Head
< ( _rtDW -> TransportDelay_IWORK_ju . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ju . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ju . Head == _rtDW -> TransportDelay_IWORK_ju . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ju . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ju . Tail , & _rtDW -> TransportDelay_IWORK_ju . Head ,
& _rtDW -> TransportDelay_IWORK_ju . Last , simTime - _rtP -> P_1431 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ju . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ju . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ju . Head ] = _rtB -> B_390_1723_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ft = _rtB -> B_390_1730_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_js . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_js .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a3 . Head = ( ( _rtDW -> TransportDelay_IWORK_a3 . Head
< ( _rtDW -> TransportDelay_IWORK_a3 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a3 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a3 . Head == _rtDW -> TransportDelay_IWORK_a3 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a3 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a3 . Tail , & _rtDW -> TransportDelay_IWORK_a3 . Head ,
& _rtDW -> TransportDelay_IWORK_a3 . Last , simTime - _rtP -> P_1438 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a3 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3 . Head ] = _rtB -> B_390_1736_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gi = _rtB -> B_390_1743_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_at . Head = ( ( _rtDW -> TransportDelay_IWORK_at . Head
< ( _rtDW -> TransportDelay_IWORK_at . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_at . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_at . Head == _rtDW -> TransportDelay_IWORK_at . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_at . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_at . Tail , & _rtDW -> TransportDelay_IWORK_at . Head ,
& _rtDW -> TransportDelay_IWORK_at . Last , simTime - _rtP -> P_1443 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_at . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_at . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_at . Head ] = _rtB -> B_390_1744_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hg = _rtB -> B_390_1751_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ce . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_h5 . Head = ( ( _rtDW -> TransportDelay_IWORK_h5 . Head
< ( _rtDW -> TransportDelay_IWORK_h5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_h5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_h5 . Head == _rtDW -> TransportDelay_IWORK_h5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_h5 . Tail , & _rtDW -> TransportDelay_IWORK_h5 . Head ,
& _rtDW -> TransportDelay_IWORK_h5 . Last , simTime - _rtP -> P_1450 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_h5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_h5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_h5 . Head ] = _rtB -> B_390_1757_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dg = _rtB -> B_390_1764_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cr . Head = ( ( _rtDW -> TransportDelay_IWORK_cr . Head
< ( _rtDW -> TransportDelay_IWORK_cr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cr . Head == _rtDW -> TransportDelay_IWORK_cr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cr . Tail , & _rtDW -> TransportDelay_IWORK_cr . Head ,
& _rtDW -> TransportDelay_IWORK_cr . Last , simTime - _rtP -> P_1455 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = _rtB -> B_390_1765_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kp = _rtB -> B_390_1772_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dv . Head = ( ( _rtDW -> TransportDelay_IWORK_dv . Head
< ( _rtDW -> TransportDelay_IWORK_dv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dv . Head == _rtDW -> TransportDelay_IWORK_dv . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dv . Tail , & _rtDW -> TransportDelay_IWORK_dv . Head ,
& _rtDW -> TransportDelay_IWORK_dv . Last , simTime - _rtP -> P_1465 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = _rtB -> B_390_1788_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fg = _rtB -> B_390_1795_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lv . Head = ( ( _rtDW -> TransportDelay_IWORK_lv . Head
< ( _rtDW -> TransportDelay_IWORK_lv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lv . Head == _rtDW -> TransportDelay_IWORK_lv . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lv . Tail , & _rtDW -> TransportDelay_IWORK_lv . Head ,
& _rtDW -> TransportDelay_IWORK_lv . Last , simTime - _rtP -> P_1470 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lv . Head ] = _rtB -> B_390_1796_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bj = _rtB -> B_390_1803_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ae . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lu . Head = ( ( _rtDW -> TransportDelay_IWORK_lu . Head
< ( _rtDW -> TransportDelay_IWORK_lu . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lu . Head == _rtDW -> TransportDelay_IWORK_lu . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lu . Tail , & _rtDW -> TransportDelay_IWORK_lu . Head ,
& _rtDW -> TransportDelay_IWORK_lu . Last , simTime - _rtP -> P_1477 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = _rtB -> B_390_1809_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_df = _rtB -> B_390_1816_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kz . Head = ( ( _rtDW -> TransportDelay_IWORK_kz . Head
< ( _rtDW -> TransportDelay_IWORK_kz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kz . Head == _rtDW -> TransportDelay_IWORK_kz . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kz . Tail , & _rtDW -> TransportDelay_IWORK_kz . Head ,
& _rtDW -> TransportDelay_IWORK_kz . Last , simTime - _rtP -> P_1482 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kz . Head ] = _rtB -> B_390_1817_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bv = _rtB -> B_390_1824_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oa . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ft . Head = ( ( _rtDW -> TransportDelay_IWORK_ft . Head
< ( _rtDW -> TransportDelay_IWORK_ft . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ft . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ft . Head == _rtDW -> TransportDelay_IWORK_ft . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ft . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ft . Tail , & _rtDW -> TransportDelay_IWORK_ft . Head ,
& _rtDW -> TransportDelay_IWORK_ft . Last , simTime - _rtP -> P_1489 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ft . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = _rtB -> B_390_1830_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eq = _rtB -> B_390_1837_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fd . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bc . Head = ( ( _rtDW -> TransportDelay_IWORK_bc . Head
< ( _rtDW -> TransportDelay_IWORK_bc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bc . Head == _rtDW -> TransportDelay_IWORK_bc . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bc . Tail , & _rtDW -> TransportDelay_IWORK_bc . Head ,
& _rtDW -> TransportDelay_IWORK_bc . Last , simTime - _rtP -> P_1494 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bc . Head ] = _rtB -> B_390_1838_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_iw = _rtB -> B_390_1845_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ej . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ej .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_by . Head = ( ( _rtDW -> TransportDelay_IWORK_by . Head
< ( _rtDW -> TransportDelay_IWORK_by . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_by . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_by . Head == _rtDW -> TransportDelay_IWORK_by . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_by . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_by . Tail , & _rtDW -> TransportDelay_IWORK_by . Head ,
& _rtDW -> TransportDelay_IWORK_by . Last , simTime - _rtP -> P_1508 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_by . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_by . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_by . Head ] = _rtB -> B_390_1869_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fqc = _rtB -> B_390_1876_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fb . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fb .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nm . Head = ( ( _rtDW -> TransportDelay_IWORK_nm . Head
< ( _rtDW -> TransportDelay_IWORK_nm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nm . Head == _rtDW -> TransportDelay_IWORK_nm . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nm . Tail , & _rtDW -> TransportDelay_IWORK_nm . Head ,
& _rtDW -> TransportDelay_IWORK_nm . Last , simTime - _rtP -> P_1513 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nm . Head ] = _rtB -> B_390_1877_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bf = _rtB -> B_390_1884_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_og .
Head = ( ( _rtDW -> TransportDelay_IWORK_og . Head < ( _rtDW ->
TransportDelay_IWORK_og . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_og . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_og . Head == _rtDW -> TransportDelay_IWORK_og . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_og . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_og . Tail , & _rtDW -> TransportDelay_IWORK_og . Head ,
& _rtDW -> TransportDelay_IWORK_og . Last , simTime - _rtP -> P_1520 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_og . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = _rtB -> B_390_1890_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ib = _rtB -> B_390_1897_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_op . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_op .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he . Head = ( ( _rtDW -> TransportDelay_IWORK_he . Head
< ( _rtDW -> TransportDelay_IWORK_he . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_he . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he . Head == _rtDW -> TransportDelay_IWORK_he . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_he . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he . Tail , & _rtDW -> TransportDelay_IWORK_he . Head ,
& _rtDW -> TransportDelay_IWORK_he . Last , simTime - _rtP -> P_1525 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he . Head ] = _rtB -> B_390_1898_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ak = _rtB -> B_390_1905_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hl . Head = ( ( _rtDW -> TransportDelay_IWORK_hl . Head
< ( _rtDW -> TransportDelay_IWORK_hl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hl . Head == _rtDW -> TransportDelay_IWORK_hl . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hl . Tail , & _rtDW -> TransportDelay_IWORK_hl . Head ,
& _rtDW -> TransportDelay_IWORK_hl . Last , simTime - _rtP -> P_1532 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hl . Head ] = _rtB -> B_390_1911_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bu = _rtB -> B_390_1918_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j1 . Head = ( ( _rtDW -> TransportDelay_IWORK_j1 . Head
< ( _rtDW -> TransportDelay_IWORK_j1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j1 . Head == _rtDW -> TransportDelay_IWORK_j1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j1 . Tail , & _rtDW -> TransportDelay_IWORK_j1 . Head ,
& _rtDW -> TransportDelay_IWORK_j1 . Last , simTime - _rtP -> P_1537 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_j1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = _rtB -> B_390_1919_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ih = _rtB -> B_390_1926_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mr . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2 . Head = ( ( _rtDW -> TransportDelay_IWORK_e2 . Head
< ( _rtDW -> TransportDelay_IWORK_e2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2 . Head == _rtDW -> TransportDelay_IWORK_e2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2 . Tail , & _rtDW -> TransportDelay_IWORK_e2 . Head ,
& _rtDW -> TransportDelay_IWORK_e2 . Last , simTime - _rtP -> P_1548 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2 . Head ] = _rtB -> B_390_1944_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_de = _rtB -> B_390_1951_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_id . Head = ( ( _rtDW -> TransportDelay_IWORK_id . Head
< ( _rtDW -> TransportDelay_IWORK_id . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_id . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_id . Head == _rtDW -> TransportDelay_IWORK_id . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_id . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_id . Tail , & _rtDW -> TransportDelay_IWORK_id . Head ,
& _rtDW -> TransportDelay_IWORK_id . Last , simTime - _rtP -> P_1553 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_id . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_id . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_id . Head ] = _rtB -> B_390_1952_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h2 = _rtB -> B_390_1959_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gr . Head = ( ( _rtDW -> TransportDelay_IWORK_gr . Head
< ( _rtDW -> TransportDelay_IWORK_gr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gr . Head == _rtDW -> TransportDelay_IWORK_gr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gr . Tail , & _rtDW -> TransportDelay_IWORK_gr . Head ,
& _rtDW -> TransportDelay_IWORK_gr . Last , simTime - _rtP -> P_1560 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gr . Head ] = _rtB -> B_390_1965_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mb = _rtB -> B_390_1972_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nd . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dl . Head = ( ( _rtDW -> TransportDelay_IWORK_dl . Head
< ( _rtDW -> TransportDelay_IWORK_dl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dl . Head == _rtDW -> TransportDelay_IWORK_dl . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dl . Tail , & _rtDW -> TransportDelay_IWORK_dl . Head ,
& _rtDW -> TransportDelay_IWORK_dl . Last , simTime - _rtP -> P_1565 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dl . Head ] = _rtB -> B_390_1973_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bz = _rtB -> B_390_1980_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hx . Head = ( ( _rtDW -> TransportDelay_IWORK_hx . Head
< ( _rtDW -> TransportDelay_IWORK_hx . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hx . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hx . Head == _rtDW -> TransportDelay_IWORK_hx . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hx . Tail , & _rtDW -> TransportDelay_IWORK_hx . Head ,
& _rtDW -> TransportDelay_IWORK_hx . Last , simTime - _rtP -> P_1572 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hx . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hx . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hx . Head ] = _rtB -> B_390_1986_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eh = _rtB -> B_390_1993_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fw . Head = ( ( _rtDW -> TransportDelay_IWORK_fw . Head
< ( _rtDW -> TransportDelay_IWORK_fw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fw . Head == _rtDW -> TransportDelay_IWORK_fw . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fw . Tail , & _rtDW -> TransportDelay_IWORK_fw . Head ,
& _rtDW -> TransportDelay_IWORK_fw . Last , simTime - _rtP -> P_1577 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fw . Head ] = _rtB -> B_390_1994_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_n = _rtB -> B_390_2001_0 ; } if ( _rtDW -> TimeStampA_b
== ( rtInf ) ) { _rtDW -> TimeStampA_b = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_h ; } else if ( _rtDW -> TimeStampB_e == ( rtInf ) ) { _rtDW ->
TimeStampB_e = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_p ; } else if (
_rtDW -> TimeStampA_b < _rtDW -> TimeStampB_e ) { _rtDW -> TimeStampA_b =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_h ; } else { _rtDW ->
TimeStampB_e = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_p ; } * lastU =
_rtB -> B_390_2124_0 ; if ( _rtDW -> TimeStampA_i == ( rtInf ) ) { _rtDW ->
TimeStampA_i = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_p ; } else if (
_rtDW -> TimeStampB_m == ( rtInf ) ) { _rtDW -> TimeStampB_m = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_h ; } else if ( _rtDW -> TimeStampA_i < _rtDW
-> TimeStampB_m ) { _rtDW -> TimeStampA_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_p ; } else { _rtDW -> TimeStampB_m = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_h ; } * lastU = _rtB -> B_390_2136_0 ; if ( _rtDW ->
TimeStampA_h == ( rtInf ) ) { _rtDW -> TimeStampA_h = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_cg ; } else if ( _rtDW -> TimeStampB_id == ( rtInf )
) { _rtDW -> TimeStampB_id = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e
; } else if ( _rtDW -> TimeStampA_h < _rtDW -> TimeStampB_id ) { _rtDW ->
TimeStampA_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cg ; } else {
_rtDW -> TimeStampB_id = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e ; }
* lastU = _rtB -> B_390_2148_0 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_be . Head = ( (
_rtDW -> TransportDelay_IWORK_be . Head < ( _rtDW -> TransportDelay_IWORK_be
. CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_be . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_be . Head == _rtDW ->
TransportDelay_IWORK_be . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_be . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_be . Tail , & _rtDW -> TransportDelay_IWORK_be . Head ,
& _rtDW -> TransportDelay_IWORK_be . Last , simTime - _rtP -> P_1721 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_be . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_be . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_be . Head ] = _rtB -> B_390_2167_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kb = _rtB -> B_390_2174_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_op . Head = ( ( _rtDW -> TransportDelay_IWORK_op . Head
< ( _rtDW -> TransportDelay_IWORK_op . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_op . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_op . Head == _rtDW -> TransportDelay_IWORK_op . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_op . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_op . Tail , & _rtDW -> TransportDelay_IWORK_op . Head ,
& _rtDW -> TransportDelay_IWORK_op . Last , simTime - _rtP -> P_1726 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_op . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_op . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_op . Head ] = _rtB -> B_390_2175_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hs = _rtB -> B_390_2182_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ma . Head = ( ( _rtDW -> TransportDelay_IWORK_ma . Head
< ( _rtDW -> TransportDelay_IWORK_ma . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ma . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ma . Head == _rtDW -> TransportDelay_IWORK_ma . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ma . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ma . Tail , & _rtDW -> TransportDelay_IWORK_ma . Head ,
& _rtDW -> TransportDelay_IWORK_ma . Last , simTime - _rtP -> P_1733 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ma . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma . Head ] = _rtB -> B_390_2188_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dv = _rtB -> B_390_2195_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ov . Head = ( ( _rtDW -> TransportDelay_IWORK_ov . Head
< ( _rtDW -> TransportDelay_IWORK_ov . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ov . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ov . Head == _rtDW -> TransportDelay_IWORK_ov . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ov . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ov . Tail , & _rtDW -> TransportDelay_IWORK_ov . Head ,
& _rtDW -> TransportDelay_IWORK_ov . Last , simTime - _rtP -> P_1738 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ov . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ov . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ov . Head ] = _rtB -> B_390_2196_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gg = _rtB -> B_390_2203_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_do . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_do .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hr . Head = ( ( _rtDW -> TransportDelay_IWORK_hr . Head
< ( _rtDW -> TransportDelay_IWORK_hr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hr . Head == _rtDW -> TransportDelay_IWORK_hr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hr . Tail , & _rtDW -> TransportDelay_IWORK_hr . Head ,
& _rtDW -> TransportDelay_IWORK_hr . Last , simTime - _rtP -> P_1745 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hr . Head ] = _rtB -> B_390_2209_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nb = _rtB -> B_390_2216_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_as . Head = ( ( _rtDW -> TransportDelay_IWORK_as . Head
< ( _rtDW -> TransportDelay_IWORK_as . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_as . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_as . Head == _rtDW -> TransportDelay_IWORK_as . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_as . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_as . Tail , & _rtDW -> TransportDelay_IWORK_as . Head ,
& _rtDW -> TransportDelay_IWORK_as . Last , simTime - _rtP -> P_1750 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_as . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_as . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_as . Head ] = _rtB -> B_390_2217_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l1 = _rtB -> B_390_2224_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_iw . Head = ( ( _rtDW -> TransportDelay_IWORK_iw . Head
< ( _rtDW -> TransportDelay_IWORK_iw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_iw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_iw . Head == _rtDW -> TransportDelay_IWORK_iw . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_iw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_iw . Tail , & _rtDW -> TransportDelay_IWORK_iw . Head ,
& _rtDW -> TransportDelay_IWORK_iw . Last , simTime - _rtP -> P_1760 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_iw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_iw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_iw . Head ] = _rtB -> B_390_2240_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ja = _rtB -> B_390_2247_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ai . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ai .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dq . Head = ( ( _rtDW -> TransportDelay_IWORK_dq . Head
< ( _rtDW -> TransportDelay_IWORK_dq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dq . Head == _rtDW -> TransportDelay_IWORK_dq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dq . Tail , & _rtDW -> TransportDelay_IWORK_dq . Head ,
& _rtDW -> TransportDelay_IWORK_dq . Last , simTime - _rtP -> P_1765 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = _rtB -> B_390_2248_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gr = _rtB -> B_390_2255_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dr . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_os . Head = ( ( _rtDW -> TransportDelay_IWORK_os . Head
< ( _rtDW -> TransportDelay_IWORK_os . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_os . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_os . Head == _rtDW -> TransportDelay_IWORK_os . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_os . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_os . Tail , & _rtDW -> TransportDelay_IWORK_os . Head ,
& _rtDW -> TransportDelay_IWORK_os . Last , simTime - _rtP -> P_1772 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_os . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = _rtB -> B_390_2261_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_c = _rtB -> B_390_2268_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pi . Head = ( ( _rtDW -> TransportDelay_IWORK_pi . Head
< ( _rtDW -> TransportDelay_IWORK_pi . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pi . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pi . Head == _rtDW -> TransportDelay_IWORK_pi . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pi . Tail , & _rtDW -> TransportDelay_IWORK_pi . Head ,
& _rtDW -> TransportDelay_IWORK_pi . Last , simTime - _rtP -> P_1777 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pi . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = _rtB -> B_390_2269_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mt = _rtB -> B_390_2276_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jo . Head = ( ( _rtDW -> TransportDelay_IWORK_jo . Head
< ( _rtDW -> TransportDelay_IWORK_jo . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jo . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jo . Head == _rtDW -> TransportDelay_IWORK_jo . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jo . Tail , & _rtDW -> TransportDelay_IWORK_jo . Head ,
& _rtDW -> TransportDelay_IWORK_jo . Last , simTime - _rtP -> P_1784 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jo . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jo . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jo . Head ] = _rtB -> B_390_2282_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nr = _rtB -> B_390_2289_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_my . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_my .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i2 . Head = ( ( _rtDW -> TransportDelay_IWORK_i2 . Head
< ( _rtDW -> TransportDelay_IWORK_i2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_i2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i2 . Head == _rtDW -> TransportDelay_IWORK_i2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_i2 . Tail , & _rtDW -> TransportDelay_IWORK_i2 . Head ,
& _rtDW -> TransportDelay_IWORK_i2 . Last , simTime - _rtP -> P_1789 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_i2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2 . Head ] = _rtB -> B_390_2290_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jr = _rtB -> B_390_2297_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cct . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cct .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pd . Head = ( ( _rtDW -> TransportDelay_IWORK_pd . Head
< ( _rtDW -> TransportDelay_IWORK_pd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pd . Head == _rtDW -> TransportDelay_IWORK_pd . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pd . Tail , & _rtDW -> TransportDelay_IWORK_pd . Head ,
& _rtDW -> TransportDelay_IWORK_pd . Last , simTime - _rtP -> P_1803 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = _rtB -> B_390_2321_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ex = _rtB -> B_390_2328_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_n2 . Head = ( ( _rtDW -> TransportDelay_IWORK_n2 . Head
< ( _rtDW -> TransportDelay_IWORK_n2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n2 . Head == _rtDW -> TransportDelay_IWORK_n2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n2 . Tail , & _rtDW -> TransportDelay_IWORK_n2 . Head ,
& _rtDW -> TransportDelay_IWORK_n2 . Last , simTime - _rtP -> P_1808 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_n2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_n2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_n2 . Head ] = _rtB -> B_390_2329_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_aa = _rtB -> B_390_2336_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_du . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_du .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lc . Head = ( ( _rtDW -> TransportDelay_IWORK_lc . Head
< ( _rtDW -> TransportDelay_IWORK_lc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lc . Head == _rtDW -> TransportDelay_IWORK_lc . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lc . Tail , & _rtDW -> TransportDelay_IWORK_lc . Head ,
& _rtDW -> TransportDelay_IWORK_lc . Last , simTime - _rtP -> P_1815 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lc . Head ] = _rtB -> B_390_2342_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bc = _rtB -> B_390_2349_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_el . Head = ( ( _rtDW -> TransportDelay_IWORK_el . Head
< ( _rtDW -> TransportDelay_IWORK_el . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_el . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_el . Head == _rtDW -> TransportDelay_IWORK_el . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_el . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_el . Tail , & _rtDW -> TransportDelay_IWORK_el . Head ,
& _rtDW -> TransportDelay_IWORK_el . Last , simTime - _rtP -> P_1820 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_el . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_el . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_el . Head ] = _rtB -> B_390_2350_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mo = _rtB -> B_390_2357_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ju .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jd . Head = ( ( _rtDW -> TransportDelay_IWORK_jd . Head
< ( _rtDW -> TransportDelay_IWORK_jd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jd . Head == _rtDW -> TransportDelay_IWORK_jd . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jd . Tail , & _rtDW -> TransportDelay_IWORK_jd . Head ,
& _rtDW -> TransportDelay_IWORK_jd . Last , simTime - _rtP -> P_1827 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = _rtB -> B_390_2363_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ct = _rtB -> B_390_2370_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_na . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_na .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_md . Head = ( ( _rtDW -> TransportDelay_IWORK_md . Head
< ( _rtDW -> TransportDelay_IWORK_md . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_md . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_md . Head == _rtDW -> TransportDelay_IWORK_md . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_md . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_md . Tail , & _rtDW -> TransportDelay_IWORK_md . Head ,
& _rtDW -> TransportDelay_IWORK_md . Last , simTime - _rtP -> P_1832 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_md . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_md . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_md . Head ] = _rtB -> B_390_2371_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ad = _rtB -> B_390_2378_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fqe . Head = ( ( _rtDW -> TransportDelay_IWORK_fqe .
Head < ( _rtDW -> TransportDelay_IWORK_fqe . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_fqe . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fqe . Head == _rtDW -> TransportDelay_IWORK_fqe . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fqe . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fqe . Tail , & _rtDW -> TransportDelay_IWORK_fqe . Head
, & _rtDW -> TransportDelay_IWORK_fqe . Last , simTime - _rtP -> P_1843 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fqe . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fqe . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fqe . Head ] = _rtB -> B_390_2396_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ow = _rtB -> B_390_2403_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m2 . Head = ( ( _rtDW -> TransportDelay_IWORK_m2 . Head
< ( _rtDW -> TransportDelay_IWORK_m2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m2 . Head == _rtDW -> TransportDelay_IWORK_m2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m2 . Tail , & _rtDW -> TransportDelay_IWORK_m2 . Head ,
& _rtDW -> TransportDelay_IWORK_m2 . Last , simTime - _rtP -> P_1848 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m2 . Head ] = _rtB -> B_390_2404_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dp = _rtB -> B_390_2411_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ko . Head = ( ( _rtDW -> TransportDelay_IWORK_ko . Head
< ( _rtDW -> TransportDelay_IWORK_ko . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ko . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ko . Head == _rtDW -> TransportDelay_IWORK_ko . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ko . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ko . Tail , & _rtDW -> TransportDelay_IWORK_ko . Head ,
& _rtDW -> TransportDelay_IWORK_ko . Last , simTime - _rtP -> P_1855 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ko . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ko . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ko . Head ] = _rtB -> B_390_2417_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a0 = _rtB -> B_390_2424_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ld . Head = ( ( _rtDW -> TransportDelay_IWORK_ld . Head
< ( _rtDW -> TransportDelay_IWORK_ld . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ld . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ld . Head == _rtDW -> TransportDelay_IWORK_ld . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ld . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ld . Tail , & _rtDW -> TransportDelay_IWORK_ld . Head ,
& _rtDW -> TransportDelay_IWORK_ld . Last , simTime - _rtP -> P_1860 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ld . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ld . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ld . Head ] = _rtB -> B_390_2425_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lf = _rtB -> B_390_2432_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_al . Head = ( ( _rtDW -> TransportDelay_IWORK_al . Head
< ( _rtDW -> TransportDelay_IWORK_al . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_al . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_al . Head == _rtDW -> TransportDelay_IWORK_al . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_al . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_al . Tail , & _rtDW -> TransportDelay_IWORK_al . Head ,
& _rtDW -> TransportDelay_IWORK_al . Last , simTime - _rtP -> P_1867 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_al . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_al . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_al . Head ] = _rtB -> B_390_2438_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ce = _rtB -> B_390_2445_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jdp . Head = ( ( _rtDW -> TransportDelay_IWORK_jdp .
Head < ( _rtDW -> TransportDelay_IWORK_jdp . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_jdp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jdp . Head == _rtDW -> TransportDelay_IWORK_jdp . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jdp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jdp . Tail , & _rtDW -> TransportDelay_IWORK_jdp . Head
, & _rtDW -> TransportDelay_IWORK_jdp . Last , simTime - _rtP -> P_1872 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jdp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdp . Head ] = _rtB -> B_390_2446_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ie = _rtB -> B_390_2453_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_grh . Head = ( ( _rtDW -> TransportDelay_IWORK_grh .
Head < ( _rtDW -> TransportDelay_IWORK_grh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_grh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_grh . Head == _rtDW -> TransportDelay_IWORK_grh . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_grh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_grh . Tail , & _rtDW -> TransportDelay_IWORK_grh . Head
, & _rtDW -> TransportDelay_IWORK_grh . Last , simTime - _rtP -> P_1989 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_grh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_grh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_grh . Head ] = _rtB -> B_390_2567_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fmc = _rtB -> B_390_2574_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b2 . Head = ( ( _rtDW -> TransportDelay_IWORK_b2 . Head
< ( _rtDW -> TransportDelay_IWORK_b2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b2 . Head == _rtDW -> TransportDelay_IWORK_b2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b2 . Tail , & _rtDW -> TransportDelay_IWORK_b2 . Head ,
& _rtDW -> TransportDelay_IWORK_b2 . Last , simTime - _rtP -> P_1994 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_b2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_b2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_b2 . Head ] = _rtB -> B_390_2575_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dm = _rtB -> B_390_2582_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2b . Head = ( ( _rtDW -> TransportDelay_IWORK_e2b .
Head < ( _rtDW -> TransportDelay_IWORK_e2b . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_e2b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2b . Head == _rtDW -> TransportDelay_IWORK_e2b . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2b . Tail , & _rtDW -> TransportDelay_IWORK_e2b . Head
, & _rtDW -> TransportDelay_IWORK_e2b . Last , simTime - _rtP -> P_2001 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2b . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2b . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2b . Head ] = _rtB -> B_390_2588_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_js = _rtB -> B_390_2595_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ns . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ns .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mz . Head = ( ( _rtDW -> TransportDelay_IWORK_mz . Head
< ( _rtDW -> TransportDelay_IWORK_mz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mz . Head == _rtDW -> TransportDelay_IWORK_mz . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mz . Tail , & _rtDW -> TransportDelay_IWORK_mz . Head ,
& _rtDW -> TransportDelay_IWORK_mz . Last , simTime - _rtP -> P_2006 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mz . Head ] = _rtB -> B_390_2596_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o2 = _rtB -> B_390_2603_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_be .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fg . Head = ( ( _rtDW -> TransportDelay_IWORK_fg . Head
< ( _rtDW -> TransportDelay_IWORK_fg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fg . Head == _rtDW -> TransportDelay_IWORK_fg . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fg . Tail , & _rtDW -> TransportDelay_IWORK_fg . Head ,
& _rtDW -> TransportDelay_IWORK_fg . Last , simTime - _rtP -> P_2013 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fg . Head ] = _rtB -> B_390_2609_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eq5 = _rtB -> B_390_2616_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_da . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_da .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ji . Head = ( ( _rtDW -> TransportDelay_IWORK_ji . Head
< ( _rtDW -> TransportDelay_IWORK_ji . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ji . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ji . Head == _rtDW -> TransportDelay_IWORK_ji . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ji . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ji . Tail , & _rtDW -> TransportDelay_IWORK_ji . Head ,
& _rtDW -> TransportDelay_IWORK_ji . Last , simTime - _rtP -> P_2018 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ji . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = _rtB -> B_390_2617_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b1 = _rtB -> B_390_2624_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ln . Head = ( ( _rtDW -> TransportDelay_IWORK_ln . Head
< ( _rtDW -> TransportDelay_IWORK_ln . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ln . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ln . Head == _rtDW -> TransportDelay_IWORK_ln . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ln . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ln . Tail , & _rtDW -> TransportDelay_IWORK_ln . Head ,
& _rtDW -> TransportDelay_IWORK_ln . Last , simTime - _rtP -> P_2028 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ln . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = _rtB -> B_390_2640_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pzf = _rtB -> B_390_2647_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_po . Head = ( ( _rtDW -> TransportDelay_IWORK_po . Head
< ( _rtDW -> TransportDelay_IWORK_po . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_po . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_po . Head == _rtDW -> TransportDelay_IWORK_po . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_po . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_po . Tail , & _rtDW -> TransportDelay_IWORK_po . Head ,
& _rtDW -> TransportDelay_IWORK_po . Last , simTime - _rtP -> P_2033 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_po . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_po . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_po . Head ] = _rtB -> B_390_2648_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fc = _rtB -> B_390_2655_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nl . Head = ( ( _rtDW -> TransportDelay_IWORK_nl . Head
< ( _rtDW -> TransportDelay_IWORK_nl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nl . Head == _rtDW -> TransportDelay_IWORK_nl . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nl . Tail , & _rtDW -> TransportDelay_IWORK_nl . Head ,
& _rtDW -> TransportDelay_IWORK_nl . Last , simTime - _rtP -> P_2040 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nl . Head ] = _rtB -> B_390_2661_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jd = _rtB -> B_390_2668_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_px . Head = ( ( _rtDW -> TransportDelay_IWORK_px . Head
< ( _rtDW -> TransportDelay_IWORK_px . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_px . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_px . Head == _rtDW -> TransportDelay_IWORK_px . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_px . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_px . Tail , & _rtDW -> TransportDelay_IWORK_px . Head ,
& _rtDW -> TransportDelay_IWORK_px . Last , simTime - _rtP -> P_2045 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_px . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_px . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_px . Head ] = _rtB -> B_390_2669_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mw = _rtB -> B_390_2676_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_io . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_io .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kx . Head = ( ( _rtDW -> TransportDelay_IWORK_kx . Head
< ( _rtDW -> TransportDelay_IWORK_kx . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kx . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kx . Head == _rtDW -> TransportDelay_IWORK_kx . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kx . Tail , & _rtDW -> TransportDelay_IWORK_kx . Head ,
& _rtDW -> TransportDelay_IWORK_kx . Last , simTime - _rtP -> P_2052 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kx . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kx . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kx . Head ] = _rtB -> B_390_2682_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_aa3 = _rtB -> B_390_2689_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ff . Head = ( ( _rtDW -> TransportDelay_IWORK_ff . Head
< ( _rtDW -> TransportDelay_IWORK_ff . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ff . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ff . Head == _rtDW -> TransportDelay_IWORK_ff . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ff . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ff . Tail , & _rtDW -> TransportDelay_IWORK_ff . Head ,
& _rtDW -> TransportDelay_IWORK_ff . Last , simTime - _rtP -> P_2057 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ff . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ff . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ff . Head ] = _rtB -> B_390_2690_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dj = _rtB -> B_390_2697_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lh . Head = ( ( _rtDW -> TransportDelay_IWORK_lh . Head
< ( _rtDW -> TransportDelay_IWORK_lh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lh . Head == _rtDW -> TransportDelay_IWORK_lh . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lh . Tail , & _rtDW -> TransportDelay_IWORK_lh . Head ,
& _rtDW -> TransportDelay_IWORK_lh . Last , simTime - _rtP -> P_2071 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lh . Head ] = _rtB -> B_390_2721_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jw = _rtB -> B_390_2728_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kt . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kt .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ip . Head = ( ( _rtDW -> TransportDelay_IWORK_ip . Head
< ( _rtDW -> TransportDelay_IWORK_ip . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ip . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ip . Head == _rtDW -> TransportDelay_IWORK_ip . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ip . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ip . Tail , & _rtDW -> TransportDelay_IWORK_ip . Head ,
& _rtDW -> TransportDelay_IWORK_ip . Last , simTime - _rtP -> P_2076 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ip . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ip . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ip . Head ] = _rtB -> B_390_2729_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b2 = _rtB -> B_390_2736_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oe .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_of . Head = ( ( _rtDW -> TransportDelay_IWORK_of . Head
< ( _rtDW -> TransportDelay_IWORK_of . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_of . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_of . Head == _rtDW -> TransportDelay_IWORK_of . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_of . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_of . Tail , & _rtDW -> TransportDelay_IWORK_of . Head ,
& _rtDW -> TransportDelay_IWORK_of . Last , simTime - _rtP -> P_2083 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_of . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_of . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_of . Head ] = _rtB -> B_390_2742_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fw = _rtB -> B_390_2749_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lk . Head = ( ( _rtDW -> TransportDelay_IWORK_lk . Head
< ( _rtDW -> TransportDelay_IWORK_lk . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lk . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lk . Head == _rtDW -> TransportDelay_IWORK_lk . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lk . Tail , & _rtDW -> TransportDelay_IWORK_lk . Head ,
& _rtDW -> TransportDelay_IWORK_lk . Last , simTime - _rtP -> P_2088 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lk . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lk . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lk . Head ] = _rtB -> B_390_2750_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ml = _rtB -> B_390_2757_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pj . Head = ( ( _rtDW -> TransportDelay_IWORK_pj . Head
< ( _rtDW -> TransportDelay_IWORK_pj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pj . Head == _rtDW -> TransportDelay_IWORK_pj . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pj . Tail , & _rtDW -> TransportDelay_IWORK_pj . Head ,
& _rtDW -> TransportDelay_IWORK_pj . Last , simTime - _rtP -> P_2095 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = _rtB -> B_390_2763_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_n1 = _rtB -> B_390_2770_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_em . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_em .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l5 . Head = ( ( _rtDW -> TransportDelay_IWORK_l5 . Head
< ( _rtDW -> TransportDelay_IWORK_l5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l5 . Head == _rtDW -> TransportDelay_IWORK_l5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l5 . Tail , & _rtDW -> TransportDelay_IWORK_l5 . Head ,
& _rtDW -> TransportDelay_IWORK_l5 . Last , simTime - _rtP -> P_2100 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l5 . Head ] = _rtB -> B_390_2771_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i5 = _rtB -> B_390_2778_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ki . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ki .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jy . Head = ( ( _rtDW -> TransportDelay_IWORK_jy . Head
< ( _rtDW -> TransportDelay_IWORK_jy . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jy . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jy . Head == _rtDW -> TransportDelay_IWORK_jy . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jy . Tail , & _rtDW -> TransportDelay_IWORK_jy . Head ,
& _rtDW -> TransportDelay_IWORK_jy . Last , simTime - _rtP -> P_2111 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jy . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = _rtB -> B_390_2796_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hz = _rtB -> B_390_2803_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ev . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ev .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ly . Head = ( ( _rtDW -> TransportDelay_IWORK_ly . Head
< ( _rtDW -> TransportDelay_IWORK_ly . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ly . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ly . Head == _rtDW -> TransportDelay_IWORK_ly . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ly . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ly . Tail , & _rtDW -> TransportDelay_IWORK_ly . Head ,
& _rtDW -> TransportDelay_IWORK_ly . Last , simTime - _rtP -> P_2116 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ly . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ly . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ly . Head ] = _rtB -> B_390_2804_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lk = _rtB -> B_390_2811_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oeh . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oeh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oq . Head = ( ( _rtDW -> TransportDelay_IWORK_oq . Head
< ( _rtDW -> TransportDelay_IWORK_oq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oq . Head == _rtDW -> TransportDelay_IWORK_oq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oq . Tail , & _rtDW -> TransportDelay_IWORK_oq . Head ,
& _rtDW -> TransportDelay_IWORK_oq . Last , simTime - _rtP -> P_2123 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oq . Head ] = _rtB -> B_390_2817_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gj = _rtB -> B_390_2824_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pr . Head = ( ( _rtDW -> TransportDelay_IWORK_pr . Head
< ( _rtDW -> TransportDelay_IWORK_pr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pr . Head == _rtDW -> TransportDelay_IWORK_pr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pr . Tail , & _rtDW -> TransportDelay_IWORK_pr . Head ,
& _rtDW -> TransportDelay_IWORK_pr . Last , simTime - _rtP -> P_2128 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pr . Head ] = _rtB -> B_390_2825_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gl = _rtB -> B_390_2832_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nt . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kj . Head = ( ( _rtDW -> TransportDelay_IWORK_kj . Head
< ( _rtDW -> TransportDelay_IWORK_kj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kj . Head == _rtDW -> TransportDelay_IWORK_kj . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kj . Tail , & _rtDW -> TransportDelay_IWORK_kj . Head ,
& _rtDW -> TransportDelay_IWORK_kj . Last , simTime - _rtP -> P_2135 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kj . Head ] = _rtB -> B_390_2838_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pd = _rtB -> B_390_2845_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pn . Head = ( ( _rtDW -> TransportDelay_IWORK_pn . Head
< ( _rtDW -> TransportDelay_IWORK_pn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pn . Head == _rtDW -> TransportDelay_IWORK_pn . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pn . Tail , & _rtDW -> TransportDelay_IWORK_pn . Head ,
& _rtDW -> TransportDelay_IWORK_pn . Last , simTime - _rtP -> P_2140 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pn . Head ] = _rtB -> B_390_2846_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ey = _rtB -> B_390_2853_0 ; } if ( _rtDW ->
TimeStampA_nv == ( rtInf ) ) { _rtDW -> TimeStampA_nv = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_d ; } else if ( _rtDW -> TimeStampB_g == ( rtInf )
) { _rtDW -> TimeStampB_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_dr
; } else if ( _rtDW -> TimeStampA_nv < _rtDW -> TimeStampB_g ) { _rtDW ->
TimeStampA_nv = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_d ; } else {
_rtDW -> TimeStampB_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_dr ; }
* lastU = _rtB -> B_390_2976_0 ; if ( _rtDW -> TimeStampA_l == ( rtInf ) ) {
_rtDW -> TimeStampA_l = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_ca ; }
else if ( _rtDW -> TimeStampB_ic == ( rtInf ) ) { _rtDW -> TimeStampB_ic =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_b ; } else if ( _rtDW ->
TimeStampA_l < _rtDW -> TimeStampB_ic ) { _rtDW -> TimeStampA_l = ssGetT ( S
) ; lastU = & _rtDW -> LastUAtTimeA_ca ; } else { _rtDW -> TimeStampB_ic =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_b ; } * lastU = _rtB ->
B_390_2988_0 ; if ( _rtDW -> TimeStampA_m == ( rtInf ) ) { _rtDW ->
TimeStampA_m = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_e ; } else if (
_rtDW -> TimeStampB_ka == ( rtInf ) ) { _rtDW -> TimeStampB_ka = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeB_c ; } else if ( _rtDW -> TimeStampA_m <
_rtDW -> TimeStampB_ka ) { _rtDW -> TimeStampA_m = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeA_e ; } else { _rtDW -> TimeStampB_ka = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_c ; } * lastU = _rtB -> B_390_3000_0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dt .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dt . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_ea5 . Head = ( ( _rtDW ->
TransportDelay_IWORK_ea5 . Head < ( _rtDW -> TransportDelay_IWORK_ea5 .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ea5 . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_ea5 . Head == _rtDW ->
TransportDelay_IWORK_ea5 . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ea5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ea5 . Tail , & _rtDW -> TransportDelay_IWORK_ea5 . Head
, & _rtDW -> TransportDelay_IWORK_ea5 . Last , simTime - _rtP -> P_2284 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ea5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea5 . Head ] = _rtB -> B_390_3019_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o4 = _rtB -> B_390_3026_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eg . Head = ( ( _rtDW -> TransportDelay_IWORK_eg . Head
< ( _rtDW -> TransportDelay_IWORK_eg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eg . Head == _rtDW -> TransportDelay_IWORK_eg . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eg . Tail , & _rtDW -> TransportDelay_IWORK_eg . Head ,
& _rtDW -> TransportDelay_IWORK_eg . Last , simTime - _rtP -> P_2289 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eg . Head ] = _rtB -> B_390_3027_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nx = _rtB -> B_390_3034_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_br . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kk . Head = ( ( _rtDW -> TransportDelay_IWORK_kk . Head
< ( _rtDW -> TransportDelay_IWORK_kk . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kk . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kk . Head == _rtDW -> TransportDelay_IWORK_kk . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kk . Tail , & _rtDW -> TransportDelay_IWORK_kk . Head ,
& _rtDW -> TransportDelay_IWORK_kk . Last , simTime - _rtP -> P_2296 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kk . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kk . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kk . Head ] = _rtB -> B_390_3040_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mq = _rtB -> B_390_3047_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he5 . Head = ( ( _rtDW -> TransportDelay_IWORK_he5 .
Head < ( _rtDW -> TransportDelay_IWORK_he5 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_he5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he5 . Head == _rtDW -> TransportDelay_IWORK_he5 . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_he5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he5 . Tail , & _rtDW -> TransportDelay_IWORK_he5 . Head
, & _rtDW -> TransportDelay_IWORK_he5 . Last , simTime - _rtP -> P_2301 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5 . Head ] = _rtB -> B_390_3048_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bq = _rtB -> B_390_3055_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dp . Head = ( ( _rtDW -> TransportDelay_IWORK_dp . Head
< ( _rtDW -> TransportDelay_IWORK_dp . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dp . Head == _rtDW -> TransportDelay_IWORK_dp . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dp . Tail , & _rtDW -> TransportDelay_IWORK_dp . Head ,
& _rtDW -> TransportDelay_IWORK_dp . Last , simTime - _rtP -> P_2308 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dp . Head ] = _rtB -> B_390_3061_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ep = _rtB -> B_390_3068_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ac . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ac .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_la . Head = ( ( _rtDW -> TransportDelay_IWORK_la . Head
< ( _rtDW -> TransportDelay_IWORK_la . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_la . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_la . Head == _rtDW -> TransportDelay_IWORK_la . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_la . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_la . Tail , & _rtDW -> TransportDelay_IWORK_la . Head ,
& _rtDW -> TransportDelay_IWORK_la . Last , simTime - _rtP -> P_2313 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_la . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_la . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_la . Head ] = _rtB -> B_390_3069_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ms = _rtB -> B_390_3076_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ev . Head = ( ( _rtDW -> TransportDelay_IWORK_ev . Head
< ( _rtDW -> TransportDelay_IWORK_ev . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ev . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ev . Head == _rtDW -> TransportDelay_IWORK_ev . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ev . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ev . Tail , & _rtDW -> TransportDelay_IWORK_ev . Head ,
& _rtDW -> TransportDelay_IWORK_ev . Last , simTime - _rtP -> P_2323 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ev . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ev . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ev . Head ] = _rtB -> B_390_3092_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dw = _rtB -> B_390_3099_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gl . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b1 . Head = ( ( _rtDW -> TransportDelay_IWORK_b1 . Head
< ( _rtDW -> TransportDelay_IWORK_b1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b1 . Head == _rtDW -> TransportDelay_IWORK_b1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b1 . Tail , & _rtDW -> TransportDelay_IWORK_b1 . Head ,
& _rtDW -> TransportDelay_IWORK_b1 . Last , simTime - _rtP -> P_2328 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_b1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_b1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_b1 . Head ] = _rtB -> B_390_3100_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jh = _rtB -> B_390_3107_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nq . Head = ( ( _rtDW -> TransportDelay_IWORK_nq . Head
< ( _rtDW -> TransportDelay_IWORK_nq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nq . Head == _rtDW -> TransportDelay_IWORK_nq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nq . Tail , & _rtDW -> TransportDelay_IWORK_nq . Head ,
& _rtDW -> TransportDelay_IWORK_nq . Last , simTime - _rtP -> P_2335 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = _rtB -> B_390_3113_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fx = _rtB -> B_390_3120_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ag . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ag .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he5l . Head = ( ( _rtDW -> TransportDelay_IWORK_he5l .
Head < ( _rtDW -> TransportDelay_IWORK_he5l . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_he5l . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he5l . Head == _rtDW -> TransportDelay_IWORK_he5l . Tail
) { if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_he5l . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he5l . Tail , & _rtDW -> TransportDelay_IWORK_he5l .
Head , & _rtDW -> TransportDelay_IWORK_he5l . Last , simTime - _rtP -> P_2340
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he5l . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5l . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5l . Head ] = _rtB -> B_390_3121_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a2 = _rtB -> B_390_3128_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p4 . Head = ( ( _rtDW -> TransportDelay_IWORK_p4 . Head
< ( _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p4 . Head == _rtDW -> TransportDelay_IWORK_p4 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4 . Tail , & _rtDW -> TransportDelay_IWORK_p4 . Head ,
& _rtDW -> TransportDelay_IWORK_p4 . Last , simTime - _rtP -> P_2347 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = _rtB -> B_390_3134_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cs = _rtB -> B_390_3141_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_og . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_og .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kjd . Head = ( ( _rtDW -> TransportDelay_IWORK_kjd .
Head < ( _rtDW -> TransportDelay_IWORK_kjd . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_kjd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kjd . Head == _rtDW -> TransportDelay_IWORK_kjd . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kjd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kjd . Tail , & _rtDW -> TransportDelay_IWORK_kjd . Head
, & _rtDW -> TransportDelay_IWORK_kjd . Last , simTime - _rtP -> P_2352 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kjd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kjd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kjd . Head ] = _rtB -> B_390_3142_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fi = _rtB -> B_390_3149_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cv . Head = ( ( _rtDW -> TransportDelay_IWORK_cv . Head
< ( _rtDW -> TransportDelay_IWORK_cv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cv . Head == _rtDW -> TransportDelay_IWORK_cv . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cv . Tail , & _rtDW -> TransportDelay_IWORK_cv . Head ,
& _rtDW -> TransportDelay_IWORK_cv . Last , simTime - _rtP -> P_2366 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cv . Head ] = _rtB -> B_390_3173_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m3 = _rtB -> B_390_3180_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p4s . Head = ( ( _rtDW -> TransportDelay_IWORK_p4s .
Head < ( _rtDW -> TransportDelay_IWORK_p4s . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_p4s . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p4s . Head == _rtDW -> TransportDelay_IWORK_p4s . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p4s . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4s . Tail , & _rtDW -> TransportDelay_IWORK_p4s . Head
, & _rtDW -> TransportDelay_IWORK_p4s . Last , simTime - _rtP -> P_2371 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p4s . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4s . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4s . Head ] = _rtB -> B_390_3181_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pu = _rtB -> B_390_3188_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ce . Head = ( ( _rtDW -> TransportDelay_IWORK_ce . Head
< ( _rtDW -> TransportDelay_IWORK_ce . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ce . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ce . Head == _rtDW -> TransportDelay_IWORK_ce . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ce . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ce . Tail , & _rtDW -> TransportDelay_IWORK_ce . Head ,
& _rtDW -> TransportDelay_IWORK_ce . Last , simTime - _rtP -> P_2378 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ce . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ce . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ce . Head ] = _rtB -> B_390_3194_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_oz = _rtB -> B_390_3201_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ck . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ck .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cc . Head = ( ( _rtDW -> TransportDelay_IWORK_cc . Head
< ( _rtDW -> TransportDelay_IWORK_cc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cc . Head == _rtDW -> TransportDelay_IWORK_cc . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cc . Tail , & _rtDW -> TransportDelay_IWORK_cc . Head ,
& _rtDW -> TransportDelay_IWORK_cc . Last , simTime - _rtP -> P_2383 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cc . Head ] = _rtB -> B_390_3202_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hjp = _rtB -> B_390_3209_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ni . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ni .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2t . Head = ( ( _rtDW -> TransportDelay_IWORK_e2t .
Head < ( _rtDW -> TransportDelay_IWORK_e2t . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_e2t . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2t . Head == _rtDW -> TransportDelay_IWORK_e2t . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2t . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2t . Tail , & _rtDW -> TransportDelay_IWORK_e2t . Head
, & _rtDW -> TransportDelay_IWORK_e2t . Last , simTime - _rtP -> P_2390 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2t . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2t . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2t . Head ] = _rtB -> B_390_3215_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kt = _rtB -> B_390_3222_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ex . Head = ( ( _rtDW -> TransportDelay_IWORK_ex . Head
< ( _rtDW -> TransportDelay_IWORK_ex . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ex . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ex . Head == _rtDW -> TransportDelay_IWORK_ex . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ex . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ex . Tail , & _rtDW -> TransportDelay_IWORK_ex . Head ,
& _rtDW -> TransportDelay_IWORK_ex . Last , simTime - _rtP -> P_2395 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ex . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ex . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ex . Head ] = _rtB -> B_390_3223_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kk = _rtB -> B_390_3230_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p5 . Head = ( ( _rtDW -> TransportDelay_IWORK_p5 . Head
< ( _rtDW -> TransportDelay_IWORK_p5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p5 . Head == _rtDW -> TransportDelay_IWORK_p5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p5 . Tail , & _rtDW -> TransportDelay_IWORK_p5 . Head ,
& _rtDW -> TransportDelay_IWORK_p5 . Last , simTime - _rtP -> P_2406 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p5 . Head ] = _rtB -> B_390_3248_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g3 = _rtB -> B_390_3255_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ld . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ld .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eu . Head = ( ( _rtDW -> TransportDelay_IWORK_eu . Head
< ( _rtDW -> TransportDelay_IWORK_eu . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eu . Head == _rtDW -> TransportDelay_IWORK_eu . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eu . Tail , & _rtDW -> TransportDelay_IWORK_eu . Head ,
& _rtDW -> TransportDelay_IWORK_eu . Last , simTime - _rtP -> P_2411 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eu . Head ] = _rtB -> B_390_3256_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o3 = _rtB -> B_390_3263_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i2y . Head = ( ( _rtDW -> TransportDelay_IWORK_i2y .
Head < ( _rtDW -> TransportDelay_IWORK_i2y . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_i2y . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i2y . Head == _rtDW -> TransportDelay_IWORK_i2y . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i2y . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_i2y . Tail , & _rtDW -> TransportDelay_IWORK_i2y . Head
, & _rtDW -> TransportDelay_IWORK_i2y . Last , simTime - _rtP -> P_2418 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_i2y . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2y . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2y . Head ] = _rtB -> B_390_3269_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dl = _rtB -> B_390_3276_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ep . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ep .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lna . Head = ( ( _rtDW -> TransportDelay_IWORK_lna .
Head < ( _rtDW -> TransportDelay_IWORK_lna . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_lna . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lna . Head == _rtDW -> TransportDelay_IWORK_lna . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lna . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lna . Tail , & _rtDW -> TransportDelay_IWORK_lna . Head
, & _rtDW -> TransportDelay_IWORK_lna . Last , simTime - _rtP -> P_2423 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lna . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lna . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lna . Head ] = _rtB -> B_390_3277_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_by = _rtB -> B_390_3284_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ln . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kw . Head = ( ( _rtDW -> TransportDelay_IWORK_kw . Head
< ( _rtDW -> TransportDelay_IWORK_kw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kw . Head == _rtDW -> TransportDelay_IWORK_kw . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kw . Tail , & _rtDW -> TransportDelay_IWORK_kw . Head ,
& _rtDW -> TransportDelay_IWORK_kw . Last , simTime - _rtP -> P_2430 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kw . Head ] = _rtB -> B_390_3290_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g3o = _rtB -> B_390_3297_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bf . Head = ( ( _rtDW -> TransportDelay_IWORK_bf . Head
< ( _rtDW -> TransportDelay_IWORK_bf . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bf . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bf . Head == _rtDW -> TransportDelay_IWORK_bf . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bf . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bf . Tail , & _rtDW -> TransportDelay_IWORK_bf . Head ,
& _rtDW -> TransportDelay_IWORK_bf . Last , simTime - _rtP -> P_2435 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bf . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bf . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bf . Head ] = _rtB -> B_390_3298_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m1 = _rtB -> B_390_3305_0 ; } if ( _rtDW ->
TimeStampA_mm == ( rtInf ) ) { _rtDW -> TimeStampA_mm = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_ed ; } else if ( _rtDW -> TimeStampB_eo == ( rtInf
) ) { _rtDW -> TimeStampB_eo = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_i ; } else if ( _rtDW -> TimeStampA_mm < _rtDW -> TimeStampB_eo
) { _rtDW -> TimeStampA_mm = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_ed ; } else { _rtDW -> TimeStampB_eo = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_i ; } * lastU = _rtB -> B_390_3428_0 ; if ( _rtDW ->
TimeStampA_j == ( rtInf ) ) { _rtDW -> TimeStampA_j = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_dt ; } else if ( _rtDW -> TimeStampB_n == ( rtInf ) )
{ _rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_po ;
} else if ( _rtDW -> TimeStampA_j < _rtDW -> TimeStampB_n ) { _rtDW ->
TimeStampA_j = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_dt ; } else {
_rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_po ; }
* lastU = _rtB -> B_390_3440_0 ; if ( _rtDW -> TimeStampA_g == ( rtInf ) ) {
_rtDW -> TimeStampA_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_g ; }
else if ( _rtDW -> TimeStampB_d == ( rtInf ) ) { _rtDW -> TimeStampB_d =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_mm ; } else if ( _rtDW ->
TimeStampA_g < _rtDW -> TimeStampB_d ) { _rtDW -> TimeStampA_g = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_g ; } else { _rtDW -> TimeStampB_d = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB_mm ; } * lastU = _rtB -> B_390_3452_0
; { real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ca .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ca . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_kd . Head = ( ( _rtDW ->
TransportDelay_IWORK_kd . Head < ( _rtDW -> TransportDelay_IWORK_kd .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_kd . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_kd . Head == _rtDW ->
TransportDelay_IWORK_kd . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kd . Tail , & _rtDW -> TransportDelay_IWORK_kd . Head ,
& _rtDW -> TransportDelay_IWORK_kd . Last , simTime - _rtP -> P_2579 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kd . Head ] = _rtB -> B_390_3471_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ev = _rtB -> B_390_3478_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_in . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_in .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mm . Head = ( ( _rtDW -> TransportDelay_IWORK_mm . Head
< ( _rtDW -> TransportDelay_IWORK_mm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mm . Head == _rtDW -> TransportDelay_IWORK_mm . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mm . Tail , & _rtDW -> TransportDelay_IWORK_mm . Head ,
& _rtDW -> TransportDelay_IWORK_mm . Last , simTime - _rtP -> P_2584 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mm . Head ] = _rtB -> B_390_3479_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fis = _rtB -> B_390_3486_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ec . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gn . Head = ( ( _rtDW -> TransportDelay_IWORK_gn . Head
< ( _rtDW -> TransportDelay_IWORK_gn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gn . Head == _rtDW -> TransportDelay_IWORK_gn . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gn . Tail , & _rtDW -> TransportDelay_IWORK_gn . Head ,
& _rtDW -> TransportDelay_IWORK_gn . Last , simTime - _rtP -> P_2591 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gn . Head ] = _rtB -> B_390_3492_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ij = _rtB -> B_390_3499_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fd . Head = ( ( _rtDW -> TransportDelay_IWORK_fd . Head
< ( _rtDW -> TransportDelay_IWORK_fd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fd . Head == _rtDW -> TransportDelay_IWORK_fd . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fd . Tail , & _rtDW -> TransportDelay_IWORK_fd . Head ,
& _rtDW -> TransportDelay_IWORK_fd . Last , simTime - _rtP -> P_2596 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fd . Head ] = _rtB -> B_390_3500_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jy = _rtB -> B_390_3507_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d53 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d53 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gz . Head = ( ( _rtDW -> TransportDelay_IWORK_gz . Head
< ( _rtDW -> TransportDelay_IWORK_gz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gz . Head == _rtDW -> TransportDelay_IWORK_gz . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gz . Tail , & _rtDW -> TransportDelay_IWORK_gz . Head ,
& _rtDW -> TransportDelay_IWORK_gz . Last , simTime - _rtP -> P_2603 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gz . Head ] = _rtB -> B_390_3513_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_grf = _rtB -> B_390_3520_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ee . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ee .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ma4 . Head = ( ( _rtDW -> TransportDelay_IWORK_ma4 .
Head < ( _rtDW -> TransportDelay_IWORK_ma4 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ma4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ma4 . Head == _rtDW -> TransportDelay_IWORK_ma4 . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ma4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ma4 . Tail , & _rtDW -> TransportDelay_IWORK_ma4 . Head
, & _rtDW -> TransportDelay_IWORK_ma4 . Last , simTime - _rtP -> P_2608 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ma4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma4 . Head ] = _rtB -> B_390_3521_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pe = _rtB -> B_390_3528_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kl . Head = ( ( _rtDW -> TransportDelay_IWORK_kl . Head
< ( _rtDW -> TransportDelay_IWORK_kl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kl . Head == _rtDW -> TransportDelay_IWORK_kl . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kl . Tail , & _rtDW -> TransportDelay_IWORK_kl . Head ,
& _rtDW -> TransportDelay_IWORK_kl . Last , simTime - _rtP -> P_2618 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kl . Head ] = _rtB -> B_390_3544_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ic = _rtB -> B_390_3551_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_er . Head = ( ( _rtDW -> TransportDelay_IWORK_er . Head
< ( _rtDW -> TransportDelay_IWORK_er . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_er . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_er . Head == _rtDW -> TransportDelay_IWORK_er . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_er . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_er . Tail , & _rtDW -> TransportDelay_IWORK_er . Head ,
& _rtDW -> TransportDelay_IWORK_er . Last , simTime - _rtP -> P_2623 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_er . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = _rtB -> B_390_3552_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_k0 = _rtB -> B_390_3559_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hq . Head = ( ( _rtDW -> TransportDelay_IWORK_hq . Head
< ( _rtDW -> TransportDelay_IWORK_hq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hq . Head == _rtDW -> TransportDelay_IWORK_hq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hq . Tail , & _rtDW -> TransportDelay_IWORK_hq . Head ,
& _rtDW -> TransportDelay_IWORK_hq . Last , simTime - _rtP -> P_2630 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hq . Head ] = _rtB -> B_390_3565_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ofm = _rtB -> B_390_3572_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj2 . Head = ( ( _rtDW -> TransportDelay_IWORK_hj2 .
Head < ( _rtDW -> TransportDelay_IWORK_hj2 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hj2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj2 . Head == _rtDW -> TransportDelay_IWORK_hj2 . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj2 . Tail , & _rtDW -> TransportDelay_IWORK_hj2 . Head
, & _rtDW -> TransportDelay_IWORK_hj2 . Last , simTime - _rtP -> P_2635 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj2 . Head ] = _rtB -> B_390_3573_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ico = _rtB -> B_390_3580_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_atb . Head = ( ( _rtDW -> TransportDelay_IWORK_atb .
Head < ( _rtDW -> TransportDelay_IWORK_atb . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_atb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_atb . Head == _rtDW -> TransportDelay_IWORK_atb . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_atb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_atb . Tail , & _rtDW -> TransportDelay_IWORK_atb . Head
, & _rtDW -> TransportDelay_IWORK_atb . Last , simTime - _rtP -> P_2642 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_atb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_atb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_atb . Head ] = _rtB -> B_390_3586_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ji = _rtB -> B_390_3593_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_or . Head = ( ( _rtDW -> TransportDelay_IWORK_or . Head
< ( _rtDW -> TransportDelay_IWORK_or . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_or . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_or . Head == _rtDW -> TransportDelay_IWORK_or . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_or . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_or . Tail , & _rtDW -> TransportDelay_IWORK_or . Head ,
& _rtDW -> TransportDelay_IWORK_or . Last , simTime - _rtP -> P_2647 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_or . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = _rtB -> B_390_3594_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nm = _rtB -> B_390_3601_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cmj . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cmj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dw . Head = ( ( _rtDW -> TransportDelay_IWORK_dw . Head
< ( _rtDW -> TransportDelay_IWORK_dw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dw . Head == _rtDW -> TransportDelay_IWORK_dw . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dw . Tail , & _rtDW -> TransportDelay_IWORK_dw . Head ,
& _rtDW -> TransportDelay_IWORK_dw . Last , simTime - _rtP -> P_2661 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dw . Head ] = _rtB -> B_390_3625_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jwz = _rtB -> B_390_3632_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c5 . Head = ( ( _rtDW -> TransportDelay_IWORK_c5 . Head
< ( _rtDW -> TransportDelay_IWORK_c5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c5 . Head == _rtDW -> TransportDelay_IWORK_c5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_c5 . Tail , & _rtDW -> TransportDelay_IWORK_c5 . Head ,
& _rtDW -> TransportDelay_IWORK_c5 . Last , simTime - _rtP -> P_2666 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_c5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_c5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_c5 . Head ] = _rtB -> B_390_3633_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lb = _rtB -> B_390_3640_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj0 . Head = ( ( _rtDW -> TransportDelay_IWORK_hj0 .
Head < ( _rtDW -> TransportDelay_IWORK_hj0 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hj0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj0 . Head == _rtDW -> TransportDelay_IWORK_hj0 . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj0 . Tail , & _rtDW -> TransportDelay_IWORK_hj0 . Head
, & _rtDW -> TransportDelay_IWORK_hj0 . Last , simTime - _rtP -> P_2673 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj0 . Head ] = _rtB -> B_390_3646_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ao = _rtB -> B_390_3653_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_da . Head = ( ( _rtDW -> TransportDelay_IWORK_da . Head
< ( _rtDW -> TransportDelay_IWORK_da . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_da . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_da . Head == _rtDW -> TransportDelay_IWORK_da . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_da . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_da . Tail , & _rtDW -> TransportDelay_IWORK_da . Head ,
& _rtDW -> TransportDelay_IWORK_da . Last , simTime - _rtP -> P_2678 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_da . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_da . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_da . Head ] = _rtB -> B_390_3654_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kf = _rtB -> B_390_3661_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ig . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ig .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j0 . Head = ( ( _rtDW -> TransportDelay_IWORK_j0 . Head
< ( _rtDW -> TransportDelay_IWORK_j0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j0 . Head == _rtDW -> TransportDelay_IWORK_j0 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j0 . Tail , & _rtDW -> TransportDelay_IWORK_j0 . Head ,
& _rtDW -> TransportDelay_IWORK_j0 . Last , simTime - _rtP -> P_2685 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_j0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_j0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_j0 . Head ] = _rtB -> B_390_3667_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pzv = _rtB -> B_390_3674_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nd . Head = ( ( _rtDW -> TransportDelay_IWORK_nd . Head
< ( _rtDW -> TransportDelay_IWORK_nd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nd . Head == _rtDW -> TransportDelay_IWORK_nd . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nd . Tail , & _rtDW -> TransportDelay_IWORK_nd . Head ,
& _rtDW -> TransportDelay_IWORK_nd . Last , simTime - _rtP -> P_2690 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nd . Head ] = _rtB -> B_390_3675_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kv = _rtB -> B_390_3682_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hs . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hs .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ou . Head = ( ( _rtDW -> TransportDelay_IWORK_ou . Head
< ( _rtDW -> TransportDelay_IWORK_ou . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ou . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ou . Head == _rtDW -> TransportDelay_IWORK_ou . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ou . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ou . Tail , & _rtDW -> TransportDelay_IWORK_ou . Head ,
& _rtDW -> TransportDelay_IWORK_ou . Last , simTime - _rtP -> P_2701 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ou . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ou . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ou . Head ] = _rtB -> B_390_3700_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m2 = _rtB -> B_390_3707_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_my . Head = ( ( _rtDW -> TransportDelay_IWORK_my . Head
< ( _rtDW -> TransportDelay_IWORK_my . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_my . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_my . Head == _rtDW -> TransportDelay_IWORK_my . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_my . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_my . Tail , & _rtDW -> TransportDelay_IWORK_my . Head ,
& _rtDW -> TransportDelay_IWORK_my . Last , simTime - _rtP -> P_2706 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_my . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_my . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_my . Head ] = _rtB -> B_390_3708_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_il = _rtB -> B_390_3715_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ayh . Head = ( ( _rtDW -> TransportDelay_IWORK_ayh .
Head < ( _rtDW -> TransportDelay_IWORK_ayh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ayh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ayh . Head == _rtDW -> TransportDelay_IWORK_ayh . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ayh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ayh . Tail , & _rtDW -> TransportDelay_IWORK_ayh . Head
, & _rtDW -> TransportDelay_IWORK_ayh . Last , simTime - _rtP -> P_2713 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ayh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayh . Head ] = _rtB -> B_390_3721_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pi = _rtB -> B_390_3728_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jyu . Head = ( ( _rtDW -> TransportDelay_IWORK_jyu .
Head < ( _rtDW -> TransportDelay_IWORK_jyu . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_jyu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jyu . Head == _rtDW -> TransportDelay_IWORK_jyu . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jyu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jyu . Tail , & _rtDW -> TransportDelay_IWORK_jyu . Head
, & _rtDW -> TransportDelay_IWORK_jyu . Last , simTime - _rtP -> P_2718 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jyu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jyu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jyu . Head ] = _rtB -> B_390_3729_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ol = _rtB -> B_390_3736_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ps . Head = ( ( _rtDW -> TransportDelay_IWORK_ps . Head
< ( _rtDW -> TransportDelay_IWORK_ps . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ps . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ps . Head == _rtDW -> TransportDelay_IWORK_ps . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ps . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ps . Tail , & _rtDW -> TransportDelay_IWORK_ps . Head ,
& _rtDW -> TransportDelay_IWORK_ps . Last , simTime - _rtP -> P_2725 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ps . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ps . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ps . Head ] = _rtB -> B_390_3742_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cj = _rtB -> B_390_3749_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ab . Head = ( ( _rtDW -> TransportDelay_IWORK_ab . Head
< ( _rtDW -> TransportDelay_IWORK_ab . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ab . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ab . Head == _rtDW -> TransportDelay_IWORK_ab . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ab . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ab . Tail , & _rtDW -> TransportDelay_IWORK_ab . Head ,
& _rtDW -> TransportDelay_IWORK_ab . Last , simTime - _rtP -> P_2730 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ab . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ab . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ab . Head ] = _rtB -> B_390_3750_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_imb = _rtB -> B_390_3757_0 ; } if ( _rtDW ->
TimeStampA_mu == ( rtInf ) ) { _rtDW -> TimeStampA_mu = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_g4 ; } else if ( _rtDW -> TimeStampB_c == ( rtInf )
) { _rtDW -> TimeStampB_c = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e5
; } else if ( _rtDW -> TimeStampA_mu < _rtDW -> TimeStampB_c ) { _rtDW ->
TimeStampA_mu = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_g4 ; } else {
_rtDW -> TimeStampB_c = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e5 ; }
* lastU = _rtB -> B_390_3880_0 ; if ( _rtDW -> TimeStampA_k == ( rtInf ) ) {
_rtDW -> TimeStampA_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_pj ; }
else if ( _rtDW -> TimeStampB_p == ( rtInf ) ) { _rtDW -> TimeStampB_p =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_o ; } else if ( _rtDW ->
TimeStampA_k < _rtDW -> TimeStampB_p ) { _rtDW -> TimeStampA_k = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_pj ; } else { _rtDW -> TimeStampB_p =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_o ; } * lastU = _rtB ->
B_390_3892_0 ; if ( _rtDW -> TimeStampA_p == ( rtInf ) ) { _rtDW ->
TimeStampA_p = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_o ; } else if (
_rtDW -> TimeStampB_ne == ( rtInf ) ) { _rtDW -> TimeStampB_ne = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeB_ou ; } else if ( _rtDW -> TimeStampA_p <
_rtDW -> TimeStampB_ne ) { _rtDW -> TimeStampA_p = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeA_o ; } else { _rtDW -> TimeStampB_ne = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_ou ; } * lastU = _rtB -> B_390_3904_0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_ogq . Head = ( ( _rtDW ->
TransportDelay_IWORK_ogq . Head < ( _rtDW -> TransportDelay_IWORK_ogq .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ogq . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_ogq . Head == _rtDW ->
TransportDelay_IWORK_ogq . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ogq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ogq . Tail , & _rtDW -> TransportDelay_IWORK_ogq . Head
, & _rtDW -> TransportDelay_IWORK_ogq . Last , simTime - _rtP -> P_2878 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ogq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ogq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ogq . Head ] = _rtB -> B_390_3925_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_csp = _rtB -> B_390_3932_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hl . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nn . Head = ( ( _rtDW -> TransportDelay_IWORK_nn . Head
< ( _rtDW -> TransportDelay_IWORK_nn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nn . Head == _rtDW -> TransportDelay_IWORK_nn . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nn . Tail , & _rtDW -> TransportDelay_IWORK_nn . Head ,
& _rtDW -> TransportDelay_IWORK_nn . Last , simTime - _rtP -> P_2883 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nn . Head ] = _rtB -> B_390_3933_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i1 = _rtB -> B_390_3940_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pik . Head = ( ( _rtDW -> TransportDelay_IWORK_pik .
Head < ( _rtDW -> TransportDelay_IWORK_pik . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_pik . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pik . Head == _rtDW -> TransportDelay_IWORK_pik . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pik . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pik . Tail , & _rtDW -> TransportDelay_IWORK_pik . Head
, & _rtDW -> TransportDelay_IWORK_pik . Last , simTime - _rtP -> P_2890 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pik . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pik . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pik . Head ] = _rtB -> B_390_3946_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a0q = _rtB -> B_390_3953_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k1 . Head = ( ( _rtDW -> TransportDelay_IWORK_k1 . Head
< ( _rtDW -> TransportDelay_IWORK_k1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k1 . Head == _rtDW -> TransportDelay_IWORK_k1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k1 . Tail , & _rtDW -> TransportDelay_IWORK_k1 . Head ,
& _rtDW -> TransportDelay_IWORK_k1 . Last , simTime - _rtP -> P_2895 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k1 . Head ] = _rtB -> B_390_3954_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dz = _rtB -> B_390_3961_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f0i . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f0i .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mn . Head = ( ( _rtDW -> TransportDelay_IWORK_mn . Head
< ( _rtDW -> TransportDelay_IWORK_mn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mn . Head == _rtDW -> TransportDelay_IWORK_mn . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mn . Tail , & _rtDW -> TransportDelay_IWORK_mn . Head ,
& _rtDW -> TransportDelay_IWORK_mn . Last , simTime - _rtP -> P_2902 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = _rtB -> B_390_3967_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kn = _rtB -> B_390_3974_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ge . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ge .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pl . Head = ( ( _rtDW -> TransportDelay_IWORK_pl . Head
< ( _rtDW -> TransportDelay_IWORK_pl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pl . Head == _rtDW -> TransportDelay_IWORK_pl . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pl . Tail , & _rtDW -> TransportDelay_IWORK_pl . Head ,
& _rtDW -> TransportDelay_IWORK_pl . Last , simTime - _rtP -> P_2907 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pl . Head ] = _rtB -> B_390_3975_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_iq = _rtB -> B_390_3982_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jj . Head = ( ( _rtDW -> TransportDelay_IWORK_jj . Head
< ( _rtDW -> TransportDelay_IWORK_jj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jj . Head == _rtDW -> TransportDelay_IWORK_jj . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jj . Tail , & _rtDW -> TransportDelay_IWORK_jj . Head ,
& _rtDW -> TransportDelay_IWORK_jj . Last , simTime - _rtP -> P_2918 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jj . Head ] = _rtB -> B_390_3999_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kz = _rtB -> B_390_4006_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dr . Head = ( ( _rtDW -> TransportDelay_IWORK_dr . Head
< ( _rtDW -> TransportDelay_IWORK_dr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dr . Head == _rtDW -> TransportDelay_IWORK_dr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dr . Tail , & _rtDW -> TransportDelay_IWORK_dr . Head ,
& _rtDW -> TransportDelay_IWORK_dr . Last , simTime - _rtP -> P_2923 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dr . Head ] = _rtB -> B_390_4007_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lp = _rtB -> B_390_4014_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ma . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ma .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a3y . Head = ( ( _rtDW -> TransportDelay_IWORK_a3y .
Head < ( _rtDW -> TransportDelay_IWORK_a3y . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_a3y . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a3y . Head == _rtDW -> TransportDelay_IWORK_a3y . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a3y . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a3y . Tail , & _rtDW -> TransportDelay_IWORK_a3y . Head
, & _rtDW -> TransportDelay_IWORK_a3y . Last , simTime - _rtP -> P_2930 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a3y . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3y . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3y . Head ] = _rtB -> B_390_4020_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ae = _rtB -> B_390_4027_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dm . Head = ( ( _rtDW -> TransportDelay_IWORK_dm . Head
< ( _rtDW -> TransportDelay_IWORK_dm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dm . Head == _rtDW -> TransportDelay_IWORK_dm . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dm . Tail , & _rtDW -> TransportDelay_IWORK_dm . Head ,
& _rtDW -> TransportDelay_IWORK_dm . Last , simTime - _rtP -> P_2935 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dm . Head ] = _rtB -> B_390_4028_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nrz = _rtB -> B_390_4035_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pdg . Head = ( ( _rtDW -> TransportDelay_IWORK_pdg .
Head < ( _rtDW -> TransportDelay_IWORK_pdg . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_pdg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pdg . Head == _rtDW -> TransportDelay_IWORK_pdg . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pdg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pdg . Tail , & _rtDW -> TransportDelay_IWORK_pdg . Head
, & _rtDW -> TransportDelay_IWORK_pdg . Last , simTime - _rtP -> P_2942 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pdg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pdg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pdg . Head ] = _rtB -> B_390_4041_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jd4 = _rtB -> B_390_4048_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ny . Head = ( ( _rtDW -> TransportDelay_IWORK_ny . Head
< ( _rtDW -> TransportDelay_IWORK_ny . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ny . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ny . Head == _rtDW -> TransportDelay_IWORK_ny . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ny . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ny . Tail , & _rtDW -> TransportDelay_IWORK_ny . Head ,
& _rtDW -> TransportDelay_IWORK_ny . Last , simTime - _rtP -> P_2947 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ny . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = _rtB -> B_390_4049_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o2n = _rtB -> B_390_4056_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay_DSTATE_c = _rtB ->
B_390_4304_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE_la = _rtB -> B_390_92_0 ; } isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_p
) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_f = 0U ; _rtDW -> Integ4_DSTATE_kt = _rtP -> P_304 *
_rtB -> B_329_11_0 [ 0 ] + _rtB -> B_329_12_0 ; ssCallAccelRunBlock ( S , 329
, 17 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_da = _rtB ->
B_329_11_0 [ 0 ] ; _rtDW -> UnitDelay1_DSTATE_mz = _rtB -> B_329_24_0 ; _rtDW
-> Integ4_SYSTEM_ENABLE_n = 0U ; _rtDW -> Integ4_DSTATE_h = _rtP -> P_320 *
_rtB -> B_329_11_0 [ 1 ] + _rtB -> B_329_25_0 ; ssCallAccelRunBlock ( S , 329
, 30 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_b = _rtB ->
B_329_11_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_o = _rtB -> B_329_37_0 ; } }
_rtDW -> Integ4_SYSTEM_ENABLE_l = 0U ; _rtDW -> Integ4_DSTATE_d = _rtP ->
P_3033 * _rtB -> B_390_4266_0 [ 1 ] + _rtB -> B_390_4267_0 ;
ssCallAccelRunBlock ( S , 390 , 4272 , SS_CALL_MDL_UPDATE ) ; _rtDW ->
UnitDelay_DSTATE_f = _rtB -> B_390_4266_0 [ 1 ] ; _rtDW ->
UnitDelay1_DSTATE_at = _rtB -> B_390_4279_0 ; _rtDW ->
DiscreteDerivative_states_c = _rtDW -> DiscreteDerivative_tmp_p ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_f += _rtP -> P_3052 * _rtB -> B_390_4280_0 ; if
( _rtDW -> DiscreteTimeIntegrator_DSTATE_f >= _rtP -> P_3054 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_f = _rtP -> P_3054 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_f <= _rtP -> P_3055 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_f = _rtP -> P_3055 ; } } _rtDW ->
Delay_x1_DSTATE_h = _rtB -> B_390_4297_0 ; _rtDW -> Delay_x2_DSTATE_c = _rtB
-> B_390_4299_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ml . Head = ( ( _rtDW
-> TransportDelay_IWORK_ml . Head < ( _rtDW -> TransportDelay_IWORK_ml .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ml . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ml . Head == _rtDW ->
TransportDelay_IWORK_ml . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ml . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ml . Tail , & _rtDW -> TransportDelay_IWORK_ml . Head ,
& _rtDW -> TransportDelay_IWORK_ml . Last , simTime - _rtP -> P_3148 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ml . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ml . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ml . Head ] = _rtB -> B_390_4459_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_on = _rtB -> B_390_4466_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ie . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ie .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_av . Head = ( ( _rtDW -> TransportDelay_IWORK_av . Head
< ( _rtDW -> TransportDelay_IWORK_av . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_av . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_av . Head == _rtDW -> TransportDelay_IWORK_av . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_av . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_av . Tail , & _rtDW -> TransportDelay_IWORK_av . Head ,
& _rtDW -> TransportDelay_IWORK_av . Last , simTime - _rtP -> P_3153 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_av . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_av . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_av . Head ] = _rtB -> B_390_4467_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nh = _rtB -> B_390_4474_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ema . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bcn . Head = ( ( _rtDW -> TransportDelay_IWORK_bcn .
Head < ( _rtDW -> TransportDelay_IWORK_bcn . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_bcn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bcn . Head == _rtDW -> TransportDelay_IWORK_bcn . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bcn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bcn . Tail , & _rtDW -> TransportDelay_IWORK_bcn . Head
, & _rtDW -> TransportDelay_IWORK_bcn . Last , simTime - _rtP -> P_3160 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bcn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bcn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bcn . Head ] = _rtB -> B_390_4480_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gw = _rtB -> B_390_4487_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ge . Head = ( ( _rtDW -> TransportDelay_IWORK_ge . Head
< ( _rtDW -> TransportDelay_IWORK_ge . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ge . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ge . Head == _rtDW -> TransportDelay_IWORK_ge . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ge . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ge . Tail , & _rtDW -> TransportDelay_IWORK_ge . Head ,
& _rtDW -> TransportDelay_IWORK_ge . Last , simTime - _rtP -> P_3165 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ge . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ge . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ge . Head ] = _rtB -> B_390_4488_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ln = _rtB -> B_390_4495_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_agi . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_agi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_np . Head = ( ( _rtDW -> TransportDelay_IWORK_np . Head
< ( _rtDW -> TransportDelay_IWORK_np . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_np . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_np . Head == _rtDW -> TransportDelay_IWORK_np . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_np . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_np . Tail , & _rtDW -> TransportDelay_IWORK_np . Head ,
& _rtDW -> TransportDelay_IWORK_np . Last , simTime - _rtP -> P_3172 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_np . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_np . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_np . Head ] = _rtB -> B_390_4501_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cy = _rtB -> B_390_4508_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p2 . Head = ( ( _rtDW -> TransportDelay_IWORK_p2 . Head
< ( _rtDW -> TransportDelay_IWORK_p2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p2 . Head == _rtDW -> TransportDelay_IWORK_p2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p2 . Tail , & _rtDW -> TransportDelay_IWORK_p2 . Head ,
& _rtDW -> TransportDelay_IWORK_p2 . Last , simTime - _rtP -> P_3177 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p2 . Head ] = _rtB -> B_390_4509_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lj = _rtB -> B_390_4516_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ais . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ais .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e0 . Head = ( ( _rtDW -> TransportDelay_IWORK_e0 . Head
< ( _rtDW -> TransportDelay_IWORK_e0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e0 . Head == _rtDW -> TransportDelay_IWORK_e0 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e0 . Tail , & _rtDW -> TransportDelay_IWORK_e0 . Head ,
& _rtDW -> TransportDelay_IWORK_e0 . Last , simTime - _rtP -> P_3188 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e0 . Head ] = _rtB -> B_390_4533_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ko = _rtB -> B_390_4540_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jr . Head = ( ( _rtDW -> TransportDelay_IWORK_jr . Head
< ( _rtDW -> TransportDelay_IWORK_jr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jr . Head == _rtDW -> TransportDelay_IWORK_jr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jr . Tail , & _rtDW -> TransportDelay_IWORK_jr . Head ,
& _rtDW -> TransportDelay_IWORK_jr . Last , simTime - _rtP -> P_3193 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jr . Head ] = _rtB -> B_390_4541_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dc = _rtB -> B_390_4548_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g2 . Head = ( ( _rtDW -> TransportDelay_IWORK_g2 . Head
< ( _rtDW -> TransportDelay_IWORK_g2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g2 . Head == _rtDW -> TransportDelay_IWORK_g2 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g2 . Tail , & _rtDW -> TransportDelay_IWORK_g2 . Head ,
& _rtDW -> TransportDelay_IWORK_g2 . Last , simTime - _rtP -> P_3200 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = _rtB -> B_390_4554_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jl = _rtB -> B_390_4561_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kq . Head = ( ( _rtDW -> TransportDelay_IWORK_kq . Head
< ( _rtDW -> TransportDelay_IWORK_kq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kq . Head == _rtDW -> TransportDelay_IWORK_kq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kq . Tail , & _rtDW -> TransportDelay_IWORK_kq . Head ,
& _rtDW -> TransportDelay_IWORK_kq . Last , simTime - _rtP -> P_3205 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kq . Head ] = _rtB -> B_390_4562_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gk = _rtB -> B_390_4569_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ood . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oh . Head = ( ( _rtDW -> TransportDelay_IWORK_oh . Head
< ( _rtDW -> TransportDelay_IWORK_oh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oh . Head == _rtDW -> TransportDelay_IWORK_oh . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oh . Tail , & _rtDW -> TransportDelay_IWORK_oh . Head ,
& _rtDW -> TransportDelay_IWORK_oh . Last , simTime - _rtP -> P_3212 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oh . Head ] = _rtB -> B_390_4575_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kh = _rtB -> B_390_4582_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eq . Head = ( ( _rtDW -> TransportDelay_IWORK_eq . Head
< ( _rtDW -> TransportDelay_IWORK_eq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eq . Head == _rtDW -> TransportDelay_IWORK_eq . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eq . Tail , & _rtDW -> TransportDelay_IWORK_eq . Head ,
& _rtDW -> TransportDelay_IWORK_eq . Last , simTime - _rtP -> P_3217 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = _rtB -> B_390_4583_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kl = _rtB -> B_390_4590_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay_DSTATE_j = _rtB ->
B_390_4838_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE_j = _rtB -> B_390_116_0 ; } isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE )
{ isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_k = 0U ; _rtDW -> Integ4_DSTATE_j = _rtP -> P_362 * _rtB
-> B_366_11_0 [ 0 ] + _rtB -> B_366_12_0 ; ssCallAccelRunBlock ( S , 366 , 17
, SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_d = _rtB -> B_366_11_0 [ 0
] ; _rtDW -> UnitDelay1_DSTATE_g = _rtB -> B_366_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_b = 0U ; _rtDW -> Integ4_DSTATE_bx = _rtP -> P_378 *
_rtB -> B_366_11_0 [ 1 ] + _rtB -> B_366_25_0 ; ssCallAccelRunBlock ( S , 366
, 30 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_e = _rtB ->
B_366_11_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_d = _rtB -> B_366_37_0 ; } }
_rtDW -> Integ4_SYSTEM_ENABLE_e = 0U ; _rtDW -> Integ4_DSTATE_k = _rtP ->
P_3303 * _rtB -> B_390_4800_0 [ 1 ] + _rtB -> B_390_4801_0 ;
ssCallAccelRunBlock ( S , 390 , 4806 , SS_CALL_MDL_UPDATE ) ; _rtDW ->
UnitDelay_DSTATE_h = _rtB -> B_390_4800_0 [ 1 ] ; _rtDW ->
UnitDelay1_DSTATE_f = _rtB -> B_390_4813_0 ; _rtDW ->
DiscreteDerivative_states_m = _rtDW -> DiscreteDerivative_tmp_m ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_p += _rtP -> P_3322 * _rtB -> B_390_4814_0 ; if
( _rtDW -> DiscreteTimeIntegrator_DSTATE_p >= _rtP -> P_3324 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_p = _rtP -> P_3324 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_p <= _rtP -> P_3325 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_p = _rtP -> P_3325 ; } } _rtDW ->
Delay_x1_DSTATE_c = _rtB -> B_390_4831_0 ; _rtDW -> Delay_x2_DSTATE_ch = _rtB
-> B_390_4833_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_emg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_emg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_fc . Head = ( ( _rtDW
-> TransportDelay_IWORK_fc . Head < ( _rtDW -> TransportDelay_IWORK_fc .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_fc . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_fc . Head == _rtDW ->
TransportDelay_IWORK_fc . Tail ) { if ( !
MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fc . Tail , & _rtDW -> TransportDelay_IWORK_fc . Head ,
& _rtDW -> TransportDelay_IWORK_fc . Last , simTime - _rtP -> P_3414 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fc . Head ] = _rtB -> B_390_4991_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jz = _rtB -> B_390_4998_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_db . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_db .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f0 . Head = ( ( _rtDW -> TransportDelay_IWORK_f0 . Head
< ( _rtDW -> TransportDelay_IWORK_f0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f0 . Head == _rtDW -> TransportDelay_IWORK_f0 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f0 . Tail , & _rtDW -> TransportDelay_IWORK_f0 . Head ,
& _rtDW -> TransportDelay_IWORK_f0 . Last , simTime - _rtP -> P_3419 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f0 . Head ] = _rtB -> B_390_4999_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cv = _rtB -> B_390_5006_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ip . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ip .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lt . Head = ( ( _rtDW -> TransportDelay_IWORK_lt . Head
< ( _rtDW -> TransportDelay_IWORK_lt . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lt . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lt . Head == _rtDW -> TransportDelay_IWORK_lt . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lt . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lt . Tail , & _rtDW -> TransportDelay_IWORK_lt . Head ,
& _rtDW -> TransportDelay_IWORK_lt . Last , simTime - _rtP -> P_3426 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lt . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lt . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lt . Head ] = _rtB -> B_390_5012_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lva = _rtB -> B_390_5019_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_es . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_es .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lr . Head = ( ( _rtDW -> TransportDelay_IWORK_lr . Head
< ( _rtDW -> TransportDelay_IWORK_lr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lr . Head == _rtDW -> TransportDelay_IWORK_lr . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lr . Tail , & _rtDW -> TransportDelay_IWORK_lr . Head ,
& _rtDW -> TransportDelay_IWORK_lr . Last , simTime - _rtP -> P_3431 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lr . Head ] = _rtB -> B_390_5020_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_el = _rtB -> B_390_5027_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_di . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ohq . Head = ( ( _rtDW -> TransportDelay_IWORK_ohq .
Head < ( _rtDW -> TransportDelay_IWORK_ohq . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ohq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ohq . Head == _rtDW -> TransportDelay_IWORK_ohq . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ohq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ohq . Tail , & _rtDW -> TransportDelay_IWORK_ohq . Head
, & _rtDW -> TransportDelay_IWORK_ohq . Last , simTime - _rtP -> P_3438 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ohq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ohq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ohq . Head ] = _rtB -> B_390_5033_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ds = _rtB -> B_390_5040_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_phw . Head = ( ( _rtDW -> TransportDelay_IWORK_phw .
Head < ( _rtDW -> TransportDelay_IWORK_phw . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_phw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_phw . Head == _rtDW -> TransportDelay_IWORK_phw . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_phw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_phw . Tail , & _rtDW -> TransportDelay_IWORK_phw . Head
, & _rtDW -> TransportDelay_IWORK_phw . Last , simTime - _rtP -> P_3443 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_phw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_phw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_phw . Head ] = _rtB -> B_390_5041_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dgf = _rtB -> B_390_5048_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_okh . Head = ( ( _rtDW -> TransportDelay_IWORK_okh .
Head < ( _rtDW -> TransportDelay_IWORK_okh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_okh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_okh . Head == _rtDW -> TransportDelay_IWORK_okh . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_okh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_okh . Tail , & _rtDW -> TransportDelay_IWORK_okh . Head
, & _rtDW -> TransportDelay_IWORK_okh . Last , simTime - _rtP -> P_3453 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_okh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_okh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_okh . Head ] = _rtB -> B_390_5064_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_peu = _rtB -> B_390_5071_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ia . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ia .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ne . Head = ( ( _rtDW -> TransportDelay_IWORK_ne . Head
< ( _rtDW -> TransportDelay_IWORK_ne . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ne . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ne . Head == _rtDW -> TransportDelay_IWORK_ne . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ne . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ne . Tail , & _rtDW -> TransportDelay_IWORK_ne . Head ,
& _rtDW -> TransportDelay_IWORK_ne . Last , simTime - _rtP -> P_3458 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ne . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ne . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ne . Head ] = _rtB -> B_390_5072_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cu = _rtB -> B_390_5079_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dpr . Head = ( ( _rtDW -> TransportDelay_IWORK_dpr .
Head < ( _rtDW -> TransportDelay_IWORK_dpr . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_dpr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dpr . Head == _rtDW -> TransportDelay_IWORK_dpr . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dpr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dpr . Tail , & _rtDW -> TransportDelay_IWORK_dpr . Head
, & _rtDW -> TransportDelay_IWORK_dpr . Last , simTime - _rtP -> P_3465 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dpr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dpr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dpr . Head ] = _rtB -> B_390_5085_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pn = _rtB -> B_390_5092_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_on . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_on .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m1 . Head = ( ( _rtDW -> TransportDelay_IWORK_m1 . Head
< ( _rtDW -> TransportDelay_IWORK_m1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m1 . Head == _rtDW -> TransportDelay_IWORK_m1 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m1 . Tail , & _rtDW -> TransportDelay_IWORK_m1 . Head ,
& _rtDW -> TransportDelay_IWORK_m1 . Last , simTime - _rtP -> P_3470 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m1 . Head ] = _rtB -> B_390_5093_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_du = _rtB -> B_390_5100_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dii . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dii .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hlb . Head = ( ( _rtDW -> TransportDelay_IWORK_hlb .
Head < ( _rtDW -> TransportDelay_IWORK_hlb . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hlb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hlb . Head == _rtDW -> TransportDelay_IWORK_hlb . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hlb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hlb . Tail , & _rtDW -> TransportDelay_IWORK_hlb . Head
, & _rtDW -> TransportDelay_IWORK_hlb . Last , simTime - _rtP -> P_3477 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hlb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hlb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hlb . Head ] = _rtB -> B_390_5106_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_oc = _rtB -> B_390_5113_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ka . Head = ( ( _rtDW -> TransportDelay_IWORK_ka . Head
< ( _rtDW -> TransportDelay_IWORK_ka . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ka . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ka . Head == _rtDW -> TransportDelay_IWORK_ka . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ka . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ka . Tail , & _rtDW -> TransportDelay_IWORK_ka . Head ,
& _rtDW -> TransportDelay_IWORK_ka . Last , simTime - _rtP -> P_3482 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ka . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = _rtB -> B_390_5114_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fr = _rtB -> B_390_5121_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cco . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cco .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lb . Head = ( ( _rtDW -> TransportDelay_IWORK_lb . Head
< ( _rtDW -> TransportDelay_IWORK_lb . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lb . Head == _rtDW -> TransportDelay_IWORK_lb . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lb . Tail , & _rtDW -> TransportDelay_IWORK_lb . Head ,
& _rtDW -> TransportDelay_IWORK_lb . Last , simTime - _rtP -> P_3496 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lb . Head ] = _rtB -> B_390_5145_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l0 = _rtB -> B_390_5152_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l3 . Head = ( ( _rtDW -> TransportDelay_IWORK_l3 . Head
< ( _rtDW -> TransportDelay_IWORK_l3 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l3 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l3 . Head == _rtDW -> TransportDelay_IWORK_l3 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l3 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l3 . Tail , & _rtDW -> TransportDelay_IWORK_l3 . Head ,
& _rtDW -> TransportDelay_IWORK_l3 . Last , simTime - _rtP -> P_3501 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l3 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l3 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l3 . Head ] = _rtB -> B_390_5153_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_he = _rtB -> B_390_5160_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ab . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ab .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gp . Head = ( ( _rtDW -> TransportDelay_IWORK_gp . Head
< ( _rtDW -> TransportDelay_IWORK_gp . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gp . Head == _rtDW -> TransportDelay_IWORK_gp . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gp . Tail , & _rtDW -> TransportDelay_IWORK_gp . Head ,
& _rtDW -> TransportDelay_IWORK_gp . Last , simTime - _rtP -> P_3508 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = _rtB -> B_390_5166_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ph = _rtB -> B_390_5173_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f5 . Head = ( ( _rtDW -> TransportDelay_IWORK_f5 . Head
< ( _rtDW -> TransportDelay_IWORK_f5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f5 . Head == _rtDW -> TransportDelay_IWORK_f5 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f5 . Tail , & _rtDW -> TransportDelay_IWORK_f5 . Head ,
& _rtDW -> TransportDelay_IWORK_f5 . Last , simTime - _rtP -> P_3513 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f5 . Head ] = _rtB -> B_390_5174_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gy = _rtB -> B_390_5181_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ha . Head = ( ( _rtDW -> TransportDelay_IWORK_ha . Head
< ( _rtDW -> TransportDelay_IWORK_ha . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ha . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ha . Head == _rtDW -> TransportDelay_IWORK_ha . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ha . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ha . Tail , & _rtDW -> TransportDelay_IWORK_ha . Head ,
& _rtDW -> TransportDelay_IWORK_ha . Last , simTime - _rtP -> P_3520 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ha . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ha . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ha . Head ] = _rtB -> B_390_5187_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jg = _rtB -> B_390_5194_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ky . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ky .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bm . Head = ( ( _rtDW -> TransportDelay_IWORK_bm . Head
< ( _rtDW -> TransportDelay_IWORK_bm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bm . Head == _rtDW -> TransportDelay_IWORK_bm . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bm . Tail , & _rtDW -> TransportDelay_IWORK_bm . Head ,
& _rtDW -> TransportDelay_IWORK_bm . Last , simTime - _rtP -> P_3525 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = _rtB -> B_390_5195_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ei = _rtB -> B_390_5202_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bro . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bro .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f4 . Head = ( ( _rtDW -> TransportDelay_IWORK_f4 . Head
< ( _rtDW -> TransportDelay_IWORK_f4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f4 . Head == _rtDW -> TransportDelay_IWORK_f4 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f4 . Tail , & _rtDW -> TransportDelay_IWORK_f4 . Head ,
& _rtDW -> TransportDelay_IWORK_f4 . Last , simTime - _rtP -> P_3536 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f4 . Head ] = _rtB -> B_390_5220_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_phq = _rtB -> B_390_5227_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kg . Head = ( ( _rtDW -> TransportDelay_IWORK_kg . Head
< ( _rtDW -> TransportDelay_IWORK_kg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kg . Head == _rtDW -> TransportDelay_IWORK_kg . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kg . Tail , & _rtDW -> TransportDelay_IWORK_kg . Head ,
& _rtDW -> TransportDelay_IWORK_kg . Last , simTime - _rtP -> P_3541 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kg . Head ] = _rtB -> B_390_5228_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lkz = _rtB -> B_390_5235_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dy4 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dy4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_o4 . Head = ( ( _rtDW -> TransportDelay_IWORK_o4 . Head
< ( _rtDW -> TransportDelay_IWORK_o4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o4 . Head == _rtDW -> TransportDelay_IWORK_o4 . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o4 . Tail , & _rtDW -> TransportDelay_IWORK_o4 . Head ,
& _rtDW -> TransportDelay_IWORK_o4 . Last , simTime - _rtP -> P_3548 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_o4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_o4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_o4 . Head ] = _rtB -> B_390_5241_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_po = _rtB -> B_390_5248_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bck . Head = ( ( _rtDW -> TransportDelay_IWORK_bck .
Head < ( _rtDW -> TransportDelay_IWORK_bck . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_bck . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bck . Head == _rtDW -> TransportDelay_IWORK_bck . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bck . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bck . Tail , & _rtDW -> TransportDelay_IWORK_bck . Head
, & _rtDW -> TransportDelay_IWORK_bck . Last , simTime - _rtP -> P_3553 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bck . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bck . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bck . Head ] = _rtB -> B_390_5249_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o1 = _rtB -> B_390_5256_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_prl . Head = ( ( _rtDW -> TransportDelay_IWORK_prl .
Head < ( _rtDW -> TransportDelay_IWORK_prl . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_prl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_prl . Head == _rtDW -> TransportDelay_IWORK_prl . Tail )
{ if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_prl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_prl . Tail , & _rtDW -> TransportDelay_IWORK_prl . Head
, & _rtDW -> TransportDelay_IWORK_prl . Last , simTime - _rtP -> P_3560 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_prl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_prl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_prl . Head ] = _rtB -> B_390_5262_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bt = _rtB -> B_390_5269_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_acd . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_acd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ca . Head = ( ( _rtDW -> TransportDelay_IWORK_ca . Head
< ( _rtDW -> TransportDelay_IWORK_ca . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ca . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ca . Head == _rtDW -> TransportDelay_IWORK_ca . Tail ) {
if ( ! MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ca . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ca . Tail , & _rtDW -> TransportDelay_IWORK_ca . Head ,
& _rtDW -> TransportDelay_IWORK_ca . Last , simTime - _rtP -> P_3565 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ca . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = _rtB -> B_390_5270_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h4 = _rtB -> B_390_5277_0 ; } if ( _rtDW ->
TimeStampA_lh == ( rtInf ) ) { _rtDW -> TimeStampA_lh = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_n ; } else if ( _rtDW -> TimeStampB_ns == ( rtInf )
) { _rtDW -> TimeStampB_ns = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_k
; } else if ( _rtDW -> TimeStampA_lh < _rtDW -> TimeStampB_ns ) { _rtDW ->
TimeStampA_lh = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_n ; } else {
_rtDW -> TimeStampB_ns = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_k ; }
* lastU = _rtB -> B_390_5399_0 ; if ( _rtDW -> TimeStampA_f == ( rtInf ) ) {
_rtDW -> TimeStampA_f = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_i ; }
else if ( _rtDW -> TimeStampB_o == ( rtInf ) ) { _rtDW -> TimeStampB_o =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_ok ; } else if ( _rtDW ->
TimeStampA_f < _rtDW -> TimeStampB_o ) { _rtDW -> TimeStampA_f = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_i ; } else { _rtDW -> TimeStampB_o = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB_ok ; } * lastU = _rtB -> B_390_5411_0
; if ( _rtDW -> TimeStampA_c == ( rtInf ) ) { _rtDW -> TimeStampA_c = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeA_ej ; } else if ( _rtDW ->
TimeStampB_kd == ( rtInf ) ) { _rtDW -> TimeStampB_kd = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeB_j ; } else if ( _rtDW -> TimeStampA_c < _rtDW ->
TimeStampB_kd ) { _rtDW -> TimeStampA_c = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_ej ; } else { _rtDW -> TimeStampB_kd = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_j ; } * lastU = _rtB -> B_390_5423_0 ; if ( _rtDW ->
TimeStampA_d == ( rtInf ) ) { _rtDW -> TimeStampA_d = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_cs ; } else if ( _rtDW -> TimeStampB_p0 == ( rtInf )
) { _rtDW -> TimeStampB_p0 = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_ec ; } else if ( _rtDW -> TimeStampA_d < _rtDW -> TimeStampB_p0
) { _rtDW -> TimeStampA_d = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cs
; } else { _rtDW -> TimeStampB_p0 = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_ec ; } * lastU = _rtB -> B_390_5453_0 ; if ( _rtDW ->
TimeStampA_hs == ( rtInf ) ) { _rtDW -> TimeStampA_hs = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_cq ; } else if ( _rtDW -> TimeStampB_h == ( rtInf )
) { _rtDW -> TimeStampB_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_f
; } else if ( _rtDW -> TimeStampA_hs < _rtDW -> TimeStampB_h ) { _rtDW ->
TimeStampA_hs = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cq ; } else {
_rtDW -> TimeStampB_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_f ; }
* lastU = _rtB -> B_390_5465_0 ; if ( _rtDW -> TimeStampA_if == ( rtInf ) ) {
_rtDW -> TimeStampA_if = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_a ; }
else if ( _rtDW -> TimeStampB_cy == ( rtInf ) ) { _rtDW -> TimeStampB_cy =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_hd ; } else if ( _rtDW ->
TimeStampA_if < _rtDW -> TimeStampB_cy ) { _rtDW -> TimeStampA_if = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA_a ; } else { _rtDW -> TimeStampB_cy =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_hd ; } * lastU = _rtB ->
B_390_5477_0 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID4 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_MR_CIGRE_RESISTIVO_T * _rtB
; P_MR_CIGRE_RESISTIVO_T * _rtP ; X_MR_CIGRE_RESISTIVO_T * _rtX ;
XDot_MR_CIGRE_RESISTIVO_T * _rtXdot ; _rtXdot = ( ( XDot_MR_CIGRE_RESISTIVO_T
* ) ssGetdX ( S ) ) ; _rtX = ( ( X_MR_CIGRE_RESISTIVO_T * ) ssGetContStates (
S ) ) ; _rtP = ( ( P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) ; _rtB =
( ( B_MR_CIGRE_RESISTIVO_T * ) _ssGetModelBlockIO ( S ) ) ; _rtXdot ->
LPF2_CSTATE = 0.0 ; _rtXdot -> LPF2_CSTATE += _rtP -> P_411 * _rtX ->
LPF2_CSTATE ; _rtXdot -> LPF2_CSTATE += _rtB -> B_390_630_0 ; _rtXdot ->
LPF1_CSTATE = 0.0 ; _rtXdot -> LPF1_CSTATE += _rtP -> P_413 * _rtX ->
LPF1_CSTATE ; _rtXdot -> LPF1_CSTATE += _rtB -> B_390_639_0 ; _rtXdot ->
Integrator1_CSTATE = _rtB -> B_390_742_0 ; _rtXdot -> LPF2_CSTATE_e = 0.0 ;
_rtXdot -> LPF2_CSTATE_e += _rtP -> P_439 * _rtX -> LPF2_CSTATE_e ; _rtXdot
-> LPF2_CSTATE_e += _rtB -> B_390_986_0 ; _rtXdot -> LPF1_CSTATE_g = 0.0 ;
_rtXdot -> LPF1_CSTATE_g += _rtP -> P_441 * _rtX -> LPF1_CSTATE_g ; _rtXdot
-> LPF1_CSTATE_g += _rtB -> B_390_995_0 ; _rtXdot -> Integrator1_CSTATE_o =
_rtB -> B_390_1082_0 ; _rtXdot -> LPF1_CSTATE_o = 0.0 ; _rtXdot ->
LPF1_CSTATE_o += _rtP -> P_591 * _rtX -> LPF1_CSTATE_o ; _rtXdot ->
LPF1_CSTATE_o += _rtB -> B_390_807_0 ; _rtXdot -> LPF2_CSTATE_c = 0.0 ;
_rtXdot -> LPF2_CSTATE_c += _rtP -> P_593 * _rtX -> LPF2_CSTATE_c ; _rtXdot
-> LPF2_CSTATE_c += _rtB -> B_390_811_0 ; _rtXdot -> integrator_CSTATE = _rtB
-> B_390_872_0 ; _rtXdot -> integrator_CSTATE_n = _rtB -> B_390_874_0 ;
_rtXdot -> integrator_CSTATE_g = _rtB -> B_390_876_0 ; _rtXdot ->
integrator_CSTATE_e = _rtB -> B_390_878_0 ; _rtXdot -> integrator_CSTATE_l =
_rtB -> B_390_880_0 ; _rtXdot -> integrator_CSTATE_p = _rtB -> B_390_882_0 ;
_rtXdot -> integrator_CSTATE_f = _rtB -> B_390_885_0 ; _rtXdot ->
integrator_CSTATE_k = _rtB -> B_390_887_0 ; _rtXdot -> integrator_CSTATE_lb =
_rtB -> B_390_889_0 ; _rtXdot -> integrator_CSTATE_gs = _rtB -> B_390_891_0 ;
_rtXdot -> integrator_CSTATE_o = _rtB -> B_390_893_0 ; _rtXdot ->
integrator_CSTATE_h = _rtB -> B_390_895_0 ; _rtXdot -> LPF1_CSTATE_e = 0.0 ;
_rtXdot -> LPF1_CSTATE_e += _rtP -> P_693 * _rtX -> LPF1_CSTATE_e ; _rtXdot
-> LPF1_CSTATE_e += _rtB -> B_390_1147_0 ; _rtXdot -> LPF2_CSTATE_m = 0.0 ;
_rtXdot -> LPF2_CSTATE_m += _rtP -> P_695 * _rtX -> LPF2_CSTATE_m ; _rtXdot
-> LPF2_CSTATE_m += _rtB -> B_390_1151_0 ; _rtXdot -> integrator_CSTATE_ok =
_rtB -> B_390_1212_0 ; _rtXdot -> integrator_CSTATE_hr = _rtB -> B_390_1214_0
; _rtXdot -> integrator_CSTATE_e5 = _rtB -> B_390_1216_0 ; _rtXdot ->
integrator_CSTATE_j = _rtB -> B_390_1218_0 ; _rtXdot -> integrator_CSTATE_n1
= _rtB -> B_390_1220_0 ; _rtXdot -> integrator_CSTATE_fv = _rtB ->
B_390_1222_0 ; _rtXdot -> integrator_CSTATE_b = _rtB -> B_390_1225_0 ;
_rtXdot -> integrator_CSTATE_kx = _rtB -> B_390_1227_0 ; _rtXdot ->
integrator_CSTATE_m = _rtB -> B_390_1229_0 ; _rtXdot -> integrator_CSTATE_c =
_rtB -> B_390_1231_0 ; _rtXdot -> integrator_CSTATE_cw = _rtB -> B_390_1233_0
; _rtXdot -> integrator_CSTATE_d = _rtB -> B_390_1235_0 ; _rtXdot ->
Integradord_CSTATE = 0.0 ; _rtXdot -> Integradord_CSTATE += _rtP -> P_797 *
_rtX -> Integradord_CSTATE ; _rtXdot -> Integradord_CSTATE += _rtB ->
B_390_646_0 ; _rtXdot -> LPF4_CSTATE = 0.0 ; _rtXdot -> LPF4_CSTATE += _rtP
-> P_808 * _rtX -> LPF4_CSTATE ; _rtXdot -> LPF4_CSTATE += _rtB ->
B_390_867_0 ; _rtXdot -> LPF2_CSTATE_p = 0.0 ; _rtXdot -> LPF2_CSTATE_p +=
_rtP -> P_814 * _rtX -> LPF2_CSTATE_p ; _rtXdot -> LPF2_CSTATE_p += _rtB ->
B_390_868_0 ; _rtXdot -> Integradord1_CSTATE = 0.0 ; _rtXdot ->
Integradord1_CSTATE += _rtP -> P_817 * _rtX -> Integradord1_CSTATE ; _rtXdot
-> Integradord1_CSTATE += _rtB -> B_390_647_0 ; _rtXdot ->
Integradord_CSTATE_l = 0.0 ; _rtXdot -> Integradord_CSTATE_l += _rtP -> P_837
* _rtX -> Integradord_CSTATE_l ; _rtXdot -> Integradord_CSTATE_l += _rtB ->
B_390_721_0 ; _rtXdot -> HPF1_CSTATE = 0.0 ; _rtXdot -> HPF1_CSTATE += _rtP
-> P_840 * _rtX -> HPF1_CSTATE ; _rtXdot -> HPF1_CSTATE += _rtB ->
B_390_673_0 ; _rtXdot -> HPF2_CSTATE = 0.0 ; _rtXdot -> HPF2_CSTATE += _rtP
-> P_854 * _rtX -> HPF2_CSTATE ; _rtXdot -> HPF2_CSTATE += _rtB ->
B_390_697_0 ; _rtXdot -> Integradorq_CSTATE = 0.0 ; _rtXdot ->
Integradorq_CSTATE += _rtP -> P_859 * _rtX -> Integradorq_CSTATE ; _rtXdot ->
Integradorq_CSTATE += _rtB -> B_390_725_0 ; _rtXdot -> Integradord_CSTATE_a =
0.0 ; _rtXdot -> Integradord_CSTATE_a += _rtP -> P_993 * _rtX ->
Integradord_CSTATE_a ; _rtXdot -> Integradord_CSTATE_a += _rtB ->
B_390_1002_0 ; _rtXdot -> LPF4_CSTATE_f = 0.0 ; _rtXdot -> LPF4_CSTATE_f +=
_rtP -> P_1004 * _rtX -> LPF4_CSTATE_f ; _rtXdot -> LPF4_CSTATE_f += _rtB ->
B_390_1206_0 ; _rtXdot -> LPF2_CSTATE_a = 0.0 ; _rtXdot -> LPF2_CSTATE_a +=
_rtP -> P_1010 * _rtX -> LPF2_CSTATE_a ; _rtXdot -> LPF2_CSTATE_a += _rtB ->
B_390_1208_0 ; _rtXdot -> Integradord1_CSTATE_n = 0.0 ; _rtXdot ->
Integradord1_CSTATE_n += _rtP -> P_1013 * _rtX -> Integradord1_CSTATE_n ;
_rtXdot -> Integradord1_CSTATE_n += _rtB -> B_390_1003_0 ; _rtXdot ->
Integradord_CSTATE_n = 0.0 ; _rtXdot -> Integradord_CSTATE_n += _rtP ->
P_1035 * _rtX -> Integradord_CSTATE_n ; _rtXdot -> Integradord_CSTATE_n +=
_rtB -> B_390_1075_0 ; _rtXdot -> HPF1_CSTATE_k = 0.0 ; _rtXdot ->
HPF1_CSTATE_k += _rtP -> P_1038 * _rtX -> HPF1_CSTATE_k ; _rtXdot ->
HPF1_CSTATE_k += _rtB -> B_390_1033_0 ; _rtXdot -> HPF2_CSTATE_g = 0.0 ;
_rtXdot -> HPF2_CSTATE_g += _rtP -> P_1050 * _rtX -> HPF2_CSTATE_g ; _rtXdot
-> HPF2_CSTATE_g += _rtB -> B_390_1053_0 ; _rtXdot -> Integradorq_CSTATE_a =
0.0 ; _rtXdot -> Integradorq_CSTATE_a += _rtP -> P_1055 * _rtX ->
Integradorq_CSTATE_a ; _rtXdot -> Integradorq_CSTATE_a += _rtB ->
B_390_1079_0 ; _rtXdot -> integrator_CSTATE_ot = _rtB -> B_390_1553_0 ;
_rtXdot -> integrator_CSTATE_a = _rtB -> B_390_1555_0 ; _rtXdot ->
integrator_CSTATE_ob = _rtB -> B_390_1557_0 ; _rtXdot -> integrator_CSTATE_pp
= _rtB -> B_390_1559_0 ; _rtXdot -> integrator_CSTATE_fq = _rtB ->
B_390_1561_0 ; _rtXdot -> integrator_CSTATE_eu = _rtB -> B_390_1563_0 ;
_rtXdot -> integrator_CSTATE_mz = _rtB -> B_390_1565_0 ; _rtXdot ->
integrator_CSTATE_fa = _rtB -> B_390_1567_0 ; _rtXdot -> integrator_CSTATE_bq
= _rtB -> B_390_1569_0 ; _rtXdot -> integrator_CSTATE_fc = _rtB ->
B_390_1571_0 ; _rtXdot -> integrator_CSTATE_nm = _rtB -> B_390_1573_0 ;
_rtXdot -> integrator_CSTATE_c1 = _rtB -> B_390_1575_0 ; _rtXdot ->
integrator_CSTATE_g2 = _rtB -> B_390_1577_0 ; _rtXdot ->
integrator_CSTATE_lbk = _rtB -> B_390_1579_0 ; _rtXdot ->
integrator_CSTATE_gw = _rtB -> B_390_1581_0 ; _rtXdot -> integrator_CSTATE_k1
= _rtB -> B_390_1583_0 ; _rtXdot -> integrator_CSTATE_obo = _rtB ->
B_390_1585_0 ; _rtXdot -> integrator_CSTATE_md = _rtB -> B_390_1587_0 ;
_rtXdot -> integrator_CSTATE_nq = _rtB -> B_390_2077_0 ; _rtXdot ->
integrator_CSTATE_fo = _rtB -> B_390_2079_0 ; _rtXdot -> integrator_CSTATE_p5
= _rtB -> B_390_2081_0 ; _rtXdot -> integrator_CSTATE_ad = _rtB ->
B_390_2083_0 ; _rtXdot -> integrator_CSTATE_cd = _rtB -> B_390_2085_0 ;
_rtXdot -> integrator_CSTATE_ds = _rtB -> B_390_2087_0 ; _rtXdot ->
integrator_CSTATE_ja = _rtB -> B_390_2089_0 ; _rtXdot -> integrator_CSTATE_an
= _rtB -> B_390_2091_0 ; _rtXdot -> integrator_CSTATE_d3 = _rtB ->
B_390_2093_0 ; _rtXdot -> integrator_CSTATE_fl = _rtB -> B_390_2095_0 ;
_rtXdot -> integrator_CSTATE_m3 = _rtB -> B_390_2097_0 ; _rtXdot ->
integrator_CSTATE_oo = _rtB -> B_390_2099_0 ; _rtXdot -> integrator_CSTATE_cf
= _rtB -> B_390_2101_0 ; _rtXdot -> integrator_CSTATE_jq = _rtB ->
B_390_2103_0 ; _rtXdot -> integrator_CSTATE_i = _rtB -> B_390_2105_0 ;
_rtXdot -> integrator_CSTATE_l1 = _rtB -> B_390_2107_0 ; _rtXdot ->
integrator_CSTATE_k1l = _rtB -> B_390_2109_0 ; _rtXdot ->
integrator_CSTATE_hs = _rtB -> B_390_2111_0 ; _rtXdot -> integrator_CSTATE_en
= _rtB -> B_390_2064_0 ; _rtXdot -> integrator_CSTATE_iv = _rtB ->
B_390_2066_0 ; _rtXdot -> integrator_CSTATE_hf = _rtB -> B_390_2068_0 ;
_rtXdot -> integrator_CSTATE_fj = _rtB -> B_390_2070_0 ; _rtXdot ->
integrator_CSTATE_ff = _rtB -> B_390_2072_0 ; _rtXdot -> integrator_CSTATE_er
= _rtB -> B_390_2074_0 ; _rtXdot -> integrator_CSTATE_pb = _rtB ->
B_390_2527_0 ; _rtXdot -> integrator_CSTATE_os = _rtB -> B_390_2529_0 ;
_rtXdot -> integrator_CSTATE_ju = _rtB -> B_390_2531_0 ; _rtXdot ->
integrator_CSTATE_a5 = _rtB -> B_390_2533_0 ; _rtXdot ->
integrator_CSTATE_pbj = _rtB -> B_390_2535_0 ; _rtXdot ->
integrator_CSTATE_mu = _rtB -> B_390_2537_0 ; _rtXdot -> integrator_CSTATE_gd
= _rtB -> B_390_2539_0 ; _rtXdot -> integrator_CSTATE_l5 = _rtB ->
B_390_2541_0 ; _rtXdot -> integrator_CSTATE_hq = _rtB -> B_390_2543_0 ;
_rtXdot -> integrator_CSTATE_cb = _rtB -> B_390_2545_0 ; _rtXdot ->
integrator_CSTATE_po = _rtB -> B_390_2547_0 ; _rtXdot -> integrator_CSTATE_ag
= _rtB -> B_390_2549_0 ; _rtXdot -> integrator_CSTATE_ba = _rtB ->
B_390_2551_0 ; _rtXdot -> integrator_CSTATE_j4 = _rtB -> B_390_2553_0 ;
_rtXdot -> integrator_CSTATE_oi = _rtB -> B_390_2555_0 ; _rtXdot ->
integrator_CSTATE_gb = _rtB -> B_390_2557_0 ; _rtXdot -> integrator_CSTATE_kn
= _rtB -> B_390_2559_0 ; _rtXdot -> integrator_CSTATE_dn = _rtB ->
B_390_2561_0 ; _rtXdot -> integrator_CSTATE_bd = _rtB -> B_390_2514_0 ;
_rtXdot -> integrator_CSTATE_mf = _rtB -> B_390_2516_0 ; _rtXdot ->
integrator_CSTATE_mt = _rtB -> B_390_2518_0 ; _rtXdot ->
integrator_CSTATE_jq4 = _rtB -> B_390_2520_0 ; _rtXdot ->
integrator_CSTATE_ez = _rtB -> B_390_2522_0 ; _rtXdot -> integrator_CSTATE_ct
= _rtB -> B_390_2524_0 ; _rtXdot -> integrator_CSTATE_bm = _rtB ->
B_390_2929_0 ; _rtXdot -> integrator_CSTATE_hk = _rtB -> B_390_2931_0 ;
_rtXdot -> integrator_CSTATE_ia = _rtB -> B_390_2933_0 ; _rtXdot ->
integrator_CSTATE_eq = _rtB -> B_390_2935_0 ; _rtXdot -> integrator_CSTATE_af
= _rtB -> B_390_2937_0 ; _rtXdot -> integrator_CSTATE_kxp = _rtB ->
B_390_2939_0 ; _rtXdot -> integrator_CSTATE_hrr = _rtB -> B_390_2941_0 ;
_rtXdot -> integrator_CSTATE_e4 = _rtB -> B_390_2943_0 ; _rtXdot ->
integrator_CSTATE_otl = _rtB -> B_390_2945_0 ; _rtXdot ->
integrator_CSTATE_pv = _rtB -> B_390_2947_0 ; _rtXdot -> integrator_CSTATE_i5
= _rtB -> B_390_2949_0 ; _rtXdot -> integrator_CSTATE_dsl = _rtB ->
B_390_2951_0 ; _rtXdot -> integrator_CSTATE_if = _rtB -> B_390_2953_0 ;
_rtXdot -> integrator_CSTATE_il = _rtB -> B_390_2955_0 ; _rtXdot ->
integrator_CSTATE_cv = _rtB -> B_390_2957_0 ; _rtXdot -> integrator_CSTATE_cl
= _rtB -> B_390_2959_0 ; _rtXdot -> integrator_CSTATE_ge = _rtB ->
B_390_2961_0 ; _rtXdot -> integrator_CSTATE_ak = _rtB -> B_390_2963_0 ;
_rtXdot -> integrator_CSTATE_mz2 = _rtB -> B_390_2916_0 ; _rtXdot ->
integrator_CSTATE_b5 = _rtB -> B_390_2918_0 ; _rtXdot -> integrator_CSTATE_hc
= _rtB -> B_390_2920_0 ; _rtXdot -> integrator_CSTATE_bn = _rtB ->
B_390_2922_0 ; _rtXdot -> integrator_CSTATE_jqz = _rtB -> B_390_2924_0 ;
_rtXdot -> integrator_CSTATE_mn = _rtB -> B_390_2926_0 ; _rtXdot ->
integrator_CSTATE_np = _rtB -> B_390_3381_0 ; _rtXdot ->
integrator_CSTATE_a5a = _rtB -> B_390_3383_0 ; _rtXdot ->
integrator_CSTATE_jo = _rtB -> B_390_3385_0 ; _rtXdot -> integrator_CSTATE_bj
= _rtB -> B_390_3387_0 ; _rtXdot -> integrator_CSTATE_bu = _rtB ->
B_390_3389_0 ; _rtXdot -> integrator_CSTATE_df = _rtB -> B_390_3391_0 ;
_rtXdot -> integrator_CSTATE_pe = _rtB -> B_390_3393_0 ; _rtXdot ->
integrator_CSTATE_di = _rtB -> B_390_3395_0 ; _rtXdot -> integrator_CSTATE_gi
= _rtB -> B_390_3397_0 ; _rtXdot -> integrator_CSTATE_fd = _rtB ->
B_390_3399_0 ; _rtXdot -> integrator_CSTATE_i2 = _rtB -> B_390_3401_0 ;
_rtXdot -> integrator_CSTATE_kz = _rtB -> B_390_3403_0 ; _rtXdot ->
integrator_CSTATE_nf = _rtB -> B_390_3405_0 ; _rtXdot -> integrator_CSTATE_dl
= _rtB -> B_390_3407_0 ; _rtXdot -> integrator_CSTATE_kf = _rtB ->
B_390_3409_0 ; _rtXdot -> integrator_CSTATE_k4 = _rtB -> B_390_3411_0 ;
_rtXdot -> integrator_CSTATE_h4 = _rtB -> B_390_3413_0 ; _rtXdot ->
integrator_CSTATE_a2 = _rtB -> B_390_3415_0 ; _rtXdot -> integrator_CSTATE_mh
= _rtB -> B_390_3368_0 ; _rtXdot -> integrator_CSTATE_fs = _rtB ->
B_390_3370_0 ; _rtXdot -> integrator_CSTATE_bdm = _rtB -> B_390_3372_0 ;
_rtXdot -> integrator_CSTATE_hn = _rtB -> B_390_3374_0 ; _rtXdot ->
integrator_CSTATE_nm2 = _rtB -> B_390_3376_0 ; _rtXdot ->
integrator_CSTATE_f3 = _rtB -> B_390_3378_0 ; _rtXdot -> integrator_CSTATE_dp
= _rtB -> B_390_3833_0 ; _rtXdot -> integrator_CSTATE_hz = _rtB ->
B_390_3835_0 ; _rtXdot -> integrator_CSTATE_oa = _rtB -> B_390_3837_0 ;
_rtXdot -> integrator_CSTATE_ow = _rtB -> B_390_3839_0 ; _rtXdot ->
integrator_CSTATE_n3 = _rtB -> B_390_3841_0 ; _rtXdot ->
integrator_CSTATE_dn3 = _rtB -> B_390_3843_0 ; _rtXdot ->
integrator_CSTATE_lx = _rtB -> B_390_3845_0 ; _rtXdot -> integrator_CSTATE_am
= _rtB -> B_390_3847_0 ; _rtXdot -> integrator_CSTATE_gew = _rtB ->
B_390_3849_0 ; _rtXdot -> integrator_CSTATE_br = _rtB -> B_390_3851_0 ;
_rtXdot -> integrator_CSTATE_bl = _rtB -> B_390_3853_0 ; _rtXdot ->
integrator_CSTATE_oe = _rtB -> B_390_3855_0 ; _rtXdot -> integrator_CSTATE_k3
= _rtB -> B_390_3857_0 ; _rtXdot -> integrator_CSTATE_ay = _rtB ->
B_390_3859_0 ; _rtXdot -> integrator_CSTATE_hx = _rtB -> B_390_3861_0 ;
_rtXdot -> integrator_CSTATE_nc = _rtB -> B_390_3863_0 ; _rtXdot ->
integrator_CSTATE_jf = _rtB -> B_390_3865_0 ; _rtXdot -> integrator_CSTATE_mc
= _rtB -> B_390_3867_0 ; _rtXdot -> integrator_CSTATE_av = _rtB ->
B_390_3820_0 ; _rtXdot -> integrator_CSTATE_gy = _rtB -> B_390_3822_0 ;
_rtXdot -> integrator_CSTATE_i3 = _rtB -> B_390_3824_0 ; _rtXdot ->
integrator_CSTATE_kg = _rtB -> B_390_3826_0 ; _rtXdot ->
integrator_CSTATE_fqj = _rtB -> B_390_3828_0 ; _rtXdot ->
integrator_CSTATE_gii = _rtB -> B_390_3830_0 ; _rtXdot -> LPF1_CSTATE_j = 0.0
; _rtXdot -> LPF1_CSTATE_j += _rtP -> P_2873 * _rtX -> LPF1_CSTATE_j ;
_rtXdot -> LPF1_CSTATE_j += _rtB -> B_390_4231_0 ; _rtXdot -> LPF2_CSTATE_m5
= 0.0 ; _rtXdot -> LPF2_CSTATE_m5 += _rtP -> P_2875 * _rtX -> LPF2_CSTATE_m5
; _rtXdot -> LPF2_CSTATE_m5 += _rtB -> B_390_4235_0 ; _rtXdot ->
integrator_CSTATE_fn = _rtB -> B_390_4306_0 ; _rtXdot -> integrator_CSTATE_ox
= _rtB -> B_390_4308_0 ; _rtXdot -> integrator_CSTATE_clt = _rtB ->
B_390_4310_0 ; _rtXdot -> integrator_CSTATE_bo = _rtB -> B_390_4312_0 ;
_rtXdot -> integrator_CSTATE_jm = _rtB -> B_390_4314_0 ; _rtXdot ->
integrator_CSTATE_gv = _rtB -> B_390_4316_0 ; _rtXdot -> integrator_CSTATE_cr
= _rtB -> B_390_4319_0 ; _rtXdot -> integrator_CSTATE_gm = _rtB ->
B_390_4321_0 ; _rtXdot -> integrator_CSTATE_l3 = _rtB -> B_390_4323_0 ;
_rtXdot -> integrator_CSTATE_hm = _rtB -> B_390_4325_0 ; _rtXdot ->
integrator_CSTATE_co = _rtB -> B_390_4327_0 ; _rtXdot ->
integrator_CSTATE_knp = _rtB -> B_390_4329_0 ; _rtXdot ->
Integradord_CSTATE_g = 0.0 ; _rtXdot -> Integradord_CSTATE_g += _rtP ->
P_2969 * _rtX -> Integradord_CSTATE_g ; _rtXdot -> Integradord_CSTATE_g +=
_rtB -> B_390_4148_0 ; _rtXdot -> Integradord1_CSTATE_m = 0.0 ; _rtXdot ->
Integradord1_CSTATE_m += _rtP -> P_2977 * _rtX -> Integradord1_CSTATE_m ;
_rtXdot -> Integradord1_CSTATE_m += _rtB -> B_390_4152_0 ; _rtXdot ->
Controled_CSTATE = 0.0 ; _rtXdot -> Controled_CSTATE += _rtP -> P_2981 * _rtX
-> Controled_CSTATE ; _rtXdot -> Controled_CSTATE += _rtB -> B_390_4112_0 ;
_rtXdot -> Controleq_CSTATE = 0.0 ; _rtXdot -> Controleq_CSTATE += _rtP ->
P_2984 * _rtX -> Controleq_CSTATE ; _rtXdot -> Controleq_CSTATE += _rtB ->
B_390_4114_0 ; _rtXdot -> LPF1_CSTATE_a = 0.0 ; _rtXdot -> LPF1_CSTATE_a +=
_rtP -> P_3143 * _rtX -> LPF1_CSTATE_a ; _rtXdot -> LPF1_CSTATE_a += _rtB ->
B_390_4765_0 ; _rtXdot -> LPF2_CSTATE_d = 0.0 ; _rtXdot -> LPF2_CSTATE_d +=
_rtP -> P_3145 * _rtX -> LPF2_CSTATE_d ; _rtXdot -> LPF2_CSTATE_d += _rtB ->
B_390_4769_0 ; _rtXdot -> integrator_CSTATE_ib = _rtB -> B_390_4840_0 ;
_rtXdot -> integrator_CSTATE_j3 = _rtB -> B_390_4842_0 ; _rtXdot ->
integrator_CSTATE_bx = _rtB -> B_390_4844_0 ; _rtXdot -> integrator_CSTATE_h0
= _rtB -> B_390_4846_0 ; _rtXdot -> integrator_CSTATE_pa = _rtB ->
B_390_4848_0 ; _rtXdot -> integrator_CSTATE_ew = _rtB -> B_390_4850_0 ;
_rtXdot -> integrator_CSTATE_hh = _rtB -> B_390_4853_0 ; _rtXdot ->
integrator_CSTATE_fcw = _rtB -> B_390_4855_0 ; _rtXdot ->
integrator_CSTATE_kg1 = _rtB -> B_390_4857_0 ; _rtXdot ->
integrator_CSTATE_it = _rtB -> B_390_4859_0 ; _rtXdot -> integrator_CSTATE_fg
= _rtB -> B_390_4861_0 ; _rtXdot -> integrator_CSTATE_jb = _rtB ->
B_390_4863_0 ; _rtXdot -> Integradord_CSTATE_an = 0.0 ; _rtXdot ->
Integradord_CSTATE_an += _rtP -> P_3239 * _rtX -> Integradord_CSTATE_an ;
_rtXdot -> Integradord_CSTATE_an += _rtB -> B_390_4682_0 ; _rtXdot ->
Integradord1_CSTATE_c = 0.0 ; _rtXdot -> Integradord1_CSTATE_c += _rtP ->
P_3247 * _rtX -> Integradord1_CSTATE_c ; _rtXdot -> Integradord1_CSTATE_c +=
_rtB -> B_390_4686_0 ; _rtXdot -> Controled_CSTATE_o = 0.0 ; _rtXdot ->
Controled_CSTATE_o += _rtP -> P_3251 * _rtX -> Controled_CSTATE_o ; _rtXdot
-> Controled_CSTATE_o += _rtB -> B_390_4646_0 ; _rtXdot -> Controleq_CSTATE_j
= 0.0 ; _rtXdot -> Controleq_CSTATE_j += _rtP -> P_3254 * _rtX ->
Controleq_CSTATE_j ; _rtXdot -> Controleq_CSTATE_j += _rtB -> B_390_4648_0 ;
_rtXdot -> integrator_CSTATE_el = _rtB -> B_390_5352_0 ; _rtXdot ->
integrator_CSTATE_ax = _rtB -> B_390_5354_0 ; _rtXdot -> integrator_CSTATE_pm
= _rtB -> B_390_5356_0 ; _rtXdot -> integrator_CSTATE_dy = _rtB ->
B_390_5358_0 ; _rtXdot -> integrator_CSTATE_kk = _rtB -> B_390_5360_0 ;
_rtXdot -> integrator_CSTATE_pg = _rtB -> B_390_5362_0 ; _rtXdot ->
integrator_CSTATE_hrp = _rtB -> B_390_5364_0 ; _rtXdot ->
integrator_CSTATE_bg = _rtB -> B_390_5366_0 ; _rtXdot -> integrator_CSTATE_dh
= _rtB -> B_390_5368_0 ; _rtXdot -> integrator_CSTATE_fof = _rtB ->
B_390_5370_0 ; _rtXdot -> integrator_CSTATE_l5v = _rtB -> B_390_5372_0 ;
_rtXdot -> integrator_CSTATE_jw = _rtB -> B_390_5374_0 ; _rtXdot ->
integrator_CSTATE_cfo = _rtB -> B_390_5376_0 ; _rtXdot ->
integrator_CSTATE_hqe = _rtB -> B_390_5378_0 ; _rtXdot ->
integrator_CSTATE_nt = _rtB -> B_390_5380_0 ; _rtXdot ->
integrator_CSTATE_hqx = _rtB -> B_390_5382_0 ; _rtXdot ->
integrator_CSTATE_gq = _rtB -> B_390_5384_0 ; _rtXdot ->
integrator_CSTATE_cvp = _rtB -> B_390_5386_0 ; _rtXdot ->
integrator_CSTATE_cs = _rtB -> B_390_5339_0 ; _rtXdot -> integrator_CSTATE_oh
= _rtB -> B_390_5341_0 ; _rtXdot -> integrator_CSTATE_mi = _rtB ->
B_390_5343_0 ; _rtXdot -> integrator_CSTATE_m4 = _rtB -> B_390_5345_0 ;
_rtXdot -> integrator_CSTATE_g2y = _rtB -> B_390_5347_0 ; _rtXdot ->
integrator_CSTATE_kf0 = _rtB -> B_390_5349_0 ; }
#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings ( SimStruct * S ) { B_MR_CIGRE_RESISTIVO_T *
_rtB ; P_MR_CIGRE_RESISTIVO_T * _rtP ; ZCV_MR_CIGRE_RESISTIVO_T * _rtZCSV ;
_rtZCSV = ( ( ZCV_MR_CIGRE_RESISTIVO_T * ) ssGetSolverZcSignalVector ( S ) )
; _rtP = ( ( P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_RESISTIVO_T * ) _ssGetModelBlockIO ( S ) ) ; _rtZCSV ->
Saturation_UprLim_ZC = _rtB -> B_390_4_0 - _rtP -> P_415 ; _rtZCSV ->
Saturation_LwrLim_ZC = _rtB -> B_390_4_0 - _rtP -> P_416 ; _rtZCSV ->
PQparaVf_StepTime_ZC = ssGetT ( S ) - _rtP -> P_422 ; _rtZCSV ->
Saturation_UprLim_ZC_o = _rtB -> B_390_42_0 - _rtP -> P_443 ; _rtZCSV ->
Saturation_LwrLim_ZC_n = _rtB -> B_390_42_0 - _rtP -> P_444 ; _rtZCSV ->
ref_StepTime_ZC = ssGetT ( S ) - _rtP -> P_579 ; _rtZCSV ->
Time_init_ref_StepTime_ZC = ssGetT ( S ) - _rtP -> P_583 ; _rtZCSV ->
ref_StepTime_ZC_m = ssGetT ( S ) - _rtP -> P_681 ; _rtZCSV ->
Time_init_ref_StepTime_ZC_p = ssGetT ( S ) - _rtP -> P_685 ; _rtZCSV ->
ref2_StepTime_ZC = ssGetT ( S ) - _rtP -> P_823 ; _rtZCSV ->
Saturation_UprLim_ZC_oi = _rtB -> B_390_712_0 - _rtP -> P_862 ; _rtZCSV ->
Saturation_LwrLim_ZC_o = _rtB -> B_390_712_0 - _rtP -> P_863 ; _rtZCSV ->
ref2_StepTime_ZC_k = ssGetT ( S ) - _rtP -> P_1019 ; _rtZCSV ->
Saturation_UprLim_ZC_d = _rtB -> B_390_1068_0 - _rtP -> P_1058 ; _rtZCSV ->
Saturation_LwrLim_ZC_c = _rtB -> B_390_1068_0 - _rtP -> P_1059 ; _rtZCSV ->
IlhamentoChavefechadaparachaveabertaemtilha_StepTime_ZC = ssGetT ( S ) - _rtP
-> P_1381 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC = _rtB -> B_390_1604_0 -
_rtP -> P_1395 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_b = _rtB ->
B_390_1616_0 - _rtP -> P_1402 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_d =
_rtB -> B_390_1628_0 - _rtP -> P_1409 ; _rtZCSV -> Step_StepTime_ZC = ssGetT
( S ) - _rtP -> P_1410 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_n = _rtB ->
B_390_2126_0 - _rtP -> P_1705 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_i =
_rtB -> B_390_2138_0 - _rtP -> P_1712 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_dd = _rtB -> B_390_2150_0 - _rtP -> P_1719 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_f = _rtB -> B_390_2978_0 - _rtP ->
P_2268 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_c = _rtB -> B_390_2990_0 -
_rtP -> P_2275 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nb = _rtB ->
B_390_3002_0 - _rtP -> P_2282 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_g =
_rtB -> B_390_3430_0 - _rtP -> P_2563 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_l = _rtB -> B_390_3442_0 - _rtP -> P_2570 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_h = _rtB -> B_390_3454_0 - _rtP ->
P_2577 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nl = _rtB -> B_390_3882_0 -
_rtP -> P_2858 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nt = _rtB ->
B_390_3894_0 - _rtP -> P_2865 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_e =
_rtB -> B_390_3906_0 - _rtP -> P_2872 ; _rtZCSV -> Saturation_UprLim_ZC_m =
_rtB -> B_390_4108_0 - _rtP -> P_2979 ; _rtZCSV -> Saturation_LwrLim_ZC_j =
_rtB -> B_390_4108_0 - _rtP -> P_2980 ; _rtZCSV -> Saturation_UprLim_ZC_k =
_rtB -> B_390_4142_0 - _rtP -> P_3000 ; _rtZCSV -> Saturation_LwrLim_ZC_i =
_rtB -> B_390_4142_0 - _rtP -> P_3001 ; _rtZCSV -> Saturation1_UprLim_ZC =
_rtB -> B_390_4138_0 - _rtP -> P_3002 ; _rtZCSV -> Saturation1_LwrLim_ZC =
_rtB -> B_390_4138_0 - _rtP -> P_3003 ; _rtZCSV -> Saturation_UprLim_ZC_kc =
_rtB -> B_390_4642_0 - _rtP -> P_3249 ; _rtZCSV -> Saturation_LwrLim_ZC_ol =
_rtB -> B_390_4642_0 - _rtP -> P_3250 ; _rtZCSV -> Saturation_UprLim_ZC_ky =
_rtB -> B_390_4676_0 - _rtP -> P_3270 ; _rtZCSV -> Saturation_LwrLim_ZC_nr =
_rtB -> B_390_4676_0 - _rtP -> P_3271 ; _rtZCSV -> Saturation1_UprLim_ZC_d =
_rtB -> B_390_4672_0 - _rtP -> P_3272 ; _rtZCSV -> Saturation1_LwrLim_ZC_a =
_rtB -> B_390_4672_0 - _rtP -> P_3273 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_fz = _rtB -> B_390_5401_0 - _rtP -> P_3693 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_gc = _rtB -> B_390_5413_0 - _rtP ->
P_3700 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_fh = _rtB -> B_390_5425_0 -
_rtP -> P_3707 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_ev = _rtB ->
B_390_5455_0 - _rtP -> P_3720 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_in =
_rtB -> B_390_5467_0 - _rtP -> P_3727 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_a = _rtB -> B_390_5479_0 - _rtP -> P_3734 ; }
static void mdlInitializeSizes ( SimStruct * S ) { ssSetChecksumVal ( S , 0 ,
212638319U ) ; ssSetChecksumVal ( S , 1 , 3550987684U ) ; ssSetChecksumVal (
S , 2 , 3722214528U ) ; ssSetChecksumVal ( S , 3 , 4198046661U ) ; { mxArray
* slVerStructMat = NULL ; mxArray * slStrMat = mxCreateString ( "simulink" )
; char slVerChar [ 10 ] ; int status = mexCallMATLAB ( 1 , & slVerStructMat ,
1 , & slStrMat , "ver" ) ; if ( status == 0 ) { mxArray * slVerMat =
mxGetField ( slVerStructMat , 0 , "Version" ) ; if ( slVerMat == NULL ) {
status = 1 ; } else { status = mxGetString ( slVerMat , slVerChar , 10 ) ; }
} mxDestroyArray ( slStrMat ) ; mxDestroyArray ( slVerStructMat ) ; if ( (
status == 1 ) || ( strcmp ( slVerChar , "9.0" ) != 0 ) ) { return ; } }
ssSetOptions ( S , SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork (
S ) != sizeof ( DW_MR_CIGRE_RESISTIVO_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_MR_CIGRE_RESISTIVO_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_MR_CIGRE_RESISTIVO_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & MR_CIGRE_RESISTIVO_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T ) )
; ( ( P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_58 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_114 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_336 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_394 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_907 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_910 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_1101 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_1104 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3054 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3057 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3136 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3140 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3324 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3327 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3406 = rtInf ; ( (
P_MR_CIGRE_RESISTIVO_T * ) ssGetModelRtp ( S ) ) -> P_3410 = rtInf ; } static
void mdlInitializeSampleTimes ( SimStruct * S ) { { SimStruct * childS ;
SysOutputFcn * callSysFcns ; childS = ssGetSFunction ( S , 3 ) ; callSysFcns
= ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 6 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 7 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 10 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; } slAccRegPrmChangeFcn ( S , mdlOutputsTID4 ) ; }
static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
