#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
	size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FBCTL, FD_FB}; 

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

extern size_t serial_write(const void *buf, size_t offset, size_t len); 
extern size_t events_read(void *buf, size_t offset, size_t len);
extern size_t dispinfo_read(void *buf, size_t offset, size_t len);
extern size_t fb_write(const void *buf, size_t offset, size_t len);

extern size_t ramdisk_read(void *buf, size_t offset, size_t len);
extern size_t ramdisk_write(const void *buf, size_t offset, size_t len);
extern size_t get_ramdisk_size();

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, serial_write},
	[FD_EVENT] = {"/dev/events", 0, 0, events_read, invalid_write},
	[FD_FBCTL] = {"/proc/dispinfo", 0, 0, dispinfo_read, invalid_write},
	[FD_FB] = {"/dev/fb", 0, 0, invalid_read, fb_write}, 
#include "files.h"
};

void init_fs() {
  // initialize the size of /dev/fb
	char info[128];
  fs_read(FD_FBCTL, info, sizeof(info) - 1);	
	int value_start = 0, width = 0, height = 0;
	for (int i = 0;  i < 128 && info[i] != '\0'; ++i) {
		if (info[i] == ':') value_start = i + 2;
		else if (info[i] == '\n') {
			info[i] = '\0';
			width = atoi(&info[value_start]);
		}
	}
	assert(value_start != 0);
	height = atoi(&info[value_start]);
  
  file_table[FD_FB].size = width * height * sizeof(uint32_t);
}

int fs_open(const char *pathname, int flags, int mode) {
	size_t file_num = sizeof(file_table) / sizeof(Finfo);
	int i;
	for (i = 0; i < file_num && strcmp(file_table[i].name, pathname) != 0; ++i); 

	assert(i < file_num);	

	file_table[i].open_offset = 0;
	if (!file_table[i].read)
		file_table[i].read = ramdisk_read;
	if (!file_table[i].write)
		file_table[i].write= ramdisk_write;
	return i;
}

#define FS_RW(FUNC) \
	if (file_table[fd].size == 0 && file_table[fd].disk_offset == 0) \
		return file_table[fd].FUNC(buf, 0, len);\
	\
	int gap = file_table[fd].size - file_table[fd].open_offset; \
	assert(gap >= 0); \
	if (gap == 0) return 0; \
	if (gap < len) len = gap; \
	\
	size_t affected = file_table[fd].FUNC(buf, file_table[fd].disk_offset + file_table[fd].open_offset, len); \
	assert(affected == len); \
	file_table[fd].open_offset += affected; \
	return affected; 
	
size_t fs_read(int fd, void *buf, size_t len) {
	FS_RW  (read)
}

size_t fs_write(int fd, const void *buf, size_t len) {
	FS_RW  (write)
}

int fs_lseek(int fd, size_t offset, int whence) {
	size_t new_offset = file_table[fd].open_offset;
	switch (whence) {
		case SEEK_SET: new_offset = offset; break;
		case SEEK_CUR: new_offset += offset; break;
		case SEEK_END: new_offset = file_table[fd].size; break;
		default:
			return -1;
	}

	if (new_offset > file_table[fd].size)
		return -1;

	file_table[fd].open_offset = new_offset;
	return new_offset;
}

int fs_close(int fd) {
	file_table[fd].open_offset = 0;
	return 0;
}
