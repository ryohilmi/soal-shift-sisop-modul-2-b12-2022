# sisop-modul-1-b12-2022

## Kelompok B12:
- [Alya Shofarizqi Inayah](https://gitlab.com/https://gitlab.com/alyashofarizqi): 5025201113
- [Ryo Hilmi Ridho](https://gitlab.com/ryohilmi): 5025201192
- [Surya Abdillah](https://gitlab.com/Surya_Abdillah): 5025201229

## _Soal 3_
Pada soal 3, praktikan diminta untuk membuat program yang dapat melakukan klasifikasi hewan.
## Pembahasan
Berikut merupakan library yang digunakan oleh praktikan:
```sh
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <time.h>
#include <dirent.h>
#include <fcntl.h>
```
### 3.a
Membuat folder `/home/<user>/modul2/darat` lalu 3 detik setelahnya membuat folder `/home/<user>/modul2/air`.
#### Penyelesaian
Kita dapat menggunakan command `mkdir()` untuk diexecute. Sedangkan, untuk membuat jeda 3 detik dapat menggunakan `sleep(3)`. Adapun, langkah-langkah yang perlu dilakukan adalah:
1. Mendapatkan `username` 
```sh
   char *cuserid(char *s);
   char *user; 
   user = (char *)malloc(10*sizeof(char)); 
   cuserid(user);
```
2. Mendapatkan path `/home/username/`
```sh
   char to_path[100]; 
   strcpy(to_path,"/home/");
   strcat(to_path,user);
```
3. Mendapatkan path `/home/username/modul2/air` dan `/home/username/modul2/darat`
```sh
   char air[100]; strcpy(air,to_path); strcat(air,"/modul2/air");  
   char darat[100]; strcpy(darat,to_path); strcat(darat,"/modul2/darat");
```
4. Membuat folder darat, lalu memberi jeda 3 detik. Diletakkan pada child pertama
```sh
    execl("/bin/mkdir","mkdir","-p",darat,NULL);
    sleep(3);
```
5. Membuat folder air. Diletakkan pada child kedua
```sh
    pid_t child_id2; child_id2 = fork();
```
> Dalam setiap state `else`, yakni parent dari hasil fork bersangkutan. dilakukan `while((wait(&status)) > 0);`. Hal ini digunakan agar proses dalam child dan parent dapat berjalan secara berurutan.
> Selain itu, juga dilakukan spawning untuk melakukan proses berikutnya. Hal ini dilakukan dengan cara `pid_t child_id; child_id = fork();`. Hal ini tidak perlu dilakukan pada proses terakhir [parent process]
### 3.b
Mengekstrak file `animal.zip` ke dalam folder `/home/<user>/modul2/`. Tidak dijelaskan dimana letak file animal.zip sehingga praktikan membuat asumsi bahwa `animal.zip berada dalam folder /home/user/Downloads/`.
1. Mendapatkan path menuju `Downloads`
```sh
    char Download[100]; strcpy(Download,to_path); strcat(Download,"/Downloads/animal.zip");
```
2. Menggunakanan command `unzip` untuk mengekstrak. Diletakkan pada child ketiga
```sh
    execl("/bin/unzip","unzip","-o","-q",Download,"-d",to_modul2,NULL);
```
### 3.c
Memisah file hasil ekstrak ke dalam folder yang sesuai, apabila gambar tidak termasuk keduanya, maka file akan dihapus
1. Mendapatkan path menuju folder `animal`
```sh
    char animal[100]; strcpy(animal,to_modul2);strcat(animal,"/animal");
```
2. Untuk memindahkan file dapat digunakan kombinasi command `find` dan `mv`, serta untuk memberi jeda dapat menggunakan `sleep()`. Diletakkan pada child keempat
```sh
    execl("/bin/find","find",animal,"-name","*air*","-exec","mv","-t",air,"{}","+",NULL);
    sleep(3);
```
3. Dengan menggunakan metode yang sama, maka dapat memindah file hewan air dengan cara berikut: (Diletakkan pada child kelima)
```sh
    execl("/bin/find","find",animal,"-name","*air*","-exec","mv","-t",air,"{}","+",NULL);
```
4. Dikarenakan hewan air dan darat dalam folder animal telah dipindah, maka hanya tersisa hewan yang tanpa keterangan air / darat. Sehingga, penghapusan dapat diartikan sebagai menghapus file yang tersisa di dalam folder animal. Kita dapat menggunakan command `find` dan `rm`. Proses ini berada di child keenam
```sh
    execl("/bin/find","find",animal,"-type","f","-name","*.jpg","-exec","rm","-f","{}","+",NULL); 
```
### 3.d 
menghapus semua `bird` yang ada dalam folder darat
1. Dengan menggunakan metode yang sama dengan bagian 3.c langkah ke empat, yakni dengan kombinasi `find` dan `rm`, maka program dapat dijalankan dengan cara berikut: (Diletakkan pada child ketujuh)
```sh
    execl("/bin/find","find",darat,"-name","*bird*","-exec","rm","-rf","-f","{}","+",NULL);
```
### 3.e
Membuat file `list.txt` yang akan menyimpan informasi berupa `UID_[UID FILE PERMISSION]_Nama File.jpg`. Contoh: conan_rwx_hewan.png
1. Mendapatkan path `to_list`, yakni path menuju file `list.txt`
```sh
    char to_list[100]; strcpy(to_list,air); strcat(to_list,"/list.txt");
```
2. Membuat file `list.txt` dengan command `touch`. Diletakkan pada child kedelapan
```sh
    execl("/bin/touch","touch",to_list,NULL);
```
3. Mengambil informasi yang diperlukan untuk sesuai dengan format, yaitu: (Diletakkan pada child kesembilan)
```sh
    execl("/bin/find","find",air,"-name","*.jpg","-fprintf",to_list,"%u_%M_%f\n",NULL);
```
>`%u` akan mengambil `username`. contoh: `surya`
>`%M` akan mengambil `file permission` untuk user, group, others. contoh: `-rwxr–rw-`
>`%f` akan mengambil `basename` dari file. contoh: `bear_darat.jpg`
4. Menghapus file permission untuk groups dan others. Diletakkan pada child kesepuluh.
```sh
    execl("/bin/gawk","gawk","-i","inplace","BEGIN{FS=\"-\"} {print $1\"-\"$2$(NF)}",to_list,NULL);
```
>`-i inplace` akan membuat perubahan disimpan dalam file list.txt secara langsung
5. Mengeliminasi bagian pada base name yang tidak diperlukan. Diletakkan pada child kesepuluh [parent terakhir]
```sh
    execl("/bin/gawk","gawk","-i","inplace","BEGIN{FS=\"_\"} { print $1\"_\"$2\"_\"$3\".jpg\"}",to_list,NULL);
```
> Pada posisi ini, isi file `list.txt` telah sesuai dengan format yang ditentukan dalam soal
### Dokumentasi Pengerjaan
- Menganalisis Soal
![Penganalisisan_Soal](/uploads/f3a0c2b9e6868a6e10777dd893bc01b2/Penganalisisan_Soal.jpeg)
-
