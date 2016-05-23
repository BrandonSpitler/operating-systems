/*
  FUSE: Filesystem in Userspace
  Copyright (C) 2001-2007  Miklos Szeredi <miklos@szeredi.hu>

  Minor modifications and note by Andy Sayler (2012) <www.andysayler.com>

  Source: fuse-2.8.7.tar.gz examples directory
  http://sourceforge.net/projects/fuse/files/fuse-2.X/

  This program can be distributed under the terms of the GNU GPL.
  See the file COPYING.

  gcc -Wall `pkg-config fuse --cflags` fusexmp.c -o fusexmp `pkg-config fuse --libs`

  Note: This implementation is largely stateless and does not maintain
        open file handels between open and release calls (fi->fh).
        Instead, files are opened and closed as necessary inside read(), write(),
        etc calls. As such, the functions that rely on maintaining file handles are
        not implmented (fgetattr(), etc). Those seeking a more efficient and
        more complete implementation may wish to add fi->fh support to minimize
        open() and close() calls and support fh dependent functions.


used:

https://github.com/connorboyd/CSCI3753-PA4/blob/7f4073dc2bb4a7ad189bce2da32c7a274a6f02ae/pa4-encfs.c

https://github.com/Poohblah/csci3753-pa4/blob/eb976f6a72f5600e585a4002087d7c1fac3e4bda/pa4-encfs.c



*/

#define FUSE_USE_VERSION 28
#define HAVE_SETXATTR

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <sys/types.h>

#ifdef linux
/* For pread()/pwrite() */
#define _XOPEN_SOURCE 500
#endif
//#include <linux/xattr.h>
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

#include <limits.h> //holds limit of PATH_MAX

#define NAME "nameOfAttribute"
//#ifdef HAVE_SETXATTR
#include <sys/xattr.h>

#include "aes-crypt.h"

//#endif
#define ENC 1
#define DEC 0

static char *key="test1";



struct encf{
	char* startDir;
	//add more to later 	
};


//char *startDir;



static struct encf home ={NULL};


static int isEncrypted(const char *npath){

	
	ssize_t size;
	size=getxattr(npath,NAME,NULL, 0);

	/*if(0>size){//on error
		return DEC;
	}*/

	char value[size]; //declare buffer of size size
	size=getxattr(npath,NAME,value, size);
	//tmpval[valsize] = '\0'; may need to add this later where tmpval is avlue and val size is size

//	char value[4]; //declare buffer of size size
//	size=getxattr(npath,NAME,value, 4);
	if(size<0){
		return DEC;
		fprintf(stderr, "size of attr is less then 0 error\n");
	}
	

	if(!strcmp(value,"true")){
		return ENC;//return of value is true shwoing that it is encrypted
	}else{
		return DEC;//returns value of false showing that it isnt encrpyted
	}
}


static void stopPath(char nfullPath[PATH_MAX],const char * path){
	fprintf(stderr, "in stop path\n");
	strcpy(nfullPath,home.startDir);//copies string at pointer ENFS_data->root directory into fpath
	strncat(nfullPath,path,PATH_MAX);//appends the current path to the root dictory if its to long then it use at most max path number of bytes
	fprintf(stderr, "done stop path\n");

}


static int xmp_getattr(const char *path, struct stat *stbuf)
{
	struct stat *tmpFileStats;
	tmpFileStats=(struct stat *) malloc(sizeof(struct stat));	
	fprintf(stderr, "in getattr\n");
	FILE* posEnc;
	FILE* decrypedFile;
	int action;
	ssize_t size;
	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);
	fprintf(stderr, "full path %s\n",fullPath);
	res = lstat(fullPath, stbuf);



	//for now do with a tmparary file there has got to be a better way
	char tPath[PATH_MAX];
	stopPath(tPath,"/tFile");
	

	
	if (res == -1){
		fprintf(stderr, "leaving getattr with error\n");
		return -errno;

	}

	if(S_ISREG(stbuf-> st_mode)){
		size=getxattr(fullPath, NAME,NULL,0);
		if(size<0){
			fprintf(stderr,"doesnt have attibuted\n");
			return 0;
		}
		char att[size+1];//might have to malloc
		size=getxattr(fullPath,NAME,att,size);
		att[size]='\0'; //null bit
		if(!strcmp(att,"true")){
			action=1;

		}else{
			action=-1;
		}
		posEnc=open(fullPath,"r");
		decrypedFile=open(tPath,"w");
		if(!do_crypt(posEnc,decrypedFile,action,key)){
			fprintf(stderr,"do crypt failed in getattr");
		}
		if(1==action){
			int tmpres=lstat(tPath,tmpFileStats);
			if(tmpres==-1){
				fprintf(stderr,"tmppres in getattr was -1\n");
				return -errno;
			}
			stbuf->st_size=tmpFileStats->st_size;
			stbuf->st_blocks=tmpFileStats->st_blocks;
			stbuf->st_blksize=tmpFileStats->st_blksize;
			
		}
		remove(decrypedFile);

		


	}
	free(tmpFileStats);//do i need this?
	fprintf(stderr, "leaving getattr\n");
	
	return res;
}

static int xmp_access(const char *path, int mask)
{
	int res;
	fprintf(stderr, "in access\n");
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);
	res = access(fullPath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	fprintf(stderr, "in get readlink\n");
	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = readlink(fullPath, buf, size - 1);//place fullPath into buf and gets rid of the null character
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}


static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
	fprintf(stderr, "in readdir\n");
	DIR *dp;
	struct dirent *de;

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	(void) offset;
	(void) fi;

	dp = opendir(fullPath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if (filler(buf, de->d_name, &st, 0))
			break;
	}

	closedir(dp);
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	fprintf(stderr, "in mknod\n");
	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(path, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fullPath, mode);
	else
		res = mknod(fullPath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode)
{
	fprintf(stderr, "in mkdir\n");
	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = mkdir(fullPath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	int res;

	res = unlink(fullPath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	fprintf(stderr, "in rmdir\n");

	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = rmdir(fullPath);
	if (res == -1)
		return -errno;

	return 0;
}




static int xmp_symlink(const char *from, const char *to)
{
	fprintf(stderr, "in symlink\n");
	int res;


	res = symlink(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to)
{
	fprintf(stderr, "in rename\n");
	int res;


	res = rename(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	fprintf(stderr, "in link\n");
	int res;



	res = link(from, to);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	fprintf(stderr, "in chmod\n");
	int res;

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = chmod(fullPath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	fprintf(stderr, "in get chown\n");
	int res;

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = lchown(fullPath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	fprintf(stderr, "in truncate\n");
	int res;

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = truncate(fullPath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	fprintf(stderr, "in utimes\n");
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);
	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(fullPath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	fprintf(stderr, "in get open\n");
	int res;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	res = open(fullPath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
	fprintf(stderr, "In read\n");
	int fd;
	int res;
	int action;
	FILE* inFile;
	char* outFileStart;
	size_t outFileSize;
	FILE* outFile;




	//findif better way to do this
	char tmpPath[PATH_MAX];
	stopPath(tmpPath,"/tmpreadfile.txt");
	FILE *fileT;
	

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);

	(void) fi;
	
	if(isEncrypted(fullPath)){
		action=DEC;
	}else{
		action=-1;
	}

	inFile = fopen(fullPath, "r");
	fileT=fopen(tmpPath,"w");
	//outFile=open_memstream(&outFileStart,&outFileSize);//opens stream to be written t
	fprintf(stderr, "do_crypts is happening\n\n\n\n");
	if(!do_crypt(inFile,fileT,action,key)){
		fprintf(stderr, "do_cryptfailrd\n\n\n\n");
		
	}//come back to

	fclose(inFile);
	fclose(fileT);

	fd=open(tmpPath,O_RDONLY);
	//if (fd == -1)
		//return -errno;
	//fseek(outFile,offset,SEEK_SET);
	res = pread(fd,buf,size,offset);
	if (res == -1)
		res = -errno;
	close(fd);
	remove(tmpPath);
	return res;//return number of bytes written
}

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{
	fprintf(stderr, "in Write\n");
	int fd;
	int res;
	size_t inFileSize;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  
 	int action;
	int revAction;
	char* inFileStart;

	FILE* inFile;
	FILE* outFile;

	(void) fi;

	outFile=fopen(fullPath,"r");
	
	//fd = open(path, O_WRONLY);
	inFile=open_memstream(&inFileStart,&inFileSize);
	//fwrite(buf,size,sizeof(char),inFile);//write to the infile	
	outFile= fopen(fullPath,"r");

	//res = pwrite(fd, buf, size, offset);
	if(isEncrypted(fullPath)){

		action=DEC;
		revAction=ENC;
		//do_crypt(outFile,inFile,0,key);//come back to latter

		//fd=open(fullPath,O_WRONLY);
		
	}else{
		action =-1;
		revAction=-1;
		//fd=open(fullPath, O_WRONLY);

	}
	fprintf(stderr, "do_crypts is happening\n\n\n\n");
	do_crypt(outFile,inFile,action,key);
	fseek(inFile,offset,SEEK_SET);
	//res=pwrite(fd,buf,size,offset);
	res=fwrite(buf,1,size,inFile);
	close(outFile);

	if (fd == -1)
		return -errno;


	if (res == -1)
		res = -errno;

	fflush(inFile);

	outFile=fopen(fullPath,"w");
	fseek(inFile,0,SEEK_SET);
	fprintf(stderr, "do_crypts is happening\n\n\n\n");
	do_crypt(inFile,outFile,revAction, key);

	fclose(outFile);
	fclose(inFile);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  
	int res;

	res = statvfs(fullPath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {
	fprintf(stderr, "in create\n");
	FILE *newFile;
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);
	FILE* outFile;
	char *outFileText;
	size_t outFileSize;
    (void) fi;

    int res;
	//outFile=open_memstream(&outFileText,&outFileSize);
	newFile=fopen(fullPath,"w");

	fprintf(stderr, "do_crypts is happening\n\n\n\n");
	if(!do_crypt(newFile, newFile, ENC,key)){
		fprintf(stderr, "do_cryptfailedd\n\n\n\n");
	}


    if(res == -1){
	return -errno;
	}
	setxattr(fullPath,NAME,"true",4,0);
	fprintf(stderr, "leaving create\n");
    close(newFile);

	   //sets attributeds cnositnet name is encrypted failes if the attribute alreads excists could be XATTR_REPLACE should really matter excpeted for maybe deaper security reasons

    return 0;
}


static int xmp_release(const char *path, struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) fi;
	return 0;
}

static int xmp_fsync(const char *path, int isdatasync,
		     struct fuse_file_info *fi)
{
	/* Just a stub.	 This method is optional and can safely be left
	   unimplemented */

	(void) path;
	(void) isdatasync;
	(void) fi;
	return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  

	int res = lsetxattr(fullPath, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
			size_t size)
{


	fprintf(stderr, "in get xattr\n");
	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  
	int res = lgetxattr(fullPath, name, value, size);
	if (res == -1)
		return -errno;

	fprintf(stderr, "leaving get xattr\n");
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  
	int res = llistxattr(fullPath, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{

	char fullPath[PATH_MAX];
	stopPath(fullPath, path);  
	int res = lremovexattr(fullPath, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create         = xmp_create,
	.release	= xmp_release,
	.fsync		= xmp_fsync,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
};

int main(int argc, char *argv[])

{
 	 fprintf(stderr, "in main");
	umask(0);
	home.startDir=realpath(argv[argc-2],NULL);

	key=argv[argc-3];
	argv[argc-3]=argv[argc-1];
	argv[argc-2]=NULL;
	//char *nstartDir=realpath(,NULL);
	argv[argc-1]=NULL;
	argc=argc-2;


	
	return fuse_main(argc, argv, &xmp_oper, NULL);
}
