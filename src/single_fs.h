#ifndef SINGLE_FS
#define SINGLE_FS

#include <stddef.h>
#include <stdint.h>

/* Utils*/

typedef struct sfs_str {
  /* null terminated string */
  char *ptr;

  /* str len excluding the null terminator*/
  size_t len;

  /* memory size including the null terminator */
  /* if len == size, then no null terminator at the end, e.g. slices */
  size_t size;
} sfs_str;

#define SFS_STR(str)                                                                                                   \
  ((sfs_str){                                                                                                          \
      .ptr = str,                                                                                                      \
      .len = sizeof(str) - 1,                                                                                          \
      .size = sizeof(str),                                                                                             \
  })

/* 8 bytes total */
typedef struct sfs_extent_header {
  /* tree depth */
  /* 0 depth means leaf node */
  uint16_t eh_depth;

  /* number of extents in this tree*/
  uint16_t eh_entries;

  /* maximum capacity available without new allocation */
  uint16_t eh_capacity;

  /* reserved to fill 8 bytes */
  uint16_t eh_reserved0;
} sfs_extent_header;

/* 8 bytes total */
typedef struct sfs_extent {
  /* logical block address */
  /* start of the extent */
  /* first bit is free/occupied */
  /* 31 bit -> 8 tb max file size */
  uint32_t e_start;

  /* logical block address */
  /* end of the extent */
  /* I could use `len` field but that would make no difference */
  /*   since this struct will be packed into 8 bytes anyway */
  uint16_t e_end;
} sfs_extent;

/* 8 bytes total */
typedef struct sfs_extent_idx {
  /* logical block address start block */
  uint32_t ei_start;

  /* reserved to fill 8 bytes */
  uint32_t ei_reserved0;
} sfs_extent_idx;

#define EXT4_N_BLOCKS sizeof(sfs_extent) * 5

typedef struct sfs_inode_block_header {
  /* number of inodes in this block */
  /* considering block size is 4096, inode size must be >= 64 */
  uint32_t ibh_occupied_inode_bit_lo;
  uint32_t ibh_occupied_inode_bit_hi;

  /* number of inodes in the block*/
  uint16_t ibh_len;

  /* reserved to fill 16 bytes*/
  uint16_t ibh_reserved0;
  uint16_t ibh_reserved1;
} sfs_inode_block_header;

typedef struct sfs_inode {
  /* inode flags */
  union {
    uint32_t flag;
    struct {
      uint8_t type;
      uint8_t reserved0;
      uint8_t reserved1;
      uint8_t reserved2;
    };
  } i_flags;

  /* size in bytes, low and high */
  /* max file size is 8tb */
  uint32_t i_size_lo;
  uint32_t i_size_hi;

  /* link count */

  /* header and 2 cases: */
  /*   1. 4 extent structs */
  /*   2. 4 extent index pointers */
  uint8_t i_blocks[EXT4_N_BLOCKS]; /* Pointers to blocks */

  uint16_t i_links_count; /* Links count */

  /* total block count in this inode */
  uint32_t i_total_blocks; /* Blocks count */

  /* creation time, low and high */
  uint32_t i_created_at_lo;
  uint32_t i_created_at_hi;

  /* modification time, low and high */
  uint32_t i_modified_at_lo;
  uint32_t i_modified_at_hi;

  /* crc32 inode */
  uint32_t i_checksum;
} sfs_inode;

typedef struct sfs_super_block {
  /* First inode Block */
  uint32_t s_root_inode_block;

  /* Inodes count */
  uint32_t s_inodes_count;

  /* Free inodes count */
  uint32_t s_free_inodes_count;

  /* Blocks count */
  uint32_t s_blocks_count;

  /* Reserved blocks count */
  uint32_t s_r_blocks_count;

  /* Free blocks count */
  uint32_t s_free_blocks_count;
} sfs_super_block;

typedef struct sfs_header {
  /* made up format magic number */
  uint32_t h_format_magic_number;

  /* version number, low and high */
  uint16_t h_version_number_major;
  uint16_t h_version_number_minor;

  /* block size, probably 4096 */
  uint16_t h_block_size;

  /* filename limit, probably 255 */
  uint16_t h_filename_limit;
} sfs_header;

#define HEADER_FORMAT_MAGIC_NUMBER 0x5346594B
#define HEADER_VERSION_NUMBER_MAJOR 0x0001
#define HEADER_VERSION_NUMBER_MINOR 0x0000
#define HEADER_BLOCK_SIZE 4096
#define HEADER_FILENAME_LIMIT 255

typedef uint64_t sfs_inode_id;

typedef struct sfs_block {

} sfs_block;

enum SFS_INODES {
  NULL_INODE,
  FREE_INODE,
  ROOT_INODE,
  RESERVED_INODE0,
  RESERVED_INODE1,
  RESERVED_INODE2,
  RESERVED_INODE3,
  RESERVED_INODE4,
  RESERVED_INODE5,
  RESERVED_INODE6,
  RESERVED_INODE7,
  REGULAR_INODES
};

#define IS_ROOT_INODE(inode) ((inode) == ROOT_INODE)
#define IS_REGULAR_INODE(inode) ((inode) >= REGULAR_INODES)

enum SFS_INODE_ENTRY_TYPE {
  SFS_FREE,
  SFS_DIRECTORY,
  SFS_FILE,
};

typedef struct sfs_stats {

} sfs_stats;

typedef struct sfs_fs_file {
  /* file pointer */
  int mf_fd;

  /* file size */
  uint64_t mf_size;

  /* available capacity */
  /* hoping this will always be same to the size */
  /* maybe I'm wrong */
  uint64_t mf_capacity;

  /* write buffer */
  struct {
    /* buffer pointer, most probably allocated */
    uint8_t *ptr;

    /* current buffer cursor */
    uint64_t len;

    /* total capacity of the buffer, usually 8192 */
    uint64_t capacity;
  } mf_buffer;
} sfs_fs_file;

typedef struct sfs_object {
  sfs_fs_file o_file;

  sfs_header o_header;
  sfs_super_block o_super_block;
} sfs_object;

typedef struct sfs_file {
} sfs_file;

// ERROR CODES

#define SFS_GENERATE_ENUM(ENUM) ENUM,
#define SFS_GENERATE_STRING(STRING) #STRING,
#define SFS_FOR_EACH_ERROR_CODE(SFS_ERROR_CODE)                                                                        \
  SFS_ERROR_CODE(SFS_NOT_OK)                                                                                           \
  SFS_ERROR_CODE(SFS_OK)                                                                                               \
  SFS_ERROR_CODE(SFS_CANT_OPEN)                                                                                        \
  SFS_ERROR_CODE(SFS_CANT_SEEK)                                                                                        \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_FILE_CANT_CREATE)                                                                                 \
  SFS_ERROR_CODE(SFS_FILE_CANT_STAT)                                                                                   \
  SFS_ERROR_CODE(SFS_FILE_CANT_ALLOCATE_BUFFER)                                                                        \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_CANT_WRITE)                                                                                       \
  SFS_ERROR_CODE(SFS_CANT_WRITE_HEADER)                                                                                \
  SFS_ERROR_CODE(SFS_CANT_WRITE_SUPER_BLOCK)                                                                           \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_CANT_READ)                                                                                        \
  SFS_ERROR_CODE(SFS_CANT_READ_HEADER)                                                                                 \
  SFS_ERROR_CODE(SFS_CANT_READ_SUPER_BLOCK)                                                                            \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_CORRUPTED)                                                                                        \
  SFS_ERROR_CODE(SFS_CORRUPTED_HEADER)                                                                                 \
  SFS_ERROR_CODE(SFS_CORRUPTED_SUPER_BLOCK)                                                                            \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_INODE_NOT_EXIST)                                                                                  \
  SFS_ERROR_CODE(SFS_INODE_CANT_CREATE)                                                                                \
  SFS_ERROR_CODE(SFS_INODE_ALREADY_EXISTS)                                                                             \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_EXPECTED_DIR_FOUND_FILE)                                                                          \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_BAD_ARGUMENT)                                                                                     \
                                                                                                                       \
  SFS_ERROR_CODE(SFS_ERROR_COUNT)

enum SFS_ERROR_CODE { SFS_FOR_EACH_ERROR_CODE(SFS_GENERATE_ENUM) };
static const char *SFS_ERROR_CODE_STRING[] = {SFS_FOR_EACH_ERROR_CODE(SFS_GENERATE_STRING)};

// API

// FS FILE API
int sfs_fs_file_open(sfs_fs_file *mf, sfs_str path);
int sfs_fs_write_buffered(sfs_fs_file *mf, void *ptr, size_t size, size_t n);

// OBJECT API
int sfs_open(sfs_object *obj, sfs_str path);
int sfs_close(sfs_object *obj);

int sfs_compact(sfs_object *obj);
sfs_stats sfs_get_stats(sfs_object *obj);

// DIRECTORY API
int sfs_mkdir(sfs_object *obj, sfs_str path, int recursive);
int sfs_rmdir(sfs_object *obj, const char *path);
int sfs_rmdir_inode(sfs_object *obj, sfs_inode_id inode);
// add read dir, step dir TODO

// FILE API
int sfs_fopen(sfs_object *obj, const char *path, sfs_file *file_out);
int sfs_fclose(sfs_object *obj, sfs_file *file_in);
int sfs_fwrite(sfs_file *file, const void *ptr, uint64_t size, uint64_t count);
// change or fix them TODO
int sfs_fseek(sfs_file *file, long offset, int whence);
long sfs_ftell(sfs_file *file);
int sfs_unlink(sfs_object *fs, const char *path);
int sfs_link(sfs_object *fs, const char *oldpath, const char *newpath); // hard link

#endif