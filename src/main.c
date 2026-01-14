#include "single_fs.h"
#include <assert.h>

int main() {

  sfs_object obj;
  // assert(sfs_open(&obj, SFS_STR("test.kyfs")) == SFS_OK);

  sfs_mkdir(&obj, SFS_STR("root//path/test/files/"), 1);

  return 0;
}