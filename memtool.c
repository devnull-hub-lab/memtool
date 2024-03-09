#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc < 4)
    {   //                       0  1   2     3  4       5          6     7            8
        fprintf(stderr, "Usage: %s -p <PID> [-r -i <start address> -e <end address>] [-N] \n\n", argv[0]);
        //fprintf(stderr, "Usage: %s -p <PID> [-w -i <start address> -s 'patern'            \n\n", argv[0]);
        fprintf(stderr, "-p: PID\n");
        fprintf(stderr, "-r: Read memory address content\n");
        fprintf(stderr, "-w: Write memory address content\n");
        fprintf(stderr, "-i: Initial memory address range\n");
        fprintf(stderr, "-e: End memory address range\n");
        fprintf(stderr, "-N: Show Null Bytes Address (0x00)\n");
        fprintf(stderr, "-s: Search value in memory\n\n");
        exit(1);
    }

    int flags_opt = 0;
    short show_null_bytes = 0;
    short flag_read = 0, flag_write = 0;
    
    int PID = 0;
    char *start_memory_address_chr = NULL, *end_memory_address_chr = NULL;
    unsigned long start_memory_address = 0x00, end_memory_address = 0x00;

    while ((flags_opt = getopt(argc, argv, "p:rwi:e:Ns:")) != -1)
    {
        switch (flags_opt)
        {
            case 'p':
                PID = atoi(optarg);
                break;
            case 'r':
                flag_read = 1;
                break;
            case 'w':
                flag_write = 1;
                break;
            case 'N':
                show_null_bytes = 1;
                break;
            case 'i':
                    start_memory_address_chr = optarg;
                    break;
            case 'e':
                    end_memory_address_chr = optarg;
                    break;

            case '?':
                if (optopt == 'p')
                    fprintf (stderr, "Option -%c requires a PID value.\n", optopt);
                else if (optopt == 's' || optopt == 'e')
                    fprintf (stderr, "Option -%c requires a Memory Address value.\n", optopt);
                else
                    fprintf (stderr, "Invalid option `-%c'.\n", optopt);
                return 1;
            default:
                fprintf(stderr, "Invalid option: -%c\n", optopt);
                exit(1);
        }
    }

    if (flag_read == 1)
    {
        start_memory_address = strtoul(start_memory_address_chr, NULL, 16);
        end_memory_address   = strtoul(end_memory_address_chr,   NULL, 16);
    }

    if (PID == 0)
    {
        fprintf(stderr, "PID cannot be 0. Enter a valid PID \n");
        exit(1);
    }

    char pathMemFile[64];
    snprintf(pathMemFile, sizeof(pathMemFile), "/proc/%d/mem", PID);

    FILE *memFile = fopen(pathMemFile, "rb");
    if (!memFile)
    {
        fprintf(stderr, "Unable to open %s\n", pathMemFile);
        return 1;
    }

    fseek(memFile, start_memory_address, SEEK_SET);
    unsigned char byte;
    while (ftell(memFile) < end_memory_address)
    {
        fread(&byte, 1, 1, memFile);
        if (show_null_bytes == 1)
            printf("%p: %02x  (%c)\n", (void *)ftell(memFile) - 1, byte, isprint(byte) ? byte : '.');
        else
            if (byte != 0x00)
                printf("%p: %02x  (%c)\n", (void *)ftell(memFile) - 1, byte, isprint(byte) ? byte : '.');
    }

    fclose(memFile);

    return 0;
}
