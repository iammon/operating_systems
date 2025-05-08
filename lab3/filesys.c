//filesys.c
//Based on a program by Michael Black, 2007
//Revised 11.3.2020 O'Neil

#include <stdio.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Usage: ./filesys <option> [filename]\n");
        return 1;
    }

    // Open the floppy image
    FILE* floppy = fopen("floppya.img", "r+");
    if (floppy == NULL) {
        printf("floppya.img not found\n");
        return 1;
    }

    // Load the disk map from sector 256
    char map[512];
    fseek(floppy, 512 * 256, SEEK_SET);
    for (int i = 0; i < 512; i++)
        map[i] = fgetc(floppy);

    // Load the directory from sector 257
    char dir[512];
    fseek(floppy, 512 * 257, SEEK_SET);
    for (int i = 0; i < 512; i++)
        dir[i] = fgetc(floppy);

    // -----------------------------------------------------
    // Option L: List files in 8.3 format and disk usage
    // -----------------------------------------------------
    if (strcmp(argv[1], "L") == 0) {
        printf("Files on disk:\n");
        int totalUsedBytes = 0;

        for (int i = 0; i < 512; i += 16) {
            if (dir[i] == 0x00) break; // end of directory entries

            // Extract filename (first 8 bytes)
            char name[9] = {0};
            for (int j = 0; j < 8; j++) {
                if (dir[i + j] == 0) break;
                name[j] = dir[i + j];
            }

            // Extension: .x or .t
            char ext = dir[i + 8];
            if (ext != 't' && ext != 'x') continue; // skip malformed

            // Get sector count
            int sectors = (unsigned char)dir[i + 10];
            int sizeInBytes = sectors * 512;
            totalUsedBytes += sizeInBytes;

            // Print filename.ext and size
            printf("%s.%c\t%d bytes\n", name, ext, sizeInBytes);
        }

        // Calculate free bytes (each 0 in map is a free sector)
        int freeSectors = 0;
        for (int i = 0; i < 512; i++) {
            if ((unsigned char)map[i] == 0x00)
                freeSectors++;
        }

        int totalFreeBytes = freeSectors * 512;
        printf("\nTotal used: %d bytes\n", totalUsedBytes);
        printf("Total free: %d bytes\n", totalFreeBytes);
    }

    /*
	//write the map and directory back to the floppy image
    fseek(floppy,512*256,SEEK_SET);
    for (i=0; i<512; i++) fputc(map[i],floppy);

    fseek(floppy,512*257,SEEK_SET);
    for (i=0; i<512; i++) fputc(dir[i],floppy);
    */
    fclose(floppy);
    return 0;
}
