#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>
#include <string.h>
#include <stddef.h>
#include <limits.h>
#include <dirent.h>

struct drakor {
	char name_file[100];
	char title[100];
	char genre[100];
	int year;
};

//membuat folder berdasarkan genre
void folder_genre(struct drakor *ptr, int index, char path[]){
	char tmp[100];
	strcpy(tmp, path);
	strcat(tmp, "/");
	strcat(tmp, (ptr+index)->genre);

	pid_t child1_id;
	child1_id = fork();
	int status;

	if(child1_id < 0){
		exit(EXIT_FAILURE);
	}
	if(child1_id == 0){
		char *argv[] = {"mkdir", "-p", tmp, NULL};
		execv("/bin/mkdir", argv);
	} else {
		while ((wait(&status)) > 0);
		return;
	}
}

//memindahkan file png di folder sesuai genrenya
void move_genre(struct drakor *ptr, int index, char path[]){
	char from[100];
	strcpy(from, path);
	strcat(from, "/");
	strcat(from, (ptr+index)->name_file);
	
	char destination[100];
	strcpy(destination, path);
	strcat(destination, (ptr+index)->genre);
	strcat(destination, "/");
	strcat(destination, (ptr+index)->title);
	strcat(destination, ".png");

	pid_t child2_id;
	child2_id = fork();
	int status;
	
	if(child2_id < 0){
		exit(EXIT_FAILURE);
	}
	if(child2_id == 0){
		char *argv[] = {"cp", from, destination, NULL};
		execv("/usr/bin/cp", argv);
	} else {
		while ((wait(&status)) > 0);
		return;
	}
}

//fungsi untuk mengekstract zip sekaligus menghapus folder yg tidak diperlukan
void extract (char *zipfile, char *dir){
        char *argv[] = { "unzip", "-o", zipfile, "-d", dir, "*.png", NULL };
        execv ("/usr/bin/unzip", argv);
}

int main(){
	char path[50];
	strcpy(path, "/home");
	strcat(path, "/alya/shift2/drakor");

	pid_t child_id;
        child_id = fork();
	int status, flag=0;
       
	 if (child_id == 0){
		//buat folder shift2
                char *argv[] = {"mkdir", "-p", path, NULL};
                execv("/bin/mkdir", argv);
	}else {
		while ((wait(&status)) > 0);
		pid_t child_id2;
		child_id2 = fork();
		if (child_id2 < 0){
			exit(EXIT_FAILURE);
		}
		if(child_id2 == 0){
   			extract("/home/alya/drakor.zip", "/home/alya/shift2/drakor");
		} else {
			while ((wait(&status)) > 0);
			struct drakor data[100];
			struct dirent *dp;
			DIR *dir = opendir(path);
			while ((dp = readdir(dir)) != NULL){
				if(strcmp(dp->d_name, ".") !=0 && strcmp(dp->d_name, "..") != 0){
				int flag1 = 0;
				strcpy(data[flag].name_file, dp->d_name);
				if(strchr(data[flag].name_file, '_')!= 0) flag1++;
				char *tok = strtok(dp->d_name, "_;.");
				for(int i=0; tok!=NULL; i++){
					if(i==0) strcpy(data[flag].title, tok);
					else if (i==1) data[flag].year=atoi(tok);
					else if (i==2) strcpy(data[flag].genre, tok);
					else if (i==3) {
						flag++;
						if(flag1!=0){
							strcpy(data[flag].name_file, data[flag-1].name_file);
							strcpy(data[flag].title, tok);
							flag1=0;
						}
						i=0;
					}
					tok = strtok (NULL, "_;.");
				}	
				}	
			}
			for(int i=0; i<flag; i++) folder_genre(data, i, path);
			for(int i=0; i<flag; i++) move_genre(data, i, path);
        	}
	}
}
