#ifndef __PBO_H__
#define __PBO_H__

#include <stdio.h>
#include <cstdlib>

typedef unsigned char       BYTE;
typedef unsigned long       DWORD;
typedef __int64 LONGLONG;

#define Int32x32To64(a, b)  ((__int64)(((__int64)((long)(a))) * ((long)(b))))
#define MAXDWORD    0xffffffff  



typedef struct _FILETIME {
    DWORD dwLowDateTime;
    DWORD dwHighDateTime;
} FILETIME, *PFILETIME, *LPFILETIME;
#define _FILETIME_

// PBO Extract status
#define STATUS_HEADER		0
#define STATUS_FILECOUNT	1
#define STATUS_FILETABLE	2
#define STATUS_DONE			3

#define FNAMELEN	512		// Max length of filenames
#define PREFIXFILE	"$PBOPREFIX$" // File name to store pbo prefix in
#define EXCLUDEFILE "$EXCLUDE$" // File name with exluded extensions
#define VERSIONSTRING	"cpbo v2.13 by Kegetys"
#define EX_NUM	256		// Max number of excludes
#define EX_LEN	256		// Max exclude length

// Function prototypes
bool pboEx(char *sf, char *dd, bool overwrite, bool gui);
bool pboPack(char *sd, char *df, bool overwrite);
bool pboDecompress(BYTE *buf, BYTE *out, int size, int outSize);

// PBO entry without null terminated string
typedef struct {
	// null terminated string here
	unsigned int	PackingMethod;
	unsigned int	OriginalSize;
	unsigned int	Reserved;
	unsigned int	TimeStamp;
	unsigned int	DataSize;
} PBOENTRY;

// Table used to store pbo filename table
typedef struct {
	char fname[FNAMELEN];
	unsigned int len;
	unsigned int timestamp;
	unsigned int origsize; // 0 = not compressed
	bool extract;
} FTENTRY;

#endif;