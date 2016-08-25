
#include "main.h"

#include "zlib.h"
#include "zconf.h"

#define CHUNK 16384

int getMaxCompressedLen( int srcLen ) 
{
	int n16kBlocks = (srcLen + (CHUNK - 1)) / CHUNK; // round up any fraction of a block
	return ( srcLen + 6 + (n16kBlocks * 5) );
}

int compressData(const void *src, int srcLen, void *dst, int dstLen)
{
	z_stream strm  = {0};
	strm.total_in  = strm.avail_in  = srcLen;
	strm.total_out = strm.avail_out = dstLen;
	strm.next_in   = (Bytef *) src;
	strm.next_out  = (Bytef *) dst;
	
	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;
	
	int err = -1;
	int ret = -1;
	
	err = deflateInit( &strm, Z_DEFAULT_COMPRESSION );
	if ( err == Z_OK )
	{
		err = deflate( &strm, Z_FINISH );
		if ( err == Z_STREAM_END )
		{
			ret = strm.total_out;
		}
	}
	deflateEnd( &strm );
	
	return ret;
}

int uncompressData(const void *src, int srcLen, void *dst, int dstLen)
{
	z_stream strm  = {0};
	strm.total_in  = strm.avail_in  = srcLen;
	strm.total_out = strm.avail_out = dstLen;
	strm.next_in   = (Bytef *) src;
	strm.next_out  = (Bytef *) dst;
	
	strm.zalloc = Z_NULL;
	strm.zfree  = Z_NULL;
	strm.opaque = Z_NULL;
	
	int err = -1;
	int ret = -1;
	
	//err = inflateInit(&strm);
	err = inflateInit2(&strm, (15 + 32)); //15 window bits, and the +32 tells zlib to to detect if using gzip or zlib
	if ( err == Z_OK )
	{
		err = inflate( &strm, Z_FINISH );
		if (err == Z_STREAM_END)
		{
			ret = strm.total_out;
		}
		else
		{
			inflateEnd(&strm);
			return err;
		}
	}
	else
	{
		inflateEnd(&strm);
		return err;
	}
	
	inflateEnd( &strm );
	
	return ret;
}

