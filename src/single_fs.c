
// #define _GNU_SOURCE // Hard TODO

#include "single_fs.h"
#include <stdio.h>
#include <string.h>

#ifdef __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#elif _WIN32
#else
#endif

static const sfs_header EMPTY_HEADER = (sfs_header){0};
static const sfs_super_block EMPTY_SUPER_BLOCK = (sfs_super_block){0};

#define KiB (1024)
#define MiB (1048576)
#define GiB (1073741824)
#define TiB (1099511627776)

const sfs_header DEFAULT_HEADER = {
    .h_format_magic_number = HEADER_FORMAT_MAGIC_NUMBER,
    .h_version_number_major = HEADER_VERSION_NUMBER_MAJOR,
    .h_version_number_minor = HEADER_VERSION_NUMBER_MINOR,
    .h_block_size = HEADER_BLOCK_SIZE,
    .h_filename_limit = HEADER_FILENAME_LIMIT,
};

// HARD TODO
static const sfs_super_block DEFAULT_SUPER_BLOCK = {
    .s_inodes_count = 0,
    .s_blocks_count = 0,
    .s_r_blocks_count = 0,
    .s_free_blocks_count = 0,
    .s_free_inodes_count = 0,
    .s_root_inode_block = 0,
};

static void *create_inode_block(sfs_object *obj) {
  const int block_size = obj->o_header.h_block_size;

  // I could also hard code this since all these are tightly arranged TODO
  const int space_left_for_inodes = block_size - sizeof(sfs_inode_block_header);
  const int inode_len = space_left_for_inodes / sizeof(sfs_inode);

  sfs_inode_block_header header = {
      .ibh_occupied_inode_bit_lo = 0,
      .ibh_occupied_inode_bit_hi = 0,
      .ibh_len = inode_len,
  };

  // uint8_t *block_ptr = calloc_block(obj);

  // memcpy(block_ptr, &header, sizeof(sfs_inode_block_header));

  // for (int i = sizeof(sfs_inode_block_header); i < block_size; i += sizeof(sfs_inode)) {
  //   sfs_inode *inode_ptr = (sfs_inode *)(block_ptr + i);
  //   inode_ptr = (sfs_inode) {

  //   }
  // }

  return NULL;
}

/* OS spesific */
#ifdef __linux__

#elif _WIN32
#else
#endif

static uint64_t lround_up(uint64_t number, uint64_t coeff) { return (number + coeff - 1) / coeff * coeff; }

// static int memory_map_object_file(FILE *fp, uint64_t capacity) {
//   if (mf->map)
//     munmap(mf->map, mf->capacity);

//   /* grow file using libc only */
//   if (fseeko(mf->fp, new_capacity - 1, SEEK_SET) != 0 || fputc(0, mf->fp) == EOF || fflush(mf->fp) != 0) {
//     perror("resize");
//     exit(1);
//   }

//   int fd = fileno(mf->fp); /* only place that needs an fd */

//   mf->map = mmap(NULL, new_capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
//   if (mf->map == MAP_FAILED) {
//     perror("mmap");
//     exit(1);
//   }
// }

static uint64_t create_inode(sfs_object *obj) { obj->o_super_block; }

int sfs_create() {}

int sfs_open(sfs_object *obj, sfs_str path) {
  int error = SFS_NOT_OK;

  sfs_header header = EMPTY_HEADER;
  sfs_super_block super_block = EMPTY_SUPER_BLOCK;

  sfs_fs_file fs_file;
  error = sfs_fs_file_open(&fs_file, path);
  if (error != SFS_OK) { return error; }

  if (fs_file.mf_size < (sizeof(sfs_header) + sizeof(sfs_super_block))) {
    // this file is corrupted or empty

    // write header
    header = DEFAULT_HEADER;

    if (fwrite(&header, 1, sizeof(sfs_header), f) != sizeof(sfs_header)) {
      error = SFS_CANT_WRITE;
      goto cleanup;
    }

    // write super block
    super_block = DEFAULT_SUPER_BLOCK;
    if (fwrite(&super_block, 1, sizeof(sfs_super_block), f) != sizeof(sfs_super_block)) {
      error = SFS_CANT_WRITE_SUPER_BLOCK;
      goto cleanup;
    }

    // write ...

    // write ...
  }

  if ((f = fopen(path.ptr, "rb+")) == NULL) {
    // try creating the file

    if ((f = fopen(path.ptr, "wb+")) == NULL) {
      error = SFS_FILE_CANT_CREATE;
      goto done;
    }

    // write header
    header = DEFAULT_HEADER;

    if (fwrite(&header, 1, sizeof(sfs_header), f) != sizeof(sfs_header)) {
      error = SFS_CANT_WRITE;
      goto cleanup;
    }

    // write super block
    super_block = DEFAULT_SUPER_BLOCK;
    if (fwrite(&super_block, 1, sizeof(sfs_super_block), f) != sizeof(sfs_super_block)) {
      error = SFS_CANT_WRITE_SUPER_BLOCK;
      goto cleanup;
    }

    // write ...

    // write ...
  } else {
    rewind(f);

    // read header
    if (fread(&header, 1L, sizeof(sfs_header), f) != sizeof(sfs_header)) {
      error = SFS_CANT_READ_HEADER;
      goto cleanup;
    }

    // validate header, maybe check version, don't know for now! TODO
    if (header.h_format_magic_number != DEFAULT_HEADER.h_format_magic_number ||
        header.h_block_size != DEFAULT_HEADER.h_block_size ||
        header.h_filename_limit != DEFAULT_HEADER.h_filename_limit) {
      error = SFS_CORRUPTED_HEADER;
      goto cleanup;
    }

    // read superblock
    if (fread(&super_block, 1L, sizeof(sfs_super_block), f) != sizeof(sfs_super_block)) {
      error = SFS_CANT_READ_SUPER_BLOCK;
      goto cleanup;
    }

    // validate super_block, don't know for now! HARD TODO
    if (0) {
      error = SFS_CORRUPTED_SUPER_BLOCK;
      goto cleanup;
    }

    // read inode btree TODO

    // read ...

    // read ...
  }

  // fill obj
  // obj->o_file.mf_fp = f;
  // obj->o_file.mf_size = ftello(f);
  // HARD TODO round up
  // obj->o_file.mf_capacity = lround_up(obj->o_file.mf_size, header.h_block_size);
  // obj->o_file.capacity = obj->o_file.size >= header.h_block_size ? obj->o_file.size : header.h_block_size;
  // obj->o_file.mf_map = memory_map_object_file(obj->o_file.mf_fp, obj->o_file.mf_capacity);
  // HARD TODO
  // obj->o_file.capacity = 0;
  // obj->o_file.memory = 0;

  obj->o_header = header;
  obj->o_super_block = super_block;

  create_inode_block(obj);

  error = SFS_OK;

cleanup:
  if (f != NULL) { fclose(f); }

done:
  return error;
}