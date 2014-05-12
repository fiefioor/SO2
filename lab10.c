// SO2 IS1 211A LAB10
// Michal Warkoczynski
// mjwarkoczynski@wi.zut.edu.pl

#include <stdio.h>
#include <stdlib.h>
#include <ftw.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

display(const char *fpath, const struct stat *sb, int tflag)
{
	struct passwd *owner = getpwuid(sb->st_uid);
	struct group *ownersGroup = getgrgid(sb->st_gid);
	time_t _time = time(NULL);
	char time[13];
	strftime(time,13,"%b %d %H:%M",localtime(&(sb->st_mtime)));

	if(S_ISDIR (sb->st_mode)) {printf("d");}
	else {printf("-");}
	
	if(S_IRUSR &sb->st_mode) {printf("r");}
	else {printf("-");}
	
	if(S_IWUSR &sb->st_mode) printf("w");
	else printf("-");
	
	if(S_IXUSR &sb->st_mode) printf("x");
	else printf("-");
	
	if(S_IRGRP &sb->st_mode) {printf("r");}
	else printf("-");
	
	if(S_IWGRP &sb->st_mode) printf("w");
	else printf("-");
	
	if(S_IXGRP &sb->st_mode) printf("x");
	else printf("-");
	
	if(S_IROTH &sb->st_mode) printf("r");
	else printf("-");
	
	if(S_IWOTH &sb->st_mode) printf("w");
	else printf("-");
	
	if(S_IXOTH &sb->st_mode) printf("x");
	else printf("-");
	
	printf("\t");
	
	printf("%d\t",(int)sb->st_nlink);
	
	printf("%s\t",owner->pw_name);
	
	printf("%s\t",ownersGroup->gr_name);
	
	printf("%d\t",(int)sb->st_size);
	
	printf("%s\t",time);
	
	printf("%s",fpath);
	
	printf("\n");
	
	return(0);
};

int main(int argc, char** argv) {

  if(argc < 2){ftw(".",display,20);}
  else {ftw(argv[1],display,20);}

  return (0);

}
