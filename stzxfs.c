/*
  "Simple TZX File System" by Erik Olofsen.

  Compiling: gcc -Wall stzxfs.c `pkg-config fuse --cflags --libs` -o stzxfs

  Mount: ./stzxfs -t <tzxfile> <mountpoint>
  Unmount: fusermount -u <mountpoint>
*/

#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>

FILE *tzx;
long pos;
char filename[33];
int filesize=0;

char zx2ascii[128] = {
/*  0- 9 */ ' ', '_', '_', '_', '_', '_', '_', '_', '_', '_', 
/* 10-19 */ '_', '\"','!', '$', ':', '?', '(', ')', '>', '<', 
/* 20-29 */ '=', '+', '-', '*', '/', ';', ',', '.', '0', '1', 
/* 30-39 */ '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 
/* 40-49 */ 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 
/* 50-59 */ 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 
/* 60-63 */ 'w', 'x', 'y', 'z'
};

/* returns the length of the generalized data block *without* the header */

int skipgdbheader()
{
	unsigned int n, totp, totd;
	unsigned short pause;
	unsigned char npp, asp, npd, asd;
	long startpos;

	fread(&n, sizeof(n), 1, tzx);

	startpos = ftell(tzx);

	fread(&pause, sizeof(pause), 1, tzx);
	fread(&totp, sizeof(totp), 1, tzx);
	fread(&npp, sizeof(npp), 1, tzx);
	fread(&asp, sizeof(asp), 1, tzx);
	fread(&totd, sizeof(totp), 1, tzx);
	fread(&npd, sizeof(npd), 1, tzx);
	fread(&asd, sizeof(asd), 1, tzx);
	if (totp>0) {
		fseek(tzx, (2*npp+1)*asp, SEEK_CUR);
		fseek(tzx, totp*3, SEEK_CUR);
	}
	fseek(tzx, (2*npd+1)*asd, SEEK_CUR);

	pos = ftell(tzx); 

	return (int) n - (pos - startpos);
}

/* puts the name of the current file in global variable filename,
   and returns the length (without ".p") */

int fillfilename()
{
	int c;
	int i = 0;
	int n = 0;

	while (1) {
		c = fgetc(tzx);
		filename[i++] = zx2ascii[c&0x3f];
		n++;
		if (c&0x80 || i==(sizeof(filename)-3)) break;
	}
	filename[i++] = '.';
	filename[i++] = 'p';
	filename[i++] = '\0';

	return n;
}

/* returns the length of the data contained in the next block */

int tzxlength(const char *path, const int skip)
{
	int id, n;
	long oldpos = pos;
	int rewound = 0;

	while (1) {

		if (pos>=oldpos && rewound) return -1;

		if (pos==0) fseek(tzx, 10, SEEK_SET); /* skip header */

		id = fgetc(tzx);

		switch (id) {

		case EOF:
			rewind(tzx); pos=0; rewound=1; break;

		case 0x19:
			n = skipgdbheader();
			n -= fillfilename();
			if (!strcmp(path,filename)) {
				if (skip) fseek(tzx,n,SEEK_CUR);
				pos = ftell(tzx);
				return n;
			}
			fseek(tzx,n,SEEK_CUR);
			break;

		case 0x30:
			n = fgetc(tzx);
			if (!strcmp(path,"README")) {
				if (skip) fseek(tzx,n,SEEK_CUR);
				pos = ftell(tzx);
				return n;
			}
			fseek(tzx,n,SEEK_CUR);
			break;

		default:
			return -2;

		}

		pos = ftell(tzx);
	}

	return 0;
}

/* returns the number of generalized data blocks / files */

int tzx_directory(void *buf, fuse_fill_dir_t filler)
{
	int id, n;
	long oldpos = pos;
	int rewound = 0;
	int nf = 0;

	while (1) {

		if (pos>=oldpos && rewound) return nf;

		if (pos==0) fseek(tzx, 10, SEEK_SET); /* skip header */

		id = fgetc(tzx);

		switch (id) {

		case EOF:
			rewind(tzx); pos=0; rewound=1; break;

		case 0x19:
			n = skipgdbheader();
			n -= fillfilename();
			filler(buf, filename, NULL, 0);
			fseek(tzx,n,SEEK_CUR);
			nf++;
			break;

		case 0x30:
			n = fgetc(tzx);
			fseek(tzx,n,SEEK_CUR);
			strcpy(filename, "README");
			filler(buf, "README", NULL, 0);
			break;

		default:
			return nf;

		}

		pos = ftell(tzx);
	}

	return nf;
}

static int stzxfs_getattr(const char *path, struct stat *stbuf)
{
	int res = 0;
	const char *name;

	name = strrchr(path,'/');
	if (name) name++; else name=path;

	memset(stbuf, 0, sizeof(struct stat));
	stbuf->st_atime = stbuf->st_mtime = stbuf->st_ctime = 347155200;

	if (strcmp(path, "/") == 0) {
		stbuf->st_mode = S_IFDIR | 0666;
		stbuf->st_nlink = 2;
	} else {
		stbuf->st_mode = S_IFREG | 0444;
		stbuf->st_nlink = 1;
		stbuf->st_size = tzxlength(name, 1);
	}

	return res;
}

static int stzxfs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
			 off_t offset, struct fuse_file_info *fi)
{
	(void) offset;
	(void) fi;

	if (strcmp(path, "/") != 0)
		return -ENOENT;

	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);

        tzx_directory(buf,filler);

	return 0;
}

static int stzxfs_open(const char *path, struct fuse_file_info *fi)
{
	int len;
	const char *name;

	name = strrchr(path,'/');
	if (name) name++; else name=path;

	len = tzxlength(name, 1);

	if (len < 0)
		return -ENOENT;

	if ((fi->flags & 3) != O_RDONLY)
		return -EACCES;

	return 0;
}

static int stzxfs_read(const char *path, char *buf, size_t size,
		       off_t offset, struct fuse_file_info *fi)
{
	int len;
	const char *name;

	name = strrchr(path,'/');
	if (name) name++; else name=path;

	(void) fi;

	len = tzxlength(name, 0);

	if (len < 0)
		return -ENOENT;

	if (offset < len) {
		if (offset + size > len)
			size = len - offset;
		fseek(tzx, offset, SEEK_CUR);
		fread(buf, size, 1, tzx);
		pos = ftell(tzx);
	} else
		size = 0;

	return size;
}

static struct fuse_operations stzxfs_oper = {
	.getattr	= stzxfs_getattr,
	.readdir	= stzxfs_readdir,
	.open		= stzxfs_open,
	.read		= stzxfs_read,
};

static const char *usage =
"usage: stzxfs [options]\n"
"\n"
"options:\n"
"    --tzxfile=NAME|-t NAME   tzx file name (mandatory)\n"
"\n";

struct stzxfs_param {
	char			*tzxfile;
	int			is_help;
};

#define STZXFS_OPT(t, p) { t, offsetof(struct stzxfs_param, p), 1 }

static const struct fuse_opt stzxfs_opts[] = {
	STZXFS_OPT("-t %s",		tzxfile),
	STZXFS_OPT("--tzxfile=%s",	tzxfile),
	FUSE_OPT_KEY("-h",		0),
	FUSE_OPT_KEY("--help",		0),
	FUSE_OPT_END
};

static int stzxfs_process_arg(void *data, const char *arg, int key,
			       struct fuse_args *outargs)
{
	struct stzxfs_param *param = data;

	(void)outargs;
	(void)arg;

	switch (key) {
	case 0:
		param->is_help = 1;
		fprintf(stderr, "%s", usage);
		return fuse_opt_add_arg(outargs, "-ho");
	default:
		return 1;
	}
}

int main(int argc, char *argv[])
{
	struct fuse_args args = FUSE_ARGS_INIT(argc, argv);
	struct stzxfs_param param = { NULL, 0 };

	fuse_opt_parse(&args, &param, stzxfs_opts, stzxfs_process_arg);

	if (!param.is_help) {
		if (!param.tzxfile) {
			fprintf(stderr, "Error: tzx file name missing\n");
			return 1;
		}
		tzx = fopen(param.tzxfile,"rb");
		if (!tzx) {
			fprintf(stderr, "Error opening tzx file\n");
			return 1;
		}
		pos = 0;
	}


	return fuse_main(args.argc, args.argv, &stzxfs_oper, NULL);
}
