#include "__cf_MR_CIGRE_GENERALIZADO.h"
#include <math.h>
#include "MR_CIGRE_GENERALIZADO_acc.h"
#include "MR_CIGRE_GENERALIZADO_acc_private.h"
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
boolean_T MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T *
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
return ( true ) ; } real_T MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
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
MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Init (
SimStruct * S ) { B_MR_CIGRE_GENERALIZADO_T * _rtB ;
P_MR_CIGRE_GENERALIZADO_T * _rtP ; DW_MR_CIGRE_GENERALIZADO_T * _rtDW ; _rtDW
= ( ( DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ;
ssCallAccelRunBlock ( S , 0 , 14 , SS_CALL_MDL_INITIALIZE_CONDITIONS ) ; }
void MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1 (
SimStruct * S ) { DW_MR_CIGRE_GENERALIZADO_T * _rtDW ; _rtDW = ( (
DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ; ssCallAccelRunBlock (
S , 392 , _rtDW -> Synthesized_Atomic_Subsystem_For_Alg_Loop_1_blkIdxToRun ,
SS_CALL_MDL_OUTPUTS ) ; } void
MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Update (
SimStruct * S ) { B_MR_CIGRE_GENERALIZADO_T * _rtB ;
P_MR_CIGRE_GENERALIZADO_T * _rtP ; DW_MR_CIGRE_GENERALIZADO_T * _rtDW ; _rtDW
= ( ( DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ;
ssCallAccelRunBlock ( S , 0 , 14 , SS_CALL_MDL_UPDATE ) ; } void
MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Term (
SimStruct * const S ) { } void MR_CIGRE_GENERALIZADO_Subsystempi2delay_Init (
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_T * localB ,
P_Subsystempi2delay_MR_CIGRE_GENERALIZADO_T * localP ) { localB -> B_2_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_2_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_Disable (
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_T * localDW ) { localDW ->
Subsystempi2delay_MODE = false ; } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( SimStruct * S , uint8_T rtu_Enable
, const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_T * localDW ) { int32_T isHit ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW ->
Subsystempi2delay_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystempi2delay_MODE = true ; } } else { if ( localDW ->
Subsystempi2delay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_Disable ( localDW ) ; } } } if (
localDW -> Subsystempi2delay_MODE ) { localB -> B_2_0_0 = rtu_alpha_beta [ 0
] * muDoubleScalarSin ( rtu_wt ) - rtu_alpha_beta [ 1 ] * muDoubleScalarCos (
rtu_wt ) ; localB -> B_2_1_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarCos (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_Term ( SimStruct * const S ) { } void
MR_CIGRE_GENERALIZADO_Subsystem1_Init ( B_Subsystem1_MR_CIGRE_GENERALIZADO_T
* localB , P_Subsystem1_MR_CIGRE_GENERALIZADO_T * localP ) { localB ->
B_3_0_0 = localP -> P_0 [ 0 ] ; localB -> B_3_1_0 = localP -> P_0 [ 1 ] ; }
void MR_CIGRE_GENERALIZADO_Subsystem1_Disable (
DW_Subsystem1_MR_CIGRE_GENERALIZADO_T * localDW ) { localDW ->
Subsystem1_MODE = false ; } void MR_CIGRE_GENERALIZADO_Subsystem1 ( SimStruct
* S , uint8_T rtu_Enable , const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt
, B_Subsystem1_MR_CIGRE_GENERALIZADO_T * localB ,
DW_Subsystem1_MR_CIGRE_GENERALIZADO_T * localDW ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW -> Subsystem1_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystem1_MODE = true ; } } else { if ( localDW -> Subsystem1_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_GENERALIZADO_Subsystem1_Disable ( localDW ) ; } } } if ( localDW ->
Subsystem1_MODE ) { localB -> B_3_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarCos ( rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; localB -> B_3_1_0 = - rtu_alpha_beta [ 0 ] * muDoubleScalarSin (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void MR_CIGRE_GENERALIZADO_Subsystem1_Term (
SimStruct * const S ) { } void MR_CIGRE_GENERALIZADO_Subsystempi2delay_f_Init
( B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_j_T * localB ,
P_Subsystempi2delay_MR_CIGRE_GENERALIZADO_h_T * localP ) { localB -> B_10_0_0
= localP -> P_0 [ 0 ] ; localB -> B_10_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_j_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_k_T * localDW ) { if ( rtu_Enable
> 0 ) { localB -> B_10_0_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarSin (
rtu_wt ) - rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; localB ->
B_10_1_0 = rtu_alpha_beta [ 0 ] * muDoubleScalarCos ( rtu_wt ) +
rtu_alpha_beta [ 1 ] * muDoubleScalarSin ( rtu_wt ) ; srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_i_Term ( SimStruct * const S ) { }
void MR_CIGRE_GENERALIZADO_Subsystem1_i_Init (
B_Subsystem1_MR_CIGRE_GENERALIZADO_n_T * localB ,
P_Subsystem1_MR_CIGRE_GENERALIZADO_l_T * localP ) { localB -> B_11_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_11_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystem1_b ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt , B_Subsystem1_MR_CIGRE_GENERALIZADO_n_T
* localB , DW_Subsystem1_MR_CIGRE_GENERALIZADO_i_T * localDW ) { if (
rtu_Enable > 0 ) { localB -> B_11_0_0 = rtu_alpha_beta [ 0 ] *
muDoubleScalarCos ( rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; localB -> B_11_1_0 = - rtu_alpha_beta [ 0 ] * muDoubleScalarSin (
rtu_wt ) + rtu_alpha_beta [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; srUpdateBC (
localDW -> Subsystem1_SubsysRanBC ) ; } } void
MR_CIGRE_GENERALIZADO_Subsystem1_j_Term ( SimStruct * const S ) { } void
MR_CIGRE_GENERALIZADO_NegSeqComputation_Disable (
DW_NegSeqComputation_MR_CIGRE_GENERALIZADO_T * localDW ) { localDW ->
NegSeqComputation_MODE = false ; } void
MR_CIGRE_GENERALIZADO_NegSeqComputation ( SimStruct * S , real_T rtu_Enable ,
creal_T rtu_In , creal_T rtu_In_p , creal_T rtu_In_d ,
B_NegSeqComputation_MR_CIGRE_GENERALIZADO_T * localB ,
DW_NegSeqComputation_MR_CIGRE_GENERALIZADO_T * localDW ,
P_NegSeqComputation_MR_CIGRE_GENERALIZADO_T * localP ) { int32_T isHit ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> NegSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S
) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
NegSeqComputation_MODE = true ; } } else { if ( localDW ->
NegSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_GENERALIZADO_NegSeqComputation_Disable ( localDW ) ; } } } if (
localDW -> NegSeqComputation_MODE ) { localB -> B_23_0_0 [ 0 ] . re = localP
-> P_1 [ 0 ] . re * rtu_In . re - localP -> P_1 [ 0 ] . im * rtu_In . im ;
localB -> B_23_0_0 [ 0 ] . im = localP -> P_1 [ 0 ] . re * rtu_In . im +
localP -> P_1 [ 0 ] . im * rtu_In . re ; localB -> B_23_0_0 [ 1 ] . re =
localP -> P_1 [ 1 ] . re * rtu_In_p . re - localP -> P_1 [ 1 ] . im *
rtu_In_p . im ; localB -> B_23_0_0 [ 1 ] . im = localP -> P_1 [ 1 ] . re *
rtu_In_p . im + localP -> P_1 [ 1 ] . im * rtu_In_p . re ; localB -> B_23_0_0
[ 2 ] . re = localP -> P_1 [ 2 ] . re * rtu_In_d . re - localP -> P_1 [ 2 ] .
im * rtu_In_d . im ; localB -> B_23_0_0 [ 2 ] . im = localP -> P_1 [ 2 ] . re
* rtu_In_d . im + localP -> P_1 [ 2 ] . im * rtu_In_d . re ; localB ->
B_23_1_0 . re = ( localB -> B_23_0_0 [ 0 ] . re + localB -> B_23_0_0 [ 1 ] .
re ) + localB -> B_23_0_0 [ 2 ] . re ; localB -> B_23_1_0 . im = ( localB ->
B_23_0_0 [ 0 ] . im + localB -> B_23_0_0 [ 1 ] . im ) + localB -> B_23_0_0 [
2 ] . im ; localB -> B_23_2_0 . re = localP -> P_0 * localB -> B_23_1_0 . re
; localB -> B_23_2_0 . im = localP -> P_0 * localB -> B_23_1_0 . im ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
NegSeqComputation_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_NegSeqComputation_Term ( SimStruct * const S ) { } void
MR_CIGRE_GENERALIZADO_ZeroSeqComputation_Disable (
DW_ZeroSeqComputation_MR_CIGRE_GENERALIZADO_T * localDW ) { localDW ->
ZeroSeqComputation_MODE = false ; } void
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( SimStruct * S , real_T rtu_Enable
, creal_T rtu_In , creal_T rtu_In_l , creal_T rtu_In_n ,
B_ZeroSeqComputation_MR_CIGRE_GENERALIZADO_T * localB ,
DW_ZeroSeqComputation_MR_CIGRE_GENERALIZADO_T * localDW ,
P_ZeroSeqComputation_MR_CIGRE_GENERALIZADO_T * localP ) { int32_T isHit ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0.0 ) { if ( ! localDW
-> ZeroSeqComputation_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart (
S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
ZeroSeqComputation_MODE = true ; } } else { if ( localDW ->
ZeroSeqComputation_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S )
; MR_CIGRE_GENERALIZADO_ZeroSeqComputation_Disable ( localDW ) ; } } } if (
localDW -> ZeroSeqComputation_MODE ) { localB -> B_25_0_0 . re = ( rtu_In .
re + rtu_In_l . re ) + rtu_In_n . re ; localB -> B_25_0_0 . im = ( rtu_In .
im + rtu_In_l . im ) + rtu_In_n . im ; localB -> B_25_1_0 . re = localP ->
P_0 * localB -> B_25_0_0 . re ; localB -> B_25_1_0 . im = localP -> P_0 *
localB -> B_25_0_0 . im ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC (
localDW -> ZeroSeqComputation_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_ZeroSeqComputation_Term ( SimStruct * const S ) { }
void MR_CIGRE_GENERALIZADO_Subsystempi2delay_fd_Init (
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_f_T * localB ,
P_Subsystempi2delay_MR_CIGRE_GENERALIZADO_p_T * localP ) { localB -> B_35_0_0
= localP -> P_0 [ 0 ] ; localB -> B_35_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m_Disable (
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_o_T * localDW ) { localDW ->
Subsystempi2delay_MODE = false ; } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_o ( SimStruct * S , uint8_T
rtu_Enable , real_T rtu_dq , real_T rtu_dq_j , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_f_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_o_T * localDW ) { int32_T isHit ;
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW ->
Subsystempi2delay_MODE ) { if ( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S )
) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystempi2delay_MODE = true ; } } else { if ( localDW ->
Subsystempi2delay_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m_Disable ( localDW ) ; } } } if (
localDW -> Subsystempi2delay_MODE ) { localB -> B_35_0_0 = rtu_dq *
muDoubleScalarSin ( rtu_wt ) + rtu_dq_j * muDoubleScalarCos ( rtu_wt ) ;
localB -> B_35_1_0 = - rtu_dq * muDoubleScalarCos ( rtu_wt ) + rtu_dq_j *
muDoubleScalarSin ( rtu_wt ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
srUpdateBC ( localDW -> Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_iz_Term ( SimStruct * const S ) { }
void MR_CIGRE_GENERALIZADO_Subsystem1_k_Init (
B_Subsystem1_MR_CIGRE_GENERALIZADO_nf_T * localB ,
P_Subsystem1_MR_CIGRE_GENERALIZADO_n_T * localP ) { localB -> B_36_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_36_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystem1_j_Disable (
DW_Subsystem1_MR_CIGRE_GENERALIZADO_g_T * localDW ) { localDW ->
Subsystem1_MODE = false ; } void MR_CIGRE_GENERALIZADO_Subsystem1_e (
SimStruct * S , uint8_T rtu_Enable , real_T rtu_dq , real_T rtu_dq_m , real_T
rtu_wt , B_Subsystem1_MR_CIGRE_GENERALIZADO_nf_T * localB ,
DW_Subsystem1_MR_CIGRE_GENERALIZADO_g_T * localDW ) { int32_T isHit ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( rtu_Enable > 0 ) { if ( ! localDW -> Subsystem1_MODE ) { if
( ssGetTaskTime ( S , 1 ) != ssGetTStart ( S ) ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } localDW ->
Subsystem1_MODE = true ; } } else { if ( localDW -> Subsystem1_MODE ) {
ssSetBlockStateForSolverChangedAtMajorStep ( S ) ;
MR_CIGRE_GENERALIZADO_Subsystem1_j_Disable ( localDW ) ; } } } if ( localDW
-> Subsystem1_MODE ) { localB -> B_36_0_0 = rtu_dq * muDoubleScalarCos (
rtu_wt ) - rtu_dq_m * muDoubleScalarSin ( rtu_wt ) ; localB -> B_36_1_0 =
rtu_dq * muDoubleScalarSin ( rtu_wt ) + rtu_dq_m * muDoubleScalarCos ( rtu_wt
) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_Subsystem1_jp_Term ( SimStruct * const S ) { } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_c_Init (
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_c_T * localB ,
P_Subsystempi2delay_MR_CIGRE_GENERALIZADO_e_T * localP ) { localB ->
B_353_0_0 = localP -> P_0 [ 0 ] ; localB -> B_353_1_0 = localP -> P_0 [ 1 ] ;
} void MR_CIGRE_GENERALIZADO_Subsystempi2delay_l ( SimStruct * S , uint8_T
rtu_Enable , const real_T rtu_dq [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_GENERALIZADO_c_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_GENERALIZADO_g_T * localDW ) { if ( rtu_Enable
> 0 ) { localB -> B_353_0_0 = rtu_dq [ 0 ] * muDoubleScalarSin ( rtu_wt ) +
rtu_dq [ 1 ] * muDoubleScalarCos ( rtu_wt ) ; localB -> B_353_1_0 = - rtu_dq
[ 0 ] * muDoubleScalarCos ( rtu_wt ) + rtu_dq [ 1 ] * muDoubleScalarSin (
rtu_wt ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystempi2delay_SubsysRanBC ) ; } } } void
MR_CIGRE_GENERALIZADO_Subsystempi2delay_k_Term ( SimStruct * const S ) { }
void MR_CIGRE_GENERALIZADO_Subsystem1_e_Init (
B_Subsystem1_MR_CIGRE_GENERALIZADO_g_T * localB ,
P_Subsystem1_MR_CIGRE_GENERALIZADO_lv_T * localP ) { localB -> B_354_0_0 =
localP -> P_0 [ 0 ] ; localB -> B_354_1_0 = localP -> P_0 [ 1 ] ; } void
MR_CIGRE_GENERALIZADO_Subsystem1_l ( SimStruct * S , uint8_T rtu_Enable ,
const real_T rtu_dq [ 2 ] , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_GENERALIZADO_g_T * localB ,
DW_Subsystem1_MR_CIGRE_GENERALIZADO_m_T * localDW ) { if ( rtu_Enable > 0 ) {
localB -> B_354_0_0 = rtu_dq [ 0 ] * muDoubleScalarCos ( rtu_wt ) - rtu_dq [
1 ] * muDoubleScalarSin ( rtu_wt ) ; localB -> B_354_1_0 = rtu_dq [ 0 ] *
muDoubleScalarSin ( rtu_wt ) + rtu_dq [ 1 ] * muDoubleScalarCos ( rtu_wt ) ;
if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( localDW ->
Subsystem1_SubsysRanBC ) ; } } } void MR_CIGRE_GENERALIZADO_Subsystem1_a_Term
( SimStruct * const S ) { } static void mdlOutputs ( SimStruct * S , int_T
tid ) { creal_T B_392_279_0 ; creal_T B_392_300_0 ; creal_T B_392_321_0 ;
creal_T B_392_353_0 ; creal_T B_392_374_0 ; creal_T B_392_395_0 ; creal_T
B_392_452_0 ; creal_T B_392_473_0 ; creal_T B_392_494_0 ; creal_T B_392_526_0
; creal_T B_392_547_0 ; creal_T B_392_568_0 ; creal_T B_392_1302_0 ; creal_T
B_392_1323_0 ; creal_T B_392_1344_0 ; creal_T B_392_1375_0 ; creal_T
B_392_1396_0 ; creal_T B_392_1417_0 ; creal_T B_392_1459_0 ; creal_T
B_392_1480_0 ; creal_T B_392_1501_0 ; creal_T B_392_1739_0 ; creal_T
B_392_1760_0 ; creal_T B_392_1781_0 ; creal_T B_392_1812_0 ; creal_T
B_392_1833_0 ; creal_T B_392_1854_0 ; creal_T B_392_1893_0 ; creal_T
B_392_1914_0 ; creal_T B_392_1935_0 ; creal_T B_392_1968_0 ; creal_T
B_392_1989_0 ; creal_T B_392_2010_0 ; creal_T B_392_2191_0 ; creal_T
B_392_2212_0 ; creal_T B_392_2233_0 ; creal_T B_392_2264_0 ; creal_T
B_392_2285_0 ; creal_T B_392_2306_0 ; creal_T B_392_2345_0 ; creal_T
B_392_2366_0 ; creal_T B_392_2387_0 ; creal_T B_392_2420_0 ; creal_T
B_392_2441_0 ; creal_T B_392_2462_0 ; creal_T B_392_2591_0 ; creal_T
B_392_2612_0 ; creal_T B_392_2633_0 ; creal_T B_392_2664_0 ; creal_T
B_392_2685_0 ; creal_T B_392_2706_0 ; creal_T B_392_2745_0 ; creal_T
B_392_2766_0 ; creal_T B_392_2787_0 ; creal_T B_392_2820_0 ; creal_T
B_392_2841_0 ; creal_T B_392_2862_0 ; creal_T B_392_3043_0 ; creal_T
B_392_3064_0 ; creal_T B_392_3085_0 ; creal_T B_392_3116_0 ; creal_T
B_392_3137_0 ; creal_T B_392_3158_0 ; creal_T B_392_3197_0 ; creal_T
B_392_3218_0 ; creal_T B_392_3239_0 ; creal_T B_392_3272_0 ; creal_T
B_392_3293_0 ; creal_T B_392_3314_0 ; creal_T B_392_3495_0 ; creal_T
B_392_3516_0 ; creal_T B_392_3537_0 ; creal_T B_392_3568_0 ; creal_T
B_392_3589_0 ; creal_T B_392_3610_0 ; creal_T B_392_3649_0 ; creal_T
B_392_3670_0 ; creal_T B_392_3691_0 ; creal_T B_392_3724_0 ; creal_T
B_392_3745_0 ; creal_T B_392_3766_0 ; creal_T B_392_3949_0 ; creal_T
B_392_3970_0 ; creal_T B_392_3991_0 ; creal_T B_392_4023_0 ; creal_T
B_392_4044_0 ; creal_T B_392_4065_0 ; creal_T B_392_4483_0 ; creal_T
B_392_4504_0 ; creal_T B_392_4525_0 ; creal_T B_392_4557_0 ; creal_T
B_392_4578_0 ; creal_T B_392_4599_0 ; creal_T B_392_5015_0 ; creal_T
B_392_5036_0 ; creal_T B_392_5057_0 ; creal_T B_392_5088_0 ; creal_T
B_392_5109_0 ; creal_T B_392_5130_0 ; creal_T B_392_5169_0 ; creal_T
B_392_5190_0 ; creal_T B_392_5211_0 ; creal_T B_392_5244_0 ; creal_T
B_392_5265_0 ; creal_T B_392_5286_0 ; real_T B_392_74_0 [ 3 ] ; real_T
B_392_98_0 [ 3 ] ; real_T B_392_260_0 ; real_T B_392_268_0 ; real_T
B_392_281_0 ; real_T B_392_289_0 ; real_T B_392_302_0 ; real_T B_392_310_0 ;
real_T B_392_334_0 ; real_T B_392_342_0 ; real_T B_392_355_0 ; real_T
B_392_363_0 ; real_T B_392_376_0 ; real_T B_392_384_0 ; real_T B_392_433_0 ;
real_T B_392_441_0 ; real_T B_392_454_0 ; real_T B_392_462_0 ; real_T
B_392_475_0 ; real_T B_392_483_0 ; real_T B_392_507_0 ; real_T B_392_515_0 ;
real_T B_392_528_0 ; real_T B_392_536_0 ; real_T B_392_549_0 ; real_T
B_392_557_0 ; real_T B_392_752_0 [ 3 ] ; real_T B_392_823_0 [ 3 ] ; real_T
B_392_889_0 ; real_T B_392_902_0 ; real_T B_392_1090_0 [ 3 ] ; real_T
B_392_1160_0 [ 3 ] ; real_T B_392_1227_0 ; real_T B_392_1240_0 ; real_T
B_392_1283_0 ; real_T B_392_1291_0 ; real_T B_392_1304_0 ; real_T
B_392_1312_0 ; real_T B_392_1325_0 ; real_T B_392_1333_0 ; real_T
B_392_1356_0 ; real_T B_392_1364_0 ; real_T B_392_1377_0 ; real_T
B_392_1385_0 ; real_T B_392_1398_0 ; real_T B_392_1406_0 ; real_T
B_392_1440_0 ; real_T B_392_1448_0 ; real_T B_392_1461_0 ; real_T
B_392_1469_0 ; real_T B_392_1482_0 ; real_T B_392_1490_0 ; real_T
B_392_1592_0 ; real_T B_392_1664_0 [ 3 ] ; real_T B_392_1665_0 [ 3 ] ; real_T
B_392_1693_0 [ 3 ] ; real_T B_392_1694_0 [ 3 ] ; real_T B_392_1720_0 ; real_T
B_392_1728_0 ; real_T B_392_1741_0 ; real_T B_392_1749_0 ; real_T
B_392_1762_0 ; real_T B_392_1770_0 ; real_T B_392_1793_0 ; real_T
B_392_1801_0 ; real_T B_392_1814_0 ; real_T B_392_1822_0 ; real_T
B_392_1835_0 ; real_T B_392_1843_0 ; real_T B_392_1874_0 ; real_T
B_392_1882_0 ; real_T B_392_1895_0 ; real_T B_392_1903_0 ; real_T
B_392_1916_0 ; real_T B_392_1924_0 ; real_T B_392_1949_0 ; real_T
B_392_1957_0 ; real_T B_392_1970_0 ; real_T B_392_1978_0 ; real_T
B_392_1991_0 ; real_T B_392_1999_0 ; real_T B_392_2079_0 ; real_T
B_392_2116_0 ; real_T B_392_2172_0 ; real_T B_392_2180_0 ; real_T
B_392_2193_0 ; real_T B_392_2201_0 ; real_T B_392_2214_0 ; real_T
B_392_2222_0 ; real_T B_392_2245_0 ; real_T B_392_2253_0 ; real_T
B_392_2266_0 ; real_T B_392_2274_0 ; real_T B_392_2287_0 ; real_T
B_392_2295_0 ; real_T B_392_2326_0 ; real_T B_392_2334_0 ; real_T
B_392_2347_0 ; real_T B_392_2355_0 ; real_T B_392_2368_0 ; real_T
B_392_2376_0 ; real_T B_392_2401_0 ; real_T B_392_2409_0 ; real_T
B_392_2422_0 ; real_T B_392_2430_0 ; real_T B_392_2443_0 ; real_T
B_392_2451_0 ; real_T B_392_2529_0 ; real_T B_392_2566_0 ; real_T
B_392_2572_0 ; real_T B_392_2580_0 ; real_T B_392_2593_0 ; real_T
B_392_2601_0 ; real_T B_392_2614_0 ; real_T B_392_2622_0 ; real_T
B_392_2645_0 ; real_T B_392_2653_0 ; real_T B_392_2666_0 ; real_T
B_392_2674_0 ; real_T B_392_2687_0 ; real_T B_392_2695_0 ; real_T
B_392_2726_0 ; real_T B_392_2734_0 ; real_T B_392_2747_0 ; real_T
B_392_2755_0 ; real_T B_392_2768_0 ; real_T B_392_2776_0 ; real_T
B_392_2801_0 ; real_T B_392_2809_0 ; real_T B_392_2822_0 ; real_T
B_392_2830_0 ; real_T B_392_2843_0 ; real_T B_392_2851_0 ; real_T
B_392_2931_0 ; real_T B_392_2968_0 ; real_T B_392_3024_0 ; real_T
B_392_3032_0 ; real_T B_392_3045_0 ; real_T B_392_3053_0 ; real_T
B_392_3066_0 ; real_T B_392_3074_0 ; real_T B_392_3097_0 ; real_T
B_392_3105_0 ; real_T B_392_3118_0 ; real_T B_392_3126_0 ; real_T
B_392_3139_0 ; real_T B_392_3147_0 ; real_T B_392_3178_0 ; real_T
B_392_3186_0 ; real_T B_392_3199_0 ; real_T B_392_3207_0 ; real_T
B_392_3220_0 ; real_T B_392_3228_0 ; real_T B_392_3253_0 ; real_T
B_392_3261_0 ; real_T B_392_3274_0 ; real_T B_392_3282_0 ; real_T
B_392_3295_0 ; real_T B_392_3303_0 ; real_T B_392_3383_0 ; real_T
B_392_3420_0 ; real_T B_392_3476_0 ; real_T B_392_3484_0 ; real_T
B_392_3497_0 ; real_T B_392_3505_0 ; real_T B_392_3518_0 ; real_T
B_392_3526_0 ; real_T B_392_3549_0 ; real_T B_392_3557_0 ; real_T
B_392_3570_0 ; real_T B_392_3578_0 ; real_T B_392_3591_0 ; real_T
B_392_3599_0 ; real_T B_392_3630_0 ; real_T B_392_3638_0 ; real_T
B_392_3651_0 ; real_T B_392_3659_0 ; real_T B_392_3672_0 ; real_T
B_392_3680_0 ; real_T B_392_3705_0 ; real_T B_392_3713_0 ; real_T
B_392_3726_0 ; real_T B_392_3734_0 ; real_T B_392_3747_0 ; real_T
B_392_3755_0 ; real_T B_392_3835_0 ; real_T B_392_3872_0 ; real_T
B_392_3930_0 ; real_T B_392_3938_0 ; real_T B_392_3951_0 ; real_T
B_392_3959_0 ; real_T B_392_3972_0 ; real_T B_392_3980_0 ; real_T
B_392_4004_0 ; real_T B_392_4012_0 ; real_T B_392_4025_0 ; real_T
B_392_4033_0 ; real_T B_392_4046_0 ; real_T B_392_4054_0 ; real_T
B_392_4088_0 [ 3 ] ; real_T B_392_4125_0 [ 3 ] ; real_T B_392_4162_0 ; real_T
B_392_4174_0 [ 3 ] ; real_T B_392_4252_0 [ 3 ] ; real_T B_392_4255_0 [ 2 ] ;
real_T B_392_4260_0 [ 3 ] ; real_T B_392_4321_0 ; real_T B_392_4334_0 ;
real_T B_392_4375_0 ; real_T B_392_4427_0 ; real_T B_392_4428_0 ; real_T
B_392_4435_0 ; real_T B_392_4436_0 ; real_T B_392_4464_0 ; real_T
B_392_4472_0 ; real_T B_392_4485_0 ; real_T B_392_4493_0 ; real_T
B_392_4506_0 ; real_T B_392_4514_0 ; real_T B_392_4538_0 ; real_T
B_392_4546_0 ; real_T B_392_4559_0 ; real_T B_392_4567_0 ; real_T
B_392_4580_0 ; real_T B_392_4588_0 ; real_T B_392_4622_0 [ 3 ] ; real_T
B_392_4659_0 [ 3 ] ; real_T B_392_4696_0 ; real_T B_392_4708_0 [ 3 ] ; real_T
B_392_4786_0 [ 3 ] ; real_T B_392_4789_0 [ 2 ] ; real_T B_392_4794_0 [ 3 ] ;
real_T B_392_4855_0 ; real_T B_392_4868_0 ; real_T B_392_4909_0 ; real_T
B_392_4961_0 ; real_T B_392_4962_0 ; real_T B_392_4969_0 ; real_T
B_392_4970_0 ; real_T B_392_4996_0 ; real_T B_392_5004_0 ; real_T
B_392_5017_0 ; real_T B_392_5025_0 ; real_T B_392_5038_0 ; real_T
B_392_5046_0 ; real_T B_392_5069_0 ; real_T B_392_5077_0 ; real_T
B_392_5090_0 ; real_T B_392_5098_0 ; real_T B_392_5111_0 ; real_T
B_392_5119_0 ; real_T B_392_5150_0 ; real_T B_392_5158_0 ; real_T
B_392_5171_0 ; real_T B_392_5179_0 ; real_T B_392_5192_0 ; real_T
B_392_5200_0 ; real_T B_392_5225_0 ; real_T B_392_5233_0 ; real_T
B_392_5246_0 ; real_T B_392_5254_0 ; real_T B_392_5267_0 ; real_T
B_392_5275_0 ; real_T B_392_5354_0 ; real_T B_392_5391_0 ; real_T
B_392_5498_0 [ 3 ] ; real_T B_392_5499_0 [ 3 ] ; real_T B_368_1_0 [ 3 ] ;
real_T B_368_42_0 ; real_T B_331_1_0 [ 3 ] ; real_T B_331_42_0 ; real_T
B_48_1_0 [ 3 ] ; real_T B_48_42_0 ; real_T B_12_1_0 [ 3 ] ; real_T B_12_42_0
; real_T * lastU ; real_T rtb_B_392_1128_0 ; real_T rtb_B_392_790_0 ; real_T
rtb_B_392_410_0 ; real_T rtb_B_392_237_0 ; real_T rtb_B_392_4670_0 ; real_T
rtb_B_392_4136_0 ; real_T rtb_B_392_430_0 ; real_T rtb_B_392_257_0 ; real_T
rtb_B_392_5451_0 ; real_T rtb_B_392_5397_0 ; real_T rtb_B_392_32_0 [ 3 ] ;
real_T rtb_B_392_3878_0 ; real_T rtb_B_392_3426_0 ; real_T rtb_B_392_2974_0 ;
real_T rtb_B_392_2122_0 ; real_T rtb_B_392_1600_0 ; boolean_T rtb_B_392_263_0
; real_T rtb_B_392_239_0 ; real_T rtb_B_392_222_0 ; real_T rtb_B_392_185_0 ;
real_T rtb_B_392_183_0 ; real_T rtb_B_392_16_0 ; real_T rtb_B_392_4_1 ;
real_T rtb_B_392_117_0 ; real_T tmp [ 3 ] ; real_T tmp_0 ; real_T tmp_1 ;
int32_T isHit ; B_MR_CIGRE_GENERALIZADO_T * _rtB ; P_MR_CIGRE_GENERALIZADO_T
* _rtP ; X_MR_CIGRE_GENERALIZADO_T * _rtX ; DW_MR_CIGRE_GENERALIZADO_T *
_rtDW ; _rtDW = ( ( DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ;
_rtX = ( ( X_MR_CIGRE_GENERALIZADO_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ; _rtB -> B_392_1_0
= 0.0 ; _rtB -> B_392_1_0 += _rtP -> P_412 * _rtX -> LPF2_CSTATE ; _rtB ->
B_392_2_0 = 0.0 ; _rtB -> B_392_2_0 += _rtP -> P_414 * _rtX -> LPF1_CSTATE ;
_rtB -> B_392_3_0 . re = _rtB -> B_392_1_0 ; _rtB -> B_392_3_0 . im = _rtB ->
B_392_2_0 ; _rtB -> B_392_4_0 = muDoubleScalarHypot ( _rtB -> B_392_3_0 . re
, _rtB -> B_392_3_0 . im ) ; rtb_B_392_4_1 = muDoubleScalarAtan2 ( _rtB ->
B_392_3_0 . im , _rtB -> B_392_3_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0
) { _rtDW -> Saturation_MODE = _rtB -> B_392_4_0 >= _rtP -> P_415 ? 1 : _rtB
-> B_392_4_0 > _rtP -> P_416 ? 0 : - 1 ; } _rtB -> B_392_5_0 = _rtDW ->
Saturation_MODE == 1 ? _rtP -> P_415 : _rtDW -> Saturation_MODE == - 1 ? _rtP
-> P_416 : _rtB -> B_392_4_0 ; _rtB -> B_392_7_0 = _rtB -> B_392_5_0 *
muDoubleScalarCos ( rtb_B_392_4_1 ) ; _rtB -> B_392_7_1 = _rtB -> B_392_5_0 *
muDoubleScalarSin ( rtb_B_392_4_1 ) ; isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_9_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE > _rtP -> P_417 ) ; if ( _rtDW ->
Initial_FirstOutputTime ) { _rtDW -> Initial_FirstOutputTime = false ; _rtB
-> B_392_11_0 = _rtP -> P_418 ; } else { _rtB -> B_392_11_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE - _rtP -> P_417 ; } if ( _rtB -> B_392_9_0 && (
_rtDW -> DiscreteTimeIntegrator_PrevResetState <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE = _rtB -> B_392_11_0 ; } _rtB -> B_392_12_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE ; } isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { rtb_B_392_16_0 = ssGetTaskTime ( S , 1 ) ; _rtDW ->
PQparaVf_MODE = ( rtb_B_392_16_0 >= _rtP -> P_422 ) ; if ( 0.0 > _rtP ->
P_425 ) { rtb_B_392_16_0 = _rtB -> B_392_13_0 ; } else if ( _rtDW ->
PQparaVf_MODE == 1 ) { rtb_B_392_16_0 = _rtP -> P_424 ; } else {
rtb_B_392_16_0 = _rtP -> P_423 ; } if ( _rtB -> B_392_14_0 > _rtP -> P_426 )
{ _rtB -> B_392_17_0 = _rtB -> B_392_13_0 ; } else { _rtB -> B_392_17_0 =
rtb_B_392_16_0 ; } } _rtB -> B_392_18_0 = _rtX -> Integrator1_CSTATE ; if (
_rtB -> B_392_17_0 > _rtP -> P_430 ) { _rtB -> B_392_19_0 = _rtB ->
B_392_12_0 ; } else { _rtB -> B_392_19_0 = _rtB -> B_392_18_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_23_0 = _rtB
-> B_392_22_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1_e ( S , _rtB -> B_392_23_0
, _rtB -> B_392_7_0 , _rtB -> B_392_7_1 , _rtB -> B_392_19_0 , & _rtB ->
Subsystem1 , & _rtDW -> Subsystem1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_27_0 = _rtB -> B_392_26_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay_o ( S , _rtB -> B_392_27_0 , _rtB ->
B_392_7_0 , _rtB -> B_392_7_1 , _rtB -> B_392_19_0 , & _rtB ->
Subsystempi2delay , & _rtDW -> Subsystempi2delay ) ; if ( _rtB -> B_392_22_0
!= 0 ) { tmp_0 = _rtB -> Subsystem1 . B_36_0_0 ; tmp_1 = _rtB -> Subsystem1 .
B_36_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay . B_35_0_0 ; tmp_1 =
_rtB -> Subsystempi2delay . B_35_1_0 ; } for ( isHit = 0 ; isHit < 3 ; isHit
++ ) { rtb_B_392_32_0 [ isHit ] = _rtP -> P_435 [ isHit + 6 ] * _rtB ->
B_392_30_0 + ( _rtP -> P_435 [ isHit + 3 ] * tmp_1 + _rtP -> P_435 [ isHit ]
* tmp_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_392_33_0 = _rtDW -> Vdc_DSTATE ; _rtB -> B_392_34_0 = _rtP -> P_437 *
_rtB -> B_392_33_0 ; } rtb_B_392_32_0 [ 0 ] *= _rtB -> B_392_34_0 ;
rtb_B_392_32_0 [ 1 ] *= _rtB -> B_392_34_0 ; _rtB -> B_392_36_0 =
rtb_B_392_32_0 [ 0 ] - rtb_B_392_32_0 [ 1 ] ; _rtB -> B_392_37_0 =
rtb_B_392_32_0 [ 1 ] - rtb_B_392_32_0 [ 2 ] * _rtB -> B_392_34_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_38_0 = _rtDW
-> Idc_DSTATE ; } _rtB -> B_392_39_0 = 0.0 ; _rtB -> B_392_39_0 += _rtP ->
P_440 * _rtX -> LPF2_CSTATE_e ; _rtB -> B_392_40_0 = 0.0 ; _rtB -> B_392_40_0
+= _rtP -> P_442 * _rtX -> LPF1_CSTATE_g ; _rtB -> B_392_41_0 . re = _rtB ->
B_392_39_0 ; _rtB -> B_392_41_0 . im = _rtB -> B_392_40_0 ; _rtB ->
B_392_42_0 = muDoubleScalarHypot ( _rtB -> B_392_41_0 . re , _rtB ->
B_392_41_0 . im ) ; rtb_B_392_4_1 = muDoubleScalarAtan2 ( _rtB -> B_392_41_0
. im , _rtB -> B_392_41_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) {
_rtDW -> Saturation_MODE_p = _rtB -> B_392_42_0 >= _rtP -> P_443 ? 1 : _rtB
-> B_392_42_0 > _rtP -> P_444 ? 0 : - 1 ; } _rtB -> B_392_43_0 = _rtDW ->
Saturation_MODE_p == 1 ? _rtP -> P_443 : _rtDW -> Saturation_MODE_p == - 1 ?
_rtP -> P_444 : _rtB -> B_392_42_0 ; _rtB -> B_392_45_0 = _rtB -> B_392_43_0
* muDoubleScalarCos ( rtb_B_392_4_1 ) ; _rtB -> B_392_45_1 = _rtB ->
B_392_43_0 * muDoubleScalarSin ( rtb_B_392_4_1 ) ; isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_47_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_k > _rtP -> P_445 ) ; if ( _rtDW ->
Initial_FirstOutputTime_f ) { _rtDW -> Initial_FirstOutputTime_f = false ;
_rtB -> B_392_49_0 = _rtP -> P_446 ; } else { _rtB -> B_392_49_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_k - _rtP -> P_445 ; } if ( _rtB -> B_392_47_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_n <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_k = _rtB -> B_392_49_0 ; } _rtB -> B_392_50_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE_k ; } isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_51_0 > _rtP -> P_449 ) { _rtB ->
B_392_52_0 = _rtB -> B_392_13_0 ; } else { _rtB -> B_392_52_0 =
rtb_B_392_16_0 ; } } _rtB -> B_392_53_0 = _rtX -> Integrator1_CSTATE_o ; if (
_rtB -> B_392_52_0 > _rtP -> P_453 ) { _rtB -> B_392_54_0 = _rtB ->
B_392_50_0 ; } else { _rtB -> B_392_54_0 = _rtB -> B_392_53_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_58_0 = _rtB
-> B_392_57_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1_e ( S , _rtB -> B_392_58_0
, _rtB -> B_392_45_0 , _rtB -> B_392_45_1 , _rtB -> B_392_54_0 , & _rtB ->
Subsystem1_e , & _rtDW -> Subsystem1_e ) ; isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { _rtB -> B_392_62_0 = _rtB -> B_392_61_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay_o ( S , _rtB -> B_392_62_0 , _rtB ->
B_392_45_0 , _rtB -> B_392_45_1 , _rtB -> B_392_54_0 , & _rtB ->
Subsystempi2delay_a , & _rtDW -> Subsystempi2delay_a ) ; if ( _rtB ->
B_392_57_0 != 0 ) { tmp_0 = _rtB -> Subsystem1_e . B_36_0_0 ; tmp_1 = _rtB ->
Subsystem1_e . B_36_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay_a .
B_35_0_0 ; tmp_1 = _rtB -> Subsystempi2delay_a . B_35_1_0 ; } for ( isHit = 0
; isHit < 3 ; isHit ++ ) { rtb_B_392_32_0 [ isHit ] = _rtP -> P_458 [ isHit +
6 ] * _rtB -> B_392_65_0 + ( _rtP -> P_458 [ isHit + 3 ] * tmp_1 + _rtP ->
P_458 [ isHit ] * tmp_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_68_0 = _rtDW -> Vdc_DSTATE_p ; _rtB ->
B_392_69_0 = _rtP -> P_460 * _rtB -> B_392_68_0 ; } rtb_B_392_32_0 [ 0 ] *=
_rtB -> B_392_69_0 ; rtb_B_392_32_0 [ 1 ] *= _rtB -> B_392_69_0 ; _rtB ->
B_392_71_0 = rtb_B_392_32_0 [ 0 ] - rtb_B_392_32_0 [ 1 ] ; _rtB -> B_392_72_0
= rtb_B_392_32_0 [ 1 ] - rtb_B_392_32_0 [ 2 ] * _rtB -> B_392_69_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_73_0 = _rtDW
-> Idc_DSTATE_b ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
B_392_74_0 [ 0 ] = _rtDW -> UnitDelay1_DSTATE [ 0 ] ; B_392_74_0 [ 1 ] =
_rtDW -> UnitDelay1_DSTATE [ 1 ] ; B_392_74_0 [ 2 ] = _rtDW ->
UnitDelay1_DSTATE [ 2 ] ; _rtB -> B_392_76_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_a > _rtP -> P_463 ) ; if ( _rtDW ->
Initial_FirstOutputTime_g ) { _rtDW -> Initial_FirstOutputTime_g = false ;
_rtB -> B_392_78_0 = _rtP -> P_464 ; } else { _rtB -> B_392_78_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_a - _rtP -> P_463 ; } if ( _rtB -> B_392_76_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_p <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_a = _rtB -> B_392_78_0 ; } _rtB -> B_392_79_0 =
_rtDW -> DiscreteTimeIntegrator_DSTATE_a ; _rtB -> B_392_83_0 = _rtB ->
B_392_82_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_l ( S , _rtB -> B_392_83_0 , &
B_392_74_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystem1_l , & _rtDW ->
Subsystem1_l ) ; _rtB -> B_392_87_0 = _rtB -> B_392_86_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_l ( S , _rtB -> B_392_87_0 , &
B_392_74_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_l , &
_rtDW -> Subsystempi2delay_l ) ; if ( _rtB -> B_392_82_0 != 0 ) { tmp_0 =
_rtB -> Subsystem1_l . B_354_0_0 ; tmp_1 = _rtB -> Subsystem1_l . B_354_1_0 ;
} else { tmp_0 = _rtB -> Subsystempi2delay_l . B_353_0_0 ; tmp_1 = _rtB ->
Subsystempi2delay_l . B_353_1_0 ; } for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ _rtB -> B_392_91_0 [ isHit ] = 0.0 ; _rtB -> B_392_91_0 [ isHit ] += _rtP
-> P_469 [ isHit ] * tmp_0 ; _rtB -> B_392_91_0 [ isHit ] += _rtP -> P_469 [
isHit + 3 ] * tmp_1 ; _rtB -> B_392_91_0 [ isHit ] += _rtP -> P_469 [ isHit +
6 ] * B_392_74_0 [ 2 ] ; } } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_92_0 = _rtDW -> Vdc_DSTATE_g ; rtb_B_392_16_0 = _rtP
-> P_471 * _rtB -> B_392_92_0 ; rtb_B_392_32_0 [ 1 ] = _rtB -> B_392_91_0 [ 1
] * rtb_B_392_16_0 ; _rtB -> B_392_95_0 = _rtB -> B_392_91_0 [ 0 ] *
rtb_B_392_16_0 - rtb_B_392_32_0 [ 1 ] ; _rtB -> B_392_96_0 = rtb_B_392_32_0 [
1 ] - _rtB -> B_392_91_0 [ 2 ] * rtb_B_392_16_0 ; _rtB -> B_392_97_0 = _rtDW
-> Idc_DSTATE_h ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
B_392_98_0 [ 0 ] = _rtDW -> UnitDelay1_DSTATE_e [ 0 ] ; B_392_98_0 [ 1 ] =
_rtDW -> UnitDelay1_DSTATE_e [ 1 ] ; B_392_98_0 [ 2 ] = _rtDW ->
UnitDelay1_DSTATE_e [ 2 ] ; _rtB -> B_392_100_0 = ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq > _rtP -> P_474 ) ; if ( _rtDW ->
Initial_FirstOutputTime_o ) { _rtDW -> Initial_FirstOutputTime_o = false ;
_rtB -> B_392_102_0 = _rtP -> P_475 ; } else { _rtB -> B_392_102_0 = _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq - _rtP -> P_474 ; } if ( _rtB -> B_392_100_0
&& ( _rtDW -> DiscreteTimeIntegrator_PrevResetState_d <= 0 ) ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq = _rtB -> B_392_102_0 ; } _rtB ->
B_392_103_0 = _rtDW -> DiscreteTimeIntegrator_DSTATE_aq ; _rtB -> B_392_107_0
= _rtB -> B_392_106_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_l ( S , _rtB ->
B_392_107_0 , & B_392_98_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB ->
Subsystem1_m , & _rtDW -> Subsystem1_m ) ; _rtB -> B_392_111_0 = _rtB ->
B_392_110_0 ; MR_CIGRE_GENERALIZADO_Subsystempi2delay_l ( S , _rtB ->
B_392_111_0 , & B_392_98_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB ->
Subsystempi2delay_l4 , & _rtDW -> Subsystempi2delay_l4 ) ; if ( _rtB ->
B_392_106_0 != 0 ) { tmp_0 = _rtB -> Subsystem1_m . B_354_0_0 ; tmp_1 = _rtB
-> Subsystem1_m . B_354_1_0 ; } else { tmp_0 = _rtB -> Subsystempi2delay_l4 .
B_353_0_0 ; tmp_1 = _rtB -> Subsystempi2delay_l4 . B_353_1_0 ; } for ( isHit
= 0 ; isHit < 3 ; isHit ++ ) { _rtB -> B_392_115_0 [ isHit ] = 0.0 ; _rtB ->
B_392_115_0 [ isHit ] += _rtP -> P_480 [ isHit ] * tmp_0 ; _rtB ->
B_392_115_0 [ isHit ] += _rtP -> P_480 [ isHit + 3 ] * tmp_1 ; _rtB ->
B_392_115_0 [ isHit ] += _rtP -> P_480 [ isHit + 6 ] * B_392_98_0 [ 2 ] ; } }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_116_0
= _rtDW -> Vdc_DSTATE_b ; rtb_B_392_117_0 = _rtP -> P_482 * _rtB ->
B_392_116_0 ; rtb_B_392_32_0 [ 1 ] = _rtB -> B_392_115_0 [ 1 ] *
rtb_B_392_117_0 ; _rtB -> B_392_119_0 = _rtB -> B_392_115_0 [ 0 ] *
rtb_B_392_117_0 - rtb_B_392_32_0 [ 1 ] ; _rtB -> B_392_120_0 = rtb_B_392_32_0
[ 1 ] - _rtB -> B_392_115_0 [ 2 ] * rtb_B_392_117_0 ; _rtB -> B_392_121_0 =
_rtDW -> Idc_DSTATE_k ; if ( _rtB -> B_392_124_0 > _rtP -> P_487 ) {
rtb_B_392_117_0 = _rtP -> P_487 ; } else if ( _rtB -> B_392_124_0 < _rtP ->
P_488 ) { rtb_B_392_117_0 = _rtP -> P_488 ; } else { rtb_B_392_117_0 = _rtB
-> B_392_124_0 ; } _rtB -> B_392_132_0 [ 0 ] = rtb_B_392_117_0 * _rtB ->
B_392_131_0 ; _rtB -> B_392_132_0 [ 1 ] = ( _rtB -> B_392_127_0 -
rtb_B_392_117_0 ) * _rtB -> B_392_131_0 ; _rtB -> B_392_133_0 = _rtDW ->
UnitDelay5_DSTATE ; _rtB -> B_392_136_0 = _rtP -> P_492 * _rtB -> B_392_133_0
- _rtDW -> UnitDelay6_DSTATE ; _rtB -> B_392_137_0 [ 0 ] = _rtDW ->
UnitDelay1_DSTATE_p [ 0 ] ; _rtB -> B_392_137_0 [ 1 ] = _rtDW ->
UnitDelay1_DSTATE_p [ 1 ] ; ssCallAccelRunBlock ( S , 318 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; if ( _rtB -> B_392_139_0 > _rtP -> P_496 ) {
rtb_B_392_117_0 = _rtP -> P_496 ; } else if ( _rtB -> B_392_139_0 < _rtP ->
P_497 ) { rtb_B_392_117_0 = _rtP -> P_497 ; } else { rtb_B_392_117_0 = _rtB
-> B_392_139_0 ; } _rtB -> B_392_147_0 [ 0 ] = rtb_B_392_117_0 * _rtB ->
B_392_146_0 ; _rtB -> B_392_147_0 [ 1 ] = ( _rtB -> B_392_142_0 -
rtb_B_392_117_0 ) * _rtB -> B_392_146_0 ; _rtB -> B_392_148_0 = _rtDW ->
UnitDelay5_DSTATE_e ; _rtB -> B_392_151_0 = _rtP -> P_501 * _rtB ->
B_392_148_0 - _rtDW -> UnitDelay6_DSTATE_l ; _rtB -> B_392_152_0 [ 0 ] =
_rtDW -> UnitDelay1_DSTATE_m [ 0 ] ; _rtB -> B_392_152_0 [ 1 ] = _rtDW ->
UnitDelay1_DSTATE_m [ 1 ] ; ssCallAccelRunBlock ( S , 355 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; rtb_B_392_183_0 = _rtDW -> Delay_x_DSTATE ; _rtB ->
B_392_184_0 = _rtP -> P_522 * _rtDW -> Delay_x_DSTATE ; rtb_B_392_185_0 =
_rtDW -> Delay_x_DSTATE_h ; _rtB -> B_392_186_0 = _rtP -> P_524 * _rtDW ->
Delay_x_DSTATE_h ; rtb_B_392_117_0 = _rtDW -> Delay_x_DSTATE_e ; _rtB ->
B_392_221_0 = _rtP -> P_547 * _rtDW -> Delay_x_DSTATE_e ; rtb_B_392_222_0 =
_rtDW -> Delay_x_DSTATE_b ; _rtB -> B_392_223_0 = _rtP -> P_549 * _rtDW ->
Delay_x_DSTATE_b ; if ( _rtDW -> systemEnable != 0 ) { _rtDW -> lastSin =
muDoubleScalarSin ( _rtP -> P_556 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
lastCos = muDoubleScalarCos ( _rtP -> P_556 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> systemEnable = 0 ; } _rtB -> B_392_228_0 = ( ( _rtDW -> lastSin *
_rtP -> P_560 + _rtDW -> lastCos * _rtP -> P_559 ) * _rtP -> P_558 + ( _rtDW
-> lastCos * _rtP -> P_560 - _rtDW -> lastSin * _rtP -> P_559 ) * _rtP ->
P_557 ) * _rtP -> P_554 + _rtP -> P_555 ; if ( _rtDW -> systemEnable_f != 0 )
{ _rtDW -> lastSin_i = muDoubleScalarSin ( _rtP -> P_563 * ssGetTaskTime ( S
, 2 ) ) ; _rtDW -> lastCos_p = muDoubleScalarCos ( _rtP -> P_563 *
ssGetTaskTime ( S , 2 ) ) ; _rtDW -> systemEnable_f = 0 ; } _rtB ->
B_392_229_0 = ( ( _rtDW -> lastSin_i * _rtP -> P_567 + _rtDW -> lastCos_p *
_rtP -> P_566 ) * _rtP -> P_565 + ( _rtDW -> lastCos_p * _rtP -> P_567 -
_rtDW -> lastSin_i * _rtP -> P_566 ) * _rtP -> P_564 ) * _rtP -> P_561 + _rtP
-> P_562 ; if ( _rtDW -> systemEnable_n != 0 ) { _rtDW -> lastSin_p =
muDoubleScalarSin ( _rtP -> P_570 * ssGetTaskTime ( S , 2 ) ) ; _rtDW ->
lastCos_c = muDoubleScalarCos ( _rtP -> P_570 * ssGetTaskTime ( S , 2 ) ) ;
_rtDW -> systemEnable_n = 0 ; } _rtB -> B_392_230_0 = ( ( _rtDW -> lastSin_p
* _rtP -> P_574 + _rtDW -> lastCos_c * _rtP -> P_573 ) * _rtP -> P_572 + (
_rtDW -> lastCos_c * _rtP -> P_574 - _rtDW -> lastSin_p * _rtP -> P_573 ) *
_rtP -> P_571 ) * _rtP -> P_568 + _rtP -> P_569 ;
MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1 ( S ) ; }
isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> ref_MODE = ( rtb_B_392_16_0 >= _rtP ->
P_579 ) ; if ( _rtDW -> ref_MODE == 1 ) { rtb_B_392_237_0 = _rtP -> P_581 ; }
else { rtb_B_392_237_0 = _rtP -> P_580 ; } rtb_B_392_16_0 = ssGetTaskTime ( S
, 1 ) ; _rtDW -> Time_init_ref_MODE = ( rtb_B_392_16_0 >= _rtP -> P_583 ) ;
if ( _rtDW -> Time_init_ref_MODE == 1 ) { rtb_B_392_239_0 = _rtP -> P_585 ; }
else { rtb_B_392_239_0 = _rtP -> P_584 ; } _rtB -> B_392_240_0 = _rtB ->
B_392_238_0 * rtb_B_392_239_0 ; if ( ( _rtDW -> LastMajorTimeA >=
ssGetTaskTime ( S , 1 ) ) && ( _rtDW -> LastMajorTimeB >= ssGetTaskTime ( S ,
1 ) ) ) { _rtB -> B_392_241_0 = _rtB -> B_392_240_0 ; } else { if ( ( ( _rtDW
-> LastMajorTimeA < _rtDW -> LastMajorTimeB ) && ( _rtDW -> LastMajorTimeB <
ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW -> LastMajorTimeA >= _rtDW ->
LastMajorTimeB ) && ( _rtDW -> LastMajorTimeA >= ssGetTaskTime ( S , 1 ) ) )
) { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeB ; rtb_B_392_4_1
= _rtDW -> PrevYB ; } else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW ->
LastMajorTimeA ; rtb_B_392_4_1 = _rtDW -> PrevYA ; } tmp_1 = tmp_0 * _rtP ->
P_586 ; rtb_B_392_16_0 = _rtB -> B_392_240_0 - rtb_B_392_4_1 ; if (
rtb_B_392_16_0 > tmp_1 ) { _rtB -> B_392_241_0 = rtb_B_392_4_1 + tmp_1 ; }
else { tmp_0 *= _rtP -> P_587 ; if ( rtb_B_392_16_0 < tmp_0 ) { _rtB ->
B_392_241_0 = rtb_B_392_4_1 + tmp_0 ; } else { _rtB -> B_392_241_0 = _rtB ->
B_392_240_0 ; } } } _rtB -> B_392_243_0 = rtb_B_392_239_0 * _rtB ->
B_392_242_0 ; if ( ( _rtDW -> LastMajorTimeA_o >= ssGetTaskTime ( S , 1 ) )
&& ( _rtDW -> LastMajorTimeB_h >= ssGetTaskTime ( S , 1 ) ) ) { _rtB ->
B_392_244_0 = _rtB -> B_392_243_0 ; } else { if ( ( ( _rtDW ->
LastMajorTimeA_o < _rtDW -> LastMajorTimeB_h ) && ( _rtDW -> LastMajorTimeB_h
< ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW -> LastMajorTimeA_o >= _rtDW ->
LastMajorTimeB_h ) && ( _rtDW -> LastMajorTimeA_o >= ssGetTaskTime ( S , 1 )
) ) ) { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeB_h ;
rtb_B_392_4_1 = _rtDW -> PrevYB_l ; } else { tmp_0 = ssGetTaskTime ( S , 1 )
- _rtDW -> LastMajorTimeA_o ; rtb_B_392_4_1 = _rtDW -> PrevYA_e ; } tmp_1 =
tmp_0 * _rtP -> P_589 ; rtb_B_392_16_0 = _rtB -> B_392_243_0 - rtb_B_392_4_1
; if ( rtb_B_392_16_0 > tmp_1 ) { _rtB -> B_392_244_0 = rtb_B_392_4_1 + tmp_1
; } else { tmp_0 *= _rtP -> P_590 ; if ( rtb_B_392_16_0 < tmp_0 ) { _rtB ->
B_392_244_0 = rtb_B_392_4_1 + tmp_0 ; } else { _rtB -> B_392_244_0 = _rtB ->
B_392_243_0 ; } } } } _rtB -> B_392_245_0 = 0.0 ; _rtB -> B_392_245_0 += _rtP
-> P_592 * _rtX -> LPF1_CSTATE_o ; _rtB -> B_392_246_0 = 0.0 ; _rtB ->
B_392_246_0 += _rtP -> P_594 * _rtX -> LPF2_CSTATE_c ; isHit = ssIsSampleHit
( S , 2 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 16 , 0 ,
SS_CALL_MDL_OUTPUTS ) ; } _rtB -> B_392_249_0 = ( _rtB -> B_392_245_0 - _rtB
-> B_16_0_1 ) * _rtP -> P_595 ; _rtB -> B_392_251_0 = ( _rtB -> B_392_246_0 -
_rtB -> B_16_0_2 ) * _rtP -> P_596 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { ssCallAccelRunBlock ( S , 1 , 0 , SS_CALL_MDL_OUTPUTS ) ;
_rtB -> B_392_254_0 = _rtP -> P_598 * _rtB -> B_1_0_1 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_256_0 = (
rtb_B_392_237_0 - _rtB -> B_392_254_0 ) * _rtP -> P_599 ; } isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_257_0 = _rtDW ->
UnitDelay_DSTATE ; _rtB -> B_392_258_0 = _rtP -> P_601 * _rtDW ->
UnitDelay_DSTATE ; } _rtB -> B_392_259_0 = _rtX -> integrator_CSTATE ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_603 ; B_392_260_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK . CircularBufSize , &
_rtDW -> TransportDelay_IWORK . Last , _rtDW -> TransportDelay_IWORK . Tail ,
_rtDW -> TransportDelay_IWORK . Head , _rtP -> P_604 , 0 , ( boolean_T ) (
ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) )
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_262_0 = _rtP -> P_605 ; } rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB ->
B_392_262_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_392_264_0 = _rtDW -> Memory_PreviousInput ; } if ( rtb_B_392_263_0
) { _rtB -> B_392_266_0 = ( _rtB -> B_392_259_0 - B_392_260_0 ) * _rtP ->
P_63 ; } else { _rtB -> B_392_266_0 = _rtB -> B_392_264_0 ; } _rtB ->
B_392_267_0 = _rtX -> integrator_CSTATE_n ; { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1
] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_608 ; B_392_268_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_h . Last , _rtDW ->
TransportDelay_IWORK_h . Tail , _rtDW -> TransportDelay_IWORK_h . Head , _rtP
-> P_609 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_270_0 = _rtP -> P_610 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_270_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_272_0 = _rtDW
-> Memory_PreviousInput_a ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_274_0 =
( _rtB -> B_392_267_0 - B_392_268_0 ) * _rtP -> P_62 ; } else { _rtB ->
B_392_274_0 = _rtB -> B_392_272_0 ; } rtb_B_392_237_0 = _rtP -> P_612 *
muDoubleScalarAtan2 ( _rtB -> B_392_274_0 , _rtB -> B_392_266_0 ) * _rtP ->
P_613 ; B_392_279_0 . re = muDoubleScalarHypot ( _rtB -> B_392_266_0 , _rtB
-> B_392_274_0 ) * muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_279_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_266_0 , _rtB -> B_392_274_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; _rtB -> B_392_280_0 = _rtX ->
integrator_CSTATE_g ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_615 ; B_392_281_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_l . Last , _rtDW -> TransportDelay_IWORK_l .
Tail , _rtDW -> TransportDelay_IWORK_l . Head , _rtP -> P_616 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_283_0 = _rtP -> P_617 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_283_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_285_0 = _rtDW -> Memory_PreviousInput_av ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_287_0 = ( _rtB -> B_392_280_0 - B_392_281_0
) * _rtP -> P_65 ; } else { _rtB -> B_392_287_0 = _rtB -> B_392_285_0 ; }
_rtB -> B_392_288_0 = _rtX -> integrator_CSTATE_e ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_620 ; B_392_289_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_k .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_k . Last , _rtDW ->
TransportDelay_IWORK_k . Tail , _rtDW -> TransportDelay_IWORK_k . Head , _rtP
-> P_621 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_291_0 = _rtP -> P_622 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_291_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_293_0 = _rtDW
-> Memory_PreviousInput_e ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_295_0 =
( _rtB -> B_392_288_0 - B_392_289_0 ) * _rtP -> P_64 ; } else { _rtB ->
B_392_295_0 = _rtB -> B_392_293_0 ; } rtb_B_392_237_0 = _rtP -> P_624 *
muDoubleScalarAtan2 ( _rtB -> B_392_295_0 , _rtB -> B_392_287_0 ) * _rtP ->
P_625 ; B_392_300_0 . re = muDoubleScalarHypot ( _rtB -> B_392_287_0 , _rtB
-> B_392_295_0 ) * muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_300_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_287_0 , _rtB -> B_392_295_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; _rtB -> B_392_301_0 = _rtX ->
integrator_CSTATE_l ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_o0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_627 ;
B_392_302_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_c . Last , _rtDW ->
TransportDelay_IWORK_c . Tail , _rtDW -> TransportDelay_IWORK_c . Head , _rtP
-> P_628 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_304_0 = _rtP -> P_629 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_304_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_306_0 = _rtDW
-> Memory_PreviousInput_d ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_308_0 =
( _rtB -> B_392_301_0 - B_392_302_0 ) * _rtP -> P_67 ; } else { _rtB ->
B_392_308_0 = _rtB -> B_392_306_0 ; } _rtB -> B_392_309_0 = _rtX ->
integrator_CSTATE_p ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_632 ; B_392_310_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_m . Last , _rtDW -> TransportDelay_IWORK_m .
Tail , _rtDW -> TransportDelay_IWORK_m . Head , _rtP -> P_633 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_312_0 = _rtP -> P_634 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_312_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_314_0 = _rtDW -> Memory_PreviousInput_ac ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_316_0 = ( _rtB -> B_392_309_0 - B_392_310_0
) * _rtP -> P_66 ; } else { _rtB -> B_392_316_0 = _rtB -> B_392_314_0 ; }
rtb_B_392_237_0 = _rtP -> P_636 * muDoubleScalarAtan2 ( _rtB -> B_392_316_0 ,
_rtB -> B_392_308_0 ) * _rtP -> P_637 ; B_392_321_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_308_0 , _rtB -> B_392_316_0 ) *
muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_321_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_308_0 , _rtB -> B_392_316_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_323_0 = _rtB -> B_392_322_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_323_0 ,
B_392_279_0 , B_392_300_0 , B_392_321_0 , & _rtB -> PosSeqComputation , &
_rtDW -> PosSeqComputation , & _rtP -> PosSeqComputation ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_326_0 = _rtB
-> B_392_325_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_326_0 , B_392_279_0 , B_392_300_0 , B_392_321_0 , & _rtB ->
NegSeqComputation , & _rtDW -> NegSeqComputation , & _rtP ->
NegSeqComputation ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_392_329_0 = _rtB -> B_392_328_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_329_0 ,
B_392_279_0 , B_392_300_0 , B_392_321_0 , & _rtB -> ZeroSeqComputation , &
_rtDW -> ZeroSeqComputation , & _rtP -> ZeroSeqComputation ) ; rtb_B_392_32_0
[ 0 ] = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation . B_23_2_0 . im ,
_rtB -> PosSeqComputation . B_23_2_0 . re ) ; _rtB -> B_392_332_0 = _rtP ->
P_641 * muDoubleScalarHypot ( _rtB -> PosSeqComputation . B_23_2_0 . re ,
_rtB -> PosSeqComputation . B_23_2_0 . im ) ; _rtB -> B_392_333_0 = _rtX ->
integrator_CSTATE_f ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_643 ; B_392_334_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_a . Last , _rtDW -> TransportDelay_IWORK_a .
Tail , _rtDW -> TransportDelay_IWORK_a . Head , _rtP -> P_644 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_336_0 = _rtP -> P_645 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_336_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_338_0 = _rtDW -> Memory_PreviousInput_am ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_340_0 = ( _rtB -> B_392_333_0 - B_392_334_0
) * _rtP -> P_69 ; } else { _rtB -> B_392_340_0 = _rtB -> B_392_338_0 ; }
_rtB -> B_392_341_0 = _rtX -> integrator_CSTATE_k ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_me . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_648 ; B_392_342_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p . Last , _rtDW -> TransportDelay_IWORK_p .
Tail , _rtDW -> TransportDelay_IWORK_p . Head , _rtP -> P_649 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_344_0 = _rtP -> P_650 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_344_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_346_0 = _rtDW -> Memory_PreviousInput_f ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_348_0 = ( _rtB -> B_392_341_0 - B_392_342_0
) * _rtP -> P_68 ; } else { _rtB -> B_392_348_0 = _rtB -> B_392_346_0 ; }
rtb_B_392_237_0 = _rtP -> P_652 * muDoubleScalarAtan2 ( _rtB -> B_392_348_0 ,
_rtB -> B_392_340_0 ) * _rtP -> P_653 ; B_392_353_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_340_0 , _rtB -> B_392_348_0 ) *
muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_353_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_340_0 , _rtB -> B_392_348_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; _rtB -> B_392_354_0 = _rtX ->
integrator_CSTATE_lb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_n . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_655 ; B_392_355_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_d . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_d . Last , _rtDW -> TransportDelay_IWORK_d .
Tail , _rtDW -> TransportDelay_IWORK_d . Head , _rtP -> P_656 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_357_0 = _rtP -> P_657 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_357_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_359_0 = _rtDW -> Memory_PreviousInput_o ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_361_0 = ( _rtB -> B_392_354_0 - B_392_355_0
) * _rtP -> P_71 ; } else { _rtB -> B_392_361_0 = _rtB -> B_392_359_0 ; }
_rtB -> B_392_362_0 = _rtX -> integrator_CSTATE_gs ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP
-> P_660 ; B_392_363_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dg
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_dg . Last , _rtDW ->
TransportDelay_IWORK_dg . Tail , _rtDW -> TransportDelay_IWORK_dg . Head ,
_rtP -> P_661 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_365_0 = _rtP -> P_662 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_365_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_367_0 = _rtDW
-> Memory_PreviousInput_l ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_369_0 =
( _rtB -> B_392_362_0 - B_392_363_0 ) * _rtP -> P_70 ; } else { _rtB ->
B_392_369_0 = _rtB -> B_392_367_0 ; } rtb_B_392_237_0 = _rtP -> P_664 *
muDoubleScalarAtan2 ( _rtB -> B_392_369_0 , _rtB -> B_392_361_0 ) * _rtP ->
P_665 ; B_392_374_0 . re = muDoubleScalarHypot ( _rtB -> B_392_361_0 , _rtB
-> B_392_369_0 ) * muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_374_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_361_0 , _rtB -> B_392_369_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; _rtB -> B_392_375_0 = _rtX ->
integrator_CSTATE_o ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_667 ; B_392_376_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_o . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_o . Last , _rtDW -> TransportDelay_IWORK_o .
Tail , _rtDW -> TransportDelay_IWORK_o . Head , _rtP -> P_668 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_378_0 = _rtP -> P_669 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_378_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_380_0 = _rtDW -> Memory_PreviousInput_j ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_382_0 = ( _rtB -> B_392_375_0 - B_392_376_0
) * _rtP -> P_73 ; } else { _rtB -> B_392_382_0 = _rtB -> B_392_380_0 ; }
_rtB -> B_392_383_0 = _rtX -> integrator_CSTATE_h ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_672 ; B_392_384_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ph . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ph . Last , _rtDW -> TransportDelay_IWORK_ph .
Tail , _rtDW -> TransportDelay_IWORK_ph . Head , _rtP -> P_673 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_386_0 = _rtP -> P_674 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_386_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_388_0 = _rtDW -> Memory_PreviousInput_b ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_390_0 = ( _rtB -> B_392_383_0 - B_392_384_0
) * _rtP -> P_72 ; } else { _rtB -> B_392_390_0 = _rtB -> B_392_388_0 ; }
rtb_B_392_237_0 = _rtP -> P_676 * muDoubleScalarAtan2 ( _rtB -> B_392_390_0 ,
_rtB -> B_392_382_0 ) * _rtP -> P_677 ; B_392_395_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_382_0 , _rtB -> B_392_390_0 ) *
muDoubleScalarCos ( rtb_B_392_237_0 ) ; B_392_395_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_382_0 , _rtB -> B_392_390_0 ) *
muDoubleScalarSin ( rtb_B_392_237_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_397_0 = _rtB -> B_392_396_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_397_0 ,
B_392_353_0 , B_392_374_0 , B_392_395_0 , & _rtB -> PosSeqComputation_d , &
_rtDW -> PosSeqComputation_d , & _rtP -> PosSeqComputation_d ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_400_0 = _rtB
-> B_392_399_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_400_0 , B_392_353_0 , B_392_374_0 , B_392_395_0 , & _rtB ->
NegSeqComputation_e , & _rtDW -> NegSeqComputation_e , & _rtP ->
NegSeqComputation_e ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_403_0 = _rtB -> B_392_402_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_403_0 ,
B_392_353_0 , B_392_374_0 , B_392_395_0 , & _rtB -> ZeroSeqComputation_p , &
_rtDW -> ZeroSeqComputation_p , & _rtP -> ZeroSeqComputation_p ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_d .
B_23_2_0 . im , _rtB -> PosSeqComputation_d . B_23_2_0 . re ) ; _rtB ->
B_392_406_0 = _rtP -> P_681 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_d . B_23_2_0 . re , _rtB -> PosSeqComputation_d . B_23_2_0
. im ) ; ssCallAccelRunBlock ( S , 392 , 408 , SS_CALL_MDL_OUTPUTS ) ; isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_16_0 =
ssGetTaskTime ( S , 1 ) ; _rtDW -> ref_MODE_f = ( rtb_B_392_16_0 >= _rtP ->
P_683 ) ; if ( _rtDW -> ref_MODE_f == 1 ) { rtb_B_392_410_0 = _rtP -> P_685 ;
} else { rtb_B_392_410_0 = _rtP -> P_684 ; } rtb_B_392_16_0 = ssGetTaskTime (
S , 1 ) ; _rtDW -> Time_init_ref_MODE_k = ( rtb_B_392_16_0 >= _rtP -> P_687 )
; if ( _rtDW -> Time_init_ref_MODE_k == 1 ) { rtb_B_392_237_0 = _rtP -> P_689
; } else { rtb_B_392_237_0 = _rtP -> P_688 ; } _rtB -> B_392_413_0 = _rtB ->
B_392_411_0 * rtb_B_392_237_0 ; if ( ( _rtDW -> LastMajorTimeA_l >=
ssGetTaskTime ( S , 1 ) ) && ( _rtDW -> LastMajorTimeB_g >= ssGetTaskTime ( S
, 1 ) ) ) { _rtB -> B_392_414_0 = _rtB -> B_392_413_0 ; } else { if ( ( (
_rtDW -> LastMajorTimeA_l < _rtDW -> LastMajorTimeB_g ) && ( _rtDW ->
LastMajorTimeB_g < ssGetTaskTime ( S , 1 ) ) ) || ( ( _rtDW ->
LastMajorTimeA_l >= _rtDW -> LastMajorTimeB_g ) && ( _rtDW ->
LastMajorTimeA_l >= ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime ( S
, 1 ) - _rtDW -> LastMajorTimeB_g ; rtb_B_392_4_1 = _rtDW -> PrevYB_c ; }
else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA_l ;
rtb_B_392_4_1 = _rtDW -> PrevYA_ep ; } tmp_1 = tmp_0 * _rtP -> P_690 ;
rtb_B_392_16_0 = _rtB -> B_392_413_0 - rtb_B_392_4_1 ; if ( rtb_B_392_16_0 >
tmp_1 ) { _rtB -> B_392_414_0 = rtb_B_392_4_1 + tmp_1 ; } else { tmp_0 *=
_rtP -> P_691 ; if ( rtb_B_392_16_0 < tmp_0 ) { _rtB -> B_392_414_0 =
rtb_B_392_4_1 + tmp_0 ; } else { _rtB -> B_392_414_0 = _rtB -> B_392_413_0 ;
} } } _rtB -> B_392_416_0 = rtb_B_392_237_0 * _rtB -> B_392_415_0 ; if ( (
_rtDW -> LastMajorTimeA_f >= ssGetTaskTime ( S , 1 ) ) && ( _rtDW ->
LastMajorTimeB_c >= ssGetTaskTime ( S , 1 ) ) ) { _rtB -> B_392_417_0 = _rtB
-> B_392_416_0 ; } else { if ( ( ( _rtDW -> LastMajorTimeA_f < _rtDW ->
LastMajorTimeB_c ) && ( _rtDW -> LastMajorTimeB_c < ssGetTaskTime ( S , 1 ) )
) || ( ( _rtDW -> LastMajorTimeA_f >= _rtDW -> LastMajorTimeB_c ) && ( _rtDW
-> LastMajorTimeA_f >= ssGetTaskTime ( S , 1 ) ) ) ) { tmp_0 = ssGetTaskTime
( S , 1 ) - _rtDW -> LastMajorTimeB_c ; rtb_B_392_4_1 = _rtDW -> PrevYB_e ; }
else { tmp_0 = ssGetTaskTime ( S , 1 ) - _rtDW -> LastMajorTimeA_f ;
rtb_B_392_4_1 = _rtDW -> PrevYA_f ; } tmp_1 = tmp_0 * _rtP -> P_693 ;
rtb_B_392_16_0 = _rtB -> B_392_416_0 - rtb_B_392_4_1 ; if ( rtb_B_392_16_0 >
tmp_1 ) { _rtB -> B_392_417_0 = rtb_B_392_4_1 + tmp_1 ; } else { tmp_0 *=
_rtP -> P_694 ; if ( rtb_B_392_16_0 < tmp_0 ) { _rtB -> B_392_417_0 =
rtb_B_392_4_1 + tmp_0 ; } else { _rtB -> B_392_417_0 = _rtB -> B_392_416_0 ;
} } } } _rtB -> B_392_418_0 = 0.0 ; _rtB -> B_392_418_0 += _rtP -> P_696 *
_rtX -> LPF1_CSTATE_e ; _rtB -> B_392_419_0 = 0.0 ; _rtB -> B_392_419_0 +=
_rtP -> P_698 * _rtX -> LPF2_CSTATE_m ; isHit = ssIsSampleHit ( S , 2 , 0 ) ;
if ( isHit != 0 ) { ssCallAccelRunBlock ( S , 54 , 0 , SS_CALL_MDL_OUTPUTS )
; } _rtB -> B_392_422_0 = ( _rtB -> B_392_418_0 - _rtB -> B_54_0_1 ) * _rtP
-> P_699 ; _rtB -> B_392_424_0 = ( _rtB -> B_392_419_0 - _rtB -> B_54_0_2 ) *
_rtP -> P_700 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
ssCallAccelRunBlock ( S , 37 , 0 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_392_427_0 = _rtP -> P_702 * _rtB -> B_37_0_1 ; } isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_429_0 = ( rtb_B_392_410_0 -
_rtB -> B_392_427_0 ) * _rtP -> P_703 ; } isHit = ssIsSampleHit ( S , 3 , 0 )
; if ( isHit != 0 ) { rtb_B_392_430_0 = _rtDW -> UnitDelay_DSTATE_o ; _rtB ->
B_392_431_0 = _rtP -> P_705 * _rtDW -> UnitDelay_DSTATE_o ; } _rtB ->
B_392_432_0 = _rtX -> integrator_CSTATE_ok ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_707 ; B_392_433_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m5
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_m5 . Last , _rtDW ->
TransportDelay_IWORK_m5 . Tail , _rtDW -> TransportDelay_IWORK_m5 . Head ,
_rtP -> P_708 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_435_0 = _rtP -> P_709 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_435_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_437_0 = _rtDW
-> Memory_PreviousInput_lv ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_439_0 =
( _rtB -> B_392_432_0 - B_392_433_0 ) * _rtP -> P_121 ; } else { _rtB ->
B_392_439_0 = _rtB -> B_392_437_0 ; } _rtB -> B_392_440_0 = _rtX ->
integrator_CSTATE_hr ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_712 ;
B_392_441_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_i . Last , _rtDW ->
TransportDelay_IWORK_i . Tail , _rtDW -> TransportDelay_IWORK_i . Head , _rtP
-> P_713 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_443_0 = _rtP -> P_714 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_443_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_445_0 = _rtDW
-> Memory_PreviousInput_ff ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_447_0 =
( _rtB -> B_392_440_0 - B_392_441_0 ) * _rtP -> P_120 ; } else { _rtB ->
B_392_447_0 = _rtB -> B_392_445_0 ; } rtb_B_392_410_0 = _rtP -> P_716 *
muDoubleScalarAtan2 ( _rtB -> B_392_447_0 , _rtB -> B_392_439_0 ) * _rtP ->
P_717 ; B_392_452_0 . re = muDoubleScalarHypot ( _rtB -> B_392_439_0 , _rtB
-> B_392_447_0 ) * muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_452_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_439_0 , _rtB -> B_392_447_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; _rtB -> B_392_453_0 = _rtX ->
integrator_CSTATE_e5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_p . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_719 ; B_392_454_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_b . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_b . Last , _rtDW -> TransportDelay_IWORK_b .
Tail , _rtDW -> TransportDelay_IWORK_b . Head , _rtP -> P_720 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_456_0 = _rtP -> P_721 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_456_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_458_0 = _rtDW -> Memory_PreviousInput_of ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_460_0 = ( _rtB -> B_392_453_0 - B_392_454_0
) * _rtP -> P_123 ; } else { _rtB -> B_392_460_0 = _rtB -> B_392_458_0 ; }
_rtB -> B_392_461_0 = _rtX -> integrator_CSTATE_j ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_724 ; B_392_462_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_an . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_an . Last , _rtDW -> TransportDelay_IWORK_an .
Tail , _rtDW -> TransportDelay_IWORK_an . Head , _rtP -> P_725 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_464_0 = _rtP -> P_726 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_464_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_466_0 = _rtDW -> Memory_PreviousInput_m ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_468_0 = ( _rtB -> B_392_461_0 - B_392_462_0
) * _rtP -> P_122 ; } else { _rtB -> B_392_468_0 = _rtB -> B_392_466_0 ; }
rtb_B_392_410_0 = _rtP -> P_728 * muDoubleScalarAtan2 ( _rtB -> B_392_468_0 ,
_rtB -> B_392_460_0 ) * _rtP -> P_729 ; B_392_473_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_460_0 , _rtB -> B_392_468_0 ) *
muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_473_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_460_0 , _rtB -> B_392_468_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; _rtB -> B_392_474_0 = _rtX ->
integrator_CSTATE_n1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oo . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oo . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_731 ;
B_392_475_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_e . Last , _rtDW ->
TransportDelay_IWORK_e . Tail , _rtDW -> TransportDelay_IWORK_e . Head , _rtP
-> P_732 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_477_0 = _rtP -> P_733 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_477_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_479_0 = _rtDW
-> Memory_PreviousInput_a3 ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_481_0 =
( _rtB -> B_392_474_0 - B_392_475_0 ) * _rtP -> P_125 ; } else { _rtB ->
B_392_481_0 = _rtB -> B_392_479_0 ; } _rtB -> B_392_482_0 = _rtX ->
integrator_CSTATE_fv ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_pk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_pk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_736 ;
B_392_483_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j . Last , _rtDW ->
TransportDelay_IWORK_j . Tail , _rtDW -> TransportDelay_IWORK_j . Head , _rtP
-> P_737 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_485_0 = _rtP -> P_738 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_485_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_487_0 = _rtDW
-> Memory_PreviousInput_ll ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_489_0 =
( _rtB -> B_392_482_0 - B_392_483_0 ) * _rtP -> P_124 ; } else { _rtB ->
B_392_489_0 = _rtB -> B_392_487_0 ; } rtb_B_392_410_0 = _rtP -> P_740 *
muDoubleScalarAtan2 ( _rtB -> B_392_489_0 , _rtB -> B_392_481_0 ) * _rtP ->
P_741 ; B_392_494_0 . re = muDoubleScalarHypot ( _rtB -> B_392_481_0 , _rtB
-> B_392_489_0 ) * muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_494_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_481_0 , _rtB -> B_392_489_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_496_0 = _rtB -> B_392_495_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_496_0 ,
B_392_452_0 , B_392_473_0 , B_392_494_0 , & _rtB -> PosSeqComputation_p , &
_rtDW -> PosSeqComputation_p , & _rtP -> PosSeqComputation_p ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_499_0 = _rtB
-> B_392_498_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_499_0 , B_392_452_0 , B_392_473_0 , B_392_494_0 , & _rtB ->
NegSeqComputation_m , & _rtDW -> NegSeqComputation_m , & _rtP ->
NegSeqComputation_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_502_0 = _rtB -> B_392_501_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_502_0 ,
B_392_452_0 , B_392_473_0 , B_392_494_0 , & _rtB -> ZeroSeqComputation_n , &
_rtDW -> ZeroSeqComputation_n , & _rtP -> ZeroSeqComputation_n ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_p .
B_23_2_0 . im , _rtB -> PosSeqComputation_p . B_23_2_0 . re ) ; _rtB ->
B_392_505_0 = _rtP -> P_745 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_p . B_23_2_0 . re , _rtB -> PosSeqComputation_p . B_23_2_0
. im ) ; _rtB -> B_392_506_0 = _rtX -> integrator_CSTATE_b ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_747 ; B_392_507_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lg . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_lg . Last , _rtDW -> TransportDelay_IWORK_lg .
Tail , _rtDW -> TransportDelay_IWORK_lg . Head , _rtP -> P_748 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_509_0 = _rtP -> P_749 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_509_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_511_0 = _rtDW -> Memory_PreviousInput_e0 ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_513_0 = ( _rtB -> B_392_506_0 - B_392_507_0
) * _rtP -> P_127 ; } else { _rtB -> B_392_513_0 = _rtB -> B_392_511_0 ; }
_rtB -> B_392_514_0 = _rtX -> integrator_CSTATE_kx ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_752 ; B_392_515_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ho . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ho . Last , _rtDW -> TransportDelay_IWORK_ho .
Tail , _rtDW -> TransportDelay_IWORK_ho . Head , _rtP -> P_753 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_517_0 = _rtP -> P_754 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_517_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_519_0 = _rtDW -> Memory_PreviousInput_h ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_521_0 = ( _rtB -> B_392_514_0 - B_392_515_0
) * _rtP -> P_126 ; } else { _rtB -> B_392_521_0 = _rtB -> B_392_519_0 ; }
rtb_B_392_410_0 = _rtP -> P_756 * muDoubleScalarAtan2 ( _rtB -> B_392_521_0 ,
_rtB -> B_392_513_0 ) * _rtP -> P_757 ; B_392_526_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_513_0 , _rtB -> B_392_521_0 ) *
muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_526_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_513_0 , _rtB -> B_392_521_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; _rtB -> B_392_527_0 = _rtX ->
integrator_CSTATE_m ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_759 ; B_392_528_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ay . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ay . Last , _rtDW -> TransportDelay_IWORK_ay .
Tail , _rtDW -> TransportDelay_IWORK_ay . Head , _rtP -> P_760 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_530_0 = _rtP -> P_761 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_530_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_532_0 = _rtDW -> Memory_PreviousInput_i ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_534_0 = ( _rtB -> B_392_527_0 - B_392_528_0
) * _rtP -> P_129 ; } else { _rtB -> B_392_534_0 = _rtB -> B_392_532_0 ; }
_rtB -> B_392_535_0 = _rtX -> integrator_CSTATE_c ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_764 ; B_392_536_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_g . Last , _rtDW -> TransportDelay_IWORK_g .
Tail , _rtDW -> TransportDelay_IWORK_g . Head , _rtP -> P_765 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_538_0 = _rtP -> P_766 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_538_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_540_0 = _rtDW -> Memory_PreviousInput_k ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_542_0 = ( _rtB -> B_392_535_0 - B_392_536_0
) * _rtP -> P_128 ; } else { _rtB -> B_392_542_0 = _rtB -> B_392_540_0 ; }
rtb_B_392_410_0 = _rtP -> P_768 * muDoubleScalarAtan2 ( _rtB -> B_392_542_0 ,
_rtB -> B_392_534_0 ) * _rtP -> P_769 ; B_392_547_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_534_0 , _rtB -> B_392_542_0 ) *
muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_547_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_534_0 , _rtB -> B_392_542_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; _rtB -> B_392_548_0 = _rtX ->
integrator_CSTATE_cw ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b3 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b3 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_771 ;
B_392_549_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_mq . Last , _rtDW ->
TransportDelay_IWORK_mq . Tail , _rtDW -> TransportDelay_IWORK_mq . Head ,
_rtP -> P_772 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_551_0 = _rtP -> P_773 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_551_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_553_0 = _rtDW
-> Memory_PreviousInput_im ; } if ( rtb_B_392_263_0 ) { _rtB -> B_392_555_0 =
( _rtB -> B_392_548_0 - B_392_549_0 ) * _rtP -> P_131 ; } else { _rtB ->
B_392_555_0 = _rtB -> B_392_553_0 ; } _rtB -> B_392_556_0 = _rtX ->
integrator_CSTATE_d ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_776 ; B_392_557_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_od . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_od . Last , _rtDW -> TransportDelay_IWORK_od .
Tail , _rtDW -> TransportDelay_IWORK_od . Head , _rtP -> P_777 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_559_0 = _rtP -> P_778 ; } rtb_B_392_263_0 = ( ssGetT ( S )
>= _rtB -> B_392_559_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_561_0 = _rtDW -> Memory_PreviousInput_jc ; } if (
rtb_B_392_263_0 ) { _rtB -> B_392_563_0 = ( _rtB -> B_392_556_0 - B_392_557_0
) * _rtP -> P_130 ; } else { _rtB -> B_392_563_0 = _rtB -> B_392_561_0 ; }
rtb_B_392_410_0 = _rtP -> P_780 * muDoubleScalarAtan2 ( _rtB -> B_392_563_0 ,
_rtB -> B_392_555_0 ) * _rtP -> P_781 ; B_392_568_0 . re =
muDoubleScalarHypot ( _rtB -> B_392_555_0 , _rtB -> B_392_563_0 ) *
muDoubleScalarCos ( rtb_B_392_410_0 ) ; B_392_568_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_555_0 , _rtB -> B_392_563_0 ) *
muDoubleScalarSin ( rtb_B_392_410_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_570_0 = _rtB -> B_392_569_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_570_0 ,
B_392_526_0 , B_392_547_0 , B_392_568_0 , & _rtB -> PosSeqComputation_a , &
_rtDW -> PosSeqComputation_a , & _rtP -> PosSeqComputation_a ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_573_0 = _rtB
-> B_392_572_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_573_0 , B_392_526_0 , B_392_547_0 , B_392_568_0 , & _rtB ->
NegSeqComputation_j , & _rtDW -> NegSeqComputation_j , & _rtP ->
NegSeqComputation_j ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_576_0 = _rtB -> B_392_575_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_576_0 ,
B_392_526_0 , B_392_547_0 , B_392_568_0 , & _rtB -> ZeroSeqComputation_o , &
_rtDW -> ZeroSeqComputation_o , & _rtP -> ZeroSeqComputation_o ) ;
rtb_B_392_4_1 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_a . B_23_2_0
. im , _rtB -> PosSeqComputation_a . B_23_2_0 . re ) ; _rtB -> B_392_579_0 =
_rtP -> P_785 * muDoubleScalarHypot ( _rtB -> PosSeqComputation_a . B_23_2_0
. re , _rtB -> PosSeqComputation_a . B_23_2_0 . im ) ; ssCallAccelRunBlock (
S , 392 , 581 , SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 392 , 582 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 392 , 583 ,
SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_392_584_0 = _rtP -> P_787 * _rtB ->
B_392_332_0 ; _rtB -> B_392_585_0 = _rtP -> P_788 * _rtB -> B_392_505_0 ;
ssCallAccelRunBlock ( S , 392 , 586 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock ( S ,
392 , 587 , SS_CALL_MDL_OUTPUTS ) ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_588_0 = _rtP -> P_789 * _rtB -> B_392_258_0
; _rtB -> B_392_589_0 = _rtP -> P_790 * _rtB -> B_392_431_0 ;
ssCallAccelRunBlock ( S , 392 , 590 , SS_CALL_MDL_OUTPUTS ) ; } _rtB ->
B_392_591_0 [ 0 ] = _rtB -> B_392_245_0 ; _rtB -> B_392_591_0 [ 1 ] = _rtB ->
B_392_258_0 ; _rtB -> B_392_591_0 [ 2 ] = _rtB -> B_392_246_0 ; _rtB ->
B_392_591_0 [ 3 ] = _rtB -> B_392_332_0 ; _rtB -> B_392_591_0 [ 4 ] = _rtB ->
B_392_406_0 ; _rtB -> B_392_591_0 [ 5 ] = _rtB -> B_392_256_0 ; _rtB ->
B_392_591_0 [ 6 ] = _rtB -> B_16_0_1 ; _rtB -> B_392_591_0 [ 7 ] = _rtB ->
B_16_0_2 ; _rtB -> B_392_591_0 [ 8 ] = 0.0 ; _rtB -> B_392_591_0 [ 9 ] = 0.0
; _rtB -> B_392_591_0 [ 10 ] = 0.0 ; _rtB -> B_392_591_0 [ 11 ] = 0.0 ;
ssCallAccelRunBlock ( S , 392 , 592 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 392 , 593 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 392 , 594 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_599_0 [ 0 ] =
_rtP -> P_792 * _rtB -> B_0_14_0 [ 41 ] * _rtP -> P_795 ; _rtB -> B_392_599_0
[ 1 ] = _rtP -> P_793 * _rtB -> B_0_14_0 [ 42 ] * _rtP -> P_795 ; _rtB ->
B_392_599_0 [ 2 ] = _rtP -> P_794 * _rtB -> B_0_14_0 [ 43 ] * _rtP -> P_795 ;
for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] = _rtP -> P_796 [
isHit + 6 ] * _rtB -> B_392_599_0 [ 2 ] + ( _rtP -> P_796 [ isHit + 3 ] *
_rtB -> B_392_599_0 [ 1 ] + _rtP -> P_796 [ isHit ] * _rtB -> B_392_599_0 [ 0
] ) ; } _rtB -> B_392_601_0 [ 0 ] = _rtP -> P_797 * tmp [ 0 ] ; _rtB ->
B_392_601_0 [ 1 ] = _rtP -> P_797 * tmp [ 1 ] ; _rtB -> B_392_601_0 [ 2 ] =
_rtP -> P_797 * tmp [ 2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_605_0 = _rtB -> B_392_604_0 ; }
MR_CIGRE_GENERALIZADO_Subsystem1 ( S , _rtB -> B_392_605_0 , & _rtB ->
B_392_601_0 [ 0 ] , _rtB -> B_392_19_0 , & _rtB -> Subsystem1_b , & _rtDW ->
Subsystem1_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_392_609_0 = _rtB -> B_392_608_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_609_0 , & _rtB ->
B_392_601_0 [ 0 ] , _rtB -> B_392_19_0 , & _rtB -> Subsystempi2delay_ls , &
_rtDW -> Subsystempi2delay_ls ) ; if ( _rtB -> B_392_604_0 != 0 ) { _rtB ->
B_392_611_0 [ 0 ] = _rtB -> Subsystem1_b . B_3_0_0 ; _rtB -> B_392_611_0 [ 1
] = _rtB -> Subsystem1_b . B_3_1_0 ; } else { _rtB -> B_392_611_0 [ 0 ] =
_rtB -> Subsystempi2delay_ls . B_2_0_0 ; _rtB -> B_392_611_0 [ 1 ] = _rtB ->
Subsystempi2delay_ls . B_2_1_0 ; } _rtB -> B_392_612_0 = 0.0 ; _rtB ->
B_392_612_0 += _rtP -> P_802 * _rtX -> Integradord_CSTATE ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_803 *
_rtB -> B_0_14_0 [ 93 ] * _rtP -> P_806 ; tmp_1 = _rtP -> P_804 * _rtB ->
B_0_14_0 [ 94 ] * _rtP -> P_806 ; rtb_B_392_16_0 = _rtP -> P_805 * _rtB ->
B_0_14_0 [ 95 ] * _rtP -> P_806 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_807 [ isHit + 6 ] * rtb_B_392_16_0 + ( _rtP ->
P_807 [ isHit + 3 ] * tmp_1 + _rtP -> P_807 [ isHit ] * tmp_0 ) ; } _rtB ->
B_392_618_0 [ 0 ] = _rtP -> P_808 * tmp [ 0 ] ; _rtB -> B_392_618_0 [ 1 ] =
_rtP -> P_808 * tmp [ 1 ] ; _rtB -> B_392_618_0 [ 2 ] = _rtP -> P_808 * tmp [
2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_622_0 = _rtB -> B_392_621_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1 ( S ,
_rtB -> B_392_622_0 , & _rtB -> B_392_618_0 [ 0 ] , _rtB -> B_392_19_0 , &
_rtB -> Subsystem1_d , & _rtDW -> Subsystem1_d ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_626_0 = _rtB -> B_392_625_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_626_0 , & _rtB ->
B_392_618_0 [ 0 ] , _rtB -> B_392_19_0 , & _rtB -> Subsystempi2delay_lc , &
_rtDW -> Subsystempi2delay_lc ) ; if ( _rtB -> B_392_621_0 != 0 ) { _rtB ->
B_392_628_0 [ 0 ] = _rtB -> Subsystem1_d . B_3_0_0 ; _rtB -> B_392_628_0 [ 1
] = _rtB -> Subsystem1_d . B_3_1_0 ; } else { _rtB -> B_392_628_0 [ 0 ] =
_rtB -> Subsystempi2delay_lc . B_2_0_0 ; _rtB -> B_392_628_0 [ 1 ] = _rtB ->
Subsystempi2delay_lc . B_2_1_0 ; } _rtB -> B_392_629_0 = 0.0 ; _rtB ->
B_392_629_0 += _rtP -> P_813 * _rtX -> LPF4_CSTATE ; _rtB -> B_392_630_0 =
_rtB -> B_392_629_0 - _rtB -> B_392_628_0 [ 0 ] ; _rtB -> B_392_631_0 = _rtP
-> P_814 * _rtB -> B_392_630_0 ; _rtB -> B_392_632_0 = _rtB -> B_392_612_0 +
_rtB -> B_392_631_0 ; _rtB -> B_392_633_0 = _rtB -> B_392_258_0 * _rtB ->
B_392_628_0 [ 1 ] ; _rtB -> B_392_634_0 = _rtP -> P_815 * _rtB -> B_392_633_0
; _rtB -> B_392_635_0 = ( _rtB -> B_392_611_0 [ 0 ] + _rtB -> B_392_632_0 ) -
_rtB -> B_392_634_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_392_637_0 = _rtDW -> UnitDelay1_DSTATE_pe / _rtB -> B_392_595_0 ;
} _rtB -> B_392_638_0 = _rtB -> B_392_635_0 / _rtB -> B_392_637_0 ; _rtB ->
B_392_639_0 = _rtB -> B_392_628_0 [ 0 ] * _rtB -> B_392_258_0 ; _rtB ->
B_392_640_0 = _rtP -> P_817 * _rtB -> B_392_639_0 ; _rtB -> B_392_641_0 = 0.0
; _rtB -> B_392_641_0 += _rtP -> P_819 * _rtX -> LPF2_CSTATE_p ; _rtB ->
B_392_642_0 = _rtB -> B_392_641_0 - _rtB -> B_392_628_0 [ 1 ] ; _rtB ->
B_392_643_0 = _rtP -> P_820 * _rtB -> B_392_642_0 ; _rtB -> B_392_644_0 = 0.0
; _rtB -> B_392_644_0 += _rtP -> P_822 * _rtX -> Integradord1_CSTATE ; _rtB
-> B_392_645_0 = _rtB -> B_392_643_0 + _rtB -> B_392_644_0 ; _rtB ->
B_392_646_0 = ( _rtB -> B_392_640_0 + _rtB -> B_392_645_0 ) + _rtB ->
B_392_611_0 [ 1 ] ; _rtB -> B_392_647_0 = 1.0 / _rtB -> B_392_637_0 * _rtB ->
B_392_646_0 ; _rtB -> B_392_648_0 = _rtB -> B_392_1_0 - _rtB -> B_392_7_0 ;
_rtB -> B_392_649_0 = _rtP -> P_823 * _rtB -> B_392_648_0 ; _rtB ->
B_392_650_0 = _rtB -> B_392_649_0 * _rtB -> B_392_637_0 ; _rtB -> B_392_651_0
= _rtB -> B_392_2_0 - _rtB -> B_392_7_1 ; _rtB -> B_392_652_0 = _rtP -> P_824
* _rtB -> B_392_651_0 ; _rtB -> B_392_653_0 = _rtB -> B_392_637_0 * _rtB ->
B_392_652_0 ; _rtB -> B_392_654_0 = _rtB -> B_392_630_0 - _rtB -> B_392_650_0
; _rtB -> B_392_655_0 = _rtB -> B_392_642_0 - _rtB -> B_392_653_0 ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_656_0 = _rtP
-> P_825 * _rtB -> B_1_0_2 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { rtb_B_392_16_0 = ssGetTaskTime ( S , 1 ) ; _rtDW -> ref2_MODE
= ( rtb_B_392_16_0 >= _rtP -> P_826 ) ; if ( _rtDW -> ref2_MODE == 1 ) {
rtb_B_392_16_0 = _rtP -> P_828 ; } else { rtb_B_392_16_0 = _rtP -> P_827 ; }
_rtB -> B_392_660_0 = ( rtb_B_392_16_0 - _rtB -> B_392_656_0 ) * _rtP ->
P_829 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_664_0 [ 0 ] = _rtP -> P_830 * _rtB -> B_0_14_0 [ 96 ] * _rtP -> P_833 ;
_rtB -> B_392_664_0 [ 1 ] = _rtP -> P_831 * _rtB -> B_0_14_0 [ 97 ] * _rtP ->
P_833 ; _rtB -> B_392_664_0 [ 2 ] = _rtP -> P_832 * _rtB -> B_0_14_0 [ 98 ] *
_rtP -> P_833 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_834 [ isHit + 6 ] * _rtB -> B_392_664_0 [ 2 ] + ( _rtP -> P_834 [
isHit + 3 ] * _rtB -> B_392_664_0 [ 1 ] + _rtP -> P_834 [ isHit ] * _rtB ->
B_392_664_0 [ 0 ] ) ; } _rtB -> B_392_666_0 [ 0 ] = _rtP -> P_835 * tmp [ 0 ]
; _rtB -> B_392_666_0 [ 1 ] = _rtP -> P_835 * tmp [ 1 ] ; _rtB -> B_392_666_0
[ 2 ] = _rtP -> P_835 * tmp [ 2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_670_0 = _rtB -> B_392_669_0 ; }
MR_CIGRE_GENERALIZADO_Subsystem1 ( S , _rtB -> B_392_670_0 , & _rtB ->
B_392_666_0 [ 0 ] , _rtB -> B_392_19_0 , & _rtB -> Subsystem1_lc , & _rtDW ->
Subsystem1_lc ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtB -> B_392_674_0 = _rtB -> B_392_673_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_674_0 , & _rtB ->
B_392_666_0 [ 0 ] , _rtB -> B_392_19_0 , & _rtB -> Subsystempi2delay_e , &
_rtDW -> Subsystempi2delay_e ) ; if ( _rtB -> B_392_669_0 != 0 ) { _rtB ->
B_392_676_0 [ 0 ] = _rtB -> Subsystem1_lc . B_3_0_0 ; _rtB -> B_392_676_0 [ 1
] = _rtB -> Subsystem1_lc . B_3_1_0 ; } else { _rtB -> B_392_676_0 [ 0 ] =
_rtB -> Subsystempi2delay_e . B_2_0_0 ; _rtB -> B_392_676_0 [ 1 ] = _rtB ->
Subsystempi2delay_e . B_2_1_0 ; } _rtB -> B_392_677_0 = _rtP -> P_839 * _rtB
-> B_392_676_0 [ 0 ] ; _rtB -> B_392_678_0 = 0.0 ; _rtB -> B_392_678_0 +=
_rtP -> P_841 * _rtX -> Integradord_CSTATE_l ; _rtB -> B_392_679_0 = _rtP ->
P_842 * _rtB -> B_392_676_0 [ 0 ] ; _rtB -> B_392_680_0 = 0.0 ; _rtB ->
B_392_680_0 += _rtP -> P_844 * _rtX -> HPF1_CSTATE ; _rtB -> B_392_680_0 +=
_rtP -> P_845 * _rtB -> B_392_679_0 ; _rtB -> B_392_681_0 = _rtP -> P_846 *
_rtB -> B_392_676_0 [ 0 ] ; _rtB -> B_392_682_0 = _rtB -> B_392_680_0 + _rtB
-> B_392_681_0 ; _rtB -> B_392_683_0 = _rtP -> P_847 * _rtB -> B_392_676_0 [
1 ] ; _rtB -> B_392_684_0 = _rtB -> B_392_683_0 * _rtB -> B_392_258_0 ; _rtB
-> B_392_685_0 = _rtB -> B_392_682_0 + _rtB -> B_392_684_0 ; _rtB ->
B_392_686_0 = _rtB -> B_392_660_0 - _rtB -> B_392_685_0 ; _rtB -> B_392_687_0
= _rtB -> B_392_686_0 - _rtB -> B_392_611_0 [ 0 ] ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_17_0 > _rtP -> P_850 ) {
_rtB -> B_392_690_0 = _rtB -> B_392_688_0 ; } else { _rtB -> B_392_690_0 =
_rtB -> B_392_689_0 ; } } _rtB -> B_392_691_0 = _rtB -> B_392_687_0 * _rtB ->
B_392_690_0 ; _rtB -> B_392_692_0 = _rtP -> P_851 * _rtB -> B_392_691_0 ;
_rtB -> B_392_693_0 = _rtB -> B_392_678_0 + _rtB -> B_392_692_0 ; _rtB ->
B_392_694_0 = _rtB -> B_392_258_0 * _rtB -> B_392_611_0 [ 1 ] ; _rtB ->
B_392_695_0 = _rtP -> P_852 * _rtB -> B_392_694_0 ; _rtB -> B_392_696_0 = (
_rtB -> B_392_677_0 + _rtB -> B_392_693_0 ) - _rtB -> B_392_695_0 ; _rtB ->
B_392_697_0 = _rtB -> B_392_696_0 * _rtB -> B_392_690_0 ; _rtB -> B_392_698_0
= _rtB -> B_392_611_0 [ 0 ] * _rtB -> B_392_258_0 ; _rtB -> B_392_699_0 =
_rtP -> P_853 * _rtB -> B_392_698_0 ; _rtB -> B_392_701_0 = _rtP -> P_855 *
_rtB -> B_392_676_0 [ 0 ] ; _rtB -> B_392_702_0 = _rtB -> B_392_258_0 * _rtB
-> B_392_701_0 ; _rtB -> B_392_703_0 = _rtP -> P_856 * _rtB -> B_392_676_0 [
1 ] ; _rtB -> B_392_704_0 = 0.0 ; _rtB -> B_392_704_0 += _rtP -> P_858 * _rtX
-> HPF2_CSTATE ; _rtB -> B_392_704_0 += _rtP -> P_859 * _rtB -> B_392_703_0 ;
_rtB -> B_392_705_0 = _rtP -> P_860 * _rtB -> B_392_676_0 [ 1 ] ; _rtB ->
B_392_706_0 = _rtB -> B_392_704_0 + _rtB -> B_392_705_0 ; _rtB -> B_392_707_0
= _rtB -> B_392_702_0 + _rtB -> B_392_706_0 ; _rtB -> B_392_708_0 = _rtB ->
B_392_700_0 - _rtB -> B_392_707_0 ; _rtB -> B_392_709_0 = _rtB -> B_392_708_0
- _rtB -> B_392_611_0 [ 1 ] ; _rtB -> B_392_710_0 = _rtB -> B_392_690_0 *
_rtB -> B_392_709_0 ; _rtB -> B_392_711_0 = _rtP -> P_861 * _rtB ->
B_392_710_0 ; _rtB -> B_392_712_0 = 0.0 ; _rtB -> B_392_712_0 += _rtP ->
P_863 * _rtX -> Integradorq_CSTATE ; _rtB -> B_392_713_0 = _rtB ->
B_392_711_0 + _rtB -> B_392_712_0 ; _rtB -> B_392_714_0 = _rtP -> P_864 *
_rtB -> B_392_676_0 [ 1 ] ; _rtB -> B_392_715_0 = ( _rtB -> B_392_699_0 +
_rtB -> B_392_713_0 ) + _rtB -> B_392_714_0 ; _rtB -> B_392_716_0 = _rtB ->
B_392_690_0 * _rtB -> B_392_715_0 ; _rtB -> B_392_717_0 . re = _rtB ->
B_392_697_0 ; _rtB -> B_392_717_0 . im = _rtB -> B_392_716_0 ; _rtB ->
B_392_718_0 = muDoubleScalarHypot ( _rtB -> B_392_717_0 . re , _rtB ->
B_392_717_0 . im ) ; rtb_B_392_410_0 = muDoubleScalarAtan2 ( _rtB ->
B_392_717_0 . im , _rtB -> B_392_717_0 . re ) ; if ( ssIsMajorTimeStep ( S )
!= 0 ) { _rtDW -> Saturation_MODE_k = _rtB -> B_392_718_0 >= _rtP -> P_865 ?
1 : _rtB -> B_392_718_0 > _rtP -> P_866 ? 0 : - 1 ; } _rtB -> B_392_719_0 =
_rtDW -> Saturation_MODE_k == 1 ? _rtP -> P_865 : _rtDW -> Saturation_MODE_k
== - 1 ? _rtP -> P_866 : _rtB -> B_392_718_0 ; _rtB -> B_392_721_0 = _rtB ->
B_392_719_0 * muDoubleScalarCos ( rtb_B_392_410_0 ) ; _rtB -> B_392_721_1 =
_rtB -> B_392_719_0 * muDoubleScalarSin ( rtb_B_392_410_0 ) ;
ssCallAccelRunBlock ( S , 392 , 722 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 392 , 723 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_392_724_0 = _rtB -> B_392_697_0 - _rtB -> B_392_721_0 ; _rtB -> B_392_725_0
= _rtP -> P_867 * _rtB -> B_392_724_0 ; _rtB -> B_392_726_0 = _rtB ->
B_392_691_0 - _rtB -> B_392_725_0 ; _rtB -> B_392_727_0 = _rtP -> P_868 *
_rtB -> B_392_726_0 ; _rtB -> B_392_728_0 = _rtB -> B_392_716_0 - _rtB ->
B_392_721_1 ; _rtB -> B_392_729_0 = _rtP -> P_869 * _rtB -> B_392_728_0 ;
_rtB -> B_392_730_0 = _rtB -> B_392_710_0 - _rtB -> B_392_729_0 ; _rtB ->
B_392_731_0 = _rtP -> P_870 * _rtB -> B_392_730_0 ; _rtB -> B_392_732_0 =
_rtB -> B_16_0_2 * _rtB -> B_392_611_0 [ 0 ] ; _rtB -> B_392_733_0 = _rtB ->
B_16_0_1 * _rtB -> B_392_611_0 [ 1 ] ; _rtB -> B_392_734_0 = _rtB ->
B_392_733_0 - _rtB -> B_392_732_0 ; _rtB -> B_392_735_0 = _rtB -> B_392_611_0
[ 0 ] * _rtB -> B_392_611_0 [ 0 ] ; _rtB -> B_392_736_0 = _rtB -> B_392_611_0
[ 1 ] * _rtB -> B_392_611_0 [ 1 ] ; _rtB -> B_392_737_0 = _rtB -> B_392_735_0
- _rtB -> B_392_736_0 ; _rtB -> B_392_738_0 = _rtB -> B_392_734_0 / _rtB ->
B_392_737_0 ; _rtB -> B_392_739_0 = _rtP -> P_871 * _rtB -> B_392_738_0 ;
ssCallAccelRunBlock ( S , 392 , 740 , SS_CALL_MDL_OUTPUTS ) ; _rtB ->
B_392_741_0 = _rtB -> B_16_0_1 * _rtB -> B_392_611_0 [ 0 ] ; _rtB ->
B_392_742_0 = _rtB -> B_16_0_2 * _rtB -> B_392_611_0 [ 1 ] ; _rtB ->
B_392_743_0 = _rtB -> B_392_741_0 - _rtB -> B_392_742_0 ; _rtB -> B_392_744_0
= _rtB -> B_392_736_0 + _rtB -> B_392_735_0 ; _rtB -> B_392_745_0 = _rtB ->
B_392_743_0 / _rtB -> B_392_744_0 ; _rtB -> B_392_746_0 = _rtP -> P_872 *
_rtB -> B_392_745_0 ; ssCallAccelRunBlock ( S , 392 , 747 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { if ( _rtB -> B_392_17_0 > _rtP -> P_873 ) { _rtB -> B_392_748_0 = _rtB ->
B_392_258_0 ; } else { _rtB -> B_392_748_0 = _rtB -> B_392_256_0 ; } } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_392_752_0 [ 0 ] = _rtP
-> P_874 * _rtB -> B_0_14_0 [ 38 ] * _rtP -> P_877 ; B_392_752_0 [ 1 ] = _rtP
-> P_875 * _rtB -> B_0_14_0 [ 39 ] * _rtP -> P_877 ; B_392_752_0 [ 2 ] = _rtP
-> P_876 * _rtB -> B_0_14_0 [ 40 ] * _rtP -> P_877 ; rtb_B_392_790_0 = _rtP
-> P_879 * _rtB -> B_392_33_0 - _rtDW -> prediction_DSTATE ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_794_0 =
rtb_B_392_790_0 + _rtB -> B_392_787_0 ; } _rtB -> B_392_795_0 = ( _rtB ->
B_392_36_0 * _rtB -> B_0_14_0 [ 148 ] - _rtB -> B_392_37_0 * _rtB -> B_0_14_0
[ 149 ] ) / _rtB -> B_392_794_0 ; _rtB -> B_392_810_0 = _rtB -> B_392_611_0 [
0 ] * _rtB -> B_392_628_0 [ 0 ] ; _rtB -> B_392_811_0 = _rtB -> B_392_611_0 [
1 ] * _rtB -> B_392_628_0 [ 1 ] ; _rtB -> B_392_812_0 = _rtB -> B_392_810_0 +
_rtB -> B_392_811_0 ; _rtB -> B_392_813_0 = _rtP -> P_881 * _rtB ->
B_392_812_0 ; _rtB -> B_392_814_0 = _rtB -> B_392_611_0 [ 0 ] * _rtB ->
B_392_628_0 [ 1 ] ; _rtB -> B_392_815_0 = _rtB -> B_392_628_0 [ 0 ] * _rtB ->
B_392_611_0 [ 1 ] ; _rtB -> B_392_816_0 = _rtB -> B_392_815_0 - _rtB ->
B_392_814_0 ; _rtB -> B_392_817_0 = _rtP -> P_882 * _rtB -> B_392_816_0 ;
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { ssCallAccelRunBlock
( S , 392 , 818 , SS_CALL_MDL_OUTPUTS ) ; _rtB -> B_392_820_0 = _rtP -> P_883
; isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( _rtB -> B_392_820_0 > 0.0 ) { if ( !
_rtDW -> AutomaticGainControl_MODE_l ) { if ( ssGetTaskTime ( S , 3 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
_rtDW -> Integ4_SYSTEM_ENABLE_a = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_ll = 1U
; _rtDW -> AutomaticGainControl_MODE_l = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE_l ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> AutomaticGainControl_MODE_l = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_l ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_21 [ isHit + 6 ] * _rtB -> B_392_599_0 [ 2 ] + ( _rtP -> P_21 [
isHit + 3 ] * _rtB -> B_392_599_0 [ 1 ] + _rtP -> P_21 [ isHit ] * _rtB ->
B_392_599_0 [ 0 ] ) ; } B_12_1_0 [ 0 ] = _rtP -> P_22 * tmp [ 0 ] ; B_12_1_0
[ 1 ] = _rtP -> P_22 * tmp [ 1 ] ; B_12_1_0 [ 2 ] = _rtP -> P_22 * tmp [ 2 ]
; _rtB -> B_12_5_0 = _rtB -> B_12_4_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_b (
_rtB -> B_12_5_0 , & B_12_1_0 [ 0 ] , _rtB -> B_392_12_0 , & _rtB ->
Subsystem1_b2 , & _rtDW -> Subsystem1_b2 ) ; _rtB -> B_12_9_0 = _rtB ->
B_12_8_0 ; MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_12_9_0 , &
B_12_1_0 [ 0 ] , _rtB -> B_392_12_0 , & _rtB -> Subsystempi2delay_m5 , &
_rtDW -> Subsystempi2delay_m5 ) ; if ( _rtB -> B_12_4_0 != 0 ) { _rtB ->
B_12_11_0 [ 0 ] = _rtB -> Subsystem1_b2 . B_11_0_0 ; _rtB -> B_12_11_0 [ 1 ]
= _rtB -> Subsystem1_b2 . B_11_1_0 ; } else { _rtB -> B_12_11_0 [ 0 ] = _rtB
-> Subsystempi2delay_m5 . B_10_0_0 ; _rtB -> B_12_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_m5 . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_a != 0
) { _rtB -> B_12_12_0 = _rtDW -> Integ4_DSTATE_e ; } else { _rtB -> B_12_12_0
= _rtP -> P_26 * _rtB -> B_12_11_0 [ 0 ] + _rtDW -> Integ4_DSTATE_e ; } if (
rtb_B_392_257_0 > _rtP -> P_28 ) { rtb_B_392_790_0 = _rtP -> P_28 ; } else if
( rtb_B_392_257_0 < _rtP -> P_29 ) { rtb_B_392_790_0 = _rtP -> P_29 ; } else
{ rtb_B_392_790_0 = rtb_B_392_257_0 ; } rtb_B_392_790_0 = 1.0 /
rtb_B_392_790_0 / 5.0e-5 ; rtb_B_392_410_0 = muDoubleScalarCeil (
rtb_B_392_790_0 ) ; _rtB -> B_12_16_0 = _rtP -> P_30 * rtb_B_392_410_0 ;
ssCallAccelRunBlock ( S , 12 , 17 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_12_20_0 ) { rtb_B_392_410_0 =
rtb_B_392_790_0 - rtb_B_392_410_0 ; _rtB -> B_12_24_0 = ( ( _rtB -> B_12_11_0
[ 0 ] - _rtDW -> UnitDelay_DSTATE_n ) * rtb_B_392_410_0 * _rtP -> P_18 + _rtB
-> B_12_11_0 [ 0 ] ) * ( rtb_B_392_410_0 / rtb_B_392_790_0 ) + ( _rtB ->
B_12_12_0 - _rtB -> B_12_17_0 ) * rtb_B_392_257_0 ; } else { _rtB ->
B_12_24_0 = _rtDW -> UnitDelay1_DSTATE_fp ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_ll != 0 ) { _rtB -> B_12_25_0 = _rtDW -> Integ4_DSTATE_c
; } else { _rtB -> B_12_25_0 = _rtP -> P_42 * _rtB -> B_12_11_0 [ 1 ] + _rtDW
-> Integ4_DSTATE_c ; } if ( rtb_B_392_257_0 > _rtP -> P_44 ) {
rtb_B_392_790_0 = _rtP -> P_44 ; } else if ( rtb_B_392_257_0 < _rtP -> P_45 )
{ rtb_B_392_790_0 = _rtP -> P_45 ; } else { rtb_B_392_790_0 = rtb_B_392_257_0
; } rtb_B_392_790_0 = 1.0 / rtb_B_392_790_0 / 5.0e-5 ; rtb_B_392_410_0 =
muDoubleScalarCeil ( rtb_B_392_790_0 ) ; _rtB -> B_12_29_0 = _rtP -> P_46 *
rtb_B_392_410_0 ; ssCallAccelRunBlock ( S , 12 , 30 , SS_CALL_MDL_OUTPUTS ) ;
if ( ssGetTaskTime ( S , 3 ) >= _rtB -> B_12_33_0 ) { rtb_B_392_410_0 =
rtb_B_392_790_0 - rtb_B_392_410_0 ; _rtB -> B_12_37_0 = ( ( _rtB -> B_12_11_0
[ 1 ] - _rtDW -> UnitDelay_DSTATE_k ) * rtb_B_392_410_0 * _rtP -> P_19 + _rtB
-> B_12_11_0 [ 1 ] ) * ( rtb_B_392_410_0 / rtb_B_392_790_0 ) + ( _rtB ->
B_12_25_0 - _rtB -> B_12_30_0 ) * rtb_B_392_257_0 ; } else { _rtB ->
B_12_37_0 = _rtDW -> UnitDelay1_DSTATE_b ; } rtb_B_392_790_0 =
muDoubleScalarHypot ( _rtB -> B_12_24_0 , _rtB -> B_12_37_0 ) ; if (
rtb_B_392_790_0 > _rtP -> P_58 ) { rtb_B_392_790_0 = _rtP -> P_58 ; } else {
if ( rtb_B_392_790_0 < _rtP -> P_59 ) { rtb_B_392_790_0 = _rtP -> P_59 ; } }
_rtB -> B_12_41_0 = 1.0 / rtb_B_392_790_0 ; B_12_42_0 = _rtP -> P_60 *
muDoubleScalarAtan2 ( _rtB -> B_12_37_0 , _rtB -> B_12_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_c ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_884 [ isHit + 6 ] * _rtB ->
B_392_599_0 [ 2 ] + ( _rtP -> P_884 [ isHit + 3 ] * _rtB -> B_392_599_0 [ 1 ]
+ _rtP -> P_884 [ isHit ] * _rtB -> B_392_599_0 [ 0 ] ) ; } B_392_823_0 [ 0 ]
= _rtP -> P_885 * tmp [ 0 ] ; B_392_823_0 [ 1 ] = _rtP -> P_885 * tmp [ 1 ] ;
B_392_823_0 [ 2 ] = _rtP -> P_885 * tmp [ 2 ] ; _rtB -> B_392_827_0 = _rtB ->
B_392_826_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_827_0 , &
B_392_823_0 [ 0 ] , _rtB -> B_392_12_0 , & _rtB -> Subsystem1_p , & _rtDW ->
Subsystem1_p ) ; _rtB -> B_392_831_0 = _rtB -> B_392_830_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_831_0 , &
B_392_823_0 [ 0 ] , _rtB -> B_392_12_0 , & _rtB -> Subsystempi2delay_d , &
_rtDW -> Subsystempi2delay_d ) ; if ( _rtB -> B_392_826_0 != 0 ) { _rtB ->
B_392_833_0 [ 0 ] = _rtB -> Subsystem1_p . B_11_0_0 ; _rtB -> B_392_833_0 [ 1
] = _rtB -> Subsystem1_p . B_11_1_0 ; } else { _rtB -> B_392_833_0 [ 0 ] =
_rtB -> Subsystempi2delay_d . B_10_0_0 ; _rtB -> B_392_833_0 [ 1 ] = _rtB ->
Subsystempi2delay_d . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE != 0 )
{ _rtB -> B_392_834_0 = _rtDW -> Integ4_DSTATE ; } else { _rtB -> B_392_834_0
= _rtP -> P_889 * _rtB -> B_392_833_0 [ 1 ] + _rtDW -> Integ4_DSTATE ; } if (
rtb_B_392_257_0 > _rtP -> P_891 ) { rtb_B_392_790_0 = _rtP -> P_891 ; } else
if ( rtb_B_392_257_0 < _rtP -> P_892 ) { rtb_B_392_790_0 = _rtP -> P_892 ; }
else { rtb_B_392_790_0 = rtb_B_392_257_0 ; } rtb_B_392_790_0 = 1.0 /
rtb_B_392_790_0 / 5.0e-5 ; rtb_B_392_410_0 = muDoubleScalarCeil (
rtb_B_392_790_0 ) ; _rtB -> B_392_838_0 = _rtP -> P_893 * rtb_B_392_410_0 ;
ssCallAccelRunBlock ( S , 392 , 839 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_392_842_0 ) { rtb_B_392_410_0 =
rtb_B_392_790_0 - rtb_B_392_410_0 ; _rtB -> B_392_846_0 = ( ( _rtB ->
B_392_833_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_i ) * rtb_B_392_410_0 * _rtP ->
P_61 + _rtB -> B_392_833_0 [ 1 ] ) * ( rtb_B_392_410_0 / rtb_B_392_790_0 ) +
( _rtB -> B_392_834_0 - _rtB -> B_392_839_0 ) * rtb_B_392_257_0 ; } else {
_rtB -> B_392_846_0 = _rtDW -> UnitDelay1_DSTATE_i ; } _rtB -> B_392_847_0 =
_rtB -> B_392_846_0 * _rtB -> B_12_41_0 ; _rtDW -> DiscreteDerivative_tmp = (
_rtB -> B_392_847_0 - _rtP -> P_906 [ 1 ] * _rtDW ->
DiscreteDerivative_states ) / _rtP -> P_906 [ 0 ] ; rtb_B_392_790_0 = ( _rtP
-> P_912 * _rtB -> B_392_847_0 + _rtDW -> DiscreteTimeIntegrator_DSTATE_c ) +
( _rtP -> P_905 [ 0 ] * _rtDW -> DiscreteDerivative_tmp + _rtP -> P_905 [ 1 ]
* _rtDW -> DiscreteDerivative_states ) ; if ( rtb_B_392_790_0 > _rtP -> P_913
) { _rtB -> B_392_852_0 = _rtP -> P_913 ; } else if ( rtb_B_392_790_0 < _rtP
-> P_914 ) { _rtB -> B_392_852_0 = _rtP -> P_914 ; } else { _rtB ->
B_392_852_0 = rtb_B_392_790_0 ; } rtb_B_392_257_0 = _rtP -> P_915 * _rtB ->
B_392_852_0 ; rtb_B_392_16_0 = rtb_B_392_257_0 - _rtDW -> PrevY ; if (
rtb_B_392_16_0 > _rtP -> P_916 ) { rtb_B_392_257_0 = _rtDW -> PrevY + _rtP ->
P_916 ; } else { if ( rtb_B_392_16_0 < _rtP -> P_917 ) { rtb_B_392_257_0 =
_rtDW -> PrevY + _rtP -> P_917 ; } } _rtDW -> PrevY = rtb_B_392_257_0 ; _rtB
-> B_392_864_0 = ( _rtP -> P_920 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_922 *
_rtDW -> Delay_x2_DSTATE ) + _rtP -> P_925 * rtb_B_392_257_0 ; _rtB ->
B_392_866_0 = ( _rtP -> P_923 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_924 *
_rtDW -> Delay_x2_DSTATE ) + _rtP -> P_926 * rtb_B_392_257_0 ; _rtB ->
B_392_871_0 = ( _rtP -> P_928 * _rtDW -> Delay_x1_DSTATE + _rtP -> P_929 *
_rtDW -> Delay_x2_DSTATE ) + _rtP -> P_927 * rtb_B_392_257_0 ; } if ( _rtB ->
B_392_17_0 > _rtP -> P_931 ) { _rtB -> B_392_873_0 = _rtB -> B_392_746_0 ; }
else { _rtB -> B_392_873_0 = _rtB -> B_392_721_0 ; } if ( _rtB -> B_392_17_0
> _rtP -> P_932 ) { _rtB -> B_392_874_0 = _rtB -> B_392_739_0 ; } else { _rtB
-> B_392_874_0 = _rtB -> B_392_721_1 ; } _rtB -> B_392_878_0 = (
muDoubleScalarSin ( _rtP -> P_937 * ssGetTaskTime ( S , 0 ) + _rtP -> P_938 )
* _rtP -> P_935 + _rtP -> P_936 ) * _rtB -> B_392_599_0 [ 0 ] ; _rtB ->
B_392_880_0 = ( muDoubleScalarSin ( _rtP -> P_941 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_942 ) * _rtP -> P_939 + _rtP -> P_940 ) * _rtB -> B_392_599_0 [ 0 ]
; _rtB -> B_392_882_0 = ( muDoubleScalarSin ( _rtP -> P_945 * ssGetTaskTime (
S , 0 ) + _rtP -> P_946 ) * _rtP -> P_943 + _rtP -> P_944 ) * _rtB ->
B_392_599_0 [ 1 ] ; _rtB -> B_392_884_0 = ( muDoubleScalarSin ( _rtP -> P_949
* ssGetTaskTime ( S , 0 ) + _rtP -> P_950 ) * _rtP -> P_947 + _rtP -> P_948 )
* _rtB -> B_392_599_0 [ 1 ] ; _rtB -> B_392_886_0 = ( muDoubleScalarSin (
_rtP -> P_953 * ssGetTaskTime ( S , 0 ) + _rtP -> P_954 ) * _rtP -> P_951 +
_rtP -> P_952 ) * _rtB -> B_392_599_0 [ 2 ] ; _rtB -> B_392_888_0 = (
muDoubleScalarSin ( _rtP -> P_957 * ssGetTaskTime ( S , 0 ) + _rtP -> P_958 )
* _rtP -> P_955 + _rtP -> P_956 ) * _rtB -> B_392_599_0 [ 2 ] ; B_392_889_0 =
_rtP -> P_959 * rtb_B_392_32_0 [ 0 ] ; _rtB -> B_392_891_0 = (
muDoubleScalarSin ( _rtP -> P_962 * ssGetTaskTime ( S , 0 ) + _rtP -> P_963 )
* _rtP -> P_960 + _rtP -> P_961 ) * _rtB -> B_392_664_0 [ 0 ] ; _rtB ->
B_392_893_0 = ( muDoubleScalarSin ( _rtP -> P_966 * ssGetTaskTime ( S , 0 ) +
_rtP -> P_967 ) * _rtP -> P_964 + _rtP -> P_965 ) * _rtB -> B_392_664_0 [ 0 ]
; _rtB -> B_392_895_0 = ( muDoubleScalarSin ( _rtP -> P_970 * ssGetTaskTime (
S , 0 ) + _rtP -> P_971 ) * _rtP -> P_968 + _rtP -> P_969 ) * _rtB ->
B_392_664_0 [ 1 ] ; _rtB -> B_392_897_0 = ( muDoubleScalarSin ( _rtP -> P_974
* ssGetTaskTime ( S , 0 ) + _rtP -> P_975 ) * _rtP -> P_972 + _rtP -> P_973 )
* _rtB -> B_392_664_0 [ 1 ] ; _rtB -> B_392_899_0 = ( muDoubleScalarSin (
_rtP -> P_978 * ssGetTaskTime ( S , 0 ) + _rtP -> P_979 ) * _rtP -> P_976 +
_rtP -> P_977 ) * _rtB -> B_392_664_0 [ 2 ] ; _rtB -> B_392_901_0 = (
muDoubleScalarSin ( _rtP -> P_982 * ssGetTaskTime ( S , 0 ) + _rtP -> P_983 )
* _rtP -> P_980 + _rtP -> P_981 ) * _rtB -> B_392_664_0 [ 2 ] ; B_392_902_0 =
_rtP -> P_984 * rtb_B_392_239_0 ; _rtB -> B_392_591_0 [ 0 ] = _rtB ->
B_392_418_0 ; _rtB -> B_392_591_0 [ 1 ] = _rtB -> B_392_431_0 ; _rtB ->
B_392_591_0 [ 2 ] = _rtB -> B_392_419_0 ; _rtB -> B_392_591_0 [ 3 ] = _rtB ->
B_392_505_0 ; _rtB -> B_392_591_0 [ 4 ] = _rtB -> B_392_579_0 ; _rtB ->
B_392_591_0 [ 5 ] = _rtB -> B_392_429_0 ; _rtB -> B_392_591_0 [ 6 ] = _rtB ->
B_54_0_1 ; _rtB -> B_392_591_0 [ 7 ] = _rtB -> B_54_0_2 ; _rtB -> B_392_591_0
[ 8 ] = 0.0 ; _rtB -> B_392_591_0 [ 9 ] = 0.0 ; _rtB -> B_392_591_0 [ 10 ] =
0.0 ; _rtB -> B_392_591_0 [ 11 ] = 0.0 ; ssCallAccelRunBlock ( S , 392 , 945
, SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 392 , 947 ,
SS_CALL_MDL_OUTPUTS ) ; ssCallAccelRunBlock ( S , 392 , 948 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_953_0 [ 0 ] = _rtP -> P_987 * _rtB -> B_0_14_0 [ 47 ] *
_rtP -> P_990 ; _rtB -> B_392_953_0 [ 1 ] = _rtP -> P_988 * _rtB -> B_0_14_0
[ 48 ] * _rtP -> P_990 ; _rtB -> B_392_953_0 [ 2 ] = _rtP -> P_989 * _rtB ->
B_0_14_0 [ 49 ] * _rtP -> P_990 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) {
tmp [ isHit ] = _rtP -> P_991 [ isHit + 6 ] * _rtB -> B_392_953_0 [ 2 ] + (
_rtP -> P_991 [ isHit + 3 ] * _rtB -> B_392_953_0 [ 1 ] + _rtP -> P_991 [
isHit ] * _rtB -> B_392_953_0 [ 0 ] ) ; } _rtB -> B_392_955_0 [ 0 ] = _rtP ->
P_992 * tmp [ 0 ] ; _rtB -> B_392_955_0 [ 1 ] = _rtP -> P_992 * tmp [ 1 ] ;
_rtB -> B_392_955_0 [ 2 ] = _rtP -> P_992 * tmp [ 2 ] ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_959_0 = _rtB
-> B_392_958_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1 ( S , _rtB -> B_392_959_0
, & _rtB -> B_392_955_0 [ 0 ] , _rtB -> B_392_54_0 , & _rtB -> Subsystem1_k ,
& _rtDW -> Subsystem1_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit
!= 0 ) { _rtB -> B_392_963_0 = _rtB -> B_392_962_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_963_0 , & _rtB ->
B_392_955_0 [ 0 ] , _rtB -> B_392_54_0 , & _rtB -> Subsystempi2delay_ac , &
_rtDW -> Subsystempi2delay_ac ) ; if ( _rtB -> B_392_958_0 != 0 ) { _rtB ->
B_392_965_0 [ 0 ] = _rtB -> Subsystem1_k . B_3_0_0 ; _rtB -> B_392_965_0 [ 1
] = _rtB -> Subsystem1_k . B_3_1_0 ; } else { _rtB -> B_392_965_0 [ 0 ] =
_rtB -> Subsystempi2delay_ac . B_2_0_0 ; _rtB -> B_392_965_0 [ 1 ] = _rtB ->
Subsystempi2delay_ac . B_2_1_0 ; } _rtB -> B_392_966_0 = 0.0 ; _rtB ->
B_392_966_0 += _rtP -> P_997 * _rtX -> Integradord_CSTATE_a ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_998 *
_rtB -> B_0_14_0 [ 99 ] * _rtP -> P_1001 ; tmp_1 = _rtP -> P_999 * _rtB ->
B_0_14_0 [ 100 ] * _rtP -> P_1001 ; rtb_B_392_16_0 = _rtP -> P_1000 * _rtB ->
B_0_14_0 [ 101 ] * _rtP -> P_1001 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_1002 [ isHit + 6 ] * rtb_B_392_16_0 + ( _rtP ->
P_1002 [ isHit + 3 ] * tmp_1 + _rtP -> P_1002 [ isHit ] * tmp_0 ) ; } _rtB ->
B_392_972_0 [ 0 ] = _rtP -> P_1003 * tmp [ 0 ] ; _rtB -> B_392_972_0 [ 1 ] =
_rtP -> P_1003 * tmp [ 1 ] ; _rtB -> B_392_972_0 [ 2 ] = _rtP -> P_1003 * tmp
[ 2 ] ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_976_0 = _rtB -> B_392_975_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1 ( S ,
_rtB -> B_392_976_0 , & _rtB -> B_392_972_0 [ 0 ] , _rtB -> B_392_54_0 , &
_rtB -> Subsystem1_h , & _rtDW -> Subsystem1_h ) ; isHit = ssIsSampleHit ( S
, 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_980_0 = _rtB -> B_392_979_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_980_0 , & _rtB ->
B_392_972_0 [ 0 ] , _rtB -> B_392_54_0 , & _rtB -> Subsystempi2delay_k , &
_rtDW -> Subsystempi2delay_k ) ; if ( _rtB -> B_392_975_0 != 0 ) { _rtB ->
B_392_982_0 [ 0 ] = _rtB -> Subsystem1_h . B_3_0_0 ; _rtB -> B_392_982_0 [ 1
] = _rtB -> Subsystem1_h . B_3_1_0 ; } else { _rtB -> B_392_982_0 [ 0 ] =
_rtB -> Subsystempi2delay_k . B_2_0_0 ; _rtB -> B_392_982_0 [ 1 ] = _rtB ->
Subsystempi2delay_k . B_2_1_0 ; } _rtB -> B_392_983_0 = 0.0 ; _rtB ->
B_392_983_0 += _rtP -> P_1008 * _rtX -> LPF4_CSTATE_f ; _rtB -> B_392_984_0 =
_rtB -> B_392_983_0 - _rtB -> B_392_982_0 [ 0 ] ; _rtB -> B_392_985_0 = _rtP
-> P_1009 * _rtB -> B_392_984_0 ; _rtB -> B_392_986_0 = _rtB -> B_392_966_0 +
_rtB -> B_392_985_0 ; _rtB -> B_392_987_0 = _rtB -> B_392_431_0 * _rtB ->
B_392_982_0 [ 1 ] ; _rtB -> B_392_988_0 = _rtP -> P_1010 * _rtB ->
B_392_987_0 ; _rtB -> B_392_989_0 = ( _rtB -> B_392_965_0 [ 0 ] + _rtB ->
B_392_986_0 ) - _rtB -> B_392_988_0 ; isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_991_0 = _rtDW -> UnitDelay1_DSTATE_a / _rtB
-> B_392_949_0 ; } _rtB -> B_392_992_0 = _rtB -> B_392_989_0 / _rtB ->
B_392_991_0 ; _rtB -> B_392_993_0 = _rtB -> B_392_982_0 [ 0 ] * _rtB ->
B_392_431_0 ; _rtB -> B_392_994_0 = _rtP -> P_1012 * _rtB -> B_392_993_0 ;
_rtB -> B_392_995_0 = 0.0 ; _rtB -> B_392_995_0 += _rtP -> P_1014 * _rtX ->
LPF2_CSTATE_a ; _rtB -> B_392_996_0 = _rtB -> B_392_995_0 - _rtB ->
B_392_982_0 [ 1 ] ; _rtB -> B_392_997_0 = _rtP -> P_1015 * _rtB ->
B_392_996_0 ; _rtB -> B_392_998_0 = 0.0 ; _rtB -> B_392_998_0 += _rtP ->
P_1017 * _rtX -> Integradord1_CSTATE_n ; _rtB -> B_392_999_0 = _rtB ->
B_392_997_0 + _rtB -> B_392_998_0 ; _rtB -> B_392_1000_0 = ( _rtB ->
B_392_994_0 + _rtB -> B_392_999_0 ) + _rtB -> B_392_965_0 [ 1 ] ; _rtB ->
B_392_1001_0 = 1.0 / _rtB -> B_392_991_0 * _rtB -> B_392_1000_0 ; _rtB ->
B_392_1002_0 = _rtB -> B_392_39_0 - _rtB -> B_392_45_0 ; _rtB -> B_392_1003_0
= _rtP -> P_1018 * _rtB -> B_392_1002_0 ; _rtB -> B_392_1004_0 = _rtB ->
B_392_1003_0 * _rtB -> B_392_991_0 ; _rtB -> B_392_1005_0 = _rtB ->
B_392_40_0 - _rtB -> B_392_45_1 ; _rtB -> B_392_1006_0 = _rtP -> P_1019 *
_rtB -> B_392_1005_0 ; _rtB -> B_392_1007_0 = _rtB -> B_392_991_0 * _rtB ->
B_392_1006_0 ; _rtB -> B_392_1008_0 = _rtB -> B_392_984_0 - _rtB ->
B_392_1004_0 ; _rtB -> B_392_1009_0 = _rtB -> B_392_996_0 - _rtB ->
B_392_1007_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_392_1010_0 = _rtP -> P_1020 * _rtB -> B_37_0_2 ; } isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_16_0 = ssGetTaskTime ( S , 1 )
; _rtDW -> ref2_MODE_l = ( rtb_B_392_16_0 >= _rtP -> P_1021 ) ; if ( _rtDW ->
ref2_MODE_l == 1 ) { rtb_B_392_16_0 = _rtP -> P_1023 ; } else {
rtb_B_392_16_0 = _rtP -> P_1022 ; } _rtB -> B_392_1014_0 = ( rtb_B_392_16_0 -
_rtB -> B_392_1010_0 ) * _rtP -> P_1024 ; } _rtB -> B_392_1015_0 = _rtB ->
B_392_965_0 [ 0 ] * _rtB -> B_392_431_0 ; _rtB -> B_392_1016_0 = _rtP ->
P_1025 * _rtB -> B_392_1015_0 ; _rtB -> B_392_1017_0 = _rtB -> B_392_431_0 *
_rtB -> B_392_965_0 [ 1 ] ; _rtB -> B_392_1018_0 = _rtP -> P_1026 * _rtB ->
B_392_1017_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB
-> B_392_1022_0 [ 0 ] = _rtP -> P_1027 * _rtB -> B_0_14_0 [ 102 ] * _rtP ->
P_1030 ; _rtB -> B_392_1022_0 [ 1 ] = _rtP -> P_1028 * _rtB -> B_0_14_0 [ 103
] * _rtP -> P_1030 ; _rtB -> B_392_1022_0 [ 2 ] = _rtP -> P_1029 * _rtB ->
B_0_14_0 [ 104 ] * _rtP -> P_1030 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_1031 [ isHit + 6 ] * _rtB -> B_392_1022_0 [ 2 ] +
( _rtP -> P_1031 [ isHit + 3 ] * _rtB -> B_392_1022_0 [ 1 ] + _rtP -> P_1031
[ isHit ] * _rtB -> B_392_1022_0 [ 0 ] ) ; } _rtB -> B_392_1024_0 [ 0 ] =
_rtP -> P_1032 * tmp [ 0 ] ; _rtB -> B_392_1024_0 [ 1 ] = _rtP -> P_1032 *
tmp [ 1 ] ; _rtB -> B_392_1024_0 [ 2 ] = _rtP -> P_1032 * tmp [ 2 ] ; } isHit
= ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1028_0 =
_rtB -> B_392_1027_0 ; } MR_CIGRE_GENERALIZADO_Subsystem1 ( S , _rtB ->
B_392_1028_0 , & _rtB -> B_392_1024_0 [ 0 ] , _rtB -> B_392_54_0 , & _rtB ->
Subsystem1_bc , & _rtDW -> Subsystem1_bc ) ; isHit = ssIsSampleHit ( S , 1 ,
0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1032_0 = _rtB -> B_392_1031_0 ; }
MR_CIGRE_GENERALIZADO_Subsystempi2delay ( S , _rtB -> B_392_1032_0 , & _rtB
-> B_392_1024_0 [ 0 ] , _rtB -> B_392_54_0 , & _rtB -> Subsystempi2delay_g ,
& _rtDW -> Subsystempi2delay_g ) ; if ( _rtB -> B_392_1027_0 != 0 ) { _rtB ->
B_392_1034_0 [ 0 ] = _rtB -> Subsystem1_bc . B_3_0_0 ; _rtB -> B_392_1034_0 [
1 ] = _rtB -> Subsystem1_bc . B_3_1_0 ; } else { _rtB -> B_392_1034_0 [ 0 ] =
_rtB -> Subsystempi2delay_g . B_2_0_0 ; _rtB -> B_392_1034_0 [ 1 ] = _rtB ->
Subsystempi2delay_g . B_2_1_0 ; } _rtB -> B_392_1035_0 = _rtP -> P_1036 *
_rtB -> B_392_1034_0 [ 0 ] ; _rtB -> B_392_1036_0 = 0.0 ; _rtB ->
B_392_1036_0 += _rtP -> P_1038 * _rtX -> Integradord_CSTATE_n ; _rtB ->
B_392_1037_0 = _rtP -> P_1039 * _rtB -> B_392_1034_0 [ 0 ] ; _rtB ->
B_392_1038_0 = 0.0 ; _rtB -> B_392_1038_0 += _rtP -> P_1041 * _rtX ->
HPF1_CSTATE_k ; _rtB -> B_392_1038_0 += _rtP -> P_1042 * _rtB -> B_392_1037_0
; _rtB -> B_392_1039_0 = _rtP -> P_1043 * _rtB -> B_392_1034_0 [ 0 ] ; _rtB
-> B_392_1040_0 = _rtB -> B_392_1038_0 + _rtB -> B_392_1039_0 ; _rtB ->
B_392_1041_0 = _rtP -> P_1044 * _rtB -> B_392_1034_0 [ 1 ] ; _rtB ->
B_392_1042_0 = _rtB -> B_392_1041_0 * _rtB -> B_392_431_0 ; _rtB ->
B_392_1043_0 = _rtB -> B_392_1040_0 + _rtB -> B_392_1042_0 ; _rtB ->
B_392_1044_0 = _rtB -> B_392_1014_0 - _rtB -> B_392_1043_0 ; _rtB ->
B_392_1045_0 = _rtB -> B_392_1044_0 - _rtB -> B_392_965_0 [ 0 ] ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_52_0 >
_rtP -> P_1047 ) { _rtB -> B_392_1048_0 = _rtB -> B_392_1046_0 ; } else {
_rtB -> B_392_1048_0 = _rtB -> B_392_1047_0 ; } } _rtB -> B_392_1049_0 = _rtB
-> B_392_1045_0 * _rtB -> B_392_1048_0 ; _rtB -> B_392_1050_0 = _rtP ->
P_1048 * _rtB -> B_392_1049_0 ; _rtB -> B_392_1051_0 = _rtB -> B_392_1036_0 +
_rtB -> B_392_1050_0 ; _rtB -> B_392_1052_0 = ( _rtB -> B_392_1035_0 + _rtB
-> B_392_1051_0 ) - _rtB -> B_392_1018_0 ; _rtB -> B_392_1053_0 = _rtB ->
B_392_1052_0 * _rtB -> B_392_1048_0 ; _rtB -> B_392_1055_0 = _rtP -> P_1050 *
_rtB -> B_392_1034_0 [ 0 ] ; _rtB -> B_392_1056_0 = _rtB -> B_392_431_0 *
_rtB -> B_392_1055_0 ; _rtB -> B_392_1057_0 = _rtP -> P_1051 * _rtB ->
B_392_1034_0 [ 1 ] ; _rtB -> B_392_1058_0 = 0.0 ; _rtB -> B_392_1058_0 +=
_rtP -> P_1053 * _rtX -> HPF2_CSTATE_g ; _rtB -> B_392_1058_0 += _rtP ->
P_1054 * _rtB -> B_392_1057_0 ; _rtB -> B_392_1059_0 = _rtP -> P_1055 * _rtB
-> B_392_1034_0 [ 1 ] ; _rtB -> B_392_1060_0 = _rtB -> B_392_1058_0 + _rtB ->
B_392_1059_0 ; _rtB -> B_392_1061_0 = _rtB -> B_392_1056_0 + _rtB ->
B_392_1060_0 ; _rtB -> B_392_1062_0 = _rtB -> B_392_1054_0 - _rtB ->
B_392_1061_0 ; _rtB -> B_392_1063_0 = _rtB -> B_392_1062_0 - _rtB ->
B_392_965_0 [ 1 ] ; _rtB -> B_392_1064_0 = _rtB -> B_392_1048_0 * _rtB ->
B_392_1063_0 ; _rtB -> B_392_1065_0 = _rtP -> P_1056 * _rtB -> B_392_1064_0 ;
_rtB -> B_392_1066_0 = 0.0 ; _rtB -> B_392_1066_0 += _rtP -> P_1058 * _rtX ->
Integradorq_CSTATE_a ; _rtB -> B_392_1067_0 = _rtB -> B_392_1065_0 + _rtB ->
B_392_1066_0 ; _rtB -> B_392_1068_0 = _rtP -> P_1059 * _rtB -> B_392_1034_0 [
1 ] ; _rtB -> B_392_1069_0 = ( _rtB -> B_392_1016_0 + _rtB -> B_392_1067_0 )
+ _rtB -> B_392_1068_0 ; _rtB -> B_392_1070_0 = _rtB -> B_392_1048_0 * _rtB
-> B_392_1069_0 ; _rtB -> B_392_1071_0 . re = _rtB -> B_392_1053_0 ; _rtB ->
B_392_1071_0 . im = _rtB -> B_392_1070_0 ; _rtB -> B_392_1072_0 =
muDoubleScalarHypot ( _rtB -> B_392_1071_0 . re , _rtB -> B_392_1071_0 . im )
; rtb_B_392_257_0 = muDoubleScalarAtan2 ( _rtB -> B_392_1071_0 . im , _rtB ->
B_392_1071_0 . re ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_n = _rtB -> B_392_1072_0 >= _rtP -> P_1060 ? 1 : _rtB ->
B_392_1072_0 > _rtP -> P_1061 ? 0 : - 1 ; } _rtB -> B_392_1073_0 = _rtDW ->
Saturation_MODE_n == 1 ? _rtP -> P_1060 : _rtDW -> Saturation_MODE_n == - 1 ?
_rtP -> P_1061 : _rtB -> B_392_1072_0 ; _rtB -> B_392_1075_0 = _rtB ->
B_392_1073_0 * muDoubleScalarCos ( rtb_B_392_257_0 ) ; _rtB -> B_392_1075_1 =
_rtB -> B_392_1073_0 * muDoubleScalarSin ( rtb_B_392_257_0 ) ; _rtB ->
B_392_1076_0 = _rtB -> B_392_1053_0 - _rtB -> B_392_1075_0 ; _rtB ->
B_392_1077_0 = _rtP -> P_1062 * _rtB -> B_392_1076_0 ; _rtB -> B_392_1078_0 =
_rtB -> B_392_1049_0 - _rtB -> B_392_1077_0 ; _rtB -> B_392_1079_0 = _rtP ->
P_1063 * _rtB -> B_392_1078_0 ; _rtB -> B_392_1080_0 = _rtB -> B_392_1070_0 -
_rtB -> B_392_1075_1 ; _rtB -> B_392_1081_0 = _rtP -> P_1064 * _rtB ->
B_392_1080_0 ; _rtB -> B_392_1082_0 = _rtB -> B_392_1064_0 - _rtB ->
B_392_1081_0 ; _rtB -> B_392_1083_0 = _rtP -> P_1065 * _rtB -> B_392_1082_0 ;
_rtB -> B_392_1084_0 = _rtB -> B_392_965_0 [ 1 ] * _rtB -> B_392_965_0 [ 1 ]
; _rtB -> B_392_1085_0 = _rtB -> B_392_965_0 [ 0 ] * _rtB -> B_392_965_0 [ 0
] ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtB ->
B_392_52_0 > _rtP -> P_1066 ) { _rtB -> B_392_1086_0 = _rtB -> B_392_431_0 ;
} else { _rtB -> B_392_1086_0 = _rtB -> B_392_429_0 ; } } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_392_1090_0 [ 0 ] = _rtP
-> P_1067 * _rtB -> B_0_14_0 [ 44 ] * _rtP -> P_1070 ; B_392_1090_0 [ 1 ] =
_rtP -> P_1068 * _rtB -> B_0_14_0 [ 45 ] * _rtP -> P_1070 ; B_392_1090_0 [ 2
] = _rtP -> P_1069 * _rtB -> B_0_14_0 [ 46 ] * _rtP -> P_1070 ;
rtb_B_392_1128_0 = _rtP -> P_1072 * _rtB -> B_392_68_0 - _rtDW ->
prediction_DSTATE_l ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1132_0 = rtb_B_392_1128_0 + _rtB -> B_392_1125_0 ; } _rtB
-> B_392_1133_0 = ( _rtB -> B_392_71_0 * _rtB -> B_0_14_0 [ 150 ] - _rtB ->
B_392_72_0 * _rtB -> B_0_14_0 [ 151 ] ) / _rtB -> B_392_1132_0 ; _rtB ->
B_392_1148_0 = _rtB -> B_392_965_0 [ 0 ] * _rtB -> B_392_982_0 [ 0 ] ; _rtB
-> B_392_1149_0 = _rtB -> B_392_965_0 [ 1 ] * _rtB -> B_392_982_0 [ 1 ] ;
_rtB -> B_392_1150_0 = _rtB -> B_392_1148_0 + _rtB -> B_392_1149_0 ; _rtB ->
B_392_1151_0 = _rtP -> P_1074 * _rtB -> B_392_1150_0 ; _rtB -> B_392_1152_0 =
_rtB -> B_392_965_0 [ 0 ] * _rtB -> B_392_982_0 [ 1 ] ; _rtB -> B_392_1153_0
= _rtB -> B_392_982_0 [ 0 ] * _rtB -> B_392_965_0 [ 1 ] ; _rtB ->
B_392_1154_0 = _rtB -> B_392_1153_0 - _rtB -> B_392_1152_0 ; _rtB ->
B_392_1155_0 = _rtP -> P_1075 * _rtB -> B_392_1154_0 ; isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1157_0 = _rtP -> P_1076 ;
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && (
ssIsMajorTimeStep ( S ) != 0 ) ) { if ( _rtB -> B_392_1157_0 > 0.0 ) { if ( !
_rtDW -> AutomaticGainControl_MODE_i ) { if ( ssGetTaskTime ( S , 3 ) !=
ssGetTStart ( S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; }
_rtDW -> Integ4_SYSTEM_ENABLE_lr = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_c = 1U
; _rtDW -> AutomaticGainControl_MODE_i = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE_i ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> AutomaticGainControl_MODE_i = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_i ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_77 [ isHit + 6 ] * _rtB -> B_392_953_0 [ 2 ] + ( _rtP -> P_77 [
isHit + 3 ] * _rtB -> B_392_953_0 [ 1 ] + _rtP -> P_77 [ isHit ] * _rtB ->
B_392_953_0 [ 0 ] ) ; } B_48_1_0 [ 0 ] = _rtP -> P_78 * tmp [ 0 ] ; B_48_1_0
[ 1 ] = _rtP -> P_78 * tmp [ 1 ] ; B_48_1_0 [ 2 ] = _rtP -> P_78 * tmp [ 2 ]
; _rtB -> B_48_5_0 = _rtB -> B_48_4_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_b (
_rtB -> B_48_5_0 , & B_48_1_0 [ 0 ] , _rtB -> B_392_50_0 , & _rtB ->
Subsystem1_hy , & _rtDW -> Subsystem1_hy ) ; _rtB -> B_48_9_0 = _rtB ->
B_48_8_0 ; MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_48_9_0 , &
B_48_1_0 [ 0 ] , _rtB -> B_392_50_0 , & _rtB -> Subsystempi2delay_ck , &
_rtDW -> Subsystempi2delay_ck ) ; if ( _rtB -> B_48_4_0 != 0 ) { _rtB ->
B_48_11_0 [ 0 ] = _rtB -> Subsystem1_hy . B_11_0_0 ; _rtB -> B_48_11_0 [ 1 ]
= _rtB -> Subsystem1_hy . B_11_1_0 ; } else { _rtB -> B_48_11_0 [ 0 ] = _rtB
-> Subsystempi2delay_ck . B_10_0_0 ; _rtB -> B_48_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_ck . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_lr !=
0 ) { _rtB -> B_48_12_0 = _rtDW -> Integ4_DSTATE_a ; } else { _rtB ->
B_48_12_0 = _rtP -> P_82 * _rtB -> B_48_11_0 [ 0 ] + _rtDW -> Integ4_DSTATE_a
; } if ( rtb_B_392_430_0 > _rtP -> P_84 ) { rtb_B_392_239_0 = _rtP -> P_84 ;
} else if ( rtb_B_392_430_0 < _rtP -> P_85 ) { rtb_B_392_239_0 = _rtP -> P_85
; } else { rtb_B_392_239_0 = rtb_B_392_430_0 ; } rtb_B_392_1128_0 = 1.0 /
rtb_B_392_239_0 / 5.0e-5 ; rtb_B_392_257_0 = muDoubleScalarCeil (
rtb_B_392_1128_0 ) ; _rtB -> B_48_16_0 = _rtP -> P_86 * rtb_B_392_257_0 ;
ssCallAccelRunBlock ( S , 48 , 17 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_48_20_0 ) { rtb_B_392_257_0 =
rtb_B_392_1128_0 - rtb_B_392_257_0 ; _rtB -> B_48_24_0 = ( ( _rtB ->
B_48_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_oc ) * rtb_B_392_257_0 * _rtP ->
P_74 + _rtB -> B_48_11_0 [ 0 ] ) * ( rtb_B_392_257_0 / rtb_B_392_1128_0 ) + (
_rtB -> B_48_12_0 - _rtB -> B_48_17_0 ) * rtb_B_392_430_0 ; } else { _rtB ->
B_48_24_0 = _rtDW -> UnitDelay1_DSTATE_c ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_c != 0 ) { _rtB -> B_48_25_0 = _rtDW -> Integ4_DSTATE_am
; } else { _rtB -> B_48_25_0 = _rtP -> P_98 * _rtB -> B_48_11_0 [ 1 ] + _rtDW
-> Integ4_DSTATE_am ; } if ( rtb_B_392_430_0 > _rtP -> P_100 ) {
rtb_B_392_239_0 = _rtP -> P_100 ; } else if ( rtb_B_392_430_0 < _rtP -> P_101
) { rtb_B_392_239_0 = _rtP -> P_101 ; } else { rtb_B_392_239_0 =
rtb_B_392_430_0 ; } rtb_B_392_1128_0 = 1.0 / rtb_B_392_239_0 / 5.0e-5 ;
rtb_B_392_257_0 = muDoubleScalarCeil ( rtb_B_392_1128_0 ) ; _rtB -> B_48_29_0
= _rtP -> P_102 * rtb_B_392_257_0 ; ssCallAccelRunBlock ( S , 48 , 30 ,
SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB -> B_48_33_0 ) {
rtb_B_392_257_0 = rtb_B_392_1128_0 - rtb_B_392_257_0 ; _rtB -> B_48_37_0 = (
( _rtB -> B_48_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_bg ) * rtb_B_392_257_0
* _rtP -> P_75 + _rtB -> B_48_11_0 [ 1 ] ) * ( rtb_B_392_257_0 /
rtb_B_392_1128_0 ) + ( _rtB -> B_48_25_0 - _rtB -> B_48_30_0 ) *
rtb_B_392_430_0 ; } else { _rtB -> B_48_37_0 = _rtDW -> UnitDelay1_DSTATE_o0
; } rtb_B_392_790_0 = muDoubleScalarHypot ( _rtB -> B_48_24_0 , _rtB ->
B_48_37_0 ) ; if ( rtb_B_392_790_0 > _rtP -> P_114 ) { rtb_B_392_790_0 = _rtP
-> P_114 ; } else { if ( rtb_B_392_790_0 < _rtP -> P_115 ) { rtb_B_392_790_0
= _rtP -> P_115 ; } } _rtB -> B_48_41_0 = 1.0 / rtb_B_392_790_0 ; B_48_42_0 =
_rtP -> P_116 * muDoubleScalarAtan2 ( _rtB -> B_48_37_0 , _rtB -> B_48_24_0 )
; } if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_l ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_1077 [ isHit + 6 ] * _rtB ->
B_392_953_0 [ 2 ] + ( _rtP -> P_1077 [ isHit + 3 ] * _rtB -> B_392_953_0 [ 1
] + _rtP -> P_1077 [ isHit ] * _rtB -> B_392_953_0 [ 0 ] ) ; } B_392_1160_0 [
0 ] = _rtP -> P_1078 * tmp [ 0 ] ; B_392_1160_0 [ 1 ] = _rtP -> P_1078 * tmp
[ 1 ] ; B_392_1160_0 [ 2 ] = _rtP -> P_1078 * tmp [ 2 ] ; _rtB ->
B_392_1164_0 = _rtB -> B_392_1163_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_b (
_rtB -> B_392_1164_0 , & B_392_1160_0 [ 0 ] , _rtB -> B_392_50_0 , & _rtB ->
Subsystem1_j , & _rtDW -> Subsystem1_j ) ; _rtB -> B_392_1168_0 = _rtB ->
B_392_1167_0 ; MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB ->
B_392_1168_0 , & B_392_1160_0 [ 0 ] , _rtB -> B_392_50_0 , & _rtB ->
Subsystempi2delay_c , & _rtDW -> Subsystempi2delay_c ) ; if ( _rtB ->
B_392_1163_0 != 0 ) { _rtB -> B_392_1170_0 [ 0 ] = _rtB -> Subsystem1_j .
B_11_0_0 ; _rtB -> B_392_1170_0 [ 1 ] = _rtB -> Subsystem1_j . B_11_1_0 ; }
else { _rtB -> B_392_1170_0 [ 0 ] = _rtB -> Subsystempi2delay_c . B_10_0_0 ;
_rtB -> B_392_1170_0 [ 1 ] = _rtB -> Subsystempi2delay_c . B_10_1_0 ; } if (
_rtDW -> Integ4_SYSTEM_ENABLE_m != 0 ) { _rtB -> B_392_1171_0 = _rtDW ->
Integ4_DSTATE_b ; } else { _rtB -> B_392_1171_0 = _rtP -> P_1082 * _rtB ->
B_392_1170_0 [ 1 ] + _rtDW -> Integ4_DSTATE_b ; } if ( rtb_B_392_430_0 > _rtP
-> P_1084 ) { rtb_B_392_239_0 = _rtP -> P_1084 ; } else if ( rtb_B_392_430_0
< _rtP -> P_1085 ) { rtb_B_392_239_0 = _rtP -> P_1085 ; } else {
rtb_B_392_239_0 = rtb_B_392_430_0 ; } rtb_B_392_1128_0 = 1.0 /
rtb_B_392_239_0 / 5.0e-5 ; rtb_B_392_257_0 = muDoubleScalarCeil (
rtb_B_392_1128_0 ) ; _rtB -> B_392_1175_0 = _rtP -> P_1086 * rtb_B_392_257_0
; ssCallAccelRunBlock ( S , 392 , 1176 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_392_1179_0 ) { rtb_B_392_257_0 =
rtb_B_392_1128_0 - rtb_B_392_257_0 ; _rtB -> B_392_1183_0 = ( ( _rtB ->
B_392_1170_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_ob ) * rtb_B_392_257_0 * _rtP
-> P_117 + _rtB -> B_392_1170_0 [ 1 ] ) * ( rtb_B_392_257_0 /
rtb_B_392_1128_0 ) + ( _rtB -> B_392_1171_0 - _rtB -> B_392_1176_0 ) *
rtb_B_392_430_0 ; } else { _rtB -> B_392_1183_0 = _rtDW ->
UnitDelay1_DSTATE_k ; } _rtB -> B_392_1184_0 = _rtB -> B_392_1183_0 * _rtB ->
B_48_41_0 ; _rtDW -> DiscreteDerivative_tmp_h = ( _rtB -> B_392_1184_0 - _rtP
-> P_1099 [ 1 ] * _rtDW -> DiscreteDerivative_states_b ) / _rtP -> P_1099 [ 0
] ; rtb_B_392_790_0 = ( _rtP -> P_1105 * _rtB -> B_392_1184_0 + _rtDW ->
DiscreteTimeIntegrator_DSTATE_ce ) + ( _rtP -> P_1098 [ 0 ] * _rtDW ->
DiscreteDerivative_tmp_h + _rtP -> P_1098 [ 1 ] * _rtDW ->
DiscreteDerivative_states_b ) ; if ( rtb_B_392_790_0 > _rtP -> P_1106 ) {
_rtB -> B_392_1189_0 = _rtP -> P_1106 ; } else if ( rtb_B_392_790_0 < _rtP ->
P_1107 ) { _rtB -> B_392_1189_0 = _rtP -> P_1107 ; } else { _rtB ->
B_392_1189_0 = rtb_B_392_790_0 ; } rtb_B_392_430_0 = _rtP -> P_1108 * _rtB ->
B_392_1189_0 ; rtb_B_392_16_0 = rtb_B_392_430_0 - _rtDW -> PrevY_o ; if (
rtb_B_392_16_0 > _rtP -> P_1109 ) { rtb_B_392_430_0 = _rtDW -> PrevY_o + _rtP
-> P_1109 ; } else { if ( rtb_B_392_16_0 < _rtP -> P_1110 ) { rtb_B_392_430_0
= _rtDW -> PrevY_o + _rtP -> P_1110 ; } } _rtDW -> PrevY_o = rtb_B_392_430_0
; _rtB -> B_392_1201_0 = ( _rtP -> P_1113 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1115 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1118 * rtb_B_392_430_0 ;
_rtB -> B_392_1203_0 = ( _rtP -> P_1116 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1117 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1119 * rtb_B_392_430_0 ;
_rtB -> B_392_1208_0 = ( _rtP -> P_1121 * _rtDW -> Delay_x1_DSTATE_f + _rtP
-> P_1122 * _rtDW -> Delay_x2_DSTATE_k ) + _rtP -> P_1120 * rtb_B_392_430_0 ;
} if ( _rtB -> B_392_52_0 > _rtP -> P_1123 ) { _rtB -> B_53_0_0 = _rtB ->
B_392_1084_0 + _rtB -> B_392_1085_0 ; _rtB -> B_53_1_0 = _rtB -> B_54_0_2 *
_rtB -> B_392_965_0 [ 1 ] ; _rtB -> B_53_2_0 = _rtB -> B_54_0_1 * _rtB ->
B_392_965_0 [ 0 ] ; _rtB -> B_53_3_0 = _rtB -> B_53_2_0 - _rtB -> B_53_1_0 ;
_rtB -> B_53_4_0 = _rtB -> B_53_3_0 / _rtB -> B_53_0_0 ; _rtB -> B_53_5_0 =
_rtP -> P_119 * _rtB -> B_53_4_0 ; _rtB -> B_392_1210_0 = _rtB -> B_53_5_0 ;
} else { _rtB -> B_392_1210_0 = _rtB -> B_392_1075_0 ; } if ( _rtB ->
B_392_52_0 > _rtP -> P_1124 ) { _rtB -> B_52_0_0 = _rtB -> B_392_1085_0 -
_rtB -> B_392_1084_0 ; _rtB -> B_52_1_0 = _rtB -> B_54_0_1 * _rtB ->
B_392_965_0 [ 1 ] ; _rtB -> B_52_2_0 = _rtB -> B_54_0_2 * _rtB -> B_392_965_0
[ 0 ] ; _rtB -> B_52_3_0 = _rtB -> B_52_1_0 - _rtB -> B_52_2_0 ; _rtB ->
B_52_4_0 = _rtB -> B_52_3_0 / _rtB -> B_52_0_0 ; _rtB -> B_52_5_0 = _rtP ->
P_118 * _rtB -> B_52_4_0 ; _rtB -> B_392_1212_0 = _rtB -> B_52_5_0 ; } else {
_rtB -> B_392_1212_0 = _rtB -> B_392_1075_1 ; } _rtB -> B_392_1216_0 = (
muDoubleScalarSin ( _rtP -> P_1129 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1130
) * _rtP -> P_1127 + _rtP -> P_1128 ) * _rtB -> B_392_953_0 [ 0 ] ; _rtB ->
B_392_1218_0 = ( muDoubleScalarSin ( _rtP -> P_1133 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1134 ) * _rtP -> P_1131 + _rtP -> P_1132 ) * _rtB -> B_392_953_0
[ 0 ] ; _rtB -> B_392_1220_0 = ( muDoubleScalarSin ( _rtP -> P_1137 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1138 ) * _rtP -> P_1135 + _rtP -> P_1136
) * _rtB -> B_392_953_0 [ 1 ] ; _rtB -> B_392_1222_0 = ( muDoubleScalarSin (
_rtP -> P_1141 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1142 ) * _rtP -> P_1139
+ _rtP -> P_1140 ) * _rtB -> B_392_953_0 [ 1 ] ; _rtB -> B_392_1224_0 = (
muDoubleScalarSin ( _rtP -> P_1145 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1146
) * _rtP -> P_1143 + _rtP -> P_1144 ) * _rtB -> B_392_953_0 [ 2 ] ; _rtB ->
B_392_1226_0 = ( muDoubleScalarSin ( _rtP -> P_1149 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1150 ) * _rtP -> P_1147 + _rtP -> P_1148 ) * _rtB -> B_392_953_0
[ 2 ] ; B_392_1227_0 = _rtP -> P_1151 * rtb_B_392_237_0 ; _rtB ->
B_392_1229_0 = ( muDoubleScalarSin ( _rtP -> P_1154 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1155 ) * _rtP -> P_1152 + _rtP -> P_1153 ) * _rtB -> B_392_1022_0
[ 0 ] ; _rtB -> B_392_1231_0 = ( muDoubleScalarSin ( _rtP -> P_1158 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1159 ) * _rtP -> P_1156 + _rtP -> P_1157
) * _rtB -> B_392_1022_0 [ 0 ] ; _rtB -> B_392_1233_0 = ( muDoubleScalarSin (
_rtP -> P_1162 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1163 ) * _rtP -> P_1160
+ _rtP -> P_1161 ) * _rtB -> B_392_1022_0 [ 1 ] ; _rtB -> B_392_1235_0 = (
muDoubleScalarSin ( _rtP -> P_1166 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1167
) * _rtP -> P_1164 + _rtP -> P_1165 ) * _rtB -> B_392_1022_0 [ 1 ] ; _rtB ->
B_392_1237_0 = ( muDoubleScalarSin ( _rtP -> P_1170 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1171 ) * _rtP -> P_1168 + _rtP -> P_1169 ) * _rtB -> B_392_1022_0
[ 2 ] ; _rtB -> B_392_1239_0 = ( muDoubleScalarSin ( _rtP -> P_1174 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1175 ) * _rtP -> P_1172 + _rtP -> P_1173
) * _rtB -> B_392_1022_0 [ 2 ] ; B_392_1240_0 = _rtP -> P_1176 *
rtb_B_392_4_1 ; _rtB -> B_392_1282_0 = _rtX -> integrator_CSTATE_ot ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ke .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ke . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1178 ; B_392_1283_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mp . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_mp . Last , _rtDW -> TransportDelay_IWORK_mp .
Tail , _rtDW -> TransportDelay_IWORK_mp . Head , _rtP -> P_1179 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1285_0 = _rtP -> P_1180 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1285_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1287_0 = _rtDW -> Memory_PreviousInput_fp ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1289_0 = ( _rtB -> B_392_1282_0 -
B_392_1283_0 ) * _rtP -> P_133 ; } else { _rtB -> B_392_1289_0 = _rtB ->
B_392_1287_0 ; } _rtB -> B_392_1290_0 = _rtX -> integrator_CSTATE_a ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1183 ; B_392_1291_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_k0 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_k0 . Last , _rtDW -> TransportDelay_IWORK_k0 .
Tail , _rtDW -> TransportDelay_IWORK_k0 . Head , _rtP -> P_1184 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1293_0 = _rtP -> P_1185 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1293_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1295_0 = _rtDW -> Memory_PreviousInput_lu ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1297_0 = ( _rtB -> B_392_1290_0 -
B_392_1291_0 ) * _rtP -> P_132 ; } else { _rtB -> B_392_1297_0 = _rtB ->
B_392_1295_0 ; } rtb_B_392_430_0 = _rtP -> P_1187 * muDoubleScalarAtan2 (
_rtB -> B_392_1297_0 , _rtB -> B_392_1289_0 ) * _rtP -> P_1188 ; B_392_1302_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1289_0 , _rtB -> B_392_1297_0 ) *
muDoubleScalarCos ( rtb_B_392_430_0 ) ; B_392_1302_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1289_0 , _rtB -> B_392_1297_0 ) *
muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_1303_0 = _rtX ->
integrator_CSTATE_ob ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1190 ; B_392_1304_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l2 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_l2 . Last , _rtDW -> TransportDelay_IWORK_l2 .
Tail , _rtDW -> TransportDelay_IWORK_l2 . Head , _rtP -> P_1191 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1306_0 = _rtP -> P_1192 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1306_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1308_0 = _rtDW -> Memory_PreviousInput_p ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1310_0 = ( _rtB -> B_392_1303_0 -
B_392_1304_0 ) * _rtP -> P_135 ; } else { _rtB -> B_392_1310_0 = _rtB ->
B_392_1308_0 ; } _rtB -> B_392_1311_0 = _rtX -> integrator_CSTATE_pp ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ha .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ha . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1195 ; B_392_1312_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_a1 . Last , _rtDW -> TransportDelay_IWORK_a1 .
Tail , _rtDW -> TransportDelay_IWORK_a1 . Head , _rtP -> P_1196 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1314_0 = _rtP -> P_1197 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1314_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1316_0 = _rtDW -> Memory_PreviousInput_bn ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1318_0 = ( _rtB -> B_392_1311_0 -
B_392_1312_0 ) * _rtP -> P_134 ; } else { _rtB -> B_392_1318_0 = _rtB ->
B_392_1316_0 ; } rtb_B_392_430_0 = _rtP -> P_1199 * muDoubleScalarAtan2 (
_rtB -> B_392_1318_0 , _rtB -> B_392_1310_0 ) * _rtP -> P_1200 ; B_392_1323_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1310_0 , _rtB -> B_392_1318_0 ) *
muDoubleScalarCos ( rtb_B_392_430_0 ) ; B_392_1323_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1310_0 , _rtB -> B_392_1318_0 ) *
muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_1324_0 = _rtX ->
integrator_CSTATE_fq ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_hu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1202 ;
B_392_1325_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_eh . Last , _rtDW ->
TransportDelay_IWORK_eh . Tail , _rtDW -> TransportDelay_IWORK_eh . Head ,
_rtP -> P_1203 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1327_0 = _rtP -> P_1204 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1327_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1329_0 =
_rtDW -> Memory_PreviousInput_pm ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1331_0 = ( _rtB -> B_392_1324_0 - B_392_1325_0 ) * _rtP -> P_137 ; }
else { _rtB -> B_392_1331_0 = _rtB -> B_392_1329_0 ; } _rtB -> B_392_1332_0 =
_rtX -> integrator_CSTATE_eu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nc . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nc . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1207 ;
B_392_1333_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_f . Last , _rtDW ->
TransportDelay_IWORK_f . Tail , _rtDW -> TransportDelay_IWORK_f . Head , _rtP
-> P_1208 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1335_0 = _rtP -> P_1209 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1335_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1337_0 =
_rtDW -> Memory_PreviousInput_an ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1339_0 = ( _rtB -> B_392_1332_0 - B_392_1333_0 ) * _rtP -> P_136 ; }
else { _rtB -> B_392_1339_0 = _rtB -> B_392_1337_0 ; } rtb_B_392_430_0 = _rtP
-> P_1211 * muDoubleScalarAtan2 ( _rtB -> B_392_1339_0 , _rtB -> B_392_1331_0
) * _rtP -> P_1212 ; B_392_1344_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1331_0 , _rtB -> B_392_1339_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_1344_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1331_0 , _rtB ->
B_392_1339_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1346_0 = _rtB
-> B_392_1345_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1346_0 , B_392_1302_0 , B_392_1323_0 , B_392_1344_0 , & _rtB ->
PosSeqComputation_l , & _rtDW -> PosSeqComputation_l , & _rtP ->
PosSeqComputation_l ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1349_0 = _rtB -> B_392_1348_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1349_0 ,
B_392_1302_0 , B_392_1323_0 , B_392_1344_0 , & _rtB -> NegSeqComputation_f ,
& _rtDW -> NegSeqComputation_f , & _rtP -> NegSeqComputation_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1352_0 = _rtB
-> B_392_1351_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_1352_0 , B_392_1302_0 , B_392_1323_0 , B_392_1344_0 , & _rtB ->
ZeroSeqComputation_k , & _rtDW -> ZeroSeqComputation_k , & _rtP ->
ZeroSeqComputation_k ) ; rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_l . B_23_2_0 . re , _rtB -> PosSeqComputation_l . B_23_2_0
. im ) ; rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_l
. B_23_2_0 . im , _rtB -> PosSeqComputation_l . B_23_2_0 . re ) ; _rtB ->
B_392_1355_0 = _rtX -> integrator_CSTATE_mz ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_1217 ; B_392_1356_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ij
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_ij . Last , _rtDW ->
TransportDelay_IWORK_ij . Tail , _rtDW -> TransportDelay_IWORK_ij . Head ,
_rtP -> P_1218 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1358_0 = _rtP -> P_1219 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1358_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1360_0 =
_rtDW -> Memory_PreviousInput_hj ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1362_0 = ( _rtB -> B_392_1355_0 - B_392_1356_0 ) * _rtP -> P_139 ; }
else { _rtB -> B_392_1362_0 = _rtB -> B_392_1360_0 ; } _rtB -> B_392_1363_0 =
_rtX -> integrator_CSTATE_fa ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_cf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cf . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1222 ;
B_392_1364_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ck .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ck . Last , _rtDW ->
TransportDelay_IWORK_ck . Tail , _rtDW -> TransportDelay_IWORK_ck . Head ,
_rtP -> P_1223 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1366_0 = _rtP -> P_1224 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1366_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1368_0 =
_rtDW -> Memory_PreviousInput_eg ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1370_0 = ( _rtB -> B_392_1363_0 - B_392_1364_0 ) * _rtP -> P_138 ; }
else { _rtB -> B_392_1370_0 = _rtB -> B_392_1368_0 ; } rtb_B_392_430_0 = _rtP
-> P_1226 * muDoubleScalarAtan2 ( _rtB -> B_392_1370_0 , _rtB -> B_392_1362_0
) * _rtP -> P_1227 ; B_392_1375_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1362_0 , _rtB -> B_392_1370_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_1375_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1362_0 , _rtB ->
B_392_1370_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_1376_0
= _rtX -> integrator_CSTATE_bq ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1229
; B_392_1377_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ok .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ok . Last , _rtDW ->
TransportDelay_IWORK_ok . Tail , _rtDW -> TransportDelay_IWORK_ok . Head ,
_rtP -> P_1230 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1379_0 = _rtP -> P_1231 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1379_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1381_0 =
_rtDW -> Memory_PreviousInput_dh ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1383_0 = ( _rtB -> B_392_1376_0 - B_392_1377_0 ) * _rtP -> P_141 ; }
else { _rtB -> B_392_1383_0 = _rtB -> B_392_1381_0 ; } _rtB -> B_392_1384_0 =
_rtX -> integrator_CSTATE_fc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1234 ;
B_392_1385_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fi .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_fi . Last , _rtDW ->
TransportDelay_IWORK_fi . Tail , _rtDW -> TransportDelay_IWORK_fi . Head ,
_rtP -> P_1235 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1387_0 = _rtP -> P_1236 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1387_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1389_0 =
_rtDW -> Memory_PreviousInput_mp ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1391_0 = ( _rtB -> B_392_1384_0 - B_392_1385_0 ) * _rtP -> P_140 ; }
else { _rtB -> B_392_1391_0 = _rtB -> B_392_1389_0 ; } rtb_B_392_430_0 = _rtP
-> P_1238 * muDoubleScalarAtan2 ( _rtB -> B_392_1391_0 , _rtB -> B_392_1383_0
) * _rtP -> P_1239 ; B_392_1396_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1383_0 , _rtB -> B_392_1391_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_1396_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1383_0 , _rtB ->
B_392_1391_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_1397_0
= _rtX -> integrator_CSTATE_nm ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1241
; B_392_1398_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ea .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ea . Last , _rtDW ->
TransportDelay_IWORK_ea . Tail , _rtDW -> TransportDelay_IWORK_ea . Head ,
_rtP -> P_1242 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1400_0 = _rtP -> P_1243 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1400_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1402_0 =
_rtDW -> Memory_PreviousInput_g ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1404_0 = ( _rtB -> B_392_1397_0 - B_392_1398_0 ) * _rtP -> P_143 ; }
else { _rtB -> B_392_1404_0 = _rtB -> B_392_1402_0 ; } _rtB -> B_392_1405_0 =
_rtX -> integrator_CSTATE_c1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1246 ;
B_392_1406_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_me .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_me . Last , _rtDW ->
TransportDelay_IWORK_me . Tail , _rtDW -> TransportDelay_IWORK_me . Head ,
_rtP -> P_1247 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1408_0 = _rtP -> P_1248 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1408_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1410_0 =
_rtDW -> Memory_PreviousInput_av3 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1412_0 = ( _rtB -> B_392_1405_0 - B_392_1406_0 ) * _rtP -> P_142 ; }
else { _rtB -> B_392_1412_0 = _rtB -> B_392_1410_0 ; } rtb_B_392_430_0 = _rtP
-> P_1250 * muDoubleScalarAtan2 ( _rtB -> B_392_1412_0 , _rtB -> B_392_1404_0
) * _rtP -> P_1251 ; B_392_1417_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1404_0 , _rtB -> B_392_1412_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_1417_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1404_0 , _rtB ->
B_392_1412_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1419_0 = _rtB
-> B_392_1418_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1419_0 , B_392_1375_0 , B_392_1396_0 , B_392_1417_0 , & _rtB ->
PosSeqComputation_m , & _rtDW -> PosSeqComputation_m , & _rtP ->
PosSeqComputation_m ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1422_0 = _rtB -> B_392_1421_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1422_0 ,
B_392_1375_0 , B_392_1396_0 , B_392_1417_0 , & _rtB -> NegSeqComputation_c ,
& _rtDW -> NegSeqComputation_c , & _rtP -> NegSeqComputation_c ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1425_0 = _rtB
-> B_392_1424_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_1425_0 , B_392_1375_0 , B_392_1396_0 , B_392_1417_0 , & _rtB ->
ZeroSeqComputation_f , & _rtDW -> ZeroSeqComputation_f , & _rtP ->
ZeroSeqComputation_f ) ; rtb_B_392_239_0 = rtb_B_392_4_1 *
muDoubleScalarHypot ( _rtB -> PosSeqComputation_m . B_23_2_0 . re , _rtB ->
PosSeqComputation_m . B_23_2_0 . im ) * _rtP -> P_1255 ; muDoubleScalarSinCos
( _rtP -> P_1258 * ( _rtP -> P_1256 * rtb_B_392_237_0 - _rtP -> P_1257 *
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_m . B_23_2_0 . im , _rtB ->
PosSeqComputation_m . B_23_2_0 . re ) ) , & rtb_B_392_1128_0 , &
rtb_B_392_430_0 ) ; rtb_B_392_430_0 = rtb_B_392_239_0 * rtb_B_392_430_0 *
_rtP -> P_1259 ; rtb_B_392_239_0 = rtb_B_392_239_0 * rtb_B_392_1128_0 * _rtP
-> P_1260 ; _rtB -> B_392_1439_0 = _rtX -> integrator_CSTATE_g2 ; { real_T *
* uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 . TUbufferPtrs
[ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_h45 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1262 ; B_392_1440_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_di . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_di . Last , _rtDW -> TransportDelay_IWORK_di .
Tail , _rtDW -> TransportDelay_IWORK_di . Head , _rtP -> P_1263 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1442_0 = _rtP -> P_1264 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1442_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1444_0 = _rtDW -> Memory_PreviousInput_fq ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1446_0 = ( _rtB -> B_392_1439_0 -
B_392_1440_0 ) * _rtP -> P_145 ; } else { _rtB -> B_392_1446_0 = _rtB ->
B_392_1444_0 ; } _rtB -> B_392_1447_0 = _rtX -> integrator_CSTATE_lbk ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_a . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ;
real_T tMinusDelay = simTime - _rtP -> P_1267 ; B_392_1448_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_n . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_n . Last , _rtDW -> TransportDelay_IWORK_n .
Tail , _rtDW -> TransportDelay_IWORK_n . Head , _rtP -> P_1268 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1450_0 = _rtP -> P_1269 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1450_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1452_0 = _rtDW -> Memory_PreviousInput_ffa ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_1454_0 = ( _rtB -> B_392_1447_0 -
B_392_1448_0 ) * _rtP -> P_144 ; } else { _rtB -> B_392_1454_0 = _rtB ->
B_392_1452_0 ; } rtb_B_392_1128_0 = _rtP -> P_1271 * muDoubleScalarAtan2 (
_rtB -> B_392_1454_0 , _rtB -> B_392_1446_0 ) * _rtP -> P_1272 ; B_392_1459_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1446_0 , _rtB -> B_392_1454_0 ) *
muDoubleScalarCos ( rtb_B_392_1128_0 ) ; B_392_1459_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1446_0 , _rtB -> B_392_1454_0 ) *
muDoubleScalarSin ( rtb_B_392_1128_0 ) ; _rtB -> B_392_1460_0 = _rtX ->
integrator_CSTATE_gw ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_om . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1274 ;
B_392_1461_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hj .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hj . Last , _rtDW ->
TransportDelay_IWORK_hj . Tail , _rtDW -> TransportDelay_IWORK_hj . Head ,
_rtP -> P_1275 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1463_0 = _rtP -> P_1276 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1463_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1465_0 =
_rtDW -> Memory_PreviousInput_fm ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1467_0 = ( _rtB -> B_392_1460_0 - B_392_1461_0 ) * _rtP -> P_147 ; }
else { _rtB -> B_392_1467_0 = _rtB -> B_392_1465_0 ; } _rtB -> B_392_1468_0 =
_rtX -> integrator_CSTATE_k1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_o5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1279 ;
B_392_1469_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oz .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_oz . Last , _rtDW ->
TransportDelay_IWORK_oz . Tail , _rtDW -> TransportDelay_IWORK_oz . Head ,
_rtP -> P_1280 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1471_0 = _rtP -> P_1281 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1471_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1473_0 =
_rtDW -> Memory_PreviousInput_fv ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1475_0 = ( _rtB -> B_392_1468_0 - B_392_1469_0 ) * _rtP -> P_146 ; }
else { _rtB -> B_392_1475_0 = _rtB -> B_392_1473_0 ; } rtb_B_392_1128_0 =
_rtP -> P_1283 * muDoubleScalarAtan2 ( _rtB -> B_392_1475_0 , _rtB ->
B_392_1467_0 ) * _rtP -> P_1284 ; B_392_1480_0 . re = muDoubleScalarHypot (
_rtB -> B_392_1467_0 , _rtB -> B_392_1475_0 ) * muDoubleScalarCos (
rtb_B_392_1128_0 ) ; B_392_1480_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_1467_0 , _rtB -> B_392_1475_0 ) * muDoubleScalarSin ( rtb_B_392_1128_0
) ; _rtB -> B_392_1481_0 = _rtX -> integrator_CSTATE_obo ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1286 ; B_392_1482_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fq . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_fq . Last , _rtDW -> TransportDelay_IWORK_fq .
Tail , _rtDW -> TransportDelay_IWORK_fq . Head , _rtP -> P_1287 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1484_0 = _rtP -> P_1288 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1484_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1486_0 = _rtDW -> Memory_PreviousInput_gx ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1488_0 = ( _rtB -> B_392_1481_0 -
B_392_1482_0 ) * _rtP -> P_149 ; } else { _rtB -> B_392_1488_0 = _rtB ->
B_392_1486_0 ; } _rtB -> B_392_1489_0 = _rtX -> integrator_CSTATE_md ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iz .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_iz . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1291 ; B_392_1490_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_g1 . Last , _rtDW -> TransportDelay_IWORK_g1 .
Tail , _rtDW -> TransportDelay_IWORK_g1 . Head , _rtP -> P_1292 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1492_0 = _rtP -> P_1293 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1492_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1494_0 = _rtDW -> Memory_PreviousInput_pc ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1496_0 = ( _rtB -> B_392_1489_0 -
B_392_1490_0 ) * _rtP -> P_148 ; } else { _rtB -> B_392_1496_0 = _rtB ->
B_392_1494_0 ; } rtb_B_392_1128_0 = _rtP -> P_1295 * muDoubleScalarAtan2 (
_rtB -> B_392_1496_0 , _rtB -> B_392_1488_0 ) * _rtP -> P_1296 ; B_392_1501_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1488_0 , _rtB -> B_392_1496_0 ) *
muDoubleScalarCos ( rtb_B_392_1128_0 ) ; B_392_1501_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1488_0 , _rtB -> B_392_1496_0 ) *
muDoubleScalarSin ( rtb_B_392_1128_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_1503_0 = _rtB -> B_392_1502_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1503_0 ,
B_392_1459_0 , B_392_1480_0 , B_392_1501_0 , & _rtB -> PosSeqComputation_i ,
& _rtDW -> PosSeqComputation_i , & _rtP -> PosSeqComputation_i ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1506_0 = _rtB
-> B_392_1505_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1506_0 , B_392_1459_0 , B_392_1480_0 , B_392_1501_0 , & _rtB ->
NegSeqComputation_mj , & _rtDW -> NegSeqComputation_mj , & _rtP ->
NegSeqComputation_mj ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_1509_0 = _rtB -> B_392_1508_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_1509_0 ,
B_392_1459_0 , B_392_1480_0 , B_392_1501_0 , & _rtB -> ZeroSeqComputation_d ,
& _rtDW -> ZeroSeqComputation_d , & _rtP -> ZeroSeqComputation_d ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_i .
B_23_2_0 . im , _rtB -> PosSeqComputation_i . B_23_2_0 . re ) ; _rtB ->
B_392_1512_0 [ 0 ] = rtb_B_392_430_0 ; _rtB -> B_392_1512_0 [ 1 ] =
rtb_B_392_239_0 ; _rtB -> B_392_1512_0 [ 2 ] = muDoubleScalarHypot ( _rtB ->
PosSeqComputation_i . B_23_2_0 . re , _rtB -> PosSeqComputation_i . B_23_2_0
. im ) ; _rtB -> B_392_1512_0 [ 3 ] = 0.0 ; ssCallAccelRunBlock ( S , 392 ,
1513 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1517_0 [ 0 ] = _rtP -> P_1300 * _rtB -> B_0_14_0
[ 105 ] * _rtP -> P_1303 ; _rtB -> B_392_1517_0 [ 1 ] = _rtP -> P_1301 * _rtB
-> B_0_14_0 [ 106 ] * _rtP -> P_1303 ; _rtB -> B_392_1517_0 [ 2 ] = _rtP ->
P_1302 * _rtB -> B_0_14_0 [ 107 ] * _rtP -> P_1303 ; _rtB -> B_392_1521_0 [ 0
] = _rtP -> P_1304 * _rtB -> B_0_14_0 [ 50 ] * _rtP -> P_1307 ; _rtB ->
B_392_1521_0 [ 1 ] = _rtP -> P_1305 * _rtB -> B_0_14_0 [ 51 ] * _rtP ->
P_1307 ; _rtB -> B_392_1521_0 [ 2 ] = _rtP -> P_1306 * _rtB -> B_0_14_0 [ 52
] * _rtP -> P_1307 ; } _rtB -> B_392_1557_0 = ( muDoubleScalarSin ( _rtP ->
P_1310 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1311 ) * _rtP -> P_1308 + _rtP
-> P_1309 ) * _rtB -> B_392_1521_0 [ 0 ] ; _rtB -> B_392_1559_0 = (
muDoubleScalarSin ( _rtP -> P_1314 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1315
) * _rtP -> P_1312 + _rtP -> P_1313 ) * _rtB -> B_392_1521_0 [ 0 ] ; _rtB ->
B_392_1561_0 = ( muDoubleScalarSin ( _rtP -> P_1318 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1319 ) * _rtP -> P_1316 + _rtP -> P_1317 ) * _rtB -> B_392_1521_0
[ 1 ] ; _rtB -> B_392_1563_0 = ( muDoubleScalarSin ( _rtP -> P_1322 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1323 ) * _rtP -> P_1320 + _rtP -> P_1321
) * _rtB -> B_392_1521_0 [ 1 ] ; _rtB -> B_392_1565_0 = ( muDoubleScalarSin (
_rtP -> P_1326 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1327 ) * _rtP -> P_1324
+ _rtP -> P_1325 ) * _rtB -> B_392_1521_0 [ 2 ] ; _rtB -> B_392_1567_0 = (
muDoubleScalarSin ( _rtP -> P_1330 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1331
) * _rtP -> P_1328 + _rtP -> P_1329 ) * _rtB -> B_392_1521_0 [ 2 ] ; _rtB ->
B_392_1569_0 = ( muDoubleScalarSin ( _rtP -> P_1334 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1335 ) * _rtP -> P_1332 + _rtP -> P_1333 ) * _rtB -> B_392_1517_0
[ 0 ] ; _rtB -> B_392_1571_0 = ( muDoubleScalarSin ( _rtP -> P_1338 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1339 ) * _rtP -> P_1336 + _rtP -> P_1337
) * _rtB -> B_392_1517_0 [ 0 ] ; _rtB -> B_392_1573_0 = ( muDoubleScalarSin (
_rtP -> P_1342 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1343 ) * _rtP -> P_1340
+ _rtP -> P_1341 ) * _rtB -> B_392_1517_0 [ 1 ] ; _rtB -> B_392_1575_0 = (
muDoubleScalarSin ( _rtP -> P_1346 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1347
) * _rtP -> P_1344 + _rtP -> P_1345 ) * _rtB -> B_392_1517_0 [ 1 ] ; _rtB ->
B_392_1577_0 = ( muDoubleScalarSin ( _rtP -> P_1350 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1351 ) * _rtP -> P_1348 + _rtP -> P_1349 ) * _rtB -> B_392_1517_0
[ 2 ] ; _rtB -> B_392_1579_0 = ( muDoubleScalarSin ( _rtP -> P_1354 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1355 ) * _rtP -> P_1352 + _rtP -> P_1353
) * _rtB -> B_392_1517_0 [ 2 ] ; _rtB -> B_392_1581_0 = ( muDoubleScalarSin (
_rtP -> P_1358 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1359 ) * _rtP -> P_1356
+ _rtP -> P_1357 ) * _rtB -> B_392_1521_0 [ 0 ] ; _rtB -> B_392_1583_0 = (
muDoubleScalarSin ( _rtP -> P_1362 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1363
) * _rtP -> P_1360 + _rtP -> P_1361 ) * _rtB -> B_392_1521_0 [ 0 ] ; _rtB ->
B_392_1585_0 = ( muDoubleScalarSin ( _rtP -> P_1366 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1367 ) * _rtP -> P_1364 + _rtP -> P_1365 ) * _rtB -> B_392_1521_0
[ 1 ] ; _rtB -> B_392_1587_0 = ( muDoubleScalarSin ( _rtP -> P_1370 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1371 ) * _rtP -> P_1368 + _rtP -> P_1369
) * _rtB -> B_392_1521_0 [ 1 ] ; _rtB -> B_392_1589_0 = ( muDoubleScalarSin (
_rtP -> P_1374 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1375 ) * _rtP -> P_1372
+ _rtP -> P_1373 ) * _rtB -> B_392_1521_0 [ 2 ] ; _rtB -> B_392_1591_0 = (
muDoubleScalarSin ( _rtP -> P_1378 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1379
) * _rtP -> P_1376 + _rtP -> P_1377 ) * _rtB -> B_392_1521_0 [ 2 ] ;
B_392_1592_0 = _rtP -> P_1380 * rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S ,
1 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_16_0 = ssGetTaskTime ( S , 1 ) ;
_rtDW -> IlhamentoChavefechadaparachaveabertaemtilha_MODE = ( rtb_B_392_16_0
>= _rtP -> P_1383 ) ; if ( _rtDW ->
IlhamentoChavefechadaparachaveabertaemtilha_MODE == 1 ) { _rtB ->
B_392_1595_0 = _rtP -> P_1385 ; } else { _rtB -> B_392_1595_0 = _rtP ->
P_1384 ; } if ( 0.0 > _rtP -> P_1386 ) { _rtB -> B_392_1596_0 = _rtB ->
B_392_1594_0 ; } else { _rtB -> B_392_1596_0 = _rtB -> B_392_1595_0 ; } }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_1599_0 = rt_Lookup ( _rtP -> P_1388 , 4 , ssGetTaskTime ( S , 2 ) ,
_rtP -> P_1389 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_1597_0 >= _rtP -> P_1390 ) { rtb_B_392_1600_0 = _rtB ->
B_392_1596_0 ; } else { rtb_B_392_1600_0 = _rtB -> B_392_1599_0 ; } if ( _rtB
-> B_392_1601_0 >= _rtP -> P_1393 ) { _rtB -> B_392_1604_0 = rtb_B_392_1600_0
; } else { _rtB -> B_392_1604_0 = _rtB -> B_392_1602_0 ; } } _rtB ->
B_392_1606_0 = rt_Lookup ( _rtP -> P_1394 , 5 , ssGetT ( S ) , _rtP -> P_1395
) ; if ( _rtB -> B_392_1593_0 >= _rtP -> P_1396 ) { _rtB -> B_392_1607_0 =
_rtB -> B_392_1604_0 ; } else { _rtB -> B_392_1607_0 = _rtB -> B_392_1606_0 ;
} if ( ( _rtDW -> TimeStampA >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB >=
ssGetT ( S ) ) ) { _rtB -> B_392_1608_0 = 0.0 ; } else { rtb_B_392_430_0 =
_rtDW -> TimeStampA ; lastU = & _rtDW -> LastUAtTimeA ; if ( _rtDW ->
TimeStampA < _rtDW -> TimeStampB ) { if ( _rtDW -> TimeStampB < ssGetT ( S )
) { rtb_B_392_430_0 = _rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ;
} } else { if ( _rtDW -> TimeStampA >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB ; lastU = & _rtDW -> LastUAtTimeB ; } } _rtB ->
B_392_1608_0 = ( _rtB -> B_392_1606_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_1614_0 >= _rtP -> P_1400 ) { _rtB -> B_392_1616_0 =
rtb_B_392_1600_0 ; } else { _rtB -> B_392_1616_0 = _rtB -> B_392_1602_0 ; } }
_rtB -> B_392_1618_0 = rt_Lookup ( _rtP -> P_1401 , 5 , ssGetT ( S ) , _rtP
-> P_1402 ) ; if ( _rtB -> B_392_1613_0 >= _rtP -> P_1403 ) { _rtB ->
B_392_1619_0 = _rtB -> B_392_1616_0 ; } else { _rtB -> B_392_1619_0 = _rtB ->
B_392_1618_0 ; } if ( ( _rtDW -> TimeStampA_n >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_i >= ssGetT ( S ) ) ) { _rtB -> B_392_1620_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_n ; lastU = & _rtDW -> LastUAtTimeA_j ;
if ( _rtDW -> TimeStampA_n < _rtDW -> TimeStampB_i ) { if ( _rtDW ->
TimeStampB_i < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_i ;
lastU = & _rtDW -> LastUAtTimeB_d ; } } else { if ( _rtDW -> TimeStampA_n >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_i ; lastU = & _rtDW ->
LastUAtTimeB_d ; } } _rtB -> B_392_1620_0 = ( _rtB -> B_392_1618_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_392_1626_0 >= _rtP -> P_1407 ) { _rtB
-> B_392_1628_0 = rtb_B_392_1600_0 ; } else { _rtB -> B_392_1628_0 = _rtB ->
B_392_1602_0 ; } } _rtB -> B_392_1630_0 = rt_Lookup ( _rtP -> P_1408 , 5 ,
ssGetT ( S ) , _rtP -> P_1409 ) ; if ( _rtB -> B_392_1625_0 >= _rtP -> P_1410
) { _rtB -> B_392_1631_0 = _rtB -> B_392_1628_0 ; } else { _rtB ->
B_392_1631_0 = _rtB -> B_392_1630_0 ; } if ( ( _rtDW -> TimeStampA_nq >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_k >= ssGetT ( S ) ) ) { _rtB ->
B_392_1632_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_nq ;
lastU = & _rtDW -> LastUAtTimeA_c ; if ( _rtDW -> TimeStampA_nq < _rtDW ->
TimeStampB_k ) { if ( _rtDW -> TimeStampB_k < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_k ; lastU = & _rtDW -> LastUAtTimeB_m ;
} } else { if ( _rtDW -> TimeStampA_nq >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_k ; lastU = & _rtDW -> LastUAtTimeB_m ; } } _rtB ->
B_392_1632_0 = ( _rtB -> B_392_1630_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 )
{ ssCallAccelRunBlock ( S , 392 , 1645 , SS_CALL_MDL_OUTPUTS ) ;
ssCallAccelRunBlock ( S , 392 , 1646 , SS_CALL_MDL_OUTPUTS ) ; rtb_B_392_16_0
= ssGetTaskTime ( S , 1 ) ; _rtDW -> Step_MODE = ( rtb_B_392_16_0 >= _rtP ->
P_1412 ) ; if ( _rtDW -> Step_MODE == 1 ) { _rtB -> B_392_1653_0 = _rtP ->
P_1414 ; } else { _rtB -> B_392_1653_0 = _rtP -> P_1413 ; } } _rtB ->
B_392_1659_0 = ( ssGetT ( S ) - _rtB -> B_392_1655_0 ) * _rtB -> B_392_1653_0
+ _rtB -> B_392_1658_0 ; ssCallAccelRunBlock ( S , 392 , 1660 ,
SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { rtb_B_392_1600_0 = _rtP -> P_1417 * _rtB -> B_0_14_0 [ 35 ] ;
rtb_B_392_430_0 = _rtP -> P_1418 * _rtB -> B_0_14_0 [ 36 ] ; rtb_B_392_239_0
= _rtP -> P_1419 * _rtB -> B_0_14_0 [ 37 ] ; B_392_1664_0 [ 0 ] = _rtP ->
P_1420 * rtb_B_392_1600_0 ; B_392_1664_0 [ 1 ] = _rtP -> P_1420 *
rtb_B_392_430_0 ; B_392_1664_0 [ 2 ] = _rtP -> P_1420 * rtb_B_392_239_0 ;
B_392_1665_0 [ 0 ] = _rtP -> P_1421 * rtb_B_392_1600_0 ; B_392_1665_0 [ 1 ] =
_rtP -> P_1421 * rtb_B_392_430_0 ; B_392_1665_0 [ 2 ] = _rtP -> P_1421 *
rtb_B_392_239_0 ; rtb_B_392_1600_0 = _rtP -> P_1422 * _rtB -> B_0_14_0 [ 32 ]
; rtb_B_392_430_0 = _rtP -> P_1423 * _rtB -> B_0_14_0 [ 33 ] ;
rtb_B_392_239_0 = _rtP -> P_1424 * _rtB -> B_0_14_0 [ 34 ] ; B_392_1693_0 [ 0
] = _rtP -> P_1425 * rtb_B_392_1600_0 ; B_392_1693_0 [ 1 ] = _rtP -> P_1425 *
rtb_B_392_430_0 ; B_392_1693_0 [ 2 ] = _rtP -> P_1425 * rtb_B_392_239_0 ;
B_392_1694_0 [ 0 ] = _rtP -> P_1426 * rtb_B_392_1600_0 ; B_392_1694_0 [ 1 ] =
_rtP -> P_1426 * rtb_B_392_430_0 ; B_392_1694_0 [ 2 ] = _rtP -> P_1426 *
rtb_B_392_239_0 ; } _rtB -> B_392_1719_0 = _rtX -> integrator_CSTATE_nq ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1428 ; B_392_1720_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hog . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hog . Last , _rtDW -> TransportDelay_IWORK_hog
. Tail , _rtDW -> TransportDelay_IWORK_hog . Head , _rtP -> P_1429 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1722_0 = _rtP -> P_1430 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1722_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1724_0 = _rtDW -> Memory_PreviousInput_pz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1726_0 = ( _rtB -> B_392_1719_0 -
B_392_1720_0 ) * _rtP -> P_181 ; } else { _rtB -> B_392_1726_0 = _rtB ->
B_392_1724_0 ; } _rtB -> B_392_1727_0 = _rtX -> integrator_CSTATE_fo ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ak .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ak . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1433 ; B_392_1728_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ju . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ju . Last , _rtDW -> TransportDelay_IWORK_ju .
Tail , _rtDW -> TransportDelay_IWORK_ju . Head , _rtP -> P_1434 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1730_0 = _rtP -> P_1435 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1730_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1732_0 = _rtDW -> Memory_PreviousInput_ft ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1734_0 = ( _rtB -> B_392_1727_0 -
B_392_1728_0 ) * _rtP -> P_180 ; } else { _rtB -> B_392_1734_0 = _rtB ->
B_392_1732_0 ; } rtb_B_392_1600_0 = _rtP -> P_1437 * muDoubleScalarAtan2 (
_rtB -> B_392_1734_0 , _rtB -> B_392_1726_0 ) * _rtP -> P_1438 ; B_392_1739_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1726_0 , _rtB -> B_392_1734_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_1739_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1726_0 , _rtB -> B_392_1734_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; _rtB -> B_392_1740_0 = _rtX ->
integrator_CSTATE_p5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_js . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_js . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1440 ;
B_392_1741_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a3 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_a3 . Last , _rtDW ->
TransportDelay_IWORK_a3 . Tail , _rtDW -> TransportDelay_IWORK_a3 . Head ,
_rtP -> P_1441 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1743_0 = _rtP -> P_1442 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1743_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1745_0 =
_rtDW -> Memory_PreviousInput_gi ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1747_0 = ( _rtB -> B_392_1740_0 - B_392_1741_0 ) * _rtP -> P_183 ; }
else { _rtB -> B_392_1747_0 = _rtB -> B_392_1745_0 ; } _rtB -> B_392_1748_0 =
_rtX -> integrator_CSTATE_ad ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_pp . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pp . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1445 ;
B_392_1749_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_at .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_at . Last , _rtDW ->
TransportDelay_IWORK_at . Tail , _rtDW -> TransportDelay_IWORK_at . Head ,
_rtP -> P_1446 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1751_0 = _rtP -> P_1447 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1751_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1753_0 =
_rtDW -> Memory_PreviousInput_hg ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1755_0 = ( _rtB -> B_392_1748_0 - B_392_1749_0 ) * _rtP -> P_182 ; }
else { _rtB -> B_392_1755_0 = _rtB -> B_392_1753_0 ; } rtb_B_392_1600_0 =
_rtP -> P_1449 * muDoubleScalarAtan2 ( _rtB -> B_392_1755_0 , _rtB ->
B_392_1747_0 ) * _rtP -> P_1450 ; B_392_1760_0 . re = muDoubleScalarHypot (
_rtB -> B_392_1747_0 , _rtB -> B_392_1755_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_1760_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_1747_0 , _rtB -> B_392_1755_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; _rtB -> B_392_1761_0 = _rtX -> integrator_CSTATE_cd ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1452 ; B_392_1762_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_h5 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_h5 . Last , _rtDW -> TransportDelay_IWORK_h5 .
Tail , _rtDW -> TransportDelay_IWORK_h5 . Head , _rtP -> P_1453 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1764_0 = _rtP -> P_1454 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1764_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1766_0 = _rtDW -> Memory_PreviousInput_dg ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1768_0 = ( _rtB -> B_392_1761_0 -
B_392_1762_0 ) * _rtP -> P_185 ; } else { _rtB -> B_392_1768_0 = _rtB ->
B_392_1766_0 ; } _rtB -> B_392_1769_0 = _rtX -> integrator_CSTATE_ds ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fp .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fp . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1457 ; B_392_1770_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cr . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_cr . Last , _rtDW -> TransportDelay_IWORK_cr .
Tail , _rtDW -> TransportDelay_IWORK_cr . Head , _rtP -> P_1458 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1772_0 = _rtP -> P_1459 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1772_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1774_0 = _rtDW -> Memory_PreviousInput_kp ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1776_0 = ( _rtB -> B_392_1769_0 -
B_392_1770_0 ) * _rtP -> P_184 ; } else { _rtB -> B_392_1776_0 = _rtB ->
B_392_1774_0 ; } rtb_B_392_1600_0 = _rtP -> P_1461 * muDoubleScalarAtan2 (
_rtB -> B_392_1776_0 , _rtB -> B_392_1768_0 ) * _rtP -> P_1462 ; B_392_1781_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1768_0 , _rtB -> B_392_1776_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_1781_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1768_0 , _rtB -> B_392_1776_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_1783_0 = _rtB -> B_392_1782_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1783_0 ,
B_392_1739_0 , B_392_1760_0 , B_392_1781_0 , & _rtB -> PosSeqComputation_aq ,
& _rtDW -> PosSeqComputation_aq , & _rtP -> PosSeqComputation_aq ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1786_0 = _rtB
-> B_392_1785_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1786_0 , B_392_1739_0 , B_392_1760_0 , B_392_1781_0 , & _rtB ->
NegSeqComputation_k , & _rtDW -> NegSeqComputation_k , & _rtP ->
NegSeqComputation_k ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1789_0 = _rtB -> B_392_1788_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_1789_0 ,
B_392_1739_0 , B_392_1760_0 , B_392_1781_0 , & _rtB -> ZeroSeqComputation_e ,
& _rtDW -> ZeroSeqComputation_e , & _rtP -> ZeroSeqComputation_e ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_aq . B_23_2_0
. re , _rtB -> PosSeqComputation_aq . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_aq . B_23_2_0 . im , _rtB ->
PosSeqComputation_aq . B_23_2_0 . re ) ; _rtB -> B_392_1792_0 = _rtX ->
integrator_CSTATE_ja ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1467 ;
B_392_1793_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dv . Last , _rtDW ->
TransportDelay_IWORK_dv . Tail , _rtDW -> TransportDelay_IWORK_dv . Head ,
_rtP -> P_1468 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1795_0 = _rtP -> P_1469 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1795_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1797_0 =
_rtDW -> Memory_PreviousInput_fg ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1799_0 = ( _rtB -> B_392_1792_0 - B_392_1793_0 ) * _rtP -> P_187 ; }
else { _rtB -> B_392_1799_0 = _rtB -> B_392_1797_0 ; } _rtB -> B_392_1800_0 =
_rtX -> integrator_CSTATE_an ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_o1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1472 ;
B_392_1801_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lv . Last , _rtDW ->
TransportDelay_IWORK_lv . Tail , _rtDW -> TransportDelay_IWORK_lv . Head ,
_rtP -> P_1473 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1803_0 = _rtP -> P_1474 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1803_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1805_0 =
_rtDW -> Memory_PreviousInput_bj ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1807_0 = ( _rtB -> B_392_1800_0 - B_392_1801_0 ) * _rtP -> P_186 ; }
else { _rtB -> B_392_1807_0 = _rtB -> B_392_1805_0 ; } rtb_B_392_1600_0 =
_rtP -> P_1476 * muDoubleScalarAtan2 ( _rtB -> B_392_1807_0 , _rtB ->
B_392_1799_0 ) * _rtP -> P_1477 ; B_392_1812_0 . re = muDoubleScalarHypot (
_rtB -> B_392_1799_0 , _rtB -> B_392_1807_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_1812_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_1799_0 , _rtB -> B_392_1807_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; _rtB -> B_392_1813_0 = _rtX -> integrator_CSTATE_d3 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1479 ; B_392_1814_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lu . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_lu . Last , _rtDW -> TransportDelay_IWORK_lu .
Tail , _rtDW -> TransportDelay_IWORK_lu . Head , _rtP -> P_1480 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1816_0 = _rtP -> P_1481 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1816_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1818_0 = _rtDW -> Memory_PreviousInput_df ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1820_0 = ( _rtB -> B_392_1813_0 -
B_392_1814_0 ) * _rtP -> P_189 ; } else { _rtB -> B_392_1820_0 = _rtB ->
B_392_1818_0 ; } _rtB -> B_392_1821_0 = _rtX -> integrator_CSTATE_fl ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_i5 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1484 ; B_392_1822_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kz . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kz . Last , _rtDW -> TransportDelay_IWORK_kz .
Tail , _rtDW -> TransportDelay_IWORK_kz . Head , _rtP -> P_1485 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1824_0 = _rtP -> P_1486 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1824_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1826_0 = _rtDW -> Memory_PreviousInput_bv ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1828_0 = ( _rtB -> B_392_1821_0 -
B_392_1822_0 ) * _rtP -> P_188 ; } else { _rtB -> B_392_1828_0 = _rtB ->
B_392_1826_0 ; } rtb_B_392_1600_0 = _rtP -> P_1488 * muDoubleScalarAtan2 (
_rtB -> B_392_1828_0 , _rtB -> B_392_1820_0 ) * _rtP -> P_1489 ; B_392_1833_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1820_0 , _rtB -> B_392_1828_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_1833_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1820_0 , _rtB -> B_392_1828_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; _rtB -> B_392_1834_0 = _rtX ->
integrator_CSTATE_m3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oa . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oa . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1491 ;
B_392_1835_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ft .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ft . Last , _rtDW ->
TransportDelay_IWORK_ft . Tail , _rtDW -> TransportDelay_IWORK_ft . Head ,
_rtP -> P_1492 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1837_0 = _rtP -> P_1493 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1837_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1839_0 =
_rtDW -> Memory_PreviousInput_eq ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1841_0 = ( _rtB -> B_392_1834_0 - B_392_1835_0 ) * _rtP -> P_191 ; }
else { _rtB -> B_392_1841_0 = _rtB -> B_392_1839_0 ; } _rtB -> B_392_1842_0 =
_rtX -> integrator_CSTATE_oo ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fd . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1496 ;
B_392_1843_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bc .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bc . Last , _rtDW ->
TransportDelay_IWORK_bc . Tail , _rtDW -> TransportDelay_IWORK_bc . Head ,
_rtP -> P_1497 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1845_0 = _rtP -> P_1498 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1845_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1847_0 =
_rtDW -> Memory_PreviousInput_iw ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1849_0 = ( _rtB -> B_392_1842_0 - B_392_1843_0 ) * _rtP -> P_190 ; }
else { _rtB -> B_392_1849_0 = _rtB -> B_392_1847_0 ; } rtb_B_392_1600_0 =
_rtP -> P_1500 * muDoubleScalarAtan2 ( _rtB -> B_392_1849_0 , _rtB ->
B_392_1841_0 ) * _rtP -> P_1501 ; B_392_1854_0 . re = muDoubleScalarHypot (
_rtB -> B_392_1841_0 , _rtB -> B_392_1849_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_1854_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_1841_0 , _rtB -> B_392_1849_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_1856_0 = _rtB -> B_392_1855_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1856_0 ,
B_392_1812_0 , B_392_1833_0 , B_392_1854_0 , & _rtB -> PosSeqComputation_e ,
& _rtDW -> PosSeqComputation_e , & _rtP -> PosSeqComputation_e ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1859_0 = _rtB
-> B_392_1858_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1859_0 , B_392_1812_0 , B_392_1833_0 , B_392_1854_0 , & _rtB ->
NegSeqComputation_ed , & _rtDW -> NegSeqComputation_ed , & _rtP ->
NegSeqComputation_ed ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_1862_0 = _rtB -> B_392_1861_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_1862_0 ,
B_392_1812_0 , B_392_1833_0 , B_392_1854_0 , & _rtB -> ZeroSeqComputation_l ,
& _rtDW -> ZeroSeqComputation_l , & _rtP -> ZeroSeqComputation_l ) ;
rtb_B_392_1600_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_e . B_23_2_0 . re , _rtB -> PosSeqComputation_e . B_23_2_0
. im ) * _rtP -> P_1505 ; muDoubleScalarSinCos ( _rtP -> P_1508 * ( _rtP ->
P_1506 * rtb_B_392_237_0 - _rtP -> P_1507 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_e . B_23_2_0 . im , _rtB -> PosSeqComputation_e . B_23_2_0
. re ) ) , & rtb_B_392_430_0 , & rtb_B_392_239_0 ) ; _rtB -> B_392_1872_0 =
rtb_B_392_1600_0 * rtb_B_392_239_0 ; _rtB -> B_392_1873_0 = _rtX ->
integrator_CSTATE_cf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ej . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ej . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1510 ;
B_392_1874_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_by .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_by . Last , _rtDW ->
TransportDelay_IWORK_by . Tail , _rtDW -> TransportDelay_IWORK_by . Head ,
_rtP -> P_1511 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1876_0 = _rtP -> P_1512 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1876_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1878_0 =
_rtDW -> Memory_PreviousInput_fqc ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1880_0 = ( _rtB -> B_392_1873_0 - B_392_1874_0 ) * _rtP -> P_193 ; }
else { _rtB -> B_392_1880_0 = _rtB -> B_392_1878_0 ; } _rtB -> B_392_1881_0 =
_rtX -> integrator_CSTATE_jq ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fb . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fb . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1515 ;
B_392_1882_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nm . Last , _rtDW ->
TransportDelay_IWORK_nm . Tail , _rtDW -> TransportDelay_IWORK_nm . Head ,
_rtP -> P_1516 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1884_0 = _rtP -> P_1517 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1884_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1886_0 =
_rtDW -> Memory_PreviousInput_bf ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1888_0 = ( _rtB -> B_392_1881_0 - B_392_1882_0 ) * _rtP -> P_192 ; }
else { _rtB -> B_392_1888_0 = _rtB -> B_392_1886_0 ; } rtb_B_392_239_0 = _rtP
-> P_1519 * muDoubleScalarAtan2 ( _rtB -> B_392_1888_0 , _rtB -> B_392_1880_0
) * _rtP -> P_1520 ; B_392_1893_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1880_0 , _rtB -> B_392_1888_0 ) * muDoubleScalarCos ( rtb_B_392_239_0 )
; B_392_1893_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1880_0 , _rtB ->
B_392_1888_0 ) * muDoubleScalarSin ( rtb_B_392_239_0 ) ; _rtB -> B_392_1894_0
= _rtX -> integrator_CSTATE_i ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1522 ;
B_392_1895_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_og .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_og . Last , _rtDW ->
TransportDelay_IWORK_og . Tail , _rtDW -> TransportDelay_IWORK_og . Head ,
_rtP -> P_1523 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1897_0 = _rtP -> P_1524 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1897_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1899_0 =
_rtDW -> Memory_PreviousInput_ib ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1901_0 = ( _rtB -> B_392_1894_0 - B_392_1895_0 ) * _rtP -> P_195 ; }
else { _rtB -> B_392_1901_0 = _rtB -> B_392_1899_0 ; } _rtB -> B_392_1902_0 =
_rtX -> integrator_CSTATE_l1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_op . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_op . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1527 ;
B_392_1903_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_he .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_he . Last , _rtDW ->
TransportDelay_IWORK_he . Tail , _rtDW -> TransportDelay_IWORK_he . Head ,
_rtP -> P_1528 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1905_0 = _rtP -> P_1529 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1905_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1907_0 =
_rtDW -> Memory_PreviousInput_ak ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1909_0 = ( _rtB -> B_392_1902_0 - B_392_1903_0 ) * _rtP -> P_194 ; }
else { _rtB -> B_392_1909_0 = _rtB -> B_392_1907_0 ; } rtb_B_392_239_0 = _rtP
-> P_1531 * muDoubleScalarAtan2 ( _rtB -> B_392_1909_0 , _rtB -> B_392_1901_0
) * _rtP -> P_1532 ; B_392_1914_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1901_0 , _rtB -> B_392_1909_0 ) * muDoubleScalarCos ( rtb_B_392_239_0 )
; B_392_1914_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1901_0 , _rtB ->
B_392_1909_0 ) * muDoubleScalarSin ( rtb_B_392_239_0 ) ; _rtB -> B_392_1915_0
= _rtX -> integrator_CSTATE_k1l ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_hk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_hk . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1534
; B_392_1916_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hl . Last , _rtDW ->
TransportDelay_IWORK_hl . Tail , _rtDW -> TransportDelay_IWORK_hl . Head ,
_rtP -> P_1535 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1918_0 = _rtP -> P_1536 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1918_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1920_0 =
_rtDW -> Memory_PreviousInput_bu ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1922_0 = ( _rtB -> B_392_1915_0 - B_392_1916_0 ) * _rtP -> P_197 ; }
else { _rtB -> B_392_1922_0 = _rtB -> B_392_1920_0 ; } _rtB -> B_392_1923_0 =
_rtX -> integrator_CSTATE_hs ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1539 ;
B_392_1924_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j1 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j1 . Last , _rtDW ->
TransportDelay_IWORK_j1 . Tail , _rtDW -> TransportDelay_IWORK_j1 . Head ,
_rtP -> P_1540 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1926_0 = _rtP -> P_1541 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1926_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1928_0 =
_rtDW -> Memory_PreviousInput_ih ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1930_0 = ( _rtB -> B_392_1923_0 - B_392_1924_0 ) * _rtP -> P_196 ; }
else { _rtB -> B_392_1930_0 = _rtB -> B_392_1928_0 ; } rtb_B_392_239_0 = _rtP
-> P_1543 * muDoubleScalarAtan2 ( _rtB -> B_392_1930_0 , _rtB -> B_392_1922_0
) * _rtP -> P_1544 ; B_392_1935_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_1922_0 , _rtB -> B_392_1930_0 ) * muDoubleScalarCos ( rtb_B_392_239_0 )
; B_392_1935_0 . im = muDoubleScalarHypot ( _rtB -> B_392_1922_0 , _rtB ->
B_392_1930_0 ) * muDoubleScalarSin ( rtb_B_392_239_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1937_0 = _rtB
-> B_392_1936_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_1937_0 , B_392_1893_0 , B_392_1914_0 , B_392_1935_0 , & _rtB ->
PosSeqComputation_h , & _rtDW -> PosSeqComputation_h , & _rtP ->
PosSeqComputation_h ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1940_0 = _rtB -> B_392_1939_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_1940_0 ,
B_392_1893_0 , B_392_1914_0 , B_392_1935_0 , & _rtB -> NegSeqComputation_o ,
& _rtDW -> NegSeqComputation_o , & _rtP -> NegSeqComputation_o ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1943_0 = _rtB
-> B_392_1942_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_1943_0 , B_392_1893_0 , B_392_1914_0 , B_392_1935_0 , & _rtB ->
ZeroSeqComputation_g , & _rtDW -> ZeroSeqComputation_g , & _rtP ->
ZeroSeqComputation_g ) ; rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_h . B_23_2_0 . im , _rtB -> PosSeqComputation_h . B_23_2_0
. re ) ; _rtB -> B_392_1946_0 = _rtP -> P_1548 * muDoubleScalarHypot ( _rtB
-> PosSeqComputation_h . B_23_2_0 . re , _rtB -> PosSeqComputation_h .
B_23_2_0 . im ) ; _rtB -> B_392_1947_0 = rtb_B_392_1600_0 * rtb_B_392_430_0 ;
_rtB -> B_392_1948_0 = _rtX -> integrator_CSTATE_en ; { real_T * * uBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1550 ; B_392_1949_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e2 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e2 . Last , _rtDW -> TransportDelay_IWORK_e2 .
Tail , _rtDW -> TransportDelay_IWORK_e2 . Head , _rtP -> P_1551 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1951_0 = _rtP -> P_1552 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1951_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1953_0 = _rtDW -> Memory_PreviousInput_de ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1955_0 = ( _rtB -> B_392_1948_0 -
B_392_1949_0 ) * _rtP -> P_175 ; } else { _rtB -> B_392_1955_0 = _rtB ->
B_392_1953_0 ; } _rtB -> B_392_1956_0 = _rtX -> integrator_CSTATE_iv ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fdx . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1555 ; B_392_1957_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_id . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_id . Last , _rtDW -> TransportDelay_IWORK_id .
Tail , _rtDW -> TransportDelay_IWORK_id . Head , _rtP -> P_1556 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1959_0 = _rtP -> P_1557 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1959_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1961_0 = _rtDW -> Memory_PreviousInput_h2 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1963_0 = ( _rtB -> B_392_1956_0 -
B_392_1957_0 ) * _rtP -> P_174 ; } else { _rtB -> B_392_1963_0 = _rtB ->
B_392_1961_0 ; } rtb_B_392_1600_0 = _rtP -> P_1559 * muDoubleScalarAtan2 (
_rtB -> B_392_1963_0 , _rtB -> B_392_1955_0 ) * _rtP -> P_1560 ; B_392_1968_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1955_0 , _rtB -> B_392_1963_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_1968_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1955_0 , _rtB -> B_392_1963_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; _rtB -> B_392_1969_0 = _rtX ->
integrator_CSTATE_hf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_lh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_lh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1562 ;
B_392_1970_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gr . Last , _rtDW ->
TransportDelay_IWORK_gr . Tail , _rtDW -> TransportDelay_IWORK_gr . Head ,
_rtP -> P_1563 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1972_0 = _rtP -> P_1564 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1972_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1974_0 =
_rtDW -> Memory_PreviousInput_mb ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1976_0 = ( _rtB -> B_392_1969_0 - B_392_1970_0 ) * _rtP -> P_177 ; }
else { _rtB -> B_392_1976_0 = _rtB -> B_392_1974_0 ; } _rtB -> B_392_1977_0 =
_rtX -> integrator_CSTATE_fj ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_nd . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nd . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1567 ;
B_392_1978_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dl . Last , _rtDW ->
TransportDelay_IWORK_dl . Tail , _rtDW -> TransportDelay_IWORK_dl . Head ,
_rtP -> P_1568 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1980_0 = _rtP -> P_1569 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_1980_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_1982_0 =
_rtDW -> Memory_PreviousInput_bz ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_1984_0 = ( _rtB -> B_392_1977_0 - B_392_1978_0 ) * _rtP -> P_176 ; }
else { _rtB -> B_392_1984_0 = _rtB -> B_392_1982_0 ; } rtb_B_392_1600_0 =
_rtP -> P_1571 * muDoubleScalarAtan2 ( _rtB -> B_392_1984_0 , _rtB ->
B_392_1976_0 ) * _rtP -> P_1572 ; B_392_1989_0 . re = muDoubleScalarHypot (
_rtB -> B_392_1976_0 , _rtB -> B_392_1984_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_1989_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_1976_0 , _rtB -> B_392_1984_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; _rtB -> B_392_1990_0 = _rtX -> integrator_CSTATE_ff ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1574 ; B_392_1991_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hx . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hx . Last , _rtDW -> TransportDelay_IWORK_hx .
Tail , _rtDW -> TransportDelay_IWORK_hx . Head , _rtP -> P_1575 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_1993_0 = _rtP -> P_1576 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_1993_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_1995_0 = _rtDW -> Memory_PreviousInput_eh ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_1997_0 = ( _rtB -> B_392_1990_0 -
B_392_1991_0 ) * _rtP -> P_179 ; } else { _rtB -> B_392_1997_0 = _rtB ->
B_392_1995_0 ; } _rtB -> B_392_1998_0 = _rtX -> integrator_CSTATE_er ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jf .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_jf . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1579 ; B_392_1999_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fw . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_fw . Last , _rtDW -> TransportDelay_IWORK_fw .
Tail , _rtDW -> TransportDelay_IWORK_fw . Head , _rtP -> P_1580 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2001_0 = _rtP -> P_1581 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2001_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2003_0 = _rtDW -> Memory_PreviousInput_n ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2005_0 = ( _rtB -> B_392_1998_0 -
B_392_1999_0 ) * _rtP -> P_178 ; } else { _rtB -> B_392_2005_0 = _rtB ->
B_392_2003_0 ; } rtb_B_392_1600_0 = _rtP -> P_1583 * muDoubleScalarAtan2 (
_rtB -> B_392_2005_0 , _rtB -> B_392_1997_0 ) * _rtP -> P_1584 ; B_392_2010_0
. re = muDoubleScalarHypot ( _rtB -> B_392_1997_0 , _rtB -> B_392_2005_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_2010_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_1997_0 , _rtB -> B_392_2005_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_2012_0 = _rtB -> B_392_2011_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2012_0 ,
B_392_1968_0 , B_392_1989_0 , B_392_2010_0 , & _rtB -> PosSeqComputation_ep ,
& _rtDW -> PosSeqComputation_ep , & _rtP -> PosSeqComputation_ep ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2015_0 = _rtB
-> B_392_2014_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2015_0 , B_392_1968_0 , B_392_1989_0 , B_392_2010_0 , & _rtB ->
NegSeqComputation_ms , & _rtDW -> NegSeqComputation_ms , & _rtP ->
NegSeqComputation_ms ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2018_0 = _rtB -> B_392_2017_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2018_0 ,
B_392_1968_0 , B_392_1989_0 , B_392_2010_0 , & _rtB -> ZeroSeqComputation_dq
, & _rtDW -> ZeroSeqComputation_dq , & _rtP -> ZeroSeqComputation_dq ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ep .
B_23_2_0 . im , _rtB -> PosSeqComputation_ep . B_23_2_0 . re ) ; _rtB ->
B_392_2021_0 = _rtP -> P_1588 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_ep . B_23_2_0 . re , _rtB -> PosSeqComputation_ep .
B_23_2_0 . im ) ; ssCallAccelRunBlock ( S , 392 , 2022 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_392_1512_0 [ 0 ] = _rtB -> B_392_1872_0 ; _rtB -> B_392_1512_0
[ 1 ] = _rtB -> B_392_1946_0 ; _rtB -> B_392_1512_0 [ 2 ] = _rtB ->
B_392_1947_0 ; _rtB -> B_392_1512_0 [ 3 ] = _rtB -> B_392_2021_0 ;
ssCallAccelRunBlock ( S , 392 , 2024 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2028_0 [ 0 ]
= _rtP -> P_1589 * _rtB -> B_0_14_0 [ 111 ] * _rtP -> P_1592 ; _rtB ->
B_392_2028_0 [ 1 ] = _rtP -> P_1590 * _rtB -> B_0_14_0 [ 112 ] * _rtP ->
P_1592 ; _rtB -> B_392_2028_0 [ 2 ] = _rtP -> P_1591 * _rtB -> B_0_14_0 [ 113
] * _rtP -> P_1592 ; _rtB -> B_392_2032_0 [ 0 ] = _rtP -> P_1593 * _rtB ->
B_0_14_0 [ 56 ] * _rtP -> P_1596 ; _rtB -> B_392_2032_0 [ 1 ] = _rtP ->
P_1594 * _rtB -> B_0_14_0 [ 57 ] * _rtP -> P_1596 ; _rtB -> B_392_2032_0 [ 2
] = _rtP -> P_1595 * _rtB -> B_0_14_0 [ 58 ] * _rtP -> P_1596 ; } _rtB ->
B_392_2068_0 = ( muDoubleScalarSin ( _rtP -> P_1599 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1600 ) * _rtP -> P_1597 + _rtP -> P_1598 ) * _rtB -> B_392_2028_0
[ 0 ] ; _rtB -> B_392_2070_0 = ( muDoubleScalarSin ( _rtP -> P_1603 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1604 ) * _rtP -> P_1601 + _rtP -> P_1602
) * _rtB -> B_392_2028_0 [ 0 ] ; _rtB -> B_392_2072_0 = ( muDoubleScalarSin (
_rtP -> P_1607 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1608 ) * _rtP -> P_1605
+ _rtP -> P_1606 ) * _rtB -> B_392_2028_0 [ 1 ] ; _rtB -> B_392_2074_0 = (
muDoubleScalarSin ( _rtP -> P_1611 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1612
) * _rtP -> P_1609 + _rtP -> P_1610 ) * _rtB -> B_392_2028_0 [ 1 ] ; _rtB ->
B_392_2076_0 = ( muDoubleScalarSin ( _rtP -> P_1615 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1616 ) * _rtP -> P_1613 + _rtP -> P_1614 ) * _rtB -> B_392_2028_0
[ 2 ] ; _rtB -> B_392_2078_0 = ( muDoubleScalarSin ( _rtP -> P_1619 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1620 ) * _rtP -> P_1617 + _rtP -> P_1618
) * _rtB -> B_392_2028_0 [ 2 ] ; B_392_2079_0 = _rtP -> P_1621 *
rtb_B_392_239_0 ; _rtB -> B_392_2081_0 = ( muDoubleScalarSin ( _rtP -> P_1624
* ssGetTaskTime ( S , 0 ) + _rtP -> P_1625 ) * _rtP -> P_1622 + _rtP ->
P_1623 ) * _rtB -> B_392_2032_0 [ 0 ] ; _rtB -> B_392_2083_0 = (
muDoubleScalarSin ( _rtP -> P_1628 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1629
) * _rtP -> P_1626 + _rtP -> P_1627 ) * _rtB -> B_392_2032_0 [ 0 ] ; _rtB ->
B_392_2085_0 = ( muDoubleScalarSin ( _rtP -> P_1632 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1633 ) * _rtP -> P_1630 + _rtP -> P_1631 ) * _rtB -> B_392_2032_0
[ 1 ] ; _rtB -> B_392_2087_0 = ( muDoubleScalarSin ( _rtP -> P_1636 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1637 ) * _rtP -> P_1634 + _rtP -> P_1635
) * _rtB -> B_392_2032_0 [ 1 ] ; _rtB -> B_392_2089_0 = ( muDoubleScalarSin (
_rtP -> P_1640 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1641 ) * _rtP -> P_1638
+ _rtP -> P_1639 ) * _rtB -> B_392_2032_0 [ 2 ] ; _rtB -> B_392_2091_0 = (
muDoubleScalarSin ( _rtP -> P_1644 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1645
) * _rtP -> P_1642 + _rtP -> P_1643 ) * _rtB -> B_392_2032_0 [ 2 ] ; _rtB ->
B_392_2093_0 = ( muDoubleScalarSin ( _rtP -> P_1648 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1649 ) * _rtP -> P_1646 + _rtP -> P_1647 ) * _rtB -> B_392_2028_0
[ 0 ] ; _rtB -> B_392_2095_0 = ( muDoubleScalarSin ( _rtP -> P_1652 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1653 ) * _rtP -> P_1650 + _rtP -> P_1651
) * _rtB -> B_392_2028_0 [ 0 ] ; _rtB -> B_392_2097_0 = ( muDoubleScalarSin (
_rtP -> P_1656 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1657 ) * _rtP -> P_1654
+ _rtP -> P_1655 ) * _rtB -> B_392_2028_0 [ 1 ] ; _rtB -> B_392_2099_0 = (
muDoubleScalarSin ( _rtP -> P_1660 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1661
) * _rtP -> P_1658 + _rtP -> P_1659 ) * _rtB -> B_392_2028_0 [ 1 ] ; _rtB ->
B_392_2101_0 = ( muDoubleScalarSin ( _rtP -> P_1664 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1665 ) * _rtP -> P_1662 + _rtP -> P_1663 ) * _rtB -> B_392_2028_0
[ 2 ] ; _rtB -> B_392_2103_0 = ( muDoubleScalarSin ( _rtP -> P_1668 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1669 ) * _rtP -> P_1666 + _rtP -> P_1667
) * _rtB -> B_392_2028_0 [ 2 ] ; _rtB -> B_392_2105_0 = ( muDoubleScalarSin (
_rtP -> P_1672 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1673 ) * _rtP -> P_1670
+ _rtP -> P_1671 ) * _rtB -> B_392_2032_0 [ 0 ] ; _rtB -> B_392_2107_0 = (
muDoubleScalarSin ( _rtP -> P_1676 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1677
) * _rtP -> P_1674 + _rtP -> P_1675 ) * _rtB -> B_392_2032_0 [ 0 ] ; _rtB ->
B_392_2109_0 = ( muDoubleScalarSin ( _rtP -> P_1680 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1681 ) * _rtP -> P_1678 + _rtP -> P_1679 ) * _rtB -> B_392_2032_0
[ 1 ] ; _rtB -> B_392_2111_0 = ( muDoubleScalarSin ( _rtP -> P_1684 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1685 ) * _rtP -> P_1682 + _rtP -> P_1683
) * _rtB -> B_392_2032_0 [ 1 ] ; _rtB -> B_392_2113_0 = ( muDoubleScalarSin (
_rtP -> P_1688 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1689 ) * _rtP -> P_1686
+ _rtP -> P_1687 ) * _rtB -> B_392_2032_0 [ 2 ] ; _rtB -> B_392_2115_0 = (
muDoubleScalarSin ( _rtP -> P_1692 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1693
) * _rtP -> P_1690 + _rtP -> P_1691 ) * _rtB -> B_392_2032_0 [ 2 ] ;
B_392_2116_0 = _rtP -> P_1694 * rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_2119_0 >= _rtP -> P_1700 ) {
rtb_B_392_2122_0 = _rtB -> B_392_2118_0 ; } else { rtb_B_392_2122_0 =
rt_Lookup ( _rtP -> P_1698 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_1699 ) ;
} if ( _rtB -> B_392_2123_0 >= _rtP -> P_1703 ) { _rtB -> B_392_2126_0 =
rtb_B_392_2122_0 ; } else { _rtB -> B_392_2126_0 = _rtB -> B_392_2124_0 ; } }
_rtB -> B_392_2128_0 = rt_Lookup ( _rtP -> P_1704 , 5 , ssGetT ( S ) , _rtP
-> P_1705 ) ; if ( _rtB -> B_392_2117_0 >= _rtP -> P_1706 ) { _rtB ->
B_392_2129_0 = _rtB -> B_392_2126_0 ; } else { _rtB -> B_392_2129_0 = _rtB ->
B_392_2128_0 ; } if ( ( _rtDW -> TimeStampA_b >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_e >= ssGetT ( S ) ) ) { _rtB -> B_392_2130_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_b ; lastU = & _rtDW -> LastUAtTimeA_h ;
if ( _rtDW -> TimeStampA_b < _rtDW -> TimeStampB_e ) { if ( _rtDW ->
TimeStampB_e < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_e ;
lastU = & _rtDW -> LastUAtTimeB_p ; } } else { if ( _rtDW -> TimeStampA_b >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_e ; lastU = & _rtDW ->
LastUAtTimeB_p ; } } _rtB -> B_392_2130_0 = ( _rtB -> B_392_2128_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_392_2136_0 >= _rtP -> P_1710 ) { _rtB
-> B_392_2138_0 = rtb_B_392_2122_0 ; } else { _rtB -> B_392_2138_0 = _rtB ->
B_392_2124_0 ; } } _rtB -> B_392_2140_0 = rt_Lookup ( _rtP -> P_1711 , 5 ,
ssGetT ( S ) , _rtP -> P_1712 ) ; if ( _rtB -> B_392_2135_0 >= _rtP -> P_1713
) { _rtB -> B_392_2141_0 = _rtB -> B_392_2138_0 ; } else { _rtB ->
B_392_2141_0 = _rtB -> B_392_2140_0 ; } if ( ( _rtDW -> TimeStampA_i >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_m >= ssGetT ( S ) ) ) { _rtB ->
B_392_2142_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_i ; lastU
= & _rtDW -> LastUAtTimeA_p ; if ( _rtDW -> TimeStampA_i < _rtDW ->
TimeStampB_m ) { if ( _rtDW -> TimeStampB_m < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_m ; lastU = & _rtDW -> LastUAtTimeB_h ;
} } else { if ( _rtDW -> TimeStampA_i >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_m ; lastU = & _rtDW -> LastUAtTimeB_h ; } } _rtB ->
B_392_2142_0 = ( _rtB -> B_392_2140_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_2148_0 >= _rtP -> P_1717 ) { _rtB -> B_392_2150_0 =
rtb_B_392_2122_0 ; } else { _rtB -> B_392_2150_0 = _rtB -> B_392_2124_0 ; } }
_rtB -> B_392_2152_0 = rt_Lookup ( _rtP -> P_1718 , 5 , ssGetT ( S ) , _rtP
-> P_1719 ) ; if ( _rtB -> B_392_2147_0 >= _rtP -> P_1720 ) { _rtB ->
B_392_2153_0 = _rtB -> B_392_2150_0 ; } else { _rtB -> B_392_2153_0 = _rtB ->
B_392_2152_0 ; } if ( ( _rtDW -> TimeStampA_h >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_id >= ssGetT ( S ) ) ) { _rtB -> B_392_2154_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_h ; lastU = & _rtDW -> LastUAtTimeA_cg
; if ( _rtDW -> TimeStampA_h < _rtDW -> TimeStampB_id ) { if ( _rtDW ->
TimeStampB_id < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_id ;
lastU = & _rtDW -> LastUAtTimeB_e ; } } else { if ( _rtDW -> TimeStampA_h >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_id ; lastU = & _rtDW
-> LastUAtTimeB_e ; } } _rtB -> B_392_2154_0 = ( _rtB -> B_392_2152_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } _rtB -> B_392_2171_0 = _rtX
-> integrator_CSTATE_pb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ph . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1723 ;
B_392_2172_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_be .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_be . Last , _rtDW ->
TransportDelay_IWORK_be . Tail , _rtDW -> TransportDelay_IWORK_be . Head ,
_rtP -> P_1724 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2174_0 = _rtP -> P_1725 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2174_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2176_0 =
_rtDW -> Memory_PreviousInput_kb ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2178_0 = ( _rtB -> B_392_2171_0 - B_392_2172_0 ) * _rtP -> P_157 ; }
else { _rtB -> B_392_2178_0 = _rtB -> B_392_2176_0 ; } _rtB -> B_392_2179_0 =
_rtX -> integrator_CSTATE_os ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_eq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1728 ;
B_392_2180_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_op .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_op . Last , _rtDW ->
TransportDelay_IWORK_op . Tail , _rtDW -> TransportDelay_IWORK_op . Head ,
_rtP -> P_1729 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2182_0 = _rtP -> P_1730 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2182_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2184_0 =
_rtDW -> Memory_PreviousInput_hs ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2186_0 = ( _rtB -> B_392_2179_0 - B_392_2180_0 ) * _rtP -> P_156 ; }
else { _rtB -> B_392_2186_0 = _rtB -> B_392_2184_0 ; } rtb_B_392_2122_0 =
_rtP -> P_1732 * muDoubleScalarAtan2 ( _rtB -> B_392_2186_0 , _rtB ->
B_392_2178_0 ) * _rtP -> P_1733 ; B_392_2191_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2178_0 , _rtB -> B_392_2186_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2191_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2178_0 , _rtB -> B_392_2186_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2192_0 = _rtX -> integrator_CSTATE_ju ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1735 ; B_392_2193_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ma . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ma . Last , _rtDW -> TransportDelay_IWORK_ma .
Tail , _rtDW -> TransportDelay_IWORK_ma . Head , _rtP -> P_1736 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2195_0 = _rtP -> P_1737 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2195_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2197_0 = _rtDW -> Memory_PreviousInput_dv ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2199_0 = ( _rtB -> B_392_2192_0 -
B_392_2193_0 ) * _rtP -> P_159 ; } else { _rtB -> B_392_2199_0 = _rtB ->
B_392_2197_0 ; } _rtB -> B_392_2200_0 = _rtX -> integrator_CSTATE_a5 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_m0c . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1740 ; B_392_2201_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ov . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ov . Last , _rtDW -> TransportDelay_IWORK_ov .
Tail , _rtDW -> TransportDelay_IWORK_ov . Head , _rtP -> P_1741 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2203_0 = _rtP -> P_1742 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2203_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2205_0 = _rtDW -> Memory_PreviousInput_gg ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2207_0 = ( _rtB -> B_392_2200_0 -
B_392_2201_0 ) * _rtP -> P_158 ; } else { _rtB -> B_392_2207_0 = _rtB ->
B_392_2205_0 ; } rtb_B_392_2122_0 = _rtP -> P_1744 * muDoubleScalarAtan2 (
_rtB -> B_392_2207_0 , _rtB -> B_392_2199_0 ) * _rtP -> P_1745 ; B_392_2212_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2199_0 , _rtB -> B_392_2207_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2212_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2199_0 , _rtB -> B_392_2207_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2213_0 = _rtX ->
integrator_CSTATE_pbj ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_do . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_do . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1747 ;
B_392_2214_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hr . Last , _rtDW ->
TransportDelay_IWORK_hr . Tail , _rtDW -> TransportDelay_IWORK_hr . Head ,
_rtP -> P_1748 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2216_0 = _rtP -> P_1749 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2216_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2218_0 =
_rtDW -> Memory_PreviousInput_nb ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2220_0 = ( _rtB -> B_392_2213_0 - B_392_2214_0 ) * _rtP -> P_161 ; }
else { _rtB -> B_392_2220_0 = _rtB -> B_392_2218_0 ; } _rtB -> B_392_2221_0 =
_rtX -> integrator_CSTATE_mu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_hi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1752 ;
B_392_2222_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_as .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_as . Last , _rtDW ->
TransportDelay_IWORK_as . Tail , _rtDW -> TransportDelay_IWORK_as . Head ,
_rtP -> P_1753 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2224_0 = _rtP -> P_1754 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2224_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2226_0 =
_rtDW -> Memory_PreviousInput_l1 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2228_0 = ( _rtB -> B_392_2221_0 - B_392_2222_0 ) * _rtP -> P_160 ; }
else { _rtB -> B_392_2228_0 = _rtB -> B_392_2226_0 ; } rtb_B_392_2122_0 =
_rtP -> P_1756 * muDoubleScalarAtan2 ( _rtB -> B_392_2228_0 , _rtB ->
B_392_2220_0 ) * _rtP -> P_1757 ; B_392_2233_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2220_0 , _rtB -> B_392_2228_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2233_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2220_0 , _rtB -> B_392_2228_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_2235_0 = _rtB -> B_392_2234_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2235_0 ,
B_392_2191_0 , B_392_2212_0 , B_392_2233_0 , & _rtB -> PosSeqComputation_k ,
& _rtDW -> PosSeqComputation_k , & _rtP -> PosSeqComputation_k ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2238_0 = _rtB
-> B_392_2237_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2238_0 , B_392_2191_0 , B_392_2212_0 , B_392_2233_0 , & _rtB ->
NegSeqComputation_l , & _rtDW -> NegSeqComputation_l , & _rtP ->
NegSeqComputation_l ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2241_0 = _rtB -> B_392_2240_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2241_0 ,
B_392_2191_0 , B_392_2212_0 , B_392_2233_0 , & _rtB -> ZeroSeqComputation_m ,
& _rtDW -> ZeroSeqComputation_m , & _rtP -> ZeroSeqComputation_m ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_k . B_23_2_0
. re , _rtB -> PosSeqComputation_k . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_k . B_23_2_0 . im , _rtB ->
PosSeqComputation_k . B_23_2_0 . re ) ; _rtB -> B_392_2244_0 = _rtX ->
integrator_CSTATE_gd ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cv . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cv . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1762 ;
B_392_2245_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_iw .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_iw . Last , _rtDW ->
TransportDelay_IWORK_iw . Tail , _rtDW -> TransportDelay_IWORK_iw . Head ,
_rtP -> P_1763 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2247_0 = _rtP -> P_1764 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2247_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2249_0 =
_rtDW -> Memory_PreviousInput_ja ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2251_0 = ( _rtB -> B_392_2244_0 - B_392_2245_0 ) * _rtP -> P_163 ; }
else { _rtB -> B_392_2251_0 = _rtB -> B_392_2249_0 ; } _rtB -> B_392_2252_0 =
_rtX -> integrator_CSTATE_l5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ai . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ai . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1767 ;
B_392_2253_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dq . Last , _rtDW ->
TransportDelay_IWORK_dq . Tail , _rtDW -> TransportDelay_IWORK_dq . Head ,
_rtP -> P_1768 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2255_0 = _rtP -> P_1769 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2255_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2257_0 =
_rtDW -> Memory_PreviousInput_gr ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2259_0 = ( _rtB -> B_392_2252_0 - B_392_2253_0 ) * _rtP -> P_162 ; }
else { _rtB -> B_392_2259_0 = _rtB -> B_392_2257_0 ; } rtb_B_392_2122_0 =
_rtP -> P_1771 * muDoubleScalarAtan2 ( _rtB -> B_392_2259_0 , _rtB ->
B_392_2251_0 ) * _rtP -> P_1772 ; B_392_2264_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2251_0 , _rtB -> B_392_2259_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2264_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2251_0 , _rtB -> B_392_2259_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2265_0 = _rtX -> integrator_CSTATE_hq ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1774 ; B_392_2266_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_os . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_os . Last , _rtDW -> TransportDelay_IWORK_os .
Tail , _rtDW -> TransportDelay_IWORK_os . Head , _rtP -> P_1775 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2268_0 = _rtP -> P_1776 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2268_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2270_0 = _rtDW -> Memory_PreviousInput_c ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2272_0 = ( _rtB -> B_392_2265_0 -
B_392_2266_0 ) * _rtP -> P_165 ; } else { _rtB -> B_392_2272_0 = _rtB ->
B_392_2270_0 ; } _rtB -> B_392_2273_0 = _rtX -> integrator_CSTATE_cb ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nvu . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1779 ; B_392_2274_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pi . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_pi . Last , _rtDW -> TransportDelay_IWORK_pi .
Tail , _rtDW -> TransportDelay_IWORK_pi . Head , _rtP -> P_1780 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2276_0 = _rtP -> P_1781 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2276_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2278_0 = _rtDW -> Memory_PreviousInput_mt ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2280_0 = ( _rtB -> B_392_2273_0 -
B_392_2274_0 ) * _rtP -> P_164 ; } else { _rtB -> B_392_2280_0 = _rtB ->
B_392_2278_0 ; } rtb_B_392_2122_0 = _rtP -> P_1783 * muDoubleScalarAtan2 (
_rtB -> B_392_2280_0 , _rtB -> B_392_2272_0 ) * _rtP -> P_1784 ; B_392_2285_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2272_0 , _rtB -> B_392_2280_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2285_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2272_0 , _rtB -> B_392_2280_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2286_0 = _rtX ->
integrator_CSTATE_po ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_mh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_mh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1786 ;
B_392_2287_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jo .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jo . Last , _rtDW ->
TransportDelay_IWORK_jo . Tail , _rtDW -> TransportDelay_IWORK_jo . Head ,
_rtP -> P_1787 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2289_0 = _rtP -> P_1788 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2289_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2291_0 =
_rtDW -> Memory_PreviousInput_nr ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2293_0 = ( _rtB -> B_392_2286_0 - B_392_2287_0 ) * _rtP -> P_167 ; }
else { _rtB -> B_392_2293_0 = _rtB -> B_392_2291_0 ; } _rtB -> B_392_2294_0 =
_rtX -> integrator_CSTATE_ag ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_my . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_my . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1791 ;
B_392_2295_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_i2 . Last , _rtDW ->
TransportDelay_IWORK_i2 . Tail , _rtDW -> TransportDelay_IWORK_i2 . Head ,
_rtP -> P_1792 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2297_0 = _rtP -> P_1793 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2297_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2299_0 =
_rtDW -> Memory_PreviousInput_jr ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2301_0 = ( _rtB -> B_392_2294_0 - B_392_2295_0 ) * _rtP -> P_166 ; }
else { _rtB -> B_392_2301_0 = _rtB -> B_392_2299_0 ; } rtb_B_392_2122_0 =
_rtP -> P_1795 * muDoubleScalarAtan2 ( _rtB -> B_392_2301_0 , _rtB ->
B_392_2293_0 ) * _rtP -> P_1796 ; B_392_2306_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2293_0 , _rtB -> B_392_2301_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2306_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2293_0 , _rtB -> B_392_2301_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_2308_0 = _rtB -> B_392_2307_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2308_0 ,
B_392_2264_0 , B_392_2285_0 , B_392_2306_0 , & _rtB -> PosSeqComputation_c ,
& _rtDW -> PosSeqComputation_c , & _rtP -> PosSeqComputation_c ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2311_0 = _rtB
-> B_392_2310_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2311_0 , B_392_2264_0 , B_392_2285_0 , B_392_2306_0 , & _rtB ->
NegSeqComputation_lv , & _rtDW -> NegSeqComputation_lv , & _rtP ->
NegSeqComputation_lv ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2314_0 = _rtB -> B_392_2313_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2314_0 ,
B_392_2264_0 , B_392_2285_0 , B_392_2306_0 , & _rtB -> ZeroSeqComputation_mr
, & _rtDW -> ZeroSeqComputation_mr , & _rtP -> ZeroSeqComputation_mr ) ;
rtb_B_392_2122_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_c . B_23_2_0 . re , _rtB -> PosSeqComputation_c . B_23_2_0
. im ) * _rtP -> P_1800 ; muDoubleScalarSinCos ( _rtP -> P_1803 * ( _rtP ->
P_1801 * rtb_B_392_237_0 - _rtP -> P_1802 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_c . B_23_2_0 . im , _rtB -> PosSeqComputation_c . B_23_2_0
. re ) ) , & rtb_B_392_1600_0 , & rtb_B_392_430_0 ) ; _rtB -> B_392_2324_0 =
rtb_B_392_2122_0 * rtb_B_392_430_0 ; _rtB -> B_392_2325_0 = _rtX ->
integrator_CSTATE_ba ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cct . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cct . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1805 ;
B_392_2326_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pd . Last , _rtDW ->
TransportDelay_IWORK_pd . Tail , _rtDW -> TransportDelay_IWORK_pd . Head ,
_rtP -> P_1806 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2328_0 = _rtP -> P_1807 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2328_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2330_0 =
_rtDW -> Memory_PreviousInput_ex ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2332_0 = ( _rtB -> B_392_2325_0 - B_392_2326_0 ) * _rtP -> P_169 ; }
else { _rtB -> B_392_2332_0 = _rtB -> B_392_2330_0 ; } _rtB -> B_392_2333_0 =
_rtX -> integrator_CSTATE_j4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1810 ;
B_392_2334_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_n2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_n2 . Last , _rtDW ->
TransportDelay_IWORK_n2 . Tail , _rtDW -> TransportDelay_IWORK_n2 . Head ,
_rtP -> P_1811 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2336_0 = _rtP -> P_1812 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2336_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2338_0 =
_rtDW -> Memory_PreviousInput_aa ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2340_0 = ( _rtB -> B_392_2333_0 - B_392_2334_0 ) * _rtP -> P_168 ; }
else { _rtB -> B_392_2340_0 = _rtB -> B_392_2338_0 ; } rtb_B_392_430_0 = _rtP
-> P_1814 * muDoubleScalarAtan2 ( _rtB -> B_392_2340_0 , _rtB -> B_392_2332_0
) * _rtP -> P_1815 ; B_392_2345_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2332_0 , _rtB -> B_392_2340_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2345_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2332_0 , _rtB ->
B_392_2340_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_2346_0
= _rtX -> integrator_CSTATE_oi ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_du . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_du . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1817
; B_392_2347_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lc .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lc . Last , _rtDW ->
TransportDelay_IWORK_lc . Tail , _rtDW -> TransportDelay_IWORK_lc . Head ,
_rtP -> P_1818 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2349_0 = _rtP -> P_1819 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2349_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2351_0 =
_rtDW -> Memory_PreviousInput_bc ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2353_0 = ( _rtB -> B_392_2346_0 - B_392_2347_0 ) * _rtP -> P_171 ; }
else { _rtB -> B_392_2353_0 = _rtB -> B_392_2351_0 ; } _rtB -> B_392_2354_0 =
_rtX -> integrator_CSTATE_gb ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_oj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1822 ;
B_392_2355_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_el .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_el . Last , _rtDW ->
TransportDelay_IWORK_el . Tail , _rtDW -> TransportDelay_IWORK_el . Head ,
_rtP -> P_1823 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2357_0 = _rtP -> P_1824 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2357_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2359_0 =
_rtDW -> Memory_PreviousInput_mo ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2361_0 = ( _rtB -> B_392_2354_0 - B_392_2355_0 ) * _rtP -> P_170 ; }
else { _rtB -> B_392_2361_0 = _rtB -> B_392_2359_0 ; } rtb_B_392_430_0 = _rtP
-> P_1826 * muDoubleScalarAtan2 ( _rtB -> B_392_2361_0 , _rtB -> B_392_2353_0
) * _rtP -> P_1827 ; B_392_2366_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2353_0 , _rtB -> B_392_2361_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2366_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2353_0 , _rtB ->
B_392_2361_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_2367_0
= _rtX -> integrator_CSTATE_kn ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1829
; B_392_2368_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jd . Last , _rtDW ->
TransportDelay_IWORK_jd . Tail , _rtDW -> TransportDelay_IWORK_jd . Head ,
_rtP -> P_1830 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2370_0 = _rtP -> P_1831 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2370_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2372_0 =
_rtDW -> Memory_PreviousInput_ct ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2374_0 = ( _rtB -> B_392_2367_0 - B_392_2368_0 ) * _rtP -> P_173 ; }
else { _rtB -> B_392_2374_0 = _rtB -> B_392_2372_0 ; } _rtB -> B_392_2375_0 =
_rtX -> integrator_CSTATE_dn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_na . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_na . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1834 ;
B_392_2376_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_md .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_md . Last , _rtDW ->
TransportDelay_IWORK_md . Tail , _rtDW -> TransportDelay_IWORK_md . Head ,
_rtP -> P_1835 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2378_0 = _rtP -> P_1836 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2378_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2380_0 =
_rtDW -> Memory_PreviousInput_ad ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2382_0 = ( _rtB -> B_392_2375_0 - B_392_2376_0 ) * _rtP -> P_172 ; }
else { _rtB -> B_392_2382_0 = _rtB -> B_392_2380_0 ; } rtb_B_392_430_0 = _rtP
-> P_1838 * muDoubleScalarAtan2 ( _rtB -> B_392_2382_0 , _rtB -> B_392_2374_0
) * _rtP -> P_1839 ; B_392_2387_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2374_0 , _rtB -> B_392_2382_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2387_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2374_0 , _rtB ->
B_392_2382_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2389_0 = _rtB
-> B_392_2388_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2389_0 , B_392_2345_0 , B_392_2366_0 , B_392_2387_0 , & _rtB ->
PosSeqComputation_n , & _rtDW -> PosSeqComputation_n , & _rtP ->
PosSeqComputation_n ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2392_0 = _rtB -> B_392_2391_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2392_0 ,
B_392_2345_0 , B_392_2366_0 , B_392_2387_0 , & _rtB -> NegSeqComputation_a ,
& _rtDW -> NegSeqComputation_a , & _rtP -> NegSeqComputation_a ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2395_0 = _rtB
-> B_392_2394_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_2395_0 , B_392_2345_0 , B_392_2366_0 , B_392_2387_0 , & _rtB ->
ZeroSeqComputation_c , & _rtDW -> ZeroSeqComputation_c , & _rtP ->
ZeroSeqComputation_c ) ; rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_n . B_23_2_0 . im , _rtB -> PosSeqComputation_n . B_23_2_0
. re ) ; _rtB -> B_392_2398_0 = _rtP -> P_1843 * muDoubleScalarHypot ( _rtB
-> PosSeqComputation_n . B_23_2_0 . re , _rtB -> PosSeqComputation_n .
B_23_2_0 . im ) ; _rtB -> B_392_2399_0 = rtb_B_392_2122_0 * rtb_B_392_1600_0
; _rtB -> B_392_2400_0 = _rtX -> integrator_CSTATE_bd ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1845 ; B_392_2401_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fqe . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_fqe . Last , _rtDW -> TransportDelay_IWORK_fqe
. Tail , _rtDW -> TransportDelay_IWORK_fqe . Head , _rtP -> P_1846 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2403_0 = _rtP -> P_1847 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2403_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2405_0 = _rtDW -> Memory_PreviousInput_ow ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2407_0 = ( _rtB -> B_392_2400_0 -
B_392_2401_0 ) * _rtP -> P_151 ; } else { _rtB -> B_392_2407_0 = _rtB ->
B_392_2405_0 ; } _rtB -> B_392_2408_0 = _rtX -> integrator_CSTATE_mf ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fv .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fv . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1850 ; B_392_2409_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m2 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_m2 . Last , _rtDW -> TransportDelay_IWORK_m2 .
Tail , _rtDW -> TransportDelay_IWORK_m2 . Head , _rtP -> P_1851 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2411_0 = _rtP -> P_1852 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2411_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2413_0 = _rtDW -> Memory_PreviousInput_dp ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2415_0 = ( _rtB -> B_392_2408_0 -
B_392_2409_0 ) * _rtP -> P_150 ; } else { _rtB -> B_392_2415_0 = _rtB ->
B_392_2413_0 ; } rtb_B_392_2122_0 = _rtP -> P_1854 * muDoubleScalarAtan2 (
_rtB -> B_392_2415_0 , _rtB -> B_392_2407_0 ) * _rtP -> P_1855 ; B_392_2420_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2407_0 , _rtB -> B_392_2415_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2420_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2407_0 , _rtB -> B_392_2415_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2421_0 = _rtX ->
integrator_CSTATE_mt ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_nq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_nq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1857 ;
B_392_2422_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ko .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ko . Last , _rtDW ->
TransportDelay_IWORK_ko . Tail , _rtDW -> TransportDelay_IWORK_ko . Head ,
_rtP -> P_1858 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2424_0 = _rtP -> P_1859 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2424_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2426_0 =
_rtDW -> Memory_PreviousInput_a0 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2428_0 = ( _rtB -> B_392_2421_0 - B_392_2422_0 ) * _rtP -> P_153 ; }
else { _rtB -> B_392_2428_0 = _rtB -> B_392_2426_0 ; } _rtB -> B_392_2429_0 =
_rtX -> integrator_CSTATE_jq4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_cw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1862 ;
B_392_2430_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ld .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ld . Last , _rtDW ->
TransportDelay_IWORK_ld . Tail , _rtDW -> TransportDelay_IWORK_ld . Head ,
_rtP -> P_1863 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2432_0 = _rtP -> P_1864 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2432_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2434_0 =
_rtDW -> Memory_PreviousInput_lf ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2436_0 = ( _rtB -> B_392_2429_0 - B_392_2430_0 ) * _rtP -> P_152 ; }
else { _rtB -> B_392_2436_0 = _rtB -> B_392_2434_0 ; } rtb_B_392_2122_0 =
_rtP -> P_1866 * muDoubleScalarAtan2 ( _rtB -> B_392_2436_0 , _rtB ->
B_392_2428_0 ) * _rtP -> P_1867 ; B_392_2441_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2428_0 , _rtB -> B_392_2436_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2441_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2428_0 , _rtB -> B_392_2436_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2442_0 = _rtX -> integrator_CSTATE_ez ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_1869 ; B_392_2443_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_al . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_al . Last , _rtDW -> TransportDelay_IWORK_al .
Tail , _rtDW -> TransportDelay_IWORK_al . Head , _rtP -> P_1870 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2445_0 = _rtP -> P_1871 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2445_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2447_0 = _rtDW -> Memory_PreviousInput_ce ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2449_0 = ( _rtB -> B_392_2442_0 -
B_392_2443_0 ) * _rtP -> P_155 ; } else { _rtB -> B_392_2449_0 = _rtB ->
B_392_2447_0 ; } _rtB -> B_392_2450_0 = _rtX -> integrator_CSTATE_ct ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_1874 ; B_392_2451_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jdp . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_jdp . Last , _rtDW -> TransportDelay_IWORK_jdp
. Tail , _rtDW -> TransportDelay_IWORK_jdp . Head , _rtP -> P_1875 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2453_0 = _rtP -> P_1876 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2453_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2455_0 = _rtDW -> Memory_PreviousInput_ie ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2457_0 = ( _rtB -> B_392_2450_0 -
B_392_2451_0 ) * _rtP -> P_154 ; } else { _rtB -> B_392_2457_0 = _rtB ->
B_392_2455_0 ; } rtb_B_392_2122_0 = _rtP -> P_1878 * muDoubleScalarAtan2 (
_rtB -> B_392_2457_0 , _rtB -> B_392_2449_0 ) * _rtP -> P_1879 ; B_392_2462_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2449_0 , _rtB -> B_392_2457_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2462_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2449_0 , _rtB -> B_392_2457_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_2464_0 = _rtB -> B_392_2463_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2464_0 ,
B_392_2420_0 , B_392_2441_0 , B_392_2462_0 , & _rtB -> PosSeqComputation_em ,
& _rtDW -> PosSeqComputation_em , & _rtP -> PosSeqComputation_em ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2467_0 = _rtB
-> B_392_2466_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2467_0 , B_392_2420_0 , B_392_2441_0 , B_392_2462_0 , & _rtB ->
NegSeqComputation_g , & _rtDW -> NegSeqComputation_g , & _rtP ->
NegSeqComputation_g ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2470_0 = _rtB -> B_392_2469_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2470_0 ,
B_392_2420_0 , B_392_2441_0 , B_392_2462_0 , & _rtB -> ZeroSeqComputation_cx
, & _rtDW -> ZeroSeqComputation_cx , & _rtP -> ZeroSeqComputation_cx ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_em .
B_23_2_0 . im , _rtB -> PosSeqComputation_em . B_23_2_0 . re ) ; _rtB ->
B_392_2473_0 = _rtP -> P_1883 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_em . B_23_2_0 . re , _rtB -> PosSeqComputation_em .
B_23_2_0 . im ) ; ssCallAccelRunBlock ( S , 392 , 2474 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_2478_0 [ 0 ] = _rtP -> P_1884 * _rtB -> B_0_14_0 [ 108 ] * _rtP ->
P_1887 ; _rtB -> B_392_2478_0 [ 1 ] = _rtP -> P_1885 * _rtB -> B_0_14_0 [ 109
] * _rtP -> P_1887 ; _rtB -> B_392_2478_0 [ 2 ] = _rtP -> P_1886 * _rtB ->
B_0_14_0 [ 110 ] * _rtP -> P_1887 ; _rtB -> B_392_2482_0 [ 0 ] = _rtP ->
P_1888 * _rtB -> B_0_14_0 [ 53 ] * _rtP -> P_1891 ; _rtB -> B_392_2482_0 [ 1
] = _rtP -> P_1889 * _rtB -> B_0_14_0 [ 54 ] * _rtP -> P_1891 ; _rtB ->
B_392_2482_0 [ 2 ] = _rtP -> P_1890 * _rtB -> B_0_14_0 [ 55 ] * _rtP ->
P_1891 ; } _rtB -> B_392_2518_0 = ( muDoubleScalarSin ( _rtP -> P_1894 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1895 ) * _rtP -> P_1892 + _rtP -> P_1893
) * _rtB -> B_392_2478_0 [ 0 ] ; _rtB -> B_392_2520_0 = ( muDoubleScalarSin (
_rtP -> P_1898 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1899 ) * _rtP -> P_1896
+ _rtP -> P_1897 ) * _rtB -> B_392_2478_0 [ 0 ] ; _rtB -> B_392_2522_0 = (
muDoubleScalarSin ( _rtP -> P_1902 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1903
) * _rtP -> P_1900 + _rtP -> P_1901 ) * _rtB -> B_392_2478_0 [ 1 ] ; _rtB ->
B_392_2524_0 = ( muDoubleScalarSin ( _rtP -> P_1906 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1907 ) * _rtP -> P_1904 + _rtP -> P_1905 ) * _rtB -> B_392_2478_0
[ 1 ] ; _rtB -> B_392_2526_0 = ( muDoubleScalarSin ( _rtP -> P_1910 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1911 ) * _rtP -> P_1908 + _rtP -> P_1909
) * _rtB -> B_392_2478_0 [ 2 ] ; _rtB -> B_392_2528_0 = ( muDoubleScalarSin (
_rtP -> P_1914 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1915 ) * _rtP -> P_1912
+ _rtP -> P_1913 ) * _rtB -> B_392_2478_0 [ 2 ] ; B_392_2529_0 = _rtP ->
P_1916 * rtb_B_392_239_0 ; _rtB -> B_392_2531_0 = ( muDoubleScalarSin ( _rtP
-> P_1919 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1920 ) * _rtP -> P_1917 +
_rtP -> P_1918 ) * _rtB -> B_392_2482_0 [ 0 ] ; _rtB -> B_392_2533_0 = (
muDoubleScalarSin ( _rtP -> P_1923 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1924
) * _rtP -> P_1921 + _rtP -> P_1922 ) * _rtB -> B_392_2482_0 [ 0 ] ; _rtB ->
B_392_2535_0 = ( muDoubleScalarSin ( _rtP -> P_1927 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1928 ) * _rtP -> P_1925 + _rtP -> P_1926 ) * _rtB -> B_392_2482_0
[ 1 ] ; _rtB -> B_392_2537_0 = ( muDoubleScalarSin ( _rtP -> P_1931 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1932 ) * _rtP -> P_1929 + _rtP -> P_1930
) * _rtB -> B_392_2482_0 [ 1 ] ; _rtB -> B_392_2539_0 = ( muDoubleScalarSin (
_rtP -> P_1935 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1936 ) * _rtP -> P_1933
+ _rtP -> P_1934 ) * _rtB -> B_392_2482_0 [ 2 ] ; _rtB -> B_392_2541_0 = (
muDoubleScalarSin ( _rtP -> P_1939 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1940
) * _rtP -> P_1937 + _rtP -> P_1938 ) * _rtB -> B_392_2482_0 [ 2 ] ; _rtB ->
B_392_2543_0 = ( muDoubleScalarSin ( _rtP -> P_1943 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1944 ) * _rtP -> P_1941 + _rtP -> P_1942 ) * _rtB -> B_392_2478_0
[ 0 ] ; _rtB -> B_392_2545_0 = ( muDoubleScalarSin ( _rtP -> P_1947 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1948 ) * _rtP -> P_1945 + _rtP -> P_1946
) * _rtB -> B_392_2478_0 [ 0 ] ; _rtB -> B_392_2547_0 = ( muDoubleScalarSin (
_rtP -> P_1951 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1952 ) * _rtP -> P_1949
+ _rtP -> P_1950 ) * _rtB -> B_392_2478_0 [ 1 ] ; _rtB -> B_392_2549_0 = (
muDoubleScalarSin ( _rtP -> P_1955 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1956
) * _rtP -> P_1953 + _rtP -> P_1954 ) * _rtB -> B_392_2478_0 [ 1 ] ; _rtB ->
B_392_2551_0 = ( muDoubleScalarSin ( _rtP -> P_1959 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1960 ) * _rtP -> P_1957 + _rtP -> P_1958 ) * _rtB -> B_392_2478_0
[ 2 ] ; _rtB -> B_392_2553_0 = ( muDoubleScalarSin ( _rtP -> P_1963 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1964 ) * _rtP -> P_1961 + _rtP -> P_1962
) * _rtB -> B_392_2478_0 [ 2 ] ; _rtB -> B_392_2555_0 = ( muDoubleScalarSin (
_rtP -> P_1967 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1968 ) * _rtP -> P_1965
+ _rtP -> P_1966 ) * _rtB -> B_392_2482_0 [ 0 ] ; _rtB -> B_392_2557_0 = (
muDoubleScalarSin ( _rtP -> P_1971 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1972
) * _rtP -> P_1969 + _rtP -> P_1970 ) * _rtB -> B_392_2482_0 [ 0 ] ; _rtB ->
B_392_2559_0 = ( muDoubleScalarSin ( _rtP -> P_1975 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_1976 ) * _rtP -> P_1973 + _rtP -> P_1974 ) * _rtB -> B_392_2482_0
[ 1 ] ; _rtB -> B_392_2561_0 = ( muDoubleScalarSin ( _rtP -> P_1979 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_1980 ) * _rtP -> P_1977 + _rtP -> P_1978
) * _rtB -> B_392_2482_0 [ 1 ] ; _rtB -> B_392_2563_0 = ( muDoubleScalarSin (
_rtP -> P_1983 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1984 ) * _rtP -> P_1981
+ _rtP -> P_1982 ) * _rtB -> B_392_2482_0 [ 2 ] ; _rtB -> B_392_2565_0 = (
muDoubleScalarSin ( _rtP -> P_1987 * ssGetTaskTime ( S , 0 ) + _rtP -> P_1988
) * _rtP -> P_1985 + _rtP -> P_1986 ) * _rtB -> B_392_2482_0 [ 2 ] ;
B_392_2566_0 = _rtP -> P_1989 * rtb_B_392_237_0 ; _rtB -> B_392_2571_0 = _rtX
-> integrator_CSTATE_bm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_mg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_mg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1991 ;
B_392_2572_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_grh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_grh . Last , _rtDW ->
TransportDelay_IWORK_grh . Tail , _rtDW -> TransportDelay_IWORK_grh . Head ,
_rtP -> P_1992 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2574_0 = _rtP -> P_1993 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2574_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2576_0 =
_rtDW -> Memory_PreviousInput_fmc ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2578_0 = ( _rtB -> B_392_2571_0 - B_392_2572_0 ) * _rtP -> P_229 ; }
else { _rtB -> B_392_2578_0 = _rtB -> B_392_2576_0 ; } _rtB -> B_392_2579_0 =
_rtX -> integrator_CSTATE_hk ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_fu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_1996 ;
B_392_2580_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_b2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_b2 . Last , _rtDW ->
TransportDelay_IWORK_b2 . Tail , _rtDW -> TransportDelay_IWORK_b2 . Head ,
_rtP -> P_1997 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2582_0 = _rtP -> P_1998 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2582_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2584_0 =
_rtDW -> Memory_PreviousInput_dm ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2586_0 = ( _rtB -> B_392_2579_0 - B_392_2580_0 ) * _rtP -> P_228 ; }
else { _rtB -> B_392_2586_0 = _rtB -> B_392_2584_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2000 * muDoubleScalarAtan2 ( _rtB -> B_392_2586_0 , _rtB ->
B_392_2578_0 ) * _rtP -> P_2001 ; B_392_2591_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2578_0 , _rtB -> B_392_2586_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2591_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2578_0 , _rtB -> B_392_2586_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2592_0 = _rtX -> integrator_CSTATE_ia ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2003 ; B_392_2593_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e2b . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e2b . Last , _rtDW -> TransportDelay_IWORK_e2b
. Tail , _rtDW -> TransportDelay_IWORK_e2b . Head , _rtP -> P_2004 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2595_0 = _rtP -> P_2005 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2595_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2597_0 = _rtDW -> Memory_PreviousInput_js ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2599_0 = ( _rtB -> B_392_2592_0 -
B_392_2593_0 ) * _rtP -> P_231 ; } else { _rtB -> B_392_2599_0 = _rtB ->
B_392_2597_0 ; } _rtB -> B_392_2600_0 = _rtX -> integrator_CSTATE_eq ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ns .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ns . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2008 ; B_392_2601_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mz . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_mz . Last , _rtDW -> TransportDelay_IWORK_mz .
Tail , _rtDW -> TransportDelay_IWORK_mz . Head , _rtP -> P_2009 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2603_0 = _rtP -> P_2010 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2603_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2605_0 = _rtDW -> Memory_PreviousInput_o2 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2607_0 = ( _rtB -> B_392_2600_0 -
B_392_2601_0 ) * _rtP -> P_230 ; } else { _rtB -> B_392_2607_0 = _rtB ->
B_392_2605_0 ; } rtb_B_392_2122_0 = _rtP -> P_2012 * muDoubleScalarAtan2 (
_rtB -> B_392_2607_0 , _rtB -> B_392_2599_0 ) * _rtP -> P_2013 ; B_392_2612_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2599_0 , _rtB -> B_392_2607_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2612_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2599_0 , _rtB -> B_392_2607_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2613_0 = _rtX ->
integrator_CSTATE_af ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2015 ;
B_392_2614_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fg .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_fg . Last , _rtDW ->
TransportDelay_IWORK_fg . Tail , _rtDW -> TransportDelay_IWORK_fg . Head ,
_rtP -> P_2016 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2616_0 = _rtP -> P_2017 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2616_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2618_0 =
_rtDW -> Memory_PreviousInput_eq5 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2620_0 = ( _rtB -> B_392_2613_0 - B_392_2614_0 ) * _rtP -> P_233 ; }
else { _rtB -> B_392_2620_0 = _rtB -> B_392_2618_0 ; } _rtB -> B_392_2621_0 =
_rtX -> integrator_CSTATE_kxp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_da . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_da . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2020 ;
B_392_2622_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ji .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ji . Last , _rtDW ->
TransportDelay_IWORK_ji . Tail , _rtDW -> TransportDelay_IWORK_ji . Head ,
_rtP -> P_2021 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2624_0 = _rtP -> P_2022 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2624_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2626_0 =
_rtDW -> Memory_PreviousInput_b1 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2628_0 = ( _rtB -> B_392_2621_0 - B_392_2622_0 ) * _rtP -> P_232 ; }
else { _rtB -> B_392_2628_0 = _rtB -> B_392_2626_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2024 * muDoubleScalarAtan2 ( _rtB -> B_392_2628_0 , _rtB ->
B_392_2620_0 ) * _rtP -> P_2025 ; B_392_2633_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2620_0 , _rtB -> B_392_2628_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2633_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2620_0 , _rtB -> B_392_2628_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_2635_0 = _rtB -> B_392_2634_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2635_0 ,
B_392_2591_0 , B_392_2612_0 , B_392_2633_0 , & _rtB -> PosSeqComputation_du ,
& _rtDW -> PosSeqComputation_du , & _rtP -> PosSeqComputation_du ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2638_0 = _rtB
-> B_392_2637_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2638_0 , B_392_2591_0 , B_392_2612_0 , B_392_2633_0 , & _rtB ->
NegSeqComputation_le , & _rtDW -> NegSeqComputation_le , & _rtP ->
NegSeqComputation_le ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2641_0 = _rtB -> B_392_2640_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2641_0 ,
B_392_2591_0 , B_392_2612_0 , B_392_2633_0 , & _rtB -> ZeroSeqComputation_ow
, & _rtDW -> ZeroSeqComputation_ow , & _rtP -> ZeroSeqComputation_ow ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_du . B_23_2_0
. re , _rtB -> PosSeqComputation_du . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_du . B_23_2_0 . im , _rtB ->
PosSeqComputation_du . B_23_2_0 . re ) ; _rtB -> B_392_2644_0 = _rtX ->
integrator_CSTATE_hrr ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e1 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_e1 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2030 ;
B_392_2645_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ln .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ln . Last , _rtDW ->
TransportDelay_IWORK_ln . Tail , _rtDW -> TransportDelay_IWORK_ln . Head ,
_rtP -> P_2031 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2647_0 = _rtP -> P_2032 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2647_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2649_0 =
_rtDW -> Memory_PreviousInput_pzf ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2651_0 = ( _rtB -> B_392_2644_0 - B_392_2645_0 ) * _rtP -> P_235 ; }
else { _rtB -> B_392_2651_0 = _rtB -> B_392_2649_0 ; } _rtB -> B_392_2652_0 =
_rtX -> integrator_CSTATE_e4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_iu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2035 ;
B_392_2653_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_po .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_po . Last , _rtDW ->
TransportDelay_IWORK_po . Tail , _rtDW -> TransportDelay_IWORK_po . Head ,
_rtP -> P_2036 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2655_0 = _rtP -> P_2037 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2655_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2657_0 =
_rtDW -> Memory_PreviousInput_fc ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2659_0 = ( _rtB -> B_392_2652_0 - B_392_2653_0 ) * _rtP -> P_234 ; }
else { _rtB -> B_392_2659_0 = _rtB -> B_392_2657_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2039 * muDoubleScalarAtan2 ( _rtB -> B_392_2659_0 , _rtB ->
B_392_2651_0 ) * _rtP -> P_2040 ; B_392_2664_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2651_0 , _rtB -> B_392_2659_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2664_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2651_0 , _rtB -> B_392_2659_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2665_0 = _rtX -> integrator_CSTATE_otl ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2042 ; B_392_2666_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nl . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_nl . Last , _rtDW -> TransportDelay_IWORK_nl .
Tail , _rtDW -> TransportDelay_IWORK_nl . Head , _rtP -> P_2043 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2668_0 = _rtP -> P_2044 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2668_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2670_0 = _rtDW -> Memory_PreviousInput_jd ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2672_0 = ( _rtB -> B_392_2665_0 -
B_392_2666_0 ) * _rtP -> P_237 ; } else { _rtB -> B_392_2672_0 = _rtB ->
B_392_2670_0 ; } _rtB -> B_392_2673_0 = _rtX -> integrator_CSTATE_pv ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2047 ; B_392_2674_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_px . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_px . Last , _rtDW -> TransportDelay_IWORK_px .
Tail , _rtDW -> TransportDelay_IWORK_px . Head , _rtP -> P_2048 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2676_0 = _rtP -> P_2049 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2676_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2678_0 = _rtDW -> Memory_PreviousInput_mw ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2680_0 = ( _rtB -> B_392_2673_0 -
B_392_2674_0 ) * _rtP -> P_236 ; } else { _rtB -> B_392_2680_0 = _rtB ->
B_392_2678_0 ; } rtb_B_392_2122_0 = _rtP -> P_2051 * muDoubleScalarAtan2 (
_rtB -> B_392_2680_0 , _rtB -> B_392_2672_0 ) * _rtP -> P_2052 ; B_392_2685_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2672_0 , _rtB -> B_392_2680_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2685_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2672_0 , _rtB -> B_392_2680_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2686_0 = _rtX ->
integrator_CSTATE_i5 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_io . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_io . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2054 ;
B_392_2687_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kx .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kx . Last , _rtDW ->
TransportDelay_IWORK_kx . Tail , _rtDW -> TransportDelay_IWORK_kx . Head ,
_rtP -> P_2055 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2689_0 = _rtP -> P_2056 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2689_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2691_0 =
_rtDW -> Memory_PreviousInput_aa3 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2693_0 = ( _rtB -> B_392_2686_0 - B_392_2687_0 ) * _rtP -> P_239 ; }
else { _rtB -> B_392_2693_0 = _rtB -> B_392_2691_0 ; } _rtB -> B_392_2694_0 =
_rtX -> integrator_CSTATE_dsl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_e4 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2059 ;
B_392_2695_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ff .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ff . Last , _rtDW ->
TransportDelay_IWORK_ff . Tail , _rtDW -> TransportDelay_IWORK_ff . Head ,
_rtP -> P_2060 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2697_0 = _rtP -> P_2061 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2697_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2699_0 =
_rtDW -> Memory_PreviousInput_dj ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2701_0 = ( _rtB -> B_392_2694_0 - B_392_2695_0 ) * _rtP -> P_238 ; }
else { _rtB -> B_392_2701_0 = _rtB -> B_392_2699_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2063 * muDoubleScalarAtan2 ( _rtB -> B_392_2701_0 , _rtB ->
B_392_2693_0 ) * _rtP -> P_2064 ; B_392_2706_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2693_0 , _rtB -> B_392_2701_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2706_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2693_0 , _rtB -> B_392_2701_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_2708_0 = _rtB -> B_392_2707_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2708_0 ,
B_392_2664_0 , B_392_2685_0 , B_392_2706_0 , & _rtB -> PosSeqComputation_pf ,
& _rtDW -> PosSeqComputation_pf , & _rtP -> PosSeqComputation_pf ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2711_0 = _rtB
-> B_392_2710_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2711_0 , B_392_2664_0 , B_392_2685_0 , B_392_2706_0 , & _rtB ->
NegSeqComputation_kv , & _rtDW -> NegSeqComputation_kv , & _rtP ->
NegSeqComputation_kv ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2714_0 = _rtB -> B_392_2713_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2714_0 ,
B_392_2664_0 , B_392_2685_0 , B_392_2706_0 , & _rtB -> ZeroSeqComputation_fm
, & _rtDW -> ZeroSeqComputation_fm , & _rtP -> ZeroSeqComputation_fm ) ;
rtb_B_392_2122_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_pf . B_23_2_0 . re , _rtB -> PosSeqComputation_pf .
B_23_2_0 . im ) * _rtP -> P_2068 ; muDoubleScalarSinCos ( _rtP -> P_2071 * (
_rtP -> P_2069 * rtb_B_392_237_0 - _rtP -> P_2070 * muDoubleScalarAtan2 (
_rtB -> PosSeqComputation_pf . B_23_2_0 . im , _rtB -> PosSeqComputation_pf .
B_23_2_0 . re ) ) , & rtb_B_392_1600_0 , & rtb_B_392_430_0 ) ; _rtB ->
B_392_2724_0 = rtb_B_392_2122_0 * rtb_B_392_430_0 ; _rtB -> B_392_2725_0 =
_rtX -> integrator_CSTATE_if ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mo . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mo . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2073 ;
B_392_2726_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lh . Last , _rtDW ->
TransportDelay_IWORK_lh . Tail , _rtDW -> TransportDelay_IWORK_lh . Head ,
_rtP -> P_2074 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2728_0 = _rtP -> P_2075 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2728_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2730_0 =
_rtDW -> Memory_PreviousInput_jw ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2732_0 = ( _rtB -> B_392_2725_0 - B_392_2726_0 ) * _rtP -> P_241 ; }
else { _rtB -> B_392_2732_0 = _rtB -> B_392_2730_0 ; } _rtB -> B_392_2733_0 =
_rtX -> integrator_CSTATE_il ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_kt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kt . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2078 ;
B_392_2734_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ip .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ip . Last , _rtDW ->
TransportDelay_IWORK_ip . Tail , _rtDW -> TransportDelay_IWORK_ip . Head ,
_rtP -> P_2079 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2736_0 = _rtP -> P_2080 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2736_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2738_0 =
_rtDW -> Memory_PreviousInput_b2 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2740_0 = ( _rtB -> B_392_2733_0 - B_392_2734_0 ) * _rtP -> P_240 ; }
else { _rtB -> B_392_2740_0 = _rtB -> B_392_2738_0 ; } rtb_B_392_430_0 = _rtP
-> P_2082 * muDoubleScalarAtan2 ( _rtB -> B_392_2740_0 , _rtB -> B_392_2732_0
) * _rtP -> P_2083 ; B_392_2745_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2732_0 , _rtB -> B_392_2740_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2745_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2732_0 , _rtB ->
B_392_2740_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_2746_0
= _rtX -> integrator_CSTATE_cv ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2085
; B_392_2747_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_of .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_of . Last , _rtDW ->
TransportDelay_IWORK_of . Tail , _rtDW -> TransportDelay_IWORK_of . Head ,
_rtP -> P_2086 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2749_0 = _rtP -> P_2087 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2749_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2751_0 =
_rtDW -> Memory_PreviousInput_fw ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2753_0 = ( _rtB -> B_392_2746_0 - B_392_2747_0 ) * _rtP -> P_243 ; }
else { _rtB -> B_392_2753_0 = _rtB -> B_392_2751_0 ; } _rtB -> B_392_2754_0 =
_rtX -> integrator_CSTATE_cl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_d5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2090 ;
B_392_2755_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lk .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lk . Last , _rtDW ->
TransportDelay_IWORK_lk . Tail , _rtDW -> TransportDelay_IWORK_lk . Head ,
_rtP -> P_2091 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2757_0 = _rtP -> P_2092 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2757_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2759_0 =
_rtDW -> Memory_PreviousInput_ml ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2761_0 = ( _rtB -> B_392_2754_0 - B_392_2755_0 ) * _rtP -> P_242 ; }
else { _rtB -> B_392_2761_0 = _rtB -> B_392_2759_0 ; } rtb_B_392_430_0 = _rtP
-> P_2094 * muDoubleScalarAtan2 ( _rtB -> B_392_2761_0 , _rtB -> B_392_2753_0
) * _rtP -> P_2095 ; B_392_2766_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2753_0 , _rtB -> B_392_2761_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2766_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2753_0 , _rtB ->
B_392_2761_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; _rtB -> B_392_2767_0
= _rtX -> integrator_CSTATE_ge ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2097
; B_392_2768_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pj .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pj . Last , _rtDW ->
TransportDelay_IWORK_pj . Tail , _rtDW -> TransportDelay_IWORK_pj . Head ,
_rtP -> P_2098 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2770_0 = _rtP -> P_2099 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2770_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2772_0 =
_rtDW -> Memory_PreviousInput_n1 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2774_0 = ( _rtB -> B_392_2767_0 - B_392_2768_0 ) * _rtP -> P_245 ; }
else { _rtB -> B_392_2774_0 = _rtB -> B_392_2772_0 ; } _rtB -> B_392_2775_0 =
_rtX -> integrator_CSTATE_ak ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_em . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_em . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2102 ;
B_392_2776_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_l5 . Last , _rtDW ->
TransportDelay_IWORK_l5 . Tail , _rtDW -> TransportDelay_IWORK_l5 . Head ,
_rtP -> P_2103 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2778_0 = _rtP -> P_2104 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2778_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2780_0 =
_rtDW -> Memory_PreviousInput_i5 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2782_0 = ( _rtB -> B_392_2775_0 - B_392_2776_0 ) * _rtP -> P_244 ; }
else { _rtB -> B_392_2782_0 = _rtB -> B_392_2780_0 ; } rtb_B_392_430_0 = _rtP
-> P_2106 * muDoubleScalarAtan2 ( _rtB -> B_392_2782_0 , _rtB -> B_392_2774_0
) * _rtP -> P_2107 ; B_392_2787_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_2774_0 , _rtB -> B_392_2782_0 ) * muDoubleScalarCos ( rtb_B_392_430_0 )
; B_392_2787_0 . im = muDoubleScalarHypot ( _rtB -> B_392_2774_0 , _rtB ->
B_392_2782_0 ) * muDoubleScalarSin ( rtb_B_392_430_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2789_0 = _rtB
-> B_392_2788_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2789_0 , B_392_2745_0 , B_392_2766_0 , B_392_2787_0 , & _rtB ->
PosSeqComputation_ko , & _rtDW -> PosSeqComputation_ko , & _rtP ->
PosSeqComputation_ko ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2792_0 = _rtB -> B_392_2791_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2792_0 ,
B_392_2745_0 , B_392_2766_0 , B_392_2787_0 , & _rtB -> NegSeqComputation_n ,
& _rtDW -> NegSeqComputation_n , & _rtP -> NegSeqComputation_n ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2795_0 = _rtB
-> B_392_2794_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_2795_0 , B_392_2745_0 , B_392_2766_0 , B_392_2787_0 , & _rtB ->
ZeroSeqComputation_j , & _rtDW -> ZeroSeqComputation_j , & _rtP ->
ZeroSeqComputation_j ) ; rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ko . B_23_2_0 . im , _rtB -> PosSeqComputation_ko .
B_23_2_0 . re ) ; _rtB -> B_392_2798_0 = _rtP -> P_2111 * muDoubleScalarHypot
( _rtB -> PosSeqComputation_ko . B_23_2_0 . re , _rtB -> PosSeqComputation_ko
. B_23_2_0 . im ) ; _rtB -> B_392_2799_0 = rtb_B_392_2122_0 *
rtb_B_392_1600_0 ; _rtB -> B_392_2800_0 = _rtX -> integrator_CSTATE_mz2 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ki .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ki . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2113 ; B_392_2801_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jy . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_jy . Last , _rtDW -> TransportDelay_IWORK_jy .
Tail , _rtDW -> TransportDelay_IWORK_jy . Head , _rtP -> P_2114 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2803_0 = _rtP -> P_2115 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2803_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2805_0 = _rtDW -> Memory_PreviousInput_hz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2807_0 = ( _rtB -> B_392_2800_0 -
B_392_2801_0 ) * _rtP -> P_223 ; } else { _rtB -> B_392_2807_0 = _rtB ->
B_392_2805_0 ; } _rtB -> B_392_2808_0 = _rtX -> integrator_CSTATE_b5 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ev .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ev . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2118 ; B_392_2809_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ly . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ly . Last , _rtDW -> TransportDelay_IWORK_ly .
Tail , _rtDW -> TransportDelay_IWORK_ly . Head , _rtP -> P_2119 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2811_0 = _rtP -> P_2120 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2811_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2813_0 = _rtDW -> Memory_PreviousInput_lk ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2815_0 = ( _rtB -> B_392_2808_0 -
B_392_2809_0 ) * _rtP -> P_222 ; } else { _rtB -> B_392_2815_0 = _rtB ->
B_392_2813_0 ; } rtb_B_392_2122_0 = _rtP -> P_2122 * muDoubleScalarAtan2 (
_rtB -> B_392_2815_0 , _rtB -> B_392_2807_0 ) * _rtP -> P_2123 ; B_392_2820_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2807_0 , _rtB -> B_392_2815_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2820_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2807_0 , _rtB -> B_392_2815_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_2821_0 = _rtX ->
integrator_CSTATE_hc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oeh . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_oeh . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2125 ;
B_392_2822_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_oq . Last , _rtDW ->
TransportDelay_IWORK_oq . Tail , _rtDW -> TransportDelay_IWORK_oq . Head ,
_rtP -> P_2126 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2824_0 = _rtP -> P_2127 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2824_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2826_0 =
_rtDW -> Memory_PreviousInput_gj ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2828_0 = ( _rtB -> B_392_2821_0 - B_392_2822_0 ) * _rtP -> P_225 ; }
else { _rtB -> B_392_2828_0 = _rtB -> B_392_2826_0 ; } _rtB -> B_392_2829_0 =
_rtX -> integrator_CSTATE_bn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mw . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mw . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2130 ;
B_392_2830_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pr . Last , _rtDW ->
TransportDelay_IWORK_pr . Tail , _rtDW -> TransportDelay_IWORK_pr . Head ,
_rtP -> P_2131 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2832_0 = _rtP -> P_2132 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_2832_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2834_0 =
_rtDW -> Memory_PreviousInput_gl ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_2836_0 = ( _rtB -> B_392_2829_0 - B_392_2830_0 ) * _rtP -> P_224 ; }
else { _rtB -> B_392_2836_0 = _rtB -> B_392_2834_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2134 * muDoubleScalarAtan2 ( _rtB -> B_392_2836_0 , _rtB ->
B_392_2828_0 ) * _rtP -> P_2135 ; B_392_2841_0 . re = muDoubleScalarHypot (
_rtB -> B_392_2828_0 , _rtB -> B_392_2836_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_2841_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_2828_0 , _rtB -> B_392_2836_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_2842_0 = _rtX -> integrator_CSTATE_jqz ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2137 ; B_392_2843_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kj . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kj . Last , _rtDW -> TransportDelay_IWORK_kj .
Tail , _rtDW -> TransportDelay_IWORK_kj . Head , _rtP -> P_2138 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2845_0 = _rtP -> P_2139 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2845_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2847_0 = _rtDW -> Memory_PreviousInput_pd ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2849_0 = ( _rtB -> B_392_2842_0 -
B_392_2843_0 ) * _rtP -> P_227 ; } else { _rtB -> B_392_2849_0 = _rtB ->
B_392_2847_0 ; } _rtB -> B_392_2850_0 = _rtX -> integrator_CSTATE_mn ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oq .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_oq . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2142 ; B_392_2851_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pn . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_pn . Last , _rtDW -> TransportDelay_IWORK_pn .
Tail , _rtDW -> TransportDelay_IWORK_pn . Head , _rtP -> P_2143 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_2853_0 = _rtP -> P_2144 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_2853_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_2855_0 = _rtDW -> Memory_PreviousInput_ey ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_2857_0 = ( _rtB -> B_392_2850_0 -
B_392_2851_0 ) * _rtP -> P_226 ; } else { _rtB -> B_392_2857_0 = _rtB ->
B_392_2855_0 ; } rtb_B_392_2122_0 = _rtP -> P_2146 * muDoubleScalarAtan2 (
_rtB -> B_392_2857_0 , _rtB -> B_392_2849_0 ) * _rtP -> P_2147 ; B_392_2862_0
. re = muDoubleScalarHypot ( _rtB -> B_392_2849_0 , _rtB -> B_392_2857_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_2862_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_2849_0 , _rtB -> B_392_2857_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_2864_0 = _rtB -> B_392_2863_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_2864_0 ,
B_392_2820_0 , B_392_2841_0 , B_392_2862_0 , & _rtB -> PosSeqComputation_c4 ,
& _rtDW -> PosSeqComputation_c4 , & _rtP -> PosSeqComputation_c4 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2867_0 = _rtB
-> B_392_2866_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_2867_0 , B_392_2820_0 , B_392_2841_0 , B_392_2862_0 , & _rtB ->
NegSeqComputation_gw , & _rtDW -> NegSeqComputation_gw , & _rtP ->
NegSeqComputation_gw ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_2870_0 = _rtB -> B_392_2869_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_2870_0 ,
B_392_2820_0 , B_392_2841_0 , B_392_2862_0 , & _rtB -> ZeroSeqComputation_mx
, & _rtDW -> ZeroSeqComputation_mx , & _rtP -> ZeroSeqComputation_mx ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_c4 .
B_23_2_0 . im , _rtB -> PosSeqComputation_c4 . B_23_2_0 . re ) ; _rtB ->
B_392_2873_0 = _rtP -> P_2151 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_c4 . B_23_2_0 . re , _rtB -> PosSeqComputation_c4 .
B_23_2_0 . im ) ; ssCallAccelRunBlock ( S , 392 , 2874 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_392_1512_0 [ 0 ] = _rtB -> B_392_2724_0 ; _rtB -> B_392_1512_0
[ 1 ] = _rtB -> B_392_2798_0 ; _rtB -> B_392_1512_0 [ 2 ] = _rtB ->
B_392_2799_0 ; _rtB -> B_392_1512_0 [ 3 ] = _rtB -> B_392_2873_0 ;
ssCallAccelRunBlock ( S , 392 , 2876 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_2880_0 [ 0 ]
= _rtP -> P_2152 * _rtB -> B_0_14_0 [ 117 ] * _rtP -> P_2155 ; _rtB ->
B_392_2880_0 [ 1 ] = _rtP -> P_2153 * _rtB -> B_0_14_0 [ 118 ] * _rtP ->
P_2155 ; _rtB -> B_392_2880_0 [ 2 ] = _rtP -> P_2154 * _rtB -> B_0_14_0 [ 119
] * _rtP -> P_2155 ; _rtB -> B_392_2884_0 [ 0 ] = _rtP -> P_2156 * _rtB ->
B_0_14_0 [ 62 ] * _rtP -> P_2159 ; _rtB -> B_392_2884_0 [ 1 ] = _rtP ->
P_2157 * _rtB -> B_0_14_0 [ 63 ] * _rtP -> P_2159 ; _rtB -> B_392_2884_0 [ 2
] = _rtP -> P_2158 * _rtB -> B_0_14_0 [ 64 ] * _rtP -> P_2159 ; } _rtB ->
B_392_2920_0 = ( muDoubleScalarSin ( _rtP -> P_2162 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2163 ) * _rtP -> P_2160 + _rtP -> P_2161 ) * _rtB -> B_392_2880_0
[ 0 ] ; _rtB -> B_392_2922_0 = ( muDoubleScalarSin ( _rtP -> P_2166 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2167 ) * _rtP -> P_2164 + _rtP -> P_2165
) * _rtB -> B_392_2880_0 [ 0 ] ; _rtB -> B_392_2924_0 = ( muDoubleScalarSin (
_rtP -> P_2170 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2171 ) * _rtP -> P_2168
+ _rtP -> P_2169 ) * _rtB -> B_392_2880_0 [ 1 ] ; _rtB -> B_392_2926_0 = (
muDoubleScalarSin ( _rtP -> P_2174 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2175
) * _rtP -> P_2172 + _rtP -> P_2173 ) * _rtB -> B_392_2880_0 [ 1 ] ; _rtB ->
B_392_2928_0 = ( muDoubleScalarSin ( _rtP -> P_2178 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2179 ) * _rtP -> P_2176 + _rtP -> P_2177 ) * _rtB -> B_392_2880_0
[ 2 ] ; _rtB -> B_392_2930_0 = ( muDoubleScalarSin ( _rtP -> P_2182 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2183 ) * _rtP -> P_2180 + _rtP -> P_2181
) * _rtB -> B_392_2880_0 [ 2 ] ; B_392_2931_0 = _rtP -> P_2184 *
rtb_B_392_239_0 ; _rtB -> B_392_2933_0 = ( muDoubleScalarSin ( _rtP -> P_2187
* ssGetTaskTime ( S , 0 ) + _rtP -> P_2188 ) * _rtP -> P_2185 + _rtP ->
P_2186 ) * _rtB -> B_392_2884_0 [ 0 ] ; _rtB -> B_392_2935_0 = (
muDoubleScalarSin ( _rtP -> P_2191 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2192
) * _rtP -> P_2189 + _rtP -> P_2190 ) * _rtB -> B_392_2884_0 [ 0 ] ; _rtB ->
B_392_2937_0 = ( muDoubleScalarSin ( _rtP -> P_2195 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2196 ) * _rtP -> P_2193 + _rtP -> P_2194 ) * _rtB -> B_392_2884_0
[ 1 ] ; _rtB -> B_392_2939_0 = ( muDoubleScalarSin ( _rtP -> P_2199 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2200 ) * _rtP -> P_2197 + _rtP -> P_2198
) * _rtB -> B_392_2884_0 [ 1 ] ; _rtB -> B_392_2941_0 = ( muDoubleScalarSin (
_rtP -> P_2203 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2204 ) * _rtP -> P_2201
+ _rtP -> P_2202 ) * _rtB -> B_392_2884_0 [ 2 ] ; _rtB -> B_392_2943_0 = (
muDoubleScalarSin ( _rtP -> P_2207 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2208
) * _rtP -> P_2205 + _rtP -> P_2206 ) * _rtB -> B_392_2884_0 [ 2 ] ; _rtB ->
B_392_2945_0 = ( muDoubleScalarSin ( _rtP -> P_2211 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2212 ) * _rtP -> P_2209 + _rtP -> P_2210 ) * _rtB -> B_392_2880_0
[ 0 ] ; _rtB -> B_392_2947_0 = ( muDoubleScalarSin ( _rtP -> P_2215 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2216 ) * _rtP -> P_2213 + _rtP -> P_2214
) * _rtB -> B_392_2880_0 [ 0 ] ; _rtB -> B_392_2949_0 = ( muDoubleScalarSin (
_rtP -> P_2219 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2220 ) * _rtP -> P_2217
+ _rtP -> P_2218 ) * _rtB -> B_392_2880_0 [ 1 ] ; _rtB -> B_392_2951_0 = (
muDoubleScalarSin ( _rtP -> P_2223 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2224
) * _rtP -> P_2221 + _rtP -> P_2222 ) * _rtB -> B_392_2880_0 [ 1 ] ; _rtB ->
B_392_2953_0 = ( muDoubleScalarSin ( _rtP -> P_2227 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2228 ) * _rtP -> P_2225 + _rtP -> P_2226 ) * _rtB -> B_392_2880_0
[ 2 ] ; _rtB -> B_392_2955_0 = ( muDoubleScalarSin ( _rtP -> P_2231 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2232 ) * _rtP -> P_2229 + _rtP -> P_2230
) * _rtB -> B_392_2880_0 [ 2 ] ; _rtB -> B_392_2957_0 = ( muDoubleScalarSin (
_rtP -> P_2235 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2236 ) * _rtP -> P_2233
+ _rtP -> P_2234 ) * _rtB -> B_392_2884_0 [ 0 ] ; _rtB -> B_392_2959_0 = (
muDoubleScalarSin ( _rtP -> P_2239 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2240
) * _rtP -> P_2237 + _rtP -> P_2238 ) * _rtB -> B_392_2884_0 [ 0 ] ; _rtB ->
B_392_2961_0 = ( muDoubleScalarSin ( _rtP -> P_2243 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2244 ) * _rtP -> P_2241 + _rtP -> P_2242 ) * _rtB -> B_392_2884_0
[ 1 ] ; _rtB -> B_392_2963_0 = ( muDoubleScalarSin ( _rtP -> P_2247 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2248 ) * _rtP -> P_2245 + _rtP -> P_2246
) * _rtB -> B_392_2884_0 [ 1 ] ; _rtB -> B_392_2965_0 = ( muDoubleScalarSin (
_rtP -> P_2251 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2252 ) * _rtP -> P_2249
+ _rtP -> P_2250 ) * _rtB -> B_392_2884_0 [ 2 ] ; _rtB -> B_392_2967_0 = (
muDoubleScalarSin ( _rtP -> P_2255 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2256
) * _rtP -> P_2253 + _rtP -> P_2254 ) * _rtB -> B_392_2884_0 [ 2 ] ;
B_392_2968_0 = _rtP -> P_2257 * rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_2971_0 >= _rtP -> P_2263 ) {
rtb_B_392_2974_0 = _rtB -> B_392_2970_0 ; } else { rtb_B_392_2974_0 =
rt_Lookup ( _rtP -> P_2261 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_2262 ) ;
} if ( _rtB -> B_392_2975_0 >= _rtP -> P_2266 ) { _rtB -> B_392_2978_0 =
rtb_B_392_2974_0 ; } else { _rtB -> B_392_2978_0 = _rtB -> B_392_2976_0 ; } }
_rtB -> B_392_2980_0 = rt_Lookup ( _rtP -> P_2267 , 5 , ssGetT ( S ) , _rtP
-> P_2268 ) ; if ( _rtB -> B_392_2969_0 >= _rtP -> P_2269 ) { _rtB ->
B_392_2981_0 = _rtB -> B_392_2978_0 ; } else { _rtB -> B_392_2981_0 = _rtB ->
B_392_2980_0 ; } if ( ( _rtDW -> TimeStampA_nv >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_g >= ssGetT ( S ) ) ) { _rtB -> B_392_2982_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_nv ; lastU = & _rtDW -> LastUAtTimeA_d
; if ( _rtDW -> TimeStampA_nv < _rtDW -> TimeStampB_g ) { if ( _rtDW ->
TimeStampB_g < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_g ;
lastU = & _rtDW -> LastUAtTimeB_dr ; } } else { if ( _rtDW -> TimeStampA_nv
>= ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_g ; lastU = & _rtDW
-> LastUAtTimeB_dr ; } } _rtB -> B_392_2982_0 = ( _rtB -> B_392_2980_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_2988_0 >= _rtP -> P_2273 ) {
_rtB -> B_392_2990_0 = rtb_B_392_2974_0 ; } else { _rtB -> B_392_2990_0 =
_rtB -> B_392_2976_0 ; } } _rtB -> B_392_2992_0 = rt_Lookup ( _rtP -> P_2274
, 5 , ssGetT ( S ) , _rtP -> P_2275 ) ; if ( _rtB -> B_392_2987_0 >= _rtP ->
P_2276 ) { _rtB -> B_392_2993_0 = _rtB -> B_392_2990_0 ; } else { _rtB ->
B_392_2993_0 = _rtB -> B_392_2992_0 ; } if ( ( _rtDW -> TimeStampA_l >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ic >= ssGetT ( S ) ) ) { _rtB ->
B_392_2994_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_l ; lastU
= & _rtDW -> LastUAtTimeA_ca ; if ( _rtDW -> TimeStampA_l < _rtDW ->
TimeStampB_ic ) { if ( _rtDW -> TimeStampB_ic < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_ic ; lastU = & _rtDW -> LastUAtTimeB_b
; } } else { if ( _rtDW -> TimeStampA_l >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_ic ; lastU = & _rtDW -> LastUAtTimeB_b ; } } _rtB ->
B_392_2994_0 = ( _rtB -> B_392_2992_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_3000_0 >= _rtP -> P_2280 ) { _rtB -> B_392_3002_0 =
rtb_B_392_2974_0 ; } else { _rtB -> B_392_3002_0 = _rtB -> B_392_2976_0 ; } }
_rtB -> B_392_3004_0 = rt_Lookup ( _rtP -> P_2281 , 5 , ssGetT ( S ) , _rtP
-> P_2282 ) ; if ( _rtB -> B_392_2999_0 >= _rtP -> P_2283 ) { _rtB ->
B_392_3005_0 = _rtB -> B_392_3002_0 ; } else { _rtB -> B_392_3005_0 = _rtB ->
B_392_3004_0 ; } if ( ( _rtDW -> TimeStampA_m >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_ka >= ssGetT ( S ) ) ) { _rtB -> B_392_3006_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_m ; lastU = & _rtDW -> LastUAtTimeA_e ;
if ( _rtDW -> TimeStampA_m < _rtDW -> TimeStampB_ka ) { if ( _rtDW ->
TimeStampB_ka < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_ka ;
lastU = & _rtDW -> LastUAtTimeB_c ; } } else { if ( _rtDW -> TimeStampA_m >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_ka ; lastU = & _rtDW
-> LastUAtTimeB_c ; } } _rtB -> B_392_3006_0 = ( _rtB -> B_392_3004_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } _rtB -> B_392_3023_0 = _rtX
-> integrator_CSTATE_np ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dt . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dt . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2286 ;
B_392_3024_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ea5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ea5 . Last , _rtDW ->
TransportDelay_IWORK_ea5 . Tail , _rtDW -> TransportDelay_IWORK_ea5 . Head ,
_rtP -> P_2287 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3026_0 = _rtP -> P_2288 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3026_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3028_0 =
_rtDW -> Memory_PreviousInput_o4 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3030_0 = ( _rtB -> B_392_3023_0 - B_392_3024_0 ) * _rtP -> P_253 ; }
else { _rtB -> B_392_3030_0 = _rtB -> B_392_3028_0 ; } _rtB -> B_392_3031_0 =
_rtX -> integrator_CSTATE_a5a ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mwl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2291
; B_392_3032_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eg .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_eg . Last , _rtDW ->
TransportDelay_IWORK_eg . Tail , _rtDW -> TransportDelay_IWORK_eg . Head ,
_rtP -> P_2292 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3034_0 = _rtP -> P_2293 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3034_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3036_0 =
_rtDW -> Memory_PreviousInput_nx ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3038_0 = ( _rtB -> B_392_3031_0 - B_392_3032_0 ) * _rtP -> P_252 ; }
else { _rtB -> B_392_3038_0 = _rtB -> B_392_3036_0 ; } rtb_B_392_2974_0 =
_rtP -> P_2295 * muDoubleScalarAtan2 ( _rtB -> B_392_3038_0 , _rtB ->
B_392_3030_0 ) * _rtP -> P_2296 ; B_392_3043_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3030_0 , _rtB -> B_392_3038_0 ) * muDoubleScalarCos (
rtb_B_392_2974_0 ) ; B_392_3043_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3030_0 , _rtB -> B_392_3038_0 ) * muDoubleScalarSin ( rtb_B_392_2974_0
) ; _rtB -> B_392_3044_0 = _rtX -> integrator_CSTATE_jo ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2298 ; B_392_3045_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kk . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kk . Last , _rtDW -> TransportDelay_IWORK_kk .
Tail , _rtDW -> TransportDelay_IWORK_kk . Head , _rtP -> P_2299 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3047_0 = _rtP -> P_2300 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3047_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3049_0 = _rtDW -> Memory_PreviousInput_mq ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3051_0 = ( _rtB -> B_392_3044_0 -
B_392_3045_0 ) * _rtP -> P_255 ; } else { _rtB -> B_392_3051_0 = _rtB ->
B_392_3049_0 ; } _rtB -> B_392_3052_0 = _rtX -> integrator_CSTATE_bj ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jp .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_jp . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2303 ; B_392_3053_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_he5 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_he5 . Last , _rtDW -> TransportDelay_IWORK_he5
. Tail , _rtDW -> TransportDelay_IWORK_he5 . Head , _rtP -> P_2304 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3055_0 = _rtP -> P_2305 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3055_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3057_0 = _rtDW -> Memory_PreviousInput_bq ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3059_0 = ( _rtB -> B_392_3052_0 -
B_392_3053_0 ) * _rtP -> P_254 ; } else { _rtB -> B_392_3059_0 = _rtB ->
B_392_3057_0 ; } rtb_B_392_2974_0 = _rtP -> P_2307 * muDoubleScalarAtan2 (
_rtB -> B_392_3059_0 , _rtB -> B_392_3051_0 ) * _rtP -> P_2308 ; B_392_3064_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3051_0 , _rtB -> B_392_3059_0 ) *
muDoubleScalarCos ( rtb_B_392_2974_0 ) ; B_392_3064_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3051_0 , _rtB -> B_392_3059_0 ) *
muDoubleScalarSin ( rtb_B_392_2974_0 ) ; _rtB -> B_392_3065_0 = _rtX ->
integrator_CSTATE_bu ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_gi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2310 ;
B_392_3066_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dp .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dp . Last , _rtDW ->
TransportDelay_IWORK_dp . Tail , _rtDW -> TransportDelay_IWORK_dp . Head ,
_rtP -> P_2311 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3068_0 = _rtP -> P_2312 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3068_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3070_0 =
_rtDW -> Memory_PreviousInput_ep ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3072_0 = ( _rtB -> B_392_3065_0 - B_392_3066_0 ) * _rtP -> P_257 ; }
else { _rtB -> B_392_3072_0 = _rtB -> B_392_3070_0 ; } _rtB -> B_392_3073_0 =
_rtX -> integrator_CSTATE_df ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ac . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ac . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2315 ;
B_392_3074_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_la .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_la . Last , _rtDW ->
TransportDelay_IWORK_la . Tail , _rtDW -> TransportDelay_IWORK_la . Head ,
_rtP -> P_2316 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3076_0 = _rtP -> P_2317 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3076_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3078_0 =
_rtDW -> Memory_PreviousInput_ms ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3080_0 = ( _rtB -> B_392_3073_0 - B_392_3074_0 ) * _rtP -> P_256 ; }
else { _rtB -> B_392_3080_0 = _rtB -> B_392_3078_0 ; } rtb_B_392_2974_0 =
_rtP -> P_2319 * muDoubleScalarAtan2 ( _rtB -> B_392_3080_0 , _rtB ->
B_392_3072_0 ) * _rtP -> P_2320 ; B_392_3085_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3072_0 , _rtB -> B_392_3080_0 ) * muDoubleScalarCos (
rtb_B_392_2974_0 ) ; B_392_3085_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3072_0 , _rtB -> B_392_3080_0 ) * muDoubleScalarSin ( rtb_B_392_2974_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3087_0 = _rtB -> B_392_3086_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3087_0 ,
B_392_3043_0 , B_392_3064_0 , B_392_3085_0 , & _rtB -> PosSeqComputation_l3 ,
& _rtDW -> PosSeqComputation_l3 , & _rtP -> PosSeqComputation_l3 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3090_0 = _rtB
-> B_392_3089_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3090_0 , B_392_3043_0 , B_392_3064_0 , B_392_3085_0 , & _rtB ->
NegSeqComputation_b , & _rtDW -> NegSeqComputation_b , & _rtP ->
NegSeqComputation_b ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3093_0 = _rtB -> B_392_3092_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3093_0 ,
B_392_3043_0 , B_392_3064_0 , B_392_3085_0 , & _rtB -> ZeroSeqComputation_oh
, & _rtDW -> ZeroSeqComputation_oh , & _rtP -> ZeroSeqComputation_oh ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_l3 . B_23_2_0
. re , _rtB -> PosSeqComputation_l3 . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_l3 . B_23_2_0 . im , _rtB ->
PosSeqComputation_l3 . B_23_2_0 . re ) ; _rtB -> B_392_3096_0 = _rtX ->
integrator_CSTATE_pe ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cm . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2325 ;
B_392_3097_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ev .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ev . Last , _rtDW ->
TransportDelay_IWORK_ev . Tail , _rtDW -> TransportDelay_IWORK_ev . Head ,
_rtP -> P_2326 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3099_0 = _rtP -> P_2327 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3099_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3101_0 =
_rtDW -> Memory_PreviousInput_dw ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3103_0 = ( _rtB -> B_392_3096_0 - B_392_3097_0 ) * _rtP -> P_259 ; }
else { _rtB -> B_392_3103_0 = _rtB -> B_392_3101_0 ; } _rtB -> B_392_3104_0 =
_rtX -> integrator_CSTATE_di ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_gl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gl . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2330 ;
B_392_3105_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_b1 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_b1 . Last , _rtDW ->
TransportDelay_IWORK_b1 . Tail , _rtDW -> TransportDelay_IWORK_b1 . Head ,
_rtP -> P_2331 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3107_0 = _rtP -> P_2332 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3107_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3109_0 =
_rtDW -> Memory_PreviousInput_jh ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3111_0 = ( _rtB -> B_392_3104_0 - B_392_3105_0 ) * _rtP -> P_258 ; }
else { _rtB -> B_392_3111_0 = _rtB -> B_392_3109_0 ; } rtb_B_392_2974_0 =
_rtP -> P_2334 * muDoubleScalarAtan2 ( _rtB -> B_392_3111_0 , _rtB ->
B_392_3103_0 ) * _rtP -> P_2335 ; B_392_3116_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3103_0 , _rtB -> B_392_3111_0 ) * muDoubleScalarCos (
rtb_B_392_2974_0 ) ; B_392_3116_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3103_0 , _rtB -> B_392_3111_0 ) * muDoubleScalarSin ( rtb_B_392_2974_0
) ; _rtB -> B_392_3117_0 = _rtX -> integrator_CSTATE_gi ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mf . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mf
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2337 ; B_392_3118_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nq . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_nq . Last , _rtDW -> TransportDelay_IWORK_nq .
Tail , _rtDW -> TransportDelay_IWORK_nq . Head , _rtP -> P_2338 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3120_0 = _rtP -> P_2339 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3120_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3122_0 = _rtDW -> Memory_PreviousInput_fx ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3124_0 = ( _rtB -> B_392_3117_0 -
B_392_3118_0 ) * _rtP -> P_261 ; } else { _rtB -> B_392_3124_0 = _rtB ->
B_392_3122_0 ; } _rtB -> B_392_3125_0 = _rtX -> integrator_CSTATE_fd ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ag .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ag . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2342 ; B_392_3126_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_he5l . CircularBufSize ,
& _rtDW -> TransportDelay_IWORK_he5l . Last , _rtDW ->
TransportDelay_IWORK_he5l . Tail , _rtDW -> TransportDelay_IWORK_he5l . Head
, _rtP -> P_2343 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3128_0 = _rtP -> P_2344 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3128_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3130_0 =
_rtDW -> Memory_PreviousInput_a2 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3132_0 = ( _rtB -> B_392_3125_0 - B_392_3126_0 ) * _rtP -> P_260 ; }
else { _rtB -> B_392_3132_0 = _rtB -> B_392_3130_0 ; } rtb_B_392_2974_0 =
_rtP -> P_2346 * muDoubleScalarAtan2 ( _rtB -> B_392_3132_0 , _rtB ->
B_392_3124_0 ) * _rtP -> P_2347 ; B_392_3137_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3124_0 , _rtB -> B_392_3132_0 ) * muDoubleScalarCos (
rtb_B_392_2974_0 ) ; B_392_3137_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3124_0 , _rtB -> B_392_3132_0 ) * muDoubleScalarSin ( rtb_B_392_2974_0
) ; _rtB -> B_392_3138_0 = _rtX -> integrator_CSTATE_i2 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2349 ; B_392_3139_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p4 . Last , _rtDW -> TransportDelay_IWORK_p4 .
Tail , _rtDW -> TransportDelay_IWORK_p4 . Head , _rtP -> P_2350 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3141_0 = _rtP -> P_2351 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3141_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3143_0 = _rtDW -> Memory_PreviousInput_cs ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3145_0 = ( _rtB -> B_392_3138_0 -
B_392_3139_0 ) * _rtP -> P_263 ; } else { _rtB -> B_392_3145_0 = _rtB ->
B_392_3143_0 ; } _rtB -> B_392_3146_0 = _rtX -> integrator_CSTATE_kz ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_og .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_og . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2354 ; B_392_3147_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kjd . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kjd . Last , _rtDW -> TransportDelay_IWORK_kjd
. Tail , _rtDW -> TransportDelay_IWORK_kjd . Head , _rtP -> P_2355 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3149_0 = _rtP -> P_2356 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3149_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3151_0 = _rtDW -> Memory_PreviousInput_fi ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3153_0 = ( _rtB -> B_392_3146_0 -
B_392_3147_0 ) * _rtP -> P_262 ; } else { _rtB -> B_392_3153_0 = _rtB ->
B_392_3151_0 ; } rtb_B_392_2974_0 = _rtP -> P_2358 * muDoubleScalarAtan2 (
_rtB -> B_392_3153_0 , _rtB -> B_392_3145_0 ) * _rtP -> P_2359 ; B_392_3158_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3145_0 , _rtB -> B_392_3153_0 ) *
muDoubleScalarCos ( rtb_B_392_2974_0 ) ; B_392_3158_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3145_0 , _rtB -> B_392_3153_0 ) *
muDoubleScalarSin ( rtb_B_392_2974_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_3160_0 = _rtB -> B_392_3159_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3160_0 ,
B_392_3116_0 , B_392_3137_0 , B_392_3158_0 , & _rtB -> PosSeqComputation_f ,
& _rtDW -> PosSeqComputation_f , & _rtP -> PosSeqComputation_f ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3163_0 = _rtB
-> B_392_3162_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3163_0 , B_392_3116_0 , B_392_3137_0 , B_392_3158_0 , & _rtB ->
NegSeqComputation_jy , & _rtDW -> NegSeqComputation_jy , & _rtP ->
NegSeqComputation_jy ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3166_0 = _rtB -> B_392_3165_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3166_0 ,
B_392_3116_0 , B_392_3137_0 , B_392_3158_0 , & _rtB -> ZeroSeqComputation_mj
, & _rtDW -> ZeroSeqComputation_mj , & _rtP -> ZeroSeqComputation_mj ) ;
rtb_B_392_2974_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_f . B_23_2_0 . re , _rtB -> PosSeqComputation_f . B_23_2_0
. im ) * _rtP -> P_2363 ; muDoubleScalarSinCos ( _rtP -> P_2366 * ( _rtP ->
P_2364 * rtb_B_392_237_0 - _rtP -> P_2365 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_f . B_23_2_0 . im , _rtB -> PosSeqComputation_f . B_23_2_0
. re ) ) , & rtb_B_392_2122_0 , & rtb_B_392_1600_0 ) ; _rtB -> B_392_3176_0 =
rtb_B_392_2974_0 * rtb_B_392_1600_0 ; _rtB -> B_392_3177_0 = _rtX ->
integrator_CSTATE_nf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_e0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2368 ;
B_392_3178_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cv .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_cv . Last , _rtDW ->
TransportDelay_IWORK_cv . Tail , _rtDW -> TransportDelay_IWORK_cv . Head ,
_rtP -> P_2369 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3180_0 = _rtP -> P_2370 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3180_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3182_0 =
_rtDW -> Memory_PreviousInput_m3 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3184_0 = ( _rtB -> B_392_3177_0 - B_392_3178_0 ) * _rtP -> P_265 ; }
else { _rtB -> B_392_3184_0 = _rtB -> B_392_3182_0 ; } _rtB -> B_392_3185_0 =
_rtX -> integrator_CSTATE_dl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2373 ;
B_392_3186_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p4s .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_p4s . Last , _rtDW ->
TransportDelay_IWORK_p4s . Tail , _rtDW -> TransportDelay_IWORK_p4s . Head ,
_rtP -> P_2374 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3188_0 = _rtP -> P_2375 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3188_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3190_0 =
_rtDW -> Memory_PreviousInput_pu ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3192_0 = ( _rtB -> B_392_3185_0 - B_392_3186_0 ) * _rtP -> P_264 ; }
else { _rtB -> B_392_3192_0 = _rtB -> B_392_3190_0 ; } rtb_B_392_1600_0 =
_rtP -> P_2377 * muDoubleScalarAtan2 ( _rtB -> B_392_3192_0 , _rtB ->
B_392_3184_0 ) * _rtP -> P_2378 ; B_392_3197_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3184_0 , _rtB -> B_392_3192_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_3197_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3184_0 , _rtB -> B_392_3192_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; _rtB -> B_392_3198_0 = _rtX -> integrator_CSTATE_kf ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2380 ; B_392_3199_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ce . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ce . Last , _rtDW -> TransportDelay_IWORK_ce .
Tail , _rtDW -> TransportDelay_IWORK_ce . Head , _rtP -> P_2381 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3201_0 = _rtP -> P_2382 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3201_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3203_0 = _rtDW -> Memory_PreviousInput_oz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3205_0 = ( _rtB -> B_392_3198_0 -
B_392_3199_0 ) * _rtP -> P_267 ; } else { _rtB -> B_392_3205_0 = _rtB ->
B_392_3203_0 ; } _rtB -> B_392_3206_0 = _rtX -> integrator_CSTATE_k4 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ck .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ck . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2385 ; B_392_3207_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_cc . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_cc . Last , _rtDW -> TransportDelay_IWORK_cc .
Tail , _rtDW -> TransportDelay_IWORK_cc . Head , _rtP -> P_2386 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3209_0 = _rtP -> P_2387 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3209_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3211_0 = _rtDW -> Memory_PreviousInput_hjp ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3213_0 = ( _rtB -> B_392_3206_0 -
B_392_3207_0 ) * _rtP -> P_266 ; } else { _rtB -> B_392_3213_0 = _rtB ->
B_392_3211_0 ; } rtb_B_392_1600_0 = _rtP -> P_2389 * muDoubleScalarAtan2 (
_rtB -> B_392_3213_0 , _rtB -> B_392_3205_0 ) * _rtP -> P_2390 ; B_392_3218_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3205_0 , _rtB -> B_392_3213_0 ) *
muDoubleScalarCos ( rtb_B_392_1600_0 ) ; B_392_3218_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3205_0 , _rtB -> B_392_3213_0 ) *
muDoubleScalarSin ( rtb_B_392_1600_0 ) ; _rtB -> B_392_3219_0 = _rtX ->
integrator_CSTATE_h4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ni . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ni . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2392 ;
B_392_3220_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e2t .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_e2t . Last , _rtDW ->
TransportDelay_IWORK_e2t . Tail , _rtDW -> TransportDelay_IWORK_e2t . Head ,
_rtP -> P_2393 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3222_0 = _rtP -> P_2394 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3222_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3224_0 =
_rtDW -> Memory_PreviousInput_kt ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3226_0 = ( _rtB -> B_392_3219_0 - B_392_3220_0 ) * _rtP -> P_269 ; }
else { _rtB -> B_392_3226_0 = _rtB -> B_392_3224_0 ; } _rtB -> B_392_3227_0 =
_rtX -> integrator_CSTATE_a2 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_lu . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lu . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2397 ;
B_392_3228_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ex .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ex . Last , _rtDW ->
TransportDelay_IWORK_ex . Tail , _rtDW -> TransportDelay_IWORK_ex . Head ,
_rtP -> P_2398 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3230_0 = _rtP -> P_2399 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3230_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3232_0 =
_rtDW -> Memory_PreviousInput_kk ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3234_0 = ( _rtB -> B_392_3227_0 - B_392_3228_0 ) * _rtP -> P_268 ; }
else { _rtB -> B_392_3234_0 = _rtB -> B_392_3232_0 ; } rtb_B_392_1600_0 =
_rtP -> P_2401 * muDoubleScalarAtan2 ( _rtB -> B_392_3234_0 , _rtB ->
B_392_3226_0 ) * _rtP -> P_2402 ; B_392_3239_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3226_0 , _rtB -> B_392_3234_0 ) * muDoubleScalarCos (
rtb_B_392_1600_0 ) ; B_392_3239_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3226_0 , _rtB -> B_392_3234_0 ) * muDoubleScalarSin ( rtb_B_392_1600_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3241_0 = _rtB -> B_392_3240_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3241_0 ,
B_392_3197_0 , B_392_3218_0 , B_392_3239_0 , & _rtB -> PosSeqComputation_ic ,
& _rtDW -> PosSeqComputation_ic , & _rtP -> PosSeqComputation_ic ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3244_0 = _rtB
-> B_392_3243_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3244_0 , B_392_3197_0 , B_392_3218_0 , B_392_3239_0 , & _rtB ->
NegSeqComputation_lx , & _rtDW -> NegSeqComputation_lx , & _rtP ->
NegSeqComputation_lx ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3247_0 = _rtB -> B_392_3246_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3247_0 ,
B_392_3197_0 , B_392_3218_0 , B_392_3239_0 , & _rtB -> ZeroSeqComputation_lw
, & _rtDW -> ZeroSeqComputation_lw , & _rtP -> ZeroSeqComputation_lw ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ic .
B_23_2_0 . im , _rtB -> PosSeqComputation_ic . B_23_2_0 . re ) ; _rtB ->
B_392_3250_0 = _rtP -> P_2406 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_ic . B_23_2_0 . re , _rtB -> PosSeqComputation_ic .
B_23_2_0 . im ) ; _rtB -> B_392_3251_0 = rtb_B_392_2974_0 * rtb_B_392_2122_0
; _rtB -> B_392_3252_0 = _rtX -> integrator_CSTATE_mh ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2408 ; B_392_3253_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p5 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_p5 . Last , _rtDW -> TransportDelay_IWORK_p5 .
Tail , _rtDW -> TransportDelay_IWORK_p5 . Head , _rtP -> P_2409 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3255_0 = _rtP -> P_2410 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3255_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3257_0 = _rtDW -> Memory_PreviousInput_g3 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3259_0 = ( _rtB -> B_392_3252_0 -
B_392_3253_0 ) * _rtP -> P_247 ; } else { _rtB -> B_392_3259_0 = _rtB ->
B_392_3257_0 ; } _rtB -> B_392_3260_0 = _rtX -> integrator_CSTATE_fs ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ld .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ld . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2413 ; B_392_3261_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eu . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_eu . Last , _rtDW -> TransportDelay_IWORK_eu .
Tail , _rtDW -> TransportDelay_IWORK_eu . Head , _rtP -> P_2414 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3263_0 = _rtP -> P_2415 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3263_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3265_0 = _rtDW -> Memory_PreviousInput_o3 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3267_0 = ( _rtB -> B_392_3260_0 -
B_392_3261_0 ) * _rtP -> P_246 ; } else { _rtB -> B_392_3267_0 = _rtB ->
B_392_3265_0 ; } rtb_B_392_2974_0 = _rtP -> P_2417 * muDoubleScalarAtan2 (
_rtB -> B_392_3267_0 , _rtB -> B_392_3259_0 ) * _rtP -> P_2418 ; B_392_3272_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3259_0 , _rtB -> B_392_3267_0 ) *
muDoubleScalarCos ( rtb_B_392_2974_0 ) ; B_392_3272_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3259_0 , _rtB -> B_392_3267_0 ) *
muDoubleScalarSin ( rtb_B_392_2974_0 ) ; _rtB -> B_392_3273_0 = _rtX ->
integrator_CSTATE_bdm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kv . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kv . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2420 ;
B_392_3274_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_i2y .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_i2y . Last , _rtDW ->
TransportDelay_IWORK_i2y . Tail , _rtDW -> TransportDelay_IWORK_i2y . Head ,
_rtP -> P_2421 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3276_0 = _rtP -> P_2422 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3276_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3278_0 =
_rtDW -> Memory_PreviousInput_dl ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3280_0 = ( _rtB -> B_392_3273_0 - B_392_3274_0 ) * _rtP -> P_249 ; }
else { _rtB -> B_392_3280_0 = _rtB -> B_392_3278_0 ; } _rtB -> B_392_3281_0 =
_rtX -> integrator_CSTATE_hn ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ep . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ep . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2425 ;
B_392_3282_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lna .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lna . Last , _rtDW ->
TransportDelay_IWORK_lna . Tail , _rtDW -> TransportDelay_IWORK_lna . Head ,
_rtP -> P_2426 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3284_0 = _rtP -> P_2427 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3284_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3286_0 =
_rtDW -> Memory_PreviousInput_by ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3288_0 = ( _rtB -> B_392_3281_0 - B_392_3282_0 ) * _rtP -> P_248 ; }
else { _rtB -> B_392_3288_0 = _rtB -> B_392_3286_0 ; } rtb_B_392_2974_0 =
_rtP -> P_2429 * muDoubleScalarAtan2 ( _rtB -> B_392_3288_0 , _rtB ->
B_392_3280_0 ) * _rtP -> P_2430 ; B_392_3293_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3280_0 , _rtB -> B_392_3288_0 ) * muDoubleScalarCos (
rtb_B_392_2974_0 ) ; B_392_3293_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3280_0 , _rtB -> B_392_3288_0 ) * muDoubleScalarSin ( rtb_B_392_2974_0
) ; _rtB -> B_392_3294_0 = _rtX -> integrator_CSTATE_nm2 ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2432 ; B_392_3295_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kw . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kw . Last , _rtDW -> TransportDelay_IWORK_kw .
Tail , _rtDW -> TransportDelay_IWORK_kw . Head , _rtP -> P_2433 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3297_0 = _rtP -> P_2434 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3297_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3299_0 = _rtDW -> Memory_PreviousInput_g3o ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3301_0 = ( _rtB -> B_392_3294_0 -
B_392_3295_0 ) * _rtP -> P_251 ; } else { _rtB -> B_392_3301_0 = _rtB ->
B_392_3299_0 ; } _rtB -> B_392_3302_0 = _rtX -> integrator_CSTATE_f3 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eo .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_eo . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2437 ; B_392_3303_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bf . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_bf . Last , _rtDW -> TransportDelay_IWORK_bf .
Tail , _rtDW -> TransportDelay_IWORK_bf . Head , _rtP -> P_2438 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3305_0 = _rtP -> P_2439 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3305_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3307_0 = _rtDW -> Memory_PreviousInput_m1 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3309_0 = ( _rtB -> B_392_3302_0 -
B_392_3303_0 ) * _rtP -> P_250 ; } else { _rtB -> B_392_3309_0 = _rtB ->
B_392_3307_0 ; } rtb_B_392_2974_0 = _rtP -> P_2441 * muDoubleScalarAtan2 (
_rtB -> B_392_3309_0 , _rtB -> B_392_3301_0 ) * _rtP -> P_2442 ; B_392_3314_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3301_0 , _rtB -> B_392_3309_0 ) *
muDoubleScalarCos ( rtb_B_392_2974_0 ) ; B_392_3314_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3301_0 , _rtB -> B_392_3309_0 ) *
muDoubleScalarSin ( rtb_B_392_2974_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_3316_0 = _rtB -> B_392_3315_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3316_0 ,
B_392_3272_0 , B_392_3293_0 , B_392_3314_0 , & _rtB -> PosSeqComputation_nf ,
& _rtDW -> PosSeqComputation_nf , & _rtP -> PosSeqComputation_nf ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3319_0 = _rtB
-> B_392_3318_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3319_0 , B_392_3272_0 , B_392_3293_0 , B_392_3314_0 , & _rtB ->
NegSeqComputation_oh , & _rtDW -> NegSeqComputation_oh , & _rtP ->
NegSeqComputation_oh ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3322_0 = _rtB -> B_392_3321_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3322_0 ,
B_392_3272_0 , B_392_3293_0 , B_392_3314_0 , & _rtB -> ZeroSeqComputation_k0
, & _rtDW -> ZeroSeqComputation_k0 , & _rtP -> ZeroSeqComputation_k0 ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_nf .
B_23_2_0 . im , _rtB -> PosSeqComputation_nf . B_23_2_0 . re ) ; _rtB ->
B_392_3325_0 = _rtP -> P_2446 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_nf . B_23_2_0 . re , _rtB -> PosSeqComputation_nf .
B_23_2_0 . im ) ; ssCallAccelRunBlock ( S , 392 , 3326 , SS_CALL_MDL_OUTPUTS
) ; _rtB -> B_392_1512_0 [ 0 ] = _rtB -> B_392_3176_0 ; _rtB -> B_392_1512_0
[ 1 ] = _rtB -> B_392_3250_0 ; _rtB -> B_392_1512_0 [ 2 ] = _rtB ->
B_392_3251_0 ; _rtB -> B_392_1512_0 [ 3 ] = _rtB -> B_392_3325_0 ;
ssCallAccelRunBlock ( S , 392 , 3328 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3332_0 [ 0 ]
= _rtP -> P_2447 * _rtB -> B_0_14_0 [ 120 ] * _rtP -> P_2450 ; _rtB ->
B_392_3332_0 [ 1 ] = _rtP -> P_2448 * _rtB -> B_0_14_0 [ 121 ] * _rtP ->
P_2450 ; _rtB -> B_392_3332_0 [ 2 ] = _rtP -> P_2449 * _rtB -> B_0_14_0 [ 122
] * _rtP -> P_2450 ; _rtB -> B_392_3336_0 [ 0 ] = _rtP -> P_2451 * _rtB ->
B_0_14_0 [ 65 ] * _rtP -> P_2454 ; _rtB -> B_392_3336_0 [ 1 ] = _rtP ->
P_2452 * _rtB -> B_0_14_0 [ 66 ] * _rtP -> P_2454 ; _rtB -> B_392_3336_0 [ 2
] = _rtP -> P_2453 * _rtB -> B_0_14_0 [ 67 ] * _rtP -> P_2454 ; } _rtB ->
B_392_3372_0 = ( muDoubleScalarSin ( _rtP -> P_2457 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2458 ) * _rtP -> P_2455 + _rtP -> P_2456 ) * _rtB -> B_392_3332_0
[ 0 ] ; _rtB -> B_392_3374_0 = ( muDoubleScalarSin ( _rtP -> P_2461 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2462 ) * _rtP -> P_2459 + _rtP -> P_2460
) * _rtB -> B_392_3332_0 [ 0 ] ; _rtB -> B_392_3376_0 = ( muDoubleScalarSin (
_rtP -> P_2465 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2466 ) * _rtP -> P_2463
+ _rtP -> P_2464 ) * _rtB -> B_392_3332_0 [ 1 ] ; _rtB -> B_392_3378_0 = (
muDoubleScalarSin ( _rtP -> P_2469 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2470
) * _rtP -> P_2467 + _rtP -> P_2468 ) * _rtB -> B_392_3332_0 [ 1 ] ; _rtB ->
B_392_3380_0 = ( muDoubleScalarSin ( _rtP -> P_2473 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2474 ) * _rtP -> P_2471 + _rtP -> P_2472 ) * _rtB -> B_392_3332_0
[ 2 ] ; _rtB -> B_392_3382_0 = ( muDoubleScalarSin ( _rtP -> P_2477 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2478 ) * _rtP -> P_2475 + _rtP -> P_2476
) * _rtB -> B_392_3332_0 [ 2 ] ; B_392_3383_0 = _rtP -> P_2479 *
rtb_B_392_239_0 ; _rtB -> B_392_3385_0 = ( muDoubleScalarSin ( _rtP -> P_2482
* ssGetTaskTime ( S , 0 ) + _rtP -> P_2483 ) * _rtP -> P_2480 + _rtP ->
P_2481 ) * _rtB -> B_392_3336_0 [ 0 ] ; _rtB -> B_392_3387_0 = (
muDoubleScalarSin ( _rtP -> P_2486 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2487
) * _rtP -> P_2484 + _rtP -> P_2485 ) * _rtB -> B_392_3336_0 [ 0 ] ; _rtB ->
B_392_3389_0 = ( muDoubleScalarSin ( _rtP -> P_2490 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2491 ) * _rtP -> P_2488 + _rtP -> P_2489 ) * _rtB -> B_392_3336_0
[ 1 ] ; _rtB -> B_392_3391_0 = ( muDoubleScalarSin ( _rtP -> P_2494 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2495 ) * _rtP -> P_2492 + _rtP -> P_2493
) * _rtB -> B_392_3336_0 [ 1 ] ; _rtB -> B_392_3393_0 = ( muDoubleScalarSin (
_rtP -> P_2498 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2499 ) * _rtP -> P_2496
+ _rtP -> P_2497 ) * _rtB -> B_392_3336_0 [ 2 ] ; _rtB -> B_392_3395_0 = (
muDoubleScalarSin ( _rtP -> P_2502 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2503
) * _rtP -> P_2500 + _rtP -> P_2501 ) * _rtB -> B_392_3336_0 [ 2 ] ; _rtB ->
B_392_3397_0 = ( muDoubleScalarSin ( _rtP -> P_2506 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2507 ) * _rtP -> P_2504 + _rtP -> P_2505 ) * _rtB -> B_392_3332_0
[ 0 ] ; _rtB -> B_392_3399_0 = ( muDoubleScalarSin ( _rtP -> P_2510 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2511 ) * _rtP -> P_2508 + _rtP -> P_2509
) * _rtB -> B_392_3332_0 [ 0 ] ; _rtB -> B_392_3401_0 = ( muDoubleScalarSin (
_rtP -> P_2514 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2515 ) * _rtP -> P_2512
+ _rtP -> P_2513 ) * _rtB -> B_392_3332_0 [ 1 ] ; _rtB -> B_392_3403_0 = (
muDoubleScalarSin ( _rtP -> P_2518 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2519
) * _rtP -> P_2516 + _rtP -> P_2517 ) * _rtB -> B_392_3332_0 [ 1 ] ; _rtB ->
B_392_3405_0 = ( muDoubleScalarSin ( _rtP -> P_2522 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2523 ) * _rtP -> P_2520 + _rtP -> P_2521 ) * _rtB -> B_392_3332_0
[ 2 ] ; _rtB -> B_392_3407_0 = ( muDoubleScalarSin ( _rtP -> P_2526 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2527 ) * _rtP -> P_2524 + _rtP -> P_2525
) * _rtB -> B_392_3332_0 [ 2 ] ; _rtB -> B_392_3409_0 = ( muDoubleScalarSin (
_rtP -> P_2530 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2531 ) * _rtP -> P_2528
+ _rtP -> P_2529 ) * _rtB -> B_392_3336_0 [ 0 ] ; _rtB -> B_392_3411_0 = (
muDoubleScalarSin ( _rtP -> P_2534 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2535
) * _rtP -> P_2532 + _rtP -> P_2533 ) * _rtB -> B_392_3336_0 [ 0 ] ; _rtB ->
B_392_3413_0 = ( muDoubleScalarSin ( _rtP -> P_2538 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2539 ) * _rtP -> P_2536 + _rtP -> P_2537 ) * _rtB -> B_392_3336_0
[ 1 ] ; _rtB -> B_392_3415_0 = ( muDoubleScalarSin ( _rtP -> P_2542 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2543 ) * _rtP -> P_2540 + _rtP -> P_2541
) * _rtB -> B_392_3336_0 [ 1 ] ; _rtB -> B_392_3417_0 = ( muDoubleScalarSin (
_rtP -> P_2546 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2547 ) * _rtP -> P_2544
+ _rtP -> P_2545 ) * _rtB -> B_392_3336_0 [ 2 ] ; _rtB -> B_392_3419_0 = (
muDoubleScalarSin ( _rtP -> P_2550 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2551
) * _rtP -> P_2548 + _rtP -> P_2549 ) * _rtB -> B_392_3336_0 [ 2 ] ;
B_392_3420_0 = _rtP -> P_2552 * rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_3423_0 >= _rtP -> P_2558 ) {
rtb_B_392_3426_0 = _rtB -> B_392_3422_0 ; } else { rtb_B_392_3426_0 =
rt_Lookup ( _rtP -> P_2556 , 6 , ssGetTaskTime ( S , 2 ) , _rtP -> P_2557 ) ;
} if ( _rtB -> B_392_3427_0 >= _rtP -> P_2561 ) { _rtB -> B_392_3430_0 =
rtb_B_392_3426_0 ; } else { _rtB -> B_392_3430_0 = _rtB -> B_392_3428_0 ; } }
_rtB -> B_392_3432_0 = rt_Lookup ( _rtP -> P_2562 , 5 , ssGetT ( S ) , _rtP
-> P_2563 ) ; if ( _rtB -> B_392_3421_0 >= _rtP -> P_2564 ) { _rtB ->
B_392_3433_0 = _rtB -> B_392_3430_0 ; } else { _rtB -> B_392_3433_0 = _rtB ->
B_392_3432_0 ; } if ( ( _rtDW -> TimeStampA_mm >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_eo >= ssGetT ( S ) ) ) { _rtB -> B_392_3434_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_mm ; lastU = & _rtDW -> LastUAtTimeA_ed
; if ( _rtDW -> TimeStampA_mm < _rtDW -> TimeStampB_eo ) { if ( _rtDW ->
TimeStampB_eo < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_eo ;
lastU = & _rtDW -> LastUAtTimeB_i ; } } else { if ( _rtDW -> TimeStampA_mm >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_eo ; lastU = & _rtDW
-> LastUAtTimeB_i ; } } _rtB -> B_392_3434_0 = ( _rtB -> B_392_3432_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_3440_0 >= _rtP -> P_2568 ) {
_rtB -> B_392_3442_0 = rtb_B_392_3426_0 ; } else { _rtB -> B_392_3442_0 =
_rtB -> B_392_3428_0 ; } } _rtB -> B_392_3444_0 = rt_Lookup ( _rtP -> P_2569
, 5 , ssGetT ( S ) , _rtP -> P_2570 ) ; if ( _rtB -> B_392_3439_0 >= _rtP ->
P_2571 ) { _rtB -> B_392_3445_0 = _rtB -> B_392_3442_0 ; } else { _rtB ->
B_392_3445_0 = _rtB -> B_392_3444_0 ; } if ( ( _rtDW -> TimeStampA_j >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_n >= ssGetT ( S ) ) ) { _rtB ->
B_392_3446_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_j ; lastU
= & _rtDW -> LastUAtTimeA_dt ; if ( _rtDW -> TimeStampA_j < _rtDW ->
TimeStampB_n ) { if ( _rtDW -> TimeStampB_n < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_n ; lastU = & _rtDW -> LastUAtTimeB_po
; } } else { if ( _rtDW -> TimeStampA_j >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_n ; lastU = & _rtDW -> LastUAtTimeB_po ; } } _rtB ->
B_392_3446_0 = ( _rtB -> B_392_3444_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_3452_0 >= _rtP -> P_2575 ) { _rtB -> B_392_3454_0 =
rtb_B_392_3426_0 ; } else { _rtB -> B_392_3454_0 = _rtB -> B_392_3428_0 ; } }
_rtB -> B_392_3456_0 = rt_Lookup ( _rtP -> P_2576 , 5 , ssGetT ( S ) , _rtP
-> P_2577 ) ; if ( _rtB -> B_392_3451_0 >= _rtP -> P_2578 ) { _rtB ->
B_392_3457_0 = _rtB -> B_392_3454_0 ; } else { _rtB -> B_392_3457_0 = _rtB ->
B_392_3456_0 ; } if ( ( _rtDW -> TimeStampA_g >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_d >= ssGetT ( S ) ) ) { _rtB -> B_392_3458_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_g ; lastU = & _rtDW -> LastUAtTimeA_g ;
if ( _rtDW -> TimeStampA_g < _rtDW -> TimeStampB_d ) { if ( _rtDW ->
TimeStampB_d < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_d ;
lastU = & _rtDW -> LastUAtTimeB_mm ; } } else { if ( _rtDW -> TimeStampA_g >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_d ; lastU = & _rtDW ->
LastUAtTimeB_mm ; } } _rtB -> B_392_3458_0 = ( _rtB -> B_392_3456_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } _rtB -> B_392_3475_0 = _rtX ->
integrator_CSTATE_dp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ca . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ca . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2581 ;
B_392_3476_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kd . Last , _rtDW ->
TransportDelay_IWORK_kd . Tail , _rtDW -> TransportDelay_IWORK_kd . Head ,
_rtP -> P_2582 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3478_0 = _rtP -> P_2583 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3478_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3480_0 =
_rtDW -> Memory_PreviousInput_ev ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3482_0 = ( _rtB -> B_392_3475_0 - B_392_3476_0 ) * _rtP -> P_277 ; }
else { _rtB -> B_392_3482_0 = _rtB -> B_392_3480_0 ; } _rtB -> B_392_3483_0 =
_rtX -> integrator_CSTATE_hz ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_in . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_in . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2586 ;
B_392_3484_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_mm . Last , _rtDW ->
TransportDelay_IWORK_mm . Tail , _rtDW -> TransportDelay_IWORK_mm . Head ,
_rtP -> P_2587 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3486_0 = _rtP -> P_2588 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3486_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3488_0 =
_rtDW -> Memory_PreviousInput_fis ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3490_0 = ( _rtB -> B_392_3483_0 - B_392_3484_0 ) * _rtP -> P_276 ; }
else { _rtB -> B_392_3490_0 = _rtB -> B_392_3488_0 ; } rtb_B_392_3426_0 =
_rtP -> P_2590 * muDoubleScalarAtan2 ( _rtB -> B_392_3490_0 , _rtB ->
B_392_3482_0 ) * _rtP -> P_2591 ; B_392_3495_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3482_0 , _rtB -> B_392_3490_0 ) * muDoubleScalarCos (
rtb_B_392_3426_0 ) ; B_392_3495_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3482_0 , _rtB -> B_392_3490_0 ) * muDoubleScalarSin ( rtb_B_392_3426_0
) ; _rtB -> B_392_3496_0 = _rtX -> integrator_CSTATE_oa ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2593 ; B_392_3497_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gn . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_gn . Last , _rtDW -> TransportDelay_IWORK_gn .
Tail , _rtDW -> TransportDelay_IWORK_gn . Head , _rtP -> P_2594 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3499_0 = _rtP -> P_2595 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3499_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3501_0 = _rtDW -> Memory_PreviousInput_ij ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3503_0 = ( _rtB -> B_392_3496_0 -
B_392_3497_0 ) * _rtP -> P_279 ; } else { _rtB -> B_392_3503_0 = _rtB ->
B_392_3501_0 ; } _rtB -> B_392_3504_0 = _rtX -> integrator_CSTATE_ow ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d0 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2598 ; B_392_3505_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fd . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_fd . Last , _rtDW -> TransportDelay_IWORK_fd .
Tail , _rtDW -> TransportDelay_IWORK_fd . Head , _rtP -> P_2599 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3507_0 = _rtP -> P_2600 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3507_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3509_0 = _rtDW -> Memory_PreviousInput_jy ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3511_0 = ( _rtB -> B_392_3504_0 -
B_392_3505_0 ) * _rtP -> P_278 ; } else { _rtB -> B_392_3511_0 = _rtB ->
B_392_3509_0 ; } rtb_B_392_3426_0 = _rtP -> P_2602 * muDoubleScalarAtan2 (
_rtB -> B_392_3511_0 , _rtB -> B_392_3503_0 ) * _rtP -> P_2603 ; B_392_3516_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3503_0 , _rtB -> B_392_3511_0 ) *
muDoubleScalarCos ( rtb_B_392_3426_0 ) ; B_392_3516_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3503_0 , _rtB -> B_392_3511_0 ) *
muDoubleScalarSin ( rtb_B_392_3426_0 ) ; _rtB -> B_392_3517_0 = _rtX ->
integrator_CSTATE_n3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_d53 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_d53 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2605 ;
B_392_3518_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gz .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gz . Last , _rtDW ->
TransportDelay_IWORK_gz . Tail , _rtDW -> TransportDelay_IWORK_gz . Head ,
_rtP -> P_2606 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3520_0 = _rtP -> P_2607 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3520_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3522_0 =
_rtDW -> Memory_PreviousInput_grf ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3524_0 = ( _rtB -> B_392_3517_0 - B_392_3518_0 ) * _rtP -> P_281 ; }
else { _rtB -> B_392_3524_0 = _rtB -> B_392_3522_0 ; } _rtB -> B_392_3525_0 =
_rtX -> integrator_CSTATE_dn3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ee . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ee . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2610 ;
B_392_3526_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ma4 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ma4 . Last , _rtDW ->
TransportDelay_IWORK_ma4 . Tail , _rtDW -> TransportDelay_IWORK_ma4 . Head ,
_rtP -> P_2611 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3528_0 = _rtP -> P_2612 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3528_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3530_0 =
_rtDW -> Memory_PreviousInput_pe ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3532_0 = ( _rtB -> B_392_3525_0 - B_392_3526_0 ) * _rtP -> P_280 ; }
else { _rtB -> B_392_3532_0 = _rtB -> B_392_3530_0 ; } rtb_B_392_3426_0 =
_rtP -> P_2614 * muDoubleScalarAtan2 ( _rtB -> B_392_3532_0 , _rtB ->
B_392_3524_0 ) * _rtP -> P_2615 ; B_392_3537_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3524_0 , _rtB -> B_392_3532_0 ) * muDoubleScalarCos (
rtb_B_392_3426_0 ) ; B_392_3537_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3524_0 , _rtB -> B_392_3532_0 ) * muDoubleScalarSin ( rtb_B_392_3426_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3539_0 = _rtB -> B_392_3538_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3539_0 ,
B_392_3495_0 , B_392_3516_0 , B_392_3537_0 , & _rtB -> PosSeqComputation_pa ,
& _rtDW -> PosSeqComputation_pa , & _rtP -> PosSeqComputation_pa ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3542_0 = _rtB
-> B_392_3541_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3542_0 , B_392_3495_0 , B_392_3516_0 , B_392_3537_0 , & _rtB ->
NegSeqComputation_i , & _rtDW -> NegSeqComputation_i , & _rtP ->
NegSeqComputation_i ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3545_0 = _rtB -> B_392_3544_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3545_0 ,
B_392_3495_0 , B_392_3516_0 , B_392_3537_0 , & _rtB -> ZeroSeqComputation_mp
, & _rtDW -> ZeroSeqComputation_mp , & _rtP -> ZeroSeqComputation_mp ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_pa . B_23_2_0
. re , _rtB -> PosSeqComputation_pa . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_pa . B_23_2_0 . im , _rtB ->
PosSeqComputation_pa . B_23_2_0 . re ) ; _rtB -> B_392_3548_0 = _rtX ->
integrator_CSTATE_lx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_gn . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_gn . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2620 ;
B_392_3549_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kl . Last , _rtDW ->
TransportDelay_IWORK_kl . Tail , _rtDW -> TransportDelay_IWORK_kl . Head ,
_rtP -> P_2621 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3551_0 = _rtP -> P_2622 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3551_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3553_0 =
_rtDW -> Memory_PreviousInput_ic ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3555_0 = ( _rtB -> B_392_3548_0 - B_392_3549_0 ) * _rtP -> P_283 ; }
else { _rtB -> B_392_3555_0 = _rtB -> B_392_3553_0 ; } _rtB -> B_392_3556_0 =
_rtX -> integrator_CSTATE_am ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_hq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2625 ;
B_392_3557_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_er .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_er . Last , _rtDW ->
TransportDelay_IWORK_er . Tail , _rtDW -> TransportDelay_IWORK_er . Head ,
_rtP -> P_2626 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3559_0 = _rtP -> P_2627 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3559_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3561_0 =
_rtDW -> Memory_PreviousInput_k0 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3563_0 = ( _rtB -> B_392_3556_0 - B_392_3557_0 ) * _rtP -> P_282 ; }
else { _rtB -> B_392_3563_0 = _rtB -> B_392_3561_0 ; } rtb_B_392_3426_0 =
_rtP -> P_2629 * muDoubleScalarAtan2 ( _rtB -> B_392_3563_0 , _rtB ->
B_392_3555_0 ) * _rtP -> P_2630 ; B_392_3568_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3555_0 , _rtB -> B_392_3563_0 ) * muDoubleScalarCos (
rtb_B_392_3426_0 ) ; B_392_3568_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3555_0 , _rtB -> B_392_3563_0 ) * muDoubleScalarSin ( rtb_B_392_3426_0
) ; _rtB -> B_392_3569_0 = _rtX -> integrator_CSTATE_gew ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2632 ; B_392_3570_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hq . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hq . Last , _rtDW -> TransportDelay_IWORK_hq .
Tail , _rtDW -> TransportDelay_IWORK_hq . Head , _rtP -> P_2633 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3572_0 = _rtP -> P_2634 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3572_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3574_0 = _rtDW -> Memory_PreviousInput_ofm ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3576_0 = ( _rtB -> B_392_3569_0 -
B_392_3570_0 ) * _rtP -> P_285 ; } else { _rtB -> B_392_3576_0 = _rtB ->
B_392_3574_0 ; } _rtB -> B_392_3577_0 = _rtX -> integrator_CSTATE_br ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dy .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dy . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2637 ; B_392_3578_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hj2 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hj2 . Last , _rtDW -> TransportDelay_IWORK_hj2
. Tail , _rtDW -> TransportDelay_IWORK_hj2 . Head , _rtP -> P_2638 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3580_0 = _rtP -> P_2639 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3580_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3582_0 = _rtDW -> Memory_PreviousInput_ico ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3584_0 = ( _rtB -> B_392_3577_0 -
B_392_3578_0 ) * _rtP -> P_284 ; } else { _rtB -> B_392_3584_0 = _rtB ->
B_392_3582_0 ; } rtb_B_392_3426_0 = _rtP -> P_2641 * muDoubleScalarAtan2 (
_rtB -> B_392_3584_0 , _rtB -> B_392_3576_0 ) * _rtP -> P_2642 ; B_392_3589_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3576_0 , _rtB -> B_392_3584_0 ) *
muDoubleScalarCos ( rtb_B_392_3426_0 ) ; B_392_3589_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3576_0 , _rtB -> B_392_3584_0 ) *
muDoubleScalarSin ( rtb_B_392_3426_0 ) ; _rtB -> B_392_3590_0 = _rtX ->
integrator_CSTATE_bl ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_l5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_l5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2644 ;
B_392_3591_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_atb .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_atb . Last , _rtDW ->
TransportDelay_IWORK_atb . Tail , _rtDW -> TransportDelay_IWORK_atb . Head ,
_rtP -> P_2645 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3593_0 = _rtP -> P_2646 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3593_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3595_0 =
_rtDW -> Memory_PreviousInput_ji ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3597_0 = ( _rtB -> B_392_3590_0 - B_392_3591_0 ) * _rtP -> P_287 ; }
else { _rtB -> B_392_3597_0 = _rtB -> B_392_3595_0 ; } _rtB -> B_392_3598_0 =
_rtX -> integrator_CSTATE_oe ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_oy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oy . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2649 ;
B_392_3599_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_or .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_or . Last , _rtDW ->
TransportDelay_IWORK_or . Tail , _rtDW -> TransportDelay_IWORK_or . Head ,
_rtP -> P_2650 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3601_0 = _rtP -> P_2651 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3601_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3603_0 =
_rtDW -> Memory_PreviousInput_nm ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3605_0 = ( _rtB -> B_392_3598_0 - B_392_3599_0 ) * _rtP -> P_286 ; }
else { _rtB -> B_392_3605_0 = _rtB -> B_392_3603_0 ; } rtb_B_392_3426_0 =
_rtP -> P_2653 * muDoubleScalarAtan2 ( _rtB -> B_392_3605_0 , _rtB ->
B_392_3597_0 ) * _rtP -> P_2654 ; B_392_3610_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3597_0 , _rtB -> B_392_3605_0 ) * muDoubleScalarCos (
rtb_B_392_3426_0 ) ; B_392_3610_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3597_0 , _rtB -> B_392_3605_0 ) * muDoubleScalarSin ( rtb_B_392_3426_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3612_0 = _rtB -> B_392_3611_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3612_0 ,
B_392_3568_0 , B_392_3589_0 , B_392_3610_0 , & _rtB -> PosSeqComputation_g ,
& _rtDW -> PosSeqComputation_g , & _rtP -> PosSeqComputation_g ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3615_0 = _rtB
-> B_392_3614_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3615_0 , B_392_3568_0 , B_392_3589_0 , B_392_3610_0 , & _rtB ->
NegSeqComputation_lk , & _rtDW -> NegSeqComputation_lk , & _rtP ->
NegSeqComputation_lk ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3618_0 = _rtB -> B_392_3617_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3618_0 ,
B_392_3568_0 , B_392_3589_0 , B_392_3610_0 , & _rtB -> ZeroSeqComputation_od
, & _rtDW -> ZeroSeqComputation_od , & _rtP -> ZeroSeqComputation_od ) ;
rtb_B_392_3426_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_g . B_23_2_0 . re , _rtB -> PosSeqComputation_g . B_23_2_0
. im ) * _rtP -> P_2658 ; muDoubleScalarSinCos ( _rtP -> P_2661 * ( _rtP ->
P_2659 * rtb_B_392_237_0 - _rtP -> P_2660 * muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_g . B_23_2_0 . im , _rtB -> PosSeqComputation_g . B_23_2_0
. re ) ) , & rtb_B_392_2974_0 , & rtb_B_392_2122_0 ) ; _rtB -> B_392_3628_0 =
rtb_B_392_3426_0 * rtb_B_392_2122_0 ; _rtB -> B_392_3629_0 = _rtX ->
integrator_CSTATE_k3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cmj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_cmj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2663 ;
B_392_3630_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dw .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dw . Last , _rtDW ->
TransportDelay_IWORK_dw . Tail , _rtDW -> TransportDelay_IWORK_dw . Head ,
_rtP -> P_2664 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3632_0 = _rtP -> P_2665 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3632_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3634_0 =
_rtDW -> Memory_PreviousInput_jwz ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3636_0 = ( _rtB -> B_392_3629_0 - B_392_3630_0 ) * _rtP -> P_289 ; }
else { _rtB -> B_392_3636_0 = _rtB -> B_392_3634_0 ; } _rtB -> B_392_3637_0 =
_rtX -> integrator_CSTATE_ay ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ojz . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2668
; B_392_3638_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_c5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_c5 . Last , _rtDW ->
TransportDelay_IWORK_c5 . Tail , _rtDW -> TransportDelay_IWORK_c5 . Head ,
_rtP -> P_2669 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3640_0 = _rtP -> P_2670 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3640_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3642_0 =
_rtDW -> Memory_PreviousInput_lb ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3644_0 = ( _rtB -> B_392_3637_0 - B_392_3638_0 ) * _rtP -> P_288 ; }
else { _rtB -> B_392_3644_0 = _rtB -> B_392_3642_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2672 * muDoubleScalarAtan2 ( _rtB -> B_392_3644_0 , _rtB ->
B_392_3636_0 ) * _rtP -> P_2673 ; B_392_3649_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3636_0 , _rtB -> B_392_3644_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_3649_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3636_0 , _rtB -> B_392_3644_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; _rtB -> B_392_3650_0 = _rtX -> integrator_CSTATE_hx ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2675 ; B_392_3651_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hj0 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_hj0 . Last , _rtDW -> TransportDelay_IWORK_hj0
. Tail , _rtDW -> TransportDelay_IWORK_hj0 . Head , _rtP -> P_2676 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3653_0 = _rtP -> P_2677 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3653_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3655_0 = _rtDW -> Memory_PreviousInput_ao ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3657_0 = ( _rtB -> B_392_3650_0 -
B_392_3651_0 ) * _rtP -> P_291 ; } else { _rtB -> B_392_3657_0 = _rtB ->
B_392_3655_0 ; } _rtB -> B_392_3658_0 = _rtX -> integrator_CSTATE_nc ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_cvh . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2680 ; B_392_3659_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_da . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_da . Last , _rtDW -> TransportDelay_IWORK_da .
Tail , _rtDW -> TransportDelay_IWORK_da . Head , _rtP -> P_2681 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3661_0 = _rtP -> P_2682 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3661_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3663_0 = _rtDW -> Memory_PreviousInput_kf ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3665_0 = ( _rtB -> B_392_3658_0 -
B_392_3659_0 ) * _rtP -> P_290 ; } else { _rtB -> B_392_3665_0 = _rtB ->
B_392_3663_0 ; } rtb_B_392_2122_0 = _rtP -> P_2684 * muDoubleScalarAtan2 (
_rtB -> B_392_3665_0 , _rtB -> B_392_3657_0 ) * _rtP -> P_2685 ; B_392_3670_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3657_0 , _rtB -> B_392_3665_0 ) *
muDoubleScalarCos ( rtb_B_392_2122_0 ) ; B_392_3670_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3657_0 , _rtB -> B_392_3665_0 ) *
muDoubleScalarSin ( rtb_B_392_2122_0 ) ; _rtB -> B_392_3671_0 = _rtX ->
integrator_CSTATE_jf ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ig . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ig . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2687 ;
B_392_3672_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_j0 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_j0 . Last , _rtDW ->
TransportDelay_IWORK_j0 . Tail , _rtDW -> TransportDelay_IWORK_j0 . Head ,
_rtP -> P_2688 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3674_0 = _rtP -> P_2689 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3674_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3676_0 =
_rtDW -> Memory_PreviousInput_pzv ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3678_0 = ( _rtB -> B_392_3671_0 - B_392_3672_0 ) * _rtP -> P_293 ; }
else { _rtB -> B_392_3678_0 = _rtB -> B_392_3676_0 ; } _rtB -> B_392_3679_0 =
_rtX -> integrator_CSTATE_mc ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_gf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gf . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2692 ;
B_392_3680_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nd .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nd . Last , _rtDW ->
TransportDelay_IWORK_nd . Tail , _rtDW -> TransportDelay_IWORK_nd . Head ,
_rtP -> P_2693 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3682_0 = _rtP -> P_2694 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3682_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3684_0 =
_rtDW -> Memory_PreviousInput_kv ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3686_0 = ( _rtB -> B_392_3679_0 - B_392_3680_0 ) * _rtP -> P_292 ; }
else { _rtB -> B_392_3686_0 = _rtB -> B_392_3684_0 ; } rtb_B_392_2122_0 =
_rtP -> P_2696 * muDoubleScalarAtan2 ( _rtB -> B_392_3686_0 , _rtB ->
B_392_3678_0 ) * _rtP -> P_2697 ; B_392_3691_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3678_0 , _rtB -> B_392_3686_0 ) * muDoubleScalarCos (
rtb_B_392_2122_0 ) ; B_392_3691_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3678_0 , _rtB -> B_392_3686_0 ) * muDoubleScalarSin ( rtb_B_392_2122_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3693_0 = _rtB -> B_392_3692_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3693_0 ,
B_392_3649_0 , B_392_3670_0 , B_392_3691_0 , & _rtB -> PosSeqComputation_ni ,
& _rtDW -> PosSeqComputation_ni , & _rtP -> PosSeqComputation_ni ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3696_0 = _rtB
-> B_392_3695_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3696_0 , B_392_3649_0 , B_392_3670_0 , B_392_3691_0 , & _rtB ->
NegSeqComputation_n5 , & _rtDW -> NegSeqComputation_n5 , & _rtP ->
NegSeqComputation_n5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3699_0 = _rtB -> B_392_3698_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3699_0 ,
B_392_3649_0 , B_392_3670_0 , B_392_3691_0 , & _rtB -> ZeroSeqComputation_fl
, & _rtDW -> ZeroSeqComputation_fl , & _rtP -> ZeroSeqComputation_fl ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ni .
B_23_2_0 . im , _rtB -> PosSeqComputation_ni . B_23_2_0 . re ) ; _rtB ->
B_392_3702_0 = _rtP -> P_2701 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_ni . B_23_2_0 . re , _rtB -> PosSeqComputation_ni .
B_23_2_0 . im ) ; _rtB -> B_392_3703_0 = rtb_B_392_3426_0 * rtb_B_392_2974_0
; _rtB -> B_392_3704_0 = _rtX -> integrator_CSTATE_av ; { real_T * * uBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hs . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hs .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2703 ; B_392_3705_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ou . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ou . Last , _rtDW -> TransportDelay_IWORK_ou .
Tail , _rtDW -> TransportDelay_IWORK_ou . Head , _rtP -> P_2704 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3707_0 = _rtP -> P_2705 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3707_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3709_0 = _rtDW -> Memory_PreviousInput_m2 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3711_0 = ( _rtB -> B_392_3704_0 -
B_392_3705_0 ) * _rtP -> P_271 ; } else { _rtB -> B_392_3711_0 = _rtB ->
B_392_3709_0 ; } _rtB -> B_392_3712_0 = _rtX -> integrator_CSTATE_gy ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o0x . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2708 ; B_392_3713_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_my . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_my . Last , _rtDW -> TransportDelay_IWORK_my .
Tail , _rtDW -> TransportDelay_IWORK_my . Head , _rtP -> P_2709 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3715_0 = _rtP -> P_2710 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3715_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3717_0 = _rtDW -> Memory_PreviousInput_il ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3719_0 = ( _rtB -> B_392_3712_0 -
B_392_3713_0 ) * _rtP -> P_270 ; } else { _rtB -> B_392_3719_0 = _rtB ->
B_392_3717_0 ; } rtb_B_392_3426_0 = _rtP -> P_2712 * muDoubleScalarAtan2 (
_rtB -> B_392_3719_0 , _rtB -> B_392_3711_0 ) * _rtP -> P_2713 ; B_392_3724_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3711_0 , _rtB -> B_392_3719_0 ) *
muDoubleScalarCos ( rtb_B_392_3426_0 ) ; B_392_3724_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3711_0 , _rtB -> B_392_3719_0 ) *
muDoubleScalarSin ( rtb_B_392_3426_0 ) ; _rtB -> B_392_3725_0 = _rtX ->
integrator_CSTATE_i3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fy . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fy . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2715 ;
B_392_3726_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ayh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ayh . Last , _rtDW ->
TransportDelay_IWORK_ayh . Tail , _rtDW -> TransportDelay_IWORK_ayh . Head ,
_rtP -> P_2716 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3728_0 = _rtP -> P_2717 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3728_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3730_0 =
_rtDW -> Memory_PreviousInput_pi ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3732_0 = ( _rtB -> B_392_3725_0 - B_392_3726_0 ) * _rtP -> P_273 ; }
else { _rtB -> B_392_3732_0 = _rtB -> B_392_3730_0 ; } _rtB -> B_392_3733_0 =
_rtX -> integrator_CSTATE_kg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_n0 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2720 ;
B_392_3734_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jyu .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_jyu . Last , _rtDW ->
TransportDelay_IWORK_jyu . Tail , _rtDW -> TransportDelay_IWORK_jyu . Head ,
_rtP -> P_2721 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3736_0 = _rtP -> P_2722 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3736_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3738_0 =
_rtDW -> Memory_PreviousInput_ol ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3740_0 = ( _rtB -> B_392_3733_0 - B_392_3734_0 ) * _rtP -> P_272 ; }
else { _rtB -> B_392_3740_0 = _rtB -> B_392_3738_0 ; } rtb_B_392_3426_0 =
_rtP -> P_2724 * muDoubleScalarAtan2 ( _rtB -> B_392_3740_0 , _rtB ->
B_392_3732_0 ) * _rtP -> P_2725 ; B_392_3745_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3732_0 , _rtB -> B_392_3740_0 ) * muDoubleScalarCos (
rtb_B_392_3426_0 ) ; B_392_3745_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3732_0 , _rtB -> B_392_3740_0 ) * muDoubleScalarSin ( rtb_B_392_3426_0
) ; _rtB -> B_392_3746_0 = _rtX -> integrator_CSTATE_fqj ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2727 ; B_392_3747_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ps . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ps . Last , _rtDW -> TransportDelay_IWORK_ps .
Tail , _rtDW -> TransportDelay_IWORK_ps . Head , _rtP -> P_2728 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3749_0 = _rtP -> P_2729 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3749_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3751_0 = _rtDW -> Memory_PreviousInput_cj ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3753_0 = ( _rtB -> B_392_3746_0 -
B_392_3747_0 ) * _rtP -> P_275 ; } else { _rtB -> B_392_3753_0 = _rtB ->
B_392_3751_0 ; } _rtB -> B_392_3754_0 = _rtX -> integrator_CSTATE_gii ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_k4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2732 ; B_392_3755_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ab . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ab . Last , _rtDW -> TransportDelay_IWORK_ab .
Tail , _rtDW -> TransportDelay_IWORK_ab . Head , _rtP -> P_2733 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3757_0 = _rtP -> P_2734 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3757_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3759_0 = _rtDW -> Memory_PreviousInput_imb ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3761_0 = ( _rtB -> B_392_3754_0 -
B_392_3755_0 ) * _rtP -> P_274 ; } else { _rtB -> B_392_3761_0 = _rtB ->
B_392_3759_0 ; } rtb_B_392_3426_0 = _rtP -> P_2736 * muDoubleScalarAtan2 (
_rtB -> B_392_3761_0 , _rtB -> B_392_3753_0 ) * _rtP -> P_2737 ; B_392_3766_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3753_0 , _rtB -> B_392_3761_0 ) *
muDoubleScalarCos ( rtb_B_392_3426_0 ) ; B_392_3766_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3753_0 , _rtB -> B_392_3761_0 ) *
muDoubleScalarSin ( rtb_B_392_3426_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_3768_0 = _rtB -> B_392_3767_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3768_0 ,
B_392_3724_0 , B_392_3745_0 , B_392_3766_0 , & _rtB -> PosSeqComputation_j ,
& _rtDW -> PosSeqComputation_j , & _rtP -> PosSeqComputation_j ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3771_0 = _rtB
-> B_392_3770_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3771_0 , B_392_3724_0 , B_392_3745_0 , B_392_3766_0 , & _rtB ->
NegSeqComputation_oj , & _rtDW -> NegSeqComputation_oj , & _rtP ->
NegSeqComputation_oj ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3774_0 = _rtB -> B_392_3773_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3774_0 ,
B_392_3724_0 , B_392_3745_0 , B_392_3766_0 , & _rtB -> ZeroSeqComputation_of
, & _rtDW -> ZeroSeqComputation_of , & _rtP -> ZeroSeqComputation_of ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_j .
B_23_2_0 . im , _rtB -> PosSeqComputation_j . B_23_2_0 . re ) ; _rtB ->
B_392_3777_0 = _rtP -> P_2741 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_j . B_23_2_0 . re , _rtB -> PosSeqComputation_j . B_23_2_0
. im ) ; ssCallAccelRunBlock ( S , 392 , 3778 , SS_CALL_MDL_OUTPUTS ) ; _rtB
-> B_392_1512_0 [ 0 ] = _rtB -> B_392_3628_0 ; _rtB -> B_392_1512_0 [ 1 ] =
_rtB -> B_392_3702_0 ; _rtB -> B_392_1512_0 [ 2 ] = _rtB -> B_392_3703_0 ;
_rtB -> B_392_1512_0 [ 3 ] = _rtB -> B_392_3777_0 ; ssCallAccelRunBlock ( S ,
392 , 3780 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_392_3784_0 [ 0 ] = _rtP -> P_2742 * _rtB ->
B_0_14_0 [ 123 ] * _rtP -> P_2745 ; _rtB -> B_392_3784_0 [ 1 ] = _rtP ->
P_2743 * _rtB -> B_0_14_0 [ 124 ] * _rtP -> P_2745 ; _rtB -> B_392_3784_0 [ 2
] = _rtP -> P_2744 * _rtB -> B_0_14_0 [ 125 ] * _rtP -> P_2745 ; _rtB ->
B_392_3788_0 [ 0 ] = _rtP -> P_2746 * _rtB -> B_0_14_0 [ 68 ] * _rtP ->
P_2749 ; _rtB -> B_392_3788_0 [ 1 ] = _rtP -> P_2747 * _rtB -> B_0_14_0 [ 69
] * _rtP -> P_2749 ; _rtB -> B_392_3788_0 [ 2 ] = _rtP -> P_2748 * _rtB ->
B_0_14_0 [ 70 ] * _rtP -> P_2749 ; } _rtB -> B_392_3824_0 = (
muDoubleScalarSin ( _rtP -> P_2752 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2753
) * _rtP -> P_2750 + _rtP -> P_2751 ) * _rtB -> B_392_3784_0 [ 0 ] ; _rtB ->
B_392_3826_0 = ( muDoubleScalarSin ( _rtP -> P_2756 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2757 ) * _rtP -> P_2754 + _rtP -> P_2755 ) * _rtB -> B_392_3784_0
[ 0 ] ; _rtB -> B_392_3828_0 = ( muDoubleScalarSin ( _rtP -> P_2760 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2761 ) * _rtP -> P_2758 + _rtP -> P_2759
) * _rtB -> B_392_3784_0 [ 1 ] ; _rtB -> B_392_3830_0 = ( muDoubleScalarSin (
_rtP -> P_2764 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2765 ) * _rtP -> P_2762
+ _rtP -> P_2763 ) * _rtB -> B_392_3784_0 [ 1 ] ; _rtB -> B_392_3832_0 = (
muDoubleScalarSin ( _rtP -> P_2768 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2769
) * _rtP -> P_2766 + _rtP -> P_2767 ) * _rtB -> B_392_3784_0 [ 2 ] ; _rtB ->
B_392_3834_0 = ( muDoubleScalarSin ( _rtP -> P_2772 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2773 ) * _rtP -> P_2770 + _rtP -> P_2771 ) * _rtB -> B_392_3784_0
[ 2 ] ; B_392_3835_0 = _rtP -> P_2774 * rtb_B_392_239_0 ; _rtB ->
B_392_3837_0 = ( muDoubleScalarSin ( _rtP -> P_2777 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2778 ) * _rtP -> P_2775 + _rtP -> P_2776 ) * _rtB -> B_392_3788_0
[ 0 ] ; _rtB -> B_392_3839_0 = ( muDoubleScalarSin ( _rtP -> P_2781 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2782 ) * _rtP -> P_2779 + _rtP -> P_2780
) * _rtB -> B_392_3788_0 [ 0 ] ; _rtB -> B_392_3841_0 = ( muDoubleScalarSin (
_rtP -> P_2785 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2786 ) * _rtP -> P_2783
+ _rtP -> P_2784 ) * _rtB -> B_392_3788_0 [ 1 ] ; _rtB -> B_392_3843_0 = (
muDoubleScalarSin ( _rtP -> P_2789 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2790
) * _rtP -> P_2787 + _rtP -> P_2788 ) * _rtB -> B_392_3788_0 [ 1 ] ; _rtB ->
B_392_3845_0 = ( muDoubleScalarSin ( _rtP -> P_2793 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2794 ) * _rtP -> P_2791 + _rtP -> P_2792 ) * _rtB -> B_392_3788_0
[ 2 ] ; _rtB -> B_392_3847_0 = ( muDoubleScalarSin ( _rtP -> P_2797 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2798 ) * _rtP -> P_2795 + _rtP -> P_2796
) * _rtB -> B_392_3788_0 [ 2 ] ; _rtB -> B_392_3849_0 = ( muDoubleScalarSin (
_rtP -> P_2801 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2802 ) * _rtP -> P_2799
+ _rtP -> P_2800 ) * _rtB -> B_392_3784_0 [ 0 ] ; _rtB -> B_392_3851_0 = (
muDoubleScalarSin ( _rtP -> P_2805 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2806
) * _rtP -> P_2803 + _rtP -> P_2804 ) * _rtB -> B_392_3784_0 [ 0 ] ; _rtB ->
B_392_3853_0 = ( muDoubleScalarSin ( _rtP -> P_2809 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2810 ) * _rtP -> P_2807 + _rtP -> P_2808 ) * _rtB -> B_392_3784_0
[ 1 ] ; _rtB -> B_392_3855_0 = ( muDoubleScalarSin ( _rtP -> P_2813 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2814 ) * _rtP -> P_2811 + _rtP -> P_2812
) * _rtB -> B_392_3784_0 [ 1 ] ; _rtB -> B_392_3857_0 = ( muDoubleScalarSin (
_rtP -> P_2817 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2818 ) * _rtP -> P_2815
+ _rtP -> P_2816 ) * _rtB -> B_392_3784_0 [ 2 ] ; _rtB -> B_392_3859_0 = (
muDoubleScalarSin ( _rtP -> P_2821 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2822
) * _rtP -> P_2819 + _rtP -> P_2820 ) * _rtB -> B_392_3784_0 [ 2 ] ; _rtB ->
B_392_3861_0 = ( muDoubleScalarSin ( _rtP -> P_2825 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2826 ) * _rtP -> P_2823 + _rtP -> P_2824 ) * _rtB -> B_392_3788_0
[ 0 ] ; _rtB -> B_392_3863_0 = ( muDoubleScalarSin ( _rtP -> P_2829 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2830 ) * _rtP -> P_2827 + _rtP -> P_2828
) * _rtB -> B_392_3788_0 [ 0 ] ; _rtB -> B_392_3865_0 = ( muDoubleScalarSin (
_rtP -> P_2833 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2834 ) * _rtP -> P_2831
+ _rtP -> P_2832 ) * _rtB -> B_392_3788_0 [ 1 ] ; _rtB -> B_392_3867_0 = (
muDoubleScalarSin ( _rtP -> P_2837 * ssGetTaskTime ( S , 0 ) + _rtP -> P_2838
) * _rtP -> P_2835 + _rtP -> P_2836 ) * _rtB -> B_392_3788_0 [ 1 ] ; _rtB ->
B_392_3869_0 = ( muDoubleScalarSin ( _rtP -> P_2841 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_2842 ) * _rtP -> P_2839 + _rtP -> P_2840 ) * _rtB -> B_392_3788_0
[ 2 ] ; _rtB -> B_392_3871_0 = ( muDoubleScalarSin ( _rtP -> P_2845 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_2846 ) * _rtP -> P_2843 + _rtP -> P_2844
) * _rtB -> B_392_3788_0 [ 2 ] ; B_392_3872_0 = _rtP -> P_2847 *
rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
if ( _rtB -> B_392_3875_0 >= _rtP -> P_2853 ) { rtb_B_392_3878_0 = _rtB ->
B_392_3874_0 ; } else { rtb_B_392_3878_0 = rt_Lookup ( _rtP -> P_2851 , 6 ,
ssGetTaskTime ( S , 2 ) , _rtP -> P_2852 ) ; } if ( _rtB -> B_392_3879_0 >=
_rtP -> P_2856 ) { _rtB -> B_392_3882_0 = rtb_B_392_3878_0 ; } else { _rtB ->
B_392_3882_0 = _rtB -> B_392_3880_0 ; } } _rtB -> B_392_3884_0 = rt_Lookup (
_rtP -> P_2857 , 5 , ssGetT ( S ) , _rtP -> P_2858 ) ; if ( _rtB ->
B_392_3873_0 >= _rtP -> P_2859 ) { _rtB -> B_392_3885_0 = _rtB ->
B_392_3882_0 ; } else { _rtB -> B_392_3885_0 = _rtB -> B_392_3884_0 ; } if (
( _rtDW -> TimeStampA_mu >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB_c >=
ssGetT ( S ) ) ) { _rtB -> B_392_3886_0 = 0.0 ; } else { rtb_B_392_430_0 =
_rtDW -> TimeStampA_mu ; lastU = & _rtDW -> LastUAtTimeA_g4 ; if ( _rtDW ->
TimeStampA_mu < _rtDW -> TimeStampB_c ) { if ( _rtDW -> TimeStampB_c < ssGetT
( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_c ; lastU = & _rtDW ->
LastUAtTimeB_e5 ; } } else { if ( _rtDW -> TimeStampA_mu >= ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_c ; lastU = & _rtDW -> LastUAtTimeB_e5
; } } _rtB -> B_392_3886_0 = ( _rtB -> B_392_3884_0 - * lastU ) / ( ssGetT (
S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit
!= 0 ) { if ( _rtB -> B_392_3892_0 >= _rtP -> P_2863 ) { _rtB -> B_392_3894_0
= rtb_B_392_3878_0 ; } else { _rtB -> B_392_3894_0 = _rtB -> B_392_3880_0 ; }
} _rtB -> B_392_3896_0 = rt_Lookup ( _rtP -> P_2864 , 5 , ssGetT ( S ) , _rtP
-> P_2865 ) ; if ( _rtB -> B_392_3891_0 >= _rtP -> P_2866 ) { _rtB ->
B_392_3897_0 = _rtB -> B_392_3894_0 ; } else { _rtB -> B_392_3897_0 = _rtB ->
B_392_3896_0 ; } if ( ( _rtDW -> TimeStampA_k >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_p >= ssGetT ( S ) ) ) { _rtB -> B_392_3898_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_k ; lastU = & _rtDW -> LastUAtTimeA_pj
; if ( _rtDW -> TimeStampA_k < _rtDW -> TimeStampB_p ) { if ( _rtDW ->
TimeStampB_p < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_p ;
lastU = & _rtDW -> LastUAtTimeB_o ; } } else { if ( _rtDW -> TimeStampA_k >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_p ; lastU = & _rtDW ->
LastUAtTimeB_o ; } } _rtB -> B_392_3898_0 = ( _rtB -> B_392_3896_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_392_3904_0 >= _rtP -> P_2870 ) { _rtB
-> B_392_3906_0 = rtb_B_392_3878_0 ; } else { _rtB -> B_392_3906_0 = _rtB ->
B_392_3880_0 ; } } _rtB -> B_392_3908_0 = rt_Lookup ( _rtP -> P_2871 , 5 ,
ssGetT ( S ) , _rtP -> P_2872 ) ; if ( _rtB -> B_392_3903_0 >= _rtP -> P_2873
) { _rtB -> B_392_3909_0 = _rtB -> B_392_3906_0 ; } else { _rtB ->
B_392_3909_0 = _rtB -> B_392_3908_0 ; } if ( ( _rtDW -> TimeStampA_p >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ne >= ssGetT ( S ) ) ) { _rtB ->
B_392_3910_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_p ; lastU
= & _rtDW -> LastUAtTimeA_o ; if ( _rtDW -> TimeStampA_p < _rtDW ->
TimeStampB_ne ) { if ( _rtDW -> TimeStampB_ne < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_ne ; lastU = & _rtDW -> LastUAtTimeB_ou
; } } else { if ( _rtDW -> TimeStampA_p >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_ne ; lastU = & _rtDW -> LastUAtTimeB_ou ; } } _rtB ->
B_392_3910_0 = ( _rtB -> B_392_3908_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } _rtB -> B_392_3927_0 = 0.0 ; _rtB -> B_392_3927_0 +=
_rtP -> P_2876 * _rtX -> LPF1_CSTATE_j ; _rtB -> B_392_3928_0 = 0.0 ; _rtB ->
B_392_3928_0 += _rtP -> P_2878 * _rtX -> LPF2_CSTATE_m5 ; _rtB ->
B_392_3929_0 = _rtX -> integrator_CSTATE_fn ; { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_j4 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j4 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP ->
P_2880 ; B_392_3930_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate (
tMinusDelay , 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ogq
. CircularBufSize , & _rtDW -> TransportDelay_IWORK_ogq . Last , _rtDW ->
TransportDelay_IWORK_ogq . Tail , _rtDW -> TransportDelay_IWORK_ogq . Head ,
_rtP -> P_2881 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3932_0 = _rtP -> P_2882 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3932_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3934_0 =
_rtDW -> Memory_PreviousInput_csp ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3936_0 = ( _rtB -> B_392_3929_0 - B_392_3930_0 ) * _rtP -> P_341 ; }
else { _rtB -> B_392_3936_0 = _rtB -> B_392_3934_0 ; } _rtB -> B_392_3937_0 =
_rtX -> integrator_CSTATE_ox ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_hl . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hl . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2885 ;
B_392_3938_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_nn .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_nn . Last , _rtDW ->
TransportDelay_IWORK_nn . Tail , _rtDW -> TransportDelay_IWORK_nn . Head ,
_rtP -> P_2886 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3940_0 = _rtP -> P_2887 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3940_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3942_0 =
_rtDW -> Memory_PreviousInput_i1 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3944_0 = ( _rtB -> B_392_3937_0 - B_392_3938_0 ) * _rtP -> P_340 ; }
else { _rtB -> B_392_3944_0 = _rtB -> B_392_3942_0 ; } rtb_B_392_3878_0 =
_rtP -> P_2889 * muDoubleScalarAtan2 ( _rtB -> B_392_3944_0 , _rtB ->
B_392_3936_0 ) * _rtP -> P_2890 ; B_392_3949_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3936_0 , _rtB -> B_392_3944_0 ) * muDoubleScalarCos (
rtb_B_392_3878_0 ) ; B_392_3949_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3936_0 , _rtB -> B_392_3944_0 ) * muDoubleScalarSin ( rtb_B_392_3878_0
) ; _rtB -> B_392_3950_0 = _rtX -> integrator_CSTATE_clt ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2892 ; B_392_3951_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pik . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_pik . Last , _rtDW -> TransportDelay_IWORK_pik
. Tail , _rtDW -> TransportDelay_IWORK_pik . Head , _rtP -> P_2893 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3953_0 = _rtP -> P_2894 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3953_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3955_0 = _rtDW -> Memory_PreviousInput_a0q ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_3957_0 = ( _rtB -> B_392_3950_0 -
B_392_3951_0 ) * _rtP -> P_343 ; } else { _rtB -> B_392_3957_0 = _rtB ->
B_392_3955_0 ; } _rtB -> B_392_3958_0 = _rtX -> integrator_CSTATE_bo ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ndm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2897 ; B_392_3959_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_k1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_k1 . Last , _rtDW -> TransportDelay_IWORK_k1 .
Tail , _rtDW -> TransportDelay_IWORK_k1 . Head , _rtP -> P_2898 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_3961_0 = _rtP -> P_2899 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_3961_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_3963_0 = _rtDW -> Memory_PreviousInput_dz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_3965_0 = ( _rtB -> B_392_3958_0 -
B_392_3959_0 ) * _rtP -> P_342 ; } else { _rtB -> B_392_3965_0 = _rtB ->
B_392_3963_0 ; } rtb_B_392_3878_0 = _rtP -> P_2901 * muDoubleScalarAtan2 (
_rtB -> B_392_3965_0 , _rtB -> B_392_3957_0 ) * _rtP -> P_2902 ; B_392_3970_0
. re = muDoubleScalarHypot ( _rtB -> B_392_3957_0 , _rtB -> B_392_3965_0 ) *
muDoubleScalarCos ( rtb_B_392_3878_0 ) ; B_392_3970_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_3957_0 , _rtB -> B_392_3965_0 ) *
muDoubleScalarSin ( rtb_B_392_3878_0 ) ; _rtB -> B_392_3971_0 = _rtX ->
integrator_CSTATE_jm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_f0i . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_f0i . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2904 ;
B_392_3972_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_mn .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_mn . Last , _rtDW ->
TransportDelay_IWORK_mn . Tail , _rtDW -> TransportDelay_IWORK_mn . Head ,
_rtP -> P_2905 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3974_0 = _rtP -> P_2906 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3974_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3976_0 =
_rtDW -> Memory_PreviousInput_kn ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3978_0 = ( _rtB -> B_392_3971_0 - B_392_3972_0 ) * _rtP -> P_345 ; }
else { _rtB -> B_392_3978_0 = _rtB -> B_392_3976_0 ; } _rtB -> B_392_3979_0 =
_rtX -> integrator_CSTATE_gv ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ge . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ge . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2909 ;
B_392_3980_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pl .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_pl . Last , _rtDW ->
TransportDelay_IWORK_pl . Tail , _rtDW -> TransportDelay_IWORK_pl . Head ,
_rtP -> P_2910 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3982_0 = _rtP -> P_2911 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_3982_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3984_0 =
_rtDW -> Memory_PreviousInput_iq ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_3986_0 = ( _rtB -> B_392_3979_0 - B_392_3980_0 ) * _rtP -> P_344 ; }
else { _rtB -> B_392_3986_0 = _rtB -> B_392_3984_0 ; } rtb_B_392_3878_0 =
_rtP -> P_2913 * muDoubleScalarAtan2 ( _rtB -> B_392_3986_0 , _rtB ->
B_392_3978_0 ) * _rtP -> P_2914 ; B_392_3991_0 . re = muDoubleScalarHypot (
_rtB -> B_392_3978_0 , _rtB -> B_392_3986_0 ) * muDoubleScalarCos (
rtb_B_392_3878_0 ) ; B_392_3991_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_3978_0 , _rtB -> B_392_3986_0 ) * muDoubleScalarSin ( rtb_B_392_3878_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_3993_0 = _rtB -> B_392_3992_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_3993_0 ,
B_392_3949_0 , B_392_3970_0 , B_392_3991_0 , & _rtB -> PosSeqComputation_n1 ,
& _rtDW -> PosSeqComputation_n1 , & _rtP -> PosSeqComputation_n1 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_3996_0 = _rtB
-> B_392_3995_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_3996_0 , B_392_3949_0 , B_392_3970_0 , B_392_3991_0 , & _rtB ->
NegSeqComputation_fo , & _rtDW -> NegSeqComputation_fo , & _rtP ->
NegSeqComputation_fo ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_3999_0 = _rtB -> B_392_3998_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_3999_0 ,
B_392_3949_0 , B_392_3970_0 , B_392_3991_0 , & _rtB -> ZeroSeqComputation_jr
, & _rtDW -> ZeroSeqComputation_jr , & _rtP -> ZeroSeqComputation_jr ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_n1 .
B_23_2_0 . im , _rtB -> PosSeqComputation_n1 . B_23_2_0 . re ) ; _rtB ->
B_392_4002_0 = _rtP -> P_2918 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_n1 . B_23_2_0 . re , _rtB -> PosSeqComputation_n1 .
B_23_2_0 . im ) ; _rtB -> B_392_4003_0 = _rtX -> integrator_CSTATE_cr ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_e1q . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2920 ; B_392_4004_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jj . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_jj . Last , _rtDW -> TransportDelay_IWORK_jj .
Tail , _rtDW -> TransportDelay_IWORK_jj . Head , _rtP -> P_2921 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4006_0 = _rtP -> P_2922 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4006_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4008_0 = _rtDW -> Memory_PreviousInput_kz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4010_0 = ( _rtB -> B_392_4003_0 -
B_392_4004_0 ) * _rtP -> P_347 ; } else { _rtB -> B_392_4010_0 = _rtB ->
B_392_4008_0 ; } _rtB -> B_392_4011_0 = _rtX -> integrator_CSTATE_gm ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_b2 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2925 ; B_392_4012_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dr . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_dr . Last , _rtDW -> TransportDelay_IWORK_dr .
Tail , _rtDW -> TransportDelay_IWORK_dr . Head , _rtP -> P_2926 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4014_0 = _rtP -> P_2927 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4014_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4016_0 = _rtDW -> Memory_PreviousInput_lp ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4018_0 = ( _rtB -> B_392_4011_0 -
B_392_4012_0 ) * _rtP -> P_346 ; } else { _rtB -> B_392_4018_0 = _rtB ->
B_392_4016_0 ; } rtb_B_392_3878_0 = _rtP -> P_2929 * muDoubleScalarAtan2 (
_rtB -> B_392_4018_0 , _rtB -> B_392_4010_0 ) * _rtP -> P_2930 ; B_392_4023_0
. re = muDoubleScalarHypot ( _rtB -> B_392_4010_0 , _rtB -> B_392_4018_0 ) *
muDoubleScalarCos ( rtb_B_392_3878_0 ) ; B_392_4023_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_4010_0 , _rtB -> B_392_4018_0 ) *
muDoubleScalarSin ( rtb_B_392_3878_0 ) ; _rtB -> B_392_4024_0 = _rtX ->
integrator_CSTATE_l3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ma . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ma . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2932 ;
B_392_4025_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_a3y .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_a3y . Last , _rtDW ->
TransportDelay_IWORK_a3y . Tail , _rtDW -> TransportDelay_IWORK_a3y . Head ,
_rtP -> P_2933 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4027_0 = _rtP -> P_2934 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4027_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4029_0 =
_rtDW -> Memory_PreviousInput_ae ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4031_0 = ( _rtB -> B_392_4024_0 - B_392_4025_0 ) * _rtP -> P_349 ; }
else { _rtB -> B_392_4031_0 = _rtB -> B_392_4029_0 ; } _rtB -> B_392_4032_0 =
_rtX -> integrator_CSTATE_hm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_b5s . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_2937
; B_392_4033_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_dm . Last , _rtDW ->
TransportDelay_IWORK_dm . Tail , _rtDW -> TransportDelay_IWORK_dm . Head ,
_rtP -> P_2938 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4035_0 = _rtP -> P_2939 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4035_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4037_0 =
_rtDW -> Memory_PreviousInput_nrz ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4039_0 = ( _rtB -> B_392_4032_0 - B_392_4033_0 ) * _rtP -> P_348 ; }
else { _rtB -> B_392_4039_0 = _rtB -> B_392_4037_0 ; } rtb_B_392_3878_0 =
_rtP -> P_2941 * muDoubleScalarAtan2 ( _rtB -> B_392_4039_0 , _rtB ->
B_392_4031_0 ) * _rtP -> P_2942 ; B_392_4044_0 . re = muDoubleScalarHypot (
_rtB -> B_392_4031_0 , _rtB -> B_392_4039_0 ) * muDoubleScalarCos (
rtb_B_392_3878_0 ) ; B_392_4044_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_4031_0 , _rtB -> B_392_4039_0 ) * muDoubleScalarSin ( rtb_B_392_3878_0
) ; _rtB -> B_392_4045_0 = _rtX -> integrator_CSTATE_co ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_2944 ; B_392_4046_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_pdg . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_pdg . Last , _rtDW -> TransportDelay_IWORK_pdg
. Tail , _rtDW -> TransportDelay_IWORK_pdg . Head , _rtP -> P_2945 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4048_0 = _rtP -> P_2946 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4048_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4050_0 = _rtDW -> Memory_PreviousInput_jd4 ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_4052_0 = ( _rtB -> B_392_4045_0 -
B_392_4046_0 ) * _rtP -> P_351 ; } else { _rtB -> B_392_4052_0 = _rtB ->
B_392_4050_0 ; } _rtB -> B_392_4053_0 = _rtX -> integrator_CSTATE_knp ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_g1 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_2949 ; B_392_4054_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ny . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ny . Last , _rtDW -> TransportDelay_IWORK_ny .
Tail , _rtDW -> TransportDelay_IWORK_ny . Head , _rtP -> P_2950 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4056_0 = _rtP -> P_2951 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4056_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4058_0 = _rtDW -> Memory_PreviousInput_o2n ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_4060_0 = ( _rtB -> B_392_4053_0 -
B_392_4054_0 ) * _rtP -> P_350 ; } else { _rtB -> B_392_4060_0 = _rtB ->
B_392_4058_0 ; } rtb_B_392_3878_0 = _rtP -> P_2953 * muDoubleScalarAtan2 (
_rtB -> B_392_4060_0 , _rtB -> B_392_4052_0 ) * _rtP -> P_2954 ; B_392_4065_0
. re = muDoubleScalarHypot ( _rtB -> B_392_4052_0 , _rtB -> B_392_4060_0 ) *
muDoubleScalarCos ( rtb_B_392_3878_0 ) ; B_392_4065_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_4052_0 , _rtB -> B_392_4060_0 ) *
muDoubleScalarSin ( rtb_B_392_3878_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_4067_0 = _rtB -> B_392_4066_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_4067_0 ,
B_392_4023_0 , B_392_4044_0 , B_392_4065_0 , & _rtB -> PosSeqComputation_b ,
& _rtDW -> PosSeqComputation_b , & _rtP -> PosSeqComputation_b ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4070_0 = _rtB
-> B_392_4069_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_4070_0 , B_392_4023_0 , B_392_4044_0 , B_392_4065_0 , & _rtB ->
NegSeqComputation_f5 , & _rtDW -> NegSeqComputation_f5 , & _rtP ->
NegSeqComputation_f5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_4073_0 = _rtB -> B_392_4072_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_4073_0 ,
B_392_4023_0 , B_392_4044_0 , B_392_4065_0 , & _rtB -> ZeroSeqComputation_dy
, & _rtDW -> ZeroSeqComputation_dy , & _rtP -> ZeroSeqComputation_dy ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_b .
B_23_2_0 . im , _rtB -> PosSeqComputation_b . B_23_2_0 . re ) ; _rtB ->
B_392_4076_0 = _rtP -> P_2958 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_b . B_23_2_0 . re , _rtB -> PosSeqComputation_b . B_23_2_0
. im ) ; ssCallAccelRunBlock ( S , 392 , 4078 , SS_CALL_MDL_OUTPUTS ) ; isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4079_0 =
_rtP -> P_2960 * _rtB -> B_0_14_0 [ 71 ] ; } _rtB -> B_392_4080_0 [ 0 ] =
_rtB -> B_392_3927_0 ; _rtB -> B_392_4080_0 [ 1 ] = _rtB -> B_392_3928_0 ;
_rtB -> B_392_4080_0 [ 2 ] = _rtB -> B_392_4002_0 ; _rtB -> B_392_4080_0 [ 3
] = _rtB -> B_392_4076_0 ; _rtB -> B_392_4080_0 [ 4 ] = _rtB -> B_392_4079_0
; ssCallAccelRunBlock ( S , 392 , 4081 , SS_CALL_MDL_OUTPUTS ) ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP -> P_2962 *
_rtB -> B_0_14_0 [ 131 ] * _rtP -> P_2965 ; tmp_1 = _rtP -> P_2963 * _rtB ->
B_0_14_0 [ 132 ] * _rtP -> P_2965 ; rtb_B_392_16_0 = _rtP -> P_2964 * _rtB ->
B_0_14_0 [ 133 ] * _rtP -> P_2965 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_2966 [ isHit + 6 ] * rtb_B_392_16_0 + ( _rtP ->
P_2966 [ isHit + 3 ] * tmp_1 + _rtP -> P_2966 [ isHit ] * tmp_0 ) ; }
B_392_4088_0 [ 0 ] = _rtP -> P_2967 * tmp [ 0 ] ; B_392_4088_0 [ 1 ] = _rtP
-> P_2967 * tmp [ 1 ] ; B_392_4088_0 [ 2 ] = _rtP -> P_2967 * tmp [ 2 ] ;
_rtB -> B_392_4092_0 = _rtB -> B_392_4091_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4092_0 , & B_392_4088_0 [
0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystem1_k0 , & _rtDW -> Subsystem1_k0
) ; _rtB -> B_392_4096_0 = _rtB -> B_392_4095_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4096_0 , &
B_392_4088_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_n , &
_rtDW -> Subsystempi2delay_n ) ; if ( _rtB -> B_392_4091_0 != 0 ) { _rtB ->
B_392_4098_0 [ 0 ] = _rtB -> Subsystem1_k0 . B_11_0_0 ; _rtB -> B_392_4098_0
[ 1 ] = _rtB -> Subsystem1_k0 . B_11_1_0 ; } else { _rtB -> B_392_4098_0 [ 0
] = _rtB -> Subsystempi2delay_n . B_10_0_0 ; _rtB -> B_392_4098_0 [ 1 ] =
_rtB -> Subsystempi2delay_n . B_10_1_0 ; } } _rtB -> B_392_4099_0 = 0.0 ;
_rtB -> B_392_4099_0 += _rtP -> P_2972 * _rtX -> Integradord_CSTATE_g ; isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4102_0 = (
_rtB -> B_392_4100_0 - _rtB -> B_392_4079_0 ) * _rtP -> P_2974 ; _rtB ->
B_392_4103_0 = _rtP -> P_2975 * _rtB -> B_392_4102_0 ; } _rtB -> B_392_4104_0
= _rtB -> B_392_4099_0 + _rtB -> B_392_4103_0 ; _rtB -> B_392_4106_0 = _rtB
-> B_392_4105_0 - _rtB -> B_392_3928_0 ; _rtB -> B_392_4107_0 = _rtP ->
P_2977 * _rtB -> B_392_4106_0 ; _rtB -> B_392_4108_0 = _rtP -> P_2978 * _rtB
-> B_392_4107_0 ; _rtB -> B_392_4109_0 = 0.0 ; _rtB -> B_392_4109_0 += _rtP
-> P_2980 * _rtX -> Integradord1_CSTATE_m ; _rtB -> B_392_4110_0 = _rtB ->
B_392_4108_0 + _rtB -> B_392_4109_0 ; _rtB -> B_392_4111_0 . re = _rtB ->
B_392_4104_0 ; _rtB -> B_392_4111_0 . im = _rtB -> B_392_4110_0 ; _rtB ->
B_392_4112_0 = muDoubleScalarHypot ( _rtB -> B_392_4111_0 . re , _rtB ->
B_392_4111_0 . im ) ; rtb_B_392_3878_0 = muDoubleScalarAtan2 ( _rtB ->
B_392_4111_0 . im , _rtB -> B_392_4111_0 . re ) ; if ( ssIsMajorTimeStep ( S
) != 0 ) { _rtDW -> Saturation_MODE_p0 = _rtB -> B_392_4112_0 >= _rtP ->
P_2981 ? 1 : _rtB -> B_392_4112_0 > _rtP -> P_2982 ? 0 : - 1 ; } _rtB ->
B_392_4113_0 = _rtDW -> Saturation_MODE_p0 == 1 ? _rtP -> P_2981 : _rtDW ->
Saturation_MODE_p0 == - 1 ? _rtP -> P_2982 : _rtB -> B_392_4112_0 ; _rtB ->
B_392_4115_0 = _rtB -> B_392_4113_0 * muDoubleScalarCos ( rtb_B_392_3878_0 )
; _rtB -> B_392_4115_1 = _rtB -> B_392_4113_0 * muDoubleScalarSin (
rtb_B_392_3878_0 ) ; _rtB -> B_392_4116_0 = _rtB -> B_392_4115_0 - _rtB ->
B_392_4098_0 [ 0 ] ; _rtB -> B_392_4117_0 = 0.0 ; _rtB -> B_392_4117_0 +=
_rtP -> P_2984 * _rtX -> Controled_CSTATE ; _rtB -> B_392_4117_0 += _rtP ->
P_2985 * _rtB -> B_392_4116_0 ; _rtB -> B_392_4118_0 = _rtB -> B_392_4115_1 -
_rtB -> B_392_4098_0 [ 1 ] ; _rtB -> B_392_4119_0 = 0.0 ; _rtB ->
B_392_4119_0 += _rtP -> P_2987 * _rtX -> Controleq_CSTATE ; _rtB ->
B_392_4119_0 += _rtP -> P_2988 * _rtB -> B_392_4118_0 ; isHit = ssIsSampleHit
( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4123_0 [ 0 ] = _rtP ->
P_2989 * _rtB -> B_0_14_0 [ 79 ] * _rtP -> P_2992 ; _rtB -> B_392_4123_0 [ 1
] = _rtP -> P_2990 * _rtB -> B_0_14_0 [ 80 ] * _rtP -> P_2992 ; _rtB ->
B_392_4123_0 [ 2 ] = _rtP -> P_2991 * _rtB -> B_0_14_0 [ 81 ] * _rtP ->
P_2992 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] = _rtP ->
P_2993 [ isHit + 6 ] * _rtB -> B_392_4123_0 [ 2 ] + ( _rtP -> P_2993 [ isHit
+ 3 ] * _rtB -> B_392_4123_0 [ 1 ] + _rtP -> P_2993 [ isHit ] * _rtB ->
B_392_4123_0 [ 0 ] ) ; } B_392_4125_0 [ 0 ] = _rtP -> P_2994 * tmp [ 0 ] ;
B_392_4125_0 [ 1 ] = _rtP -> P_2994 * tmp [ 1 ] ; B_392_4125_0 [ 2 ] = _rtP
-> P_2994 * tmp [ 2 ] ; _rtB -> B_392_4129_0 = _rtB -> B_392_4128_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4129_0 , & B_392_4125_0 [
0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystem1_m2 , & _rtDW -> Subsystem1_m2
) ; _rtB -> B_392_4133_0 = _rtB -> B_392_4132_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4133_0 , &
B_392_4125_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_cj , &
_rtDW -> Subsystempi2delay_cj ) ; if ( _rtB -> B_392_4128_0 != 0 ) { _rtB ->
B_392_4135_0 [ 0 ] = _rtB -> Subsystem1_m2 . B_11_0_0 ; _rtB -> B_392_4135_0
[ 1 ] = _rtB -> Subsystem1_m2 . B_11_1_0 ; } else { _rtB -> B_392_4135_0 [ 0
] = _rtB -> Subsystempi2delay_cj . B_10_0_0 ; _rtB -> B_392_4135_0 [ 1 ] =
_rtB -> Subsystempi2delay_cj . B_10_1_0 ; } } isHit = ssIsSampleHit ( S , 3 ,
0 ) ; if ( isHit != 0 ) { rtb_B_392_4136_0 = _rtDW -> UnitDelay_DSTATE_c ;
_rtB -> B_392_4137_0 = _rtP -> P_2999 * _rtDW -> UnitDelay_DSTATE_c ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4139_0 =
_rtB -> B_392_4137_0 * _rtB -> B_392_4098_0 [ 1 ] * _rtP -> P_3000 ; } isHit
= ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4141_0 =
_rtB -> B_392_4079_0 * _rtB -> B_392_4082_0 ; } _rtB -> B_392_4142_0 = ( (
_rtB -> B_392_4135_0 [ 0 ] + _rtB -> B_392_4117_0 ) - _rtB -> B_392_4139_0 )
/ _rtB -> B_392_4141_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_4144_0 = _rtB -> B_392_4098_0 [ 0 ] * _rtB ->
B_392_4137_0 * _rtP -> P_3001 ; } _rtB -> B_392_4146_0 = ( ( _rtB ->
B_392_4144_0 + _rtB -> B_392_4119_0 ) + _rtB -> B_392_4135_0 [ 1 ] ) * ( 1.0
/ _rtB -> B_392_4141_0 ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_k5 = _rtB -> B_392_4146_0 >= _rtP -> P_3002 ? 1 : _rtB ->
B_392_4146_0 > _rtP -> P_3003 ? 0 : - 1 ; _rtDW -> Saturation1_MODE = _rtB ->
B_392_4142_0 >= _rtP -> P_3004 ? 1 : _rtB -> B_392_4142_0 > _rtP -> P_3005 ?
0 : - 1 ; } _rtB -> B_392_4147_0 = _rtDW -> Saturation_MODE_k5 == 1 ? _rtP ->
P_3002 : _rtDW -> Saturation_MODE_k5 == - 1 ? _rtP -> P_3003 : _rtB ->
B_392_4146_0 ; _rtB -> B_392_4148_0 = _rtDW -> Saturation1_MODE == 1 ? _rtP
-> P_3004 : _rtDW -> Saturation1_MODE == - 1 ? _rtP -> P_3005 : _rtB ->
B_392_4142_0 ; _rtB -> B_392_4149_0 = _rtB -> B_392_4104_0 - _rtB ->
B_392_4115_0 ; _rtB -> B_392_4150_0 = _rtP -> P_3006 * _rtB -> B_392_4149_0 ;
_rtB -> B_392_4151_0 = _rtB -> B_392_4102_0 - _rtB -> B_392_4150_0 ; _rtB ->
B_392_4152_0 = _rtP -> P_3007 * _rtB -> B_392_4151_0 ; _rtB -> B_392_4153_0 =
_rtB -> B_392_4110_0 - _rtB -> B_392_4115_1 ; _rtB -> B_392_4154_0 = _rtP ->
P_3008 * _rtB -> B_392_4153_0 ; _rtB -> B_392_4155_0 = _rtB -> B_392_4107_0 -
_rtB -> B_392_4154_0 ; _rtB -> B_392_4156_0 = _rtP -> P_3009 * _rtB ->
B_392_4155_0 ; ssCallAccelRunBlock ( S , 392 , 4157 , SS_CALL_MDL_OUTPUTS ) ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_392_4162_0 = _rtP
-> P_3010 * _rtB -> B_0_14_0 [ 126 ] * _rtB -> B_392_4079_0 ; B_392_4174_0 [
0 ] = _rtP -> P_3011 * _rtB -> B_0_14_0 [ 76 ] * _rtP -> P_3014 ;
B_392_4174_0 [ 1 ] = _rtP -> P_3012 * _rtB -> B_0_14_0 [ 77 ] * _rtP ->
P_3014 ; B_392_4174_0 [ 2 ] = _rtP -> P_3013 * _rtB -> B_0_14_0 [ 78 ] * _rtP
-> P_3014 ; _rtB -> B_392_4217_0 = ( _rtB -> B_392_95_0 * _rtB -> B_0_14_0 [
152 ] - _rtB -> B_392_96_0 * _rtB -> B_0_14_0 [ 153 ] ) / ( ( _rtP -> P_3016
* _rtB -> B_392_92_0 - _rtDW -> prediction_DSTATE_la ) + _rtB -> B_392_4209_0
) ; _rtB -> B_392_4235_0 = ( _rtB -> B_392_4135_0 [ 0 ] * _rtB ->
B_392_4098_0 [ 0 ] + _rtB -> B_392_4135_0 [ 1 ] * _rtB -> B_392_4098_0 [ 1 ]
) * _rtP -> P_3018 ; _rtB -> B_392_4239_0 = ( _rtB -> B_392_4098_0 [ 0 ] *
_rtB -> B_392_4135_0 [ 1 ] - _rtB -> B_392_4135_0 [ 0 ] * _rtB ->
B_392_4098_0 [ 1 ] ) * _rtP -> P_3019 ; _rtB -> B_392_4245_0 = _rtB ->
B_392_4244_0 ; _rtB -> B_392_4246_0 = _rtB -> B_392_4242_0 ; _rtB ->
B_392_4250_0 [ 0 ] = _rtP -> P_3023 * _rtB -> B_0_14_0 [ 134 ] * _rtP ->
P_3026 ; _rtB -> B_392_4250_0 [ 1 ] = _rtP -> P_3024 * _rtB -> B_0_14_0 [ 135
] * _rtP -> P_3026 ; _rtB -> B_392_4250_0 [ 2 ] = _rtP -> P_3025 * _rtB ->
B_0_14_0 [ 136 ] * _rtP -> P_3026 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_3027 [ isHit + 6 ] * _rtB -> B_392_4250_0 [ 2 ] +
( _rtP -> P_3027 [ isHit + 3 ] * _rtB -> B_392_4250_0 [ 1 ] + _rtP -> P_3027
[ isHit ] * _rtB -> B_392_4250_0 [ 0 ] ) ; } B_392_4252_0 [ 0 ] = _rtP ->
P_3028 * tmp [ 0 ] ; B_392_4252_0 [ 1 ] = _rtP -> P_3028 * tmp [ 1 ] ;
B_392_4252_0 [ 2 ] = _rtP -> P_3028 * tmp [ 2 ] ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4245_0 , &
B_392_4252_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_j , &
_rtDW -> Subsystempi2delay_j ) ; MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB ->
B_392_4246_0 , & B_392_4252_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB ->
Subsystem1_i , & _rtDW -> Subsystem1_i ) ; if ( _rtB -> B_392_4242_0 != 0 ) {
B_392_4255_0 [ 0 ] = _rtB -> Subsystem1_i . B_11_0_0 ; B_392_4255_0 [ 1 ] =
_rtB -> Subsystem1_i . B_11_1_0 ; } else { B_392_4255_0 [ 0 ] = _rtB ->
Subsystempi2delay_j . B_10_0_0 ; B_392_4255_0 [ 1 ] = _rtB ->
Subsystempi2delay_j . B_10_1_0 ; } } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_392_4257_0 = _rtP -> P_3029 ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( _rtB -> B_392_4257_0 > 0.0 ) { if ( ! _rtDW ->
AutomaticGainControl_MODE_p ) { if ( ssGetTaskTime ( S , 3 ) != ssGetTStart (
S ) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
Integ4_SYSTEM_ENABLE_f = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_n = 1U ; _rtDW ->
AutomaticGainControl_MODE_p = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE_p ) { ssSetBlockStateForSolverChangedAtMajorStep (
S ) ; _rtDW -> AutomaticGainControl_MODE_p = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE_p ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_299 [ isHit + 6 ] * _rtB -> B_392_4123_0 [ 2 ] + ( _rtP -> P_299 [
isHit + 3 ] * _rtB -> B_392_4123_0 [ 1 ] + _rtP -> P_299 [ isHit ] * _rtB ->
B_392_4123_0 [ 0 ] ) ; } B_331_1_0 [ 0 ] = _rtP -> P_300 * tmp [ 0 ] ;
B_331_1_0 [ 1 ] = _rtP -> P_300 * tmp [ 1 ] ; B_331_1_0 [ 2 ] = _rtP -> P_300
* tmp [ 2 ] ; _rtB -> B_331_5_0 = _rtB -> B_331_4_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_331_5_0 , & B_331_1_0 [ 0 ] ,
_rtB -> B_392_79_0 , & _rtB -> Subsystem1_eh , & _rtDW -> Subsystem1_eh ) ;
_rtB -> B_331_9_0 = _rtB -> B_331_8_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_331_9_0 , & B_331_1_0 [
0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_a1 , & _rtDW ->
Subsystempi2delay_a1 ) ; if ( _rtB -> B_331_4_0 != 0 ) { _rtB -> B_331_11_0 [
0 ] = _rtB -> Subsystem1_eh . B_11_0_0 ; _rtB -> B_331_11_0 [ 1 ] = _rtB ->
Subsystem1_eh . B_11_1_0 ; } else { _rtB -> B_331_11_0 [ 0 ] = _rtB ->
Subsystempi2delay_a1 . B_10_0_0 ; _rtB -> B_331_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_a1 . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_f != 0
) { _rtB -> B_331_12_0 = _rtDW -> Integ4_DSTATE_kt ; } else { _rtB ->
B_331_12_0 = _rtP -> P_304 * _rtB -> B_331_11_0 [ 0 ] + _rtDW ->
Integ4_DSTATE_kt ; } if ( rtb_B_392_4136_0 > _rtP -> P_306 ) {
rtb_B_392_430_0 = _rtP -> P_306 ; } else if ( rtb_B_392_4136_0 < _rtP ->
P_307 ) { rtb_B_392_430_0 = _rtP -> P_307 ; } else { rtb_B_392_430_0 =
rtb_B_392_4136_0 ; } rtb_B_392_3878_0 = 1.0 / rtb_B_392_430_0 / 5.0e-5 ;
rtb_B_392_3426_0 = muDoubleScalarCeil ( rtb_B_392_3878_0 ) ; _rtB ->
B_331_16_0 = _rtP -> P_308 * rtb_B_392_3426_0 ; ssCallAccelRunBlock ( S , 331
, 17 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_331_20_0 ) { rtb_B_392_3426_0 = rtb_B_392_3878_0 - rtb_B_392_3426_0 ; _rtB
-> B_331_24_0 = ( ( _rtB -> B_331_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_da )
* rtb_B_392_3426_0 * _rtP -> P_296 + _rtB -> B_331_11_0 [ 0 ] ) * (
rtb_B_392_3426_0 / rtb_B_392_3878_0 ) + ( _rtB -> B_331_12_0 - _rtB ->
B_331_17_0 ) * rtb_B_392_4136_0 ; } else { _rtB -> B_331_24_0 = _rtDW ->
UnitDelay1_DSTATE_mz ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_n != 0 ) { _rtB
-> B_331_25_0 = _rtDW -> Integ4_DSTATE_h ; } else { _rtB -> B_331_25_0 = _rtP
-> P_320 * _rtB -> B_331_11_0 [ 1 ] + _rtDW -> Integ4_DSTATE_h ; } if (
rtb_B_392_4136_0 > _rtP -> P_322 ) { rtb_B_392_430_0 = _rtP -> P_322 ; } else
if ( rtb_B_392_4136_0 < _rtP -> P_323 ) { rtb_B_392_430_0 = _rtP -> P_323 ; }
else { rtb_B_392_430_0 = rtb_B_392_4136_0 ; } rtb_B_392_3878_0 = 1.0 /
rtb_B_392_430_0 / 5.0e-5 ; rtb_B_392_3426_0 = muDoubleScalarCeil (
rtb_B_392_3878_0 ) ; _rtB -> B_331_29_0 = _rtP -> P_324 * rtb_B_392_3426_0 ;
ssCallAccelRunBlock ( S , 331 , 30 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_331_33_0 ) { rtb_B_392_3426_0 =
rtb_B_392_3878_0 - rtb_B_392_3426_0 ; _rtB -> B_331_37_0 = ( ( _rtB ->
B_331_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_b ) * rtb_B_392_3426_0 * _rtP ->
P_297 + _rtB -> B_331_11_0 [ 1 ] ) * ( rtb_B_392_3426_0 / rtb_B_392_3878_0 )
+ ( _rtB -> B_331_25_0 - _rtB -> B_331_30_0 ) * rtb_B_392_4136_0 ; } else {
_rtB -> B_331_37_0 = _rtDW -> UnitDelay1_DSTATE_o ; } rtb_B_392_790_0 =
muDoubleScalarHypot ( _rtB -> B_331_24_0 , _rtB -> B_331_37_0 ) ; if (
rtb_B_392_790_0 > _rtP -> P_336 ) { rtb_B_392_790_0 = _rtP -> P_336 ; } else
{ if ( rtb_B_392_790_0 < _rtP -> P_337 ) { rtb_B_392_790_0 = _rtP -> P_337 ;
} } _rtB -> B_331_41_0 = 1.0 / rtb_B_392_790_0 ; B_331_42_0 = _rtP -> P_338 *
muDoubleScalarAtan2 ( _rtB -> B_331_37_0 , _rtB -> B_331_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_n ) ; } } for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_3030 [ isHit + 6 ] * _rtB ->
B_392_4123_0 [ 2 ] + ( _rtP -> P_3030 [ isHit + 3 ] * _rtB -> B_392_4123_0 [
1 ] + _rtP -> P_3030 [ isHit ] * _rtB -> B_392_4123_0 [ 0 ] ) ; }
B_392_4260_0 [ 0 ] = _rtP -> P_3031 * tmp [ 0 ] ; B_392_4260_0 [ 1 ] = _rtP
-> P_3031 * tmp [ 1 ] ; B_392_4260_0 [ 2 ] = _rtP -> P_3031 * tmp [ 2 ] ;
_rtB -> B_392_4264_0 = _rtB -> B_392_4263_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4264_0 , & B_392_4260_0 [
0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystem1_hr , & _rtDW -> Subsystem1_hr
) ; _rtB -> B_392_4268_0 = _rtB -> B_392_4267_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4268_0 , &
B_392_4260_0 [ 0 ] , _rtB -> B_392_79_0 , & _rtB -> Subsystempi2delay_o , &
_rtDW -> Subsystempi2delay_o ) ; if ( _rtB -> B_392_4263_0 != 0 ) { _rtB ->
B_392_4270_0 [ 0 ] = _rtB -> Subsystem1_hr . B_11_0_0 ; _rtB -> B_392_4270_0
[ 1 ] = _rtB -> Subsystem1_hr . B_11_1_0 ; } else { _rtB -> B_392_4270_0 [ 0
] = _rtB -> Subsystempi2delay_o . B_10_0_0 ; _rtB -> B_392_4270_0 [ 1 ] =
_rtB -> Subsystempi2delay_o . B_10_1_0 ; } if ( _rtDW ->
Integ4_SYSTEM_ENABLE_l != 0 ) { _rtB -> B_392_4271_0 = _rtDW ->
Integ4_DSTATE_d ; } else { _rtB -> B_392_4271_0 = _rtP -> P_3035 * _rtB ->
B_392_4270_0 [ 1 ] + _rtDW -> Integ4_DSTATE_d ; } if ( rtb_B_392_4136_0 >
_rtP -> P_3037 ) { rtb_B_392_430_0 = _rtP -> P_3037 ; } else if (
rtb_B_392_4136_0 < _rtP -> P_3038 ) { rtb_B_392_430_0 = _rtP -> P_3038 ; }
else { rtb_B_392_430_0 = rtb_B_392_4136_0 ; } rtb_B_392_3878_0 = 1.0 /
rtb_B_392_430_0 / 5.0e-5 ; rtb_B_392_3426_0 = muDoubleScalarCeil (
rtb_B_392_3878_0 ) ; _rtB -> B_392_4275_0 = _rtP -> P_3039 * rtb_B_392_3426_0
; ssCallAccelRunBlock ( S , 392 , 4276 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_392_4279_0 ) { rtb_B_392_3426_0 =
rtb_B_392_3878_0 - rtb_B_392_3426_0 ; _rtB -> B_392_4283_0 = ( ( _rtB ->
B_392_4270_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_f ) * rtb_B_392_3426_0 * _rtP
-> P_339 + _rtB -> B_392_4270_0 [ 1 ] ) * ( rtb_B_392_3426_0 /
rtb_B_392_3878_0 ) + ( _rtB -> B_392_4271_0 - _rtB -> B_392_4276_0 ) *
rtb_B_392_4136_0 ; } else { _rtB -> B_392_4283_0 = _rtDW ->
UnitDelay1_DSTATE_at ; } _rtB -> B_392_4284_0 = _rtB -> B_392_4283_0 * _rtB
-> B_331_41_0 ; _rtDW -> DiscreteDerivative_tmp_p = ( _rtB -> B_392_4284_0 -
_rtP -> P_3052 [ 1 ] * _rtDW -> DiscreteDerivative_states_c ) / _rtP ->
P_3052 [ 0 ] ; rtb_B_392_790_0 = ( _rtP -> P_3058 * _rtB -> B_392_4284_0 +
_rtDW -> DiscreteTimeIntegrator_DSTATE_f ) + ( _rtP -> P_3051 [ 0 ] * _rtDW
-> DiscreteDerivative_tmp_p + _rtP -> P_3051 [ 1 ] * _rtDW ->
DiscreteDerivative_states_c ) ; if ( rtb_B_392_790_0 > _rtP -> P_3059 ) {
_rtB -> B_392_4289_0 = _rtP -> P_3059 ; } else if ( rtb_B_392_790_0 < _rtP ->
P_3060 ) { _rtB -> B_392_4289_0 = _rtP -> P_3060 ; } else { _rtB ->
B_392_4289_0 = rtb_B_392_790_0 ; } rtb_B_392_4136_0 = _rtP -> P_3061 * _rtB
-> B_392_4289_0 ; rtb_B_392_16_0 = rtb_B_392_4136_0 - _rtDW -> PrevY_i ; if (
rtb_B_392_16_0 > _rtP -> P_3062 ) { rtb_B_392_4136_0 = _rtDW -> PrevY_i +
_rtP -> P_3062 ; } else { if ( rtb_B_392_16_0 < _rtP -> P_3063 ) {
rtb_B_392_4136_0 = _rtDW -> PrevY_i + _rtP -> P_3063 ; } } _rtDW -> PrevY_i =
rtb_B_392_4136_0 ; _rtB -> B_392_4301_0 = ( _rtP -> P_3066 * _rtDW ->
Delay_x1_DSTATE_h + _rtP -> P_3068 * _rtDW -> Delay_x2_DSTATE_c ) + _rtP ->
P_3071 * rtb_B_392_4136_0 ; _rtB -> B_392_4303_0 = ( _rtP -> P_3069 * _rtDW
-> Delay_x1_DSTATE_h + _rtP -> P_3070 * _rtDW -> Delay_x2_DSTATE_c ) + _rtP
-> P_3072 * rtb_B_392_4136_0 ; _rtB -> B_392_4308_0 = ( _rtP -> P_3074 *
_rtDW -> Delay_x1_DSTATE_h + _rtP -> P_3075 * _rtDW -> Delay_x2_DSTATE_c ) +
_rtP -> P_3073 * rtb_B_392_4136_0 ; } _rtB -> B_392_4310_0 = (
muDoubleScalarSin ( _rtP -> P_3078 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3079
) * _rtP -> P_3076 + _rtP -> P_3077 ) * _rtB -> B_392_4123_0 [ 0 ] ; _rtB ->
B_392_4312_0 = ( muDoubleScalarSin ( _rtP -> P_3082 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3083 ) * _rtP -> P_3080 + _rtP -> P_3081 ) * _rtB -> B_392_4123_0
[ 0 ] ; _rtB -> B_392_4314_0 = ( muDoubleScalarSin ( _rtP -> P_3086 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3087 ) * _rtP -> P_3084 + _rtP -> P_3085
) * _rtB -> B_392_4123_0 [ 1 ] ; _rtB -> B_392_4316_0 = ( muDoubleScalarSin (
_rtP -> P_3090 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3091 ) * _rtP -> P_3088
+ _rtP -> P_3089 ) * _rtB -> B_392_4123_0 [ 1 ] ; _rtB -> B_392_4318_0 = (
muDoubleScalarSin ( _rtP -> P_3094 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3095
) * _rtP -> P_3092 + _rtP -> P_3093 ) * _rtB -> B_392_4123_0 [ 2 ] ; _rtB ->
B_392_4320_0 = ( muDoubleScalarSin ( _rtP -> P_3098 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3099 ) * _rtP -> P_3096 + _rtP -> P_3097 ) * _rtB -> B_392_4123_0
[ 2 ] ; B_392_4321_0 = _rtP -> P_3100 * rtb_B_392_237_0 ; _rtB ->
B_392_4323_0 = ( muDoubleScalarSin ( _rtP -> P_3103 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3104 ) * _rtP -> P_3101 + _rtP -> P_3102 ) * _rtB -> B_392_4250_0
[ 0 ] ; _rtB -> B_392_4325_0 = ( muDoubleScalarSin ( _rtP -> P_3107 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3108 ) * _rtP -> P_3105 + _rtP -> P_3106
) * _rtB -> B_392_4250_0 [ 0 ] ; _rtB -> B_392_4327_0 = ( muDoubleScalarSin (
_rtP -> P_3111 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3112 ) * _rtP -> P_3109
+ _rtP -> P_3110 ) * _rtB -> B_392_4250_0 [ 1 ] ; _rtB -> B_392_4329_0 = (
muDoubleScalarSin ( _rtP -> P_3115 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3116
) * _rtP -> P_3113 + _rtP -> P_3114 ) * _rtB -> B_392_4250_0 [ 1 ] ; _rtB ->
B_392_4331_0 = ( muDoubleScalarSin ( _rtP -> P_3119 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3120 ) * _rtP -> P_3117 + _rtP -> P_3118 ) * _rtB -> B_392_4250_0
[ 2 ] ; _rtB -> B_392_4333_0 = ( muDoubleScalarSin ( _rtP -> P_3123 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3124 ) * _rtP -> P_3121 + _rtP -> P_3122
) * _rtB -> B_392_4250_0 [ 2 ] ; B_392_4334_0 = _rtP -> P_3125 *
rtb_B_392_239_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
B_392_4375_0 = _rtP -> P_3127 * _rtB -> B_0_14_0 [ 127 ] * ( _rtP -> P_3128 *
_rtB -> B_0_14_0 [ 72 ] ) ; _rtB -> B_392_4389_0 = _rtP -> P_3129 *
rtb_B_392_185_0 + _rtP -> P_3130 * _rtB -> B_0_23_0 ; _rtB -> B_392_4395_0 =
_rtP -> P_3131 * rtb_B_392_183_0 + _rtP -> P_3132 * _rtB -> B_0_27_0 ; _rtB
-> B_392_4403_0 = _rtP -> P_3133 * _rtB -> B_0_14_0 [ 75 ] ; _rtB ->
B_392_4407_0 = _rtP -> P_3134 * _rtB -> B_0_14_0 [ 129 ] ; _rtB ->
B_392_4411_0 = _rtP -> P_3135 * _rtB -> B_0_14_0 [ 130 ] ; if ( _rtB ->
B_0_14_1 [ 21 ] >= _rtP -> P_3137 ) { rtb_B_392_4136_0 = _rtP -> P_294 * _rtB
-> B_0_14_0 [ 21 ] ; } else { rtb_B_392_4136_0 = _rtB -> B_392_4424_0 ; } if
( rtb_B_392_4136_0 > _rtP -> P_3138 ) { B_392_4427_0 = _rtP -> P_3138 ; }
else if ( rtb_B_392_4136_0 < _rtP -> P_3139 ) { B_392_4427_0 = _rtP -> P_3139
; } else { B_392_4427_0 = rtb_B_392_4136_0 ; } B_392_4428_0 = _rtB ->
B_0_14_0 [ 21 ] + _rtB -> B_392_231_0 ; if ( _rtB -> B_0_14_1 [ 22 ] >= _rtP
-> P_3141 ) { rtb_B_392_4136_0 = _rtP -> P_295 * _rtB -> B_0_14_0 [ 22 ] ; }
else { rtb_B_392_4136_0 = _rtB -> B_392_4432_0 ; } if ( rtb_B_392_4136_0 >
_rtP -> P_3142 ) { B_392_4435_0 = _rtP -> P_3142 ; } else if (
rtb_B_392_4136_0 < _rtP -> P_3143 ) { B_392_4435_0 = _rtP -> P_3143 ; } else
{ B_392_4435_0 = rtb_B_392_4136_0 ; } B_392_4436_0 = _rtB -> B_0_14_0 [ 22 ]
+ _rtB -> B_392_232_0 ; _rtB -> B_392_4450_0 = _rtP -> P_3144 * _rtB ->
B_0_14_0 [ 73 ] ; ssCallAccelRunBlock ( S , 392 , 4451 , SS_CALL_MDL_OUTPUTS
) ; } _rtB -> B_392_4461_0 = 0.0 ; _rtB -> B_392_4461_0 += _rtP -> P_3146 *
_rtX -> LPF1_CSTATE_a ; _rtB -> B_392_4462_0 = 0.0 ; _rtB -> B_392_4462_0 +=
_rtP -> P_3148 * _rtX -> LPF2_CSTATE_d ; _rtB -> B_392_4463_0 = _rtX ->
integrator_CSTATE_ib ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3150 ;
B_392_4464_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ml .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ml . Last , _rtDW ->
TransportDelay_IWORK_ml . Tail , _rtDW -> TransportDelay_IWORK_ml . Head ,
_rtP -> P_3151 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4466_0 = _rtP -> P_3152 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4466_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4468_0 =
_rtDW -> Memory_PreviousInput_on ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4470_0 = ( _rtB -> B_392_4463_0 - B_392_4464_0 ) * _rtP -> P_399 ; }
else { _rtB -> B_392_4470_0 = _rtB -> B_392_4468_0 ; } _rtB -> B_392_4471_0 =
_rtX -> integrator_CSTATE_j3 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ie . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ie . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3155 ;
B_392_4472_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_av .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_av . Last , _rtDW ->
TransportDelay_IWORK_av . Tail , _rtDW -> TransportDelay_IWORK_av . Head ,
_rtP -> P_3156 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4474_0 = _rtP -> P_3157 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4474_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4476_0 =
_rtDW -> Memory_PreviousInput_nh ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4478_0 = ( _rtB -> B_392_4471_0 - B_392_4472_0 ) * _rtP -> P_398 ; }
else { _rtB -> B_392_4478_0 = _rtB -> B_392_4476_0 ; } rtb_B_392_4136_0 =
_rtP -> P_3159 * muDoubleScalarAtan2 ( _rtB -> B_392_4478_0 , _rtB ->
B_392_4470_0 ) * _rtP -> P_3160 ; B_392_4483_0 . re = muDoubleScalarHypot (
_rtB -> B_392_4470_0 , _rtB -> B_392_4478_0 ) * muDoubleScalarCos (
rtb_B_392_4136_0 ) ; B_392_4483_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_4470_0 , _rtB -> B_392_4478_0 ) * muDoubleScalarSin ( rtb_B_392_4136_0
) ; _rtB -> B_392_4484_0 = _rtX -> integrator_CSTATE_bx ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3162 ; B_392_4485_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bcn . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_bcn . Last , _rtDW -> TransportDelay_IWORK_bcn
. Tail , _rtDW -> TransportDelay_IWORK_bcn . Head , _rtP -> P_3163 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4487_0 = _rtP -> P_3164 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4487_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4489_0 = _rtDW -> Memory_PreviousInput_gw ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4491_0 = ( _rtB -> B_392_4484_0 -
B_392_4485_0 ) * _rtP -> P_401 ; } else { _rtB -> B_392_4491_0 = _rtB ->
B_392_4489_0 ; } _rtB -> B_392_4492_0 = _rtX -> integrator_CSTATE_h0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_hm . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3167 ; B_392_4493_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ge . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ge . Last , _rtDW -> TransportDelay_IWORK_ge .
Tail , _rtDW -> TransportDelay_IWORK_ge . Head , _rtP -> P_3168 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4495_0 = _rtP -> P_3169 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4495_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4497_0 = _rtDW -> Memory_PreviousInput_ln ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4499_0 = ( _rtB -> B_392_4492_0 -
B_392_4493_0 ) * _rtP -> P_400 ; } else { _rtB -> B_392_4499_0 = _rtB ->
B_392_4497_0 ; } rtb_B_392_4136_0 = _rtP -> P_3171 * muDoubleScalarAtan2 (
_rtB -> B_392_4499_0 , _rtB -> B_392_4491_0 ) * _rtP -> P_3172 ; B_392_4504_0
. re = muDoubleScalarHypot ( _rtB -> B_392_4491_0 , _rtB -> B_392_4499_0 ) *
muDoubleScalarCos ( rtb_B_392_4136_0 ) ; B_392_4504_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_4491_0 , _rtB -> B_392_4499_0 ) *
muDoubleScalarSin ( rtb_B_392_4136_0 ) ; _rtB -> B_392_4505_0 = _rtX ->
integrator_CSTATE_pa ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_agi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_agi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3174 ;
B_392_4506_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_np .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_np . Last , _rtDW ->
TransportDelay_IWORK_np . Tail , _rtDW -> TransportDelay_IWORK_np . Head ,
_rtP -> P_3175 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4508_0 = _rtP -> P_3176 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4508_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4510_0 =
_rtDW -> Memory_PreviousInput_cy ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4512_0 = ( _rtB -> B_392_4505_0 - B_392_4506_0 ) * _rtP -> P_403 ; }
else { _rtB -> B_392_4512_0 = _rtB -> B_392_4510_0 ; } _rtB -> B_392_4513_0 =
_rtX -> integrator_CSTATE_ew ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_iyk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3179
; B_392_4514_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_p2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_p2 . Last , _rtDW ->
TransportDelay_IWORK_p2 . Tail , _rtDW -> TransportDelay_IWORK_p2 . Head ,
_rtP -> P_3180 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4516_0 = _rtP -> P_3181 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4516_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4518_0 =
_rtDW -> Memory_PreviousInput_lj ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4520_0 = ( _rtB -> B_392_4513_0 - B_392_4514_0 ) * _rtP -> P_402 ; }
else { _rtB -> B_392_4520_0 = _rtB -> B_392_4518_0 ; } rtb_B_392_4136_0 =
_rtP -> P_3183 * muDoubleScalarAtan2 ( _rtB -> B_392_4520_0 , _rtB ->
B_392_4512_0 ) * _rtP -> P_3184 ; B_392_4525_0 . re = muDoubleScalarHypot (
_rtB -> B_392_4512_0 , _rtB -> B_392_4520_0 ) * muDoubleScalarCos (
rtb_B_392_4136_0 ) ; B_392_4525_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_4512_0 , _rtB -> B_392_4520_0 ) * muDoubleScalarSin ( rtb_B_392_4136_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_4527_0 = _rtB -> B_392_4526_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_4527_0 ,
B_392_4483_0 , B_392_4504_0 , B_392_4525_0 , & _rtB -> PosSeqComputation_jw ,
& _rtDW -> PosSeqComputation_jw , & _rtP -> PosSeqComputation_jw ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4530_0 = _rtB
-> B_392_4529_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_4530_0 , B_392_4483_0 , B_392_4504_0 , B_392_4525_0 , & _rtB ->
NegSeqComputation_p , & _rtDW -> NegSeqComputation_p , & _rtP ->
NegSeqComputation_p ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4533_0 = _rtB -> B_392_4532_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_4533_0 ,
B_392_4483_0 , B_392_4504_0 , B_392_4525_0 , & _rtB -> ZeroSeqComputation_jv
, & _rtDW -> ZeroSeqComputation_jv , & _rtP -> ZeroSeqComputation_jv ) ;
rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_jw .
B_23_2_0 . im , _rtB -> PosSeqComputation_jw . B_23_2_0 . re ) ; _rtB ->
B_392_4536_0 = _rtP -> P_3188 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_jw . B_23_2_0 . re , _rtB -> PosSeqComputation_jw .
B_23_2_0 . im ) ; _rtB -> B_392_4537_0 = _rtX -> integrator_CSTATE_hh ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ais .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ais . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3190 ; B_392_4538_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_e0 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_e0 . Last , _rtDW -> TransportDelay_IWORK_e0 .
Tail , _rtDW -> TransportDelay_IWORK_e0 . Head , _rtP -> P_3191 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4540_0 = _rtP -> P_3192 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4540_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4542_0 = _rtDW -> Memory_PreviousInput_ko ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4544_0 = ( _rtB -> B_392_4537_0 -
B_392_4538_0 ) * _rtP -> P_405 ; } else { _rtB -> B_392_4544_0 = _rtB ->
B_392_4542_0 ; } _rtB -> B_392_4545_0 = _rtX -> integrator_CSTATE_fcw ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fh .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fh . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3195 ; B_392_4546_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_jr . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_jr . Last , _rtDW -> TransportDelay_IWORK_jr .
Tail , _rtDW -> TransportDelay_IWORK_jr . Head , _rtP -> P_3196 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4548_0 = _rtP -> P_3197 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4548_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4550_0 = _rtDW -> Memory_PreviousInput_dc ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4552_0 = ( _rtB -> B_392_4545_0 -
B_392_4546_0 ) * _rtP -> P_404 ; } else { _rtB -> B_392_4552_0 = _rtB ->
B_392_4550_0 ; } rtb_B_392_4136_0 = _rtP -> P_3199 * muDoubleScalarAtan2 (
_rtB -> B_392_4552_0 , _rtB -> B_392_4544_0 ) * _rtP -> P_3200 ; B_392_4557_0
. re = muDoubleScalarHypot ( _rtB -> B_392_4544_0 , _rtB -> B_392_4552_0 ) *
muDoubleScalarCos ( rtb_B_392_4136_0 ) ; B_392_4557_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_4544_0 , _rtB -> B_392_4552_0 ) *
muDoubleScalarSin ( rtb_B_392_4136_0 ) ; _rtB -> B_392_4558_0 = _rtX ->
integrator_CSTATE_kg1 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dq . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dq . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3202 ;
B_392_4559_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_g2 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_g2 . Last , _rtDW ->
TransportDelay_IWORK_g2 . Tail , _rtDW -> TransportDelay_IWORK_g2 . Head ,
_rtP -> P_3203 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4561_0 = _rtP -> P_3204 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4561_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4563_0 =
_rtDW -> Memory_PreviousInput_jl ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4565_0 = ( _rtB -> B_392_4558_0 - B_392_4559_0 ) * _rtP -> P_407 ; }
else { _rtB -> B_392_4565_0 = _rtB -> B_392_4563_0 ; } _rtB -> B_392_4566_0 =
_rtX -> integrator_CSTATE_it ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ejf . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3207
; B_392_4567_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kq .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_kq . Last , _rtDW ->
TransportDelay_IWORK_kq . Tail , _rtDW -> TransportDelay_IWORK_kq . Head ,
_rtP -> P_3208 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4569_0 = _rtP -> P_3209 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_4569_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4571_0 =
_rtDW -> Memory_PreviousInput_gk ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_4573_0 = ( _rtB -> B_392_4566_0 - B_392_4567_0 ) * _rtP -> P_406 ; }
else { _rtB -> B_392_4573_0 = _rtB -> B_392_4571_0 ; } rtb_B_392_4136_0 =
_rtP -> P_3211 * muDoubleScalarAtan2 ( _rtB -> B_392_4573_0 , _rtB ->
B_392_4565_0 ) * _rtP -> P_3212 ; B_392_4578_0 . re = muDoubleScalarHypot (
_rtB -> B_392_4565_0 , _rtB -> B_392_4573_0 ) * muDoubleScalarCos (
rtb_B_392_4136_0 ) ; B_392_4578_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_4565_0 , _rtB -> B_392_4573_0 ) * muDoubleScalarSin ( rtb_B_392_4136_0
) ; _rtB -> B_392_4579_0 = _rtX -> integrator_CSTATE_fg ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3214 ; B_392_4580_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_oh . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_oh . Last , _rtDW -> TransportDelay_IWORK_oh .
Tail , _rtDW -> TransportDelay_IWORK_oh . Head , _rtP -> P_3215 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4582_0 = _rtP -> P_3216 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4582_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4584_0 = _rtDW -> Memory_PreviousInput_kh ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4586_0 = ( _rtB -> B_392_4579_0 -
B_392_4580_0 ) * _rtP -> P_409 ; } else { _rtB -> B_392_4586_0 = _rtB ->
B_392_4584_0 ; } _rtB -> B_392_4587_0 = _rtX -> integrator_CSTATE_jb ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bz .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bz . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3219 ; B_392_4588_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_eq . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_eq . Last , _rtDW -> TransportDelay_IWORK_eq .
Tail , _rtDW -> TransportDelay_IWORK_eq . Head , _rtP -> P_3220 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4590_0 = _rtP -> P_3221 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4590_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_4592_0 = _rtDW -> Memory_PreviousInput_kl ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_4594_0 = ( _rtB -> B_392_4587_0 -
B_392_4588_0 ) * _rtP -> P_408 ; } else { _rtB -> B_392_4594_0 = _rtB ->
B_392_4592_0 ; } rtb_B_392_4136_0 = _rtP -> P_3223 * muDoubleScalarAtan2 (
_rtB -> B_392_4594_0 , _rtB -> B_392_4586_0 ) * _rtP -> P_3224 ; B_392_4599_0
. re = muDoubleScalarHypot ( _rtB -> B_392_4586_0 , _rtB -> B_392_4594_0 ) *
muDoubleScalarCos ( rtb_B_392_4136_0 ) ; B_392_4599_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_4586_0 , _rtB -> B_392_4594_0 ) *
muDoubleScalarSin ( rtb_B_392_4136_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_4601_0 = _rtB -> B_392_4600_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_4601_0 ,
B_392_4557_0 , B_392_4578_0 , B_392_4599_0 , & _rtB -> PosSeqComputation_bv ,
& _rtDW -> PosSeqComputation_bv , & _rtP -> PosSeqComputation_bv ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4604_0 = _rtB
-> B_392_4603_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_4604_0 , B_392_4557_0 , B_392_4578_0 , B_392_4599_0 , & _rtB ->
NegSeqComputation_ih , & _rtDW -> NegSeqComputation_ih , & _rtP ->
NegSeqComputation_ih ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_4607_0 = _rtB -> B_392_4606_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_4607_0 ,
B_392_4557_0 , B_392_4578_0 , B_392_4599_0 , & _rtB -> ZeroSeqComputation_i ,
& _rtDW -> ZeroSeqComputation_i , & _rtP -> ZeroSeqComputation_i ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_bv .
B_23_2_0 . im , _rtB -> PosSeqComputation_bv . B_23_2_0 . re ) ; _rtB ->
B_392_4610_0 = _rtP -> P_3228 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_bv . B_23_2_0 . re , _rtB -> PosSeqComputation_bv .
B_23_2_0 . im ) ; ssCallAccelRunBlock ( S , 392 , 4612 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_4613_0 = _rtP -> P_3230 * _rtB -> B_0_14_0 [ 82 ] ; } _rtB ->
B_392_4080_0 [ 0 ] = _rtB -> B_392_4461_0 ; _rtB -> B_392_4080_0 [ 1 ] = _rtB
-> B_392_4462_0 ; _rtB -> B_392_4080_0 [ 2 ] = _rtB -> B_392_4536_0 ; _rtB ->
B_392_4080_0 [ 3 ] = _rtB -> B_392_4610_0 ; _rtB -> B_392_4080_0 [ 4 ] = _rtB
-> B_392_4613_0 ; ssCallAccelRunBlock ( S , 392 , 4615 , SS_CALL_MDL_OUTPUTS
) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { tmp_0 = _rtP ->
P_3232 * _rtB -> B_0_14_0 [ 142 ] * _rtP -> P_3235 ; tmp_1 = _rtP -> P_3233 *
_rtB -> B_0_14_0 [ 143 ] * _rtP -> P_3235 ; rtb_B_392_16_0 = _rtP -> P_3234 *
_rtB -> B_0_14_0 [ 144 ] * _rtP -> P_3235 ; for ( isHit = 0 ; isHit < 3 ;
isHit ++ ) { tmp [ isHit ] = _rtP -> P_3236 [ isHit + 6 ] * rtb_B_392_16_0 +
( _rtP -> P_3236 [ isHit + 3 ] * tmp_1 + _rtP -> P_3236 [ isHit ] * tmp_0 ) ;
} B_392_4622_0 [ 0 ] = _rtP -> P_3237 * tmp [ 0 ] ; B_392_4622_0 [ 1 ] = _rtP
-> P_3237 * tmp [ 1 ] ; B_392_4622_0 [ 2 ] = _rtP -> P_3237 * tmp [ 2 ] ;
_rtB -> B_392_4626_0 = _rtB -> B_392_4625_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4626_0 , & B_392_4622_0 [
0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystem1_n , & _rtDW -> Subsystem1_n
) ; _rtB -> B_392_4630_0 = _rtB -> B_392_4629_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4630_0 , &
B_392_4622_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystempi2delay_m , &
_rtDW -> Subsystempi2delay_m ) ; if ( _rtB -> B_392_4625_0 != 0 ) { _rtB ->
B_392_4632_0 [ 0 ] = _rtB -> Subsystem1_n . B_11_0_0 ; _rtB -> B_392_4632_0 [
1 ] = _rtB -> Subsystem1_n . B_11_1_0 ; } else { _rtB -> B_392_4632_0 [ 0 ] =
_rtB -> Subsystempi2delay_m . B_10_0_0 ; _rtB -> B_392_4632_0 [ 1 ] = _rtB ->
Subsystempi2delay_m . B_10_1_0 ; } } _rtB -> B_392_4633_0 = 0.0 ; _rtB ->
B_392_4633_0 += _rtP -> P_3242 * _rtX -> Integradord_CSTATE_an ; isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4636_0 = (
_rtB -> B_392_4634_0 - _rtB -> B_392_4613_0 ) * _rtP -> P_3244 ; _rtB ->
B_392_4637_0 = _rtP -> P_3245 * _rtB -> B_392_4636_0 ; } _rtB -> B_392_4638_0
= _rtB -> B_392_4633_0 + _rtB -> B_392_4637_0 ; _rtB -> B_392_4640_0 = _rtB
-> B_392_4639_0 - _rtB -> B_392_4462_0 ; _rtB -> B_392_4641_0 = _rtP ->
P_3247 * _rtB -> B_392_4640_0 ; _rtB -> B_392_4642_0 = _rtP -> P_3248 * _rtB
-> B_392_4641_0 ; _rtB -> B_392_4643_0 = 0.0 ; _rtB -> B_392_4643_0 += _rtP
-> P_3250 * _rtX -> Integradord1_CSTATE_c ; _rtB -> B_392_4644_0 = _rtB ->
B_392_4642_0 + _rtB -> B_392_4643_0 ; _rtB -> B_392_4645_0 . re = _rtB ->
B_392_4638_0 ; _rtB -> B_392_4645_0 . im = _rtB -> B_392_4644_0 ; _rtB ->
B_392_4646_0 = muDoubleScalarHypot ( _rtB -> B_392_4645_0 . re , _rtB ->
B_392_4645_0 . im ) ; rtb_B_392_4136_0 = muDoubleScalarAtan2 ( _rtB ->
B_392_4645_0 . im , _rtB -> B_392_4645_0 . re ) ; if ( ssIsMajorTimeStep ( S
) != 0 ) { _rtDW -> Saturation_MODE_i = _rtB -> B_392_4646_0 >= _rtP ->
P_3251 ? 1 : _rtB -> B_392_4646_0 > _rtP -> P_3252 ? 0 : - 1 ; } _rtB ->
B_392_4647_0 = _rtDW -> Saturation_MODE_i == 1 ? _rtP -> P_3251 : _rtDW ->
Saturation_MODE_i == - 1 ? _rtP -> P_3252 : _rtB -> B_392_4646_0 ; _rtB ->
B_392_4649_0 = _rtB -> B_392_4647_0 * muDoubleScalarCos ( rtb_B_392_4136_0 )
; _rtB -> B_392_4649_1 = _rtB -> B_392_4647_0 * muDoubleScalarSin (
rtb_B_392_4136_0 ) ; _rtB -> B_392_4650_0 = _rtB -> B_392_4649_0 - _rtB ->
B_392_4632_0 [ 0 ] ; _rtB -> B_392_4651_0 = 0.0 ; _rtB -> B_392_4651_0 +=
_rtP -> P_3254 * _rtX -> Controled_CSTATE_o ; _rtB -> B_392_4651_0 += _rtP ->
P_3255 * _rtB -> B_392_4650_0 ; _rtB -> B_392_4652_0 = _rtB -> B_392_4649_1 -
_rtB -> B_392_4632_0 [ 1 ] ; _rtB -> B_392_4653_0 = 0.0 ; _rtB ->
B_392_4653_0 += _rtP -> P_3257 * _rtX -> Controleq_CSTATE_j ; _rtB ->
B_392_4653_0 += _rtP -> P_3258 * _rtB -> B_392_4652_0 ; isHit = ssIsSampleHit
( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4657_0 [ 0 ] = _rtP ->
P_3259 * _rtB -> B_0_14_0 [ 90 ] * _rtP -> P_3262 ; _rtB -> B_392_4657_0 [ 1
] = _rtP -> P_3260 * _rtB -> B_0_14_0 [ 91 ] * _rtP -> P_3262 ; _rtB ->
B_392_4657_0 [ 2 ] = _rtP -> P_3261 * _rtB -> B_0_14_0 [ 92 ] * _rtP ->
P_3262 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] = _rtP ->
P_3263 [ isHit + 6 ] * _rtB -> B_392_4657_0 [ 2 ] + ( _rtP -> P_3263 [ isHit
+ 3 ] * _rtB -> B_392_4657_0 [ 1 ] + _rtP -> P_3263 [ isHit ] * _rtB ->
B_392_4657_0 [ 0 ] ) ; } B_392_4659_0 [ 0 ] = _rtP -> P_3264 * tmp [ 0 ] ;
B_392_4659_0 [ 1 ] = _rtP -> P_3264 * tmp [ 1 ] ; B_392_4659_0 [ 2 ] = _rtP
-> P_3264 * tmp [ 2 ] ; _rtB -> B_392_4663_0 = _rtB -> B_392_4662_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4663_0 , & B_392_4659_0 [
0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystem1_i4 , & _rtDW ->
Subsystem1_i4 ) ; _rtB -> B_392_4667_0 = _rtB -> B_392_4666_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4667_0 , &
B_392_4659_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystempi2delay_cjn ,
& _rtDW -> Subsystempi2delay_cjn ) ; if ( _rtB -> B_392_4662_0 != 0 ) { _rtB
-> B_392_4669_0 [ 0 ] = _rtB -> Subsystem1_i4 . B_11_0_0 ; _rtB ->
B_392_4669_0 [ 1 ] = _rtB -> Subsystem1_i4 . B_11_1_0 ; } else { _rtB ->
B_392_4669_0 [ 0 ] = _rtB -> Subsystempi2delay_cjn . B_10_0_0 ; _rtB ->
B_392_4669_0 [ 1 ] = _rtB -> Subsystempi2delay_cjn . B_10_1_0 ; } } isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_4670_0 = _rtDW ->
UnitDelay_DSTATE_j ; _rtB -> B_392_4671_0 = _rtP -> P_3269 * _rtDW ->
UnitDelay_DSTATE_j ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4673_0 = _rtB -> B_392_4671_0 * _rtB -> B_392_4632_0 [ 1 ]
* _rtP -> P_3270 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ _rtB -> B_392_4675_0 = _rtB -> B_392_4613_0 * _rtB -> B_392_4616_0 ; } _rtB
-> B_392_4676_0 = ( ( _rtB -> B_392_4669_0 [ 0 ] + _rtB -> B_392_4651_0 ) -
_rtB -> B_392_4673_0 ) / _rtB -> B_392_4675_0 ; isHit = ssIsSampleHit ( S , 2
, 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_4678_0 = _rtB -> B_392_4632_0 [ 0 ]
* _rtB -> B_392_4671_0 * _rtP -> P_3271 ; } _rtB -> B_392_4680_0 = ( ( _rtB
-> B_392_4678_0 + _rtB -> B_392_4653_0 ) + _rtB -> B_392_4669_0 [ 1 ] ) * (
1.0 / _rtB -> B_392_4675_0 ) ; if ( ssIsMajorTimeStep ( S ) != 0 ) { _rtDW ->
Saturation_MODE_e = _rtB -> B_392_4680_0 >= _rtP -> P_3272 ? 1 : _rtB ->
B_392_4680_0 > _rtP -> P_3273 ? 0 : - 1 ; _rtDW -> Saturation1_MODE_l = _rtB
-> B_392_4676_0 >= _rtP -> P_3274 ? 1 : _rtB -> B_392_4676_0 > _rtP -> P_3275
? 0 : - 1 ; } _rtB -> B_392_4681_0 = _rtDW -> Saturation_MODE_e == 1 ? _rtP
-> P_3272 : _rtDW -> Saturation_MODE_e == - 1 ? _rtP -> P_3273 : _rtB ->
B_392_4680_0 ; _rtB -> B_392_4682_0 = _rtDW -> Saturation1_MODE_l == 1 ? _rtP
-> P_3274 : _rtDW -> Saturation1_MODE_l == - 1 ? _rtP -> P_3275 : _rtB ->
B_392_4676_0 ; _rtB -> B_392_4683_0 = _rtB -> B_392_4638_0 - _rtB ->
B_392_4649_0 ; _rtB -> B_392_4684_0 = _rtP -> P_3276 * _rtB -> B_392_4683_0 ;
_rtB -> B_392_4685_0 = _rtB -> B_392_4636_0 - _rtB -> B_392_4684_0 ; _rtB ->
B_392_4686_0 = _rtP -> P_3277 * _rtB -> B_392_4685_0 ; _rtB -> B_392_4687_0 =
_rtB -> B_392_4644_0 - _rtB -> B_392_4649_1 ; _rtB -> B_392_4688_0 = _rtP ->
P_3278 * _rtB -> B_392_4687_0 ; _rtB -> B_392_4689_0 = _rtB -> B_392_4641_0 -
_rtB -> B_392_4688_0 ; _rtB -> B_392_4690_0 = _rtP -> P_3279 * _rtB ->
B_392_4689_0 ; ssCallAccelRunBlock ( S , 392 , 4691 , SS_CALL_MDL_OUTPUTS ) ;
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { B_392_4696_0 = _rtP
-> P_3280 * _rtB -> B_0_14_0 [ 137 ] * _rtB -> B_392_4613_0 ; B_392_4708_0 [
0 ] = _rtP -> P_3281 * _rtB -> B_0_14_0 [ 87 ] * _rtP -> P_3284 ;
B_392_4708_0 [ 1 ] = _rtP -> P_3282 * _rtB -> B_0_14_0 [ 88 ] * _rtP ->
P_3284 ; B_392_4708_0 [ 2 ] = _rtP -> P_3283 * _rtB -> B_0_14_0 [ 89 ] * _rtP
-> P_3284 ; _rtB -> B_392_4751_0 = ( _rtB -> B_392_119_0 * _rtB -> B_0_14_0 [
154 ] - _rtB -> B_392_120_0 * _rtB -> B_0_14_0 [ 155 ] ) / ( ( _rtP -> P_3286
* _rtB -> B_392_116_0 - _rtDW -> prediction_DSTATE_j ) + _rtB -> B_392_4743_0
) ; _rtB -> B_392_4769_0 = ( _rtB -> B_392_4669_0 [ 0 ] * _rtB ->
B_392_4632_0 [ 0 ] + _rtB -> B_392_4669_0 [ 1 ] * _rtB -> B_392_4632_0 [ 1 ]
) * _rtP -> P_3288 ; _rtB -> B_392_4773_0 = ( _rtB -> B_392_4632_0 [ 0 ] *
_rtB -> B_392_4669_0 [ 1 ] - _rtB -> B_392_4669_0 [ 0 ] * _rtB ->
B_392_4632_0 [ 1 ] ) * _rtP -> P_3289 ; _rtB -> B_392_4779_0 = _rtB ->
B_392_4778_0 ; _rtB -> B_392_4780_0 = _rtB -> B_392_4776_0 ; _rtB ->
B_392_4784_0 [ 0 ] = _rtP -> P_3293 * _rtB -> B_0_14_0 [ 145 ] * _rtP ->
P_3296 ; _rtB -> B_392_4784_0 [ 1 ] = _rtP -> P_3294 * _rtB -> B_0_14_0 [ 146
] * _rtP -> P_3296 ; _rtB -> B_392_4784_0 [ 2 ] = _rtP -> P_3295 * _rtB ->
B_0_14_0 [ 147 ] * _rtP -> P_3296 ; for ( isHit = 0 ; isHit < 3 ; isHit ++ )
{ tmp [ isHit ] = _rtP -> P_3297 [ isHit + 6 ] * _rtB -> B_392_4784_0 [ 2 ] +
( _rtP -> P_3297 [ isHit + 3 ] * _rtB -> B_392_4784_0 [ 1 ] + _rtP -> P_3297
[ isHit ] * _rtB -> B_392_4784_0 [ 0 ] ) ; } B_392_4786_0 [ 0 ] = _rtP ->
P_3298 * tmp [ 0 ] ; B_392_4786_0 [ 1 ] = _rtP -> P_3298 * tmp [ 1 ] ;
B_392_4786_0 [ 2 ] = _rtP -> P_3298 * tmp [ 2 ] ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4779_0 , &
B_392_4786_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystempi2delay_kg , &
_rtDW -> Subsystempi2delay_kg ) ; MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB
-> B_392_4780_0 , & B_392_4786_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB ->
Subsystem1_o , & _rtDW -> Subsystem1_o ) ; if ( _rtB -> B_392_4776_0 != 0 ) {
B_392_4789_0 [ 0 ] = _rtB -> Subsystem1_o . B_11_0_0 ; B_392_4789_0 [ 1 ] =
_rtB -> Subsystem1_o . B_11_1_0 ; } else { B_392_4789_0 [ 0 ] = _rtB ->
Subsystempi2delay_kg . B_10_0_0 ; B_392_4789_0 [ 1 ] = _rtB ->
Subsystempi2delay_kg . B_10_1_0 ; } } isHit = ssIsSampleHit ( S , 3 , 0 ) ;
if ( isHit != 0 ) { _rtB -> B_392_4791_0 = _rtP -> P_3299 ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( ( isHit != 0 ) && ( ssIsMajorTimeStep ( S
) != 0 ) ) { if ( _rtB -> B_392_4791_0 > 0.0 ) { if ( ! _rtDW ->
AutomaticGainControl_MODE ) { if ( ssGetTaskTime ( S , 3 ) != ssGetTStart ( S
) ) { ssSetBlockStateForSolverChangedAtMajorStep ( S ) ; } _rtDW ->
Integ4_SYSTEM_ENABLE_k = 1U ; _rtDW -> Integ4_SYSTEM_ENABLE_b = 1U ; _rtDW ->
AutomaticGainControl_MODE = true ; } } else { if ( _rtDW ->
AutomaticGainControl_MODE ) { ssSetBlockStateForSolverChangedAtMajorStep ( S
) ; _rtDW -> AutomaticGainControl_MODE = false ; } } } if ( _rtDW ->
AutomaticGainControl_MODE ) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if (
isHit != 0 ) { for ( isHit = 0 ; isHit < 3 ; isHit ++ ) { tmp [ isHit ] =
_rtP -> P_357 [ isHit + 6 ] * _rtB -> B_392_4657_0 [ 2 ] + ( _rtP -> P_357 [
isHit + 3 ] * _rtB -> B_392_4657_0 [ 1 ] + _rtP -> P_357 [ isHit ] * _rtB ->
B_392_4657_0 [ 0 ] ) ; } B_368_1_0 [ 0 ] = _rtP -> P_358 * tmp [ 0 ] ;
B_368_1_0 [ 1 ] = _rtP -> P_358 * tmp [ 1 ] ; B_368_1_0 [ 2 ] = _rtP -> P_358
* tmp [ 2 ] ; _rtB -> B_368_5_0 = _rtB -> B_368_4_0 ;
MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_368_5_0 , & B_368_1_0 [ 0 ] ,
_rtB -> B_392_103_0 , & _rtB -> Subsystem1_lu , & _rtDW -> Subsystem1_lu ) ;
_rtB -> B_368_9_0 = _rtB -> B_368_8_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_368_9_0 , & B_368_1_0 [
0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystempi2delay_b , & _rtDW ->
Subsystempi2delay_b ) ; if ( _rtB -> B_368_4_0 != 0 ) { _rtB -> B_368_11_0 [
0 ] = _rtB -> Subsystem1_lu . B_11_0_0 ; _rtB -> B_368_11_0 [ 1 ] = _rtB ->
Subsystem1_lu . B_11_1_0 ; } else { _rtB -> B_368_11_0 [ 0 ] = _rtB ->
Subsystempi2delay_b . B_10_0_0 ; _rtB -> B_368_11_0 [ 1 ] = _rtB ->
Subsystempi2delay_b . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_k != 0
) { _rtB -> B_368_12_0 = _rtDW -> Integ4_DSTATE_j ; } else { _rtB ->
B_368_12_0 = _rtP -> P_362 * _rtB -> B_368_11_0 [ 0 ] + _rtDW ->
Integ4_DSTATE_j ; } if ( rtb_B_392_4670_0 > _rtP -> P_364 ) {
rtb_B_392_4136_0 = _rtP -> P_364 ; } else if ( rtb_B_392_4670_0 < _rtP ->
P_365 ) { rtb_B_392_4136_0 = _rtP -> P_365 ; } else { rtb_B_392_4136_0 =
rtb_B_392_4670_0 ; } rtb_B_392_4136_0 = 1.0 / rtb_B_392_4136_0 / 5.0e-5 ;
rtb_B_392_183_0 = muDoubleScalarCeil ( rtb_B_392_4136_0 ) ; _rtB ->
B_368_16_0 = _rtP -> P_366 * rtb_B_392_183_0 ; ssCallAccelRunBlock ( S , 368
, 17 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_368_20_0 ) { rtb_B_392_183_0 = rtb_B_392_4136_0 - rtb_B_392_183_0 ; _rtB ->
B_368_24_0 = ( ( _rtB -> B_368_11_0 [ 0 ] - _rtDW -> UnitDelay_DSTATE_d ) *
rtb_B_392_183_0 * _rtP -> P_354 + _rtB -> B_368_11_0 [ 0 ] ) * (
rtb_B_392_183_0 / rtb_B_392_4136_0 ) + ( _rtB -> B_368_12_0 - _rtB ->
B_368_17_0 ) * rtb_B_392_4670_0 ; } else { _rtB -> B_368_24_0 = _rtDW ->
UnitDelay1_DSTATE_g ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_b != 0 ) { _rtB ->
B_368_25_0 = _rtDW -> Integ4_DSTATE_bx ; } else { _rtB -> B_368_25_0 = _rtP
-> P_378 * _rtB -> B_368_11_0 [ 1 ] + _rtDW -> Integ4_DSTATE_bx ; } if (
rtb_B_392_4670_0 > _rtP -> P_380 ) { rtb_B_392_4136_0 = _rtP -> P_380 ; }
else if ( rtb_B_392_4670_0 < _rtP -> P_381 ) { rtb_B_392_4136_0 = _rtP ->
P_381 ; } else { rtb_B_392_4136_0 = rtb_B_392_4670_0 ; } rtb_B_392_4136_0 =
1.0 / rtb_B_392_4136_0 / 5.0e-5 ; rtb_B_392_183_0 = muDoubleScalarCeil (
rtb_B_392_4136_0 ) ; _rtB -> B_368_29_0 = _rtP -> P_382 * rtb_B_392_183_0 ;
ssCallAccelRunBlock ( S , 368 , 30 , SS_CALL_MDL_OUTPUTS ) ; if (
ssGetTaskTime ( S , 3 ) >= _rtB -> B_368_33_0 ) { rtb_B_392_183_0 =
rtb_B_392_4136_0 - rtb_B_392_183_0 ; _rtB -> B_368_37_0 = ( ( _rtB ->
B_368_11_0 [ 1 ] - _rtDW -> UnitDelay_DSTATE_e ) * rtb_B_392_183_0 * _rtP ->
P_355 + _rtB -> B_368_11_0 [ 1 ] ) * ( rtb_B_392_183_0 / rtb_B_392_4136_0 ) +
( _rtB -> B_368_25_0 - _rtB -> B_368_30_0 ) * rtb_B_392_4670_0 ; } else {
_rtB -> B_368_37_0 = _rtDW -> UnitDelay1_DSTATE_d ; } rtb_B_392_790_0 =
muDoubleScalarHypot ( _rtB -> B_368_24_0 , _rtB -> B_368_37_0 ) ; if (
rtb_B_392_790_0 > _rtP -> P_394 ) { rtb_B_392_790_0 = _rtP -> P_394 ; } else
{ if ( rtb_B_392_790_0 < _rtP -> P_395 ) { rtb_B_392_790_0 = _rtP -> P_395 ;
} } _rtB -> B_368_41_0 = 1.0 / rtb_B_392_790_0 ; B_368_42_0 = _rtP -> P_396 *
muDoubleScalarAtan2 ( _rtB -> B_368_37_0 , _rtB -> B_368_24_0 ) ; } if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC ) ; } } for ( isHit = 0 ; isHit < 3 ; isHit
++ ) { tmp [ isHit ] = _rtP -> P_3300 [ isHit + 6 ] * _rtB -> B_392_4657_0 [
2 ] + ( _rtP -> P_3300 [ isHit + 3 ] * _rtB -> B_392_4657_0 [ 1 ] + _rtP ->
P_3300 [ isHit ] * _rtB -> B_392_4657_0 [ 0 ] ) ; } B_392_4794_0 [ 0 ] = _rtP
-> P_3301 * tmp [ 0 ] ; B_392_4794_0 [ 1 ] = _rtP -> P_3301 * tmp [ 1 ] ;
B_392_4794_0 [ 2 ] = _rtP -> P_3301 * tmp [ 2 ] ; _rtB -> B_392_4798_0 = _rtB
-> B_392_4797_0 ; MR_CIGRE_GENERALIZADO_Subsystem1_b ( _rtB -> B_392_4798_0 ,
& B_392_4794_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystem1_c , & _rtDW
-> Subsystem1_c ) ; _rtB -> B_392_4802_0 = _rtB -> B_392_4801_0 ;
MR_CIGRE_GENERALIZADO_Subsystempi2delay_m ( _rtB -> B_392_4802_0 , &
B_392_4794_0 [ 0 ] , _rtB -> B_392_103_0 , & _rtB -> Subsystempi2delay_p , &
_rtDW -> Subsystempi2delay_p ) ; if ( _rtB -> B_392_4797_0 != 0 ) { _rtB ->
B_392_4804_0 [ 0 ] = _rtB -> Subsystem1_c . B_11_0_0 ; _rtB -> B_392_4804_0 [
1 ] = _rtB -> Subsystem1_c . B_11_1_0 ; } else { _rtB -> B_392_4804_0 [ 0 ] =
_rtB -> Subsystempi2delay_p . B_10_0_0 ; _rtB -> B_392_4804_0 [ 1 ] = _rtB ->
Subsystempi2delay_p . B_10_1_0 ; } if ( _rtDW -> Integ4_SYSTEM_ENABLE_e != 0
) { _rtB -> B_392_4805_0 = _rtDW -> Integ4_DSTATE_k ; } else { _rtB ->
B_392_4805_0 = _rtP -> P_3305 * _rtB -> B_392_4804_0 [ 1 ] + _rtDW ->
Integ4_DSTATE_k ; } if ( rtb_B_392_4670_0 > _rtP -> P_3307 ) {
rtb_B_392_4136_0 = _rtP -> P_3307 ; } else if ( rtb_B_392_4670_0 < _rtP ->
P_3308 ) { rtb_B_392_4136_0 = _rtP -> P_3308 ; } else { rtb_B_392_4136_0 =
rtb_B_392_4670_0 ; } rtb_B_392_4136_0 = 1.0 / rtb_B_392_4136_0 / 5.0e-5 ;
rtb_B_392_183_0 = muDoubleScalarCeil ( rtb_B_392_4136_0 ) ; _rtB ->
B_392_4809_0 = _rtP -> P_3309 * rtb_B_392_183_0 ; ssCallAccelRunBlock ( S ,
392 , 4810 , SS_CALL_MDL_OUTPUTS ) ; if ( ssGetTaskTime ( S , 3 ) >= _rtB ->
B_392_4813_0 ) { rtb_B_392_183_0 = rtb_B_392_4136_0 - rtb_B_392_183_0 ; _rtB
-> B_392_4817_0 = ( ( _rtB -> B_392_4804_0 [ 1 ] - _rtDW ->
UnitDelay_DSTATE_h ) * rtb_B_392_183_0 * _rtP -> P_397 + _rtB -> B_392_4804_0
[ 1 ] ) * ( rtb_B_392_183_0 / rtb_B_392_4136_0 ) + ( _rtB -> B_392_4805_0 -
_rtB -> B_392_4810_0 ) * rtb_B_392_4670_0 ; } else { _rtB -> B_392_4817_0 =
_rtDW -> UnitDelay1_DSTATE_f ; } _rtB -> B_392_4818_0 = _rtB -> B_392_4817_0
* _rtB -> B_368_41_0 ; _rtDW -> DiscreteDerivative_tmp_m = ( _rtB ->
B_392_4818_0 - _rtP -> P_3322 [ 1 ] * _rtDW -> DiscreteDerivative_states_m )
/ _rtP -> P_3322 [ 0 ] ; rtb_B_392_790_0 = ( _rtP -> P_3328 * _rtB ->
B_392_4818_0 + _rtDW -> DiscreteTimeIntegrator_DSTATE_p ) + ( _rtP -> P_3321
[ 0 ] * _rtDW -> DiscreteDerivative_tmp_m + _rtP -> P_3321 [ 1 ] * _rtDW ->
DiscreteDerivative_states_m ) ; if ( rtb_B_392_790_0 > _rtP -> P_3329 ) {
_rtB -> B_392_4823_0 = _rtP -> P_3329 ; } else if ( rtb_B_392_790_0 < _rtP ->
P_3330 ) { _rtB -> B_392_4823_0 = _rtP -> P_3330 ; } else { _rtB ->
B_392_4823_0 = rtb_B_392_790_0 ; } rtb_B_392_4670_0 = _rtP -> P_3331 * _rtB
-> B_392_4823_0 ; rtb_B_392_16_0 = rtb_B_392_4670_0 - _rtDW -> PrevY_c ; if (
rtb_B_392_16_0 > _rtP -> P_3332 ) { rtb_B_392_4670_0 = _rtDW -> PrevY_c +
_rtP -> P_3332 ; } else { if ( rtb_B_392_16_0 < _rtP -> P_3333 ) {
rtb_B_392_4670_0 = _rtDW -> PrevY_c + _rtP -> P_3333 ; } } _rtDW -> PrevY_c =
rtb_B_392_4670_0 ; _rtB -> B_392_4835_0 = ( _rtP -> P_3336 * _rtDW ->
Delay_x1_DSTATE_c + _rtP -> P_3338 * _rtDW -> Delay_x2_DSTATE_ch ) + _rtP ->
P_3341 * rtb_B_392_4670_0 ; _rtB -> B_392_4837_0 = ( _rtP -> P_3339 * _rtDW
-> Delay_x1_DSTATE_c + _rtP -> P_3340 * _rtDW -> Delay_x2_DSTATE_ch ) + _rtP
-> P_3342 * rtb_B_392_4670_0 ; _rtB -> B_392_4842_0 = ( _rtP -> P_3344 *
_rtDW -> Delay_x1_DSTATE_c + _rtP -> P_3345 * _rtDW -> Delay_x2_DSTATE_ch ) +
_rtP -> P_3343 * rtb_B_392_4670_0 ; } _rtB -> B_392_4844_0 = (
muDoubleScalarSin ( _rtP -> P_3348 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3349
) * _rtP -> P_3346 + _rtP -> P_3347 ) * _rtB -> B_392_4657_0 [ 0 ] ; _rtB ->
B_392_4846_0 = ( muDoubleScalarSin ( _rtP -> P_3352 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3353 ) * _rtP -> P_3350 + _rtP -> P_3351 ) * _rtB -> B_392_4657_0
[ 0 ] ; _rtB -> B_392_4848_0 = ( muDoubleScalarSin ( _rtP -> P_3356 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3357 ) * _rtP -> P_3354 + _rtP -> P_3355
) * _rtB -> B_392_4657_0 [ 1 ] ; _rtB -> B_392_4850_0 = ( muDoubleScalarSin (
_rtP -> P_3360 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3361 ) * _rtP -> P_3358
+ _rtP -> P_3359 ) * _rtB -> B_392_4657_0 [ 1 ] ; _rtB -> B_392_4852_0 = (
muDoubleScalarSin ( _rtP -> P_3364 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3365
) * _rtP -> P_3362 + _rtP -> P_3363 ) * _rtB -> B_392_4657_0 [ 2 ] ; _rtB ->
B_392_4854_0 = ( muDoubleScalarSin ( _rtP -> P_3368 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3369 ) * _rtP -> P_3366 + _rtP -> P_3367 ) * _rtB -> B_392_4657_0
[ 2 ] ; B_392_4855_0 = _rtP -> P_3370 * rtb_B_392_237_0 ; _rtB ->
B_392_4857_0 = ( muDoubleScalarSin ( _rtP -> P_3373 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3374 ) * _rtP -> P_3371 + _rtP -> P_3372 ) * _rtB -> B_392_4784_0
[ 0 ] ; _rtB -> B_392_4859_0 = ( muDoubleScalarSin ( _rtP -> P_3377 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3378 ) * _rtP -> P_3375 + _rtP -> P_3376
) * _rtB -> B_392_4784_0 [ 0 ] ; _rtB -> B_392_4861_0 = ( muDoubleScalarSin (
_rtP -> P_3381 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3382 ) * _rtP -> P_3379
+ _rtP -> P_3380 ) * _rtB -> B_392_4784_0 [ 1 ] ; _rtB -> B_392_4863_0 = (
muDoubleScalarSin ( _rtP -> P_3385 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3386
) * _rtP -> P_3383 + _rtP -> P_3384 ) * _rtB -> B_392_4784_0 [ 1 ] ; _rtB ->
B_392_4865_0 = ( muDoubleScalarSin ( _rtP -> P_3389 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3390 ) * _rtP -> P_3387 + _rtP -> P_3388 ) * _rtB -> B_392_4784_0
[ 2 ] ; _rtB -> B_392_4867_0 = ( muDoubleScalarSin ( _rtP -> P_3393 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3394 ) * _rtP -> P_3391 + _rtP -> P_3392
) * _rtB -> B_392_4784_0 [ 2 ] ; B_392_4868_0 = _rtP -> P_3395 *
rtb_B_392_239_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
B_392_4909_0 = _rtP -> P_3397 * _rtB -> B_0_14_0 [ 138 ] * ( _rtP -> P_3398 *
_rtB -> B_0_14_0 [ 83 ] ) ; _rtB -> B_392_4923_0 = _rtP -> P_3399 *
rtb_B_392_222_0 + _rtP -> P_3400 * _rtB -> B_0_15_0 ; _rtB -> B_392_4929_0 =
_rtP -> P_3401 * rtb_B_392_117_0 + _rtP -> P_3402 * _rtB -> B_0_19_0 ; _rtB
-> B_392_4937_0 = _rtP -> P_3403 * _rtB -> B_0_14_0 [ 86 ] ; _rtB ->
B_392_4941_0 = _rtP -> P_3404 * _rtB -> B_0_14_0 [ 140 ] ; _rtB ->
B_392_4945_0 = _rtP -> P_3405 * _rtB -> B_0_14_0 [ 141 ] ; if ( _rtB ->
B_0_14_1 [ 23 ] >= _rtP -> P_3407 ) { rtb_B_392_4670_0 = _rtP -> P_352 * _rtB
-> B_0_14_0 [ 23 ] ; } else { rtb_B_392_4670_0 = _rtB -> B_392_4958_0 ; } if
( rtb_B_392_4670_0 > _rtP -> P_3408 ) { B_392_4961_0 = _rtP -> P_3408 ; }
else if ( rtb_B_392_4670_0 < _rtP -> P_3409 ) { B_392_4961_0 = _rtP -> P_3409
; } else { B_392_4961_0 = rtb_B_392_4670_0 ; } B_392_4962_0 = _rtB ->
B_0_14_0 [ 23 ] + _rtB -> B_392_233_0 ; if ( _rtB -> B_0_14_1 [ 24 ] >= _rtP
-> P_3411 ) { rtb_B_392_4670_0 = _rtP -> P_353 * _rtB -> B_0_14_0 [ 24 ] ; }
else { rtb_B_392_4670_0 = _rtB -> B_392_4966_0 ; } if ( rtb_B_392_4670_0 >
_rtP -> P_3412 ) { B_392_4969_0 = _rtP -> P_3412 ; } else if (
rtb_B_392_4670_0 < _rtP -> P_3413 ) { B_392_4969_0 = _rtP -> P_3413 ; } else
{ B_392_4969_0 = rtb_B_392_4670_0 ; } B_392_4970_0 = _rtB -> B_0_14_0 [ 24 ]
+ _rtB -> B_392_234_0 ; _rtB -> B_392_4984_0 = _rtP -> P_3414 * _rtB ->
B_0_14_0 [ 84 ] ; ssCallAccelRunBlock ( S , 392 , 4985 , SS_CALL_MDL_OUTPUTS
) ; } _rtB -> B_392_4995_0 = _rtX -> integrator_CSTATE_el ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_emg . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_emg
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3416 ; B_392_4996_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_fc . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_fc . Last , _rtDW -> TransportDelay_IWORK_fc .
Tail , _rtDW -> TransportDelay_IWORK_fc . Head , _rtP -> P_3417 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_4998_0 = _rtP -> P_3418 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_4998_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5000_0 = _rtDW -> Memory_PreviousInput_jz ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5002_0 = ( _rtB -> B_392_4995_0 -
B_392_4996_0 ) * _rtP -> P_205 ; } else { _rtB -> B_392_5002_0 = _rtB ->
B_392_5000_0 ; } _rtB -> B_392_5003_0 = _rtX -> integrator_CSTATE_ax ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_db .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_db . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3421 ; B_392_5004_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f0 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_f0 . Last , _rtDW -> TransportDelay_IWORK_f0 .
Tail , _rtDW -> TransportDelay_IWORK_f0 . Head , _rtP -> P_3422 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5006_0 = _rtP -> P_3423 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5006_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5008_0 = _rtDW -> Memory_PreviousInput_cv ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5010_0 = ( _rtB -> B_392_5003_0 -
B_392_5004_0 ) * _rtP -> P_204 ; } else { _rtB -> B_392_5010_0 = _rtB ->
B_392_5008_0 ; } rtb_B_392_4670_0 = _rtP -> P_3425 * muDoubleScalarAtan2 (
_rtB -> B_392_5010_0 , _rtB -> B_392_5002_0 ) * _rtP -> P_3426 ; B_392_5015_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5002_0 , _rtB -> B_392_5010_0 ) *
muDoubleScalarCos ( rtb_B_392_4670_0 ) ; B_392_5015_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5002_0 , _rtB -> B_392_5010_0 ) *
muDoubleScalarSin ( rtb_B_392_4670_0 ) ; _rtB -> B_392_5016_0 = _rtX ->
integrator_CSTATE_pm ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ip . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ip . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3428 ;
B_392_5017_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lt .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lt . Last , _rtDW ->
TransportDelay_IWORK_lt . Tail , _rtDW -> TransportDelay_IWORK_lt . Head ,
_rtP -> P_3429 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5019_0 = _rtP -> P_3430 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5019_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5021_0 =
_rtDW -> Memory_PreviousInput_lva ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5023_0 = ( _rtB -> B_392_5016_0 - B_392_5017_0 ) * _rtP -> P_207 ; }
else { _rtB -> B_392_5023_0 = _rtB -> B_392_5021_0 ; } _rtB -> B_392_5024_0 =
_rtX -> integrator_CSTATE_dy ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_es . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_es . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3433 ;
B_392_5025_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lr .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_lr . Last , _rtDW ->
TransportDelay_IWORK_lr . Tail , _rtDW -> TransportDelay_IWORK_lr . Head ,
_rtP -> P_3434 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5027_0 = _rtP -> P_3435 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5027_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5029_0 =
_rtDW -> Memory_PreviousInput_el ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5031_0 = ( _rtB -> B_392_5024_0 - B_392_5025_0 ) * _rtP -> P_206 ; }
else { _rtB -> B_392_5031_0 = _rtB -> B_392_5029_0 ; } rtb_B_392_4670_0 =
_rtP -> P_3437 * muDoubleScalarAtan2 ( _rtB -> B_392_5031_0 , _rtB ->
B_392_5023_0 ) * _rtP -> P_3438 ; B_392_5036_0 . re = muDoubleScalarHypot (
_rtB -> B_392_5023_0 , _rtB -> B_392_5031_0 ) * muDoubleScalarCos (
rtb_B_392_4670_0 ) ; B_392_5036_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_5023_0 , _rtB -> B_392_5031_0 ) * muDoubleScalarSin ( rtb_B_392_4670_0
) ; _rtB -> B_392_5037_0 = _rtX -> integrator_CSTATE_kk ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3440 ; B_392_5038_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ohq . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ohq . Last , _rtDW -> TransportDelay_IWORK_ohq
. Tail , _rtDW -> TransportDelay_IWORK_ohq . Head , _rtP -> P_3441 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5040_0 = _rtP -> P_3442 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5040_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5042_0 = _rtDW -> Memory_PreviousInput_ds ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5044_0 = ( _rtB -> B_392_5037_0 -
B_392_5038_0 ) * _rtP -> P_209 ; } else { _rtB -> B_392_5044_0 = _rtB ->
B_392_5042_0 ; } _rtB -> B_392_5045_0 = _rtX -> integrator_CSTATE_pg ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kf .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kf . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3445 ; B_392_5046_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_phw . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_phw . Last , _rtDW -> TransportDelay_IWORK_phw
. Tail , _rtDW -> TransportDelay_IWORK_phw . Head , _rtP -> P_3446 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5048_0 = _rtP -> P_3447 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5048_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5050_0 = _rtDW -> Memory_PreviousInput_dgf ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_5052_0 = ( _rtB -> B_392_5045_0 -
B_392_5046_0 ) * _rtP -> P_208 ; } else { _rtB -> B_392_5052_0 = _rtB ->
B_392_5050_0 ; } rtb_B_392_4670_0 = _rtP -> P_3449 * muDoubleScalarAtan2 (
_rtB -> B_392_5052_0 , _rtB -> B_392_5044_0 ) * _rtP -> P_3450 ; B_392_5057_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5044_0 , _rtB -> B_392_5052_0 ) *
muDoubleScalarCos ( rtb_B_392_4670_0 ) ; B_392_5057_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5044_0 , _rtB -> B_392_5052_0 ) *
muDoubleScalarSin ( rtb_B_392_4670_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_5059_0 = _rtB -> B_392_5058_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_5059_0 ,
B_392_5015_0 , B_392_5036_0 , B_392_5057_0 , & _rtB -> PosSeqComputation_o ,
& _rtDW -> PosSeqComputation_o , & _rtP -> PosSeqComputation_o ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5062_0 = _rtB
-> B_392_5061_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_5062_0 , B_392_5015_0 , B_392_5036_0 , B_392_5057_0 , & _rtB ->
NegSeqComputation_m1 , & _rtDW -> NegSeqComputation_m1 , & _rtP ->
NegSeqComputation_m1 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_5065_0 = _rtB -> B_392_5064_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_5065_0 ,
B_392_5015_0 , B_392_5036_0 , B_392_5057_0 , & _rtB -> ZeroSeqComputation_lj
, & _rtDW -> ZeroSeqComputation_lj , & _rtP -> ZeroSeqComputation_lj ) ;
rtb_B_392_4_1 = muDoubleScalarHypot ( _rtB -> PosSeqComputation_o . B_23_2_0
. re , _rtB -> PosSeqComputation_o . B_23_2_0 . im ) ; rtb_B_392_237_0 =
muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_o . B_23_2_0 . im , _rtB ->
PosSeqComputation_o . B_23_2_0 . re ) ; _rtB -> B_392_5068_0 = _rtX ->
integrator_CSTATE_hrp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fj . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_fj . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3455 ;
B_392_5069_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_okh .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_okh . Last , _rtDW ->
TransportDelay_IWORK_okh . Tail , _rtDW -> TransportDelay_IWORK_okh . Head ,
_rtP -> P_3456 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5071_0 = _rtP -> P_3457 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5071_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5073_0 =
_rtDW -> Memory_PreviousInput_peu ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5075_0 = ( _rtB -> B_392_5068_0 - B_392_5069_0 ) * _rtP -> P_211 ; }
else { _rtB -> B_392_5075_0 = _rtB -> B_392_5073_0 ; } _rtB -> B_392_5076_0 =
_rtX -> integrator_CSTATE_bg ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ia . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ia . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3460 ;
B_392_5077_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ne .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ne . Last , _rtDW ->
TransportDelay_IWORK_ne . Tail , _rtDW -> TransportDelay_IWORK_ne . Head ,
_rtP -> P_3461 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5079_0 = _rtP -> P_3462 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5079_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5081_0 =
_rtDW -> Memory_PreviousInput_cu ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5083_0 = ( _rtB -> B_392_5076_0 - B_392_5077_0 ) * _rtP -> P_210 ; }
else { _rtB -> B_392_5083_0 = _rtB -> B_392_5081_0 ; } rtb_B_392_4670_0 =
_rtP -> P_3464 * muDoubleScalarAtan2 ( _rtB -> B_392_5083_0 , _rtB ->
B_392_5075_0 ) * _rtP -> P_3465 ; B_392_5088_0 . re = muDoubleScalarHypot (
_rtB -> B_392_5075_0 , _rtB -> B_392_5083_0 ) * muDoubleScalarCos (
rtb_B_392_4670_0 ) ; B_392_5088_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_5075_0 , _rtB -> B_392_5083_0 ) * muDoubleScalarSin ( rtb_B_392_4670_0
) ; _rtB -> B_392_5089_0 = _rtX -> integrator_CSTATE_dh ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3467 ; B_392_5090_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_dpr . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_dpr . Last , _rtDW -> TransportDelay_IWORK_dpr
. Tail , _rtDW -> TransportDelay_IWORK_dpr . Head , _rtP -> P_3468 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5092_0 = _rtP -> P_3469 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5092_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5094_0 = _rtDW -> Memory_PreviousInput_pn ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5096_0 = ( _rtB -> B_392_5089_0 -
B_392_5090_0 ) * _rtP -> P_213 ; } else { _rtB -> B_392_5096_0 = _rtB ->
B_392_5094_0 ; } _rtB -> B_392_5097_0 = _rtX -> integrator_CSTATE_fof ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_on .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_on . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3472 ; B_392_5098_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_m1 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_m1 . Last , _rtDW -> TransportDelay_IWORK_m1 .
Tail , _rtDW -> TransportDelay_IWORK_m1 . Head , _rtP -> P_3473 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5100_0 = _rtP -> P_3474 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5100_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5102_0 = _rtDW -> Memory_PreviousInput_du ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5104_0 = ( _rtB -> B_392_5097_0 -
B_392_5098_0 ) * _rtP -> P_212 ; } else { _rtB -> B_392_5104_0 = _rtB ->
B_392_5102_0 ; } rtb_B_392_4670_0 = _rtP -> P_3476 * muDoubleScalarAtan2 (
_rtB -> B_392_5104_0 , _rtB -> B_392_5096_0 ) * _rtP -> P_3477 ; B_392_5109_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5096_0 , _rtB -> B_392_5104_0 ) *
muDoubleScalarCos ( rtb_B_392_4670_0 ) ; B_392_5109_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5096_0 , _rtB -> B_392_5104_0 ) *
muDoubleScalarSin ( rtb_B_392_4670_0 ) ; _rtB -> B_392_5110_0 = _rtX ->
integrator_CSTATE_l5v ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dii . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dii . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3479 ;
B_392_5111_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_hlb .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_hlb . Last , _rtDW ->
TransportDelay_IWORK_hlb . Tail , _rtDW -> TransportDelay_IWORK_hlb . Head ,
_rtP -> P_3480 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5113_0 = _rtP -> P_3481 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5113_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5115_0 =
_rtDW -> Memory_PreviousInput_oc ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5117_0 = ( _rtB -> B_392_5110_0 - B_392_5111_0 ) * _rtP -> P_215 ; }
else { _rtB -> B_392_5117_0 = _rtB -> B_392_5115_0 ; } _rtB -> B_392_5118_0 =
_rtX -> integrator_CSTATE_jw ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_pi . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pi . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3484 ;
B_392_5119_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ka .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ka . Last , _rtDW ->
TransportDelay_IWORK_ka . Tail , _rtDW -> TransportDelay_IWORK_ka . Head ,
_rtP -> P_3485 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5121_0 = _rtP -> P_3486 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5121_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5123_0 =
_rtDW -> Memory_PreviousInput_fr ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5125_0 = ( _rtB -> B_392_5118_0 - B_392_5119_0 ) * _rtP -> P_214 ; }
else { _rtB -> B_392_5125_0 = _rtB -> B_392_5123_0 ; } rtb_B_392_4670_0 =
_rtP -> P_3488 * muDoubleScalarAtan2 ( _rtB -> B_392_5125_0 , _rtB ->
B_392_5117_0 ) * _rtP -> P_3489 ; B_392_5130_0 . re = muDoubleScalarHypot (
_rtB -> B_392_5117_0 , _rtB -> B_392_5125_0 ) * muDoubleScalarCos (
rtb_B_392_4670_0 ) ; B_392_5130_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_5117_0 , _rtB -> B_392_5125_0 ) * muDoubleScalarSin ( rtb_B_392_4670_0
) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB ->
B_392_5132_0 = _rtB -> B_392_5131_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_5132_0 ,
B_392_5088_0 , B_392_5109_0 , B_392_5130_0 , & _rtB -> PosSeqComputation_g4 ,
& _rtDW -> PosSeqComputation_g4 , & _rtP -> PosSeqComputation_g4 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5135_0 = _rtB
-> B_392_5134_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_5135_0 , B_392_5088_0 , B_392_5109_0 , B_392_5130_0 , & _rtB ->
NegSeqComputation_fm , & _rtDW -> NegSeqComputation_fm , & _rtP ->
NegSeqComputation_fm ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_5138_0 = _rtB -> B_392_5137_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_5138_0 ,
B_392_5088_0 , B_392_5109_0 , B_392_5130_0 , & _rtB -> ZeroSeqComputation_cn
, & _rtDW -> ZeroSeqComputation_cn , & _rtP -> ZeroSeqComputation_cn ) ;
rtb_B_392_4670_0 = rtb_B_392_4_1 * muDoubleScalarHypot ( _rtB ->
PosSeqComputation_g4 . B_23_2_0 . re , _rtB -> PosSeqComputation_g4 .
B_23_2_0 . im ) * _rtP -> P_3493 ; muDoubleScalarSinCos ( _rtP -> P_3496 * (
_rtP -> P_3494 * rtb_B_392_237_0 - _rtP -> P_3495 * muDoubleScalarAtan2 (
_rtB -> PosSeqComputation_g4 . B_23_2_0 . im , _rtB -> PosSeqComputation_g4 .
B_23_2_0 . re ) ) , & rtb_B_392_183_0 , & rtb_B_392_117_0 ) ; _rtB ->
B_392_5149_0 = _rtX -> integrator_CSTATE_cfo ; { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cco . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cco .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3498 ; B_392_5150_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_lb . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_lb . Last , _rtDW -> TransportDelay_IWORK_lb .
Tail , _rtDW -> TransportDelay_IWORK_lb . Head , _rtP -> P_3499 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5152_0 = _rtP -> P_3500 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5152_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5154_0 = _rtDW -> Memory_PreviousInput_l0 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5156_0 = ( _rtB -> B_392_5149_0 -
B_392_5150_0 ) * _rtP -> P_217 ; } else { _rtB -> B_392_5156_0 = _rtB ->
B_392_5154_0 ; } _rtB -> B_392_5157_0 = _rtX -> integrator_CSTATE_hqe ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bw .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bw . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3503 ; B_392_5158_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_l3 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_l3 . Last , _rtDW -> TransportDelay_IWORK_l3 .
Tail , _rtDW -> TransportDelay_IWORK_l3 . Head , _rtP -> P_3504 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5160_0 = _rtP -> P_3505 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5160_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5162_0 = _rtDW -> Memory_PreviousInput_he ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5164_0 = ( _rtB -> B_392_5157_0 -
B_392_5158_0 ) * _rtP -> P_216 ; } else { _rtB -> B_392_5164_0 = _rtB ->
B_392_5162_0 ; } rtb_B_392_222_0 = _rtP -> P_3507 * muDoubleScalarAtan2 (
_rtB -> B_392_5164_0 , _rtB -> B_392_5156_0 ) * _rtP -> P_3508 ; B_392_5169_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5156_0 , _rtB -> B_392_5164_0 ) *
muDoubleScalarCos ( rtb_B_392_222_0 ) ; B_392_5169_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5156_0 , _rtB -> B_392_5164_0 ) *
muDoubleScalarSin ( rtb_B_392_222_0 ) ; _rtB -> B_392_5170_0 = _rtX ->
integrator_CSTATE_nt ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ab . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ab . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3510 ;
B_392_5171_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_gp .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_gp . Last , _rtDW ->
TransportDelay_IWORK_gp . Tail , _rtDW -> TransportDelay_IWORK_gp . Head ,
_rtP -> P_3511 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5173_0 = _rtP -> P_3512 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5173_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5175_0 =
_rtDW -> Memory_PreviousInput_ph ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5177_0 = ( _rtB -> B_392_5170_0 - B_392_5171_0 ) * _rtP -> P_219 ; }
else { _rtB -> B_392_5177_0 = _rtB -> B_392_5175_0 ; } _rtB -> B_392_5178_0 =
_rtX -> integrator_CSTATE_hqx ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_mwe . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3515
; B_392_5179_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f5 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_f5 . Last , _rtDW ->
TransportDelay_IWORK_f5 . Tail , _rtDW -> TransportDelay_IWORK_f5 . Head ,
_rtP -> P_3516 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5181_0 = _rtP -> P_3517 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5181_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5183_0 =
_rtDW -> Memory_PreviousInput_gy ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5185_0 = ( _rtB -> B_392_5178_0 - B_392_5179_0 ) * _rtP -> P_218 ; }
else { _rtB -> B_392_5185_0 = _rtB -> B_392_5183_0 ; } rtb_B_392_222_0 = _rtP
-> P_3519 * muDoubleScalarAtan2 ( _rtB -> B_392_5185_0 , _rtB -> B_392_5177_0
) * _rtP -> P_3520 ; B_392_5190_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_5177_0 , _rtB -> B_392_5185_0 ) * muDoubleScalarCos ( rtb_B_392_222_0 )
; B_392_5190_0 . im = muDoubleScalarHypot ( _rtB -> B_392_5177_0 , _rtB ->
B_392_5185_0 ) * muDoubleScalarSin ( rtb_B_392_222_0 ) ; _rtB -> B_392_5191_0
= _rtX -> integrator_CSTATE_gq ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3522
; B_392_5192_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay
, 0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ha .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_ha . Last , _rtDW ->
TransportDelay_IWORK_ha . Tail , _rtDW -> TransportDelay_IWORK_ha . Head ,
_rtP -> P_3523 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5194_0 = _rtP -> P_3524 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5194_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5196_0 =
_rtDW -> Memory_PreviousInput_jg ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5198_0 = ( _rtB -> B_392_5191_0 - B_392_5192_0 ) * _rtP -> P_221 ; }
else { _rtB -> B_392_5198_0 = _rtB -> B_392_5196_0 ; } _rtB -> B_392_5199_0 =
_rtX -> integrator_CSTATE_cvp ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_ky . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ky . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3527 ;
B_392_5200_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bm .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bm . Last , _rtDW ->
TransportDelay_IWORK_bm . Tail , _rtDW -> TransportDelay_IWORK_bm . Head ,
_rtP -> P_3528 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5202_0 = _rtP -> P_3529 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5202_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5204_0 =
_rtDW -> Memory_PreviousInput_ei ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5206_0 = ( _rtB -> B_392_5199_0 - B_392_5200_0 ) * _rtP -> P_220 ; }
else { _rtB -> B_392_5206_0 = _rtB -> B_392_5204_0 ; } rtb_B_392_222_0 = _rtP
-> P_3531 * muDoubleScalarAtan2 ( _rtB -> B_392_5206_0 , _rtB -> B_392_5198_0
) * _rtP -> P_3532 ; B_392_5211_0 . re = muDoubleScalarHypot ( _rtB ->
B_392_5198_0 , _rtB -> B_392_5206_0 ) * muDoubleScalarCos ( rtb_B_392_222_0 )
; B_392_5211_0 . im = muDoubleScalarHypot ( _rtB -> B_392_5198_0 , _rtB ->
B_392_5206_0 ) * muDoubleScalarSin ( rtb_B_392_222_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5213_0 = _rtB
-> B_392_5212_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_5213_0 , B_392_5169_0 , B_392_5190_0 , B_392_5211_0 , & _rtB ->
PosSeqComputation_ao , & _rtDW -> PosSeqComputation_ao , & _rtP ->
PosSeqComputation_ao ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_5216_0 = _rtB -> B_392_5215_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_5216_0 ,
B_392_5169_0 , B_392_5190_0 , B_392_5211_0 , & _rtB -> NegSeqComputation_kc ,
& _rtDW -> NegSeqComputation_kc , & _rtP -> NegSeqComputation_kc ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5219_0 = _rtB
-> B_392_5218_0 ; } MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB ->
B_392_5219_0 , B_392_5169_0 , B_392_5190_0 , B_392_5211_0 , & _rtB ->
ZeroSeqComputation_oz , & _rtDW -> ZeroSeqComputation_oz , & _rtP ->
ZeroSeqComputation_oz ) ; rtb_B_392_237_0 = muDoubleScalarAtan2 ( _rtB ->
PosSeqComputation_ao . B_23_2_0 . im , _rtB -> PosSeqComputation_ao .
B_23_2_0 . re ) ; rtb_B_392_222_0 = _rtP -> P_3536 * muDoubleScalarHypot (
_rtB -> PosSeqComputation_ao . B_23_2_0 . re , _rtB -> PosSeqComputation_ao .
B_23_2_0 . im ) ; _rtB -> B_392_5224_0 = _rtX -> integrator_CSTATE_cs ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bro .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_bro . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3538 ; B_392_5225_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_f4 . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_f4 . Last , _rtDW -> TransportDelay_IWORK_f4 .
Tail , _rtDW -> TransportDelay_IWORK_f4 . Head , _rtP -> P_3539 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5227_0 = _rtP -> P_3540 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5227_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5229_0 = _rtDW -> Memory_PreviousInput_phq ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_5231_0 = ( _rtB -> B_392_5224_0 -
B_392_5225_0 ) * _rtP -> P_199 ; } else { _rtB -> B_392_5231_0 = _rtB ->
B_392_5229_0 ; } _rtB -> B_392_5232_0 = _rtX -> integrator_CSTATE_oh ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fn .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_fn . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3543 ; B_392_5233_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_kg . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_kg . Last , _rtDW -> TransportDelay_IWORK_kg .
Tail , _rtDW -> TransportDelay_IWORK_kg . Head , _rtP -> P_3544 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5235_0 = _rtP -> P_3545 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5235_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5237_0 = _rtDW -> Memory_PreviousInput_lkz ; }
if ( rtb_B_392_263_0 ) { _rtB -> B_392_5239_0 = ( _rtB -> B_392_5232_0 -
B_392_5233_0 ) * _rtP -> P_198 ; } else { _rtB -> B_392_5239_0 = _rtB ->
B_392_5237_0 ; } rtb_B_392_4136_0 = _rtP -> P_3547 * muDoubleScalarAtan2 (
_rtB -> B_392_5239_0 , _rtB -> B_392_5231_0 ) * _rtP -> P_3548 ; B_392_5244_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5231_0 , _rtB -> B_392_5239_0 ) *
muDoubleScalarCos ( rtb_B_392_4136_0 ) ; B_392_5244_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5231_0 , _rtB -> B_392_5239_0 ) *
muDoubleScalarSin ( rtb_B_392_4136_0 ) ; _rtB -> B_392_5245_0 = _rtX ->
integrator_CSTATE_mi ; { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dy4 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_dy4 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3550 ;
B_392_5246_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_o4 .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_o4 . Last , _rtDW ->
TransportDelay_IWORK_o4 . Tail , _rtDW -> TransportDelay_IWORK_o4 . Head ,
_rtP -> P_3551 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5248_0 = _rtP -> P_3552 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5248_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5250_0 =
_rtDW -> Memory_PreviousInput_po ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5252_0 = ( _rtB -> B_392_5245_0 - B_392_5246_0 ) * _rtP -> P_201 ; }
else { _rtB -> B_392_5252_0 = _rtB -> B_392_5250_0 ; } _rtB -> B_392_5253_0 =
_rtX -> integrator_CSTATE_m4 ; { real_T * * uBuffer = ( real_T * * ) & _rtDW
-> TransportDelay_PWORK_m5 . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; real_T tMinusDelay = simTime - _rtP -> P_3555 ;
B_392_5254_0 = MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay ,
0.0 , * tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_bck .
CircularBufSize , & _rtDW -> TransportDelay_IWORK_bck . Last , _rtDW ->
TransportDelay_IWORK_bck . Tail , _rtDW -> TransportDelay_IWORK_bck . Head ,
_rtP -> P_3556 , 0 , ( boolean_T ) ( ssIsMinorTimeStep ( S ) && (
ssGetTimeOfLastOutput ( S ) == ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit (
S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5256_0 = _rtP -> P_3557 ; }
rtb_B_392_263_0 = ( ssGetT ( S ) >= _rtB -> B_392_5256_0 ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5258_0 =
_rtDW -> Memory_PreviousInput_o1 ; } if ( rtb_B_392_263_0 ) { _rtB ->
B_392_5260_0 = ( _rtB -> B_392_5253_0 - B_392_5254_0 ) * _rtP -> P_200 ; }
else { _rtB -> B_392_5260_0 = _rtB -> B_392_5258_0 ; } rtb_B_392_4136_0 =
_rtP -> P_3559 * muDoubleScalarAtan2 ( _rtB -> B_392_5260_0 , _rtB ->
B_392_5252_0 ) * _rtP -> P_3560 ; B_392_5265_0 . re = muDoubleScalarHypot (
_rtB -> B_392_5252_0 , _rtB -> B_392_5260_0 ) * muDoubleScalarCos (
rtb_B_392_4136_0 ) ; B_392_5265_0 . im = muDoubleScalarHypot ( _rtB ->
B_392_5252_0 , _rtB -> B_392_5260_0 ) * muDoubleScalarSin ( rtb_B_392_4136_0
) ; _rtB -> B_392_5266_0 = _rtX -> integrator_CSTATE_g2y ; { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; real_T tMinusDelay =
simTime - _rtP -> P_3562 ; B_392_5267_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_prl . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_prl . Last , _rtDW -> TransportDelay_IWORK_prl
. Tail , _rtDW -> TransportDelay_IWORK_prl . Head , _rtP -> P_3563 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5269_0 = _rtP -> P_3564 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5269_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5271_0 = _rtDW -> Memory_PreviousInput_bt ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5273_0 = ( _rtB -> B_392_5266_0 -
B_392_5267_0 ) * _rtP -> P_203 ; } else { _rtB -> B_392_5273_0 = _rtB ->
B_392_5271_0 ; } _rtB -> B_392_5274_0 = _rtX -> integrator_CSTATE_kf0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_acd .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_acd . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; real_T tMinusDelay = simTime - _rtP -> P_3567 ; B_392_5275_0 =
MR_CIGRE_GENERALIZADO_acc_rt_TDelayInterpolate ( tMinusDelay , 0.0 , *
tBuffer , * uBuffer , _rtDW -> TransportDelay_IWORK_ca . CircularBufSize , &
_rtDW -> TransportDelay_IWORK_ca . Last , _rtDW -> TransportDelay_IWORK_ca .
Tail , _rtDW -> TransportDelay_IWORK_ca . Head , _rtP -> P_3568 , 0 , (
boolean_T ) ( ssIsMinorTimeStep ( S ) && ( ssGetTimeOfLastOutput ( S ) ==
ssGetT ( S ) ) ) ) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0
) { _rtB -> B_392_5277_0 = _rtP -> P_3569 ; } rtb_B_392_263_0 = ( ssGetT ( S
) >= _rtB -> B_392_5277_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if (
isHit != 0 ) { _rtB -> B_392_5279_0 = _rtDW -> Memory_PreviousInput_h4 ; } if
( rtb_B_392_263_0 ) { _rtB -> B_392_5281_0 = ( _rtB -> B_392_5274_0 -
B_392_5275_0 ) * _rtP -> P_202 ; } else { _rtB -> B_392_5281_0 = _rtB ->
B_392_5279_0 ; } rtb_B_392_4136_0 = _rtP -> P_3571 * muDoubleScalarAtan2 (
_rtB -> B_392_5281_0 , _rtB -> B_392_5273_0 ) * _rtP -> P_3572 ; B_392_5286_0
. re = muDoubleScalarHypot ( _rtB -> B_392_5273_0 , _rtB -> B_392_5281_0 ) *
muDoubleScalarCos ( rtb_B_392_4136_0 ) ; B_392_5286_0 . im =
muDoubleScalarHypot ( _rtB -> B_392_5273_0 , _rtB -> B_392_5281_0 ) *
muDoubleScalarSin ( rtb_B_392_4136_0 ) ; isHit = ssIsSampleHit ( S , 1 , 0 )
; if ( isHit != 0 ) { _rtB -> B_392_5288_0 = _rtB -> B_392_5287_0 ; }
MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB -> B_392_5288_0 ,
B_392_5244_0 , B_392_5265_0 , B_392_5286_0 , & _rtB -> PosSeqComputation_ow ,
& _rtDW -> PosSeqComputation_ow , & _rtP -> PosSeqComputation_ow ) ; isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtB -> B_392_5291_0 = _rtB
-> B_392_5290_0 ; } MR_CIGRE_GENERALIZADO_NegSeqComputation ( S , _rtB ->
B_392_5291_0 , B_392_5244_0 , B_392_5265_0 , B_392_5286_0 , & _rtB ->
NegSeqComputation_g5 , & _rtDW -> NegSeqComputation_g5 , & _rtP ->
NegSeqComputation_g5 ) ; isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit !=
0 ) { _rtB -> B_392_5294_0 = _rtB -> B_392_5293_0 ; }
MR_CIGRE_GENERALIZADO_ZeroSeqComputation ( S , _rtB -> B_392_5294_0 ,
B_392_5244_0 , B_392_5265_0 , B_392_5286_0 , & _rtB -> ZeroSeqComputation_ko
, & _rtDW -> ZeroSeqComputation_ko , & _rtP -> ZeroSeqComputation_ko ) ;
rtb_B_392_239_0 = muDoubleScalarAtan2 ( _rtB -> PosSeqComputation_ow .
B_23_2_0 . im , _rtB -> PosSeqComputation_ow . B_23_2_0 . re ) ; _rtB ->
B_392_1512_0 [ 0 ] = rtb_B_392_4670_0 * rtb_B_392_117_0 ; _rtB ->
B_392_1512_0 [ 1 ] = rtb_B_392_222_0 ; _rtB -> B_392_1512_0 [ 2 ] =
rtb_B_392_4670_0 * rtb_B_392_183_0 ; _rtB -> B_392_1512_0 [ 3 ] = _rtP ->
P_3576 * muDoubleScalarHypot ( _rtB -> PosSeqComputation_ow . B_23_2_0 . re ,
_rtB -> PosSeqComputation_ow . B_23_2_0 . im ) ; ssCallAccelRunBlock ( S ,
392 , 5299 , SS_CALL_MDL_OUTPUTS ) ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if
( isHit != 0 ) { _rtB -> B_392_5303_0 [ 0 ] = _rtP -> P_3577 * _rtB ->
B_0_14_0 [ 114 ] * _rtP -> P_3580 ; _rtB -> B_392_5303_0 [ 1 ] = _rtP ->
P_3578 * _rtB -> B_0_14_0 [ 115 ] * _rtP -> P_3580 ; _rtB -> B_392_5303_0 [ 2
] = _rtP -> P_3579 * _rtB -> B_0_14_0 [ 116 ] * _rtP -> P_3580 ; _rtB ->
B_392_5307_0 [ 0 ] = _rtP -> P_3581 * _rtB -> B_0_14_0 [ 59 ] * _rtP ->
P_3584 ; _rtB -> B_392_5307_0 [ 1 ] = _rtP -> P_3582 * _rtB -> B_0_14_0 [ 60
] * _rtP -> P_3584 ; _rtB -> B_392_5307_0 [ 2 ] = _rtP -> P_3583 * _rtB ->
B_0_14_0 [ 61 ] * _rtP -> P_3584 ; } _rtB -> B_392_5343_0 = (
muDoubleScalarSin ( _rtP -> P_3587 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3588
) * _rtP -> P_3585 + _rtP -> P_3586 ) * _rtB -> B_392_5303_0 [ 0 ] ; _rtB ->
B_392_5345_0 = ( muDoubleScalarSin ( _rtP -> P_3591 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3592 ) * _rtP -> P_3589 + _rtP -> P_3590 ) * _rtB -> B_392_5303_0
[ 0 ] ; _rtB -> B_392_5347_0 = ( muDoubleScalarSin ( _rtP -> P_3595 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3596 ) * _rtP -> P_3593 + _rtP -> P_3594
) * _rtB -> B_392_5303_0 [ 1 ] ; _rtB -> B_392_5349_0 = ( muDoubleScalarSin (
_rtP -> P_3599 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3600 ) * _rtP -> P_3597
+ _rtP -> P_3598 ) * _rtB -> B_392_5303_0 [ 1 ] ; _rtB -> B_392_5351_0 = (
muDoubleScalarSin ( _rtP -> P_3603 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3604
) * _rtP -> P_3601 + _rtP -> P_3602 ) * _rtB -> B_392_5303_0 [ 2 ] ; _rtB ->
B_392_5353_0 = ( muDoubleScalarSin ( _rtP -> P_3607 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3608 ) * _rtP -> P_3605 + _rtP -> P_3606 ) * _rtB -> B_392_5303_0
[ 2 ] ; B_392_5354_0 = _rtP -> P_3609 * rtb_B_392_239_0 ; _rtB ->
B_392_5356_0 = ( muDoubleScalarSin ( _rtP -> P_3612 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3613 ) * _rtP -> P_3610 + _rtP -> P_3611 ) * _rtB -> B_392_5307_0
[ 0 ] ; _rtB -> B_392_5358_0 = ( muDoubleScalarSin ( _rtP -> P_3616 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3617 ) * _rtP -> P_3614 + _rtP -> P_3615
) * _rtB -> B_392_5307_0 [ 0 ] ; _rtB -> B_392_5360_0 = ( muDoubleScalarSin (
_rtP -> P_3620 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3621 ) * _rtP -> P_3618
+ _rtP -> P_3619 ) * _rtB -> B_392_5307_0 [ 1 ] ; _rtB -> B_392_5362_0 = (
muDoubleScalarSin ( _rtP -> P_3624 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3625
) * _rtP -> P_3622 + _rtP -> P_3623 ) * _rtB -> B_392_5307_0 [ 1 ] ; _rtB ->
B_392_5364_0 = ( muDoubleScalarSin ( _rtP -> P_3628 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3629 ) * _rtP -> P_3626 + _rtP -> P_3627 ) * _rtB -> B_392_5307_0
[ 2 ] ; _rtB -> B_392_5366_0 = ( muDoubleScalarSin ( _rtP -> P_3632 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3633 ) * _rtP -> P_3630 + _rtP -> P_3631
) * _rtB -> B_392_5307_0 [ 2 ] ; _rtB -> B_392_5368_0 = ( muDoubleScalarSin (
_rtP -> P_3636 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3637 ) * _rtP -> P_3634
+ _rtP -> P_3635 ) * _rtB -> B_392_5303_0 [ 0 ] ; _rtB -> B_392_5370_0 = (
muDoubleScalarSin ( _rtP -> P_3640 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3641
) * _rtP -> P_3638 + _rtP -> P_3639 ) * _rtB -> B_392_5303_0 [ 0 ] ; _rtB ->
B_392_5372_0 = ( muDoubleScalarSin ( _rtP -> P_3644 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3645 ) * _rtP -> P_3642 + _rtP -> P_3643 ) * _rtB -> B_392_5303_0
[ 1 ] ; _rtB -> B_392_5374_0 = ( muDoubleScalarSin ( _rtP -> P_3648 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3649 ) * _rtP -> P_3646 + _rtP -> P_3647
) * _rtB -> B_392_5303_0 [ 1 ] ; _rtB -> B_392_5376_0 = ( muDoubleScalarSin (
_rtP -> P_3652 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3653 ) * _rtP -> P_3650
+ _rtP -> P_3651 ) * _rtB -> B_392_5303_0 [ 2 ] ; _rtB -> B_392_5378_0 = (
muDoubleScalarSin ( _rtP -> P_3656 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3657
) * _rtP -> P_3654 + _rtP -> P_3655 ) * _rtB -> B_392_5303_0 [ 2 ] ; _rtB ->
B_392_5380_0 = ( muDoubleScalarSin ( _rtP -> P_3660 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3661 ) * _rtP -> P_3658 + _rtP -> P_3659 ) * _rtB -> B_392_5307_0
[ 0 ] ; _rtB -> B_392_5382_0 = ( muDoubleScalarSin ( _rtP -> P_3664 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3665 ) * _rtP -> P_3662 + _rtP -> P_3663
) * _rtB -> B_392_5307_0 [ 0 ] ; _rtB -> B_392_5384_0 = ( muDoubleScalarSin (
_rtP -> P_3668 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3669 ) * _rtP -> P_3666
+ _rtP -> P_3667 ) * _rtB -> B_392_5307_0 [ 1 ] ; _rtB -> B_392_5386_0 = (
muDoubleScalarSin ( _rtP -> P_3672 * ssGetTaskTime ( S , 0 ) + _rtP -> P_3673
) * _rtP -> P_3670 + _rtP -> P_3671 ) * _rtB -> B_392_5307_0 [ 1 ] ; _rtB ->
B_392_5388_0 = ( muDoubleScalarSin ( _rtP -> P_3676 * ssGetTaskTime ( S , 0 )
+ _rtP -> P_3677 ) * _rtP -> P_3674 + _rtP -> P_3675 ) * _rtB -> B_392_5307_0
[ 2 ] ; _rtB -> B_392_5390_0 = ( muDoubleScalarSin ( _rtP -> P_3680 *
ssGetTaskTime ( S , 0 ) + _rtP -> P_3681 ) * _rtP -> P_3678 + _rtP -> P_3679
) * _rtB -> B_392_5307_0 [ 2 ] ; B_392_5391_0 = _rtP -> P_3682 *
rtb_B_392_237_0 ; isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
if ( _rtB -> B_392_5394_0 >= _rtP -> P_3688 ) { rtb_B_392_5397_0 = _rtB ->
B_392_5393_0 ; } else { rtb_B_392_5397_0 = rt_Lookup ( _rtP -> P_3686 , 6 ,
ssGetTaskTime ( S , 2 ) , _rtP -> P_3687 ) ; } if ( _rtB -> B_392_5398_0 >=
_rtP -> P_3691 ) { _rtB -> B_392_5401_0 = rtb_B_392_5397_0 ; } else { _rtB ->
B_392_5401_0 = _rtB -> B_392_5399_0 ; } } _rtB -> B_392_5403_0 = rt_Lookup (
_rtP -> P_3692 , 5 , ssGetT ( S ) , _rtP -> P_3693 ) ; if ( _rtB ->
B_392_5392_0 >= _rtP -> P_3694 ) { _rtB -> B_392_5404_0 = _rtB ->
B_392_5401_0 ; } else { _rtB -> B_392_5404_0 = _rtB -> B_392_5403_0 ; } if (
( _rtDW -> TimeStampA_lh >= ssGetT ( S ) ) && ( _rtDW -> TimeStampB_ns >=
ssGetT ( S ) ) ) { _rtB -> B_392_5405_0 = 0.0 ; } else { rtb_B_392_430_0 =
_rtDW -> TimeStampA_lh ; lastU = & _rtDW -> LastUAtTimeA_n ; if ( _rtDW ->
TimeStampA_lh < _rtDW -> TimeStampB_ns ) { if ( _rtDW -> TimeStampB_ns <
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_ns ; lastU = & _rtDW
-> LastUAtTimeB_k ; } } else { if ( _rtDW -> TimeStampA_lh >= ssGetT ( S ) )
{ rtb_B_392_430_0 = _rtDW -> TimeStampB_ns ; lastU = & _rtDW ->
LastUAtTimeB_k ; } } _rtB -> B_392_5405_0 = ( _rtB -> B_392_5403_0 - * lastU
) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0
) ; if ( isHit != 0 ) { if ( _rtB -> B_392_5411_0 >= _rtP -> P_3698 ) { _rtB
-> B_392_5413_0 = rtb_B_392_5397_0 ; } else { _rtB -> B_392_5413_0 = _rtB ->
B_392_5399_0 ; } } _rtB -> B_392_5415_0 = rt_Lookup ( _rtP -> P_3699 , 5 ,
ssGetT ( S ) , _rtP -> P_3700 ) ; if ( _rtB -> B_392_5410_0 >= _rtP -> P_3701
) { _rtB -> B_392_5416_0 = _rtB -> B_392_5413_0 ; } else { _rtB ->
B_392_5416_0 = _rtB -> B_392_5415_0 ; } if ( ( _rtDW -> TimeStampA_f >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_o >= ssGetT ( S ) ) ) { _rtB ->
B_392_5417_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_f ; lastU
= & _rtDW -> LastUAtTimeA_i ; if ( _rtDW -> TimeStampA_f < _rtDW ->
TimeStampB_o ) { if ( _rtDW -> TimeStampB_o < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_o ; lastU = & _rtDW -> LastUAtTimeB_ok
; } } else { if ( _rtDW -> TimeStampA_f >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_o ; lastU = & _rtDW -> LastUAtTimeB_ok ; } } _rtB ->
B_392_5417_0 = ( _rtB -> B_392_5415_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_5423_0 >= _rtP -> P_3705 ) { _rtB -> B_392_5425_0 =
rtb_B_392_5397_0 ; } else { _rtB -> B_392_5425_0 = _rtB -> B_392_5399_0 ; } }
_rtB -> B_392_5427_0 = rt_Lookup ( _rtP -> P_3706 , 5 , ssGetT ( S ) , _rtP
-> P_3707 ) ; if ( _rtB -> B_392_5422_0 >= _rtP -> P_3708 ) { _rtB ->
B_392_5428_0 = _rtB -> B_392_5425_0 ; } else { _rtB -> B_392_5428_0 = _rtB ->
B_392_5427_0 ; } if ( ( _rtDW -> TimeStampA_c >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_kd >= ssGetT ( S ) ) ) { _rtB -> B_392_5429_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_c ; lastU = & _rtDW -> LastUAtTimeA_ej
; if ( _rtDW -> TimeStampA_c < _rtDW -> TimeStampB_kd ) { if ( _rtDW ->
TimeStampB_kd < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_kd ;
lastU = & _rtDW -> LastUAtTimeB_j ; } } else { if ( _rtDW -> TimeStampA_c >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_kd ; lastU = & _rtDW
-> LastUAtTimeB_j ; } } _rtB -> B_392_5429_0 = ( _rtB -> B_392_5427_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_5448_0 >= _rtP -> P_3715 ) {
rtb_B_392_5451_0 = _rtB -> B_392_5447_0 ; } else { rtb_B_392_5451_0 =
rt_Lookup ( _rtP -> P_3713 , 4 , ssGetTaskTime ( S , 2 ) , _rtP -> P_3714 ) ;
} if ( _rtB -> B_392_5452_0 >= _rtP -> P_3718 ) { _rtB -> B_392_5455_0 =
rtb_B_392_5451_0 ; } else { _rtB -> B_392_5455_0 = _rtB -> B_392_5453_0 ; } }
_rtB -> B_392_5457_0 = rt_Lookup ( _rtP -> P_3719 , 5 , ssGetT ( S ) , _rtP
-> P_3720 ) ; if ( _rtB -> B_392_5446_0 >= _rtP -> P_3721 ) { _rtB ->
B_392_5458_0 = _rtB -> B_392_5455_0 ; } else { _rtB -> B_392_5458_0 = _rtB ->
B_392_5457_0 ; } if ( ( _rtDW -> TimeStampA_d >= ssGetT ( S ) ) && ( _rtDW ->
TimeStampB_p0 >= ssGetT ( S ) ) ) { _rtB -> B_392_5459_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_d ; lastU = & _rtDW -> LastUAtTimeA_cs
; if ( _rtDW -> TimeStampA_d < _rtDW -> TimeStampB_p0 ) { if ( _rtDW ->
TimeStampB_p0 < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_p0 ;
lastU = & _rtDW -> LastUAtTimeB_ec ; } } else { if ( _rtDW -> TimeStampA_d >=
ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_p0 ; lastU = & _rtDW
-> LastUAtTimeB_ec ; } } _rtB -> B_392_5459_0 = ( _rtB -> B_392_5457_0 - *
lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S ,
2 , 0 ) ; if ( isHit != 0 ) { if ( _rtB -> B_392_5465_0 >= _rtP -> P_3725 ) {
_rtB -> B_392_5467_0 = rtb_B_392_5451_0 ; } else { _rtB -> B_392_5467_0 =
_rtB -> B_392_5453_0 ; } } _rtB -> B_392_5469_0 = rt_Lookup ( _rtP -> P_3726
, 5 , ssGetT ( S ) , _rtP -> P_3727 ) ; if ( _rtB -> B_392_5464_0 >= _rtP ->
P_3728 ) { _rtB -> B_392_5470_0 = _rtB -> B_392_5467_0 ; } else { _rtB ->
B_392_5470_0 = _rtB -> B_392_5469_0 ; } if ( ( _rtDW -> TimeStampA_hs >=
ssGetT ( S ) ) && ( _rtDW -> TimeStampB_h >= ssGetT ( S ) ) ) { _rtB ->
B_392_5471_0 = 0.0 ; } else { rtb_B_392_430_0 = _rtDW -> TimeStampA_hs ;
lastU = & _rtDW -> LastUAtTimeA_cq ; if ( _rtDW -> TimeStampA_hs < _rtDW ->
TimeStampB_h ) { if ( _rtDW -> TimeStampB_h < ssGetT ( S ) ) {
rtb_B_392_430_0 = _rtDW -> TimeStampB_h ; lastU = & _rtDW -> LastUAtTimeB_f ;
} } else { if ( _rtDW -> TimeStampA_hs >= ssGetT ( S ) ) { rtb_B_392_430_0 =
_rtDW -> TimeStampB_h ; lastU = & _rtDW -> LastUAtTimeB_f ; } } _rtB ->
B_392_5471_0 = ( _rtB -> B_392_5469_0 - * lastU ) / ( ssGetT ( S ) -
rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 )
{ if ( _rtB -> B_392_5477_0 >= _rtP -> P_3732 ) { _rtB -> B_392_5479_0 =
rtb_B_392_5451_0 ; } else { _rtB -> B_392_5479_0 = _rtB -> B_392_5453_0 ; } }
_rtB -> B_392_5481_0 = rt_Lookup ( _rtP -> P_3733 , 5 , ssGetT ( S ) , _rtP
-> P_3734 ) ; if ( _rtB -> B_392_5476_0 >= _rtP -> P_3735 ) { _rtB ->
B_392_5482_0 = _rtB -> B_392_5479_0 ; } else { _rtB -> B_392_5482_0 = _rtB ->
B_392_5481_0 ; } if ( ( _rtDW -> TimeStampA_if >= ssGetT ( S ) ) && ( _rtDW
-> TimeStampB_cy >= ssGetT ( S ) ) ) { _rtB -> B_392_5483_0 = 0.0 ; } else {
rtb_B_392_430_0 = _rtDW -> TimeStampA_if ; lastU = & _rtDW -> LastUAtTimeA_a
; if ( _rtDW -> TimeStampA_if < _rtDW -> TimeStampB_cy ) { if ( _rtDW ->
TimeStampB_cy < ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_cy ;
lastU = & _rtDW -> LastUAtTimeB_hd ; } } else { if ( _rtDW -> TimeStampA_if
>= ssGetT ( S ) ) { rtb_B_392_430_0 = _rtDW -> TimeStampB_cy ; lastU = &
_rtDW -> LastUAtTimeB_hd ; } } _rtB -> B_392_5483_0 = ( _rtB -> B_392_5481_0
- * lastU ) / ( ssGetT ( S ) - rtb_B_392_430_0 ) ; } isHit = ssIsSampleHit (
S , 2 , 0 ) ; if ( isHit != 0 ) { rtb_B_392_5397_0 = _rtP -> P_3737 * _rtB ->
B_0_14_0 [ 29 ] ; rtb_B_392_5451_0 = _rtP -> P_3738 * _rtB -> B_0_14_0 [ 30 ]
; rtb_B_392_4670_0 = _rtP -> P_3739 * _rtB -> B_0_14_0 [ 31 ] ; B_392_5498_0
[ 0 ] = _rtP -> P_3740 * rtb_B_392_5397_0 ; B_392_5498_0 [ 1 ] = _rtP ->
P_3740 * rtb_B_392_5451_0 ; B_392_5498_0 [ 2 ] = _rtP -> P_3740 *
rtb_B_392_4670_0 ; B_392_5499_0 [ 0 ] = _rtP -> P_3741 * rtb_B_392_5397_0 ;
B_392_5499_0 [ 1 ] = _rtP -> P_3741 * rtb_B_392_5451_0 ; B_392_5499_0 [ 2 ] =
_rtP -> P_3741 * rtb_B_392_4670_0 ; } UNUSED_PARAMETER ( tid ) ; } static
void mdlOutputsTID4 ( SimStruct * S , int_T tid ) { real_T rtb_B_392_156_0 ;
real_T rtb_B_392_178_0 ; B_MR_CIGRE_GENERALIZADO_T * _rtB ;
P_MR_CIGRE_GENERALIZADO_T * _rtP ; DW_MR_CIGRE_GENERALIZADO_T * _rtDW ; _rtDW
= ( ( DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ; memcpy ( & _rtB ->
B_392_0_0 [ 0 ] , & _rtP -> P_410 [ 0 ] , 25U * sizeof ( real_T ) ) ; _rtB ->
B_392_13_0 = _rtP -> P_420 ; _rtB -> B_392_14_0 = _rtP -> P_421 ; _rtB ->
B_392_22_0 = ( uint8_T ) ( _rtP -> P_431 == _rtP -> P_432 ) ; _rtB ->
B_392_26_0 = ( uint8_T ) ( _rtP -> P_431 == _rtP -> P_433 ) ; _rtB ->
B_392_30_0 = _rtP -> P_434 ; _rtB -> B_392_51_0 = _rtP -> P_448 ; _rtB ->
B_392_57_0 = ( uint8_T ) ( _rtP -> P_454 == _rtP -> P_455 ) ; _rtB ->
B_392_61_0 = ( uint8_T ) ( _rtP -> P_454 == _rtP -> P_456 ) ; _rtB ->
B_392_65_0 = _rtP -> P_457 ; _rtB -> B_392_82_0 = ( uint8_T ) ( _rtP -> P_466
== _rtP -> P_467 ) ; _rtB -> B_392_86_0 = ( uint8_T ) ( _rtP -> P_466 == _rtP
-> P_468 ) ; _rtB -> B_392_106_0 = ( uint8_T ) ( _rtP -> P_477 == _rtP ->
P_478 ) ; _rtB -> B_392_110_0 = ( uint8_T ) ( _rtP -> P_477 == _rtP -> P_479
) ; _rtB -> B_392_122_0 = _rtP -> P_484 ; _rtB -> B_392_123_0 = _rtP -> P_485
; _rtB -> B_392_124_0 = _rtP -> P_486 ; _rtB -> B_392_127_0 = _rtP -> P_489 ;
_rtB -> B_392_131_0 = ( 0.0 <= _rtP -> P_490 ) ; _rtB -> B_392_139_0 = _rtP
-> P_495 ; _rtB -> B_392_142_0 = _rtP -> P_498 ; _rtB -> B_392_146_0 = ( 0.0
<= _rtP -> P_499 ) ; rtb_B_392_156_0 = _rtP -> P_504 + _rtP -> P_505 ; _rtB
-> B_392_164_0 = _rtP -> P_511 * _rtP -> P_510 ; _rtB -> B_392_165_0 = ( (
rtb_B_392_156_0 - _rtP -> P_506 ) * _rtP -> P_507 + _rtP -> P_508 ) * _rtP ->
P_509 * _rtB -> B_392_164_0 ; rtb_B_392_178_0 = rtb_B_392_156_0 / _rtP ->
P_518 ; _rtB -> B_392_181_0 = muDoubleScalarExp ( _rtP -> P_512 - ( (
rtb_B_392_156_0 - _rtP -> P_514 ) * _rtP -> P_515 + _rtP -> P_513 ) * _rtP ->
P_516 / ( _rtP -> P_517 * rtb_B_392_156_0 ) ) * ( _rtP -> P_519 *
muDoubleScalarPower ( rtb_B_392_178_0 , 3.0 ) ) ; _rtB -> B_392_182_0 = _rtP
-> P_520 * rtb_B_392_178_0 ; _rtB -> B_392_187_0 = _rtP -> P_525 ; _rtB ->
B_392_188_0 = _rtP -> P_526 ; _rtB -> B_392_189_0 = _rtP -> P_527 ; _rtB ->
B_392_190_0 = _rtP -> P_528 ; rtb_B_392_156_0 = _rtP -> P_529 + _rtP -> P_530
; _rtB -> B_392_201_0 = _rtP -> P_536 * _rtP -> P_535 ; _rtB -> B_392_202_0 =
( ( rtb_B_392_156_0 - _rtP -> P_531 ) * _rtP -> P_532 + _rtP -> P_533 ) *
_rtP -> P_534 * _rtB -> B_392_201_0 ; rtb_B_392_178_0 = rtb_B_392_156_0 /
_rtP -> P_543 ; _rtB -> B_392_218_0 = muDoubleScalarExp ( _rtP -> P_537 - ( (
rtb_B_392_156_0 - _rtP -> P_539 ) * _rtP -> P_540 + _rtP -> P_538 ) * _rtP ->
P_541 / ( _rtP -> P_542 * rtb_B_392_156_0 ) ) * ( _rtP -> P_544 *
muDoubleScalarPower ( rtb_B_392_178_0 , 3.0 ) ) ; _rtB -> B_392_219_0 = _rtP
-> P_545 * rtb_B_392_178_0 ; _rtB -> B_392_224_0 = _rtP -> P_550 ; _rtB ->
B_392_225_0 = _rtP -> P_551 ; _rtB -> B_392_226_0 = _rtP -> P_552 ; _rtB ->
B_392_227_0 = _rtP -> P_553 ; _rtB -> B_392_231_0 = _rtP -> P_575 ; _rtB ->
B_392_232_0 = _rtP -> P_576 ; _rtB -> B_392_233_0 = _rtP -> P_577 ; _rtB ->
B_392_234_0 = _rtP -> P_578 ; _rtB -> B_392_238_0 = _rtP -> P_582 ; _rtB ->
B_392_242_0 = _rtP -> P_588 ; _rtB -> B_392_252_0 = _rtP -> P_597 ; _rtB ->
B_392_322_0 = _rtP -> P_638 ; _rtB -> B_392_325_0 = _rtP -> P_639 ; _rtB ->
B_392_328_0 = _rtP -> P_640 ; _rtB -> B_392_396_0 = _rtP -> P_678 ; _rtB ->
B_392_399_0 = _rtP -> P_679 ; _rtB -> B_392_402_0 = _rtP -> P_680 ; _rtB ->
B_392_407_0 = _rtP -> P_682 ; _rtB -> B_392_411_0 = _rtP -> P_686 ; _rtB ->
B_392_415_0 = _rtP -> P_692 ; _rtB -> B_392_425_0 = _rtP -> P_701 ; _rtB ->
B_392_495_0 = _rtP -> P_742 ; _rtB -> B_392_498_0 = _rtP -> P_743 ; _rtB ->
B_392_501_0 = _rtP -> P_744 ; _rtB -> B_392_569_0 = _rtP -> P_782 ; _rtB ->
B_392_572_0 = _rtP -> P_783 ; _rtB -> B_392_575_0 = _rtP -> P_784 ; _rtB ->
B_392_580_0 = _rtP -> P_786 ; _rtB -> B_392_595_0 = _rtP -> P_791 ; _rtB ->
B_392_604_0 = ( uint8_T ) ( _rtP -> P_798 == _rtP -> P_799 ) ; _rtB ->
B_392_608_0 = ( uint8_T ) ( _rtP -> P_798 == _rtP -> P_800 ) ; _rtB ->
B_392_621_0 = ( uint8_T ) ( _rtP -> P_809 == _rtP -> P_810 ) ; _rtB ->
B_392_625_0 = ( uint8_T ) ( _rtP -> P_809 == _rtP -> P_811 ) ; _rtB ->
B_392_669_0 = ( uint8_T ) ( _rtP -> P_836 == _rtP -> P_837 ) ; _rtB ->
B_392_673_0 = ( uint8_T ) ( _rtP -> P_836 == _rtP -> P_838 ) ; _rtB ->
B_392_688_0 = _rtP -> P_848 ; _rtB -> B_392_689_0 = _rtP -> P_849 ; _rtB ->
B_392_700_0 = _rtP -> P_854 ; _rtB -> B_392_787_0 = _rtP -> P_878 ; _rtB ->
B_12_4_0 = ( uint8_T ) ( _rtP -> P_23 == _rtP -> P_24 ) ; _rtB -> B_12_8_0 =
( uint8_T ) ( _rtP -> P_23 == _rtP -> P_25 ) ; _rtB -> B_12_20_0 = _rtP ->
P_40 ; _rtB -> B_12_33_0 = _rtP -> P_56 ; if ( ssIsMajorTimeStep ( S ) != 0 )
{ srUpdateBC ( _rtDW -> AutomaticGainControl_SubsysRanBC_c ) ; } _rtB ->
B_392_826_0 = ( uint8_T ) ( _rtP -> P_886 == _rtP -> P_887 ) ; _rtB ->
B_392_830_0 = ( uint8_T ) ( _rtP -> P_886 == _rtP -> P_888 ) ; _rtB ->
B_392_842_0 = _rtP -> P_903 ; _rtB -> B_392_872_0 = _rtP -> P_930 ; _rtB ->
B_392_946_0 = _rtP -> P_985 ; _rtB -> B_392_949_0 = _rtP -> P_986 ; _rtB ->
B_392_958_0 = ( uint8_T ) ( _rtP -> P_993 == _rtP -> P_994 ) ; _rtB ->
B_392_962_0 = ( uint8_T ) ( _rtP -> P_993 == _rtP -> P_995 ) ; _rtB ->
B_392_975_0 = ( uint8_T ) ( _rtP -> P_1004 == _rtP -> P_1005 ) ; _rtB ->
B_392_979_0 = ( uint8_T ) ( _rtP -> P_1004 == _rtP -> P_1006 ) ; _rtB ->
B_392_1027_0 = ( uint8_T ) ( _rtP -> P_1033 == _rtP -> P_1034 ) ; _rtB ->
B_392_1031_0 = ( uint8_T ) ( _rtP -> P_1033 == _rtP -> P_1035 ) ; _rtB ->
B_392_1046_0 = _rtP -> P_1045 ; _rtB -> B_392_1047_0 = _rtP -> P_1046 ; _rtB
-> B_392_1054_0 = _rtP -> P_1049 ; _rtB -> B_392_1125_0 = _rtP -> P_1071 ;
_rtB -> B_48_4_0 = ( uint8_T ) ( _rtP -> P_79 == _rtP -> P_80 ) ; _rtB ->
B_48_8_0 = ( uint8_T ) ( _rtP -> P_79 == _rtP -> P_81 ) ; _rtB -> B_48_20_0 =
_rtP -> P_96 ; _rtB -> B_48_33_0 = _rtP -> P_112 ; if ( ssIsMajorTimeStep ( S
) != 0 ) { srUpdateBC ( _rtDW -> AutomaticGainControl_SubsysRanBC_l ) ; }
_rtB -> B_392_1163_0 = ( uint8_T ) ( _rtP -> P_1079 == _rtP -> P_1080 ) ;
_rtB -> B_392_1167_0 = ( uint8_T ) ( _rtP -> P_1079 == _rtP -> P_1081 ) ;
_rtB -> B_392_1179_0 = _rtP -> P_1096 ; _rtB -> B_392_1345_0 = _rtP -> P_1213
; _rtB -> B_392_1348_0 = _rtP -> P_1214 ; _rtB -> B_392_1351_0 = _rtP ->
P_1215 ; _rtB -> B_392_1418_0 = _rtP -> P_1252 ; _rtB -> B_392_1421_0 = _rtP
-> P_1253 ; _rtB -> B_392_1424_0 = _rtP -> P_1254 ; _rtB -> B_392_1502_0 =
_rtP -> P_1297 ; _rtB -> B_392_1505_0 = _rtP -> P_1298 ; _rtB -> B_392_1508_0
= _rtP -> P_1299 ; _rtB -> B_392_1593_0 = _rtP -> P_1381 ; _rtB ->
B_392_1594_0 = _rtP -> P_1382 ; _rtB -> B_392_1597_0 = _rtP -> P_1387 ; _rtB
-> B_392_1601_0 = _rtP -> P_1391 ; _rtB -> B_392_1602_0 = _rtP -> P_1392 ;
_rtB -> B_392_1613_0 = _rtP -> P_1398 ; _rtB -> B_392_1614_0 = _rtP -> P_1399
; _rtB -> B_392_1625_0 = _rtP -> P_1405 ; _rtB -> B_392_1626_0 = _rtP ->
P_1406 ; ssCallAccelRunBlock ( S , 392 , 1644 , SS_CALL_MDL_OUTPUTS ) ; _rtB
-> B_392_1655_0 = _rtP -> P_1415 ; _rtB -> B_392_1658_0 = _rtP -> P_1416 ;
_rtB -> B_392_1782_0 = _rtP -> P_1463 ; _rtB -> B_392_1785_0 = _rtP -> P_1464
; _rtB -> B_392_1788_0 = _rtP -> P_1465 ; _rtB -> B_392_1855_0 = _rtP ->
P_1502 ; _rtB -> B_392_1858_0 = _rtP -> P_1503 ; _rtB -> B_392_1861_0 = _rtP
-> P_1504 ; _rtB -> B_392_1936_0 = _rtP -> P_1545 ; _rtB -> B_392_1939_0 =
_rtP -> P_1546 ; _rtB -> B_392_1942_0 = _rtP -> P_1547 ; _rtB -> B_392_2011_0
= _rtP -> P_1585 ; _rtB -> B_392_2014_0 = _rtP -> P_1586 ; _rtB ->
B_392_2017_0 = _rtP -> P_1587 ; _rtB -> B_392_2117_0 = _rtP -> P_1695 ; _rtB
-> B_392_2118_0 = _rtP -> P_1696 ; _rtB -> B_392_2119_0 = _rtP -> P_1697 ;
_rtB -> B_392_2123_0 = _rtP -> P_1701 ; _rtB -> B_392_2124_0 = _rtP -> P_1702
; _rtB -> B_392_2135_0 = _rtP -> P_1708 ; _rtB -> B_392_2136_0 = _rtP ->
P_1709 ; _rtB -> B_392_2147_0 = _rtP -> P_1715 ; _rtB -> B_392_2148_0 = _rtP
-> P_1716 ; _rtB -> B_392_2234_0 = _rtP -> P_1758 ; _rtB -> B_392_2237_0 =
_rtP -> P_1759 ; _rtB -> B_392_2240_0 = _rtP -> P_1760 ; _rtB -> B_392_2307_0
= _rtP -> P_1797 ; _rtB -> B_392_2310_0 = _rtP -> P_1798 ; _rtB ->
B_392_2313_0 = _rtP -> P_1799 ; _rtB -> B_392_2388_0 = _rtP -> P_1840 ; _rtB
-> B_392_2391_0 = _rtP -> P_1841 ; _rtB -> B_392_2394_0 = _rtP -> P_1842 ;
_rtB -> B_392_2463_0 = _rtP -> P_1880 ; _rtB -> B_392_2466_0 = _rtP -> P_1881
; _rtB -> B_392_2469_0 = _rtP -> P_1882 ; _rtB -> B_392_2634_0 = _rtP ->
P_2026 ; _rtB -> B_392_2637_0 = _rtP -> P_2027 ; _rtB -> B_392_2640_0 = _rtP
-> P_2028 ; _rtB -> B_392_2707_0 = _rtP -> P_2065 ; _rtB -> B_392_2710_0 =
_rtP -> P_2066 ; _rtB -> B_392_2713_0 = _rtP -> P_2067 ; _rtB -> B_392_2788_0
= _rtP -> P_2108 ; _rtB -> B_392_2791_0 = _rtP -> P_2109 ; _rtB ->
B_392_2794_0 = _rtP -> P_2110 ; _rtB -> B_392_2863_0 = _rtP -> P_2148 ; _rtB
-> B_392_2866_0 = _rtP -> P_2149 ; _rtB -> B_392_2869_0 = _rtP -> P_2150 ;
_rtB -> B_392_2969_0 = _rtP -> P_2258 ; _rtB -> B_392_2970_0 = _rtP -> P_2259
; _rtB -> B_392_2971_0 = _rtP -> P_2260 ; _rtB -> B_392_2975_0 = _rtP ->
P_2264 ; _rtB -> B_392_2976_0 = _rtP -> P_2265 ; _rtB -> B_392_2987_0 = _rtP
-> P_2271 ; _rtB -> B_392_2988_0 = _rtP -> P_2272 ; _rtB -> B_392_2999_0 =
_rtP -> P_2278 ; _rtB -> B_392_3000_0 = _rtP -> P_2279 ; _rtB -> B_392_3086_0
= _rtP -> P_2321 ; _rtB -> B_392_3089_0 = _rtP -> P_2322 ; _rtB ->
B_392_3092_0 = _rtP -> P_2323 ; _rtB -> B_392_3159_0 = _rtP -> P_2360 ; _rtB
-> B_392_3162_0 = _rtP -> P_2361 ; _rtB -> B_392_3165_0 = _rtP -> P_2362 ;
_rtB -> B_392_3240_0 = _rtP -> P_2403 ; _rtB -> B_392_3243_0 = _rtP -> P_2404
; _rtB -> B_392_3246_0 = _rtP -> P_2405 ; _rtB -> B_392_3315_0 = _rtP ->
P_2443 ; _rtB -> B_392_3318_0 = _rtP -> P_2444 ; _rtB -> B_392_3321_0 = _rtP
-> P_2445 ; _rtB -> B_392_3421_0 = _rtP -> P_2553 ; _rtB -> B_392_3422_0 =
_rtP -> P_2554 ; _rtB -> B_392_3423_0 = _rtP -> P_2555 ; _rtB -> B_392_3427_0
= _rtP -> P_2559 ; _rtB -> B_392_3428_0 = _rtP -> P_2560 ; _rtB ->
B_392_3439_0 = _rtP -> P_2566 ; _rtB -> B_392_3440_0 = _rtP -> P_2567 ; _rtB
-> B_392_3451_0 = _rtP -> P_2573 ; _rtB -> B_392_3452_0 = _rtP -> P_2574 ;
_rtB -> B_392_3538_0 = _rtP -> P_2616 ; _rtB -> B_392_3541_0 = _rtP -> P_2617
; _rtB -> B_392_3544_0 = _rtP -> P_2618 ; _rtB -> B_392_3611_0 = _rtP ->
P_2655 ; _rtB -> B_392_3614_0 = _rtP -> P_2656 ; _rtB -> B_392_3617_0 = _rtP
-> P_2657 ; _rtB -> B_392_3692_0 = _rtP -> P_2698 ; _rtB -> B_392_3695_0 =
_rtP -> P_2699 ; _rtB -> B_392_3698_0 = _rtP -> P_2700 ; _rtB -> B_392_3767_0
= _rtP -> P_2738 ; _rtB -> B_392_3770_0 = _rtP -> P_2739 ; _rtB ->
B_392_3773_0 = _rtP -> P_2740 ; _rtB -> B_392_3873_0 = _rtP -> P_2848 ; _rtB
-> B_392_3874_0 = _rtP -> P_2849 ; _rtB -> B_392_3875_0 = _rtP -> P_2850 ;
_rtB -> B_392_3879_0 = _rtP -> P_2854 ; _rtB -> B_392_3880_0 = _rtP -> P_2855
; _rtB -> B_392_3891_0 = _rtP -> P_2861 ; _rtB -> B_392_3892_0 = _rtP ->
P_2862 ; _rtB -> B_392_3903_0 = _rtP -> P_2868 ; _rtB -> B_392_3904_0 = _rtP
-> P_2869 ; _rtB -> B_392_3992_0 = _rtP -> P_2915 ; _rtB -> B_392_3995_0 =
_rtP -> P_2916 ; _rtB -> B_392_3998_0 = _rtP -> P_2917 ; _rtB -> B_392_4066_0
= _rtP -> P_2955 ; _rtB -> B_392_4069_0 = _rtP -> P_2956 ; _rtB ->
B_392_4072_0 = _rtP -> P_2957 ; _rtB -> B_392_4077_0 = _rtP -> P_2959 ; _rtB
-> B_392_4082_0 = _rtP -> P_2961 ; _rtB -> B_392_4091_0 = ( uint8_T ) ( _rtP
-> P_2968 == _rtP -> P_2969 ) ; _rtB -> B_392_4095_0 = ( uint8_T ) ( _rtP ->
P_2968 == _rtP -> P_2970 ) ; _rtB -> B_392_4100_0 = _rtP -> P_2973 ; _rtB ->
B_392_4105_0 = _rtP -> P_2976 ; _rtB -> B_392_4128_0 = ( uint8_T ) ( _rtP ->
P_2995 == _rtP -> P_2996 ) ; _rtB -> B_392_4132_0 = ( uint8_T ) ( _rtP ->
P_2995 == _rtP -> P_2997 ) ; _rtB -> B_392_4209_0 = _rtP -> P_3015 ; _rtB ->
B_392_4242_0 = ( uint8_T ) ( _rtP -> P_3020 == _rtP -> P_3021 ) ; _rtB ->
B_392_4244_0 = ( uint8_T ) ( _rtP -> P_3020 == _rtP -> P_3022 ) ; _rtB ->
B_331_4_0 = ( uint8_T ) ( _rtP -> P_301 == _rtP -> P_302 ) ; _rtB ->
B_331_8_0 = ( uint8_T ) ( _rtP -> P_301 == _rtP -> P_303 ) ; _rtB ->
B_331_20_0 = _rtP -> P_318 ; _rtB -> B_331_33_0 = _rtP -> P_334 ; if (
ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC_n ) ; } _rtB -> B_392_4263_0 = ( uint8_T ) (
_rtP -> P_3032 == _rtP -> P_3033 ) ; _rtB -> B_392_4267_0 = ( uint8_T ) (
_rtP -> P_3032 == _rtP -> P_3034 ) ; _rtB -> B_392_4279_0 = _rtP -> P_3049 ;
_rtB -> B_392_4369_0 = _rtP -> P_3126 ; _rtB -> B_392_4424_0 = _rtP -> P_3136
; _rtB -> B_392_4432_0 = _rtP -> P_3140 ; _rtB -> B_392_4526_0 = _rtP ->
P_3185 ; _rtB -> B_392_4529_0 = _rtP -> P_3186 ; _rtB -> B_392_4532_0 = _rtP
-> P_3187 ; _rtB -> B_392_4600_0 = _rtP -> P_3225 ; _rtB -> B_392_4603_0 =
_rtP -> P_3226 ; _rtB -> B_392_4606_0 = _rtP -> P_3227 ; _rtB -> B_392_4611_0
= _rtP -> P_3229 ; _rtB -> B_392_4616_0 = _rtP -> P_3231 ; _rtB ->
B_392_4625_0 = ( uint8_T ) ( _rtP -> P_3238 == _rtP -> P_3239 ) ; _rtB ->
B_392_4629_0 = ( uint8_T ) ( _rtP -> P_3238 == _rtP -> P_3240 ) ; _rtB ->
B_392_4634_0 = _rtP -> P_3243 ; _rtB -> B_392_4639_0 = _rtP -> P_3246 ; _rtB
-> B_392_4662_0 = ( uint8_T ) ( _rtP -> P_3265 == _rtP -> P_3266 ) ; _rtB ->
B_392_4666_0 = ( uint8_T ) ( _rtP -> P_3265 == _rtP -> P_3267 ) ; _rtB ->
B_392_4743_0 = _rtP -> P_3285 ; _rtB -> B_392_4776_0 = ( uint8_T ) ( _rtP ->
P_3290 == _rtP -> P_3291 ) ; _rtB -> B_392_4778_0 = ( uint8_T ) ( _rtP ->
P_3290 == _rtP -> P_3292 ) ; _rtB -> B_368_4_0 = ( uint8_T ) ( _rtP -> P_359
== _rtP -> P_360 ) ; _rtB -> B_368_8_0 = ( uint8_T ) ( _rtP -> P_359 == _rtP
-> P_361 ) ; _rtB -> B_368_20_0 = _rtP -> P_376 ; _rtB -> B_368_33_0 = _rtP
-> P_392 ; if ( ssIsMajorTimeStep ( S ) != 0 ) { srUpdateBC ( _rtDW ->
AutomaticGainControl_SubsysRanBC ) ; } _rtB -> B_392_4797_0 = ( uint8_T ) (
_rtP -> P_3302 == _rtP -> P_3303 ) ; _rtB -> B_392_4801_0 = ( uint8_T ) (
_rtP -> P_3302 == _rtP -> P_3304 ) ; _rtB -> B_392_4813_0 = _rtP -> P_3319 ;
_rtB -> B_392_4903_0 = _rtP -> P_3396 ; _rtB -> B_392_4958_0 = _rtP -> P_3406
; _rtB -> B_392_4966_0 = _rtP -> P_3410 ; _rtB -> B_392_5058_0 = _rtP ->
P_3451 ; _rtB -> B_392_5061_0 = _rtP -> P_3452 ; _rtB -> B_392_5064_0 = _rtP
-> P_3453 ; _rtB -> B_392_5131_0 = _rtP -> P_3490 ; _rtB -> B_392_5134_0 =
_rtP -> P_3491 ; _rtB -> B_392_5137_0 = _rtP -> P_3492 ; _rtB -> B_392_5212_0
= _rtP -> P_3533 ; _rtB -> B_392_5215_0 = _rtP -> P_3534 ; _rtB ->
B_392_5218_0 = _rtP -> P_3535 ; _rtB -> B_392_5287_0 = _rtP -> P_3573 ; _rtB
-> B_392_5290_0 = _rtP -> P_3574 ; _rtB -> B_392_5293_0 = _rtP -> P_3575 ;
_rtB -> B_392_5392_0 = _rtP -> P_3683 ; _rtB -> B_392_5393_0 = _rtP -> P_3684
; _rtB -> B_392_5394_0 = _rtP -> P_3685 ; _rtB -> B_392_5398_0 = _rtP ->
P_3689 ; _rtB -> B_392_5399_0 = _rtP -> P_3690 ; _rtB -> B_392_5410_0 = _rtP
-> P_3696 ; _rtB -> B_392_5411_0 = _rtP -> P_3697 ; _rtB -> B_392_5422_0 =
_rtP -> P_3703 ; _rtB -> B_392_5423_0 = _rtP -> P_3704 ; _rtB -> B_392_5446_0
= _rtP -> P_3710 ; _rtB -> B_392_5447_0 = _rtP -> P_3711 ; _rtB ->
B_392_5448_0 = _rtP -> P_3712 ; _rtB -> B_392_5452_0 = _rtP -> P_3716 ; _rtB
-> B_392_5453_0 = _rtP -> P_3717 ; _rtB -> B_392_5464_0 = _rtP -> P_3723 ;
_rtB -> B_392_5465_0 = _rtP -> P_3724 ; _rtB -> B_392_5476_0 = _rtP -> P_3730
; _rtB -> B_392_5477_0 = _rtP -> P_3731 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdate ( SimStruct * S , int_T tid ) { real_T HoldSine ;
real_T * lastU ; int32_T isHit ; B_MR_CIGRE_GENERALIZADO_T * _rtB ;
P_MR_CIGRE_GENERALIZADO_T * _rtP ; DW_MR_CIGRE_GENERALIZADO_T * _rtDW ; _rtDW
= ( ( DW_MR_CIGRE_GENERALIZADO_T * ) ssGetRootDWork ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ; isHit =
ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE += _rtP -> P_419 * _rtB -> B_392_852_0 ; _rtDW
-> DiscreteTimeIntegrator_PrevResetState = ( int8_T ) _rtB -> B_392_9_0 ; }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Vdc_DSTATE
= _rtB -> B_0_14_0 [ 25 ] ; _rtDW -> Idc_DSTATE = _rtB -> B_392_795_0 ; }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_k += _rtP -> P_447 * _rtB -> B_392_1189_0 ;
_rtDW -> DiscreteTimeIntegrator_PrevResetState_n = ( int8_T ) _rtB ->
B_392_47_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Vdc_DSTATE_p = _rtB -> B_0_14_0 [ 26 ] ; _rtDW -> Idc_DSTATE_b =
_rtB -> B_392_1133_0 ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit !=
0 ) { _rtDW -> UnitDelay1_DSTATE [ 0 ] = _rtB -> B_392_4148_0 ; _rtDW ->
UnitDelay1_DSTATE [ 1 ] = _rtB -> B_392_4147_0 ; _rtDW -> UnitDelay1_DSTATE [
2 ] = _rtB -> B_392_4369_0 ; _rtDW -> DiscreteTimeIntegrator_DSTATE_a += _rtP
-> P_465 * _rtB -> B_392_4289_0 ; _rtDW ->
DiscreteTimeIntegrator_PrevResetState_p = ( int8_T ) _rtB -> B_392_76_0 ; }
isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Vdc_DSTATE_g = _rtB -> B_0_14_0 [ 27 ] ; _rtDW -> Idc_DSTATE_h = _rtB ->
B_392_4217_0 ; } isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> UnitDelay1_DSTATE_e [ 0 ] = _rtB -> B_392_4682_0 ; _rtDW ->
UnitDelay1_DSTATE_e [ 1 ] = _rtB -> B_392_4681_0 ; _rtDW ->
UnitDelay1_DSTATE_e [ 2 ] = _rtB -> B_392_4903_0 ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_aq += _rtP -> P_476 * _rtB -> B_392_4823_0 ;
_rtDW -> DiscreteTimeIntegrator_PrevResetState_d = ( int8_T ) _rtB ->
B_392_100_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Vdc_DSTATE_b = _rtB -> B_0_14_0 [ 28 ] ; _rtDW -> Idc_DSTATE_k =
_rtB -> B_392_4751_0 ; _rtDW -> UnitDelay5_DSTATE = _rtB -> B_392_4403_0 ;
_rtDW -> UnitDelay6_DSTATE = _rtB -> B_392_133_0 ; _rtDW ->
UnitDelay1_DSTATE_p [ 0 ] = _rtB -> B_392_4411_0 ; _rtDW ->
UnitDelay1_DSTATE_p [ 1 ] = _rtB -> B_392_4407_0 ; _rtDW ->
UnitDelay5_DSTATE_e = _rtB -> B_392_4937_0 ; _rtDW -> UnitDelay6_DSTATE_l =
_rtB -> B_392_148_0 ; _rtDW -> UnitDelay1_DSTATE_m [ 0 ] = _rtB ->
B_392_4945_0 ; _rtDW -> UnitDelay1_DSTATE_m [ 1 ] = _rtB -> B_392_4941_0 ;
_rtDW -> Delay_x_DSTATE = _rtB -> B_392_4395_0 ; _rtDW -> Delay_x_DSTATE_h =
_rtB -> B_392_4389_0 ; _rtDW -> Delay_x_DSTATE_e = _rtB -> B_392_4929_0 ;
_rtDW -> Delay_x_DSTATE_b = _rtB -> B_392_4923_0 ; HoldSine = _rtDW ->
lastSin ; _rtDW -> lastSin = _rtDW -> lastSin * _rtP -> P_558 + _rtDW ->
lastCos * _rtP -> P_557 ; _rtDW -> lastCos = _rtDW -> lastCos * _rtP -> P_558
- HoldSine * _rtP -> P_557 ; HoldSine = _rtDW -> lastSin_i ; _rtDW ->
lastSin_i = _rtDW -> lastSin_i * _rtP -> P_565 + _rtDW -> lastCos_p * _rtP ->
P_564 ; _rtDW -> lastCos_p = _rtDW -> lastCos_p * _rtP -> P_565 - HoldSine *
_rtP -> P_564 ; HoldSine = _rtDW -> lastSin_p ; _rtDW -> lastSin_p = _rtDW ->
lastSin_p * _rtP -> P_572 + _rtDW -> lastCos_c * _rtP -> P_571 ; _rtDW ->
lastCos_c = _rtDW -> lastCos_c * _rtP -> P_572 - HoldSine * _rtP -> P_571 ;
MR_CIGRE_GENERALIZADO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Update ( S
) ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW ->
LastMajorTimeA == ( rtInf ) ) { _rtDW -> LastMajorTimeA = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYA = _rtB -> B_392_241_0 ; } else if ( _rtDW ->
LastMajorTimeB == ( rtInf ) ) { _rtDW -> LastMajorTimeB = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYB = _rtB -> B_392_241_0 ; } else if ( _rtDW ->
LastMajorTimeA < _rtDW -> LastMajorTimeB ) { _rtDW -> LastMajorTimeA =
ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA = _rtB -> B_392_241_0 ; } else {
_rtDW -> LastMajorTimeB = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB = _rtB ->
B_392_241_0 ; } if ( _rtDW -> LastMajorTimeA_o == ( rtInf ) ) { _rtDW ->
LastMajorTimeA_o = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA_e = _rtB ->
B_392_244_0 ; } else if ( _rtDW -> LastMajorTimeB_h == ( rtInf ) ) { _rtDW ->
LastMajorTimeB_h = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_l = _rtB ->
B_392_244_0 ; } else if ( _rtDW -> LastMajorTimeA_o < _rtDW ->
LastMajorTimeB_h ) { _rtDW -> LastMajorTimeA_o = ssGetTaskTime ( S , 1 ) ;
_rtDW -> PrevYA_e = _rtB -> B_392_244_0 ; } else { _rtDW -> LastMajorTimeB_h
= ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_l = _rtB -> B_392_244_0 ; } }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
UnitDelay_DSTATE = _rtB -> B_392_871_0 ; } { real_T * * uBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 0 ] ; real_T * * tBuffer
= ( real_T * * ) & _rtDW -> TransportDelay_PWORK . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK . Head = ( (
_rtDW -> TransportDelay_IWORK . Head < ( _rtDW -> TransportDelay_IWORK .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK . Head + 1 ) : 0 )
; if ( _rtDW -> TransportDelay_IWORK . Head == _rtDW -> TransportDelay_IWORK
. Tail ) { if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( &
_rtDW -> TransportDelay_IWORK . CircularBufSize , & _rtDW ->
TransportDelay_IWORK . Tail , & _rtDW -> TransportDelay_IWORK . Head , &
_rtDW -> TransportDelay_IWORK . Last , simTime - _rtP -> P_603 , tBuffer ,
uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK . Head ] = _rtB -> B_392_259_0 ; } isHit = ssIsSampleHit
( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW -> Memory_PreviousInput = _rtB ->
B_392_266_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_o . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T *
* ) & _rtDW -> TransportDelay_PWORK_o . TUbufferPtrs [ 1 ] ; real_T simTime =
ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_h . Head = ( ( _rtDW ->
TransportDelay_IWORK_h . Head < ( _rtDW -> TransportDelay_IWORK_h .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_h . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_h . Head == _rtDW ->
TransportDelay_IWORK_h . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h . CircularBufSize , & _rtDW -> TransportDelay_IWORK_h
. Tail , & _rtDW -> TransportDelay_IWORK_h . Head , & _rtDW ->
TransportDelay_IWORK_h . Last , simTime - _rtP -> P_608 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_h .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_h . Head ] = _rtB ->
B_392_267_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_a = _rtB -> B_392_274_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l . Head = ( ( _rtDW -> TransportDelay_IWORK_l . Head <
( _rtDW -> TransportDelay_IWORK_l . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l . Head == _rtDW -> TransportDelay_IWORK_l . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l . CircularBufSize , & _rtDW -> TransportDelay_IWORK_l
. Tail , & _rtDW -> TransportDelay_IWORK_l . Head , & _rtDW ->
TransportDelay_IWORK_l . Last , simTime - _rtP -> P_615 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_l .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_l . Head ] = _rtB ->
B_392_280_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_av = _rtB -> B_392_287_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k . Head = ( ( _rtDW -> TransportDelay_IWORK_k . Head <
( _rtDW -> TransportDelay_IWORK_k . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k . Head == _rtDW -> TransportDelay_IWORK_k . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k . CircularBufSize , & _rtDW -> TransportDelay_IWORK_k
. Tail , & _rtDW -> TransportDelay_IWORK_k . Head , & _rtDW ->
TransportDelay_IWORK_k . Last , simTime - _rtP -> P_620 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_k .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_k . Head ] = _rtB ->
B_392_288_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_e = _rtB -> B_392_295_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c . Head = ( ( _rtDW -> TransportDelay_IWORK_c . Head <
( _rtDW -> TransportDelay_IWORK_c . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c . Head == _rtDW -> TransportDelay_IWORK_c . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c . CircularBufSize , & _rtDW -> TransportDelay_IWORK_c
. Tail , & _rtDW -> TransportDelay_IWORK_c . Head , & _rtDW ->
TransportDelay_IWORK_c . Last , simTime - _rtP -> P_627 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_c .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_c . Head ] = _rtB ->
B_392_301_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_d = _rtB -> B_392_308_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m . Head = ( ( _rtDW -> TransportDelay_IWORK_m . Head <
( _rtDW -> TransportDelay_IWORK_m . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m . Head == _rtDW -> TransportDelay_IWORK_m . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m . CircularBufSize , & _rtDW -> TransportDelay_IWORK_m
. Tail , & _rtDW -> TransportDelay_IWORK_m . Head , & _rtDW ->
TransportDelay_IWORK_m . Last , simTime - _rtP -> P_632 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_m .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_m . Head ] = _rtB ->
B_392_309_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ac = _rtB -> B_392_316_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a . Head = ( ( _rtDW -> TransportDelay_IWORK_a . Head <
( _rtDW -> TransportDelay_IWORK_a . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a . Head == _rtDW -> TransportDelay_IWORK_a . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a . CircularBufSize , & _rtDW -> TransportDelay_IWORK_a
. Tail , & _rtDW -> TransportDelay_IWORK_a . Head , & _rtDW ->
TransportDelay_IWORK_a . Last , simTime - _rtP -> P_643 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_a .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_a . Head ] = _rtB ->
B_392_333_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_am = _rtB -> B_392_340_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_me
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p . Head = ( ( _rtDW -> TransportDelay_IWORK_p . Head <
( _rtDW -> TransportDelay_IWORK_p . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p . Head == _rtDW -> TransportDelay_IWORK_p . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p . CircularBufSize , & _rtDW -> TransportDelay_IWORK_p
. Tail , & _rtDW -> TransportDelay_IWORK_p . Head , & _rtDW ->
TransportDelay_IWORK_p . Last , simTime - _rtP -> P_648 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_p .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_p . Head ] = _rtB ->
B_392_341_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_f = _rtB -> B_392_348_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_d . Head = ( ( _rtDW -> TransportDelay_IWORK_d . Head <
( _rtDW -> TransportDelay_IWORK_d . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_d . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_d . Head == _rtDW -> TransportDelay_IWORK_d . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_d . CircularBufSize , & _rtDW -> TransportDelay_IWORK_d
. Tail , & _rtDW -> TransportDelay_IWORK_d . Head , & _rtDW ->
TransportDelay_IWORK_d . Last , simTime - _rtP -> P_655 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_d .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_d . Head ] = _rtB ->
B_392_354_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_o = _rtB -> B_392_361_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dg . Head = ( ( _rtDW -> TransportDelay_IWORK_dg . Head
< ( _rtDW -> TransportDelay_IWORK_dg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dg . Head == _rtDW -> TransportDelay_IWORK_dg . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dg . Tail , & _rtDW -> TransportDelay_IWORK_dg . Head ,
& _rtDW -> TransportDelay_IWORK_dg . Last , simTime - _rtP -> P_660 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dg . Head ] = _rtB -> B_392_362_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l = _rtB -> B_392_369_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_o .
Head = ( ( _rtDW -> TransportDelay_IWORK_o . Head < ( _rtDW ->
TransportDelay_IWORK_o . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o . Head == _rtDW -> TransportDelay_IWORK_o . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o . CircularBufSize , & _rtDW -> TransportDelay_IWORK_o
. Tail , & _rtDW -> TransportDelay_IWORK_o . Head , & _rtDW ->
TransportDelay_IWORK_o . Last , simTime - _rtP -> P_667 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_o .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_o . Head ] = _rtB ->
B_392_375_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_j = _rtB -> B_392_382_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hf
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ph . Head = ( ( _rtDW -> TransportDelay_IWORK_ph . Head
< ( _rtDW -> TransportDelay_IWORK_ph . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ph . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ph . Head == _rtDW -> TransportDelay_IWORK_ph . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ph . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ph . Tail , & _rtDW -> TransportDelay_IWORK_ph . Head ,
& _rtDW -> TransportDelay_IWORK_ph . Last , simTime - _rtP -> P_672 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ph . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ph . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ph . Head ] = _rtB -> B_392_383_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b = _rtB -> B_392_390_0 ; if ( _rtDW -> LastMajorTimeA_l
== ( rtInf ) ) { _rtDW -> LastMajorTimeA_l = ssGetTaskTime ( S , 1 ) ; _rtDW
-> PrevYA_ep = _rtB -> B_392_414_0 ; } else if ( _rtDW -> LastMajorTimeB_g ==
( rtInf ) ) { _rtDW -> LastMajorTimeB_g = ssGetTaskTime ( S , 1 ) ; _rtDW ->
PrevYB_c = _rtB -> B_392_414_0 ; } else if ( _rtDW -> LastMajorTimeA_l <
_rtDW -> LastMajorTimeB_g ) { _rtDW -> LastMajorTimeA_l = ssGetTaskTime ( S ,
1 ) ; _rtDW -> PrevYA_ep = _rtB -> B_392_414_0 ; } else { _rtDW ->
LastMajorTimeB_g = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_c = _rtB ->
B_392_414_0 ; } if ( _rtDW -> LastMajorTimeA_f == ( rtInf ) ) { _rtDW ->
LastMajorTimeA_f = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYA_f = _rtB ->
B_392_417_0 ; } else if ( _rtDW -> LastMajorTimeB_c == ( rtInf ) ) { _rtDW ->
LastMajorTimeB_c = ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_e = _rtB ->
B_392_417_0 ; } else if ( _rtDW -> LastMajorTimeA_f < _rtDW ->
LastMajorTimeB_c ) { _rtDW -> LastMajorTimeA_f = ssGetTaskTime ( S , 1 ) ;
_rtDW -> PrevYA_f = _rtB -> B_392_417_0 ; } else { _rtDW -> LastMajorTimeB_c
= ssGetTaskTime ( S , 1 ) ; _rtDW -> PrevYB_e = _rtB -> B_392_417_0 ; } }
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
UnitDelay_DSTATE_o = _rtB -> B_392_1208_0 ; } { real_T * * uBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [ 0 ] ; real_T * *
tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5 . TUbufferPtrs [
1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_m5 . Head
= ( ( _rtDW -> TransportDelay_IWORK_m5 . Head < ( _rtDW ->
TransportDelay_IWORK_m5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m5 . Head == _rtDW -> TransportDelay_IWORK_m5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m5 . Tail , & _rtDW -> TransportDelay_IWORK_m5 . Head ,
& _rtDW -> TransportDelay_IWORK_m5 . Last , simTime - _rtP -> P_707 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m5 . Head ] = _rtB -> B_392_432_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lv = _rtB -> B_392_439_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i . Head = ( ( _rtDW -> TransportDelay_IWORK_i . Head <
( _rtDW -> TransportDelay_IWORK_i . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_i . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i . Head == _rtDW -> TransportDelay_IWORK_i . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i . CircularBufSize , & _rtDW -> TransportDelay_IWORK_i
. Tail , & _rtDW -> TransportDelay_IWORK_i . Head , & _rtDW ->
TransportDelay_IWORK_i . Last , simTime - _rtP -> P_712 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_i .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_i . Head ] = _rtB ->
B_392_440_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ff = _rtB -> B_392_447_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p . TUbufferPtrs [ 0
] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b . Head = ( ( _rtDW -> TransportDelay_IWORK_b . Head <
( _rtDW -> TransportDelay_IWORK_b . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b . Head == _rtDW -> TransportDelay_IWORK_b . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b . CircularBufSize , & _rtDW -> TransportDelay_IWORK_b
. Tail , & _rtDW -> TransportDelay_IWORK_b . Head , & _rtDW ->
TransportDelay_IWORK_b . Last , simTime - _rtP -> P_719 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_b .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_b . Head ] = _rtB ->
B_392_453_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_of = _rtB -> B_392_460_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_an . Head = ( ( _rtDW -> TransportDelay_IWORK_an . Head
< ( _rtDW -> TransportDelay_IWORK_an . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_an . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_an . Head == _rtDW -> TransportDelay_IWORK_an . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_an . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_an . Tail , & _rtDW -> TransportDelay_IWORK_an . Head ,
& _rtDW -> TransportDelay_IWORK_an . Last , simTime - _rtP -> P_724 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_an . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_an . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_an . Head ] = _rtB -> B_392_461_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m = _rtB -> B_392_468_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e . Head = ( ( _rtDW -> TransportDelay_IWORK_e . Head <
( _rtDW -> TransportDelay_IWORK_e . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e . Head == _rtDW -> TransportDelay_IWORK_e . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e . CircularBufSize , & _rtDW -> TransportDelay_IWORK_e
. Tail , & _rtDW -> TransportDelay_IWORK_e . Head , & _rtDW ->
TransportDelay_IWORK_e . Last , simTime - _rtP -> P_731 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_e .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_e . Head ] = _rtB ->
B_392_474_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_a3 = _rtB -> B_392_481_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pk . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pk
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j . Head = ( ( _rtDW -> TransportDelay_IWORK_j . Head <
( _rtDW -> TransportDelay_IWORK_j . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j . Head == _rtDW -> TransportDelay_IWORK_j . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j . CircularBufSize , & _rtDW -> TransportDelay_IWORK_j
. Tail , & _rtDW -> TransportDelay_IWORK_j . Head , & _rtDW ->
TransportDelay_IWORK_j . Last , simTime - _rtP -> P_736 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_j .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_j . Head ] = _rtB ->
B_392_482_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ll = _rtB -> B_392_489_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dx
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lg . Head = ( ( _rtDW -> TransportDelay_IWORK_lg . Head
< ( _rtDW -> TransportDelay_IWORK_lg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lg . Head == _rtDW -> TransportDelay_IWORK_lg . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lg . Tail , & _rtDW -> TransportDelay_IWORK_lg . Head ,
& _rtDW -> TransportDelay_IWORK_lg . Last , simTime - _rtP -> P_747 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lg . Head ] = _rtB -> B_392_506_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_e0 = _rtB -> B_392_513_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_p3 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ho . Head = ( ( _rtDW -> TransportDelay_IWORK_ho . Head
< ( _rtDW -> TransportDelay_IWORK_ho . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ho . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ho . Head == _rtDW -> TransportDelay_IWORK_ho . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ho . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ho . Tail , & _rtDW -> TransportDelay_IWORK_ho . Head ,
& _rtDW -> TransportDelay_IWORK_ho . Last , simTime - _rtP -> P_752 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ho . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ho . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ho . Head ] = _rtB -> B_392_514_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h = _rtB -> B_392_521_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ay .
Head = ( ( _rtDW -> TransportDelay_IWORK_ay . Head < ( _rtDW ->
TransportDelay_IWORK_ay . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ay . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ay . Head == _rtDW -> TransportDelay_IWORK_ay . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ay . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ay . Tail , & _rtDW -> TransportDelay_IWORK_ay . Head ,
& _rtDW -> TransportDelay_IWORK_ay . Last , simTime - _rtP -> P_759 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ay . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ay . Head ] = _rtB -> B_392_527_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i = _rtB -> B_392_534_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g . Head = ( ( _rtDW -> TransportDelay_IWORK_g . Head <
( _rtDW -> TransportDelay_IWORK_g . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g . Head == _rtDW -> TransportDelay_IWORK_g . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g . CircularBufSize , & _rtDW -> TransportDelay_IWORK_g
. Tail , & _rtDW -> TransportDelay_IWORK_g . Head , & _rtDW ->
TransportDelay_IWORK_g . Last , simTime - _rtP -> P_764 , tBuffer , uBuffer ,
( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_g .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_g . Head ] = _rtB ->
B_392_535_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_k = _rtB -> B_392_542_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b3 . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b3
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mq . Head = ( ( _rtDW -> TransportDelay_IWORK_mq . Head
< ( _rtDW -> TransportDelay_IWORK_mq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mq . Head == _rtDW -> TransportDelay_IWORK_mq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mq . Tail , & _rtDW -> TransportDelay_IWORK_mq . Head ,
& _rtDW -> TransportDelay_IWORK_mq . Last , simTime - _rtP -> P_771 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mq . Head ] = _rtB -> B_392_548_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_im = _rtB -> B_392_555_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_od .
Head = ( ( _rtDW -> TransportDelay_IWORK_od . Head < ( _rtDW ->
TransportDelay_IWORK_od . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_od . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_od . Head == _rtDW -> TransportDelay_IWORK_od . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_od . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_od . Tail , & _rtDW -> TransportDelay_IWORK_od . Head ,
& _rtDW -> TransportDelay_IWORK_od . Last , simTime - _rtP -> P_776 , tBuffer
, uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_od . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_od . Head ] = _rtB -> B_392_556_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jc = _rtB -> B_392_563_0 ; } isHit = ssIsSampleHit ( S ,
3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay1_DSTATE_pe = _rtB ->
B_392_872_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE = _rtB -> B_392_33_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_l ) {
isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_a = 0U ; _rtDW -> Integ4_DSTATE_e = _rtP -> P_26 * _rtB
-> B_12_11_0 [ 0 ] + _rtB -> B_12_12_0 ; ssCallAccelRunBlock ( S , 12 , 17 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_n = _rtB -> B_12_11_0 [ 0 ]
; _rtDW -> UnitDelay1_DSTATE_fp = _rtB -> B_12_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_ll = 0U ; _rtDW -> Integ4_DSTATE_c = _rtP -> P_42 * _rtB
-> B_12_11_0 [ 1 ] + _rtB -> B_12_25_0 ; ssCallAccelRunBlock ( S , 12 , 30 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_k = _rtB -> B_12_11_0 [ 1 ]
; _rtDW -> UnitDelay1_DSTATE_b = _rtB -> B_12_37_0 ; } } _rtDW ->
Integ4_SYSTEM_ENABLE = 0U ; _rtDW -> Integ4_DSTATE = _rtP -> P_889 * _rtB ->
B_392_833_0 [ 1 ] + _rtB -> B_392_834_0 ; ssCallAccelRunBlock ( S , 392 , 839
, SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_i = _rtB -> B_392_833_0 [
1 ] ; _rtDW -> UnitDelay1_DSTATE_i = _rtB -> B_392_846_0 ; _rtDW ->
DiscreteDerivative_states = _rtDW -> DiscreteDerivative_tmp ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_c += _rtP -> P_908 * _rtB -> B_392_847_0 ; if (
_rtDW -> DiscreteTimeIntegrator_DSTATE_c >= _rtP -> P_910 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_c = _rtP -> P_910 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_c <= _rtP -> P_911 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_c = _rtP -> P_911 ; } } _rtDW ->
Delay_x1_DSTATE = _rtB -> B_392_864_0 ; _rtDW -> Delay_x2_DSTATE = _rtB ->
B_392_866_0 ; _rtDW -> UnitDelay1_DSTATE_a = _rtB -> B_392_946_0 ; } isHit =
ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
prediction_DSTATE_l = _rtB -> B_392_68_0 ; } isHit = ssIsSampleHit ( S , 3 ,
0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_i ) { isHit
= ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_lr = 0U ; _rtDW -> Integ4_DSTATE_a = _rtP -> P_82 * _rtB
-> B_48_11_0 [ 0 ] + _rtB -> B_48_12_0 ; ssCallAccelRunBlock ( S , 48 , 17 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_oc = _rtB -> B_48_11_0 [ 0 ]
; _rtDW -> UnitDelay1_DSTATE_c = _rtB -> B_48_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_c = 0U ; _rtDW -> Integ4_DSTATE_am = _rtP -> P_98 * _rtB
-> B_48_11_0 [ 1 ] + _rtB -> B_48_25_0 ; ssCallAccelRunBlock ( S , 48 , 30 ,
SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_bg = _rtB -> B_48_11_0 [ 1 ]
; _rtDW -> UnitDelay1_DSTATE_o0 = _rtB -> B_48_37_0 ; } } _rtDW ->
Integ4_SYSTEM_ENABLE_m = 0U ; _rtDW -> Integ4_DSTATE_b = _rtP -> P_1082 *
_rtB -> B_392_1170_0 [ 1 ] + _rtB -> B_392_1171_0 ; ssCallAccelRunBlock ( S ,
392 , 1176 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_ob = _rtB ->
B_392_1170_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_k = _rtB -> B_392_1183_0 ;
_rtDW -> DiscreteDerivative_states_b = _rtDW -> DiscreteDerivative_tmp_h ;
_rtDW -> DiscreteTimeIntegrator_DSTATE_ce += _rtP -> P_1101 * _rtB ->
B_392_1184_0 ; if ( _rtDW -> DiscreteTimeIntegrator_DSTATE_ce >= _rtP ->
P_1103 ) { _rtDW -> DiscreteTimeIntegrator_DSTATE_ce = _rtP -> P_1103 ; }
else { if ( _rtDW -> DiscreteTimeIntegrator_DSTATE_ce <= _rtP -> P_1104 ) {
_rtDW -> DiscreteTimeIntegrator_DSTATE_ce = _rtP -> P_1104 ; } } _rtDW ->
Delay_x1_DSTATE_f = _rtB -> B_392_1201_0 ; _rtDW -> Delay_x2_DSTATE_k = _rtB
-> B_392_1203_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ke . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_ke . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_mp . Head = ( ( _rtDW
-> TransportDelay_IWORK_mp . Head < ( _rtDW -> TransportDelay_IWORK_mp .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_mp . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_mp . Head == _rtDW ->
TransportDelay_IWORK_mp . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mp . Tail , & _rtDW -> TransportDelay_IWORK_mp . Head ,
& _rtDW -> TransportDelay_IWORK_mp . Last , simTime - _rtP -> P_1178 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mp . Head ] = _rtB -> B_392_1282_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fp = _rtB -> B_392_1289_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k0 . Head = ( ( _rtDW -> TransportDelay_IWORK_k0 . Head
< ( _rtDW -> TransportDelay_IWORK_k0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k0 . Head == _rtDW -> TransportDelay_IWORK_k0 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k0 . Tail , & _rtDW -> TransportDelay_IWORK_k0 . Head ,
& _rtDW -> TransportDelay_IWORK_k0 . Last , simTime - _rtP -> P_1183 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k0 . Head ] = _rtB -> B_392_1290_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lu = _rtB -> B_392_1297_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_l2 .
Head = ( ( _rtDW -> TransportDelay_IWORK_l2 . Head < ( _rtDW ->
TransportDelay_IWORK_l2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l2 . Head == _rtDW -> TransportDelay_IWORK_l2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l2 . Tail , & _rtDW -> TransportDelay_IWORK_l2 . Head ,
& _rtDW -> TransportDelay_IWORK_l2 . Last , simTime - _rtP -> P_1190 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l2 . Head ] = _rtB -> B_392_1303_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_p = _rtB -> B_392_1310_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ha . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ha .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a1 . Head = ( ( _rtDW -> TransportDelay_IWORK_a1 . Head
< ( _rtDW -> TransportDelay_IWORK_a1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a1 . Head == _rtDW -> TransportDelay_IWORK_a1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a1 . Tail , & _rtDW -> TransportDelay_IWORK_a1 . Head ,
& _rtDW -> TransportDelay_IWORK_a1 . Last , simTime - _rtP -> P_1195 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a1 . Head ] = _rtB -> B_392_1311_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bn = _rtB -> B_392_1318_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eh . Head = ( ( _rtDW -> TransportDelay_IWORK_eh . Head
< ( _rtDW -> TransportDelay_IWORK_eh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eh . Head == _rtDW -> TransportDelay_IWORK_eh . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eh . Tail , & _rtDW -> TransportDelay_IWORK_eh . Head ,
& _rtDW -> TransportDelay_IWORK_eh . Last , simTime - _rtP -> P_1202 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eh . Head ] = _rtB -> B_392_1324_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pm = _rtB -> B_392_1331_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f . Head = ( ( _rtDW -> TransportDelay_IWORK_f . Head <
( _rtDW -> TransportDelay_IWORK_f . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f . Head == _rtDW -> TransportDelay_IWORK_f . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f . CircularBufSize , & _rtDW -> TransportDelay_IWORK_f
. Tail , & _rtDW -> TransportDelay_IWORK_f . Head , & _rtDW ->
TransportDelay_IWORK_f . Last , simTime - _rtP -> P_1207 , tBuffer , uBuffer
, ( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_f .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_f . Head ] = _rtB ->
B_392_1332_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_an = _rtB -> B_392_1339_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bm
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ij . Head = ( ( _rtDW -> TransportDelay_IWORK_ij . Head
< ( _rtDW -> TransportDelay_IWORK_ij . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ij . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ij . Head == _rtDW -> TransportDelay_IWORK_ij . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ij . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ij . Tail , & _rtDW -> TransportDelay_IWORK_ij . Head ,
& _rtDW -> TransportDelay_IWORK_ij . Last , simTime - _rtP -> P_1217 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ij . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ij . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ij . Head ] = _rtB -> B_392_1355_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hj = _rtB -> B_392_1362_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ck . Head = ( ( _rtDW -> TransportDelay_IWORK_ck . Head
< ( _rtDW -> TransportDelay_IWORK_ck . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ck . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ck . Head == _rtDW -> TransportDelay_IWORK_ck . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ck . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ck . Tail , & _rtDW -> TransportDelay_IWORK_ck . Head ,
& _rtDW -> TransportDelay_IWORK_ck . Last , simTime - _rtP -> P_1222 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ck . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ck . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ck . Head ] = _rtB -> B_392_1363_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eg = _rtB -> B_392_1370_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5f .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ok . Head = ( ( _rtDW -> TransportDelay_IWORK_ok . Head
< ( _rtDW -> TransportDelay_IWORK_ok . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ok . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ok . Head == _rtDW -> TransportDelay_IWORK_ok . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ok . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ok . Tail , & _rtDW -> TransportDelay_IWORK_ok . Head ,
& _rtDW -> TransportDelay_IWORK_ok . Last , simTime - _rtP -> P_1229 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ok . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ok . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ok . Head ] = _rtB -> B_392_1376_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dh = _rtB -> B_392_1383_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_fi .
Head = ( ( _rtDW -> TransportDelay_IWORK_fi . Head < ( _rtDW ->
TransportDelay_IWORK_fi . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fi . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fi . Head == _rtDW -> TransportDelay_IWORK_fi . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fi . Tail , & _rtDW -> TransportDelay_IWORK_fi . Head ,
& _rtDW -> TransportDelay_IWORK_fi . Last , simTime - _rtP -> P_1234 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fi . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fi . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fi . Head ] = _rtB -> B_392_1384_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mp = _rtB -> B_392_1391_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ea . Head = ( ( _rtDW -> TransportDelay_IWORK_ea . Head
< ( _rtDW -> TransportDelay_IWORK_ea . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ea . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ea . Head == _rtDW -> TransportDelay_IWORK_ea . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ea . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ea . Tail , & _rtDW -> TransportDelay_IWORK_ea . Head ,
& _rtDW -> TransportDelay_IWORK_ea . Last , simTime - _rtP -> P_1241 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ea . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea . Head ] = _rtB -> B_392_1397_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g = _rtB -> B_392_1404_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_me .
Head = ( ( _rtDW -> TransportDelay_IWORK_me . Head < ( _rtDW ->
TransportDelay_IWORK_me . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_me . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_me . Head == _rtDW -> TransportDelay_IWORK_me . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_me . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_me . Tail , & _rtDW -> TransportDelay_IWORK_me . Head ,
& _rtDW -> TransportDelay_IWORK_me . Last , simTime - _rtP -> P_1246 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_me . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_me . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_me . Head ] = _rtB -> B_392_1405_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_av3 = _rtB -> B_392_1412_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h45 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_di . Head = ( ( _rtDW -> TransportDelay_IWORK_di . Head
< ( _rtDW -> TransportDelay_IWORK_di . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_di . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_di . Head == _rtDW -> TransportDelay_IWORK_di . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_di . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_di . Tail , & _rtDW -> TransportDelay_IWORK_di . Head ,
& _rtDW -> TransportDelay_IWORK_di . Last , simTime - _rtP -> P_1262 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_di . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_di . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_di . Head ] = _rtB -> B_392_1439_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fq = _rtB -> B_392_1446_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_a . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_n .
Head = ( ( _rtDW -> TransportDelay_IWORK_n . Head < ( _rtDW ->
TransportDelay_IWORK_n . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n . Head == _rtDW -> TransportDelay_IWORK_n . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n . CircularBufSize , & _rtDW -> TransportDelay_IWORK_n
. Tail , & _rtDW -> TransportDelay_IWORK_n . Head , & _rtDW ->
TransportDelay_IWORK_n . Last , simTime - _rtP -> P_1267 , tBuffer , uBuffer
, ( NULL ) , ( boolean_T ) 0 , false , & _rtDW -> TransportDelay_IWORK_n .
MaxNewBufSize ) ) { ssSetErrorStatus ( S , "tdelay memory allocation error" )
; return ; } } ( * tBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] =
simTime ; ( * uBuffer ) [ _rtDW -> TransportDelay_IWORK_n . Head ] = _rtB ->
B_392_1447_0 ; } isHit = ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> Memory_PreviousInput_ffa = _rtB -> B_392_1454_0 ; } { real_T * *
uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om . TUbufferPtrs [
0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_om
. TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj . Head = ( ( _rtDW -> TransportDelay_IWORK_hj . Head
< ( _rtDW -> TransportDelay_IWORK_hj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj . Head == _rtDW -> TransportDelay_IWORK_hj . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj . Tail , & _rtDW -> TransportDelay_IWORK_hj . Head ,
& _rtDW -> TransportDelay_IWORK_hj . Last , simTime - _rtP -> P_1274 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj . Head ] = _rtB -> B_392_1460_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fm = _rtB -> B_392_1467_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oz . Head = ( ( _rtDW -> TransportDelay_IWORK_oz . Head
< ( _rtDW -> TransportDelay_IWORK_oz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oz . Head == _rtDW -> TransportDelay_IWORK_oz . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oz . Tail , & _rtDW -> TransportDelay_IWORK_oz . Head ,
& _rtDW -> TransportDelay_IWORK_oz . Last , simTime - _rtP -> P_1279 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oz . Head ] = _rtB -> B_392_1468_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fv = _rtB -> B_392_1475_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ek . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ek .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fq . Head = ( ( _rtDW -> TransportDelay_IWORK_fq . Head
< ( _rtDW -> TransportDelay_IWORK_fq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fq . Head == _rtDW -> TransportDelay_IWORK_fq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fq . Tail , & _rtDW -> TransportDelay_IWORK_fq . Head ,
& _rtDW -> TransportDelay_IWORK_fq . Last , simTime - _rtP -> P_1286 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fq . Head ] = _rtB -> B_392_1481_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gx = _rtB -> B_392_1488_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g1 . Head = ( ( _rtDW -> TransportDelay_IWORK_g1 . Head
< ( _rtDW -> TransportDelay_IWORK_g1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g1 . Head == _rtDW -> TransportDelay_IWORK_g1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g1 . Tail , & _rtDW -> TransportDelay_IWORK_g1 . Head ,
& _rtDW -> TransportDelay_IWORK_g1 . Last , simTime - _rtP -> P_1291 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g1 . Head ] = _rtB -> B_392_1489_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pc = _rtB -> B_392_1496_0 ; } if ( _rtDW -> TimeStampA
== ( rtInf ) ) { _rtDW -> TimeStampA = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA ; } else if ( _rtDW -> TimeStampB == ( rtInf ) ) { _rtDW ->
TimeStampB = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB ; } else if (
_rtDW -> TimeStampA < _rtDW -> TimeStampB ) { _rtDW -> TimeStampA = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA ; } else { _rtDW -> TimeStampB = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB ; } * lastU = _rtB -> B_392_1606_0 ;
if ( _rtDW -> TimeStampA_n == ( rtInf ) ) { _rtDW -> TimeStampA_n = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA_j ; } else if ( _rtDW -> TimeStampB_i
== ( rtInf ) ) { _rtDW -> TimeStampB_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_d ; } else if ( _rtDW -> TimeStampA_n < _rtDW -> TimeStampB_i )
{ _rtDW -> TimeStampA_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_j ;
} else { _rtDW -> TimeStampB_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_d ; } * lastU = _rtB -> B_392_1618_0 ; if ( _rtDW ->
TimeStampA_nq == ( rtInf ) ) { _rtDW -> TimeStampA_nq = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_c ; } else if ( _rtDW -> TimeStampB_k == ( rtInf )
) { _rtDW -> TimeStampB_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_m
; } else if ( _rtDW -> TimeStampA_nq < _rtDW -> TimeStampB_k ) { _rtDW ->
TimeStampA_nq = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_c ; } else {
_rtDW -> TimeStampB_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_m ; }
* lastU = _rtB -> B_392_1630_0 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_nm . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_nm . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_hog . Head = (
( _rtDW -> TransportDelay_IWORK_hog . Head < ( _rtDW ->
TransportDelay_IWORK_hog . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hog . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hog . Head == _rtDW -> TransportDelay_IWORK_hog . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hog . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hog . Tail , & _rtDW -> TransportDelay_IWORK_hog . Head
, & _rtDW -> TransportDelay_IWORK_hog . Last , simTime - _rtP -> P_1428 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hog . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hog . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hog . Head ] = _rtB -> B_392_1719_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pz = _rtB -> B_392_1726_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ak . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ak .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ju . Head = ( ( _rtDW -> TransportDelay_IWORK_ju . Head
< ( _rtDW -> TransportDelay_IWORK_ju . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ju . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ju . Head == _rtDW -> TransportDelay_IWORK_ju . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ju . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ju . Tail , & _rtDW -> TransportDelay_IWORK_ju . Head ,
& _rtDW -> TransportDelay_IWORK_ju . Last , simTime - _rtP -> P_1433 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ju . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ju . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ju . Head ] = _rtB -> B_392_1727_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ft = _rtB -> B_392_1734_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_js . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_js .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a3 . Head = ( ( _rtDW -> TransportDelay_IWORK_a3 . Head
< ( _rtDW -> TransportDelay_IWORK_a3 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_a3 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a3 . Head == _rtDW -> TransportDelay_IWORK_a3 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a3 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a3 . Tail , & _rtDW -> TransportDelay_IWORK_a3 . Head ,
& _rtDW -> TransportDelay_IWORK_a3 . Last , simTime - _rtP -> P_1440 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a3 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3 . Head ] = _rtB -> B_392_1740_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gi = _rtB -> B_392_1747_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_at . Head = ( ( _rtDW -> TransportDelay_IWORK_at . Head
< ( _rtDW -> TransportDelay_IWORK_at . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_at . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_at . Head == _rtDW -> TransportDelay_IWORK_at . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_at . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_at . Tail , & _rtDW -> TransportDelay_IWORK_at . Head ,
& _rtDW -> TransportDelay_IWORK_at . Last , simTime - _rtP -> P_1445 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_at . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_at . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_at . Head ] = _rtB -> B_392_1748_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hg = _rtB -> B_392_1755_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ce . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ce .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_h5 . Head = ( ( _rtDW -> TransportDelay_IWORK_h5 . Head
< ( _rtDW -> TransportDelay_IWORK_h5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_h5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_h5 . Head == _rtDW -> TransportDelay_IWORK_h5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_h5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_h5 . Tail , & _rtDW -> TransportDelay_IWORK_h5 . Head ,
& _rtDW -> TransportDelay_IWORK_h5 . Last , simTime - _rtP -> P_1452 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_h5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_h5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_h5 . Head ] = _rtB -> B_392_1761_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dg = _rtB -> B_392_1768_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cr . Head = ( ( _rtDW -> TransportDelay_IWORK_cr . Head
< ( _rtDW -> TransportDelay_IWORK_cr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cr . Head == _rtDW -> TransportDelay_IWORK_cr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cr . Tail , & _rtDW -> TransportDelay_IWORK_cr . Head ,
& _rtDW -> TransportDelay_IWORK_cr . Last , simTime - _rtP -> P_1457 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cr . Head ] = _rtB -> B_392_1769_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kp = _rtB -> B_392_1776_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_c0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dv . Head = ( ( _rtDW -> TransportDelay_IWORK_dv . Head
< ( _rtDW -> TransportDelay_IWORK_dv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dv . Head == _rtDW -> TransportDelay_IWORK_dv . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dv . Tail , & _rtDW -> TransportDelay_IWORK_dv . Head ,
& _rtDW -> TransportDelay_IWORK_dv . Last , simTime - _rtP -> P_1467 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dv . Head ] = _rtB -> B_392_1792_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fg = _rtB -> B_392_1799_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lv . Head = ( ( _rtDW -> TransportDelay_IWORK_lv . Head
< ( _rtDW -> TransportDelay_IWORK_lv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lv . Head == _rtDW -> TransportDelay_IWORK_lv . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lv . Tail , & _rtDW -> TransportDelay_IWORK_lv . Head ,
& _rtDW -> TransportDelay_IWORK_lv . Last , simTime - _rtP -> P_1472 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lv . Head ] = _rtB -> B_392_1800_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bj = _rtB -> B_392_1807_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ae . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ae .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lu . Head = ( ( _rtDW -> TransportDelay_IWORK_lu . Head
< ( _rtDW -> TransportDelay_IWORK_lu . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lu . Head == _rtDW -> TransportDelay_IWORK_lu . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lu . Tail , & _rtDW -> TransportDelay_IWORK_lu . Head ,
& _rtDW -> TransportDelay_IWORK_lu . Last , simTime - _rtP -> P_1479 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lu . Head ] = _rtB -> B_392_1813_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_df = _rtB -> B_392_1820_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kz . Head = ( ( _rtDW -> TransportDelay_IWORK_kz . Head
< ( _rtDW -> TransportDelay_IWORK_kz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kz . Head == _rtDW -> TransportDelay_IWORK_kz . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kz . Tail , & _rtDW -> TransportDelay_IWORK_kz . Head ,
& _rtDW -> TransportDelay_IWORK_kz . Last , simTime - _rtP -> P_1484 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kz . Head ] = _rtB -> B_392_1821_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bv = _rtB -> B_392_1828_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oa . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ft . Head = ( ( _rtDW -> TransportDelay_IWORK_ft . Head
< ( _rtDW -> TransportDelay_IWORK_ft . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ft . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ft . Head == _rtDW -> TransportDelay_IWORK_ft . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ft . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ft . Tail , & _rtDW -> TransportDelay_IWORK_ft . Head ,
& _rtDW -> TransportDelay_IWORK_ft . Last , simTime - _rtP -> P_1491 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ft . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ft . Head ] = _rtB -> B_392_1834_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eq = _rtB -> B_392_1841_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fd . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bc . Head = ( ( _rtDW -> TransportDelay_IWORK_bc . Head
< ( _rtDW -> TransportDelay_IWORK_bc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bc . Head == _rtDW -> TransportDelay_IWORK_bc . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bc . Tail , & _rtDW -> TransportDelay_IWORK_bc . Head ,
& _rtDW -> TransportDelay_IWORK_bc . Last , simTime - _rtP -> P_1496 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bc . Head ] = _rtB -> B_392_1842_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_iw = _rtB -> B_392_1849_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ej . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ej .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_by . Head = ( ( _rtDW -> TransportDelay_IWORK_by . Head
< ( _rtDW -> TransportDelay_IWORK_by . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_by . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_by . Head == _rtDW -> TransportDelay_IWORK_by . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_by . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_by . Tail , & _rtDW -> TransportDelay_IWORK_by . Head ,
& _rtDW -> TransportDelay_IWORK_by . Last , simTime - _rtP -> P_1510 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_by . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_by . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_by . Head ] = _rtB -> B_392_1873_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fqc = _rtB -> B_392_1880_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fb . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fb .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nm . Head = ( ( _rtDW -> TransportDelay_IWORK_nm . Head
< ( _rtDW -> TransportDelay_IWORK_nm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nm . Head == _rtDW -> TransportDelay_IWORK_nm . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nm . Tail , & _rtDW -> TransportDelay_IWORK_nm . Head ,
& _rtDW -> TransportDelay_IWORK_nm . Last , simTime - _rtP -> P_1515 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nm . Head ] = _rtB -> B_392_1881_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bf = _rtB -> B_392_1888_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l . TUbufferPtrs
[ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_og .
Head = ( ( _rtDW -> TransportDelay_IWORK_og . Head < ( _rtDW ->
TransportDelay_IWORK_og . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_og . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_og . Head == _rtDW -> TransportDelay_IWORK_og . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_og . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_og . Tail , & _rtDW -> TransportDelay_IWORK_og . Head ,
& _rtDW -> TransportDelay_IWORK_og . Last , simTime - _rtP -> P_1522 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_og . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_og . Head ] = _rtB -> B_392_1894_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ib = _rtB -> B_392_1901_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_op . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_op .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he . Head = ( ( _rtDW -> TransportDelay_IWORK_he . Head
< ( _rtDW -> TransportDelay_IWORK_he . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_he . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he . Head == _rtDW -> TransportDelay_IWORK_he . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_he . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he . Tail , & _rtDW -> TransportDelay_IWORK_he . Head ,
& _rtDW -> TransportDelay_IWORK_he . Last , simTime - _rtP -> P_1527 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he . Head ] = _rtB -> B_392_1902_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ak = _rtB -> B_392_1909_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hl . Head = ( ( _rtDW -> TransportDelay_IWORK_hl . Head
< ( _rtDW -> TransportDelay_IWORK_hl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hl . Head == _rtDW -> TransportDelay_IWORK_hl . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hl . Tail , & _rtDW -> TransportDelay_IWORK_hl . Head ,
& _rtDW -> TransportDelay_IWORK_hl . Last , simTime - _rtP -> P_1534 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hl . Head ] = _rtB -> B_392_1915_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bu = _rtB -> B_392_1922_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j1 . Head = ( ( _rtDW -> TransportDelay_IWORK_j1 . Head
< ( _rtDW -> TransportDelay_IWORK_j1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j1 . Head == _rtDW -> TransportDelay_IWORK_j1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j1 . Tail , & _rtDW -> TransportDelay_IWORK_j1 . Head ,
& _rtDW -> TransportDelay_IWORK_j1 . Last , simTime - _rtP -> P_1539 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_j1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_j1 . Head ] = _rtB -> B_392_1923_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ih = _rtB -> B_392_1930_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mr . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2 . Head = ( ( _rtDW -> TransportDelay_IWORK_e2 . Head
< ( _rtDW -> TransportDelay_IWORK_e2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2 . Head == _rtDW -> TransportDelay_IWORK_e2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2 . Tail , & _rtDW -> TransportDelay_IWORK_e2 . Head ,
& _rtDW -> TransportDelay_IWORK_e2 . Last , simTime - _rtP -> P_1550 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2 . Head ] = _rtB -> B_392_1948_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_de = _rtB -> B_392_1955_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fdx .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_id . Head = ( ( _rtDW -> TransportDelay_IWORK_id . Head
< ( _rtDW -> TransportDelay_IWORK_id . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_id . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_id . Head == _rtDW -> TransportDelay_IWORK_id . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_id . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_id . Tail , & _rtDW -> TransportDelay_IWORK_id . Head ,
& _rtDW -> TransportDelay_IWORK_id . Last , simTime - _rtP -> P_1555 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_id . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_id . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_id . Head ] = _rtB -> B_392_1956_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h2 = _rtB -> B_392_1963_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gr . Head = ( ( _rtDW -> TransportDelay_IWORK_gr . Head
< ( _rtDW -> TransportDelay_IWORK_gr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gr . Head == _rtDW -> TransportDelay_IWORK_gr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gr . Tail , & _rtDW -> TransportDelay_IWORK_gr . Head ,
& _rtDW -> TransportDelay_IWORK_gr . Last , simTime - _rtP -> P_1562 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gr . Head ] = _rtB -> B_392_1969_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mb = _rtB -> B_392_1976_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nd . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dl . Head = ( ( _rtDW -> TransportDelay_IWORK_dl . Head
< ( _rtDW -> TransportDelay_IWORK_dl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dl . Head == _rtDW -> TransportDelay_IWORK_dl . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dl . Tail , & _rtDW -> TransportDelay_IWORK_dl . Head ,
& _rtDW -> TransportDelay_IWORK_dl . Last , simTime - _rtP -> P_1567 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dl . Head ] = _rtB -> B_392_1977_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bz = _rtB -> B_392_1984_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hx . Head = ( ( _rtDW -> TransportDelay_IWORK_hx . Head
< ( _rtDW -> TransportDelay_IWORK_hx . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hx . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hx . Head == _rtDW -> TransportDelay_IWORK_hx . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hx . Tail , & _rtDW -> TransportDelay_IWORK_hx . Head ,
& _rtDW -> TransportDelay_IWORK_hx . Last , simTime - _rtP -> P_1574 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hx . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hx . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hx . Head ] = _rtB -> B_392_1990_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eh = _rtB -> B_392_1997_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fw . Head = ( ( _rtDW -> TransportDelay_IWORK_fw . Head
< ( _rtDW -> TransportDelay_IWORK_fw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fw . Head == _rtDW -> TransportDelay_IWORK_fw . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fw . Tail , & _rtDW -> TransportDelay_IWORK_fw . Head ,
& _rtDW -> TransportDelay_IWORK_fw . Last , simTime - _rtP -> P_1579 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fw . Head ] = _rtB -> B_392_1998_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_n = _rtB -> B_392_2005_0 ; } if ( _rtDW -> TimeStampA_b
== ( rtInf ) ) { _rtDW -> TimeStampA_b = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_h ; } else if ( _rtDW -> TimeStampB_e == ( rtInf ) ) { _rtDW ->
TimeStampB_e = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_p ; } else if (
_rtDW -> TimeStampA_b < _rtDW -> TimeStampB_e ) { _rtDW -> TimeStampA_b =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_h ; } else { _rtDW ->
TimeStampB_e = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_p ; } * lastU =
_rtB -> B_392_2128_0 ; if ( _rtDW -> TimeStampA_i == ( rtInf ) ) { _rtDW ->
TimeStampA_i = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_p ; } else if (
_rtDW -> TimeStampB_m == ( rtInf ) ) { _rtDW -> TimeStampB_m = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_h ; } else if ( _rtDW -> TimeStampA_i < _rtDW
-> TimeStampB_m ) { _rtDW -> TimeStampA_i = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_p ; } else { _rtDW -> TimeStampB_m = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_h ; } * lastU = _rtB -> B_392_2140_0 ; if ( _rtDW ->
TimeStampA_h == ( rtInf ) ) { _rtDW -> TimeStampA_h = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_cg ; } else if ( _rtDW -> TimeStampB_id == ( rtInf )
) { _rtDW -> TimeStampB_id = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e
; } else if ( _rtDW -> TimeStampA_h < _rtDW -> TimeStampB_id ) { _rtDW ->
TimeStampA_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cg ; } else {
_rtDW -> TimeStampB_id = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e ; }
* lastU = _rtB -> B_392_2152_0 ; { real_T * * uBuffer = ( real_T * * ) &
_rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 0 ] ; real_T * * tBuffer =
( real_T * * ) & _rtDW -> TransportDelay_PWORK_ph . TUbufferPtrs [ 1 ] ;
real_T simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_be . Head = ( (
_rtDW -> TransportDelay_IWORK_be . Head < ( _rtDW -> TransportDelay_IWORK_be
. CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_be . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_be . Head == _rtDW ->
TransportDelay_IWORK_be . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_be . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_be . Tail , & _rtDW -> TransportDelay_IWORK_be . Head ,
& _rtDW -> TransportDelay_IWORK_be . Last , simTime - _rtP -> P_1723 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_be . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_be . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_be . Head ] = _rtB -> B_392_2171_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kb = _rtB -> B_392_2178_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_op . Head = ( ( _rtDW -> TransportDelay_IWORK_op . Head
< ( _rtDW -> TransportDelay_IWORK_op . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_op . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_op . Head == _rtDW -> TransportDelay_IWORK_op . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_op . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_op . Tail , & _rtDW -> TransportDelay_IWORK_op . Head ,
& _rtDW -> TransportDelay_IWORK_op . Last , simTime - _rtP -> P_1728 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_op . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_op . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_op . Head ] = _rtB -> B_392_2179_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hs = _rtB -> B_392_2186_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ma . Head = ( ( _rtDW -> TransportDelay_IWORK_ma . Head
< ( _rtDW -> TransportDelay_IWORK_ma . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ma . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ma . Head == _rtDW -> TransportDelay_IWORK_ma . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ma . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ma . Tail , & _rtDW -> TransportDelay_IWORK_ma . Head ,
& _rtDW -> TransportDelay_IWORK_ma . Last , simTime - _rtP -> P_1735 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ma . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma . Head ] = _rtB -> B_392_2192_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dv = _rtB -> B_392_2199_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m0c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ov . Head = ( ( _rtDW -> TransportDelay_IWORK_ov . Head
< ( _rtDW -> TransportDelay_IWORK_ov . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ov . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ov . Head == _rtDW -> TransportDelay_IWORK_ov . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ov . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ov . Tail , & _rtDW -> TransportDelay_IWORK_ov . Head ,
& _rtDW -> TransportDelay_IWORK_ov . Last , simTime - _rtP -> P_1740 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ov . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ov . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ov . Head ] = _rtB -> B_392_2200_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gg = _rtB -> B_392_2207_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_do . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_do .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hr . Head = ( ( _rtDW -> TransportDelay_IWORK_hr . Head
< ( _rtDW -> TransportDelay_IWORK_hr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hr . Head == _rtDW -> TransportDelay_IWORK_hr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hr . Tail , & _rtDW -> TransportDelay_IWORK_hr . Head ,
& _rtDW -> TransportDelay_IWORK_hr . Last , simTime - _rtP -> P_1747 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hr . Head ] = _rtB -> B_392_2213_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nb = _rtB -> B_392_2220_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_as . Head = ( ( _rtDW -> TransportDelay_IWORK_as . Head
< ( _rtDW -> TransportDelay_IWORK_as . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_as . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_as . Head == _rtDW -> TransportDelay_IWORK_as . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_as . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_as . Tail , & _rtDW -> TransportDelay_IWORK_as . Head ,
& _rtDW -> TransportDelay_IWORK_as . Last , simTime - _rtP -> P_1752 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_as . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_as . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_as . Head ] = _rtB -> B_392_2221_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l1 = _rtB -> B_392_2228_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_iw . Head = ( ( _rtDW -> TransportDelay_IWORK_iw . Head
< ( _rtDW -> TransportDelay_IWORK_iw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_iw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_iw . Head == _rtDW -> TransportDelay_IWORK_iw . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_iw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_iw . Tail , & _rtDW -> TransportDelay_IWORK_iw . Head ,
& _rtDW -> TransportDelay_IWORK_iw . Last , simTime - _rtP -> P_1762 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_iw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_iw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_iw . Head ] = _rtB -> B_392_2244_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ja = _rtB -> B_392_2251_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ai . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ai .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dq . Head = ( ( _rtDW -> TransportDelay_IWORK_dq . Head
< ( _rtDW -> TransportDelay_IWORK_dq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dq . Head == _rtDW -> TransportDelay_IWORK_dq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dq . Tail , & _rtDW -> TransportDelay_IWORK_dq . Head ,
& _rtDW -> TransportDelay_IWORK_dq . Last , simTime - _rtP -> P_1767 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dq . Head ] = _rtB -> B_392_2252_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gr = _rtB -> B_392_2259_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dr . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dr .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_os . Head = ( ( _rtDW -> TransportDelay_IWORK_os . Head
< ( _rtDW -> TransportDelay_IWORK_os . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_os . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_os . Head == _rtDW -> TransportDelay_IWORK_os . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_os . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_os . Tail , & _rtDW -> TransportDelay_IWORK_os . Head ,
& _rtDW -> TransportDelay_IWORK_os . Last , simTime - _rtP -> P_1774 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_os . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_os . Head ] = _rtB -> B_392_2265_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_c = _rtB -> B_392_2272_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pi . Head = ( ( _rtDW -> TransportDelay_IWORK_pi . Head
< ( _rtDW -> TransportDelay_IWORK_pi . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pi . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pi . Head == _rtDW -> TransportDelay_IWORK_pi . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pi . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pi . Tail , & _rtDW -> TransportDelay_IWORK_pi . Head ,
& _rtDW -> TransportDelay_IWORK_pi . Last , simTime - _rtP -> P_1779 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pi . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pi . Head ] = _rtB -> B_392_2273_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mt = _rtB -> B_392_2280_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jo . Head = ( ( _rtDW -> TransportDelay_IWORK_jo . Head
< ( _rtDW -> TransportDelay_IWORK_jo . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jo . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jo . Head == _rtDW -> TransportDelay_IWORK_jo . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jo . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jo . Tail , & _rtDW -> TransportDelay_IWORK_jo . Head ,
& _rtDW -> TransportDelay_IWORK_jo . Last , simTime - _rtP -> P_1786 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jo . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jo . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jo . Head ] = _rtB -> B_392_2286_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nr = _rtB -> B_392_2293_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_my . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_my .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i2 . Head = ( ( _rtDW -> TransportDelay_IWORK_i2 . Head
< ( _rtDW -> TransportDelay_IWORK_i2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_i2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i2 . Head == _rtDW -> TransportDelay_IWORK_i2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_i2 . Tail , & _rtDW -> TransportDelay_IWORK_i2 . Head ,
& _rtDW -> TransportDelay_IWORK_i2 . Last , simTime - _rtP -> P_1791 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_i2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2 . Head ] = _rtB -> B_392_2294_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jr = _rtB -> B_392_2301_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cct . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cct .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pd . Head = ( ( _rtDW -> TransportDelay_IWORK_pd . Head
< ( _rtDW -> TransportDelay_IWORK_pd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pd . Head == _rtDW -> TransportDelay_IWORK_pd . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pd . Tail , & _rtDW -> TransportDelay_IWORK_pd . Head ,
& _rtDW -> TransportDelay_IWORK_pd . Last , simTime - _rtP -> P_1805 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pd . Head ] = _rtB -> B_392_2325_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ex = _rtB -> B_392_2332_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fk . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_n2 . Head = ( ( _rtDW -> TransportDelay_IWORK_n2 . Head
< ( _rtDW -> TransportDelay_IWORK_n2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_n2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_n2 . Head == _rtDW -> TransportDelay_IWORK_n2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_n2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_n2 . Tail , & _rtDW -> TransportDelay_IWORK_n2 . Head ,
& _rtDW -> TransportDelay_IWORK_n2 . Last , simTime - _rtP -> P_1810 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_n2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_n2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_n2 . Head ] = _rtB -> B_392_2333_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_aa = _rtB -> B_392_2340_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_du . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_du .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lc . Head = ( ( _rtDW -> TransportDelay_IWORK_lc . Head
< ( _rtDW -> TransportDelay_IWORK_lc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lc . Head == _rtDW -> TransportDelay_IWORK_lc . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lc . Tail , & _rtDW -> TransportDelay_IWORK_lc . Head ,
& _rtDW -> TransportDelay_IWORK_lc . Last , simTime - _rtP -> P_1817 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lc . Head ] = _rtB -> B_392_2346_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bc = _rtB -> B_392_2353_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_el . Head = ( ( _rtDW -> TransportDelay_IWORK_el . Head
< ( _rtDW -> TransportDelay_IWORK_el . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_el . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_el . Head == _rtDW -> TransportDelay_IWORK_el . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_el . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_el . Tail , & _rtDW -> TransportDelay_IWORK_el . Head ,
& _rtDW -> TransportDelay_IWORK_el . Last , simTime - _rtP -> P_1822 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_el . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_el . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_el . Head ] = _rtB -> B_392_2354_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mo = _rtB -> B_392_2361_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ju . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ju .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jd . Head = ( ( _rtDW -> TransportDelay_IWORK_jd . Head
< ( _rtDW -> TransportDelay_IWORK_jd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jd . Head == _rtDW -> TransportDelay_IWORK_jd . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jd . Tail , & _rtDW -> TransportDelay_IWORK_jd . Head ,
& _rtDW -> TransportDelay_IWORK_jd . Last , simTime - _rtP -> P_1829 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jd . Head ] = _rtB -> B_392_2367_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ct = _rtB -> B_392_2374_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_na . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_na .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_md . Head = ( ( _rtDW -> TransportDelay_IWORK_md . Head
< ( _rtDW -> TransportDelay_IWORK_md . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_md . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_md . Head == _rtDW -> TransportDelay_IWORK_md . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_md . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_md . Tail , & _rtDW -> TransportDelay_IWORK_md . Head ,
& _rtDW -> TransportDelay_IWORK_md . Last , simTime - _rtP -> P_1834 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_md . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_md . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_md . Head ] = _rtB -> B_392_2375_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ad = _rtB -> B_392_2382_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fqe . Head = ( ( _rtDW -> TransportDelay_IWORK_fqe .
Head < ( _rtDW -> TransportDelay_IWORK_fqe . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_fqe . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fqe . Head == _rtDW -> TransportDelay_IWORK_fqe . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fqe . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fqe . Tail , & _rtDW -> TransportDelay_IWORK_fqe . Head
, & _rtDW -> TransportDelay_IWORK_fqe . Last , simTime - _rtP -> P_1845 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fqe . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fqe . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fqe . Head ] = _rtB -> B_392_2400_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ow = _rtB -> B_392_2407_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m2 . Head = ( ( _rtDW -> TransportDelay_IWORK_m2 . Head
< ( _rtDW -> TransportDelay_IWORK_m2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m2 . Head == _rtDW -> TransportDelay_IWORK_m2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m2 . Tail , & _rtDW -> TransportDelay_IWORK_m2 . Head ,
& _rtDW -> TransportDelay_IWORK_m2 . Last , simTime - _rtP -> P_1850 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m2 . Head ] = _rtB -> B_392_2408_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dp = _rtB -> B_392_2415_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ko . Head = ( ( _rtDW -> TransportDelay_IWORK_ko . Head
< ( _rtDW -> TransportDelay_IWORK_ko . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ko . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ko . Head == _rtDW -> TransportDelay_IWORK_ko . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ko . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ko . Tail , & _rtDW -> TransportDelay_IWORK_ko . Head ,
& _rtDW -> TransportDelay_IWORK_ko . Last , simTime - _rtP -> P_1857 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ko . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ko . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ko . Head ] = _rtB -> B_392_2421_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a0 = _rtB -> B_392_2428_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ld . Head = ( ( _rtDW -> TransportDelay_IWORK_ld . Head
< ( _rtDW -> TransportDelay_IWORK_ld . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ld . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ld . Head == _rtDW -> TransportDelay_IWORK_ld . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ld . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ld . Tail , & _rtDW -> TransportDelay_IWORK_ld . Head ,
& _rtDW -> TransportDelay_IWORK_ld . Last , simTime - _rtP -> P_1862 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ld . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ld . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ld . Head ] = _rtB -> B_392_2429_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lf = _rtB -> B_392_2436_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_h0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_al . Head = ( ( _rtDW -> TransportDelay_IWORK_al . Head
< ( _rtDW -> TransportDelay_IWORK_al . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_al . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_al . Head == _rtDW -> TransportDelay_IWORK_al . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_al . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_al . Tail , & _rtDW -> TransportDelay_IWORK_al . Head ,
& _rtDW -> TransportDelay_IWORK_al . Last , simTime - _rtP -> P_1869 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_al . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_al . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_al . Head ] = _rtB -> B_392_2442_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ce = _rtB -> B_392_2449_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jdp . Head = ( ( _rtDW -> TransportDelay_IWORK_jdp .
Head < ( _rtDW -> TransportDelay_IWORK_jdp . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_jdp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jdp . Head == _rtDW -> TransportDelay_IWORK_jdp . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jdp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jdp . Tail , & _rtDW -> TransportDelay_IWORK_jdp . Head
, & _rtDW -> TransportDelay_IWORK_jdp . Last , simTime - _rtP -> P_1874 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jdp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jdp . Head ] = _rtB -> B_392_2450_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ie = _rtB -> B_392_2457_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_grh . Head = ( ( _rtDW -> TransportDelay_IWORK_grh .
Head < ( _rtDW -> TransportDelay_IWORK_grh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_grh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_grh . Head == _rtDW -> TransportDelay_IWORK_grh . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_grh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_grh . Tail , & _rtDW -> TransportDelay_IWORK_grh . Head
, & _rtDW -> TransportDelay_IWORK_grh . Last , simTime - _rtP -> P_1991 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_grh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_grh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_grh . Head ] = _rtB -> B_392_2571_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fmc = _rtB -> B_392_2578_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b2 . Head = ( ( _rtDW -> TransportDelay_IWORK_b2 . Head
< ( _rtDW -> TransportDelay_IWORK_b2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b2 . Head == _rtDW -> TransportDelay_IWORK_b2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b2 . Tail , & _rtDW -> TransportDelay_IWORK_b2 . Head ,
& _rtDW -> TransportDelay_IWORK_b2 . Last , simTime - _rtP -> P_1996 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_b2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_b2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_b2 . Head ] = _rtB -> B_392_2579_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dm = _rtB -> B_392_2586_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2b . Head = ( ( _rtDW -> TransportDelay_IWORK_e2b .
Head < ( _rtDW -> TransportDelay_IWORK_e2b . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_e2b . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2b . Head == _rtDW -> TransportDelay_IWORK_e2b . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2b . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2b . Tail , & _rtDW -> TransportDelay_IWORK_e2b . Head
, & _rtDW -> TransportDelay_IWORK_e2b . Last , simTime - _rtP -> P_2003 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2b . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2b . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2b . Head ] = _rtB -> B_392_2592_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_js = _rtB -> B_392_2599_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ns . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ns .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mz . Head = ( ( _rtDW -> TransportDelay_IWORK_mz . Head
< ( _rtDW -> TransportDelay_IWORK_mz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mz . Head == _rtDW -> TransportDelay_IWORK_mz . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mz . Tail , & _rtDW -> TransportDelay_IWORK_mz . Head ,
& _rtDW -> TransportDelay_IWORK_mz . Last , simTime - _rtP -> P_2008 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mz . Head ] = _rtB -> B_392_2600_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o2 = _rtB -> B_392_2607_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_be . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_be .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fg . Head = ( ( _rtDW -> TransportDelay_IWORK_fg . Head
< ( _rtDW -> TransportDelay_IWORK_fg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fg . Head == _rtDW -> TransportDelay_IWORK_fg . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fg . Tail , & _rtDW -> TransportDelay_IWORK_fg . Head ,
& _rtDW -> TransportDelay_IWORK_fg . Last , simTime - _rtP -> P_2015 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fg . Head ] = _rtB -> B_392_2613_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_eq5 = _rtB -> B_392_2620_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_da . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_da .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ji . Head = ( ( _rtDW -> TransportDelay_IWORK_ji . Head
< ( _rtDW -> TransportDelay_IWORK_ji . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ji . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ji . Head == _rtDW -> TransportDelay_IWORK_ji . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ji . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ji . Tail , & _rtDW -> TransportDelay_IWORK_ji . Head ,
& _rtDW -> TransportDelay_IWORK_ji . Last , simTime - _rtP -> P_2020 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ji . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ji . Head ] = _rtB -> B_392_2621_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b1 = _rtB -> B_392_2628_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ln . Head = ( ( _rtDW -> TransportDelay_IWORK_ln . Head
< ( _rtDW -> TransportDelay_IWORK_ln . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ln . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ln . Head == _rtDW -> TransportDelay_IWORK_ln . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ln . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ln . Tail , & _rtDW -> TransportDelay_IWORK_ln . Head ,
& _rtDW -> TransportDelay_IWORK_ln . Last , simTime - _rtP -> P_2030 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ln . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ln . Head ] = _rtB -> B_392_2644_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pzf = _rtB -> B_392_2651_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_po . Head = ( ( _rtDW -> TransportDelay_IWORK_po . Head
< ( _rtDW -> TransportDelay_IWORK_po . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_po . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_po . Head == _rtDW -> TransportDelay_IWORK_po . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_po . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_po . Tail , & _rtDW -> TransportDelay_IWORK_po . Head ,
& _rtDW -> TransportDelay_IWORK_po . Last , simTime - _rtP -> P_2035 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_po . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_po . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_po . Head ] = _rtB -> B_392_2652_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fc = _rtB -> B_392_2659_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nl . Head = ( ( _rtDW -> TransportDelay_IWORK_nl . Head
< ( _rtDW -> TransportDelay_IWORK_nl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nl . Head == _rtDW -> TransportDelay_IWORK_nl . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nl . Tail , & _rtDW -> TransportDelay_IWORK_nl . Head ,
& _rtDW -> TransportDelay_IWORK_nl . Last , simTime - _rtP -> P_2042 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nl . Head ] = _rtB -> B_392_2665_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jd = _rtB -> B_392_2672_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_px . Head = ( ( _rtDW -> TransportDelay_IWORK_px . Head
< ( _rtDW -> TransportDelay_IWORK_px . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_px . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_px . Head == _rtDW -> TransportDelay_IWORK_px . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_px . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_px . Tail , & _rtDW -> TransportDelay_IWORK_px . Head ,
& _rtDW -> TransportDelay_IWORK_px . Last , simTime - _rtP -> P_2047 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_px . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_px . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_px . Head ] = _rtB -> B_392_2673_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mw = _rtB -> B_392_2680_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_io . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_io .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kx . Head = ( ( _rtDW -> TransportDelay_IWORK_kx . Head
< ( _rtDW -> TransportDelay_IWORK_kx . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kx . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kx . Head == _rtDW -> TransportDelay_IWORK_kx . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kx . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kx . Tail , & _rtDW -> TransportDelay_IWORK_kx . Head ,
& _rtDW -> TransportDelay_IWORK_kx . Last , simTime - _rtP -> P_2054 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kx . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kx . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kx . Head ] = _rtB -> B_392_2686_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_aa3 = _rtB -> B_392_2693_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ff . Head = ( ( _rtDW -> TransportDelay_IWORK_ff . Head
< ( _rtDW -> TransportDelay_IWORK_ff . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ff . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ff . Head == _rtDW -> TransportDelay_IWORK_ff . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ff . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ff . Tail , & _rtDW -> TransportDelay_IWORK_ff . Head ,
& _rtDW -> TransportDelay_IWORK_ff . Last , simTime - _rtP -> P_2059 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ff . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ff . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ff . Head ] = _rtB -> B_392_2694_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dj = _rtB -> B_392_2701_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lh . Head = ( ( _rtDW -> TransportDelay_IWORK_lh . Head
< ( _rtDW -> TransportDelay_IWORK_lh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lh . Head == _rtDW -> TransportDelay_IWORK_lh . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lh . Tail , & _rtDW -> TransportDelay_IWORK_lh . Head ,
& _rtDW -> TransportDelay_IWORK_lh . Last , simTime - _rtP -> P_2073 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lh . Head ] = _rtB -> B_392_2725_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jw = _rtB -> B_392_2732_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kt . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kt .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ip . Head = ( ( _rtDW -> TransportDelay_IWORK_ip . Head
< ( _rtDW -> TransportDelay_IWORK_ip . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ip . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ip . Head == _rtDW -> TransportDelay_IWORK_ip . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ip . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ip . Tail , & _rtDW -> TransportDelay_IWORK_ip . Head ,
& _rtDW -> TransportDelay_IWORK_ip . Last , simTime - _rtP -> P_2078 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ip . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ip . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ip . Head ] = _rtB -> B_392_2733_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_b2 = _rtB -> B_392_2740_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oe . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oe .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_of . Head = ( ( _rtDW -> TransportDelay_IWORK_of . Head
< ( _rtDW -> TransportDelay_IWORK_of . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_of . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_of . Head == _rtDW -> TransportDelay_IWORK_of . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_of . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_of . Tail , & _rtDW -> TransportDelay_IWORK_of . Head ,
& _rtDW -> TransportDelay_IWORK_of . Last , simTime - _rtP -> P_2085 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_of . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_of . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_of . Head ] = _rtB -> B_392_2746_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fw = _rtB -> B_392_2753_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lk . Head = ( ( _rtDW -> TransportDelay_IWORK_lk . Head
< ( _rtDW -> TransportDelay_IWORK_lk . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lk . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lk . Head == _rtDW -> TransportDelay_IWORK_lk . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lk . Tail , & _rtDW -> TransportDelay_IWORK_lk . Head ,
& _rtDW -> TransportDelay_IWORK_lk . Last , simTime - _rtP -> P_2090 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lk . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lk . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lk . Head ] = _rtB -> B_392_2754_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ml = _rtB -> B_392_2761_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pj . Head = ( ( _rtDW -> TransportDelay_IWORK_pj . Head
< ( _rtDW -> TransportDelay_IWORK_pj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pj . Head == _rtDW -> TransportDelay_IWORK_pj . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pj . Tail , & _rtDW -> TransportDelay_IWORK_pj . Head ,
& _rtDW -> TransportDelay_IWORK_pj . Last , simTime - _rtP -> P_2097 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pj . Head ] = _rtB -> B_392_2767_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_n1 = _rtB -> B_392_2774_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_em . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_em .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l5 . Head = ( ( _rtDW -> TransportDelay_IWORK_l5 . Head
< ( _rtDW -> TransportDelay_IWORK_l5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l5 . Head == _rtDW -> TransportDelay_IWORK_l5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l5 . Tail , & _rtDW -> TransportDelay_IWORK_l5 . Head ,
& _rtDW -> TransportDelay_IWORK_l5 . Last , simTime - _rtP -> P_2102 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l5 . Head ] = _rtB -> B_392_2775_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i5 = _rtB -> B_392_2782_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ki . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ki .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jy . Head = ( ( _rtDW -> TransportDelay_IWORK_jy . Head
< ( _rtDW -> TransportDelay_IWORK_jy . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jy . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jy . Head == _rtDW -> TransportDelay_IWORK_jy . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jy . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jy . Tail , & _rtDW -> TransportDelay_IWORK_jy . Head ,
& _rtDW -> TransportDelay_IWORK_jy . Last , simTime - _rtP -> P_2113 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jy . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jy . Head ] = _rtB -> B_392_2800_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hz = _rtB -> B_392_2807_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ev . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ev .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ly . Head = ( ( _rtDW -> TransportDelay_IWORK_ly . Head
< ( _rtDW -> TransportDelay_IWORK_ly . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ly . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ly . Head == _rtDW -> TransportDelay_IWORK_ly . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ly . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ly . Tail , & _rtDW -> TransportDelay_IWORK_ly . Head ,
& _rtDW -> TransportDelay_IWORK_ly . Last , simTime - _rtP -> P_2118 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ly . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ly . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ly . Head ] = _rtB -> B_392_2808_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lk = _rtB -> B_392_2815_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oeh . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oeh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oq . Head = ( ( _rtDW -> TransportDelay_IWORK_oq . Head
< ( _rtDW -> TransportDelay_IWORK_oq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oq . Head == _rtDW -> TransportDelay_IWORK_oq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oq . Tail , & _rtDW -> TransportDelay_IWORK_oq . Head ,
& _rtDW -> TransportDelay_IWORK_oq . Last , simTime - _rtP -> P_2125 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oq . Head ] = _rtB -> B_392_2821_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gj = _rtB -> B_392_2828_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pr . Head = ( ( _rtDW -> TransportDelay_IWORK_pr . Head
< ( _rtDW -> TransportDelay_IWORK_pr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pr . Head == _rtDW -> TransportDelay_IWORK_pr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pr . Tail , & _rtDW -> TransportDelay_IWORK_pr . Head ,
& _rtDW -> TransportDelay_IWORK_pr . Last , simTime - _rtP -> P_2130 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pr . Head ] = _rtB -> B_392_2829_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gl = _rtB -> B_392_2836_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nt . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nt .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kj . Head = ( ( _rtDW -> TransportDelay_IWORK_kj . Head
< ( _rtDW -> TransportDelay_IWORK_kj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kj . Head == _rtDW -> TransportDelay_IWORK_kj . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kj . Tail , & _rtDW -> TransportDelay_IWORK_kj . Head ,
& _rtDW -> TransportDelay_IWORK_kj . Last , simTime - _rtP -> P_2137 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kj . Head ] = _rtB -> B_392_2842_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pd = _rtB -> B_392_2849_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pn . Head = ( ( _rtDW -> TransportDelay_IWORK_pn . Head
< ( _rtDW -> TransportDelay_IWORK_pn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pn . Head == _rtDW -> TransportDelay_IWORK_pn . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pn . Tail , & _rtDW -> TransportDelay_IWORK_pn . Head ,
& _rtDW -> TransportDelay_IWORK_pn . Last , simTime - _rtP -> P_2142 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pn . Head ] = _rtB -> B_392_2850_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ey = _rtB -> B_392_2857_0 ; } if ( _rtDW ->
TimeStampA_nv == ( rtInf ) ) { _rtDW -> TimeStampA_nv = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_d ; } else if ( _rtDW -> TimeStampB_g == ( rtInf )
) { _rtDW -> TimeStampB_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_dr
; } else if ( _rtDW -> TimeStampA_nv < _rtDW -> TimeStampB_g ) { _rtDW ->
TimeStampA_nv = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_d ; } else {
_rtDW -> TimeStampB_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_dr ; }
* lastU = _rtB -> B_392_2980_0 ; if ( _rtDW -> TimeStampA_l == ( rtInf ) ) {
_rtDW -> TimeStampA_l = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_ca ; }
else if ( _rtDW -> TimeStampB_ic == ( rtInf ) ) { _rtDW -> TimeStampB_ic =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_b ; } else if ( _rtDW ->
TimeStampA_l < _rtDW -> TimeStampB_ic ) { _rtDW -> TimeStampA_l = ssGetT ( S
) ; lastU = & _rtDW -> LastUAtTimeA_ca ; } else { _rtDW -> TimeStampB_ic =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_b ; } * lastU = _rtB ->
B_392_2992_0 ; if ( _rtDW -> TimeStampA_m == ( rtInf ) ) { _rtDW ->
TimeStampA_m = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_e ; } else if (
_rtDW -> TimeStampB_ka == ( rtInf ) ) { _rtDW -> TimeStampB_ka = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeB_c ; } else if ( _rtDW -> TimeStampA_m <
_rtDW -> TimeStampB_ka ) { _rtDW -> TimeStampA_m = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeA_e ; } else { _rtDW -> TimeStampB_ka = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_c ; } * lastU = _rtB -> B_392_3004_0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dt .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_dt . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_ea5 . Head = ( ( _rtDW ->
TransportDelay_IWORK_ea5 . Head < ( _rtDW -> TransportDelay_IWORK_ea5 .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ea5 . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_ea5 . Head == _rtDW ->
TransportDelay_IWORK_ea5 . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ea5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ea5 . Tail , & _rtDW -> TransportDelay_IWORK_ea5 . Head
, & _rtDW -> TransportDelay_IWORK_ea5 . Last , simTime - _rtP -> P_2286 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ea5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ea5 . Head ] = _rtB -> B_392_3023_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o4 = _rtB -> B_392_3030_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mwl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eg . Head = ( ( _rtDW -> TransportDelay_IWORK_eg . Head
< ( _rtDW -> TransportDelay_IWORK_eg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eg . Head == _rtDW -> TransportDelay_IWORK_eg . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eg . Tail , & _rtDW -> TransportDelay_IWORK_eg . Head ,
& _rtDW -> TransportDelay_IWORK_eg . Last , simTime - _rtP -> P_2291 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eg . Head ] = _rtB -> B_392_3031_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nx = _rtB -> B_392_3038_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_br . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_br .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kk . Head = ( ( _rtDW -> TransportDelay_IWORK_kk . Head
< ( _rtDW -> TransportDelay_IWORK_kk . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kk . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kk . Head == _rtDW -> TransportDelay_IWORK_kk . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kk . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kk . Tail , & _rtDW -> TransportDelay_IWORK_kk . Head ,
& _rtDW -> TransportDelay_IWORK_kk . Last , simTime - _rtP -> P_2298 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kk . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kk . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kk . Head ] = _rtB -> B_392_3044_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_mq = _rtB -> B_392_3051_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jp . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jp .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he5 . Head = ( ( _rtDW -> TransportDelay_IWORK_he5 .
Head < ( _rtDW -> TransportDelay_IWORK_he5 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_he5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he5 . Head == _rtDW -> TransportDelay_IWORK_he5 . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_he5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he5 . Tail , & _rtDW -> TransportDelay_IWORK_he5 . Head
, & _rtDW -> TransportDelay_IWORK_he5 . Last , simTime - _rtP -> P_2303 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5 . Head ] = _rtB -> B_392_3052_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bq = _rtB -> B_392_3059_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dp . Head = ( ( _rtDW -> TransportDelay_IWORK_dp . Head
< ( _rtDW -> TransportDelay_IWORK_dp . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dp . Head == _rtDW -> TransportDelay_IWORK_dp . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dp . Tail , & _rtDW -> TransportDelay_IWORK_dp . Head ,
& _rtDW -> TransportDelay_IWORK_dp . Last , simTime - _rtP -> P_2310 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dp . Head ] = _rtB -> B_392_3065_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ep = _rtB -> B_392_3072_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ac . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ac .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_la . Head = ( ( _rtDW -> TransportDelay_IWORK_la . Head
< ( _rtDW -> TransportDelay_IWORK_la . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_la . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_la . Head == _rtDW -> TransportDelay_IWORK_la . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_la . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_la . Tail , & _rtDW -> TransportDelay_IWORK_la . Head ,
& _rtDW -> TransportDelay_IWORK_la . Last , simTime - _rtP -> P_2315 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_la . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_la . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_la . Head ] = _rtB -> B_392_3073_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ms = _rtB -> B_392_3080_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ev . Head = ( ( _rtDW -> TransportDelay_IWORK_ev . Head
< ( _rtDW -> TransportDelay_IWORK_ev . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ev . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ev . Head == _rtDW -> TransportDelay_IWORK_ev . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ev . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ev . Tail , & _rtDW -> TransportDelay_IWORK_ev . Head ,
& _rtDW -> TransportDelay_IWORK_ev . Last , simTime - _rtP -> P_2325 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ev . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ev . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ev . Head ] = _rtB -> B_392_3096_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dw = _rtB -> B_392_3103_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gl . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_b1 . Head = ( ( _rtDW -> TransportDelay_IWORK_b1 . Head
< ( _rtDW -> TransportDelay_IWORK_b1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_b1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_b1 . Head == _rtDW -> TransportDelay_IWORK_b1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_b1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_b1 . Tail , & _rtDW -> TransportDelay_IWORK_b1 . Head ,
& _rtDW -> TransportDelay_IWORK_b1 . Last , simTime - _rtP -> P_2330 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_b1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_b1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_b1 . Head ] = _rtB -> B_392_3104_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jh = _rtB -> B_392_3111_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nq . Head = ( ( _rtDW -> TransportDelay_IWORK_nq . Head
< ( _rtDW -> TransportDelay_IWORK_nq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nq . Head == _rtDW -> TransportDelay_IWORK_nq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nq . Tail , & _rtDW -> TransportDelay_IWORK_nq . Head ,
& _rtDW -> TransportDelay_IWORK_nq . Last , simTime - _rtP -> P_2337 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nq . Head ] = _rtB -> B_392_3117_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fx = _rtB -> B_392_3124_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ag . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ag .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_he5l . Head = ( ( _rtDW -> TransportDelay_IWORK_he5l .
Head < ( _rtDW -> TransportDelay_IWORK_he5l . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_he5l . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_he5l . Head == _rtDW -> TransportDelay_IWORK_he5l . Tail
) { if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW
-> TransportDelay_IWORK_he5l . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_he5l . Tail , & _rtDW -> TransportDelay_IWORK_he5l .
Head , & _rtDW -> TransportDelay_IWORK_he5l . Last , simTime - _rtP -> P_2342
, tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_he5l . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5l . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_he5l . Head ] = _rtB -> B_392_3125_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a2 = _rtB -> B_392_3132_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_jg . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_jg .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p4 . Head = ( ( _rtDW -> TransportDelay_IWORK_p4 . Head
< ( _rtDW -> TransportDelay_IWORK_p4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p4 . Head == _rtDW -> TransportDelay_IWORK_p4 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4 . Tail , & _rtDW -> TransportDelay_IWORK_p4 . Head ,
& _rtDW -> TransportDelay_IWORK_p4 . Last , simTime - _rtP -> P_2349 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4 . Head ] = _rtB -> B_392_3138_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cs = _rtB -> B_392_3145_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_og . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_og .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kjd . Head = ( ( _rtDW -> TransportDelay_IWORK_kjd .
Head < ( _rtDW -> TransportDelay_IWORK_kjd . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_kjd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kjd . Head == _rtDW -> TransportDelay_IWORK_kjd . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kjd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kjd . Tail , & _rtDW -> TransportDelay_IWORK_kjd . Head
, & _rtDW -> TransportDelay_IWORK_kjd . Last , simTime - _rtP -> P_2354 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kjd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kjd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kjd . Head ] = _rtB -> B_392_3146_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fi = _rtB -> B_392_3153_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cv . Head = ( ( _rtDW -> TransportDelay_IWORK_cv . Head
< ( _rtDW -> TransportDelay_IWORK_cv . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cv . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cv . Head == _rtDW -> TransportDelay_IWORK_cv . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cv . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cv . Tail , & _rtDW -> TransportDelay_IWORK_cv . Head ,
& _rtDW -> TransportDelay_IWORK_cv . Last , simTime - _rtP -> P_2368 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cv . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cv . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cv . Head ] = _rtB -> B_392_3177_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m3 = _rtB -> B_392_3184_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p4s . Head = ( ( _rtDW -> TransportDelay_IWORK_p4s .
Head < ( _rtDW -> TransportDelay_IWORK_p4s . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_p4s . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p4s . Head == _rtDW -> TransportDelay_IWORK_p4s . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p4s . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p4s . Tail , & _rtDW -> TransportDelay_IWORK_p4s . Head
, & _rtDW -> TransportDelay_IWORK_p4s . Last , simTime - _rtP -> P_2373 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p4s . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4s . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p4s . Head ] = _rtB -> B_392_3185_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pu = _rtB -> B_392_3192_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_i0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ce . Head = ( ( _rtDW -> TransportDelay_IWORK_ce . Head
< ( _rtDW -> TransportDelay_IWORK_ce . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ce . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ce . Head == _rtDW -> TransportDelay_IWORK_ce . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ce . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ce . Tail , & _rtDW -> TransportDelay_IWORK_ce . Head ,
& _rtDW -> TransportDelay_IWORK_ce . Last , simTime - _rtP -> P_2380 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ce . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ce . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ce . Head ] = _rtB -> B_392_3198_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_oz = _rtB -> B_392_3205_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ck . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ck .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_cc . Head = ( ( _rtDW -> TransportDelay_IWORK_cc . Head
< ( _rtDW -> TransportDelay_IWORK_cc . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_cc . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_cc . Head == _rtDW -> TransportDelay_IWORK_cc . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_cc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_cc . Tail , & _rtDW -> TransportDelay_IWORK_cc . Head ,
& _rtDW -> TransportDelay_IWORK_cc . Last , simTime - _rtP -> P_2385 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_cc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_cc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_cc . Head ] = _rtB -> B_392_3206_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_hjp = _rtB -> B_392_3213_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ni . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ni .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e2t . Head = ( ( _rtDW -> TransportDelay_IWORK_e2t .
Head < ( _rtDW -> TransportDelay_IWORK_e2t . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_e2t . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e2t . Head == _rtDW -> TransportDelay_IWORK_e2t . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e2t . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e2t . Tail , & _rtDW -> TransportDelay_IWORK_e2t . Head
, & _rtDW -> TransportDelay_IWORK_e2t . Last , simTime - _rtP -> P_2392 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e2t . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2t . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e2t . Head ] = _rtB -> B_392_3219_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kt = _rtB -> B_392_3226_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_lu . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_lu .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ex . Head = ( ( _rtDW -> TransportDelay_IWORK_ex . Head
< ( _rtDW -> TransportDelay_IWORK_ex . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ex . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ex . Head == _rtDW -> TransportDelay_IWORK_ex . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ex . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ex . Tail , & _rtDW -> TransportDelay_IWORK_ex . Head ,
& _rtDW -> TransportDelay_IWORK_ex . Last , simTime - _rtP -> P_2397 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ex . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ex . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ex . Head ] = _rtB -> B_392_3227_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kk = _rtB -> B_392_3234_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p5 . Head = ( ( _rtDW -> TransportDelay_IWORK_p5 . Head
< ( _rtDW -> TransportDelay_IWORK_p5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p5 . Head == _rtDW -> TransportDelay_IWORK_p5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p5 . Tail , & _rtDW -> TransportDelay_IWORK_p5 . Head ,
& _rtDW -> TransportDelay_IWORK_p5 . Last , simTime - _rtP -> P_2408 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p5 . Head ] = _rtB -> B_392_3252_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g3 = _rtB -> B_392_3259_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ld . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ld .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eu . Head = ( ( _rtDW -> TransportDelay_IWORK_eu . Head
< ( _rtDW -> TransportDelay_IWORK_eu . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eu . Head == _rtDW -> TransportDelay_IWORK_eu . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eu . Tail , & _rtDW -> TransportDelay_IWORK_eu . Head ,
& _rtDW -> TransportDelay_IWORK_eu . Last , simTime - _rtP -> P_2413 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eu . Head ] = _rtB -> B_392_3260_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o3 = _rtB -> B_392_3267_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kv . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kv .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_i2y . Head = ( ( _rtDW -> TransportDelay_IWORK_i2y .
Head < ( _rtDW -> TransportDelay_IWORK_i2y . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_i2y . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_i2y . Head == _rtDW -> TransportDelay_IWORK_i2y . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_i2y . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_i2y . Tail , & _rtDW -> TransportDelay_IWORK_i2y . Head
, & _rtDW -> TransportDelay_IWORK_i2y . Last , simTime - _rtP -> P_2420 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_i2y . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2y . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_i2y . Head ] = _rtB -> B_392_3273_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dl = _rtB -> B_392_3280_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ep . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ep .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lna . Head = ( ( _rtDW -> TransportDelay_IWORK_lna .
Head < ( _rtDW -> TransportDelay_IWORK_lna . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_lna . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lna . Head == _rtDW -> TransportDelay_IWORK_lna . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lna . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lna . Tail , & _rtDW -> TransportDelay_IWORK_lna . Head
, & _rtDW -> TransportDelay_IWORK_lna . Last , simTime - _rtP -> P_2425 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lna . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lna . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lna . Head ] = _rtB -> B_392_3281_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_by = _rtB -> B_392_3288_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ln . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ln .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kw . Head = ( ( _rtDW -> TransportDelay_IWORK_kw . Head
< ( _rtDW -> TransportDelay_IWORK_kw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kw . Head == _rtDW -> TransportDelay_IWORK_kw . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kw . Tail , & _rtDW -> TransportDelay_IWORK_kw . Head ,
& _rtDW -> TransportDelay_IWORK_kw . Last , simTime - _rtP -> P_2432 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kw . Head ] = _rtB -> B_392_3294_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_g3o = _rtB -> B_392_3301_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_eo . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_eo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bf . Head = ( ( _rtDW -> TransportDelay_IWORK_bf . Head
< ( _rtDW -> TransportDelay_IWORK_bf . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bf . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bf . Head == _rtDW -> TransportDelay_IWORK_bf . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bf . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bf . Tail , & _rtDW -> TransportDelay_IWORK_bf . Head ,
& _rtDW -> TransportDelay_IWORK_bf . Last , simTime - _rtP -> P_2437 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bf . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bf . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bf . Head ] = _rtB -> B_392_3302_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m1 = _rtB -> B_392_3309_0 ; } if ( _rtDW ->
TimeStampA_mm == ( rtInf ) ) { _rtDW -> TimeStampA_mm = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_ed ; } else if ( _rtDW -> TimeStampB_eo == ( rtInf
) ) { _rtDW -> TimeStampB_eo = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_i ; } else if ( _rtDW -> TimeStampA_mm < _rtDW -> TimeStampB_eo
) { _rtDW -> TimeStampA_mm = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_ed ; } else { _rtDW -> TimeStampB_eo = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_i ; } * lastU = _rtB -> B_392_3432_0 ; if ( _rtDW ->
TimeStampA_j == ( rtInf ) ) { _rtDW -> TimeStampA_j = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_dt ; } else if ( _rtDW -> TimeStampB_n == ( rtInf ) )
{ _rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_po ;
} else if ( _rtDW -> TimeStampA_j < _rtDW -> TimeStampB_n ) { _rtDW ->
TimeStampA_j = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_dt ; } else {
_rtDW -> TimeStampB_n = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_po ; }
* lastU = _rtB -> B_392_3444_0 ; if ( _rtDW -> TimeStampA_g == ( rtInf ) ) {
_rtDW -> TimeStampA_g = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_g ; }
else if ( _rtDW -> TimeStampB_d == ( rtInf ) ) { _rtDW -> TimeStampB_d =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_mm ; } else if ( _rtDW ->
TimeStampA_g < _rtDW -> TimeStampB_d ) { _rtDW -> TimeStampA_g = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_g ; } else { _rtDW -> TimeStampB_d = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB_mm ; } * lastU = _rtB -> B_392_3456_0
; { real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ca .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_ca . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_kd . Head = ( ( _rtDW ->
TransportDelay_IWORK_kd . Head < ( _rtDW -> TransportDelay_IWORK_kd .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_kd . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_kd . Head == _rtDW ->
TransportDelay_IWORK_kd . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kd . Tail , & _rtDW -> TransportDelay_IWORK_kd . Head ,
& _rtDW -> TransportDelay_IWORK_kd . Last , simTime - _rtP -> P_2581 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kd . Head ] = _rtB -> B_392_3475_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ev = _rtB -> B_392_3482_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_in . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_in .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mm . Head = ( ( _rtDW -> TransportDelay_IWORK_mm . Head
< ( _rtDW -> TransportDelay_IWORK_mm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mm . Head == _rtDW -> TransportDelay_IWORK_mm . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mm . Tail , & _rtDW -> TransportDelay_IWORK_mm . Head ,
& _rtDW -> TransportDelay_IWORK_mm . Last , simTime - _rtP -> P_2586 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mm . Head ] = _rtB -> B_392_3483_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fis = _rtB -> B_392_3490_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ec . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ec .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gn . Head = ( ( _rtDW -> TransportDelay_IWORK_gn . Head
< ( _rtDW -> TransportDelay_IWORK_gn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gn . Head == _rtDW -> TransportDelay_IWORK_gn . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gn . Tail , & _rtDW -> TransportDelay_IWORK_gn . Head ,
& _rtDW -> TransportDelay_IWORK_gn . Last , simTime - _rtP -> P_2593 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gn . Head ] = _rtB -> B_392_3496_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ij = _rtB -> B_392_3503_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_fd . Head = ( ( _rtDW -> TransportDelay_IWORK_fd . Head
< ( _rtDW -> TransportDelay_IWORK_fd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_fd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_fd . Head == _rtDW -> TransportDelay_IWORK_fd . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fd . Tail , & _rtDW -> TransportDelay_IWORK_fd . Head ,
& _rtDW -> TransportDelay_IWORK_fd . Last , simTime - _rtP -> P_2598 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fd . Head ] = _rtB -> B_392_3504_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jy = _rtB -> B_392_3511_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_d53 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_d53 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gz . Head = ( ( _rtDW -> TransportDelay_IWORK_gz . Head
< ( _rtDW -> TransportDelay_IWORK_gz . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gz . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gz . Head == _rtDW -> TransportDelay_IWORK_gz . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gz . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gz . Tail , & _rtDW -> TransportDelay_IWORK_gz . Head ,
& _rtDW -> TransportDelay_IWORK_gz . Last , simTime - _rtP -> P_2605 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gz . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gz . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gz . Head ] = _rtB -> B_392_3517_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_grf = _rtB -> B_392_3524_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ee . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ee .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ma4 . Head = ( ( _rtDW -> TransportDelay_IWORK_ma4 .
Head < ( _rtDW -> TransportDelay_IWORK_ma4 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ma4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ma4 . Head == _rtDW -> TransportDelay_IWORK_ma4 . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ma4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ma4 . Tail , & _rtDW -> TransportDelay_IWORK_ma4 . Head
, & _rtDW -> TransportDelay_IWORK_ma4 . Last , simTime - _rtP -> P_2610 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ma4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ma4 . Head ] = _rtB -> B_392_3525_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pe = _rtB -> B_392_3532_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kl . Head = ( ( _rtDW -> TransportDelay_IWORK_kl . Head
< ( _rtDW -> TransportDelay_IWORK_kl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kl . Head == _rtDW -> TransportDelay_IWORK_kl . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kl . Tail , & _rtDW -> TransportDelay_IWORK_kl . Head ,
& _rtDW -> TransportDelay_IWORK_kl . Last , simTime - _rtP -> P_2620 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kl . Head ] = _rtB -> B_392_3548_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ic = _rtB -> B_392_3555_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_er . Head = ( ( _rtDW -> TransportDelay_IWORK_er . Head
< ( _rtDW -> TransportDelay_IWORK_er . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_er . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_er . Head == _rtDW -> TransportDelay_IWORK_er . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_er . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_er . Tail , & _rtDW -> TransportDelay_IWORK_er . Head ,
& _rtDW -> TransportDelay_IWORK_er . Last , simTime - _rtP -> P_2625 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_er . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_er . Head ] = _rtB -> B_392_3556_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_k0 = _rtB -> B_392_3563_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fc . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fc .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hq . Head = ( ( _rtDW -> TransportDelay_IWORK_hq . Head
< ( _rtDW -> TransportDelay_IWORK_hq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_hq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hq . Head == _rtDW -> TransportDelay_IWORK_hq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hq . Tail , & _rtDW -> TransportDelay_IWORK_hq . Head ,
& _rtDW -> TransportDelay_IWORK_hq . Last , simTime - _rtP -> P_2632 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hq . Head ] = _rtB -> B_392_3569_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ofm = _rtB -> B_392_3576_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj2 . Head = ( ( _rtDW -> TransportDelay_IWORK_hj2 .
Head < ( _rtDW -> TransportDelay_IWORK_hj2 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hj2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj2 . Head == _rtDW -> TransportDelay_IWORK_hj2 . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj2 . Tail , & _rtDW -> TransportDelay_IWORK_hj2 . Head
, & _rtDW -> TransportDelay_IWORK_hj2 . Last , simTime - _rtP -> P_2637 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj2 . Head ] = _rtB -> B_392_3577_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ico = _rtB -> B_392_3584_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_l5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_l5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_atb . Head = ( ( _rtDW -> TransportDelay_IWORK_atb .
Head < ( _rtDW -> TransportDelay_IWORK_atb . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_atb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_atb . Head == _rtDW -> TransportDelay_IWORK_atb . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_atb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_atb . Tail , & _rtDW -> TransportDelay_IWORK_atb . Head
, & _rtDW -> TransportDelay_IWORK_atb . Last , simTime - _rtP -> P_2644 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_atb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_atb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_atb . Head ] = _rtB -> B_392_3590_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ji = _rtB -> B_392_3597_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_or . Head = ( ( _rtDW -> TransportDelay_IWORK_or . Head
< ( _rtDW -> TransportDelay_IWORK_or . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_or . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_or . Head == _rtDW -> TransportDelay_IWORK_or . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_or . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_or . Tail , & _rtDW -> TransportDelay_IWORK_or . Head ,
& _rtDW -> TransportDelay_IWORK_or . Last , simTime - _rtP -> P_2649 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_or . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_or . Head ] = _rtB -> B_392_3598_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nm = _rtB -> B_392_3605_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cmj . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cmj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dw . Head = ( ( _rtDW -> TransportDelay_IWORK_dw . Head
< ( _rtDW -> TransportDelay_IWORK_dw . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dw . Head == _rtDW -> TransportDelay_IWORK_dw . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dw . Tail , & _rtDW -> TransportDelay_IWORK_dw . Head ,
& _rtDW -> TransportDelay_IWORK_dw . Last , simTime - _rtP -> P_2663 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dw . Head ] = _rtB -> B_392_3629_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jwz = _rtB -> B_392_3636_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ojz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_c5 . Head = ( ( _rtDW -> TransportDelay_IWORK_c5 . Head
< ( _rtDW -> TransportDelay_IWORK_c5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_c5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_c5 . Head == _rtDW -> TransportDelay_IWORK_c5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_c5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_c5 . Tail , & _rtDW -> TransportDelay_IWORK_c5 . Head ,
& _rtDW -> TransportDelay_IWORK_c5 . Last , simTime - _rtP -> P_2668 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_c5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_c5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_c5 . Head ] = _rtB -> B_392_3637_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lb = _rtB -> B_392_3644_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5n .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hj0 . Head = ( ( _rtDW -> TransportDelay_IWORK_hj0 .
Head < ( _rtDW -> TransportDelay_IWORK_hj0 . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hj0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hj0 . Head == _rtDW -> TransportDelay_IWORK_hj0 . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hj0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hj0 . Tail , & _rtDW -> TransportDelay_IWORK_hj0 . Head
, & _rtDW -> TransportDelay_IWORK_hj0 . Last , simTime - _rtP -> P_2675 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hj0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hj0 . Head ] = _rtB -> B_392_3650_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ao = _rtB -> B_392_3657_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cvh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_da . Head = ( ( _rtDW -> TransportDelay_IWORK_da . Head
< ( _rtDW -> TransportDelay_IWORK_da . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_da . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_da . Head == _rtDW -> TransportDelay_IWORK_da . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_da . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_da . Tail , & _rtDW -> TransportDelay_IWORK_da . Head ,
& _rtDW -> TransportDelay_IWORK_da . Last , simTime - _rtP -> P_2680 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_da . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_da . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_da . Head ] = _rtB -> B_392_3658_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kf = _rtB -> B_392_3665_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ig . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ig .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_j0 . Head = ( ( _rtDW -> TransportDelay_IWORK_j0 . Head
< ( _rtDW -> TransportDelay_IWORK_j0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_j0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_j0 . Head == _rtDW -> TransportDelay_IWORK_j0 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_j0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_j0 . Tail , & _rtDW -> TransportDelay_IWORK_j0 . Head ,
& _rtDW -> TransportDelay_IWORK_j0 . Last , simTime - _rtP -> P_2687 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_j0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_j0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_j0 . Head ] = _rtB -> B_392_3671_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pzv = _rtB -> B_392_3678_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_gf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_gf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nd . Head = ( ( _rtDW -> TransportDelay_IWORK_nd . Head
< ( _rtDW -> TransportDelay_IWORK_nd . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nd . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nd . Head == _rtDW -> TransportDelay_IWORK_nd . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nd . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nd . Tail , & _rtDW -> TransportDelay_IWORK_nd . Head ,
& _rtDW -> TransportDelay_IWORK_nd . Last , simTime - _rtP -> P_2692 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nd . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nd . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nd . Head ] = _rtB -> B_392_3679_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kv = _rtB -> B_392_3686_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hs . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hs .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ou . Head = ( ( _rtDW -> TransportDelay_IWORK_ou . Head
< ( _rtDW -> TransportDelay_IWORK_ou . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ou . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ou . Head == _rtDW -> TransportDelay_IWORK_ou . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ou . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ou . Tail , & _rtDW -> TransportDelay_IWORK_ou . Head ,
& _rtDW -> TransportDelay_IWORK_ou . Last , simTime - _rtP -> P_2703 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ou . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ou . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ou . Head ] = _rtB -> B_392_3704_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_m2 = _rtB -> B_392_3711_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_o0x .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_my . Head = ( ( _rtDW -> TransportDelay_IWORK_my . Head
< ( _rtDW -> TransportDelay_IWORK_my . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_my . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_my . Head == _rtDW -> TransportDelay_IWORK_my . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_my . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_my . Tail , & _rtDW -> TransportDelay_IWORK_my . Head ,
& _rtDW -> TransportDelay_IWORK_my . Last , simTime - _rtP -> P_2708 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_my . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_my . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_my . Head ] = _rtB -> B_392_3712_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_il = _rtB -> B_392_3719_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ayh . Head = ( ( _rtDW -> TransportDelay_IWORK_ayh .
Head < ( _rtDW -> TransportDelay_IWORK_ayh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ayh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ayh . Head == _rtDW -> TransportDelay_IWORK_ayh . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ayh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ayh . Tail , & _rtDW -> TransportDelay_IWORK_ayh . Head
, & _rtDW -> TransportDelay_IWORK_ayh . Last , simTime - _rtP -> P_2715 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ayh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ayh . Head ] = _rtB -> B_392_3725_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pi = _rtB -> B_392_3732_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_n0 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jyu . Head = ( ( _rtDW -> TransportDelay_IWORK_jyu .
Head < ( _rtDW -> TransportDelay_IWORK_jyu . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_jyu . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jyu . Head == _rtDW -> TransportDelay_IWORK_jyu . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jyu . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jyu . Tail , & _rtDW -> TransportDelay_IWORK_jyu . Head
, & _rtDW -> TransportDelay_IWORK_jyu . Last , simTime - _rtP -> P_2720 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jyu . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jyu . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jyu . Head ] = _rtB -> B_392_3733_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ol = _rtB -> B_392_3740_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_oa2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ps . Head = ( ( _rtDW -> TransportDelay_IWORK_ps . Head
< ( _rtDW -> TransportDelay_IWORK_ps . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ps . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ps . Head == _rtDW -> TransportDelay_IWORK_ps . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ps . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ps . Tail , & _rtDW -> TransportDelay_IWORK_ps . Head ,
& _rtDW -> TransportDelay_IWORK_ps . Last , simTime - _rtP -> P_2727 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ps . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ps . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ps . Head ] = _rtB -> B_392_3746_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cj = _rtB -> B_392_3753_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_k4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ab . Head = ( ( _rtDW -> TransportDelay_IWORK_ab . Head
< ( _rtDW -> TransportDelay_IWORK_ab . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ab . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ab . Head == _rtDW -> TransportDelay_IWORK_ab . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ab . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ab . Tail , & _rtDW -> TransportDelay_IWORK_ab . Head ,
& _rtDW -> TransportDelay_IWORK_ab . Last , simTime - _rtP -> P_2732 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ab . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ab . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ab . Head ] = _rtB -> B_392_3754_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_imb = _rtB -> B_392_3761_0 ; } if ( _rtDW ->
TimeStampA_mu == ( rtInf ) ) { _rtDW -> TimeStampA_mu = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_g4 ; } else if ( _rtDW -> TimeStampB_c == ( rtInf )
) { _rtDW -> TimeStampB_c = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e5
; } else if ( _rtDW -> TimeStampA_mu < _rtDW -> TimeStampB_c ) { _rtDW ->
TimeStampA_mu = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_g4 ; } else {
_rtDW -> TimeStampB_c = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_e5 ; }
* lastU = _rtB -> B_392_3884_0 ; if ( _rtDW -> TimeStampA_k == ( rtInf ) ) {
_rtDW -> TimeStampA_k = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_pj ; }
else if ( _rtDW -> TimeStampB_p == ( rtInf ) ) { _rtDW -> TimeStampB_p =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_o ; } else if ( _rtDW ->
TimeStampA_k < _rtDW -> TimeStampB_p ) { _rtDW -> TimeStampA_k = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_pj ; } else { _rtDW -> TimeStampB_p =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_o ; } * lastU = _rtB ->
B_392_3896_0 ; if ( _rtDW -> TimeStampA_p == ( rtInf ) ) { _rtDW ->
TimeStampA_p = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_o ; } else if (
_rtDW -> TimeStampB_ne == ( rtInf ) ) { _rtDW -> TimeStampB_ne = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeB_ou ; } else if ( _rtDW -> TimeStampA_p <
_rtDW -> TimeStampB_ne ) { _rtDW -> TimeStampA_p = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeA_o ; } else { _rtDW -> TimeStampB_ne = ssGetT ( S ) ;
lastU = & _rtDW -> LastUAtTimeB_ou ; } * lastU = _rtB -> B_392_3908_0 ; {
real_T * * uBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_j4 .
TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_j4 . TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S )
; _rtDW -> TransportDelay_IWORK_ogq . Head = ( ( _rtDW ->
TransportDelay_IWORK_ogq . Head < ( _rtDW -> TransportDelay_IWORK_ogq .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ogq . Head + 1 ) :
0 ) ; if ( _rtDW -> TransportDelay_IWORK_ogq . Head == _rtDW ->
TransportDelay_IWORK_ogq . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ogq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ogq . Tail , & _rtDW -> TransportDelay_IWORK_ogq . Head
, & _rtDW -> TransportDelay_IWORK_ogq . Last , simTime - _rtP -> P_2880 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ogq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ogq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ogq . Head ] = _rtB -> B_392_3929_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_csp = _rtB -> B_392_3936_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hl . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hl .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_nn . Head = ( ( _rtDW -> TransportDelay_IWORK_nn . Head
< ( _rtDW -> TransportDelay_IWORK_nn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_nn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_nn . Head == _rtDW -> TransportDelay_IWORK_nn . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_nn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_nn . Tail , & _rtDW -> TransportDelay_IWORK_nn . Head ,
& _rtDW -> TransportDelay_IWORK_nn . Last , simTime - _rtP -> P_2885 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_nn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_nn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_nn . Head ] = _rtB -> B_392_3937_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_i1 = _rtB -> B_392_3944_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iy . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iy .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pik . Head = ( ( _rtDW -> TransportDelay_IWORK_pik .
Head < ( _rtDW -> TransportDelay_IWORK_pik . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_pik . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pik . Head == _rtDW -> TransportDelay_IWORK_pik . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pik . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pik . Tail , & _rtDW -> TransportDelay_IWORK_pik . Head
, & _rtDW -> TransportDelay_IWORK_pik . Last , simTime - _rtP -> P_2892 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pik . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pik . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pik . Head ] = _rtB -> B_392_3950_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_a0q = _rtB -> B_392_3957_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ndm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_k1 . Head = ( ( _rtDW -> TransportDelay_IWORK_k1 . Head
< ( _rtDW -> TransportDelay_IWORK_k1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_k1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_k1 . Head == _rtDW -> TransportDelay_IWORK_k1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_k1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_k1 . Tail , & _rtDW -> TransportDelay_IWORK_k1 . Head ,
& _rtDW -> TransportDelay_IWORK_k1 . Last , simTime - _rtP -> P_2897 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_k1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_k1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_k1 . Head ] = _rtB -> B_392_3958_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dz = _rtB -> B_392_3965_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_f0i . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_f0i .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_mn . Head = ( ( _rtDW -> TransportDelay_IWORK_mn . Head
< ( _rtDW -> TransportDelay_IWORK_mn . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_mn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_mn . Head == _rtDW -> TransportDelay_IWORK_mn . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_mn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_mn . Tail , & _rtDW -> TransportDelay_IWORK_mn . Head ,
& _rtDW -> TransportDelay_IWORK_mn . Last , simTime - _rtP -> P_2904 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_mn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_mn . Head ] = _rtB -> B_392_3971_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kn = _rtB -> B_392_3978_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ge . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ge .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pl . Head = ( ( _rtDW -> TransportDelay_IWORK_pl . Head
< ( _rtDW -> TransportDelay_IWORK_pl . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_pl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pl . Head == _rtDW -> TransportDelay_IWORK_pl . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pl . Tail , & _rtDW -> TransportDelay_IWORK_pl . Head ,
& _rtDW -> TransportDelay_IWORK_pl . Last , simTime - _rtP -> P_2909 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pl . Head ] = _rtB -> B_392_3979_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_iq = _rtB -> B_392_3986_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_e1q .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jj . Head = ( ( _rtDW -> TransportDelay_IWORK_jj . Head
< ( _rtDW -> TransportDelay_IWORK_jj . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jj . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jj . Head == _rtDW -> TransportDelay_IWORK_jj . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jj . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jj . Tail , & _rtDW -> TransportDelay_IWORK_jj . Head ,
& _rtDW -> TransportDelay_IWORK_jj . Last , simTime - _rtP -> P_2920 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jj . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jj . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jj . Head ] = _rtB -> B_392_4003_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kz = _rtB -> B_392_4010_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b2 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dr . Head = ( ( _rtDW -> TransportDelay_IWORK_dr . Head
< ( _rtDW -> TransportDelay_IWORK_dr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dr . Head == _rtDW -> TransportDelay_IWORK_dr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dr . Tail , & _rtDW -> TransportDelay_IWORK_dr . Head ,
& _rtDW -> TransportDelay_IWORK_dr . Last , simTime - _rtP -> P_2925 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dr . Head ] = _rtB -> B_392_4011_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lp = _rtB -> B_392_4018_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ma . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ma .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_a3y . Head = ( ( _rtDW -> TransportDelay_IWORK_a3y .
Head < ( _rtDW -> TransportDelay_IWORK_a3y . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_a3y . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_a3y . Head == _rtDW -> TransportDelay_IWORK_a3y . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_a3y . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_a3y . Tail , & _rtDW -> TransportDelay_IWORK_a3y . Head
, & _rtDW -> TransportDelay_IWORK_a3y . Last , simTime - _rtP -> P_2932 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_a3y . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3y . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_a3y . Head ] = _rtB -> B_392_4024_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ae = _rtB -> B_392_4031_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b5s .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dm . Head = ( ( _rtDW -> TransportDelay_IWORK_dm . Head
< ( _rtDW -> TransportDelay_IWORK_dm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_dm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dm . Head == _rtDW -> TransportDelay_IWORK_dm . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dm . Tail , & _rtDW -> TransportDelay_IWORK_dm . Head ,
& _rtDW -> TransportDelay_IWORK_dm . Last , simTime - _rtP -> P_2937 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dm . Head ] = _rtB -> B_392_4032_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nrz = _rtB -> B_392_4039_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pnn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_pdg . Head = ( ( _rtDW -> TransportDelay_IWORK_pdg .
Head < ( _rtDW -> TransportDelay_IWORK_pdg . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_pdg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_pdg . Head == _rtDW -> TransportDelay_IWORK_pdg . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_pdg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_pdg . Tail , & _rtDW -> TransportDelay_IWORK_pdg . Head
, & _rtDW -> TransportDelay_IWORK_pdg . Last , simTime - _rtP -> P_2944 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_pdg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_pdg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_pdg . Head ] = _rtB -> B_392_4045_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jd4 = _rtB -> B_392_4052_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_g1 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ny . Head = ( ( _rtDW -> TransportDelay_IWORK_ny . Head
< ( _rtDW -> TransportDelay_IWORK_ny . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ny . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ny . Head == _rtDW -> TransportDelay_IWORK_ny . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ny . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ny . Tail , & _rtDW -> TransportDelay_IWORK_ny . Head ,
& _rtDW -> TransportDelay_IWORK_ny . Last , simTime - _rtP -> P_2949 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ny . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ny . Head ] = _rtB -> B_392_4053_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o2n = _rtB -> B_392_4060_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay_DSTATE_c = _rtB ->
B_392_4308_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE_la = _rtB -> B_392_92_0 ; } isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE_p
) { isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_f = 0U ; _rtDW -> Integ4_DSTATE_kt = _rtP -> P_304 *
_rtB -> B_331_11_0 [ 0 ] + _rtB -> B_331_12_0 ; ssCallAccelRunBlock ( S , 331
, 17 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_da = _rtB ->
B_331_11_0 [ 0 ] ; _rtDW -> UnitDelay1_DSTATE_mz = _rtB -> B_331_24_0 ; _rtDW
-> Integ4_SYSTEM_ENABLE_n = 0U ; _rtDW -> Integ4_DSTATE_h = _rtP -> P_320 *
_rtB -> B_331_11_0 [ 1 ] + _rtB -> B_331_25_0 ; ssCallAccelRunBlock ( S , 331
, 30 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_b = _rtB ->
B_331_11_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_o = _rtB -> B_331_37_0 ; } }
_rtDW -> Integ4_SYSTEM_ENABLE_l = 0U ; _rtDW -> Integ4_DSTATE_d = _rtP ->
P_3035 * _rtB -> B_392_4270_0 [ 1 ] + _rtB -> B_392_4271_0 ;
ssCallAccelRunBlock ( S , 392 , 4276 , SS_CALL_MDL_UPDATE ) ; _rtDW ->
UnitDelay_DSTATE_f = _rtB -> B_392_4270_0 [ 1 ] ; _rtDW ->
UnitDelay1_DSTATE_at = _rtB -> B_392_4283_0 ; _rtDW ->
DiscreteDerivative_states_c = _rtDW -> DiscreteDerivative_tmp_p ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_f += _rtP -> P_3054 * _rtB -> B_392_4284_0 ; if
( _rtDW -> DiscreteTimeIntegrator_DSTATE_f >= _rtP -> P_3056 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_f = _rtP -> P_3056 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_f <= _rtP -> P_3057 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_f = _rtP -> P_3057 ; } } _rtDW ->
Delay_x1_DSTATE_h = _rtB -> B_392_4301_0 ; _rtDW -> Delay_x2_DSTATE_c = _rtB
-> B_392_4303_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_kk . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_kk . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_ml . Head = ( ( _rtDW
-> TransportDelay_IWORK_ml . Head < ( _rtDW -> TransportDelay_IWORK_ml .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_ml . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_ml . Head == _rtDW ->
TransportDelay_IWORK_ml . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ml . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ml . Tail , & _rtDW -> TransportDelay_IWORK_ml . Head ,
& _rtDW -> TransportDelay_IWORK_ml . Last , simTime - _rtP -> P_3150 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ml . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ml . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ml . Head ] = _rtB -> B_392_4463_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_on = _rtB -> B_392_4470_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ie . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ie .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_av . Head = ( ( _rtDW -> TransportDelay_IWORK_av . Head
< ( _rtDW -> TransportDelay_IWORK_av . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_av . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_av . Head == _rtDW -> TransportDelay_IWORK_av . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_av . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_av . Tail , & _rtDW -> TransportDelay_IWORK_av . Head ,
& _rtDW -> TransportDelay_IWORK_av . Last , simTime - _rtP -> P_3155 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_av . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_av . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_av . Head ] = _rtB -> B_392_4471_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_nh = _rtB -> B_392_4478_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ema . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ema .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bcn . Head = ( ( _rtDW -> TransportDelay_IWORK_bcn .
Head < ( _rtDW -> TransportDelay_IWORK_bcn . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_bcn . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bcn . Head == _rtDW -> TransportDelay_IWORK_bcn . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bcn . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bcn . Tail , & _rtDW -> TransportDelay_IWORK_bcn . Head
, & _rtDW -> TransportDelay_IWORK_bcn . Last , simTime - _rtP -> P_3162 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bcn . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bcn . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bcn . Head ] = _rtB -> B_392_4484_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gw = _rtB -> B_392_4491_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_hm . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_hm .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ge . Head = ( ( _rtDW -> TransportDelay_IWORK_ge . Head
< ( _rtDW -> TransportDelay_IWORK_ge . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ge . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ge . Head == _rtDW -> TransportDelay_IWORK_ge . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ge . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ge . Tail , & _rtDW -> TransportDelay_IWORK_ge . Head ,
& _rtDW -> TransportDelay_IWORK_ge . Last , simTime - _rtP -> P_3167 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ge . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ge . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ge . Head ] = _rtB -> B_392_4492_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ln = _rtB -> B_392_4499_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_agi . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_agi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_np . Head = ( ( _rtDW -> TransportDelay_IWORK_np . Head
< ( _rtDW -> TransportDelay_IWORK_np . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_np . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_np . Head == _rtDW -> TransportDelay_IWORK_np . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_np . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_np . Tail , & _rtDW -> TransportDelay_IWORK_np . Head ,
& _rtDW -> TransportDelay_IWORK_np . Last , simTime - _rtP -> P_3174 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_np . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_np . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_np . Head ] = _rtB -> B_392_4505_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cy = _rtB -> B_392_4512_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_iyk .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_p2 . Head = ( ( _rtDW -> TransportDelay_IWORK_p2 . Head
< ( _rtDW -> TransportDelay_IWORK_p2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_p2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_p2 . Head == _rtDW -> TransportDelay_IWORK_p2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_p2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_p2 . Tail , & _rtDW -> TransportDelay_IWORK_p2 . Head ,
& _rtDW -> TransportDelay_IWORK_p2 . Last , simTime - _rtP -> P_3179 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_p2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_p2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_p2 . Head ] = _rtB -> B_392_4513_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lj = _rtB -> B_392_4520_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ais . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ais .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_e0 . Head = ( ( _rtDW -> TransportDelay_IWORK_e0 . Head
< ( _rtDW -> TransportDelay_IWORK_e0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_e0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_e0 . Head == _rtDW -> TransportDelay_IWORK_e0 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_e0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_e0 . Tail , & _rtDW -> TransportDelay_IWORK_e0 . Head ,
& _rtDW -> TransportDelay_IWORK_e0 . Last , simTime - _rtP -> P_3190 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_e0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_e0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_e0 . Head ] = _rtB -> B_392_4537_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ko = _rtB -> B_392_4544_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fh . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fh .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_jr . Head = ( ( _rtDW -> TransportDelay_IWORK_jr . Head
< ( _rtDW -> TransportDelay_IWORK_jr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_jr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_jr . Head == _rtDW -> TransportDelay_IWORK_jr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_jr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_jr . Tail , & _rtDW -> TransportDelay_IWORK_jr . Head ,
& _rtDW -> TransportDelay_IWORK_jr . Last , simTime - _rtP -> P_3195 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_jr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_jr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_jr . Head ] = _rtB -> B_392_4545_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dc = _rtB -> B_392_4552_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dq . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dq .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_g2 . Head = ( ( _rtDW -> TransportDelay_IWORK_g2 . Head
< ( _rtDW -> TransportDelay_IWORK_g2 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_g2 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_g2 . Head == _rtDW -> TransportDelay_IWORK_g2 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_g2 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_g2 . Tail , & _rtDW -> TransportDelay_IWORK_g2 . Head ,
& _rtDW -> TransportDelay_IWORK_g2 . Last , simTime - _rtP -> P_3202 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_g2 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_g2 . Head ] = _rtB -> B_392_4558_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jl = _rtB -> B_392_4565_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ejf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kq . Head = ( ( _rtDW -> TransportDelay_IWORK_kq . Head
< ( _rtDW -> TransportDelay_IWORK_kq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kq . Head == _rtDW -> TransportDelay_IWORK_kq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kq . Tail , & _rtDW -> TransportDelay_IWORK_kq . Head ,
& _rtDW -> TransportDelay_IWORK_kq . Last , simTime - _rtP -> P_3207 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kq . Head ] = _rtB -> B_392_4566_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gk = _rtB -> B_392_4573_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ood . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ood .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_oh . Head = ( ( _rtDW -> TransportDelay_IWORK_oh . Head
< ( _rtDW -> TransportDelay_IWORK_oh . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_oh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_oh . Head == _rtDW -> TransportDelay_IWORK_oh . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_oh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_oh . Tail , & _rtDW -> TransportDelay_IWORK_oh . Head ,
& _rtDW -> TransportDelay_IWORK_oh . Last , simTime - _rtP -> P_3214 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_oh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_oh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_oh . Head ] = _rtB -> B_392_4579_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kh = _rtB -> B_392_4586_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_eq . Head = ( ( _rtDW -> TransportDelay_IWORK_eq . Head
< ( _rtDW -> TransportDelay_IWORK_eq . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_eq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_eq . Head == _rtDW -> TransportDelay_IWORK_eq . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_eq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_eq . Tail , & _rtDW -> TransportDelay_IWORK_eq . Head ,
& _rtDW -> TransportDelay_IWORK_eq . Last , simTime - _rtP -> P_3219 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_eq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_eq . Head ] = _rtB -> B_392_4587_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_kl = _rtB -> B_392_4594_0 ; } isHit = ssIsSampleHit ( S
, 3 , 0 ) ; if ( isHit != 0 ) { _rtDW -> UnitDelay_DSTATE_j = _rtB ->
B_392_4842_0 ; } isHit = ssIsSampleHit ( S , 2 , 0 ) ; if ( isHit != 0 ) {
_rtDW -> prediction_DSTATE_j = _rtB -> B_392_116_0 ; } isHit = ssIsSampleHit
( S , 3 , 0 ) ; if ( isHit != 0 ) { if ( _rtDW -> AutomaticGainControl_MODE )
{ isHit = ssIsSampleHit ( S , 3 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Integ4_SYSTEM_ENABLE_k = 0U ; _rtDW -> Integ4_DSTATE_j = _rtP -> P_362 * _rtB
-> B_368_11_0 [ 0 ] + _rtB -> B_368_12_0 ; ssCallAccelRunBlock ( S , 368 , 17
, SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_d = _rtB -> B_368_11_0 [ 0
] ; _rtDW -> UnitDelay1_DSTATE_g = _rtB -> B_368_24_0 ; _rtDW ->
Integ4_SYSTEM_ENABLE_b = 0U ; _rtDW -> Integ4_DSTATE_bx = _rtP -> P_378 *
_rtB -> B_368_11_0 [ 1 ] + _rtB -> B_368_25_0 ; ssCallAccelRunBlock ( S , 368
, 30 , SS_CALL_MDL_UPDATE ) ; _rtDW -> UnitDelay_DSTATE_e = _rtB ->
B_368_11_0 [ 1 ] ; _rtDW -> UnitDelay1_DSTATE_d = _rtB -> B_368_37_0 ; } }
_rtDW -> Integ4_SYSTEM_ENABLE_e = 0U ; _rtDW -> Integ4_DSTATE_k = _rtP ->
P_3305 * _rtB -> B_392_4804_0 [ 1 ] + _rtB -> B_392_4805_0 ;
ssCallAccelRunBlock ( S , 392 , 4810 , SS_CALL_MDL_UPDATE ) ; _rtDW ->
UnitDelay_DSTATE_h = _rtB -> B_392_4804_0 [ 1 ] ; _rtDW ->
UnitDelay1_DSTATE_f = _rtB -> B_392_4817_0 ; _rtDW ->
DiscreteDerivative_states_m = _rtDW -> DiscreteDerivative_tmp_m ; _rtDW ->
DiscreteTimeIntegrator_DSTATE_p += _rtP -> P_3324 * _rtB -> B_392_4818_0 ; if
( _rtDW -> DiscreteTimeIntegrator_DSTATE_p >= _rtP -> P_3326 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_p = _rtP -> P_3326 ; } else { if ( _rtDW ->
DiscreteTimeIntegrator_DSTATE_p <= _rtP -> P_3327 ) { _rtDW ->
DiscreteTimeIntegrator_DSTATE_p = _rtP -> P_3327 ; } } _rtDW ->
Delay_x1_DSTATE_c = _rtB -> B_392_4835_0 ; _rtDW -> Delay_x2_DSTATE_ch = _rtB
-> B_392_4837_0 ; } { real_T * * uBuffer = ( real_T * * ) & _rtDW ->
TransportDelay_PWORK_emg . TUbufferPtrs [ 0 ] ; real_T * * tBuffer = ( real_T
* * ) & _rtDW -> TransportDelay_PWORK_emg . TUbufferPtrs [ 1 ] ; real_T
simTime = ssGetT ( S ) ; _rtDW -> TransportDelay_IWORK_fc . Head = ( ( _rtDW
-> TransportDelay_IWORK_fc . Head < ( _rtDW -> TransportDelay_IWORK_fc .
CircularBufSize - 1 ) ) ? ( _rtDW -> TransportDelay_IWORK_fc . Head + 1 ) : 0
) ; if ( _rtDW -> TransportDelay_IWORK_fc . Head == _rtDW ->
TransportDelay_IWORK_fc . Tail ) { if ( !
MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_fc . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_fc . Tail , & _rtDW -> TransportDelay_IWORK_fc . Head ,
& _rtDW -> TransportDelay_IWORK_fc . Last , simTime - _rtP -> P_3416 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_fc . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_fc . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_fc . Head ] = _rtB -> B_392_4995_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jz = _rtB -> B_392_5002_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_db . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_db .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f0 . Head = ( ( _rtDW -> TransportDelay_IWORK_f0 . Head
< ( _rtDW -> TransportDelay_IWORK_f0 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f0 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f0 . Head == _rtDW -> TransportDelay_IWORK_f0 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f0 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f0 . Tail , & _rtDW -> TransportDelay_IWORK_f0 . Head ,
& _rtDW -> TransportDelay_IWORK_f0 . Last , simTime - _rtP -> P_3421 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f0 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f0 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f0 . Head ] = _rtB -> B_392_5003_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cv = _rtB -> B_392_5010_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ip . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ip .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lt . Head = ( ( _rtDW -> TransportDelay_IWORK_lt . Head
< ( _rtDW -> TransportDelay_IWORK_lt . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lt . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lt . Head == _rtDW -> TransportDelay_IWORK_lt . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lt . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lt . Tail , & _rtDW -> TransportDelay_IWORK_lt . Head ,
& _rtDW -> TransportDelay_IWORK_lt . Last , simTime - _rtP -> P_3428 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lt . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lt . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lt . Head ] = _rtB -> B_392_5016_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lva = _rtB -> B_392_5023_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_es . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_es .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lr . Head = ( ( _rtDW -> TransportDelay_IWORK_lr . Head
< ( _rtDW -> TransportDelay_IWORK_lr . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lr . Head == _rtDW -> TransportDelay_IWORK_lr . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lr . Tail , & _rtDW -> TransportDelay_IWORK_lr . Head ,
& _rtDW -> TransportDelay_IWORK_lr . Last , simTime - _rtP -> P_3433 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lr . Head ] = _rtB -> B_392_5024_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_el = _rtB -> B_392_5031_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_di . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_di .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ohq . Head = ( ( _rtDW -> TransportDelay_IWORK_ohq .
Head < ( _rtDW -> TransportDelay_IWORK_ohq . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_ohq . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ohq . Head == _rtDW -> TransportDelay_IWORK_ohq . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ohq . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ohq . Tail , & _rtDW -> TransportDelay_IWORK_ohq . Head
, & _rtDW -> TransportDelay_IWORK_ohq . Last , simTime - _rtP -> P_3440 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ohq . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ohq . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ohq . Head ] = _rtB -> B_392_5037_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ds = _rtB -> B_392_5044_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_kf . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_kf .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_phw . Head = ( ( _rtDW -> TransportDelay_IWORK_phw .
Head < ( _rtDW -> TransportDelay_IWORK_phw . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_phw . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_phw . Head == _rtDW -> TransportDelay_IWORK_phw . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_phw . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_phw . Tail , & _rtDW -> TransportDelay_IWORK_phw . Head
, & _rtDW -> TransportDelay_IWORK_phw . Last , simTime - _rtP -> P_3445 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_phw . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_phw . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_phw . Head ] = _rtB -> B_392_5045_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_dgf = _rtB -> B_392_5052_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fj . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fj .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_okh . Head = ( ( _rtDW -> TransportDelay_IWORK_okh .
Head < ( _rtDW -> TransportDelay_IWORK_okh . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_okh . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_okh . Head == _rtDW -> TransportDelay_IWORK_okh . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_okh . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_okh . Tail , & _rtDW -> TransportDelay_IWORK_okh . Head
, & _rtDW -> TransportDelay_IWORK_okh . Last , simTime - _rtP -> P_3455 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_okh . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_okh . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_okh . Head ] = _rtB -> B_392_5068_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_peu = _rtB -> B_392_5075_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ia . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ia .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ne . Head = ( ( _rtDW -> TransportDelay_IWORK_ne . Head
< ( _rtDW -> TransportDelay_IWORK_ne . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ne . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ne . Head == _rtDW -> TransportDelay_IWORK_ne . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ne . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ne . Tail , & _rtDW -> TransportDelay_IWORK_ne . Head ,
& _rtDW -> TransportDelay_IWORK_ne . Last , simTime - _rtP -> P_3460 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ne . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ne . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ne . Head ] = _rtB -> B_392_5076_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_cu = _rtB -> B_392_5083_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dz . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dz .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_dpr . Head = ( ( _rtDW -> TransportDelay_IWORK_dpr .
Head < ( _rtDW -> TransportDelay_IWORK_dpr . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_dpr . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_dpr . Head == _rtDW -> TransportDelay_IWORK_dpr . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_dpr . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_dpr . Tail , & _rtDW -> TransportDelay_IWORK_dpr . Head
, & _rtDW -> TransportDelay_IWORK_dpr . Last , simTime - _rtP -> P_3467 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_dpr . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_dpr . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_dpr . Head ] = _rtB -> B_392_5089_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_pn = _rtB -> B_392_5096_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_on . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_on .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_m1 . Head = ( ( _rtDW -> TransportDelay_IWORK_m1 . Head
< ( _rtDW -> TransportDelay_IWORK_m1 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_m1 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_m1 . Head == _rtDW -> TransportDelay_IWORK_m1 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_m1 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_m1 . Tail , & _rtDW -> TransportDelay_IWORK_m1 . Head ,
& _rtDW -> TransportDelay_IWORK_m1 . Last , simTime - _rtP -> P_3472 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_m1 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_m1 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_m1 . Head ] = _rtB -> B_392_5097_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_du = _rtB -> B_392_5104_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dii . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dii .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_hlb . Head = ( ( _rtDW -> TransportDelay_IWORK_hlb .
Head < ( _rtDW -> TransportDelay_IWORK_hlb . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_hlb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_hlb . Head == _rtDW -> TransportDelay_IWORK_hlb . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_hlb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_hlb . Tail , & _rtDW -> TransportDelay_IWORK_hlb . Head
, & _rtDW -> TransportDelay_IWORK_hlb . Last , simTime - _rtP -> P_3479 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_hlb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_hlb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_hlb . Head ] = _rtB -> B_392_5110_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_oc = _rtB -> B_392_5117_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_pi . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_pi .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ka . Head = ( ( _rtDW -> TransportDelay_IWORK_ka . Head
< ( _rtDW -> TransportDelay_IWORK_ka . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ka . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ka . Head == _rtDW -> TransportDelay_IWORK_ka . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ka . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ka . Tail , & _rtDW -> TransportDelay_IWORK_ka . Head ,
& _rtDW -> TransportDelay_IWORK_ka . Last , simTime - _rtP -> P_3484 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ka . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ka . Head ] = _rtB -> B_392_5118_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_fr = _rtB -> B_392_5125_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_cco . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_cco .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_lb . Head = ( ( _rtDW -> TransportDelay_IWORK_lb . Head
< ( _rtDW -> TransportDelay_IWORK_lb . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_lb . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_lb . Head == _rtDW -> TransportDelay_IWORK_lb . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_lb . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_lb . Tail , & _rtDW -> TransportDelay_IWORK_lb . Head ,
& _rtDW -> TransportDelay_IWORK_lb . Last , simTime - _rtP -> P_3498 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_lb . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_lb . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_lb . Head ] = _rtB -> B_392_5149_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_l0 = _rtB -> B_392_5156_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bw . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bw .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_l3 . Head = ( ( _rtDW -> TransportDelay_IWORK_l3 . Head
< ( _rtDW -> TransportDelay_IWORK_l3 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_l3 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_l3 . Head == _rtDW -> TransportDelay_IWORK_l3 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_l3 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_l3 . Tail , & _rtDW -> TransportDelay_IWORK_l3 . Head ,
& _rtDW -> TransportDelay_IWORK_l3 . Last , simTime - _rtP -> P_3503 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_l3 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_l3 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_l3 . Head ] = _rtB -> B_392_5157_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_he = _rtB -> B_392_5164_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ab . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ab .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_gp . Head = ( ( _rtDW -> TransportDelay_IWORK_gp . Head
< ( _rtDW -> TransportDelay_IWORK_gp . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_gp . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_gp . Head == _rtDW -> TransportDelay_IWORK_gp . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_gp . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_gp . Tail , & _rtDW -> TransportDelay_IWORK_gp . Head ,
& _rtDW -> TransportDelay_IWORK_gp . Last , simTime - _rtP -> P_3510 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_gp . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_gp . Head ] = _rtB -> B_392_5170_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ph = _rtB -> B_392_5177_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_mwe .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f5 . Head = ( ( _rtDW -> TransportDelay_IWORK_f5 . Head
< ( _rtDW -> TransportDelay_IWORK_f5 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f5 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f5 . Head == _rtDW -> TransportDelay_IWORK_f5 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f5 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f5 . Tail , & _rtDW -> TransportDelay_IWORK_f5 . Head ,
& _rtDW -> TransportDelay_IWORK_f5 . Last , simTime - _rtP -> P_3515 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f5 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f5 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f5 . Head ] = _rtB -> B_392_5178_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_gy = _rtB -> B_392_5185_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_nvo .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ha . Head = ( ( _rtDW -> TransportDelay_IWORK_ha . Head
< ( _rtDW -> TransportDelay_IWORK_ha . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ha . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ha . Head == _rtDW -> TransportDelay_IWORK_ha . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ha . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ha . Tail , & _rtDW -> TransportDelay_IWORK_ha . Head ,
& _rtDW -> TransportDelay_IWORK_ha . Last , simTime - _rtP -> P_3522 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ha . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ha . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ha . Head ] = _rtB -> B_392_5191_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_jg = _rtB -> B_392_5198_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_ky . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_ky .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bm . Head = ( ( _rtDW -> TransportDelay_IWORK_bm . Head
< ( _rtDW -> TransportDelay_IWORK_bm . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_bm . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bm . Head == _rtDW -> TransportDelay_IWORK_bm . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bm . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bm . Tail , & _rtDW -> TransportDelay_IWORK_bm . Head ,
& _rtDW -> TransportDelay_IWORK_bm . Last , simTime - _rtP -> P_3527 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bm . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bm . Head ] = _rtB -> B_392_5199_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_ei = _rtB -> B_392_5206_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_bro . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_bro .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_f4 . Head = ( ( _rtDW -> TransportDelay_IWORK_f4 . Head
< ( _rtDW -> TransportDelay_IWORK_f4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_f4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_f4 . Head == _rtDW -> TransportDelay_IWORK_f4 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_f4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_f4 . Tail , & _rtDW -> TransportDelay_IWORK_f4 . Head ,
& _rtDW -> TransportDelay_IWORK_f4 . Last , simTime - _rtP -> P_3538 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_f4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_f4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_f4 . Head ] = _rtB -> B_392_5224_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_phq = _rtB -> B_392_5231_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_fn . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_fn .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_kg . Head = ( ( _rtDW -> TransportDelay_IWORK_kg . Head
< ( _rtDW -> TransportDelay_IWORK_kg . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_kg . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_kg . Head == _rtDW -> TransportDelay_IWORK_kg . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_kg . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_kg . Tail , & _rtDW -> TransportDelay_IWORK_kg . Head ,
& _rtDW -> TransportDelay_IWORK_kg . Last , simTime - _rtP -> P_3543 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_kg . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_kg . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_kg . Head ] = _rtB -> B_392_5232_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_lkz = _rtB -> B_392_5239_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_dy4 . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_dy4 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_o4 . Head = ( ( _rtDW -> TransportDelay_IWORK_o4 . Head
< ( _rtDW -> TransportDelay_IWORK_o4 . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_o4 . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_o4 . Head == _rtDW -> TransportDelay_IWORK_o4 . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_o4 . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_o4 . Tail , & _rtDW -> TransportDelay_IWORK_o4 . Head ,
& _rtDW -> TransportDelay_IWORK_o4 . Last , simTime - _rtP -> P_3550 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_o4 . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_o4 . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_o4 . Head ] = _rtB -> B_392_5245_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_po = _rtB -> B_392_5252_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 . TUbufferPtrs [ 0 ] ; real_T
* * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_m5 .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_bck . Head = ( ( _rtDW -> TransportDelay_IWORK_bck .
Head < ( _rtDW -> TransportDelay_IWORK_bck . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_bck . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_bck . Head == _rtDW -> TransportDelay_IWORK_bck . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_bck . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_bck . Tail , & _rtDW -> TransportDelay_IWORK_bck . Head
, & _rtDW -> TransportDelay_IWORK_bck . Last , simTime - _rtP -> P_3555 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_bck . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_bck . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_bck . Head ] = _rtB -> B_392_5253_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_o1 = _rtB -> B_392_5260_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_b0c .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_prl . Head = ( ( _rtDW -> TransportDelay_IWORK_prl .
Head < ( _rtDW -> TransportDelay_IWORK_prl . CircularBufSize - 1 ) ) ? (
_rtDW -> TransportDelay_IWORK_prl . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_prl . Head == _rtDW -> TransportDelay_IWORK_prl . Tail )
{ if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_prl . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_prl . Tail , & _rtDW -> TransportDelay_IWORK_prl . Head
, & _rtDW -> TransportDelay_IWORK_prl . Last , simTime - _rtP -> P_3562 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_prl . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_prl . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_prl . Head ] = _rtB -> B_392_5266_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_bt = _rtB -> B_392_5273_0 ; } { real_T * * uBuffer = (
real_T * * ) & _rtDW -> TransportDelay_PWORK_acd . TUbufferPtrs [ 0 ] ;
real_T * * tBuffer = ( real_T * * ) & _rtDW -> TransportDelay_PWORK_acd .
TUbufferPtrs [ 1 ] ; real_T simTime = ssGetT ( S ) ; _rtDW ->
TransportDelay_IWORK_ca . Head = ( ( _rtDW -> TransportDelay_IWORK_ca . Head
< ( _rtDW -> TransportDelay_IWORK_ca . CircularBufSize - 1 ) ) ? ( _rtDW ->
TransportDelay_IWORK_ca . Head + 1 ) : 0 ) ; if ( _rtDW ->
TransportDelay_IWORK_ca . Head == _rtDW -> TransportDelay_IWORK_ca . Tail ) {
if ( ! MR_CIGRE_GENERALIZADO_acc_rt_TDelayUpdateTailOrGrowBuf ( & _rtDW ->
TransportDelay_IWORK_ca . CircularBufSize , & _rtDW ->
TransportDelay_IWORK_ca . Tail , & _rtDW -> TransportDelay_IWORK_ca . Head ,
& _rtDW -> TransportDelay_IWORK_ca . Last , simTime - _rtP -> P_3567 ,
tBuffer , uBuffer , ( NULL ) , ( boolean_T ) 0 , false , & _rtDW ->
TransportDelay_IWORK_ca . MaxNewBufSize ) ) { ssSetErrorStatus ( S ,
"tdelay memory allocation error" ) ; return ; } } ( * tBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = simTime ; ( * uBuffer ) [ _rtDW ->
TransportDelay_IWORK_ca . Head ] = _rtB -> B_392_5274_0 ; } isHit =
ssIsSampleHit ( S , 1 , 0 ) ; if ( isHit != 0 ) { _rtDW ->
Memory_PreviousInput_h4 = _rtB -> B_392_5281_0 ; } if ( _rtDW ->
TimeStampA_lh == ( rtInf ) ) { _rtDW -> TimeStampA_lh = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_n ; } else if ( _rtDW -> TimeStampB_ns == ( rtInf )
) { _rtDW -> TimeStampB_ns = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_k
; } else if ( _rtDW -> TimeStampA_lh < _rtDW -> TimeStampB_ns ) { _rtDW ->
TimeStampA_lh = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_n ; } else {
_rtDW -> TimeStampB_ns = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_k ; }
* lastU = _rtB -> B_392_5403_0 ; if ( _rtDW -> TimeStampA_f == ( rtInf ) ) {
_rtDW -> TimeStampA_f = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_i ; }
else if ( _rtDW -> TimeStampB_o == ( rtInf ) ) { _rtDW -> TimeStampB_o =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_ok ; } else if ( _rtDW ->
TimeStampA_f < _rtDW -> TimeStampB_o ) { _rtDW -> TimeStampA_f = ssGetT ( S )
; lastU = & _rtDW -> LastUAtTimeA_i ; } else { _rtDW -> TimeStampB_o = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeB_ok ; } * lastU = _rtB -> B_392_5415_0
; if ( _rtDW -> TimeStampA_c == ( rtInf ) ) { _rtDW -> TimeStampA_c = ssGetT
( S ) ; lastU = & _rtDW -> LastUAtTimeA_ej ; } else if ( _rtDW ->
TimeStampB_kd == ( rtInf ) ) { _rtDW -> TimeStampB_kd = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeB_j ; } else if ( _rtDW -> TimeStampA_c < _rtDW ->
TimeStampB_kd ) { _rtDW -> TimeStampA_c = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeA_ej ; } else { _rtDW -> TimeStampB_kd = ssGetT ( S ) ; lastU = &
_rtDW -> LastUAtTimeB_j ; } * lastU = _rtB -> B_392_5427_0 ; if ( _rtDW ->
TimeStampA_d == ( rtInf ) ) { _rtDW -> TimeStampA_d = ssGetT ( S ) ; lastU =
& _rtDW -> LastUAtTimeA_cs ; } else if ( _rtDW -> TimeStampB_p0 == ( rtInf )
) { _rtDW -> TimeStampB_p0 = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_ec ; } else if ( _rtDW -> TimeStampA_d < _rtDW -> TimeStampB_p0
) { _rtDW -> TimeStampA_d = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cs
; } else { _rtDW -> TimeStampB_p0 = ssGetT ( S ) ; lastU = & _rtDW ->
LastUAtTimeB_ec ; } * lastU = _rtB -> B_392_5457_0 ; if ( _rtDW ->
TimeStampA_hs == ( rtInf ) ) { _rtDW -> TimeStampA_hs = ssGetT ( S ) ; lastU
= & _rtDW -> LastUAtTimeA_cq ; } else if ( _rtDW -> TimeStampB_h == ( rtInf )
) { _rtDW -> TimeStampB_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_f
; } else if ( _rtDW -> TimeStampA_hs < _rtDW -> TimeStampB_h ) { _rtDW ->
TimeStampA_hs = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_cq ; } else {
_rtDW -> TimeStampB_h = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_f ; }
* lastU = _rtB -> B_392_5469_0 ; if ( _rtDW -> TimeStampA_if == ( rtInf ) ) {
_rtDW -> TimeStampA_if = ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeA_a ; }
else if ( _rtDW -> TimeStampB_cy == ( rtInf ) ) { _rtDW -> TimeStampB_cy =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_hd ; } else if ( _rtDW ->
TimeStampA_if < _rtDW -> TimeStampB_cy ) { _rtDW -> TimeStampA_if = ssGetT (
S ) ; lastU = & _rtDW -> LastUAtTimeA_a ; } else { _rtDW -> TimeStampB_cy =
ssGetT ( S ) ; lastU = & _rtDW -> LastUAtTimeB_hd ; } * lastU = _rtB ->
B_392_5481_0 ; UNUSED_PARAMETER ( tid ) ; }
#define MDL_UPDATE
static void mdlUpdateTID4 ( SimStruct * S , int_T tid ) { UNUSED_PARAMETER (
tid ) ; }
#define MDL_DERIVATIVES
static void mdlDerivatives ( SimStruct * S ) { B_MR_CIGRE_GENERALIZADO_T *
_rtB ; P_MR_CIGRE_GENERALIZADO_T * _rtP ; X_MR_CIGRE_GENERALIZADO_T * _rtX ;
XDot_MR_CIGRE_GENERALIZADO_T * _rtXdot ; _rtXdot = ( (
XDot_MR_CIGRE_GENERALIZADO_T * ) ssGetdX ( S ) ) ; _rtX = ( (
X_MR_CIGRE_GENERALIZADO_T * ) ssGetContStates ( S ) ) ; _rtP = ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) ; _rtB = ( (
B_MR_CIGRE_GENERALIZADO_T * ) _ssGetModelBlockIO ( S ) ) ; _rtXdot ->
LPF2_CSTATE = 0.0 ; _rtXdot -> LPF2_CSTATE += _rtP -> P_411 * _rtX ->
LPF2_CSTATE ; _rtXdot -> LPF2_CSTATE += _rtB -> B_392_638_0 ; _rtXdot ->
LPF1_CSTATE = 0.0 ; _rtXdot -> LPF1_CSTATE += _rtP -> P_413 * _rtX ->
LPF1_CSTATE ; _rtXdot -> LPF1_CSTATE += _rtB -> B_392_647_0 ; _rtXdot ->
Integrator1_CSTATE = _rtB -> B_392_748_0 ; _rtXdot -> LPF2_CSTATE_e = 0.0 ;
_rtXdot -> LPF2_CSTATE_e += _rtP -> P_439 * _rtX -> LPF2_CSTATE_e ; _rtXdot
-> LPF2_CSTATE_e += _rtB -> B_392_992_0 ; _rtXdot -> LPF1_CSTATE_g = 0.0 ;
_rtXdot -> LPF1_CSTATE_g += _rtP -> P_441 * _rtX -> LPF1_CSTATE_g ; _rtXdot
-> LPF1_CSTATE_g += _rtB -> B_392_1001_0 ; _rtXdot -> Integrator1_CSTATE_o =
_rtB -> B_392_1086_0 ; _rtXdot -> LPF1_CSTATE_o = 0.0 ; _rtXdot ->
LPF1_CSTATE_o += _rtP -> P_591 * _rtX -> LPF1_CSTATE_o ; _rtXdot ->
LPF1_CSTATE_o += _rtB -> B_392_813_0 ; _rtXdot -> LPF2_CSTATE_c = 0.0 ;
_rtXdot -> LPF2_CSTATE_c += _rtP -> P_593 * _rtX -> LPF2_CSTATE_c ; _rtXdot
-> LPF2_CSTATE_c += _rtB -> B_392_817_0 ; _rtXdot -> integrator_CSTATE = _rtB
-> B_392_878_0 ; _rtXdot -> integrator_CSTATE_n = _rtB -> B_392_880_0 ;
_rtXdot -> integrator_CSTATE_g = _rtB -> B_392_882_0 ; _rtXdot ->
integrator_CSTATE_e = _rtB -> B_392_884_0 ; _rtXdot -> integrator_CSTATE_l =
_rtB -> B_392_886_0 ; _rtXdot -> integrator_CSTATE_p = _rtB -> B_392_888_0 ;
_rtXdot -> integrator_CSTATE_f = _rtB -> B_392_891_0 ; _rtXdot ->
integrator_CSTATE_k = _rtB -> B_392_893_0 ; _rtXdot -> integrator_CSTATE_lb =
_rtB -> B_392_895_0 ; _rtXdot -> integrator_CSTATE_gs = _rtB -> B_392_897_0 ;
_rtXdot -> integrator_CSTATE_o = _rtB -> B_392_899_0 ; _rtXdot ->
integrator_CSTATE_h = _rtB -> B_392_901_0 ; _rtXdot -> LPF1_CSTATE_e = 0.0 ;
_rtXdot -> LPF1_CSTATE_e += _rtP -> P_695 * _rtX -> LPF1_CSTATE_e ; _rtXdot
-> LPF1_CSTATE_e += _rtB -> B_392_1151_0 ; _rtXdot -> LPF2_CSTATE_m = 0.0 ;
_rtXdot -> LPF2_CSTATE_m += _rtP -> P_697 * _rtX -> LPF2_CSTATE_m ; _rtXdot
-> LPF2_CSTATE_m += _rtB -> B_392_1155_0 ; _rtXdot -> integrator_CSTATE_ok =
_rtB -> B_392_1216_0 ; _rtXdot -> integrator_CSTATE_hr = _rtB -> B_392_1218_0
; _rtXdot -> integrator_CSTATE_e5 = _rtB -> B_392_1220_0 ; _rtXdot ->
integrator_CSTATE_j = _rtB -> B_392_1222_0 ; _rtXdot -> integrator_CSTATE_n1
= _rtB -> B_392_1224_0 ; _rtXdot -> integrator_CSTATE_fv = _rtB ->
B_392_1226_0 ; _rtXdot -> integrator_CSTATE_b = _rtB -> B_392_1229_0 ;
_rtXdot -> integrator_CSTATE_kx = _rtB -> B_392_1231_0 ; _rtXdot ->
integrator_CSTATE_m = _rtB -> B_392_1233_0 ; _rtXdot -> integrator_CSTATE_c =
_rtB -> B_392_1235_0 ; _rtXdot -> integrator_CSTATE_cw = _rtB -> B_392_1237_0
; _rtXdot -> integrator_CSTATE_d = _rtB -> B_392_1239_0 ; _rtXdot ->
Integradord_CSTATE = 0.0 ; _rtXdot -> Integradord_CSTATE += _rtP -> P_801 *
_rtX -> Integradord_CSTATE ; _rtXdot -> Integradord_CSTATE += _rtB ->
B_392_654_0 ; _rtXdot -> LPF4_CSTATE = 0.0 ; _rtXdot -> LPF4_CSTATE += _rtP
-> P_812 * _rtX -> LPF4_CSTATE ; _rtXdot -> LPF4_CSTATE += _rtB ->
B_392_873_0 ; _rtXdot -> LPF2_CSTATE_p = 0.0 ; _rtXdot -> LPF2_CSTATE_p +=
_rtP -> P_818 * _rtX -> LPF2_CSTATE_p ; _rtXdot -> LPF2_CSTATE_p += _rtB ->
B_392_874_0 ; _rtXdot -> Integradord1_CSTATE = 0.0 ; _rtXdot ->
Integradord1_CSTATE += _rtP -> P_821 * _rtX -> Integradord1_CSTATE ; _rtXdot
-> Integradord1_CSTATE += _rtB -> B_392_655_0 ; _rtXdot ->
Integradord_CSTATE_l = 0.0 ; _rtXdot -> Integradord_CSTATE_l += _rtP -> P_840
* _rtX -> Integradord_CSTATE_l ; _rtXdot -> Integradord_CSTATE_l += _rtB ->
B_392_727_0 ; _rtXdot -> HPF1_CSTATE = 0.0 ; _rtXdot -> HPF1_CSTATE += _rtP
-> P_843 * _rtX -> HPF1_CSTATE ; _rtXdot -> HPF1_CSTATE += _rtB ->
B_392_679_0 ; _rtXdot -> HPF2_CSTATE = 0.0 ; _rtXdot -> HPF2_CSTATE += _rtP
-> P_857 * _rtX -> HPF2_CSTATE ; _rtXdot -> HPF2_CSTATE += _rtB ->
B_392_703_0 ; _rtXdot -> Integradorq_CSTATE = 0.0 ; _rtXdot ->
Integradorq_CSTATE += _rtP -> P_862 * _rtX -> Integradorq_CSTATE ; _rtXdot ->
Integradorq_CSTATE += _rtB -> B_392_731_0 ; _rtXdot -> Integradord_CSTATE_a =
0.0 ; _rtXdot -> Integradord_CSTATE_a += _rtP -> P_996 * _rtX ->
Integradord_CSTATE_a ; _rtXdot -> Integradord_CSTATE_a += _rtB ->
B_392_1008_0 ; _rtXdot -> LPF4_CSTATE_f = 0.0 ; _rtXdot -> LPF4_CSTATE_f +=
_rtP -> P_1007 * _rtX -> LPF4_CSTATE_f ; _rtXdot -> LPF4_CSTATE_f += _rtB ->
B_392_1210_0 ; _rtXdot -> LPF2_CSTATE_a = 0.0 ; _rtXdot -> LPF2_CSTATE_a +=
_rtP -> P_1013 * _rtX -> LPF2_CSTATE_a ; _rtXdot -> LPF2_CSTATE_a += _rtB ->
B_392_1212_0 ; _rtXdot -> Integradord1_CSTATE_n = 0.0 ; _rtXdot ->
Integradord1_CSTATE_n += _rtP -> P_1016 * _rtX -> Integradord1_CSTATE_n ;
_rtXdot -> Integradord1_CSTATE_n += _rtB -> B_392_1009_0 ; _rtXdot ->
Integradord_CSTATE_n = 0.0 ; _rtXdot -> Integradord_CSTATE_n += _rtP ->
P_1037 * _rtX -> Integradord_CSTATE_n ; _rtXdot -> Integradord_CSTATE_n +=
_rtB -> B_392_1079_0 ; _rtXdot -> HPF1_CSTATE_k = 0.0 ; _rtXdot ->
HPF1_CSTATE_k += _rtP -> P_1040 * _rtX -> HPF1_CSTATE_k ; _rtXdot ->
HPF1_CSTATE_k += _rtB -> B_392_1037_0 ; _rtXdot -> HPF2_CSTATE_g = 0.0 ;
_rtXdot -> HPF2_CSTATE_g += _rtP -> P_1052 * _rtX -> HPF2_CSTATE_g ; _rtXdot
-> HPF2_CSTATE_g += _rtB -> B_392_1057_0 ; _rtXdot -> Integradorq_CSTATE_a =
0.0 ; _rtXdot -> Integradorq_CSTATE_a += _rtP -> P_1057 * _rtX ->
Integradorq_CSTATE_a ; _rtXdot -> Integradorq_CSTATE_a += _rtB ->
B_392_1083_0 ; _rtXdot -> integrator_CSTATE_ot = _rtB -> B_392_1557_0 ;
_rtXdot -> integrator_CSTATE_a = _rtB -> B_392_1559_0 ; _rtXdot ->
integrator_CSTATE_ob = _rtB -> B_392_1561_0 ; _rtXdot -> integrator_CSTATE_pp
= _rtB -> B_392_1563_0 ; _rtXdot -> integrator_CSTATE_fq = _rtB ->
B_392_1565_0 ; _rtXdot -> integrator_CSTATE_eu = _rtB -> B_392_1567_0 ;
_rtXdot -> integrator_CSTATE_mz = _rtB -> B_392_1569_0 ; _rtXdot ->
integrator_CSTATE_fa = _rtB -> B_392_1571_0 ; _rtXdot -> integrator_CSTATE_bq
= _rtB -> B_392_1573_0 ; _rtXdot -> integrator_CSTATE_fc = _rtB ->
B_392_1575_0 ; _rtXdot -> integrator_CSTATE_nm = _rtB -> B_392_1577_0 ;
_rtXdot -> integrator_CSTATE_c1 = _rtB -> B_392_1579_0 ; _rtXdot ->
integrator_CSTATE_g2 = _rtB -> B_392_1581_0 ; _rtXdot ->
integrator_CSTATE_lbk = _rtB -> B_392_1583_0 ; _rtXdot ->
integrator_CSTATE_gw = _rtB -> B_392_1585_0 ; _rtXdot -> integrator_CSTATE_k1
= _rtB -> B_392_1587_0 ; _rtXdot -> integrator_CSTATE_obo = _rtB ->
B_392_1589_0 ; _rtXdot -> integrator_CSTATE_md = _rtB -> B_392_1591_0 ;
_rtXdot -> integrator_CSTATE_nq = _rtB -> B_392_2081_0 ; _rtXdot ->
integrator_CSTATE_fo = _rtB -> B_392_2083_0 ; _rtXdot -> integrator_CSTATE_p5
= _rtB -> B_392_2085_0 ; _rtXdot -> integrator_CSTATE_ad = _rtB ->
B_392_2087_0 ; _rtXdot -> integrator_CSTATE_cd = _rtB -> B_392_2089_0 ;
_rtXdot -> integrator_CSTATE_ds = _rtB -> B_392_2091_0 ; _rtXdot ->
integrator_CSTATE_ja = _rtB -> B_392_2093_0 ; _rtXdot -> integrator_CSTATE_an
= _rtB -> B_392_2095_0 ; _rtXdot -> integrator_CSTATE_d3 = _rtB ->
B_392_2097_0 ; _rtXdot -> integrator_CSTATE_fl = _rtB -> B_392_2099_0 ;
_rtXdot -> integrator_CSTATE_m3 = _rtB -> B_392_2101_0 ; _rtXdot ->
integrator_CSTATE_oo = _rtB -> B_392_2103_0 ; _rtXdot -> integrator_CSTATE_cf
= _rtB -> B_392_2105_0 ; _rtXdot -> integrator_CSTATE_jq = _rtB ->
B_392_2107_0 ; _rtXdot -> integrator_CSTATE_i = _rtB -> B_392_2109_0 ;
_rtXdot -> integrator_CSTATE_l1 = _rtB -> B_392_2111_0 ; _rtXdot ->
integrator_CSTATE_k1l = _rtB -> B_392_2113_0 ; _rtXdot ->
integrator_CSTATE_hs = _rtB -> B_392_2115_0 ; _rtXdot -> integrator_CSTATE_en
= _rtB -> B_392_2068_0 ; _rtXdot -> integrator_CSTATE_iv = _rtB ->
B_392_2070_0 ; _rtXdot -> integrator_CSTATE_hf = _rtB -> B_392_2072_0 ;
_rtXdot -> integrator_CSTATE_fj = _rtB -> B_392_2074_0 ; _rtXdot ->
integrator_CSTATE_ff = _rtB -> B_392_2076_0 ; _rtXdot -> integrator_CSTATE_er
= _rtB -> B_392_2078_0 ; _rtXdot -> integrator_CSTATE_pb = _rtB ->
B_392_2531_0 ; _rtXdot -> integrator_CSTATE_os = _rtB -> B_392_2533_0 ;
_rtXdot -> integrator_CSTATE_ju = _rtB -> B_392_2535_0 ; _rtXdot ->
integrator_CSTATE_a5 = _rtB -> B_392_2537_0 ; _rtXdot ->
integrator_CSTATE_pbj = _rtB -> B_392_2539_0 ; _rtXdot ->
integrator_CSTATE_mu = _rtB -> B_392_2541_0 ; _rtXdot -> integrator_CSTATE_gd
= _rtB -> B_392_2543_0 ; _rtXdot -> integrator_CSTATE_l5 = _rtB ->
B_392_2545_0 ; _rtXdot -> integrator_CSTATE_hq = _rtB -> B_392_2547_0 ;
_rtXdot -> integrator_CSTATE_cb = _rtB -> B_392_2549_0 ; _rtXdot ->
integrator_CSTATE_po = _rtB -> B_392_2551_0 ; _rtXdot -> integrator_CSTATE_ag
= _rtB -> B_392_2553_0 ; _rtXdot -> integrator_CSTATE_ba = _rtB ->
B_392_2555_0 ; _rtXdot -> integrator_CSTATE_j4 = _rtB -> B_392_2557_0 ;
_rtXdot -> integrator_CSTATE_oi = _rtB -> B_392_2559_0 ; _rtXdot ->
integrator_CSTATE_gb = _rtB -> B_392_2561_0 ; _rtXdot -> integrator_CSTATE_kn
= _rtB -> B_392_2563_0 ; _rtXdot -> integrator_CSTATE_dn = _rtB ->
B_392_2565_0 ; _rtXdot -> integrator_CSTATE_bd = _rtB -> B_392_2518_0 ;
_rtXdot -> integrator_CSTATE_mf = _rtB -> B_392_2520_0 ; _rtXdot ->
integrator_CSTATE_mt = _rtB -> B_392_2522_0 ; _rtXdot ->
integrator_CSTATE_jq4 = _rtB -> B_392_2524_0 ; _rtXdot ->
integrator_CSTATE_ez = _rtB -> B_392_2526_0 ; _rtXdot -> integrator_CSTATE_ct
= _rtB -> B_392_2528_0 ; _rtXdot -> integrator_CSTATE_bm = _rtB ->
B_392_2933_0 ; _rtXdot -> integrator_CSTATE_hk = _rtB -> B_392_2935_0 ;
_rtXdot -> integrator_CSTATE_ia = _rtB -> B_392_2937_0 ; _rtXdot ->
integrator_CSTATE_eq = _rtB -> B_392_2939_0 ; _rtXdot -> integrator_CSTATE_af
= _rtB -> B_392_2941_0 ; _rtXdot -> integrator_CSTATE_kxp = _rtB ->
B_392_2943_0 ; _rtXdot -> integrator_CSTATE_hrr = _rtB -> B_392_2945_0 ;
_rtXdot -> integrator_CSTATE_e4 = _rtB -> B_392_2947_0 ; _rtXdot ->
integrator_CSTATE_otl = _rtB -> B_392_2949_0 ; _rtXdot ->
integrator_CSTATE_pv = _rtB -> B_392_2951_0 ; _rtXdot -> integrator_CSTATE_i5
= _rtB -> B_392_2953_0 ; _rtXdot -> integrator_CSTATE_dsl = _rtB ->
B_392_2955_0 ; _rtXdot -> integrator_CSTATE_if = _rtB -> B_392_2957_0 ;
_rtXdot -> integrator_CSTATE_il = _rtB -> B_392_2959_0 ; _rtXdot ->
integrator_CSTATE_cv = _rtB -> B_392_2961_0 ; _rtXdot -> integrator_CSTATE_cl
= _rtB -> B_392_2963_0 ; _rtXdot -> integrator_CSTATE_ge = _rtB ->
B_392_2965_0 ; _rtXdot -> integrator_CSTATE_ak = _rtB -> B_392_2967_0 ;
_rtXdot -> integrator_CSTATE_mz2 = _rtB -> B_392_2920_0 ; _rtXdot ->
integrator_CSTATE_b5 = _rtB -> B_392_2922_0 ; _rtXdot -> integrator_CSTATE_hc
= _rtB -> B_392_2924_0 ; _rtXdot -> integrator_CSTATE_bn = _rtB ->
B_392_2926_0 ; _rtXdot -> integrator_CSTATE_jqz = _rtB -> B_392_2928_0 ;
_rtXdot -> integrator_CSTATE_mn = _rtB -> B_392_2930_0 ; _rtXdot ->
integrator_CSTATE_np = _rtB -> B_392_3385_0 ; _rtXdot ->
integrator_CSTATE_a5a = _rtB -> B_392_3387_0 ; _rtXdot ->
integrator_CSTATE_jo = _rtB -> B_392_3389_0 ; _rtXdot -> integrator_CSTATE_bj
= _rtB -> B_392_3391_0 ; _rtXdot -> integrator_CSTATE_bu = _rtB ->
B_392_3393_0 ; _rtXdot -> integrator_CSTATE_df = _rtB -> B_392_3395_0 ;
_rtXdot -> integrator_CSTATE_pe = _rtB -> B_392_3397_0 ; _rtXdot ->
integrator_CSTATE_di = _rtB -> B_392_3399_0 ; _rtXdot -> integrator_CSTATE_gi
= _rtB -> B_392_3401_0 ; _rtXdot -> integrator_CSTATE_fd = _rtB ->
B_392_3403_0 ; _rtXdot -> integrator_CSTATE_i2 = _rtB -> B_392_3405_0 ;
_rtXdot -> integrator_CSTATE_kz = _rtB -> B_392_3407_0 ; _rtXdot ->
integrator_CSTATE_nf = _rtB -> B_392_3409_0 ; _rtXdot -> integrator_CSTATE_dl
= _rtB -> B_392_3411_0 ; _rtXdot -> integrator_CSTATE_kf = _rtB ->
B_392_3413_0 ; _rtXdot -> integrator_CSTATE_k4 = _rtB -> B_392_3415_0 ;
_rtXdot -> integrator_CSTATE_h4 = _rtB -> B_392_3417_0 ; _rtXdot ->
integrator_CSTATE_a2 = _rtB -> B_392_3419_0 ; _rtXdot -> integrator_CSTATE_mh
= _rtB -> B_392_3372_0 ; _rtXdot -> integrator_CSTATE_fs = _rtB ->
B_392_3374_0 ; _rtXdot -> integrator_CSTATE_bdm = _rtB -> B_392_3376_0 ;
_rtXdot -> integrator_CSTATE_hn = _rtB -> B_392_3378_0 ; _rtXdot ->
integrator_CSTATE_nm2 = _rtB -> B_392_3380_0 ; _rtXdot ->
integrator_CSTATE_f3 = _rtB -> B_392_3382_0 ; _rtXdot -> integrator_CSTATE_dp
= _rtB -> B_392_3837_0 ; _rtXdot -> integrator_CSTATE_hz = _rtB ->
B_392_3839_0 ; _rtXdot -> integrator_CSTATE_oa = _rtB -> B_392_3841_0 ;
_rtXdot -> integrator_CSTATE_ow = _rtB -> B_392_3843_0 ; _rtXdot ->
integrator_CSTATE_n3 = _rtB -> B_392_3845_0 ; _rtXdot ->
integrator_CSTATE_dn3 = _rtB -> B_392_3847_0 ; _rtXdot ->
integrator_CSTATE_lx = _rtB -> B_392_3849_0 ; _rtXdot -> integrator_CSTATE_am
= _rtB -> B_392_3851_0 ; _rtXdot -> integrator_CSTATE_gew = _rtB ->
B_392_3853_0 ; _rtXdot -> integrator_CSTATE_br = _rtB -> B_392_3855_0 ;
_rtXdot -> integrator_CSTATE_bl = _rtB -> B_392_3857_0 ; _rtXdot ->
integrator_CSTATE_oe = _rtB -> B_392_3859_0 ; _rtXdot -> integrator_CSTATE_k3
= _rtB -> B_392_3861_0 ; _rtXdot -> integrator_CSTATE_ay = _rtB ->
B_392_3863_0 ; _rtXdot -> integrator_CSTATE_hx = _rtB -> B_392_3865_0 ;
_rtXdot -> integrator_CSTATE_nc = _rtB -> B_392_3867_0 ; _rtXdot ->
integrator_CSTATE_jf = _rtB -> B_392_3869_0 ; _rtXdot -> integrator_CSTATE_mc
= _rtB -> B_392_3871_0 ; _rtXdot -> integrator_CSTATE_av = _rtB ->
B_392_3824_0 ; _rtXdot -> integrator_CSTATE_gy = _rtB -> B_392_3826_0 ;
_rtXdot -> integrator_CSTATE_i3 = _rtB -> B_392_3828_0 ; _rtXdot ->
integrator_CSTATE_kg = _rtB -> B_392_3830_0 ; _rtXdot ->
integrator_CSTATE_fqj = _rtB -> B_392_3832_0 ; _rtXdot ->
integrator_CSTATE_gii = _rtB -> B_392_3834_0 ; _rtXdot -> LPF1_CSTATE_j = 0.0
; _rtXdot -> LPF1_CSTATE_j += _rtP -> P_2875 * _rtX -> LPF1_CSTATE_j ;
_rtXdot -> LPF1_CSTATE_j += _rtB -> B_392_4235_0 ; _rtXdot -> LPF2_CSTATE_m5
= 0.0 ; _rtXdot -> LPF2_CSTATE_m5 += _rtP -> P_2877 * _rtX -> LPF2_CSTATE_m5
; _rtXdot -> LPF2_CSTATE_m5 += _rtB -> B_392_4239_0 ; _rtXdot ->
integrator_CSTATE_fn = _rtB -> B_392_4310_0 ; _rtXdot -> integrator_CSTATE_ox
= _rtB -> B_392_4312_0 ; _rtXdot -> integrator_CSTATE_clt = _rtB ->
B_392_4314_0 ; _rtXdot -> integrator_CSTATE_bo = _rtB -> B_392_4316_0 ;
_rtXdot -> integrator_CSTATE_jm = _rtB -> B_392_4318_0 ; _rtXdot ->
integrator_CSTATE_gv = _rtB -> B_392_4320_0 ; _rtXdot -> integrator_CSTATE_cr
= _rtB -> B_392_4323_0 ; _rtXdot -> integrator_CSTATE_gm = _rtB ->
B_392_4325_0 ; _rtXdot -> integrator_CSTATE_l3 = _rtB -> B_392_4327_0 ;
_rtXdot -> integrator_CSTATE_hm = _rtB -> B_392_4329_0 ; _rtXdot ->
integrator_CSTATE_co = _rtB -> B_392_4331_0 ; _rtXdot ->
integrator_CSTATE_knp = _rtB -> B_392_4333_0 ; _rtXdot ->
Integradord_CSTATE_g = 0.0 ; _rtXdot -> Integradord_CSTATE_g += _rtP ->
P_2971 * _rtX -> Integradord_CSTATE_g ; _rtXdot -> Integradord_CSTATE_g +=
_rtB -> B_392_4152_0 ; _rtXdot -> Integradord1_CSTATE_m = 0.0 ; _rtXdot ->
Integradord1_CSTATE_m += _rtP -> P_2979 * _rtX -> Integradord1_CSTATE_m ;
_rtXdot -> Integradord1_CSTATE_m += _rtB -> B_392_4156_0 ; _rtXdot ->
Controled_CSTATE = 0.0 ; _rtXdot -> Controled_CSTATE += _rtP -> P_2983 * _rtX
-> Controled_CSTATE ; _rtXdot -> Controled_CSTATE += _rtB -> B_392_4116_0 ;
_rtXdot -> Controleq_CSTATE = 0.0 ; _rtXdot -> Controleq_CSTATE += _rtP ->
P_2986 * _rtX -> Controleq_CSTATE ; _rtXdot -> Controleq_CSTATE += _rtB ->
B_392_4118_0 ; _rtXdot -> LPF1_CSTATE_a = 0.0 ; _rtXdot -> LPF1_CSTATE_a +=
_rtP -> P_3145 * _rtX -> LPF1_CSTATE_a ; _rtXdot -> LPF1_CSTATE_a += _rtB ->
B_392_4769_0 ; _rtXdot -> LPF2_CSTATE_d = 0.0 ; _rtXdot -> LPF2_CSTATE_d +=
_rtP -> P_3147 * _rtX -> LPF2_CSTATE_d ; _rtXdot -> LPF2_CSTATE_d += _rtB ->
B_392_4773_0 ; _rtXdot -> integrator_CSTATE_ib = _rtB -> B_392_4844_0 ;
_rtXdot -> integrator_CSTATE_j3 = _rtB -> B_392_4846_0 ; _rtXdot ->
integrator_CSTATE_bx = _rtB -> B_392_4848_0 ; _rtXdot -> integrator_CSTATE_h0
= _rtB -> B_392_4850_0 ; _rtXdot -> integrator_CSTATE_pa = _rtB ->
B_392_4852_0 ; _rtXdot -> integrator_CSTATE_ew = _rtB -> B_392_4854_0 ;
_rtXdot -> integrator_CSTATE_hh = _rtB -> B_392_4857_0 ; _rtXdot ->
integrator_CSTATE_fcw = _rtB -> B_392_4859_0 ; _rtXdot ->
integrator_CSTATE_kg1 = _rtB -> B_392_4861_0 ; _rtXdot ->
integrator_CSTATE_it = _rtB -> B_392_4863_0 ; _rtXdot -> integrator_CSTATE_fg
= _rtB -> B_392_4865_0 ; _rtXdot -> integrator_CSTATE_jb = _rtB ->
B_392_4867_0 ; _rtXdot -> Integradord_CSTATE_an = 0.0 ; _rtXdot ->
Integradord_CSTATE_an += _rtP -> P_3241 * _rtX -> Integradord_CSTATE_an ;
_rtXdot -> Integradord_CSTATE_an += _rtB -> B_392_4686_0 ; _rtXdot ->
Integradord1_CSTATE_c = 0.0 ; _rtXdot -> Integradord1_CSTATE_c += _rtP ->
P_3249 * _rtX -> Integradord1_CSTATE_c ; _rtXdot -> Integradord1_CSTATE_c +=
_rtB -> B_392_4690_0 ; _rtXdot -> Controled_CSTATE_o = 0.0 ; _rtXdot ->
Controled_CSTATE_o += _rtP -> P_3253 * _rtX -> Controled_CSTATE_o ; _rtXdot
-> Controled_CSTATE_o += _rtB -> B_392_4650_0 ; _rtXdot -> Controleq_CSTATE_j
= 0.0 ; _rtXdot -> Controleq_CSTATE_j += _rtP -> P_3256 * _rtX ->
Controleq_CSTATE_j ; _rtXdot -> Controleq_CSTATE_j += _rtB -> B_392_4652_0 ;
_rtXdot -> integrator_CSTATE_el = _rtB -> B_392_5356_0 ; _rtXdot ->
integrator_CSTATE_ax = _rtB -> B_392_5358_0 ; _rtXdot -> integrator_CSTATE_pm
= _rtB -> B_392_5360_0 ; _rtXdot -> integrator_CSTATE_dy = _rtB ->
B_392_5362_0 ; _rtXdot -> integrator_CSTATE_kk = _rtB -> B_392_5364_0 ;
_rtXdot -> integrator_CSTATE_pg = _rtB -> B_392_5366_0 ; _rtXdot ->
integrator_CSTATE_hrp = _rtB -> B_392_5368_0 ; _rtXdot ->
integrator_CSTATE_bg = _rtB -> B_392_5370_0 ; _rtXdot -> integrator_CSTATE_dh
= _rtB -> B_392_5372_0 ; _rtXdot -> integrator_CSTATE_fof = _rtB ->
B_392_5374_0 ; _rtXdot -> integrator_CSTATE_l5v = _rtB -> B_392_5376_0 ;
_rtXdot -> integrator_CSTATE_jw = _rtB -> B_392_5378_0 ; _rtXdot ->
integrator_CSTATE_cfo = _rtB -> B_392_5380_0 ; _rtXdot ->
integrator_CSTATE_hqe = _rtB -> B_392_5382_0 ; _rtXdot ->
integrator_CSTATE_nt = _rtB -> B_392_5384_0 ; _rtXdot ->
integrator_CSTATE_hqx = _rtB -> B_392_5386_0 ; _rtXdot ->
integrator_CSTATE_gq = _rtB -> B_392_5388_0 ; _rtXdot ->
integrator_CSTATE_cvp = _rtB -> B_392_5390_0 ; _rtXdot ->
integrator_CSTATE_cs = _rtB -> B_392_5343_0 ; _rtXdot -> integrator_CSTATE_oh
= _rtB -> B_392_5345_0 ; _rtXdot -> integrator_CSTATE_mi = _rtB ->
B_392_5347_0 ; _rtXdot -> integrator_CSTATE_m4 = _rtB -> B_392_5349_0 ;
_rtXdot -> integrator_CSTATE_g2y = _rtB -> B_392_5351_0 ; _rtXdot ->
integrator_CSTATE_kf0 = _rtB -> B_392_5353_0 ; }
#define MDL_ZERO_CROSSINGS
static void mdlZeroCrossings ( SimStruct * S ) { B_MR_CIGRE_GENERALIZADO_T *
_rtB ; P_MR_CIGRE_GENERALIZADO_T * _rtP ; ZCV_MR_CIGRE_GENERALIZADO_T *
_rtZCSV ; _rtZCSV = ( ( ZCV_MR_CIGRE_GENERALIZADO_T * )
ssGetSolverZcSignalVector ( S ) ) ; _rtP = ( ( P_MR_CIGRE_GENERALIZADO_T * )
ssGetModelRtp ( S ) ) ; _rtB = ( ( B_MR_CIGRE_GENERALIZADO_T * )
_ssGetModelBlockIO ( S ) ) ; _rtZCSV -> Saturation_UprLim_ZC = _rtB ->
B_392_4_0 - _rtP -> P_415 ; _rtZCSV -> Saturation_LwrLim_ZC = _rtB ->
B_392_4_0 - _rtP -> P_416 ; _rtZCSV -> PQparaVf_StepTime_ZC = ssGetT ( S ) -
_rtP -> P_422 ; _rtZCSV -> Saturation_UprLim_ZC_o = _rtB -> B_392_42_0 - _rtP
-> P_443 ; _rtZCSV -> Saturation_LwrLim_ZC_n = _rtB -> B_392_42_0 - _rtP ->
P_444 ; _rtZCSV -> ref_StepTime_ZC = ssGetT ( S ) - _rtP -> P_579 ; _rtZCSV
-> Time_init_ref_StepTime_ZC = ssGetT ( S ) - _rtP -> P_583 ; _rtZCSV ->
ref_StepTime_ZC_m = ssGetT ( S ) - _rtP -> P_683 ; _rtZCSV ->
Time_init_ref_StepTime_ZC_p = ssGetT ( S ) - _rtP -> P_687 ; _rtZCSV ->
ref2_StepTime_ZC = ssGetT ( S ) - _rtP -> P_826 ; _rtZCSV ->
Saturation_UprLim_ZC_oi = _rtB -> B_392_718_0 - _rtP -> P_865 ; _rtZCSV ->
Saturation_LwrLim_ZC_o = _rtB -> B_392_718_0 - _rtP -> P_866 ; _rtZCSV ->
ref2_StepTime_ZC_k = ssGetT ( S ) - _rtP -> P_1021 ; _rtZCSV ->
Saturation_UprLim_ZC_d = _rtB -> B_392_1072_0 - _rtP -> P_1060 ; _rtZCSV ->
Saturation_LwrLim_ZC_c = _rtB -> B_392_1072_0 - _rtP -> P_1061 ; _rtZCSV ->
IlhamentoChavefechadaparachaveabertaemtilha_StepTime_ZC = ssGetT ( S ) - _rtP
-> P_1383 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC = _rtB -> B_392_1608_0 -
_rtP -> P_1397 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_b = _rtB ->
B_392_1620_0 - _rtP -> P_1404 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_d =
_rtB -> B_392_1632_0 - _rtP -> P_1411 ; _rtZCSV -> Step_StepTime_ZC = ssGetT
( S ) - _rtP -> P_1412 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_n = _rtB ->
B_392_2130_0 - _rtP -> P_1707 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_i =
_rtB -> B_392_2142_0 - _rtP -> P_1714 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_dd = _rtB -> B_392_2154_0 - _rtP -> P_1721 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_f = _rtB -> B_392_2982_0 - _rtP ->
P_2270 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_c = _rtB -> B_392_2994_0 -
_rtP -> P_2277 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nb = _rtB ->
B_392_3006_0 - _rtP -> P_2284 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_g =
_rtB -> B_392_3434_0 - _rtP -> P_2565 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_l = _rtB -> B_392_3446_0 - _rtP -> P_2572 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_h = _rtB -> B_392_3458_0 - _rtP ->
P_2579 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nl = _rtB -> B_392_3886_0 -
_rtP -> P_2860 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_nt = _rtB ->
B_392_3898_0 - _rtP -> P_2867 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_e =
_rtB -> B_392_3910_0 - _rtP -> P_2874 ; _rtZCSV -> Saturation_UprLim_ZC_m =
_rtB -> B_392_4112_0 - _rtP -> P_2981 ; _rtZCSV -> Saturation_LwrLim_ZC_j =
_rtB -> B_392_4112_0 - _rtP -> P_2982 ; _rtZCSV -> Saturation_UprLim_ZC_k =
_rtB -> B_392_4146_0 - _rtP -> P_3002 ; _rtZCSV -> Saturation_LwrLim_ZC_i =
_rtB -> B_392_4146_0 - _rtP -> P_3003 ; _rtZCSV -> Saturation1_UprLim_ZC =
_rtB -> B_392_4142_0 - _rtP -> P_3004 ; _rtZCSV -> Saturation1_LwrLim_ZC =
_rtB -> B_392_4142_0 - _rtP -> P_3005 ; _rtZCSV -> Saturation_UprLim_ZC_kc =
_rtB -> B_392_4646_0 - _rtP -> P_3251 ; _rtZCSV -> Saturation_LwrLim_ZC_ol =
_rtB -> B_392_4646_0 - _rtP -> P_3252 ; _rtZCSV -> Saturation_UprLim_ZC_ky =
_rtB -> B_392_4680_0 - _rtP -> P_3272 ; _rtZCSV -> Saturation_LwrLim_ZC_nr =
_rtB -> B_392_4680_0 - _rtP -> P_3273 ; _rtZCSV -> Saturation1_UprLim_ZC_d =
_rtB -> B_392_4676_0 - _rtP -> P_3274 ; _rtZCSV -> Saturation1_LwrLim_ZC_a =
_rtB -> B_392_4676_0 - _rtP -> P_3275 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_fz = _rtB -> B_392_5405_0 - _rtP -> P_3695 ;
_rtZCSV -> HitCrossing_HitNoOutput_ZC_gc = _rtB -> B_392_5417_0 - _rtP ->
P_3702 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_fh = _rtB -> B_392_5429_0 -
_rtP -> P_3709 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_ev = _rtB ->
B_392_5459_0 - _rtP -> P_3722 ; _rtZCSV -> HitCrossing_HitNoOutput_ZC_in =
_rtB -> B_392_5471_0 - _rtP -> P_3729 ; _rtZCSV ->
HitCrossing_HitNoOutput_ZC_a = _rtB -> B_392_5483_0 - _rtP -> P_3736 ; }
static void mdlInitializeSizes ( SimStruct * S ) { ssSetChecksumVal ( S , 0 ,
1643708805U ) ; ssSetChecksumVal ( S , 1 , 3076651386U ) ; ssSetChecksumVal (
S , 2 , 4011859333U ) ; ssSetChecksumVal ( S , 3 , 1161657196U ) ; { mxArray
* slVerStructMat = NULL ; mxArray * slStrMat = mxCreateString ( "simulink" )
; char slVerChar [ 10 ] ; int status = mexCallMATLAB ( 1 , & slVerStructMat ,
1 , & slStrMat , "ver" ) ; if ( status == 0 ) { mxArray * slVerMat =
mxGetField ( slVerStructMat , 0 , "Version" ) ; if ( slVerMat == NULL ) {
status = 1 ; } else { status = mxGetString ( slVerMat , slVerChar , 10 ) ; }
} mxDestroyArray ( slStrMat ) ; mxDestroyArray ( slVerStructMat ) ; if ( (
status == 1 ) || ( strcmp ( slVerChar , "9.0" ) != 0 ) ) { return ; } }
ssSetOptions ( S , SS_OPTION_EXCEPTION_FREE_CODE ) ; if ( ssGetSizeofDWork (
S ) != sizeof ( DW_MR_CIGRE_GENERALIZADO_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal DWork sizes do "
"not match for accelerator mex file." ) ; } if ( ssGetSizeofGlobalBlockIO ( S
) != sizeof ( B_MR_CIGRE_GENERALIZADO_T ) ) { ssSetErrorStatus ( S ,
"Unexpected error: Internal BlockIO sizes do "
"not match for accelerator mex file." ) ; } { int ssSizeofParams ;
ssGetSizeofParams ( S , & ssSizeofParams ) ; if ( ssSizeofParams != sizeof (
P_MR_CIGRE_GENERALIZADO_T ) ) { static char msg [ 256 ] ; sprintf ( msg ,
"Unexpected error: Internal Parameters sizes do "
"not match for accelerator mex file." ) ; } } _ssSetModelRtp ( S , ( real_T *
) & MR_CIGRE_GENERALIZADO_rtDefaultP ) ; rt_InitInfAndNaN ( sizeof ( real_T )
) ; ( ( P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_58 = rtInf ;
( ( P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_114 = rtInf ; (
( P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_336 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_394 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_910 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_913 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_1103 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_1106 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3056 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3059 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3138 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3142 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3326 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3329 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3408 = rtInf ; ( (
P_MR_CIGRE_GENERALIZADO_T * ) ssGetModelRtp ( S ) ) -> P_3412 = rtInf ; }
static void mdlInitializeSampleTimes ( SimStruct * S ) { { SimStruct * childS
; SysOutputFcn * callSysFcns ; childS = ssGetSFunction ( S , 1 ) ;
callSysFcns = ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ]
= ( SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 4 ) ; callSysFcns
= ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 5 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 8 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 9 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; childS = ssGetSFunction ( S , 12 ) ; callSysFcns =
ssGetCallSystemOutputFcnList ( childS ) ; callSysFcns [ 3 + 0 ] = (
SysOutputFcn ) ( NULL ) ; } slAccRegPrmChangeFcn ( S , mdlOutputsTID4 ) ; }
static void mdlTerminate ( SimStruct * S ) { }
#include "simulink.c"
