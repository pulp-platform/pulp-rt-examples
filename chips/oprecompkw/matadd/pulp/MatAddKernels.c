#include "MatAddKernels.h"
#include "rt/rt_api.h"

RT_L1_GLOBAL_DATA char *L1_Memory;
RT_L2_DATA char *L2_Memory;
void MatADD(
		uint64_t  In1,
		uint64_t  In2,
		uint64_t  Out)

{
	/* Shared L1: 43200 bytes, L2 buffer: 0 bytes */
	/* Local variables used by this kernel */
	int DmaR_Evt1;
	int DmaR_Evt2;
	int DmaW_Evt1;
	MatrixAdd_Arg_T S_KerArg0, *KerArg0 = &S_KerArg0;

	/* Iteration space related variables */
	int T0Ind, T0Ind_Total=0, T0Ind_Last, T0Ind_NextLast;
	/* User kernel arguments related variables */
	unsigned int _N_In1;
	unsigned int _SN_In1;
	unsigned int _N_In2;
	unsigned int _SN_In2;
	unsigned int _C_Out;
	unsigned int _SP_Out, _SC_Out;
	/*============================= Ker Arg Iter Spaces =========================================
	User Kernel Iteration Space:
		[Tile0 Dim: 6]
	Ker Arg: In1, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 6 logical tiles, 6 physical tiles
			Total Size: 40000 [Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		Tile0: [0, 7200, 7200], Tile1: [7200, 7200, 7200], Tile2; [14400, 7200, 7200]
		T0: [Tile0: 0], T1: [Tile0: 1], T2: [Tile0: 2]
	Ker Arg: In2, Tiled Space: Tile0
		Min Pipe Depth: 0, Max Pipe Depth: 1
		KerArgItSpace: 6 logical tiles, 6 physical tiles
			Total Size: 40000 [Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		Tile0: [0, 7200, 7200], Tile1: [7200, 7200, 7200], Tile2; [14400, 7200, 7200]
		T0: [Tile0: 0], T1: [Tile0: 1], T2: [Tile0: 2]
	Ker Arg: Out, Tiled Space: Tile0
		Min Pipe Depth: -1, Max Pipe Depth: 0
		KerArgItSpace: 6 logical tiles, 6 physical tiles
			Total Size: 40000 [Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		KerArgItSpace (User Kernel Iter Order):
			[Tile0, 6:[100x18, 4:100x18, 100x10], 4]
		Tile0: [0, 7200, 7200], Tile1: [7200, 7200, 7200], Tile2; [14400, 7200, 7200]
		T0: [Tile0: 0], T1: [Tile0: 1], T2: [Tile0: 2]
	======================== End Ker Arg Iter Spaces =========================================*/
	/*=========================== Call Kernel, Invariant assignment =====================*/
	KerArg0->W = (unsigned int) (100);
	/*================================= Read Tiles Prolog ===============================*/
	_N_In1=0;
	DmaR_Evt1 = plp_dma_memcpy((In1+0), ((uint32_t) L1_Memory+0+0), 7200, 1);
	_N_In2=0;
	DmaR_Evt2 = plp_dma_memcpy((In2+0), ((uint32_t) L1_Memory+14400+0), 7200, 1);
	_C_Out=0; _SC_Out=7200;
	_SP_Out=0;
	/*============================= End Read Tiles Prolog ===============================*/
	for (T0Ind=0; T0Ind<6; T0Ind++, T0Ind_Total++) { /* Iteration on Tile0 */
		int T0Ind_Last = (T0Ind==5), T0Ind_NextLast = ((T0Ind+1)==5);
		/*================================= Prepare Tiles ===================================*/
		_SN_In1 = 0;
		if (!(T0Ind_Last)) {
			_N_In1 = _N_In1 + (7200); _SN_In1 = ((T0Ind_NextLast)?4000:7200); 
		}
		_SN_In2 = 0;
		if (!(T0Ind_Last)) {
			_N_In2 = _N_In2 + (7200); _SN_In2 = ((T0Ind_NextLast)?4000:7200); 
		}
		/*============================= End Prepare Tiles ===================================*/
		/*================================= Read Tiles ======================================*/
		plp_dma_wait(DmaR_Evt1); /* Wait previous DMA read In1 */
		if (_SN_In1) {
			DmaR_Evt1 = plp_dma_memcpy(( In1+_N_In1), ((uint32_t) L1_Memory+0+7200*((T0Ind_Total+1)%2)), _SN_In1, 1);
		}
		plp_dma_wait(DmaR_Evt2); /* Wait previous DMA read In2 */
		if (_SN_In2) {
			DmaR_Evt2 = plp_dma_memcpy(( In2+_N_In2), ((uint32_t) L1_Memory+14400+7200*((T0Ind_Total+1)%2)), _SN_In2, 1);
		}
		/*============================= End Read Tiles ======================================*/
		/*====================== Call Kernel LOC_LOOP =========================*/
		KerArg0->In1 = (int * __restrict__) (L1_Memory+0+7200*((T0Ind_Total)%2));
		KerArg0->In2 = (int * __restrict__) (L1_Memory+14400+7200*((T0Ind_Total)%2));
		KerArg0->Out = (int * __restrict__) (L1_Memory+28800+7200*((T0Ind_Total)%2));
		KerArg0->H = (unsigned int) (T0Ind_Last?10:18);
		rt_team_fork(rt_nb_pe(), (void *) MatSumPar, (void *) KerArg0);
		/*================================= Write Tiles =====================================*/
		if (_SP_Out) plp_dma_wait(DmaW_Evt1); /* Wait previous DMA write Out */
		DmaW_Evt1 = plp_dma_memcpy(( Out+_C_Out), ((uint32_t) L1_Memory+28800+7200*((T0Ind_Total)%2)),
				_SC_Out, 0);
		/*============================= End Write Tiles =====================================*/
		/*================================= Update Arg Pipeline =============================*/
		
		
		_SP_Out = _SC_Out;
		/*============================= End Update Arg Pipeline =============================*/
		/*================================= Prepare Tiles ===================================*/
		_SC_Out = 0;
		if (!(T0Ind_Last)) {
			_C_Out = _C_Out + (7200); _SC_Out = ((T0Ind_NextLast)?4000:7200); 
		}
		/*============================= End Prepare Tiles ===================================*/
	} /* End iteration on Tile0 */
	/*================================ Write Tiles Epilog ===============================*/
	plp_dma_wait(DmaW_Evt1); /* Wait previous DMA write Out */
	/*============================ End Write Tiles Epilog ===============================*/
}
