#include "__cf_MR_CIGRE_RESISTIVO.h"
#ifndef RTW_HEADER_MR_CIGRE_RESISTIVO_acc_private_h_
#define RTW_HEADER_MR_CIGRE_RESISTIVO_acc_private_h_
#include "rtwtypes.h"
#include "multiword_types.h"
#include "MR_CIGRE_RESISTIVO_acc.h"
#if !defined(ss_VALIDATE_MEMORY)
#define ss_VALIDATE_MEMORY(S, ptr)   if(!(ptr)) {\
  ssSetErrorStatus(S, RT_MEMORY_ALLOCATION_ERROR);\
  }
#endif
#if !defined(rt_FREE)
#if !defined(_WIN32)
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((ptr));\
  (ptr) = (NULL);\
  }
#else
#define rt_FREE(ptr)   if((ptr) != (NULL)) {\
  free((void *)(ptr));\
  (ptr) = (NULL);\
  }
#endif
#endif
#ifndef __RTW_UTFREE__
extern void * utMalloc ( size_t ) ; extern void utFree ( void * ) ;
#endif
boolean_T MR_CIGRE_RESISTIVO_acc_rt_TDelayUpdateTailOrGrowBuf ( int_T *
bufSzPtr , int_T * tailPtr , int_T * headPtr , int_T * lastPtr , real_T
tMinusDelay , real_T * * tBufPtr , real_T * * uBufPtr , real_T * * xBufPtr ,
boolean_T isfixedbuf , boolean_T istransportdelay , int_T * maxNewBufSzPtr )
; real_T MR_CIGRE_RESISTIVO_acc_rt_TDelayInterpolate ( real_T tMinusDelay ,
real_T tStart , real_T * tBuf , real_T * uBuf , int_T bufSz , int_T * lastIdx
, int_T oldestIdx , int_T newIdx , real_T initOutput , boolean_T discrete ,
boolean_T minorStepAndTAtLastMajorOutput ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_Disable (
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay ( SimStruct * S , uint8_T rtu_Enable ,
const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_Disable ( DW_Subsystem1_MR_CIGRE_RESISTIVO_T *
localDW ) ; void MR_CIGRE_RESISTIVO_Subsystem1 ( SimStruct * S , uint8_T
rtu_Enable , const real_T rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_RESISTIVO_T * localB ,
DW_Subsystem1_MR_CIGRE_RESISTIVO_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_n_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_j_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_h_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_m ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_j_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_k_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_a_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_n_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_l_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_b ( uint8_T rtu_Enable , const real_T
rtu_alpha_beta [ 2 ] , real_T rtu_wt , B_Subsystem1_MR_CIGRE_RESISTIVO_n_T *
localB , DW_Subsystem1_MR_CIGRE_RESISTIVO_i_T * localDW ) ; void
MR_CIGRE_RESISTIVO_NegSeqComputation_Disable (
DW_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ) ; void
MR_CIGRE_RESISTIVO_NegSeqComputation ( SimStruct * S , real_T rtu_Enable ,
creal_T rtu_In , creal_T rtu_In_p , creal_T rtu_In_d ,
B_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localB ,
DW_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ,
P_NegSeqComputation_MR_CIGRE_RESISTIVO_T * localP ) ; void
MR_CIGRE_RESISTIVO_ZeroSeqComputation_Disable (
DW_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ) ; void
MR_CIGRE_RESISTIVO_ZeroSeqComputation ( SimStruct * S , real_T rtu_Enable ,
creal_T rtu_In , creal_T rtu_In_l , creal_T rtu_In_n ,
B_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localB ,
DW_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localDW ,
P_ZeroSeqComputation_MR_CIGRE_RESISTIVO_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_g_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_f_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_p_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_l_Disable (
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_o_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_o ( SimStruct * S , uint8_T rtu_Enable ,
real_T rtu_dq , real_T rtu_dq_j , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_f_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_o_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_j_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_nf_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_n_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_c_Disable (
DW_Subsystem1_MR_CIGRE_RESISTIVO_g_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_e ( SimStruct * S , uint8_T rtu_Enable , real_T
rtu_dq , real_T rtu_dq_m , real_T rtu_wt ,
B_Subsystem1_MR_CIGRE_RESISTIVO_nf_T * localB ,
DW_Subsystem1_MR_CIGRE_RESISTIVO_g_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_h_Init (
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_c_T * localB ,
P_Subsystempi2delay_MR_CIGRE_RESISTIVO_e_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystempi2delay_l ( SimStruct * S , uint8_T rtu_Enable ,
const real_T rtu_dq [ 2 ] , real_T rtu_wt ,
B_Subsystempi2delay_MR_CIGRE_RESISTIVO_c_T * localB ,
DW_Subsystempi2delay_MR_CIGRE_RESISTIVO_g_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_e_Init ( B_Subsystem1_MR_CIGRE_RESISTIVO_g_T *
localB , P_Subsystem1_MR_CIGRE_RESISTIVO_lv_T * localP ) ; void
MR_CIGRE_RESISTIVO_Subsystem1_l ( SimStruct * S , uint8_T rtu_Enable , const
real_T rtu_dq [ 2 ] , real_T rtu_wt , B_Subsystem1_MR_CIGRE_RESISTIVO_g_T *
localB , DW_Subsystem1_MR_CIGRE_RESISTIVO_m_T * localDW ) ; void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Init (
SimStruct * S ) ; void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1_Update (
SimStruct * S ) ; void
MR_CIGRE_RESISTIVO_Synthesized_Atomic_Subsystem_For_Alg_Loop_1 ( SimStruct *
S ) ;
#endif
