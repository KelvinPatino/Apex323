// Bitmap Reader
// This program reads, displays, and processes BMP files, supporting actions such as hiding or extracting hidden data.

#include "BitmapReader.h"  // Include the header with definitions for BITMAPFILEHEADER, BITMAPINFOHEADER, RGBQUAD, constants, etc.

// =========================
// GLOBAL VARIABLES
// =========================

// Pointers to BMP file headers
BITMAPFILEHEADER *gpCoverFileHdr, *gpStegoFileHdr;

// Pointers to BMP info headers
BITMAPINFOHEADER *gpCoverFileInfoHdr, *gpStegoFileInfoHdr;

// Pointers to color palettes (if used)
RGBQUAD *gpCoverPalette, *gpStegoPalette;

// File sizes for the cover, message, and stego BMP files
unsigned int gCoverFileSize, gMsgFileSize, gStegoFileSize;

// File path and file name storage for each file
char gCoverPathFileName[MAX_PATH], *gCoverFileName;
char gMsgPathFileName[MAX_PATH], *gMsgFileName;
char gStegoPathFileName[MAX_PATH], *gStegoFileName;

// Action to perform (e.g., hide, extract, etc.)
char gAction;

// Number of bits to use for hiding/extracting data
char gBitsToSet;

// =========================
// Initialize global variables
// =========================
void initGlobals()
{
    // Reset all pointer variables to NULL
    gpCoverFileHdr = NULL;
    gpStegoFileHdr = NULL;
    gpCoverFileInfoHdr = NULL;
    gpStegoFileInfoHdr = NULL;
    gpCoverPalette = NULL;
    gpStegoPalette = NULL;

    // Reset file sizes
    gCoverFileSize = gMsgFileSize = gStegoFileSize = 0;

    // Reset all command-line string paths and file name pointers
    gCoverPathFileName[0] = 0;
    gCoverFileName = NULL;
    gMsgPathFileName[0] = 0;
    gMsgFileName = NULL;
    gStegoPathFileName[0] = 0;
    gStegoFileName = NULL;

    gAction = 0;       // Default action is none
    gBitsToSet = 1;    // Default to 1 bit per pixel for steganography

    return;
}

// =========================
// Display BMP file info
// =========================
void displayFileInfo(char *pFileName,
                     BITMAPFILEHEADER *pFileHdr, 
                     BITMAPINFOHEADER *pFileInfo,
                     RGBQUAD *ptrPalette,
                     unsigned char *pixelData)
{
    int numColors, i;

    // Print the name of the file being analyzed
    printf("\nFile Information for %s: \n\n", pFileName);

    // Show file header details
    printf("File Header Info:\n");
    printf(
    "File Type: %c%c\n"       // Display 2-character file type (usually "BM" for Bitmap)
    "File Size: %d\n"         // Display file size in bytes
    "Data Offset: %d\n\n",    // Display offset to pixel data
     
    // Extract and print the first character of bfType
    // bfType is a 16-bit value (e.g., 0x4D42 for "BM")
    // & 0xFF extracts the lower byte (e.g., 0x42 -> 'B')
    (pFileHdr->bfType & 0xFF),

    // Shift right 8 bits to get the high byte (e.g., 0x4D -> 'M')
    (pFileHdr->bfType >> 8),

    // Print the total size of the BMP file in bytes
    pFileHdr->bfSize,

    // Print the byte offset where pixel data begins
    // For standard BMPs, this is usually 54 (14-byte file header + 40-byte info header)
    pFileHdr->bfOffBits
);

    // Determine the number of colors based on bits per pixel
    switch(pFileInfo->biBitCount)
    {
        case 1: numColors = 2; break;
        case 4: numColors = 16; break;
        case 8: numColors = 256; break;
        case 16: numColors = 65536; break;
        case 24: numColors = 16777216; break;
        default: numColors = -1; break;
    }

    // Display image header info
    printf("Bit Map Image Info:\n\nSize Info Header:%d\nWidth:%d\nHeight:%d\nPlanes:%d\n"
           "Bits/Pixel:%d ==> %d colors\n"
           "Compression:%d\nImage Size:%d\nRes X:%d\nRes Y:%d\nColors:%d\nImportant Colors:%d\n\n",
           pFileInfo->biSize, 
           pFileInfo->biWidth, 
           pFileInfo->biHeight, 
           pFileInfo->biPlanes, 
           pFileInfo->biBitCount, numColors,
           pFileInfo->biCompression, 
           pFileInfo->biSizeImage, 
           pFileInfo->biXPelsPerMeter,
           pFileInfo->biYPelsPerMeter,
           pFileInfo->biClrUsed,
           pFileInfo->biClrImportant);

    // If palette is used (only for <= 8-bit images), display it
    if(pFileInfo->biBitCount > 16 || numColors == -1)
    {
        printf("\nNo Palette\n\n");
    }
    else
    {
        printf("Palette:\n\n");
        for(i = 0; i < numColors; i++)
        {
            printf("R:%02x   G:%02x   B:%02x\n", 
                   ptrPalette->rgbRed, ptrPalette->rgbGreen, ptrPalette->rgbBlue);
            ptrPalette++;
        }
    }

    // Display first 24 bytes of pixel data for preview
    printf("\n Pixel Data: \n\n");
    // Loop through the first 24 bytes of the pixel data
	for(int i = 0; i < 24; i++)
	{
		// Dereference the pointer to get the byte at position i
		// and print it as a 2-digit uppercase hexadecimal number
		// %02X = format specifier:
		//   %X  = print as hex (uppercase)
		//   02  = pad with zero if less than 2 digits (e.g., '0A', '3F', '00')
		// *(pixelData + i) = get the byte i positions after pixelData
		printf("%02X ", *(pixelData + i));
	}
}

// =========================
// Quick validity check for BMP format
// =========================
bool isValidBitMap(char *filedata, BITMAPINFOHEADER *pFileInfo, BITMAPFILEHEADER *pFileHdr)
{
    char *pixelData;

	if( filedata[0] != 'B' || filedata[1] != 'M') {
	printf("Error, file does not cotain a bitmap type");
	return false;
	}

	if(pFileInfo->biBitCount < 24){

		printf("Error, file does not cotain a 24 bitmap image");
		return false;

		//only allow a bitmap position to start at the bottome left 
	}
	//if((pixelData - filedata) > pFileHdr->bfOffBits){
	//	printf("Error, file did not obtain correct data starting position");
	//	return false;
	//} 
	return true;
	
}

// =========================
// Read entire BMP file into memory
// =========================
unsigned char *readBitmapFile(char *fileName, unsigned int *fileSize)
{
    FILE *ptrFile;
    unsigned char *pFile;

    // Open file for binary read
    ptrFile = fopen(fileName, "rb");
    if(ptrFile == NULL)
    {
        printf("Error in opening file: %s.\n\n", fileName);
        exit(-1);
    }

    // Determine file size
    fseek(ptrFile, 0, SEEK_END);
    *fileSize = ftell(ptrFile);
    fseek(ptrFile, 0, SEEK_SET);

    // Allocate memory to store file contents
    pFile = (unsigned char *) malloc(*fileSize);
    if(pFile == NULL)
    {
        printf("Error - Could not allocate %d bytes of memory for bitmap file.\n\n", *fileSize);
        exit(-1);
    }

    // Read entire file into buffer
    fread(pFile, sizeof(unsigned char), *fileSize, ptrFile);
    fclose(ptrFile);

    return(pFile);
}

// =========================
// Write buffer to disk as BMP file
// =========================
int writeFile(char *filename, int fileSize, unsigned char *pFile)
{
    FILE *ptrFile;
    int x;

    // Open file for binary write
    ptrFile = fopen(filename, "wb+");
    if(ptrFile == NULL)
    {
        printf("Error opening file (%s) for writing.\n\n", filename);
        exit(-1);
    }

    // Write entire buffer to file
    x = (int) fwrite(pFile, sizeof(unsigned char), fileSize, ptrFile);
    if(x != fileSize)
    {
        printf("Error writing file %s.\n\n", filename);
        exit(-1);
    }

    fclose(ptrFile);
    return(SUCCESS);
}

// =========================
// Print usage/help message
// =========================
void Usage(char *programName)
{
    char prgname[MAX_PATH];
	int idx;

	idx = strlen(programName);
	while (idx >= 0)
	{
		if(programName[idx] == '\\') break;
		idx--;
	}

	strcpy(prgname, &programName[idx + 1]);
	fprintf(stdout, "\n\n");
	
	int choice;

	fprintf(stdout, "Program intentions: read a cover image and secret message\n");
	fprintf(stdout, "embed data in a 24-bit image using differential value encoding\n");
	fprintf(stdout, "resulting pixel value applies secret message, then  allows for extraction to process\n");
	fprintf(stdout, "Enter 1 to get information or 9 to exit: ");
	scanf("%d", &choice);
	
	if(choice == 1){
	fprintf(stdout, "To print bitmap information:\n\n");
	fprintf(stdout, "%s -c < filename.bmp >\n\n", prgname);
	
	fprintf(stdout, "To Hide:\n");
	fprintf(stdout, "\t%s -hide -c <cover file> -m <msg file> [-s <stego file>] [-b <number of bits>]\n\n", prgname);

	fprintf(stdout, "To Extract:\n");
	fprintf(stdout, "\t%s -extract -s <stego file> [-m <message file>] [-b <number of bits>]\n\n", prgname);

	fprintf(stdout, "Parameters:\n");
	fprintf(stdout, "Set number of bits to hide per pixel:	-b ( 1 to 7 )\n\n");

	fprintf(stdout, "\n\tNOTES:\n\t1.Order of parameters is irrelevant.\n\t2.All selections in \"[]\" are optional.\n\n");

	}
	else if(choice == 9){
		exit(0);
	}

}

// =========================
// Parse command-line arguments
// =========================
void parseCommandLine(int argc, char *argv[])
{
    int cnt;

    // If no arguments, show usage
    if(argc < 2) Usage(argv[0]);

    gAction = 0;
    gCoverPathFileName[0] = 0;
    gMsgPathFileName[0] = 0;
    gStegoPathFileName[0] = 0;

    cnt = 1;  // Start after program name

    while(cnt < argc)
    {
        if(_stricmp(argv[cnt], "-c") == 0) // Cover file
        {
            cnt++;
            if(cnt == argc) { fprintf(stderr, "Error - no file after %s\n", argv[cnt-1]); exit(-1); }
			printf("after errorcheck\n");
            if(gCoverPathFileName[0] != 0) { fprintf(stderr, "Cover file already specified.\n"); exit(-2); }
			printf("after Cover file exist already check\n");
			DWORD result = GetFullPathName(argv[cnt], MAX_PATH, gCoverPathFileName, &gCoverFileName);
				if (result == 0 || result > MAX_PATH) {
					fprintf(stderr, "Error: could not resolve path for %s\n", argv[cnt]);
					exit(-1);
				}
			printf("next argument:%d=%s\ncoverpathfilename = %s\n", cnt, argv[cnt], gCoverPathFileName);

        }
        else if(_stricmp(argv[cnt], "-m") == 0) // Message file
        {
            cnt++;
            if(cnt == argc) { fprintf(stderr, "Error - no file after %s\n", argv[cnt-1]); exit(-1); }
            if(gMsgPathFileName[0] != 0) { fprintf(stderr, "Message file already specified.\n"); exit(-2); }
			DWORD result = GetFullPathName(argv[cnt], MAX_PATH, gMsgPathFileName, &gMsgFileName);
				if (result == 0 || result > MAX_PATH) {
					fprintf(stderr, "Error: could not resolve path for %s\n", argv[cnt]);
					exit(-1);
				}
        }
        else if(_stricmp(argv[cnt], "-s") == 0) // Stego file
        {
            cnt++;
            if(cnt == argc) { fprintf(stderr, "Error - no file after %s\n", argv[cnt-1]); exit(-1); }
            if(gStegoPathFileName[0] != 0) { fprintf(stderr, "Stego file already specified.\n"); exit(-2); }
            GetFullPathName(argv[cnt], MAX_PATH, gStegoPathFileName, &gStegoFileName);
			DWORD result = GetFullPathName(argv[cnt], MAX_PATH, gStegoPathFileName, &gStegoFileName);
				if (result == 0 || result > MAX_PATH) {
					fprintf(stderr, "Error: could not resolve path for %s\n", argv[cnt]);
					exit(-1);
				}
        }
        else if(_stricmp(argv[cnt], "-b") == 0) // Bits to set
        {
            cnt++;
            if(cnt == argc) { fprintf(stderr, "Error - no number after %s\n", argv[cnt-1]); exit(-1); }

            char temp[3] = {0};
            strncpy(temp, argv[cnt], 2); // Get first 2 characters
            gBitsToSet = atoi(temp);

            if(gBitsToSet < 1 || gBitsToSet > 7)
            {
                fprintf(stderr, "Bits to hide must be between 1 and 7.\n");
                exit(-1);
            }
        }
        else if(_stricmp(argv[cnt], "-hide") == 0) // Hide action
        {
            if(gAction) { fprintf(stderr, "Only one action allowed.\n"); exit(-1); }
            gAction = ACTION_HIDE;
            printf("you chose to hide!\n");
        }
        else if(_stricmp(argv[cnt], "-extract") == 0) // Extract action
        {
            if(gAction) { fprintf(stderr, "Only one action allowed.\n"); exit(-1); }
            gAction = ACTION_EXTRACT;
            printf("you chose to extract!\n");
        }
		else if(_stricmp(argv[cnt], "-copy") == 0) // Copy action
        {
			cnt++;
            if(gAction) { fprintf(stderr, "Only one action allowed.\n"); exit(-1); }
            gAction = ACTION_COPY;
			printf("\nyou chose to copy\n");
        }
		else{
			printf("all else case\n");
			printf("\ncurrent arg counter: %d, official argcount: %d\n", cnt, argc);
			if (cnt == argc){
				printf("\nreached end of arguments, exiting while loop\n");
			}
			cnt++;
		}
    }
	printf("\n done parsing arguments. action= %d\n", gAction);
}

// =========================
// Main Program Entry Point
// =========================
int main(int argc, char *argv[])
{
    unsigned char *coverData, *pixelData;

    // Initialize variables
    initGlobals();

    // Parse user input
    parseCommandLine(argc, argv);
	printf("\narguments parsed\n");
    // If a cover file is specified, read and display its contents
    if(gCoverPathFileName[0] != 0)
    {
		printf("\ncoverfile specified\n");
        // Read BMP file into memory
        coverData = readBitmapFile(gCoverPathFileName, &gCoverFileSize);

        // Assign pointers to various BMP structures
        gpCoverFileHdr = (BITMAPFILEHEADER *) coverData;
        gpCoverFileInfoHdr = (BITMAPINFOHEADER *) (coverData + sizeof(BITMAPFILEHEADER));
		if(!isValidBitMap((char*)coverData, gpCoverFileInfoHdr, gpCoverFileHdr))
		{
			free(coverData);  
			exit(-1);
		}
        gpCoverPalette = (RGBQUAD *) (coverData + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER));
        pixelData = coverData + gpCoverFileHdr->bfOffBits;

        // Display file header, info header, palette, and pixel data
        displayFileInfo(gCoverPathFileName, gpCoverFileHdr, gpCoverFileInfoHdr, gpCoverPalette, pixelData);
		//void displayFileInfo(char *pFileName, BITMAPFILEHEADER *pFileHdr,  BITMAPINFOHEADER *pFileInfo, RGBQUAD *ptrPalette, unsigned char *pixelData)

    }
	if(gAction == 3 && gCoverPathFileName[0] !=0 ){
		printf("\nYou chose to copy a cover file: \n");
		//int writeFile(char *filename, int fileSize, unsigned char *pFile)
		char outputName[11] = "output.bmp";
		int coverFileSize = (int) gCoverFileSize;
		int didWriteFile = writeFile(outputName,coverFileSize, coverData  );
		if (didWriteFile){
			printf("\nFile succesfully Output\n");
		}
		else{
			printf("\n File failed to write\n");
		}
	

	}

    return 0;
}
