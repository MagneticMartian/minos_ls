#include "include/apue.h"
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

enum {
	DT_UNKNOWN = 0,
	DT_FIFO    = 1,
	DT_CHR     = 2,
	DT_DIR     = 4,
	DT_BLK 	   = 6,
	DT_REG     = 8,
	DT_LNK     = 10,
	DT_SOK     = 12,
	DT_WHT     = 14
};

char* result(unsigned val)
{
	char* perms;
	switch (val){
		case 0:
			perms = "---"; break;
		case 1:
			perms = "--x"; break;
		case 2:
			perms = "-w-"; break;
		case 3:
			perms = "-wx"; break;
		case 4:
			perms = "r--"; break;
		case 5:
			perms = "r-x"; break;
		case 6:
			perms = "rw-"; break;
		case 7:
			perms = "rwx"; break;
	}
	return perms;
}

char* concat(char *first, char *second)
{
	int i = 0, j = 0;
	char *concatination;

	while(first[i] != '\0'){
		concatination[i] = first[i];
		++i;
	}

	while(second[j] != '\0'){
		concatination[i] = second[j];
		++i;
		++j;
	}
	concatination[i] ='\0';

	return concatination;
}

void list(DIR *dp, char* pwd)
{
	struct dirent *dirp;
	struct stat sb;
	unsigned num_dir = 0, num_reg = 0;

	while((dirp = readdir(dp)) != NULL){
		unsigned long inode = dirp->d_ino; /* INODE of file/directory */
		unsigned char type = dirp->d_type; /* file type */
		char *name = dirp->d_name; /* file/directory name */
		char *init_path = concat(pwd,"/");
		char *path = concat(init_path,name);

		if (stat(path, &sb) == -1)
		{
			err_quit("unable to open");
		}

		if (*name == '.'){ /* ignore hidden files/directories */
			continue;
		} else {
			char ch;
			switch (type){
				case DT_UNKNOWN:
					ch = 'u'; break;
				case DT_FIFO:
					ch = 'f'; break;
				case DT_CHR:
					ch = 'c'; break;
				case DT_DIR:
					ch = 'd'; 
					num_dir += 1; break;
				case DT_BLK:
					ch = 'b'; break;
				case DT_REG:
					ch = '-'; 
					num_reg += 1; break;
				case DT_LNK:
					ch = 'l'; break;
				case DT_SOK:
					ch = 's'; break;
				case DT_WHT:
					ch = 'w'; break;
			}

			long perms = (long)sb.st_mode;
			unsigned own_perm = (perms & 0700) >> 6;
			char* own = result(own_perm);
			unsigned grp_perm = (perms & 070) >> 3;
			char* grp = result(grp_perm);
			unsigned usr_perm = perms & 07;
			char* usr = result(usr_perm);
                        if(ch == 'l'){
				char buf[1024];
				ssize_t len = readlink(path, buf, sizeof(buf));
				buf[len]='\0';
				printf("%ld %c%s%s%s %10ld %s -> %s\n", inode, ch, own, grp, usr, (long)sb.st_size, name, buf);
			} else {
				printf("%ld %c%s%s%s %10ld %s\n", inode, ch, own, grp, usr, (long)sb.st_size, name);
			}
		}
	}
}

int main (int argc, char* argv[])
{
	DIR *dp;

	if(argc !=2) 
		err_quit("usage: ls directory_name");

	if((dp = opendir(argv[1])) == NULL) 
		err_sys("can't open %s", argv[1]);

	list(dp, argv[1]);
	closedir(dp);
}
