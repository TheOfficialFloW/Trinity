#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zlib.h>

#include "types.h"
#include "endian.h"
#include "kirk_engine.h"
#include "psp_headers.h"
#include "pboot_headers.h"

typedef struct Header_List {
	unsigned char *pspHeader;
	unsigned char *kirkHeader;
} Header_List;

Header_List header_list[] = {
	{ pspHeader_smallest, kirkHeader_smallest },
	{ pspHeader_small,    kirkHeader_small },
	{ pspHeader_middle,   kirkHeader_middle },
	{ pspHeader_big,      kirkHeader_big },
	{ pspHeader_biggest,  kirkHeader_biggest },
};

u8 in_buffer[1024*1024*10];
u8 out_buffer[1024*1024*10];

u8 kirk_raw[1024*1024*10];
u8 kirk_enc[1024*1024*10];
u8 elf[1024*1024*10];

typedef struct header_keys
{
    u8 AES[16];
    u8 CMAC[16];
}header_keys;

int load_elf(char *elff)
{
	FILE *fp = fopen(elff, "rb");

	if(fp == NULL) {
		return -1;
	}

	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	fread(elf, 1, size, fp);
	fclose(fp);

	return size;
}

int dumpFile(char *name, void *in, int size)
{
	FILE *fp = fopen(name, "wb");

	if(fp == NULL) {
		return -1;
	}

	fwrite(in, 1, size, fp);
	fclose(fp);

	return 0;
}

int get_kirk_size(u8 *key_hdr)
{
	int krawSize = *(u32*)(key_hdr+0x70);

	if(krawSize % 0x10) {
		krawSize += 0x10 - (krawSize % 0x10); // 16 bytes aligned
	}

	krawSize += 0x920;

	return krawSize;
}

Header_List *get_header_list(int size)
{
	int i;
	int h_size;

	for(i=0; i<sizeof(header_list)/sizeof(header_list[i]); i++) {
		h_size = get_kirk_size(header_list[i].kirkHeader);
		h_size -= 0x960;

		if( h_size >= size ) {
			return &header_list[i];
		}
	}

	return NULL;
}

int is_compressed(u8 *psp_header)
{
	if (*(u16*)(psp_header+6) == 1) {
		return 1;
	}

	return 0;
}

int get_elf_size(u8 *psp_header)
{
	return *(u32*)(psp_header+0x28);
}

int gzip_compress(u8 *dst, const u8 *src, int size)
{
	int ret;
	z_stream strm;
	u8 *elf_compress;
	const int compress_max_size = 10 * 1024 * 1024;

	strm.zalloc = Z_NULL;
	strm.zfree = Z_NULL;
	strm.opaque = Z_NULL;

	elf_compress = malloc(compress_max_size);

	if(elf_compress == NULL) {
		return -1;
	}

	ret = deflateInit2(&strm, 9, Z_DEFLATED, 15+16, 8, Z_DEFAULT_STRATEGY);

	if(ret != Z_OK) {
		printf("%s: compress error\n", __func__);
		free(elf_compress);
		
		return -2;
	}

	strm.avail_in = size;
	strm.next_in = (void*)src;
	strm.avail_out = compress_max_size;
	strm.next_out = elf_compress;

	ret = deflate(&strm, Z_FINISH);

	if(ret == Z_STREAM_ERROR) {
		deflateEnd(&strm);
		printf("%s: compress error\n", __func__);
		free(elf_compress);

		return -3;
	}

	memcpy(dst, elf_compress, strm.total_out);
	deflateEnd(&strm);
	free(elf_compress);

	return 0;
}

int main(int argc, char **argv)
{
	header_keys keys;
	u8 rawkheaderBk[0x90];

	if(argc < 2)
	{
		printf("USAGE: [exe] [prx]\n");
		return 0;
	}

	memset(in_buffer, 0, sizeof(in_buffer));
	memset(out_buffer, 0, sizeof(out_buffer));
	memset(kirk_raw, 0, sizeof(kirk_raw));
	memset(kirk_enc, 0, sizeof(kirk_enc));
	memset(elf, 0, sizeof(elf));

	kirk_init();

	int elfSize = load_elf(argv[1]);

	if(elfSize < 0) {
		printf("Cannot open %s\n", argv[1]);

		return 0;
	}

	Header_List *target_header = get_header_list( elfSize );

	if( target_header == NULL ) {
		printf("PRX SIGNER: Elf is to big\n");

		return 0;
	}

	u8 *kirkHeader	= target_header->kirkHeader;
	u8 *pspHeader	= target_header->pspHeader;
	int krawSize = get_kirk_size(kirkHeader);

	if (is_compressed(pspHeader)) {
		elfSize = get_elf_size(pspHeader);
		gzip_compress(elf, elf, elfSize);
	}

	memcpy(kirk_raw, kirkHeader, 0x920);
	memcpy(rawkheaderBk, kirk_raw, sizeof(rawkheaderBk));

	kirk_decrypt_keys((u8*)&keys, kirk_raw);
	memcpy(kirk_raw, &keys, sizeof(header_keys));
	memcpy(kirk_raw+0x920, elf, elfSize);

	if(kirk_CMD0(kirk_enc, kirk_raw, sizeof(kirk_enc), 0) != 0)
	{
		printf("PRX SIGNER: Could not encrypt elf\n");
		return 0;
	}

	memcpy(kirk_enc, rawkheaderBk, sizeof(rawkheaderBk));

	if(kirk_forge(kirk_enc, sizeof(kirk_enc)) != 0)
	{
		printf("PRX SIGNER: Could not forge cmac block\n");

		return 0;
	}

	memcpy(out_buffer, pspHeader, 0x960);
	memcpy(out_buffer+0x960, kirk_enc+0x920, krawSize-0x920);

	return dumpFile("./DATA.PSP", out_buffer, (krawSize-0x920)+0x960);
}
