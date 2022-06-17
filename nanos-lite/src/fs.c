#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;//disk_offset=last diskoffset + last size
  size_t open_offset;//operating site
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
#define VFS_NUM 7
static Finfo file_table[] __attribute__((used)) = { //new 0 for open_offset
  [FD_STDIN]  = {"stdin", 0, 0,0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0,0, invalid_read, serial_write},//invalid_write to serial_write
  [FD_STDERR] = {"stderr", 0, 0,0, invalid_read, serial_write},//invalid_write to serial_write
  [3]={"/dev/fb",0,0,0,invalid_read,fb_write},
  [4]={"/proc/dispinfo",128,0,0,dispinfo_read,invalid_write},
  [5]={"/dev/events",0,0,0,events_read,invalid_write},
  [6]={"/dev/tty",0,0,0,invalid_read,serial_write},
#include "files.h"
};

void init_fs() {
  // TODO: initialize the size of /dev/fb
  int width=io_read(AM_GPU_CONFIG).width;
  int height=io_read(AM_GPU_CONFIG).height;

  file_table[3].size=width*height*4;
  Log("init_fs_fbsize:%d\n",file_table[3].size);
}




#define NR_FILES (sizeof(file_table)/sizeof(file_table[0]))

int fs_open(const char *pathname,int flags,int mode){

    int fd=-1;
	for(int i=0;i<NR_FILES;++i){
        if(strcmp(file_table[i].name,pathname)==0){fd=i;break;}
	}

	if(fd==-1) {
			printf("file %s does not exist\n",pathname);
			return -1;
	}
	else{
        file_table[fd].open_offset=0;
		Log("File %s fd=%d loaded",pathname,fd);
		return fd;
	}
}

int fs_close(int fd){
    file_table[fd].open_offset=0;
	return 0;
}


extern size_t ramdisk_write(const void *buf,size_t offset,size_t len);
extern size_t ramdisk_read(const void* buf,size_t offset,size_t len);


size_t fs_read(int fd,void* buf,size_t len){
	//file to buf;
	//if file is ramdisk,then ramdisk to buf
//	printf("in fs_read\n");
       size_t inclen=0;
	   if(file_table[fd].read){
         // 		   printf("in fs_read1\n");
           inclen=file_table[fd].read(buf,file_table[fd].open_offset,len);
	   }
	   else{
		 //  printf("in fs_read2 offset:%d len:%d\n",file_table[fd].disk_offset+file_table[fd].open_offset,len);
           size_t left=file_table[fd].size-file_table[fd].open_offset;
		   inclen=(len<left)?len:left;
		   ramdisk_read(buf,file_table[fd].disk_offset+file_table[fd].open_offset,inclen);
	   }
	   file_table[fd].open_offset +=inclen;
	   return inclen;


}


size_t fs_write(int fd,const void *buf,size_t len){
	//buf to file;
	//if file is ramdisk,then buf to ramdisk
//	printf("in fs_write\n");
     if(file_table[fd].write!=NULL){
         size_t real_len=file_table[fd].write(buf,file_table[fd].open_offset,len);
		 file_table[fd].open_offset +=real_len;
		 return real_len;
	 }
	 else {
          size_t left=file_table[fd].size - file_table[fd].open_offset;
		  if(left<len) panic("Out of Bounadry");
		  ramdisk_write(buf,file_table[fd].disk_offset+file_table[fd].open_offset,len);
		  file_table[fd].open_offset+=len;
		  return len;
	 }
}

int fs_lseek(int fd,int offset,int whence){
//	printf("in fs_lseek\n");
     int start=0;
	 switch(whence){
           case SEEK_SET:start=file_table[fd].disk_offset;break;
		   case SEEK_CUR:start=file_table[fd].disk_offset+file_table[fd].open_offset;break;
		   case SEEK_END:start=file_table[fd].disk_offset+file_table[fd].size;break;
	 }
	 int pos=start+offset;

     if(fd<VFS_NUM||(file_table[fd].disk_offset<=pos&&pos<=file_table[fd].disk_offset+file_table[fd].size)){
         file_table[fd].open_offset=pos-file_table[fd].disk_offset;
	 }
	 else{
         panic("End of File is %d,pointer locates %d,Out of file Bound!",file_table[fd].disk_offset+file_table[fd].size,pos);
		 return -1;
	 }
	 return file_table[fd].open_offset;


}

size_t fs_filesz(int fd){return file_table[fd].size;}
