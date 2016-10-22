// license:BSD-3-Clause
// copyright-holders:Nathan Woods
/****************************************************************************

    filteoln.c

    Native end-of-line filter

*****************************************************************************/

#include <string.h>

#include "imgtool.h"

#define EOLN (CRLF == 1 ? "\r" : (CRLF == 2 ? "\n" : (CRLF == 3 ? "\r\n" : NULL)))



static imgtoolerr_t convert_stream_eolns(imgtool::stream &source, imgtool::stream &dest, const char *eoln)
{
	size_t len, i, pos;
	char buffer[2000];
	int hit_cr = FALSE;

	while((len = source.read(buffer, sizeof(buffer))) > 0)
	{
		pos = 0;

		for (i = 0; i < len; i++)
		{
			switch(buffer[i])
			{
				case '\r':
				case '\n':
					if (!hit_cr || (buffer[i] != '\n'))
					{
						if (i > pos)
							dest.write(buffer + pos, i - pos);
						dest.write(eoln, strlen(eoln));
					}
					pos = i + 1;
					break;
			}
			hit_cr = (buffer[i] == '\r');
		}

		if (i > pos)
			dest.write(buffer + pos, i - pos);
	}

	return IMGTOOLERR_SUCCESS;
}



static imgtoolerr_t ascii_readfile(imgtool::partition &partition, const char *filename, const char *fork, imgtool::stream &destf)
{
	imgtoolerr_t err;
	imgtool::stream *mem_stream;

	mem_stream = imgtool::stream::open_mem(nullptr, 0);
	if (!mem_stream)
	{
		err = IMGTOOLERR_OUTOFMEMORY;
		goto done;
	}

	err = partition.read_file(filename, fork, *mem_stream, nullptr);
	if (err)
		goto done;

	mem_stream->seek(SEEK_SET, 0);
	err = convert_stream_eolns(*mem_stream, destf, EOLN);
	if (err)
		goto done;

done:
	if (mem_stream)
		delete mem_stream;
	return err;
}



static imgtoolerr_t ascii_writefile(imgtool::partition &partition, const char *filename, const char *fork, imgtool::stream &sourcef, util::option_resolution *opts)
{
	imgtoolerr_t err;
	imgtool::stream *mem_stream = nullptr;
	const char *eoln;

	/* create a stream */
	mem_stream = imgtool::stream::open_mem(nullptr, 0);
	if (!mem_stream)
	{
		err = IMGTOOLERR_OUTOFMEMORY;
		goto done;
	}

	eoln = partition.get_info_string(IMGTOOLINFO_STR_EOLN);

	err = convert_stream_eolns(sourcef, *mem_stream, eoln);
	if (err)
		goto done;
	mem_stream->seek(SEEK_SET, 0);

	err = partition.write_file(filename, fork, *mem_stream, opts, nullptr);
	if (err)
		goto done;

done:
	if (mem_stream)
		delete mem_stream;
	return err;
}



void filter_eoln_getinfo(uint32_t state, union filterinfo *info)
{
	switch(state)
	{
		case FILTINFO_STR_NAME:         info->s = "ascii"; break;
		case FILTINFO_STR_HUMANNAME:    info->s = "Ascii"; break;
		case FILTINFO_PTR_READFILE:     info->read_file = ascii_readfile; break;
		case FILTINFO_PTR_WRITEFILE:    info->write_file = ascii_writefile; break;
	}
}
