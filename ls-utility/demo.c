/*
 *   usage: ./a.out dirpath
 */
#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>
#include <sys/sysmacros.h>
#include <sys/ioctl.h>
#include <time.h>

#define TEST printf("Test\n");

static bool L_FLAG = false, A_FLAG = false, I_FLAG = false, R_FLAG;
extern int errno;
static int no_of_options = 0;

char get_file_type(struct stat);
void get_permissions(struct stat, char *);
void get_user_name(int, char *);
void get_group_name(int, char *);
void set_file_size(struct dirent*, struct stat, char *);
void set_time(long, char *);
int set_opt_flags(int , char **);
void set_stat_info(struct stat *,char *);
void do_ls(char*);
void print_file_name(char* , char, int);

int flag = 0;
static char * dir_path;

int main(int argc, char* argv[]){
   int no_of_arg_without_options = argc - set_opt_flags(argc,argv);
   if (no_of_arg_without_options == 1){
	   do_ls(".");
   }

   else{
   	for(int i = 1; i < argc; i++){
         if(argv[i][0] == '-') { continue;}
         else
   		    printf("Directory listing of %s:\n", argv[i] );
   		do_ls(argv[i]);
   	}
   }
   return 0;
}


void do_ls(char * dir){
   dir_path = dir;
   if(R_FLAG)  printf("%s: \n",dir_path);
   struct dirent **namelist; 
   int n;

   n = scandir(dir_path, &namelist, NULL, alphasort);
   if (n == -1 && !R_FLAG) {
      perror("scandir");
      return;
      // exit(EXIT_FAILURE);
   }

   char path_name[PATH_MAX];
   struct stat stat_info[n];
   int total_blocks = 0;
   char** dir_names = (char**)malloc(n*sizeof(char));
   int r = 0;
   for (int i = 0; i < n; ++i){
      sprintf(path_name, "%s/%s", dir_path, namelist[i]->d_name);
    //   printf(path_name, "%s/%s", dir_path, namelist[i]->d_name);
      set_stat_info(&stat_info[i],path_name);
      total_blocks += stat_info[i].st_blocks;
   }
   printf("total %d\n", total_blocks/2);


   char file_type, permissions[10], user[32], group[32], size[50], time[17];

   int fileNameMaxSize = 0, offset;

   for (int i = 0; i < n; ++i){
      get_permissions(stat_info[i],permissions);
      get_user_name(stat_info[i].st_uid,user);
      file_type =  get_file_type(stat_info[i]);
      get_group_name(stat_info[i].st_gid,group);
      set_file_size(namelist[i],stat_info[i],size);
      set_time(stat_info[i].st_mtime,time);

      if(file_type == 'd' && strcmp(namelist[i]->d_name, ".") != 0 && strcmp(namelist[i]->d_name,"..") != 0 && namelist[i]->d_name[0] != '.'){
         dir_names[r++] = namelist[i]->d_name;
      }

      if(I_FLAG)  printf("%5ld ",stat_info[i].st_ino);
      if(!A_FLAG && namelist[i]->d_name[0] == '.') continue; 
      if(L_FLAG){
         printf("%c%s. %d %s %s %6s %s  ",file_type,
                                          permissions,
                                          stat_info[i].st_nlink,
                                          user,
                                          group,
                                          size, time);
         print_file_name(namelist[i]->d_name,get_file_type(stat_info[i]),fileNameMaxSize);
         printf("\n");
      }

      
      
      // printing
      if(namelist[i]->d_type == DT_LNK && L_FLAG){
         char filename[PATH_MAX];
         int n = readlink(namelist[i]->d_name, filename, PATH_MAX);
         filename[n] = '\0';
         printf(" -> %s", filename);
      }
      
   }
   if(!L_FLAG){
      // Setting screen size
      struct winsize wbuf;
      ioctl(0,TIOCGWINSZ,&wbuf);
      size_t cols = wbuf.ws_col;
      size_t rows = wbuf.ws_row;

      
      for (int i=0;i<n;i++){
         int temp = strlen (namelist[i]->d_name) ;
            if (temp > fileNameMaxSize)
               fileNameMaxSize = temp;
         }

      int noofCols = cols/(fileNameMaxSize+2) ;
      offset = (double)n/noofCols + 1;

      for(int i = 0; i< offset; i++){
         for(int j = i; j < n; j+=offset){
            if(namelist[j]->d_name[0] == '.'  && !A_FLAG) continue;
            print_file_name(namelist[j]->d_name, file_type, fileNameMaxSize+2);
         }
         printf("\n");
      }
      printf("\n");
   }
   if(R_FLAG){
      for (int i = 0; i < r; ++i){
         do_ls(dir_names[i]);
      }
    //  freeing the memory 
   }
   for (int i = 0; i < n; ++i){
      free(namelist[i]);
   }
   free(namelist);
}

void set_stat_info(struct stat * info,char * fname){
   int rv = lstat(fname, info);
   if (rv == -1){
      perror("stat failed");
      exit(1);
   }
}

int set_opt_flags(int argc, char **argv){
  int c, count = 0;
  opterr = 0;
  for (int i = 1; i < argc; ++i){
     if(argv[i][0] == '-') count++;
  }
  while ((c = getopt (argc, argv, "aliR")) != -1){

    switch (c){
      case 'a':
        A_FLAG = true;
        break;
      case 'l':
        L_FLAG = true;
        break;
      case 'i':
        I_FLAG = true;
        break;
      case 'R':
        R_FLAG = true;
        break;
      default:
         if (isprint(optopt))
            fprintf(stderr, "Unknown option `-%c'.\n", optopt);
         else
            fprintf(stderr, "Unknown option character `\\x%x'.\n",optopt);
        exit(-1);
      }
   }
   return count;
}

char get_file_type(struct stat st){
   switch (st.st_mode & S_IFMT) {
      case S_IFBLK:  return 'b';
      case S_IFCHR:  return 'c';
      case S_IFDIR:  return 'd';
      case S_IFIFO:  return 'p';
      case S_IFLNK:  return 'l';
      case S_IFREG:  return '-';
      case S_IFSOCK: return 's';
      default:       return '?';
   }
}

void get_permissions(struct stat st, char *permissions){
    strcpy(permissions, "----------");
   //owner  permissions
   if((st.st_mode & 0000400) == 0000400) permissions[0] = 'r';
   if((st.st_mode & 0000200) == 0000200) permissions[1] = 'w';
   if((st.st_mode & 0000100) == 0000100) permissions[2] = 'x';
    //group permissions
   if((st.st_mode & 0000040) == 0000040) permissions[3] = 'r';
   if((st.st_mode & 0000020) == 0000020) permissions[4] = 'w';
   if((st.st_mode & 0000010) == 0000010) permissions[5] = 'x';
    //others  permissions
   if((st.st_mode & 0000004) == 0000004) permissions[6] = 'r';
   if((st.st_mode & 0000002) == 0000002) permissions[7] = 'w';
   if((st.st_mode & 0000001) == 0000001) permissions[8] = 'x';
    //special  permissions
   if((st.st_mode & 0004000) == 0004000) permissions[2] = 's';
   if((st.st_mode & 0002000) == 0002000) permissions[5] = 's';
   if((st.st_mode & 0001000) == 0001000) permissions[8] = 't';

   permissions[9] = '\0';
}
void get_user_name(int uid, char *user_name){
    struct passwd * pwd = getpwuid(uid);
    memset(user_name, '\0', 32);
    strcpy(user_name,pwd->pw_name);
}     
void get_group_name(int gid, char *group_name){
    struct group * grp = getgrgid(gid);
    memset(group_name, '\0', 32);
    strcpy(group_name, grp->gr_name);
}
void set_file_size(struct dirent * entry, struct stat info, char* size){
    memset(size, '\0', 50);

    // handle character and block special files
    if(entry-> d_type == DT_BLK || entry-> d_type == DT_CHR){
        unsigned int maj = major(info.st_rdev);
        unsigned int min = minor(info.st_rdev);
        sprintf(size, "%d, %d",maj, min);
    }
    else{
      sprintf(size, "%jd", info.st_size);
   }
}
void set_time(long seconds, char *times){
    char timer[25]; 
    memset(times, '-', sizeof(char) * 16);
    times[16] = '\0';
    sprintf(timer, "%s", ctime(&seconds));
    memcpy(times, timer, 16);
}
void print_file_name(char* file_name, char file_type, int s){
  switch (file_type){
  case 'p':
    printf("%-*s", s, file_name);
    break;
  case 'c':
  case 'b':
    printf("\033[7m%-*s\033[m", s, file_name);
    break;
  case 'd':
    printf("\e[0;34m%-*s\033[0m", s, file_name);
    break;
  case 'l':
    printf("\033[95m%-*s\033[0m", s, file_name);
    break;
  case 's':
    printf("%-*s", s, file_name);
    break;
  case '-':
    if (strstr(file_name, ".out") || strstr(file_name, ".out"))
      printf("\e[0;32m%-*s\033[0m", s, file_name);
    else if (strstr(file_name, ".tar"))
      printf("\e[0;31m%-*s\033[0m", s, file_name);
    else
      printf("%-*s", s, file_name);
    break;
  }
}