#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>

int main() { 
   //GET USER NAME
   char *cuserid(char *s);
   char *user; 
   user = (char *)malloc(10*sizeof(char)); 
   cuserid(user);

   //GET PATH to USER
   char to_path[100]; 
   strcpy(to_path,"/home/");
   strcat(to_path,user);

   //GET PATH DARAT AIR
   char air[100]; strcpy(air,to_path); strcat(air,"/modul2/air");  
   char darat[100]; strcpy(darat,to_path); strcat(darat,"/modul2/darat");
   char to_modul2[100]; strcpy(to_modul2,to_path); strcat(to_modul2,"/modul2");
   char Download[100]; strcpy(Download,to_path); strcat(Download,"/Downloads/animal.zip");
   char animal[100]; strcpy(animal,to_modul2);strcat(animal,"/animal");
   char to_list[100]; strcpy(to_list,air); strcat(to_list,"/list.txt");

   int status;
   pid_t child_id; child_id = fork();

   if (child_id < 0){
      exit(EXIT_FAILURE);
   }

   if (child_id == 0){
     execl("/bin/mkdir","mkdir","-p",darat,NULL);
     sleep(3);
   } 
   else {
     while((wait(&status)) > 0);
     pid_t child_id2; child_id2 = fork();

      if(child_id2 == 0){
        execl("/bin/mkdir","mkdir","-p",air,NULL);
      } else {
        while((wait(&status)) > 0);
        pid_t child_id3; child_id3 = fork();

        if(child_id3 == 0){
          //ASUMSI bahwa DEFAULT mendownload file ada di folder ~/DOwnloads
          execl("/bin/unzip","unzip","-o","-q",Download,"-d",to_modul2,NULL);
        } else {
          while((wait(&status)) > 0);
          pid_t child_id4; child_id4 = fork();

          if(child_id4 == 0){
            execl("/bin/find","find",animal,"-name","*darat*","-exec","mv","-t",darat,"{}","+",NULL);
            sleep(3);
          } else{
            while((wait(&status)) > 0);
            pid_t child_id5; child_id5 = fork();

            if(child_id5 == 0){
              execl("/bin/find","find",animal,"-name","*air*","-exec","mv","-t",air,"{}","+",NULL);
            } else {
              while((wait(&status)) > 0);
              pid_t child_id6; child_id6 = fork(); 

              if(child_id6 == 0){
                execl("/bin/find","find",animal,"-type","f","-name","*.jpg","-exec","rm","-f","{}","+",NULL);                  
              } else {
                while((wait(&status)) > 0);
                pid_t child_id7; child_id7 = fork();

              if(child_id7 == 0){
                execl("/bin/find","find",darat,"-name","*bird*","-exec","rm","-rf","-f","{}","+",NULL);
              } else {
                while((wait(&status)) > 0);
                pid_t child_id8; child_id8 = fork();

                if(child_id8 == 0){
                  execl("/bin/touch","touch",to_list,NULL);
                } else {
                  while((wait(&status)) > 0);
                  pid_t child_id9; child_id9 = fork();
                  FILE *file_list = fopen(to_list,"r+");

                  if(child_id9 == 0){
                    execl("/bin/find","find",air,"-name","*.jpg","-fprintf",to_list,"%u_%M_%f\n",NULL);
                  } else {
                    while((wait(&status)) > 0);
                    pid_t child_id10; child_id10 = fork();

                    if(child_id10 == 0){
                      execl("/bin/gawk","gawk","-i","inplace","BEGIN{FS=\"-\"} {print $1\"-\"$2$6}",to_list,NULL);
                    } else {
                      while((wait(&status)) > 0);
                      execl("/bin/gawk","gawk","-i","inplace","BEGIN{FS=\"_\"} { print $1\"_\"$2\"_\"$3\".jpg\"}",to_list,NULL);
                      }
                    }
                  }
                }
              }
            }
          }
        }
      }
   }
}

