#include "single_fs.h"

#include <stdlib.h>
#include <string.h>

#ifdef __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#elif _WIN32
#else
#endif

#define FILE_WRITE_BUFFER_SIZE 8192

int sfs_fs_file_open(sfs_fs_file *mf, sfs_str path) {
  if (!mf || !path.ptr) { return SFS_BAD_ARGUMENT; }

  int fd = open(path.ptr, O_RDWR | O_CREAT, 0666); // adjust the permission HARD TODO
  if (fd == -1) { return SFS_FILE_CANT_CREATE; }

  uint64_t file_size = 0;
  uint64_t capacity = 0;
#ifdef __linux__
  {
    struct stat st_stat;
    if (fstat(fd, &st_stat) == -1) {
      close(fd);
      return SFS_FILE_CANT_STAT;
    }
    file_size = st_stat.st_size;
  }
#elif _WIN32
#else
#endif

  mf->mf_buffer.ptr = calloc(FILE_WRITE_BUFFER_SIZE, 1);

  if (!mf->mf_buffer.ptr) {
    close(fd);
    return SFS_FILE_CANT_ALLOCATE_BUFFER;
  }

  mf->mf_buffer.capacity = FILE_WRITE_BUFFER_SIZE;
  mf->mf_buffer.len = 0;

  mf->mf_fd = fd;
  mf->mf_size = file_size;
  mf->mf_capacity = capacity;

  return SFS_OK;
}

int sfs_fs_write(sfs_fs_file *mf, void *ptr, size_t size, size_t n) {
  // not thread safe TODO
}