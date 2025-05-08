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
        printf("Error: floppya.img not found\n");
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
            if (dir[i] == 0x00) break;

            // Extract filename (first 8 bytes)
            char name[9] = {0};
            for (int j = 0; j < 8; j++) {
                if (dir[i + j] == 0) break;
                name[j] = dir[i + j];
            }

            // Get file type and sector count
            char ext = dir[i + 8];
            int sectors = (unsigned char)dir[i + 10];
            int size = sectors * 512;
            totalUsedBytes += size;

            // Print in 8.3 format
            printf("%s.%c\t%d bytes\n", name, ext, size);
        }

        // Count free sectors
        int freeSectors = 0;
        for (int i = 0; i < 512; i++) {
            if ((unsigned char)map[i] == 0x00)
                freeSectors++;
        }

        int totalFreeBytes = freeSectors * 512;
        printf("\nTotal used: %d bytes\n", totalUsedBytes);
        printf("Total free: %d bytes\n", totalFreeBytes);
    }

    // -----------------------------------------------------
    // Option P: Print a text file to screen
    // -----------------------------------------------------
    else if (strcmp(argv[1], "P") == 0) {
        if (argc < 3) {
            printf("Usage: ./filesys P filename\n");
            fclose(floppy);
            return 1;
        }

        char* target = argv[2];
        int found = 0;

        for (int i = 0; i < 512; i += 16) {
            if (dir[i] == 0x00) continue;

            // Build filename from dir
            char name[9] = {0};
            for (int j = 0; j < 8; j++) {
                if (dir[i + j] == 0) break;
                name[j] = dir[i + j];
            }

            if (strncmp(target, name, 8) == 0) {
                found = 1;

                if (dir[i + 8] != 't') {
                    printf("Error: '%s' is not a text file.\n", name);
                    fclose(floppy);
                    return 1;
                }

                int start = (unsigned char)dir[i + 9];
                int length = (unsigned char)dir[i + 10];

                char buffer[12288];
                fseek(floppy, 512 * start, SEEK_SET);
                for (int j = 0; j < length * 512; j++) {
                    buffer[j] = fgetc(floppy);
                    if (buffer[j] == 0x00) break;
                    putchar(buffer[j]);
                }

                printf("\n");
                break;
            }
        }

        if (!found) {
            printf("Error: File '%s' not found.\n", argv[2]);
        }
    }

    // -----------------------------------------------------
    // Option M: Make a new text file (1 sector, 512 bytes)
    // -----------------------------------------------------
    else if (strcmp(argv[1], "M") == 0) {
        if (argc < 3) {
            printf("Usage: ./filesys M filename\n");
            fclose(floppy);
            return 1;
        }

        char* newName = argv[2];
        if (strlen(newName) == 0) {
            printf("Error: Invalid file name.\n");
            fclose(floppy);
            return 1;
        }

        // Step 1: Search directory
        int dirIndex = -1;
        for (int i = 0; i < 512; i += 16) {
            if (dir[i] == 0x00 && dirIndex == -1) {
                dirIndex = i; // First free slot
            } else {
                char existing[9] = {0};
                for (int j = 0; j < 8 && dir[i + j] != 0; j++)
                    existing[j] = dir[i + j];

                if (strncmp(newName, existing, 8) == 0) {
                    printf("Error: Duplicate or invalid file name.\n");
                    fclose(floppy);
                    return 1;
                }
            }
        }

        if (dirIndex == -1) {
            printf("Error: No free directory entries.\n");
            fclose(floppy);
            return 1;
        }

        // Step 2: Find a free sector
        int sectorIndex = -1;
        for (int i = 0; i < 512; i++) {
            if ((unsigned char)map[i] == 0x00) {
                sectorIndex = i;
                break;
            }
        }

        if (sectorIndex == -1) {
            printf("Error: Insufficient disk space.\n");
            fclose(floppy);
            return 1;
        }

        // Step 3: Prompt user for content
        char buffer[512] = {0};
        printf("Enter file content (max 511 characters):\n");
        fgets(buffer, 511, stdin); // Safe input with newline
        buffer[strcspn(buffer, "\n")] = '\0'; // remove newline

        // Step 4: Fill directory entry
        for (int i = 0; i < 8; i++) {
            if (i < strlen(newName)) dir[dirIndex + i] = newName[i];
            else dir[dirIndex + i] = 0x00;
        }

        dir[dirIndex + 8] = 't';                 // file type
        dir[dirIndex + 9] = (char)sectorIndex;   // start sector
        dir[dirIndex + 10] = 1;                  // length (1 sector)

        // Step 5: Mark map and write to disk
        map[sectorIndex] = (char)0xFF;

        fseek(floppy, 512 * sectorIndex, SEEK_SET);
        for (int i = 0; i < 512; i++) {
            fputc(buffer[i], floppy);
        }

        // Step 6: Write map and dir back
        fseek(floppy, 512 * 256, SEEK_SET);
        for (int i = 0; i < 512; i++) fputc(map[i], floppy);

        fseek(floppy, 512 * 257, SEEK_SET);
        for (int i = 0; i < 512; i++) fputc(dir[i], floppy);

        printf("File '%s.t' created using sector %d.\n", newName, sectorIndex);
    }

    // -----------------------------------------------------
    // Catch invalid options
    // -----------------------------------------------------
    else {
        printf("Invalid option. Use L, P, M, or D.\n");
    }

    fclose(floppy);
    return 0;
}

