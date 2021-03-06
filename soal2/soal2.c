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

//check array of struct
void check_struct(struct drakor *ptr, int flag){
	for(int i=0; i<flag; i++){
		printf("file name: %s\n", (ptr+i)->name_file);
		printf("title : %s\n", (ptr+i)->title);
		printf("year: %d\n", (ptr+i)->year);
		printf("genre : %s\n", (ptr+i)->genre);
		printf("\n");
	}
	return;
}

//
void list_genre(struct drakor *ptr, int flag){
	struct drakor list[100];
	int index_list = flag;
	for(int i=0; i<flag; i++){
		strcpy(list[i].genre, (ptr+i)->genre);
	}
	//hapus duplicate genre
	for(int i=0; i<index_list; i++){
		for(int j=i+1; j<index_list; j++){
			if(strcmp(list[i].genre, list[j].genre)==0){
				for(int k=j; k<index_list; k++){
					strcpy(list[k].genre, list[k+1].genre);
				}
				j--;
				index_list--;
			}	
		}
	}
	//cek list genre
	printf("Ada %d genre, yaitu : ", index_list);
	for(int i=0; i<index_list; i++){
		printf("%s\n", list[i].genre);
	}

	//text
	for(int i=0; i<index_list; i++){
		struct drakor tmp[100];
		int index_tmp=0;
		for(int j=0; j<flag; j++){
			if(strcmp(list[i].genre, (ptr+j)->genre)==0){
				strcpy(tmp[index_tmp].title, (ptr+j)->title);
				tmp[index_tmp].year=(ptr+j)->year;
				index_tmp++;
			}
		}
		for(int j=0; j<index_tmp-1; j++){
			int min_id=j;
			for(int k=j+1; k<index_tmp; k++){
				if(tmp[k].year<tmp[min_id].year){
				min_id=k;
				struct drakor tmp2;
 				strcpy(tmp2.title,tmp[min_id].title);
				tmp2.year=tmp[min_id].year;
				strcpy(tmp[min_id].title, tmp[j].title);
				tmp[min_id].year=tmp[j].year;
				strcpy(tmp[j].title, tmp2.title);
				tmp[j].year=tmp2.year;
				}		
			}
		}
		char text_path[100];
		strcpy(text_path, "/home/alya/shift2/drakor/");
		strcat(text_path, list[i].genre);
		strcat(text_path, "/data.txt");

		pid_t child_id;
		child_id = fork();
		int status;

		if(child_id <0){
			exit(EXIT_FAILURE);
		}
		if(child_id == 0){
			char *argv[] = {"touch", text_path, NULL};
			execv("/usr/bin/touch", argv);
		} else {
			while((wait(&status)) > 0);
		}

		char isi_text[1000];
		strcpy(isi_text, "kategori : ");
		strcat(isi_text, list[i].genre);
		for(int i=0; i<index_tmp; i++){
			strcat(isi_text, "\n\n");
			strcat(isi_text, "nama : ");
			strcat(isi_text, tmp[i].title);
			strcat(isi_text, "\n");
			strcat(isi_text, "rilis : ");
			char movie_year[10];
			sprintf(movie_year, "%d", (tmp[i].year));
			strcat(isi_text, movie_year);
		}
		FILE *fptr=fopen(text_path, "a");
		fputs(isi_text, fptr);
		fclose(fptr);
		}
		return;
}

//menghapus file png diluar folder genre
void delete_file(struct drakor *ptr, int i){
	pid_t child_id;
	child_id = fork();
	int status;

	if(child_id < 0){
		exit(EXIT_FAILURE);
	}
	if(child_id == 0){
		char del_path[100];
		strcpy(del_path, "/home/alya/shift2/drakor");
		strcat(del_path, "/");
		strcat(del_path, (ptr+i)->name_file);
		char *argv[] = {"find", del_path, "-delete", NULL};
		execv("/bin/find", argv);
	} else {
		while((wait(&status)) > 0);
		return;
	}
}

//mengekstract zip sekaligus menghapus folder yg tidak diperlukan
void extract (char *zipfile, char *dir){
        char *argv[] = { "unzip", "-o", zipfile, "-d", dir, "*.png", NULL };
        execv ("/usr/bin/unzip", argv);
}

int main(){
	char path[50];
	strcpy(path, "/home");
	strcat(path, "/alya/shift2/drakor/");

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
			check_struct(data, flag);
			list_genre(data, flag);
			
			for(int i=0; i<flag; i++) delete_file(data, i);
			char *argv[] = {"find", path, NULL};
			execv("/bin/find", argv);
			closedir(dir);
			
        	}
	}
}
