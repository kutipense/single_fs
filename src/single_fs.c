
#define _GNU_SOURCE // Hard TODO

#include "single_fs.h"
#include <stdio.h>
#include <string.h>

static const sfs_header EMPTY_HEADER = (sfs_header){0};
static const sfs_super_block EMPTY_SUPER_BLOCK = (sfs_super_block){0};

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

  uint8_t *block_ptr = calloc_block(obj);

  memcpy(block_ptr, &header, sizeof(sfs_inode_block_header));

  // for (int i = sizeof(sfs_inode_block_header); i < block_size; i += sizeof(sfs_inode)) {
  //   sfs_inode *inode_ptr = (sfs_inode *)(block_ptr + i);
  //   inode_ptr = (sfs_inode) {

  //   }
  // }

  return NULL;
}

static uint64_t create_inode(sfs_object *obj) { obj->o_super_block; }

int sfs_create() {}

int sfs_open(sfs_object *obj, const char *path) {
  int error = SFS_NOT_OK;

  sfs_header header = EMPTY_HEADER;
  sfs_super_block super_block = EMPTY_SUPER_BLOCK;

  FILE *f = NULL;
  if ((f = fopen(path, "rb+")) == NULL) {
    // try creating the file

    if ((f = fopen(path, "wb+")) == NULL) {
      error = SFS_CANT_CREATE;
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
  obj->o_header = header;
  obj->o_super_block = super_block;

  create_inode_block(obj);

  error = SFS_OK;

cleanup:
  if (f != NULL) {
    fclose(f);
  }

done:
  return error;
}