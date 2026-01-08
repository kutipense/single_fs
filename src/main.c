#include "single_fs.h"
#include <assert.h>

int main() {

  sfs_object obj;
  assert(sfs_open(&obj, "test.kyfs") == SFS_OK);

  return 0;
}