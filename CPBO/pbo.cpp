/*
cpbo Copyright (C) 2006-2014 Keijo Ruotsalainen

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License, version 2.1 as published by the Free Software Foundation.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
MA  02110-1301  USA
*/

// PBO functionality

#include "pbo.h"
#include "sha1.h"
#include <iostream>
#include <cstring>
#include <iterator>
#include "boost/filesystem.hpp"
#include "boost/format.hpp"

//using namespace std;
//using namespace boost::filesystem;
using namespace boost;


// Does file exist?
bool fileExists(char *filename) {
    FILE *f = fopen(filename, "rb");
    if(f) {
        fclose(f);
        return true;
    }
    return false;
}


int getDirFiles(char *sd, FTENTRY *ftable, int *fti, char excludes[EX_NUM][EX_LEN]) {
    char dir[FNAMELEN];
    sprintf(dir, "%s\\*.*", sd);

    int res = 1;
    int count = 0;

    filesystem::path p (dir);

    try
    {
        if (exists(p))
        {
            if (is_directory(p))
            {

                for (filesystem::path::iterator it = p.begin(); it != p.end(); ++it)
                {
                    if (!strcmp(it->filename().string().c_str(), ".."))
                        continue;
                    if (!strcmp(it->filename().string().c_str(), "."))
                        continue;
                    if (!strcasecmp(it->filename().string().c_str(), PREFIXFILE)) // Do not pack prefix file
                        continue;
                    if (!strcasecmp(it->filename().string().c_str(), EXCLUDEFILE))
                        continue;

                    if (filesystem::is_directory((filesystem::path)*it)) {
                        char foo[1024];
                        sprintf(foo, "%s\\%s", sd, it->filename().string().c_str());
                        count += getDirFiles(foo, ftable, fti, excludes);
                    } else {
                        // Check for exclude
                        bool skip = false;
                        for(int i=0; i<EX_NUM; i++) {
                            if (strlen(excludes[i]) > 1 && !strcasecmp(excludes[i], &it->filename().string().c_str()[strlen(it->filename().string().c_str()) - strlen(excludes[i])])) {
                                // printf("Skipping: %s - %s\n", fd.cFileName, excludes[i]);
                                skip = true;
                                break;
                            }
                        }
                        if(skip)
                            continue; // File extension is excluded

                        count++;
                        if(ftable != NULL) {
                            // Fill table. filename...
                            static char foo[1024];
                            sprintf(foo, "%s\\%s", sd, it->filename().string().c_str());
                            strcpy(ftable[*fti].fname, foo);

                            // Modification time
//WARNING original mit creation time
//TODO
//							ftable[*fti].timestamp = (DWORD) FILETIMEToUnixTime(fd.ftCreationTime);
                            ftable[*fti].timestamp = static_cast<long int>(last_write_time(p));

                            // Size, TODO: 4GB limit check?
                            //ftable[*fti].len = (fd.nFileSizeHigh * MAXDWORD) + fd.nFileSizeLow;
//INCOMPLETE
//TODO
                            ftable[*fti].len = file_size(p);

                            (*fti)++;
                        }
                    }
                }


                return count;

            }
        }
    }
    catch (const filesystem::filesystem_error& ex)
    {
        printf(ex.what());
    }
}

// Create a PBO, sd = source directory, df = target file
bool pboPack(char *sd, char *df, bool overwrite) {
    // Check for excludes file
    char excludes[EX_NUM][EX_LEN];
    memset(excludes, 0, EX_NUM*EX_LEN);
    char exname[FNAMELEN];
    sprintf(exname, "%s\\%s", sd, EXCLUDEFILE);
    FILE *ef = fopen(exname, "rb");
    int eidx = 0;
    if(ef) {
        printf("Excluded: ");
        while(fgets(excludes[eidx], EX_LEN, ef)) {
            // Strip line feed
            if(excludes[eidx][strlen(excludes[eidx])-2] == 0x0D) // DOS
                excludes[eidx][strlen(excludes[eidx])-2] = 0x00;
            if(excludes[eidx][strlen(excludes[eidx])-1] == 0x0A) // Unix
                excludes[eidx][strlen(excludes[eidx])-1] = 0x00;
            printf("<%s> ", excludes[eidx]);
            eidx++;
        }
        printf("\n");
        fclose(ef);
    }

    int c = getDirFiles(sd, NULL, NULL, excludes); // Get number of files
    if(c == 0)
        return false;
    printf("%d files\n", c);

    // Allocate file table & fill it
    FTENTRY *ft = new FTENTRY[c];
    static int fti;
    fti = 0;
    getDirFiles(sd, ft, &fti, excludes); // Get files

    // Open output file and create header
    char outname[FNAMELEN];
    if(strlen(df) != 0)
        sprintf(outname, "%s", df);
    else
        sprintf(outname, "%s.pbo", sd);

    // Ask for overwriting
    if(!overwrite && fileExists(outname)) {
        char str[256];
        sprintf(str, "File %s already exists", outname);
        return true; // Abort cleanly
    }

    FILE *o = fopen(outname, "w+b");
    if(!o) {
        printf("Unable to open %s for writing!\n", outname);
        return false;
    }

    // Prepare SHA-1
    sha1_context ctx;
    sha1_starts(&ctx);

    // "sreV" Header
    char hdrb[21];
    memset(hdrb, 0, 21);
    strcpy(hdrb+1, "sreV");
    fwrite(hdrb, 21, 1, o);

    // Check for prefix file & write it
    char foo[FNAMELEN];
    sprintf(foo, "%s\\%s", sd, PREFIXFILE);
    FILE *hf = fopen(foo, "rb");
    if(hf) {
        char prefix[FNAMELEN];
        fgets(prefix, FNAMELEN, hf);
        fclose(hf);
        fputs("prefix", o);
        fputc(0x00, o);
        fputs(prefix, o);
        fputc(0x00, o);
        fputc(0x00, o);
        printf("prefix: %s\n", prefix);
    } else {
        fputc(0x00, o); // Header terminator
    }

    // Write file table
    for(int i=0; i<fti; i++) {
        fputs(ft[i].fname+strlen(sd)+1, o);
        fputc(0x00, o);

        PBOENTRY e;
        e.PackingMethod = 0;
        e.OriginalSize = 0;
        e.Reserved = 0;
        e.TimeStamp = ft[i].timestamp;
        e.DataSize = ft[i].len;
        fwrite(&e, sizeof(PBOENTRY), 1, o);
        //printf("file %d: %s\n", i, ft[i].fname);
    }

    // Write blank separator block
    memset(hdrb, 0, 21);
    fwrite(hdrb, 21, 1, o);

    // Seek back & calculate hash for current data
    DWORD fooptr = ftell(o);
    BYTE *food = new BYTE[fooptr];
    fseek(o, 0, SEEK_SET);
    fread(food, fooptr, 1, o);
    fseek(o, fooptr, SEEK_SET);
    sha1_update(&ctx, (uchar*) food, fooptr);
    delete[] food;

    // Write file data
    for(int i=0; i<fti; i++) {
        printf("file %d/%d: %s (%d KB)\n", i, fti, ft[i].fname, ft[i].len/1024);

        FILE *inp = fopen(ft[i].fname, "rb");
        if(!inp) {
            printf("Warning! Cannot open file for reading!\n");
            continue;
        }

        // Read from file, write to pbo
#define BUFSIZE	1024*1024*4
        char *buf = new char[BUFSIZE];
        int w = ft[i].len;
        while(w > 0) {
            int read = fread(buf, 1, w>BUFSIZE?BUFSIZE:w, inp);
            fwrite(buf, read, 1, o);
            w-=BUFSIZE;
            sha1_update(&ctx, (uchar*) buf, read);
        }

        delete[] buf;
        fclose(inp);
    }

    // Write 0x00 + SHA-1 hash
    fputc(0x00, o);
    BYTE sha1sum[20];
    sha1_finish(&ctx, sha1sum);
    fwrite(sha1sum, 20, 1, o);

    fclose(o);
    delete[] ft;

    return true;
}
