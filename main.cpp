
/*
 * Author: Pavel Kuchugov (pkuchugov at gmail dot com)
 * Date  : 2016-08-24
 */

#include "main.h"

/* Global number of cells along each direction in Cartesian coordinate system */
int    nx, ny, nz;
/* Number of subdomains along each direction */
int    nsd_x, nsd_y, nsd_z;
/* Additional grid data on each MPI-process */
info_t info;

int main(int argc, char *argv[])
{
	register int i, j, k, idx;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &(info.rank));
	MPI_Comm_size(MPI_COMM_WORLD, &(info.nranks));
	
	/* Set manually in program or through command line arguments */
	nsd_x = 3;
	nsd_y = 1;
	nsd_z = 1;
	
	if (info.nranks != (nsd_x * nsd_y * nsd_z))	
	{
		if (info.rank == 0)
			fprintf(stderr, "You should run this program on another number of processes.\n");
		
		return -1;
	}
	
	/* Set manually in program or through command line arguments */
	nx = 50;
	ny = 50;
	nz = 50;
	
	/* Global domain boundaries */
	double xMin = 0.0;
	double xMax = 1.0;
	double yMin = 0.0;
	double yMax = 1.0;
	double zMin = 0.0;
	double zMax = 1.0;
	
	/* Spatial steps */
	double hx = (xMax - xMin) / nx;
	double hy = (yMax - yMin) / ny;
	double hz = (zMax - zMin) / nz;
	
	/* Store additional grid data on current MPI-process */
	info.rank_z = (info.rank) / (nsd_x * nsd_y);
	info.rank_y = (info.rank - nsd_x * nsd_y * info.rank_z) / nsd_x;
	info.rank_x = (info.rank - nsd_x * info.rank_y - nsd_x * nsd_y * info.rank_z);
	
	info.ncells_x = (info.rank_x == (nsd_x - 1)) ? nx - (nx / nsd_x) * (nsd_x - 1) : nx / nsd_x;
	info.ncells_y = (info.rank_y == (nsd_y - 1)) ? ny - (ny / nsd_y) * (nsd_y - 1) : ny / nsd_y;
	info.ncells_z = (info.rank_z == (nsd_z - 1)) ? nz - (nz / nsd_z) * (nsd_z - 1) : nz / nsd_z;
	
	info.offset_x = nx / nsd_x * info.rank_x + 1;
	info.offset_y = ny / nsd_y * info.rank_y + 1;
	info.offset_z = nz / nsd_z * info.rank_z + 1;
	
	info.limit_x = (info.rank_x == (nsd_x - 1)) ? nx : (info.rank_x + 1) * (nx / nsd_x);
	info.limit_y = (info.rank_y == (nsd_y - 1)) ? ny : (info.rank_y + 1) * (ny / nsd_y);
	info.limit_z = (info.rank_z == (nsd_z - 1)) ? nz : (info.rank_z + 1) * (nz / nsd_z);
	
	info.npoints_x = info.ncells_x + 1;
	info.npoints_y = info.ncells_y + 1;
	info.npoints_z = info.ncells_z + 1;
		
	/* Allocate memory for mesh */
	mesh_t mesh;
	mesh.x = (double *)malloc(info.npoints_x * sizeof(double));
	mesh.y = (double *)malloc(info.npoints_y * sizeof(double));
	mesh.z = (double *)malloc(info.npoints_z * sizeof(double));	
	
	/* Allocate memory for data */
	state_t flowState;
	flowState.data = (double *)malloc(info.ncells_x * info.ncells_y * info.ncells_z * sizeof(double));
	
	/* Initialize mesh and data */
	for (i = 0; i < info.npoints_x; i++)
	{
		mesh.x[i] = xMin + (i + info.offset_x - 1) * hx;
	}
	
	for (j = 0; j < info.npoints_y; j++)
	{
		mesh.y[j] = yMin + (j + info.offset_y - 1) * hy;
	}
	
	for (k = 0; k < info.npoints_z; k++)
	{
		mesh.z[k] = zMin + (k + info.offset_z - 1) * hz;
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
	
	/* Save ParaView files collection in parallel */
	write_pv_files_collection(mesh, flowState);
	
	if (info.rank == 0)
		fprintf(stdout, "All was done!\n");
	
	/* Release alloacated memory */
	free(mesh.x);
	free(mesh.y);
	free(mesh.z);
	free(flowState.data);
	
	MPI_Finalize();
	
	return 0;
}

