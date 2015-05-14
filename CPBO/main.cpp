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

#include <iostream>
#include <cstring>
#include <cstdlib>
#include "pbo.h"


void usage(void) {
  printf("\nUsage:\n");
  printf("Extract a pbo:\n");
  printf("  cpbo.exe [-y] -e (filename.pbo) [directory]\n");
  printf("  directory name is optional, PBO file name used if undefined\n");
  printf("  optional -y parameter overwrites directory without asking\n\n");
  printf("Make a pbo:\n");
  printf("  cpbo.exe [-y] -p (directory) [filename.pbo]\n");
  printf("  pbo name is optional, directory name used if undefined\n");
  printf("  optional -y parameter overwrites pbo without asking\n\n");
  printf("Associate cpbo with PBO files and directories:\n");
  printf("  cpbo.exe -a\n");
  exit(0);	
}


int main(int argc, char* argv[]) {
  // Compare console title & try to find out if we were run from terminal or directly


  char rnd[256];
  sprintf(rnd, "_cpbo_tmp__%d__", 132465789);


  printf("%s <http://www.kegetys.net>\n", VERSIONSTRING);

  if(argc < 2) {
	  usage(); // Ran from console
  }

  // Parse parameters
  bool overwrite = false;
  for(int ai=1;ai<argc;ai++) {

	if(!strcasecmp("-y", argv[ai])) {
	  // Overwrite all files
	  overwrite = true;
	}
/*
	if(!strcasecmp("-e", argv[ai])) {
	  if(argc-ai < 2)
		usage();

	  // Extract...
	  char *odir = "";
	  if(argc >= ai+3)
		odir = argv[ai+2];

	  printf("Extracting %s\n", argv[ai+1]);
	  if(pboEx(argv[ai+1], odir, overwrite, false)) {
		printf("Done.\n");
		return 1;
	  } else {
		printf("Extract of one or more files failed");
		return -1;
	  }
	}
*/
	if(!strcasecmp("-p", argv[ai])) {
	  if(argc-ai < 2)
		usage();

	  // Create PBO.
	  char *ofile = "";
	  if(argc >= ai+3)
		ofile = argv[ai+2];

	  printf("Creating %s\n", argv[ai+1]);
	  if(pboPack(argv[ai+1], ofile, overwrite)) {
		printf("Done.\n");
		return 1;
	  } else {
		printf("PBO creation failed");
		return -1;
	  }
	}

  }
}
