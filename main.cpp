
/*
 * Author: Pavel Kuchugov (pkuchugov at gmail dot com)
 * Date  : 2016-08-24
 */

#include "main.h"

int    Nx, Ny, Nz;
int    NPX, NPY, NPZ;
info_t info;

int main(int argc, char *argv[])
{
	register int i, j, k, idx;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &(info.rank));
	MPI_Comm_size(MPI_COMM_WORLD, &(info.nranks));
	
	NPX = 3;
	NPY = 1;
	NPZ = 1;
	
	if (info.nranks != (NPX * NPY * NPZ))	
	{
		if (info.rank == 0)
			fprintf(stderr, "You should run this program on another number of processes.\n");
		
		return -1;
	}
	
	Nx = 50;
	Ny = 50;
	Nz = 50;
	
	double Xmin = 0.0;
	double Xmax = 1.0;
	double Ymin = 0.0;
	double Ymax = 1.0;
	double Zmin = 0.0;
	double Zmax = 1.0;
	
	double Hx = (Xmax - Xmin) / Nx;
	double Hy = (Ymax - Ymin) / Ny;
	double Hz = (Zmax - Zmin) / Nz;
	
	info.rank_z =  info.rank / (NPX * NPY);
	info.rank_y = (info.rank - NPX * NPY * info.rank_z) / NPX;
	info.rank_x = (info.rank - NPX * info.rank_y - NPX * NPY * info.rank_z);
	
	info.ncells_x = (info.rank_x == (NPX - 1)) ? Nx - (Nx / NPX) * (NPX - 1) : Nx / NPX;
	info.ncells_y = (info.rank_y == (NPY - 1)) ? Ny - (Ny / NPY) * (NPY - 1) : Ny / NPY;
	info.ncells_z = (info.rank_z == (NPZ - 1)) ? Nz - (Nz / NPZ) * (NPZ - 1) : Nz / NPZ;
	
	info.offset_x = Nx / NPX * info.rank_x + 1;
	info.offset_y = Ny / NPY * info.rank_y + 1;
	info.offset_z = Nz / NPZ * info.rank_z + 1;
	
	info.limit_x = (info.rank_x == (NPX - 1)) ? Nx : (info.rank_x + 1) * (Nx / NPX);
	info.limit_y = (info.rank_y == (NPY - 1)) ? Ny : (info.rank_y + 1) * (Ny / NPY);
	info.limit_z = (info.rank_z == (NPZ - 1)) ? Nz : (info.rank_z + 1) * (Nz / NPZ);
	
	info.npoints_x = info.ncells_x + 1;
	info.npoints_y = info.ncells_y + 1;
	info.npoints_z = info.ncells_z + 1;
		
	/* Allocate memory for mesh */
	mesh_t mesh;
	mesh.X = (double *)malloc(info.npoints_x * sizeof(double));
	mesh.Y = (double *)malloc(info.npoints_y * sizeof(double));
	mesh.Z = (double *)malloc(info.npoints_z * sizeof(double));	
	
	/* Allocate memory for data */
	state_t flowState;
	flowState.data = (double *)malloc(info.ncells_x * info.ncells_y * info.ncells_z * sizeof(double));
	
	/* Initialize mesh and data */
	for (i = 0; i < info.npoints_x; i++)
	{
		mesh.X[i] = Xmin + (i + info.offset_x - 1) * Hx;
	}
	
	for (j = 0; j < info.npoints_y; j++)
	{
		mesh.Y[j] = Ymin + (j + info.offset_y - 1) * Hy;
	}
	
	for (k = 0; k < info.npoints_z; k++)
	{
		mesh.Z[k] = Zmin + (k + info.offset_z - 1) * Hz;
	}
	
	for (k = 0; k < info.ncells_z; k++)
	{
		for (j = 0; j < info.ncells_y; j++)
		{
			for (i = 0; i < info.ncells_x; i++)
			{
				idx = i + info.ncells_x * j + info.ncells_x * info.ncells_y * k;
				flowState.data[idx] = (double)(i + info.offset_x - 1 + j + info.offset_y - 1 + k + info.offset_z - 1);
			}
		}
	}
	
	write_pv_files_collection(mesh, flowState);
	
	if (info.rank == 0)
		fprintf(stdout, "All was done!\n");
	
	/* Release alloacated memory */
	free(mesh.X);
	free(mesh.Y);
	free(mesh.Z);
	free(flowState.data);
	
	MPI_Finalize();
	
	return 0;
}

