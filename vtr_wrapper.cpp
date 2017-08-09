
#include "main.h"

void write_vtr_ascii_file(mesh_t mesh, state_t state)
{
	register int i, j, k;
	
	int size = 1;
	if (info.rank != 0)
		size = floor(log10(abs(info.rank)))+1;
	
	char *vtr_file_name = (char *)malloc((size + 8) * sizeof(char));
	sprintf(vtr_file_name, "%s_%d.vtr", (char *)"out", info.rank);
	
	FILE *vtr = 0;
	vtr = fopen(vtr_file_name, "w");
	
	fprintf(vtr, "<?xml version=\"1.0\"?>\n");
	fprintf(vtr, "<VTKFile type=\"RectilinearGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
	fprintf(vtr, " <RectilinearGrid WholeExtent= \"%d %d %d %d %d %d\" GhostLevel=\"0\">\n", info.offset_x-1, info.limit_x, 
												 info.offset_y-1, info.limit_y, 
												 info.offset_z-1, info.limit_z );
	fprintf(vtr, "   <Piece Extent=\"%d %d %d %d %d %d\">\n", info.offset_x-1, info.limit_x, 
								  info.offset_y-1, info.limit_y, 
								  info.offset_z-1, info.limit_z );
	fprintf(vtr, "     <Coordinates>\n");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"X\" NumberOfComponents=\"1\" format=\"ascii\">\n");
	for (i = 0; i < info.npoints_x; i++)
	{
		fprintf(vtr, "         %E\n", *(mesh.x + i));
	}
	fprintf(vtr, "       </DataArray>\n");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Y\" NumberOfComponents=\"1\" format=\"ascii\">\n");
	for (j = 0; j < info.npoints_y; j++)
	{
		fprintf(vtr, "         %E\n", *(mesh.y + j));
	}
	fprintf(vtr, "       </DataArray>\n");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Z\" NumberOfComponents=\"1\" format=\"ascii\">\n");
	for (k = 0; k < info.npoints_z; k++)
	{
		fprintf(vtr, "         %E\n", *(mesh.z + k));
	}
	fprintf(vtr, "       </DataArray>\n");
	
	fprintf(vtr, "     </Coordinates>\n");
	fprintf(vtr, "     <CellData Scalars=\"%s\">\n", (char *)"flowState");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"%s\" NumberOfComponents=\"1\" format=\"ascii\">\n", "F");
	
	for (k = 0; k < info.ncells_z; k++)
	{
		for (j = 0; j < info.ncells_y; j++)
		{
			for (i = 0; i < info.ncells_x; i++)
			{
				fprintf(vtr, "         %E\n", *(state.data));
				state.data++;
			}
		}
	}
	fprintf(vtr, "       </DataArray>\n");
	
	fprintf(vtr, "     </CellData>\n");
	fprintf(vtr, "   </Piece>\n");
	fprintf(vtr, " </RectilinearGrid>\n");
	fprintf(vtr, "</VTKFile>\n");
	
	fclose(vtr);
	
	free(vtr_file_name);
	
	return;
}

void write_vtr_binary_file(mesh_t mesh, state_t state)
{
	register int i, j, k;
	int          offset;
	
	int size = 1;
	if (info.rank != 0)
		size = floor(log10(abs(info.rank)))+1;
	
	char *vtr_file_name = (char *)malloc((size + 8) * sizeof(char));
	sprintf(vtr_file_name, "%s_%d.vtr", (char *)"out", info.rank);
	
	FILE *vtr = 0;
	vtr = fopen(vtr_file_name, "wb");
	
	fprintf(vtr, "<?xml version=\"1.0\"?>\n");
	fprintf(vtr, "<VTKFile type=\"RectilinearGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
	fprintf(vtr, " <RectilinearGrid WholeExtent= \"%d %d %d %d %d %d\" GhostLevel=\"0\">\n", info.offset_x-1, info.limit_x, 
												 info.offset_y-1, info.limit_y, 
												 info.offset_z-1, info.limit_z );
	fprintf(vtr, "   <Piece Extent=\"%d %d %d %d %d %d\">\n", info.offset_x-1, info.limit_x, 
								  info.offset_y-1, info.limit_y, 
								  info.offset_z-1, info.limit_z );
	fprintf(vtr, "     <Coordinates>\n");
	
	offset = 0;
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"X\" NumberOfComponents=\"1\" format=\"appended\" offset=\"%d\"/>\n", offset);
	
	offset += info.npoints_x * sizeof(double) + sizeof(int);
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Y\" NumberOfComponents=\"1\" format=\"appended\" offset=\"%d\"/>\n", offset);
	
	offset += info.npoints_y * sizeof(double) + sizeof(int);
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Z\" NumberOfComponents=\"1\" format=\"appended\" offset=\"%d\"/>\n", offset);
	
	offset += info.npoints_z * sizeof(double) + sizeof(int);
	
	fprintf(vtr, "     </Coordinates>\n");
	fprintf(vtr, "     <CellData Scalars=\"%s\">\n", (char *)"flowState");
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"F\" NumberOfComponents=\"1\" format=\"appended\" offset=\"%d\"/>\n", offset);
	fprintf(vtr, "     </CellData>\n");
	fprintf(vtr, "   </Piece>\n");
	fprintf(vtr, " </RectilinearGrid>\n");
	fprintf(vtr, " <AppendedData encoding=\"raw\">\n");
	fprintf(vtr, "   _");
	
	offset = info.npoints_x * sizeof(double);
	fwrite(&offset, sizeof(int), 1, vtr);
	for (i = 0; i < info.npoints_x; i++)
	{
		fwrite(mesh.x + i, sizeof(double), 1, vtr);
	}
	
	offset = info.npoints_y * sizeof(double);
	fwrite(&offset, sizeof(int), 1, vtr);
	for (j = 0; j < info.npoints_y; j++)
	{
		fwrite(mesh.y + j, sizeof(double), 1, vtr);
	}
	
	offset = info.npoints_z * sizeof(double);
	fwrite(&offset, sizeof(int), 1, vtr);
	for (k = 0; k < info.npoints_z; k++)
	{
		fwrite(mesh.z + k, sizeof(double), 1, vtr);
	}
	
	offset = info.ncells_x * info.ncells_y * info.ncells_z * sizeof(double);
	fwrite(&offset, sizeof(int), 1, vtr);
	for (k = 0; k < info.ncells_z; k++)
	{
		for (j = 0; j < info.ncells_y; j++)
		{
			for (i = 0; i < info.ncells_x; i++)
			{
				fwrite(state.data, sizeof(double), 1, vtr);
				state.data++;
			}
		}
	}
	
	fprintf(vtr, "\n");
	fprintf(vtr, " </AppendedData>\n");
	fprintf(vtr, "</VTKFile>\n");
	
	fclose(vtr);
	
	free(vtr_file_name);
	
	return;
}

int insertValue(FILE *pFile, size_t pos, int val)
{
	char buf[256];
	sprintf(buf, "%d", val);
	fseek(pFile, pos, SEEK_SET);
	fputs(buf, pFile);
	
	return 0;
}

void write_vtr_binary_compressed_file(mesh_t mesh, state_t state)
{
	register int i, j, k;
	int          srcLen, dstLen;
	int          compresedSize;
	int          offsets[4];
	int          pos[4];
	int          headers[4] = {1, 0, 0, 0};
	double      *src;
	char        *dst;
	char        *blanks = (char *)"                ";
	
	int size = 1;
	if (info.rank != 0)
		size = floor(log10(abs(info.rank)))+1;
	
	char *vtr_file_name = (char *)malloc((size + 8) * sizeof(char));
	sprintf(vtr_file_name, "%s_%d.vtr", (char *)"out", info.rank);
	
	FILE *vtr = 0;
	vtr = fopen(vtr_file_name, "wb");
	
	fprintf(vtr, "<?xml version=\"1.0\"?>\n");
	fprintf(vtr, "<VTKFile type=\"RectilinearGrid\" version=\"0.1\" byte_order=\"LittleEndian\" compressor=\"vtkZLibDataCompressor\">\n");
	fprintf(vtr, " <RectilinearGrid WholeExtent= \"%d %d %d %d %d %d\" GhostLevel=\"0\">\n", info.offset_x-1, info.limit_x, 
												 info.offset_y-1, info.limit_y, 
												 info.offset_z-1, info.limit_z );
	fprintf(vtr, "   <Piece Extent=\"%d %d %d %d %d %d\">\n", info.offset_x-1, info.limit_x, 
								  info.offset_y-1, info.limit_y, 
								  info.offset_z-1, info.limit_z );
	fprintf(vtr, "     <Coordinates>\n");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"X\" NumberOfComponents=\"1\" format=\"appended\" offset=\"");
	pos[0] = ftell(vtr);
	fprintf(vtr, "%s\"/>\n", blanks);
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Y\" NumberOfComponents=\"1\" format=\"appended\" offset=\"");
	pos[1] = ftell(vtr);
	fprintf(vtr, "%s\"/>\n", blanks);
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"Z\" NumberOfComponents=\"1\" format=\"appended\" offset=\"");
	pos[2] = ftell(vtr);
	fprintf(vtr, "%s\"/>\n", blanks);
	
	fprintf(vtr, "     </Coordinates>\n");
	fprintf(vtr, "     <CellData Scalars=\"%s\">\n", (char *)"flowState");
	
	fprintf(vtr, "       <DataArray type=\"Float64\" Name=\"F\" NumberOfComponents=\"1\" format=\"appended\" offset=\"");
	pos[3] = ftell(vtr);
	fprintf(vtr, "%s\"/>\n", blanks);
	
	fprintf(vtr, "     </CellData>\n");
	fprintf(vtr, "   </Piece>\n");
	fprintf(vtr, " </RectilinearGrid>\n");
	fprintf(vtr, " <AppendedData encoding=\"raw\">\n");
	fprintf(vtr, "   _");
	
	/***/
	offsets[0] = 0;
	
	srcLen = info.npoints_x * sizeof(double);
	dstLen = getMaxCompressedLen( srcLen );
	dst    = (char *)malloc(dstLen);
	compresedSize = compressData((void *)mesh.x, srcLen, (void *)dst, dstLen);
	
	headers[1] = srcLen;
	headers[3] = compresedSize;
	
	fwrite((char *)headers, sizeof(char), 4 * sizeof(int), vtr);
	fwrite(dst, sizeof(char), headers[3], vtr);
	
	free(dst);
	
	/***/
	offsets[1] = offsets[0] + compresedSize + 4 * sizeof(int);
	
	srcLen = info.npoints_y * sizeof(double);
	dstLen = getMaxCompressedLen( srcLen );
	dst    = (char *)malloc(dstLen);
	compresedSize = compressData((void *)mesh.y, srcLen, (void *)dst, dstLen);
	
	headers[1] = srcLen;
	headers[3] = compresedSize;
	
	fwrite((char *)headers, sizeof(char), 4 * sizeof(int), vtr);
	fwrite(dst, sizeof(char), headers[3], vtr);
	
	free(dst);
	
	/***/
	offsets[2] = offsets[1] + compresedSize + 4 * sizeof(int);
	
	srcLen = info.npoints_z * sizeof(double);
	dstLen = getMaxCompressedLen( srcLen );
	dst    = (char *)malloc(dstLen);
	compresedSize = compressData((void *)mesh.z, srcLen, (void *)dst, dstLen);
	
	headers[1] = srcLen;
	headers[3] = compresedSize;
	
	fwrite((char *)headers, sizeof(char), 4 * sizeof(int), vtr);
	fwrite(dst, sizeof(char), headers[3], vtr);
	
	free(dst);
	
	/***/
	offsets[3] = offsets[2] + compresedSize + 4 * sizeof(int);
	
	srcLen = info.ncells_x * info.ncells_y * info.ncells_z * sizeof(double);
	dstLen = getMaxCompressedLen( srcLen );
	dst    = (char *)malloc(dstLen);
	compresedSize = compressData((void *)state.data, srcLen, (void *)dst, dstLen);
	
	headers[1] = srcLen;
	headers[3] = compresedSize;
	
	fwrite((char *)headers, sizeof(char), 4 * sizeof(int), vtr);
	fwrite(dst, sizeof(char), headers[3], vtr);
	
	free(dst);
	
	fprintf(vtr, "\n");
	fprintf(vtr, " </AppendedData>\n");
	fprintf(vtr, "</VTKFile>\n");
	
	insertValue(vtr, pos[0], offsets[0]);
	insertValue(vtr, pos[1], offsets[1]);
	insertValue(vtr, pos[2], offsets[2]);
	insertValue(vtr, pos[3], offsets[3]);
	
	fclose(vtr);
	
	free(vtr_file_name);
	
	return;
}

void write_pvd_file()
{
	/* This file should be written by root process */
	if (info.rank != 0)
		return;
	
	char *pvd_file_name    = (char *)malloc(7 * sizeof(char));
	sprintf(pvd_file_name,    "%s.pvd", (char *)"out");
	
	FILE *pvd = 0;
	pvd = fopen(pvd_file_name, "w");
	
	fprintf(pvd, "<?xml version=\"1.0\"?>\n");
	fprintf(pvd, "<VTKFile type=\"Collection\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
	fprintf(pvd, " <Collection>\n");
	fprintf(pvd, "   <DataSet timestep=\"%E\" group=\"\" part=\"0\" file=\"%s.pvtr\"/>\n", 0.0, (char *)"out");
	fprintf(pvd, " </Collection>\n");
	fprintf(pvd, "</VTKFile>\n");
	
	fclose(pvd);
	
	free(pvd_file_name);
	
	return;
}

/* Write file for rectilinear grid */
void write_pvtr_file()
{
	/* This file should be written by root process */
	if (info.rank != 0)
		return;
	
	char *pvtr_file_name   = (char *)malloc(8 * sizeof(char));
	sprintf(pvtr_file_name,   "%s.pvtr", (char *)"out");
	
	FILE *pvtr = 0;
	pvtr = fopen(pvtr_file_name, "w");
	
	fprintf(pvtr, "<?xml version=\"1.0\"?>\n");
	fprintf(pvtr, "<VTKFile type=\"PRectilinearGrid\" version=\"0.1\" byte_order=\"LittleEndian\">\n");
	fprintf(pvtr, " <PRectilinearGrid WholeExtent= \"%d %d %d %d %d %d\" GhostLevel=\"0\">\n", 0, nx, 0, ny, 0, nz);
	fprintf(pvtr, "   <PCoordinates>\n");
	fprintf(pvtr, "      <PDataArray type=\"Float64\" Name=\"X\" NumberOfComponents=\"1\" format=\"appended\"/>\n");
	fprintf(pvtr, "      <PDataArray type=\"Float64\" Name=\"Y\" NumberOfComponents=\"1\" format=\"appended\"/>\n");
	fprintf(pvtr, "      <PDataArray type=\"Float64\" Name=\"Z\" NumberOfComponents=\"1\" format=\"appended\"/>\n");
	fprintf(pvtr, "   </PCoordinates>\n");
	fprintf(pvtr, "   <PCellData Scalars=\"%s\">\n", "flowState");
	fprintf(pvtr, "     <PDataArray type=\"Float64\" Name=\"%s\" NumberOfComponents=\"1\" format=\"appended\"/>\n", "F");
	fprintf(pvtr, "   </PCellData>\n");
	
	register int p;
	info_t info1;
	
	for (p = 0; p < info.nranks; p++)
	{
		info1.rank_z = (p) / (nsd_x * nsd_y);
		info1.rank_y = (p - nsd_x * nsd_y * info1.rank_z) / nsd_x;
		info1.rank_x = (p - nsd_x * info1.rank_y - nsd_x * nsd_y * info1.rank_z);
		
		info1.offset_x = nx / nsd_x * info1.rank_x + 1;
		info1.offset_y = ny / nsd_y * info1.rank_y + 1;
		info1.offset_z = nz / nsd_z * info1.rank_z + 1;
		
		info1.limit_x = (info1.rank_x == (nsd_x - 1)) ? nx : (info1.rank_x + 1) * (nx / nsd_x);
		info1.limit_y = (info1.rank_y == (nsd_y - 1)) ? ny : (info1.rank_y + 1) * (ny / nsd_y);
		info1.limit_z = (info1.rank_z == (nsd_z - 1)) ? nz : (info1.rank_z + 1) * (nz / nsd_z);
		
		fprintf(pvtr, "   <Piece Extent=\"%d %d %d %d %d %d\" Source=\"%s_%d.vtr\"/>\n", info1.offset_x-1, info1.limit_x, 
												 info1.offset_y-1, info1.limit_y, 
												 info1.offset_z-1, info1.limit_z, 
												 (char *)"out", p);
	}
	
	fprintf(pvtr, " </PRectilinearGrid>\n");
	fprintf(pvtr, "</VTKFile>\n");
	
	fclose(pvtr);
	
	free(pvtr_file_name);
	
	return;
}

int write_pv_files_collection(mesh_t mesh, state_t state)
{
	/*
	write_vtr_ascii_file(mesh, state);
	*/
	/*
	write_vtr_binary_file(mesh, state);
	*/
	write_vtr_binary_compressed_file(mesh, state);
	write_pvtr_file();
	write_pvd_file();
	
	return 0;
}

