//---------------------------------------------------------------------------

#pragma hdrstop

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dir.h>

#include "sndfile.h"

//---------------------------------------------------------------------------
#pragma pack(push,1)
typedef int int32;
typedef unsigned char byte;
struct TTCWFHeader
{
	char mark[6];  // = "TCWF0\x1a"
	byte channels; // チャネル数
	byte reserved;
	int32 frequency; // サンプリング周波数
	int32 numblocks; // ブロック数
	int32 bytesperblock; // ブロックごとのバイト数
	int32 samplesperblock; // ブロックごとのサンプル数
};

struct TTCWUnexpectedPeak
{
	unsigned short int pos;
	short int revise;
};

struct TTCWBlockHeader  // ブロックヘッダ ( ステレオの場合はブロックが右・左の順に２つ続く)
{
	short int ms_sample0;
	short int ms_sample1;
	short int ms_idelta;
	byte ms_bpred;
	byte ima_stepindex;
	TTCWUnexpectedPeak peaks[6];
};
#pragma pack(pop)
//---------------------------------------------------------------------------
static
unsigned int srate2blocksize (unsigned int srate)
{	if (srate < 12000)
		return 256 ;
	if (srate < 23000)
		return 512 ;
	return 1024 ;
} /* srate2blocksize */
//---------------------------------------------------------------------------

/*============================================================================================
** MS ADPCM static data.
*/
static int AdaptationTable []    =
{	230, 230, 230, 230, 307, 409, 512, 614,
	768, 614, 512, 409, 307, 230, 230, 230
} ;


/* TODO : The first 7 coef's are are always hardcode and must
   appear in the actual WAVE file.  They should be read in
   in case a sound program added extras to the list. */

static int AdaptCoeff1 [] =
{	256, 512, 0, 192, 240, 460, 392
} ;

static int AdaptCoeff2 [] =
{	0, -256, 0, 64, 0, -208, -232
} ;

/*============================================================================================
** Predefined IMA ADPCM data.
*/
static int ima_index_adjust [16] =
{	-1, -1, -1, -1,		// +0 - +3, decrease the step size
	 2,  4,  6,  8,     // +4 - +7, increase the step size
	-1, -1, -1, -1,		// -0 - -3, decrease the step size
	 2,  4,  6,  8,		// -4 - -7, increase the step size
} ;


static int ima_step_size [89] = 
{	7, 8, 9, 10, 11, 12, 13, 14, 16, 17, 19, 21, 23, 25, 28, 31, 34, 37, 41, 45, 
	50, 55, 60, 66, 73, 80, 88, 97, 107, 118, 130, 143, 157, 173, 190, 209, 230, 
	253, 279, 307, 337, 371, 408, 449, 494, 544, 598, 658, 724, 796, 876, 963, 
	1060, 1166, 1282, 1411, 1552, 1707, 1878, 2066, 2272, 2499, 2749, 3024, 3327, 
	3660, 4026, 4428, 4871, 5358, 5894, 6484, 7132, 7845, 8630, 9493, 10442,
	11487, 12635, 13899, 15289, 16818, 18500, 20350, 22385, 24623, 27086, 29794, 
	32767
} ;


/*----------------------------------------------------------------------------------------
**	Choosing the block predictor.
**	Each block requires a predictor and an idelta for each channel. 
**	The predictor is in the range [0..6] which is an index into the	two AdaptCoeff tables. 
**	The predictor is chosen by trying all of the possible predictors on a small set of
**	samples at the beginning of the block. The predictor with the smallest average
**	abs (idelta) is chosen as the best predictor for this block. 
**	The value of idelta is chosen to to give a 4 bit code value of +/- 4 (approx. half the 
**	max. code value). If the average abs (idelta) is zero, the sixth predictor is chosen.
**	If the value of idelta is less then 16 it is set to 16.
**
**	Microsoft uses an IDELTA_COUNT (number of sample pairs used to choose best predictor)
**	value of 3. The best possible results would be obtained by using all the samples to
**	choose the predictor.
*/
static
void	choose_predictor (int IDELTA_COUNT, short *data,
	int *block_pred, int *idelta)
{	unsigned int	chan, k, bpred, idelta_sum, best_bpred, best_idelta ;

	best_bpred = best_idelta = 0 ;

	for (bpred = 0 ; bpred < 7 ; bpred++)
	{
		idelta_sum = 0 ;
		for (k = 2 ; k < 2 + IDELTA_COUNT ; k++)
			idelta_sum += abs (data [k] - ((data [k-1] * AdaptCoeff1 [bpred] + data [k-2] * AdaptCoeff2 [bpred]) >> 8)) ;
		idelta_sum /= (4 * IDELTA_COUNT) ;

		if (bpred == 0 || idelta_sum < best_idelta)
		{	best_bpred = bpred ;
			best_idelta = idelta_sum ;
			} ;

		if (! idelta_sum)
		{	best_bpred = bpred ;
			best_idelta = 16 ;
			break ;
			} ;
			} ; /* for bpred ... */

	if (best_idelta < 16)
		best_idelta = 16 ;

	*block_pred = best_bpred ;
	*idelta     = best_idelta ;

	return ;
} /* choose_predictor */

//---------------------------------------------------------------------------
void encode_block(TTCWFHeader *header, short int *samples, int numsamples, FILE *fout)
{
	TTCWBlockHeader bheader;
	short temp[32768];
	unsigned char out[32768];

	int bpred=0;
	int idelta=0;
	int	k, predict, errordelta, newsamp, ms_diff;
	byte ms_nibble;
	byte ima_nibble;
	static int stepindex = 0; /* static !! */
	int prev = 0;
	int diff;
	int step;
	int vpdiff;
	int mask;

	choose_predictor (numsamples-2, samples,  &bpred, &idelta);

	bheader.ms_bpred = bpred;
	bheader.ms_idelta = idelta;
	bheader.ms_sample0 = samples[0];
	bheader.ms_sample1 = samples[1];
	bheader.ima_stepindex = stepindex;

	temp[0]=0;
	temp[1]=0;

	int p=0;
	for (k = 2 ; k < numsamples ; k++)
	{
		/* MS ADPCM 圧縮部 */

		predict = (samples[k-1] * AdaptCoeff1 [bpred] + samples [k-2] * AdaptCoeff2 [bpred]) >> 8 ;
		errordelta = (samples [k] - predict) / idelta;

		if (errordelta < -8)
			errordelta = -8 ;
		else if (errordelta > 7)
			errordelta = 7 ;

		newsamp = predict + (idelta* errordelta) ;

		if (newsamp > 32767)
			newsamp = 32767 ;
		else if (newsamp < -32768)
			newsamp = -32768 ;
		if (errordelta < 0)
			errordelta += 0x10 ;

		ms_nibble = (errordelta & 0xF);

		idelta= (idelta * AdaptationTable [errordelta]) >> 8 ;

		if (idelta< 16)
			idelta= 16 ;

		ms_diff = samples[k] - newsamp;


		short int orgsamp = samples[k];
		samples [k] = newsamp ;

		/* IMA ADPCM 圧縮部 */

		diff = ms_diff-prev ;


		ima_nibble = 0 ;
		step = ima_step_size[stepindex] ;
		vpdiff = step >> 3 ;
		if (diff < 0)
		{
			ima_nibble = 8 ;
			diff = -diff ;
		}
		mask = 4 ;

		while (mask)
		{
			if (diff >= step)
			{
				ima_nibble |= mask ;
				diff -= step ;
				vpdiff += step ;
			}
			step >>= 1 ;
			mask >>= 1 ;
		}

		if (ima_nibble & 8)
			prev -= vpdiff ;
		else
			prev += vpdiff ;

		if (prev > 32767) prev = 32767;
		else if (prev < -32768) prev = -32768;

		temp[k] = orgsamp - (newsamp + prev);

		stepindex += ima_index_adjust [ima_nibble] ;
		if (stepindex< 0) stepindex = 0 ;
		else if (stepindex > 88) stepindex= 88 ;

		ima_nibble&=0x0f;

		out[p++] = (unsigned char)((ms_nibble<<0) + (ima_nibble<<4));

	}

	/* Unexpected Peak の検出と記録 */

	for(int i=0; i<6; i++)
	{
		int max =0;
		int max_pos = 0;
		int k;

		for(k=2; k<numsamples; k++)
		{
			if(temp[k]<0)
			{
				if(-temp[k]>max)
				{
					max=-temp[k];
					max_pos=k;
				}
			}
			else
			{
				if(temp[k]>max)
				{
					max=temp[k];
					max_pos=k;
				}
			}
		}
		bheader.peaks[i].pos = max_pos;
		bheader.peaks[i].revise = -temp[max_pos];
		temp[max_pos] += bheader.peaks[i].revise; // 0 になる
	}


	// ファイルに出力
	fwrite(&bheader, sizeof(bheader), 1, fout);
	fwrite(out, p, 1, fout);
}
//---------------------------------------------------------------------------
void convert(char *srcfilename, char*destfilename)
{
	short int buf[32768];
	SNDFILE *sfin;
	FILE *fout;
	SF_INFO sfiin;
	size_t read;
	int i;


	// ソースファイルを開く
	memset(&sfiin, sizeof(sfiin), 0);
	sfin = sf_open_read(srcfilename, &sfiin);
	if(!sfin)
	{
		printf("Can't open : %s\n", srcfilename);
		return;
	}

	// 出力ファイルを開く
	fout = fopen(destfilename, "wb");
	if(!fout)
	{
		printf("Can't open : %s\n", destfilename);
	}

	// 表示
	if(srcfilename && destfilename)
	{
		char src[MAXPATH], srcext[MAXPATH];
		char dest[MAXPATH], destext[MAXPATH];
		fnsplit(srcfilename, NULL, NULL, src, srcext);
		fnsplit(destfilename, NULL, NULL, dest, destext);
		strcat(src, srcext);
		strcat(dest, destext);
		printf("Compressing %s -> %s ...\n", src , dest);
	}


	// ヘッダを準備
	int chans;
	TTCWFHeader header;
	memcpy(header.mark, "TCWF0\x1a", 6);
	chans = header.channels = sfiin.channels;
	header.reserved = 0;
	header.frequency = sfiin.samplerate;
	header.numblocks = 0; // ここは後でうめる
	header.bytesperblock = srate2blocksize(header.frequency);
	header.samplesperblock = header.bytesperblock - sizeof(TTCWBlockHeader) + 2;

	fwrite(&header, sizeof(header), 1, fout); // とりあえずいったん書いておく

	int numblocks = 0;
	int totalblocks = sfiin.samples*chans / header.samplesperblock;
	int dispinterval = totalblocks / 80;
	int interval=0;

	while( 0 < ( read = sf_read_short(sfin, buf, header.samplesperblock*chans) )   )
	{
		// ステレオの場合はインターリーブ解除
		short int buf2[2][32768];
		memset(buf2[0], sizeof(short int)*32768, 0);
		memset(buf2[1], sizeof(short int)*32768, 0);

		if(chans==1)
		{
			memcpy(buf2[0], buf, sizeof(short int)*read);
		}
		else
		{
			for(i=0; i<read; i+=2)
			{
				buf2[0][i/2]=buf[i];
				buf2[1][i/2]=buf[i+1];
			}
		}

		if(chans==1)
		{
			encode_block(&header, buf2[0], header.samplesperblock, fout);
			numblocks ++;
		}
		else
		{
			encode_block(&header, buf2[0], header.samplesperblock, fout);
			encode_block(&header, buf2[1], header.samplesperblock, fout);
			numblocks += 2;
		}

		if(interval == 0)
		{
			printf("%2d %% done ...\r", numblocks *100 / totalblocks);
			interval = dispinterval;
		}
		interval--;
	}

	header.numblocks = numblocks;
	fseek(fout, 0, SEEK_SET);
	fwrite(&header, sizeof(header), 1, fout); // ヘッダをもう一回書き込む

	sf_close(sfin);
	fclose(fout);

    printf("\r                  \r");

	return;
}
//---------------------------------------------------------------------------
void userwait(void)
{
	printf("\nPress ENTER key to exit ...\n");
	char buf[256];
	fgets(buf, 255, stdin);
}
//---------------------------------------------------------------------------
#pragma argsused
int main(int argc, char* argv[])
{
	if(argc<=1)
	{
		printf("TCWF compressor  copyright (C) 2000 W.Dee\n");
		printf("\n");
		printf("Usage : tcwfcomp <filename>\n");
		printf("         Under Windows GUI, you can drag source file(s) that you want\n");
		printf("        to compress, then drop on this application.\n");
		printf("\n");
		printf("  <filename> file can be one of following format :\n");
		printf("  .WAV .AIFF .AU .PAF .SVX\n");
		printf("\n");
		printf("  <filename> can contain wildcards.\n");
		printf("\n");
		printf("  Output filename will be automatically the same as input filename,\n"
			   " except for its extension that has \".tcw\".\n");
		printf("\n");
		printf("  You can use/modify/redistribute this program under GNU GPL, see\n"
			   " the document for details.\n");
		userwait();
		return 0;
	}

	int i;
	for(i=1; i<argc; i++)
	{
		struct ffblk ff;
		int done;

		char argdrive[MAXPATH];
		char argdir[MAXPATH];
		fnsplit(argv[i], argdrive, argdir, NULL, NULL);

		done = findfirst(argv[i], &ff, FA_HIDDEN | FA_RDONLY | FA_ARCH | FA_SYSTEM);
		while(!done)
		{
			char paths[MAXPATH];
			char pathd[MAXPATH];
			char drive[MAXPATH];
			char dir[MAXPATH];
			char name[MAXPATH];
			char ext[MAXPATH];
			fnsplit(ff.ff_name, drive, dir, name, ext);
			strcpy(drive, argdrive);
			strcpy(dir, argdir);
			fnmerge(paths, drive, dir, name, ext);
			strcpy(ext, ".tcw");
			fnmerge(pathd, drive, dir, name, ext);
			convert(paths, pathd);

			done = findnext(&ff);
		}
	}

	userwait();
}
//---------------------------------------------------------------------------




