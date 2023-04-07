#include "zc_io.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

// the zc_file struct is analogous to the FILE struct that you get from fopen.
struct zc_file
{
    // pointer to the virtual memory space
    void *fileLocation;
    // offset from the start of the virtual memory
    size_t fileOffset;
    // total size of the file
    size_t fileSize;
    // file descriptor for the opened file
    int fileDescriptor;
    // mutex for access to the memory space and number of readers
};

/**************
 * Exercise 1 *
 **************/

zc_file *zc_open(const char *path)
{
    // initialize file structure
    zc_file *file = (zc_file *)malloc(sizeof(zc_file));

    // creates file from path with read write exec permissions if it does not exist, else open
    int fd = open(path, O_CREAT | O_RDWR, 00700);
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

    // we cannot map a file size of 0
    if (file->fileSize != 0)
    {
        // address = NULL means kernel can map anywhere, length = no. of B to be mapped, protect = define access, we set to READ and WRITE
        // flags = sharable by other processes, file descriptor, offset
        // mmap helps to map virtual memory and allows us to access it like an array
        void *fp = mmap(NULL, file->fileSize, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, file->fileDescriptor, file->fileOffset);
        if (fp == MAP_FAILED)
        {
            perror("ERROR: Failed to map virtual memory for pre-existing file!\n");
        }
        file->fileLocation = fp;
    }

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
    // can cast a void pointer to a char pointer, commences at file offset
    // since char* is just a string, this is basically just the starting addr of the first char + size of substring
    char *dataPointer = ((char *)file->fileLocation) + file->fileOffset;
    file->fileOffset += *size;
    return dataPointer;
}

void zc_read_end(zc_file *file)
{
    // To implement
}

char *zc_write_start(zc_file *file, size_t size)
{
    // cond 1: if file size is 0, it means we initially created a new file, and now want to write to it, so create virtual memory
    // cond 2: alternatively, we have a pre existing file but of insufficient space
    if (file->fileSize == 0 || file->fileOffset + size > file->fileSize)
    {
        void *fp;
        if (file->fileSize == 0)
        {
            // this only maps the virtual memory, but does not change the file size of the created file (empty)
            fp = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED_VALIDATE, file->fileDescriptor, file->fileOffset);
            if (fp == MAP_FAILED)
            {
                perror("ERROR: Failed to map virtual memory for newly created file!\n");
            }
        }
        else
        {
            // insufficient space, we need to remap, MREMAP_MAYMOVE signifies that kernel can relocate mapping if fails to expand
            fp = mremap(file->fileLocation, file->fileSize, file->fileSize + size, MREMAP_MAYMOVE);
            if (fp == MAP_FAILED)
            {
                perror("ERROR: Failed to remap virtual memory!\n");
            }
        }
        file->fileLocation = fp;
        file->fileSize += size;

        // we then truncate to extend the size of the file, once we have sucessfully remapped
        int status = ftruncate(file->fileDescriptor, file->fileOffset + size);
        if (status == -1)
        {
            perror("Failed to truncate file size when writing!\n");
        }
    }
    // sufficient space now, so just get the data
    char *dataPointer = ((char *)file->fileLocation) + file->fileOffset;
    file->fileOffset += size;
    return dataPointer;
}

void zc_write_end(zc_file *file)
{
    int syncStatus = msync(file->fileLocation, file->fileSize, MS_SYNC);
    if (syncStatus == -1)
    {
        perror("ERROR: Failed to sync memory to file!\n");
    }
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