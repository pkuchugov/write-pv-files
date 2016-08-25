
#ifndef __MAIN_H__
#define __MAIN_H__
	
	#include <stdio.h>
	#include <stdlib.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>
	
	#include <mpi.h>
	
	typedef struct
	{
		double *X;
		double *Y;
		double *Z;
	} mesh_t;
	
	typedef struct
	{
		double *data;
	} state_t;
	
	typedef struct
	{
		int rank;
		int rank_x;
		int rank_y;
		int rank_z;
		int nranks;
		int ncells_x;
		int ncells_y;
		int ncells_z;
		int npoints_x;
		int npoints_y;
		int npoints_z;
		int offset_x;
		int offset_y;
		int offset_z;
		int limit_x;
		int limit_y;
		int limit_z;
	} info_t;

	extern int    Nx, Ny, Nz;
	extern int    NPX, NPY, NPZ;
	extern info_t info;
	
	extern int  compressData(const void *src, int srcLen, void *dst, int dstLen);
	extern int  uncompressData(const void *src, int srcLen, void *dst, int dstLen);
	extern int  getMaxCompressedLen( int srcLen );
	
	extern int  write_pv_files_collection(mesh_t, state_t);
	
#endif /* __MAIN_H__ */
