#include "zc_io.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// The zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file
{
    /* Some suggested fields :
        - pointer to the virtual memory space
        - offset from the start of the virtual memory
        - total size of the file
        - file descriptor to the opened file
        - mutex for access to the memory space and number of readers
    */
    void *fileLocation;
    size_t fileOffset;
    size_t fileSize;
    int fileDescriptor;
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path)
{
    // initialize file structure
    zc_file *file = (zc_file *)malloc(sizeof(zc_file));

    // creates file from path if it does not exist, else open
    int fd = open(path, O_CREAT | O_RDWR);
    if (fd == -1)
    {
        perror("ERROR: Error when opening/ creating file!\n");
    }
    file->fileDescriptor = fd;

    // for fstat
    struct stat buffer;
    int fs = fstat(fd, &buffer);
    if (fs == -1)
    {
        perror("ERROR: Failed to read file status of file!\n");
    }
    file->fileSize = buffer.st_size;
    file->fileOffset = 0;

    // address = NULL means kernel can map anywhere, length = no. of B to be mapped, protect = define access, we set to READ and WRITE
    // flags = sharable by other processes, file descriptor, offset
    // mmap helps to map virtual memory and allows us to access it like an array
    void *fp = mmap(NULL, file->fileSize, PROT_READ | PROT_WRITE, MAP_SHARED, file->fileDescriptor, file->fileOffset);
    if (fp == MAP_FAILED)
    {
        perror("ERROR: Failed to map virtual memory!\n");
    }
    file->fileLocation = fp;

    // return pointer to struct
    return file;
}

int zc_close(zc_file *file)
{
    // to sync data in virtual memory to file
    int syncStatus = msync(file->fileLocation, file->fileSize, MS_SYNC);
    if (syncStatus == -1)
    {
        perror("ERROR: Failed to sync memory to file!\n");
        return -1;
    }

    // to teardown shared memory, this can sync but does not guarantee
    int unmapStatus = munmap(file->fileLocation, file->fileSize);
    if (unmapStatus == -1)
    {
        perror("ERROR: Failed to unmap virtual memory!\n");
        return -1;
    }

    // to close the file we originally opened
    int fileStatus = close(file->fileDescriptor);
    if (fileStatus == -1)
    {
        perror("ERROR: Failed to close file!\n");
        return -1;
    }

    return fileStatus;
}

const char *zc_read_start(zc_file *file, size_t *size)
{
    size_t remainingSize = file->fileSize - file->fileOffset;
    // if size remaining is less than size to be read, update size so we don't read out of bounds
    // remember! size is a ptr, to dereference it use *size, to get the addr of a var i, use int* ptr = &i
    if (remainingSize < *size)
    {
        *size = remainingSize;
    }
    // read sys call reads up to the number of 'count' bytes given into the buffer
    //  can cast a void pointer to a char pointer, commences at file offset
    char *str = ((char *)file->fileLocation) + file->fileOffset;
    file->fileOffset += *size;
    return str;
}

void zc_read_end(zc_file *file)
{
    // To implement
}

char *zc_write_start(zc_file *file, size_t size)
{
    // To implement
    return NULL;
}

void zc_write_end(zc_file *file)
{
    // To implement
}

/**************
 * Exercise 2 *
 **************/

off_t zc_lseek(zc_file *file, long offset, int whence)
{
    // To implement
    return -1;
}

/**************
 * Exercise 3 *
 **************/

int zc_copyfile(const char *source, const char *dest)
{
    // To implement
    return -1;
}

/**************
 * Bonus Exercise *
 **************/

const char *zc_read_offset(zc_file *file, size_t *size, long offset)
{
    // To implement
    return NULL;
}

char *zc_write_offset(zc_file *file, size_t size, long offset)
{
    // To implement
    return NULL;
}