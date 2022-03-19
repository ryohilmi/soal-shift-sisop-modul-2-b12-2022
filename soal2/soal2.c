#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <wait.h>

//fungsi untuk mengekstract zip
void extract(char *zipfile, char *dir)
{
    char *argv[] = {"unzip", "-o", zipfile, "-d", dir, NULL};
    execv("/usr/bin/unzip", argv);
}

//fungsi untuk menghapus (remove)
void rmv(char *direct1, char *direct2, char *direct3)
{
    char *argv[] = {"rm", "-rf", direct1, direct2, direct3, NULL};
    execv("/usr/bin/rm", argv);
}

int main()
{
    /*
     	 //get username
	char *userid(char *s)
	char *user;
	user = (char *)malloc(10*sizeof(char));
	userid(user);

	//get path
	char to_path[100];
	strcpy(to_path, "/home/");
	strcat(to_path, user);
	*/

    pid_t child_id;
    child_id = fork();
    if (child_id == 0)
    {
        //buat folder shift2
        char *argv[] = {"mkdir", "-p", "/home/alya/shift2", NULL};
        execv("/bin/mkdir", argv);
    }

    pid_t child_id1;
    int status;
    child_id1 = fork();
    while ((wait(&status)) > 0)
        ;

    if (child_id1 == 0)
    {
        //membuat directory untuk folder tujuan extract
        char *argv[] = {"mkdir", "-p", "/home/alya/shift2/drakor", NULL};
        execv("/bin/mkdir", argv);
    }
    else
    {
        pid_t child_id2;
        child_id2 = fork();
        if (child_id2 == 0)
        {
            //extract zip
            extract("/home/alya/drakor.zip", "/home/alya/shift2/drakor");
        }
        else
        {
            while ((wait(&status)) > 0)
                ;
            //menghapus folder yang tidak dibutuhkan
            rmv("/home/alya/shift2/drakor/song", "/home/alya/shift2/drakor/coding", "/home/alya/shift2/drakor/trash");

            //char *argv[] = {"find", locpwd, "-mindepth", "1", "-maxdepth", "1", "-not", "-name", "*.png", "-exec", "rm, "-rf", "{}", "+", NULL};
            //execv("/usr/bin/find", argv);
        }
    }

    //gatau gmn caranya buat folder otomatis tanpa cek satu persatu dari namafile.png
    /*
	pid_t child_id3;
	child_id3 = fork();

	if(child_id3 < 0){
		exit(EXIT_FAILURE);
	}
	
	if(child_id3 == 0){
		char *argv[] = {{"mkdir", "-p", "home/alya/shift2/drakor/action", NULL}, {"mkdir", "-p", "home/alya/shift2/drakor/thriller", NULL}, 
				{"mkdir", "-p", "home/alya/shift2/drakor/horror", NULL}, {"mkdir", "-p", "home/alya/shift2/drakor/romance", NULL},
				{"mkdir", "-p", "home/alya/shift/drakor/comedy", NULL}, {"mkdir", "-p", "home/alya/shift2/drakor/fantasy", NULL},
				{"mkdir", "-p", "home/alya/shift2/drakor/school", NULL}};
		for(int i=0; i<8; i++}{
		if(fork()==0) continue;
		sleep(1);
		execv("usr/bin/mkdir", arg[i]);
	}
	*/
}
