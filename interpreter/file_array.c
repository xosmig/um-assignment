
#include <stdlib.h>
#include <stdio.h>

#include "file_array.h"

static array_base_t* file_array_clone(array_base_t* base, platter clone_id) {
  (void) base;
  (void) clone_id;
  fprintf(stderr, "Clonning of file_array is not supported\n");
  exit(63);
}

static struct array_base_vtable file_array_vtable = {
  .get = (typeof(file_array_vtable.get)) file_array_get,
  .set = (typeof(file_array_vtable.set)) file_array_set,
  .destroy = (typeof(file_array_vtable.destroy)) destroy_file_array,
  .clone = file_array_clone,
};

static void set_idx_or_fail(file_array_t* self, platter idx) {
  if (fseek(self->filp, idx * sizeof(platter), SEEK_SET) != 0) {
    fprintf(stderr, "Error accessing file\n");
    exit(6);
  }
}

file_array_t* create_file_array(const char* filename, platter id) {
  file_array_t* res = malloc(sizeof(file_array_t));
  *res = (file_array_t) {
    .base = {
      .vtable = &file_array_vtable,
      .id = id,
      .lnode = LIST_HEAD_INITIALIZER(res->base.lnode),
    },
    .filp = fopen(filename, "rwb"),
  };
  return res;
}

platter file_array_get(file_array_t* self, platter idx) {
  set_idx_or_fail(self, idx);

  unsigned char buf[sizeof(platter)];
  if (fread(buf, sizeof(buf), 1, self->filp) != 1) {
    fprintf(stderr, "Error reading from file\n");
    exit(7);
  }

  return (buf[0] << 24) + (buf[1] << 16) + (buf[2] << 8) + buf[3];
}

void file_array_set(file_array_t* self, platter idx, platter value) {
  set_idx_or_fail(self, idx);

  if (fwrite(&value, sizeof(value), 1, self->filp) != sizeof(value)) {
    fprintf(stderr, "Error writing to file\n");
    exit(9);
  }
}

void destroy_file_array(file_array_t* self) {
  if (fclose(self->filp)) {
    fprintf(stderr, "Error closing file");
    exit(15);
  }
  destroy_array_base((array_base_t*) self);
}
