# sisop-modul-1-b12-2022

## Kelompok B12:
- [Alya Shofarizqi Inayah](https://gitlab.com/https://gitlab.com/alyashofarizqi): 5025201113
- [Ryo Hilmi Ridho](https://gitlab.com/ryohilmi): 5025201192
- [Surya Abdillah](https://gitlab.com/Surya_Abdillah): 5025201229

# Soal 1
## Penjelasan Soal
Pada soal nomor 1 ini, praktikan diminta untuk membuat sebuah program daemon yang melakukan gacha, dengan data-data characters dan weapon yang diberikan dalam bentuk json, yang kemudian di-extract dan di-parse

## Pengerjaan
Untuk mempermudah pengerjaan, saya membuat sebuah fungsi yang digunakan untuk menjalankan exec tanpa harus melakukan fork secara manual

```c
void run_exec(char path[], char *argv[])
{
    pid_t child_id;
    int status;

    child_id = fork();

    if (child_id < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (child_id == 0)
    {
        execv(path, argv);
        exit(0);
    }
    else
    {
        while ((wait(&status)) > 0)
            ;
    }
}
```

## 1.a
Saat program berjalan, dua file zip character dan weapon akan di-download kemudian di-extract ke folder gacha_gacha. Saya membuat 3 fungsi yang digunakan untuk download, extract zip dan membuat folder
```c

void download(char filename[], char url[])
{
    char *argv[] = {"wget",
                    url,
                    "-O",
                    filename,
                    "-q",
                    NULL};

    run_exec("/usr/bin/wget", argv);
}

void extract_zip(char filename[], char output[])
{
    char *arg[] = {"unzip", "-q", filename, "-d", output, NULL};
    run_exec("/usr/bin/unzip", arg);
}

void make_dir(char dir[])
{
    char *arg[] = {"mkdir", "-p", dir, NULL};
    run_exec("/bin/mkdir", arg);
}

```

Tiga fungsi tersebut dipanggil seperti berikut
```c
download("characters.zip", "https://drive.google.com/uc?export=download&id=1xYYmsslb-9s8-4BDvosym7R4EmPi6BHp");
download("weapon.zip", "https://drive.google.com/uc?export=download&id=1XSkAqqjkNmzZ0AdIZQt_eWGOZ0eJyNlT");
make_dir("gacha_gacha");
extract_zip("characters.zip", "gacha_gacha/");
extract_zip("weapon.zip", "gacha_gacha/");
```

## 1.b, 1.c, dan 1.d
Sub soal b, c, dan d saling berhubungan. Untuk pengerjaannya, saya membuat sebuah struct bernama GachaItem yang berfungsi untuk menampung nama dan rarity dari character dan weapon yang akan di-gacha, lalu membuat 2 array kosong yang akan diisi dengan semua character dan weapon yang ada. Setelah data di-parse dan dimasukkan ke dalam array, akan dilakukan gacha sesuai dengan jumlah primogems yang ada sesuai ketentuan.

```c
struct
{
    char name[100];
    int rarity;
} typedef GachaItem;


int primogems = 79000;

GachaItem characters[50];
GachaItem weapons[150];

int *characters_count = malloc(sizeof(int));
int *weapons_count = malloc(sizeof(int));
int *gacha_count = malloc(sizeof(int));
*gacha_count = 0;

parse_json(weapons, WEAPON, weapons_count);
parse_json(characters, CHARACTER, characters_count);

while (primogems >= 160)
{
	if (*gacha_count % 2 == 0)
	{
		gacha(characters, characters_count, gacha_count, primogems);
	}
	else
	{
		gacha(weapons, weapons_count, gacha_count, primogems);
	}

	(*gacha_count)++;
	primogems -= 160;
}
```

Untuk parse json-nya, saya menggunakan 2 fungsi, yaitu `read_file` dan `parse_json`. `parse_json` berfungsi untuk menge-loop semua file yang ada dan akan memanggil `read_file` yang berfungsi untuk parse data name dan rarity lalu memasukkannya ke dalam array.
```c
void read_file(char dir[], char filename[], GachaItem *character)
{
    FILE *fp;
    char buffer[5120];
    char target_file[100];

    struct json_object *parsed_json;
    struct json_object *name;
    struct json_object *rarity;

    strcpy(target_file, dir);
    strcat(target_file, filename);

    fp = fopen(target_file, "r");
    fread(buffer, sizeof(char), 5120, fp);
    fclose(fp);

    parsed_json = json_tokener_parse(buffer);

    json_object_object_get_ex(parsed_json, "name", &name);
    json_object_object_get_ex(parsed_json, "rarity", &rarity);

    strcpy(character->name, json_object_get_string(name));
    character->rarity = json_object_get_int(rarity);
}

void parse_json(GachaItem items[], int type, int *count)
{
    DIR *dp;
    struct dirent *ep;

    char *path = (type == CHARACTER ? "gacha_gacha/characters/" : "gacha_gacha/weapons/");

    dp = opendir(path);

    if (dp != NULL)
    {
        for (int i = 0; (ep = readdir(dp)); i++)
        {
            GachaItem *item = malloc(sizeof(GachaItem));
            items[i] = *item;
            read_file(path, ep->d_name, &items[i]);

            *count = i;
        }

        (void)closedir(dp);
    }
}

```

Untuk proses gachanya sendiri, saya membuat fungsi bernama `gacha`. Pada fungsi tersebut, pertama-tama diambil waktu untuk nama file, lalu mengambil angka random untuk memilih item random. Dibuat variabel `gacha_dir` dan `gacha_file` untuk menampung nama directory dan filenya.

```c
void gacha(GachaItem items[], int *item_count, int *gacha_count, int primogems)
{
    struct tm *local;
    time_t t = time(NULL);

    local = localtime(&t);

    int random = rand() % (*item_count - 2);

    static char gacha_dir[100];
    static char gacha_file[100];
    char dir_count[10];
    char file_count[10];

    if (*gacha_count % 90 == 0)
    {
        sprintf(dir_count, "%d", *gacha_count + (primogems < 14400 ? primogems / 160 : 90));
        strcpy(gacha_dir, "gacha_gacha/total_gacha_");
        strcat(gacha_dir, dir_count);
        make_dir(gacha_dir);
    }

    if (*gacha_count % 10 == 0)
    {
        sprintf(file_count, "%d", *gacha_count + (primogems < 1600 ? primogems / 160 : 10));
        sprintf(gacha_file, "%s/%02d:%02d:%02d_gacha_%s.txt", gacha_dir, local->tm_hour, local->tm_min, local->tm_sec, file_count);
        sleep(1);
    }

    FILE *fp;
    fp = fopen(gacha_file, "a");
    fprintf(fp, "%d_%s_%s_%d\n", *gacha_count + 1, *gacha_count % 2 == 0 ? "character" : "weapon", items[random].name, items[random].rarity);
    fclose(fp);
}
```

## 1.e 
Pada sub soal 1.e, praktikan diminta untuk mengecek waktu sebelum proses gachanya dilakukan. Jika gacha sudah selesai, folder gacha_gacha akan di-zip kemudian dihapus

```c
if (local->tm_mday == 30 && local->tm_mon == 2 && local->tm_hour == 4 && local->tm_min == 44)
{
	// gacha
}

if (local->tm_mday == 30 && local->tm_mon == 2 && local->tm_hour == 7 && local->tm_min == 44)
{
	zip();
	remove_files("gacha_gacha/");
	remove_files("characters.zip");
	remove_files("weapon.zip");
}
```

Berikut isi dari funngsi zip dan remove
```c
void zip()
{
    char *arg[] = {"zip", "-r", "-qq", "-P", "satuduatiga", "not_safe_for_wibu.zip", "gacha_gacha/", NULL};
    run_exec("/usr/bin/zip", arg);
}

void remove_files(char dir[])
{
    char *arg[] = {"rm", "-rf", dir, NULL};
    run_exec("/bin/rm", arg);
}
```

## Dokumentasi Pengerjaan
- Pembuatan script c
![script_c](https://cdn.discordapp.com/attachments/856447778091696148/957650041202425896/unknown-150.png)
- hasil gacha
![hasil_gacha](https://cdn.discordapp.com/attachments/856447778091696148/957650041407963226/unknown-160.png)

## Kendala
Terdapat beberapa kendala yang muncul saat melakukan pengerjaan, diantaranya:
- fork() yang dipanggil beberapa kali menyebabkan kodenya sulit dibaca, sehingga akhirnya saya membuat fungsi `run_exec` untuk merapihkan kodenya
- Pada saat pengerjaan, program dibuat tanpa menggunakan daemon terlebih dahulu. Setelah diubah menjadi daemon, program tidak berjalan seperti semestinya yang ternyata disebabkan oleh `wget` dan `zip` yang belum diberi quiet option, sehingga memberikan output ke stdout dan program pun stuck.



# Soal 2
### Penjelasan Soal
Pada soal nomor 2 ini diberikan folder zip bernama drakor.zip yang berisi poster-poster drama korea dengan format png. Dengan folder zip ini, kami ditugaskan untuk mengekstrak dan memisahkan poster-poster drama korea tersebut tergantung dengan kategorinya.

## 2.a
Hal pertama yang perlu dilakukan oleh program adalah mengextract zip yang diberikan ke dalam folder `“/home/[user]/shift2/drakor”` dan menghapus folder-folder yang tidak diperlukan.

```sh
void extract (char *zipfile, char *dir){
        char *argv[] = { "unzip", "-o", zipfile, "-d", dir, "*.png", NULL };
        execv ("/usr/bin/unzip", argv);
}
```

Membuat fungsi `extract` yang memanggil command `unzip` untuk mengekstrak zip file. Didalam fungsi ini sekaligus menghapus folder-folder yang tidak diperlukan dengan menambahkan `"*.png"` dimana mengekstrak format png saja.

```sh
char path[50];
	strcpy(path, "/home");
	strcat(path, "/alya/shift2/drakor/");
```
>Pada int main, menginisiasi working space kedalam beberapa variabel untuk mempermudah dalam menuliskan code

```sh
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

```
Sebelum memanggil fungsi extract yang telah dibuat, membuat folder destination extract `shift2` dan `drakor` terlebih dahulu karena tidak diperbolehkan untuk membuat foldernya secara manual. Setelah itu baru memanggil fungsi `extract`.

## 2.b
Poster drama korea perlu dikategorikan sesuai jenisnya, maka program harus membuat folder untuk setiap jenis drama korea yang sudah ter-extract. Karena tidak mungkin memeriksa satu-persatu manual, maka program harus membuatkan folder-folder yang dibutuhkan sesuai dengan genre pada poster drama korea.

```sh
struct drakor {
	char name_file[100];
	char title[100];
	char genre[100];
	int year;
};
```
> Membuat `struct drakor` untuk menyimpan setiap data dari poster drakor yang tersedia.

```sh
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
...
```
Membaca semua file path dan mengambil setiap data dari name file. Lalu, memasukkan setiap data kedalam `struct drakor` yang telah diinialisasi sebelumnya, gunakan juga `strtok` untuk mengambil data title, year, dan genre dari setiap file yang tersedia. Setelah didapat semua data pada `path`, kirim data dan looping sebanyak jumlah flag untuk membuat folder dengan melengkapi path sesuai genre apa saja yang tersedia dalam setiap nama file di `path`.

```sh
void folder_genre(struct drakor *ptr, int index, char path[]){
	char tmp[100];
	strcpy(tmp, path);
	strcat(tmp, "/");
	strcat(tmp, (ptr+index)->genre);
	...
	char *argv[] = {"mkdir", "-p", tmp, NULL};
	execv("/bin/mkdir", argv);
	...
}
```
> Membuat fungsi `folder_genre` untuk buat folder sesuai dengan genrenya menggunakan `mkdir -p` dan dipanggil pada int main

```sh
...
for(int i=0; i<flag; i++) folder_genre(data, i, path);
...
```

## 2.c
Setelah folder kategori berhasil dibuat, program akan memindahkan poster ke folder dengan kategori yang sesuai dan di rename dengan nama. Contoh: “/drakor/romance/start-up.png”.

```sh
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
    ...
	if(child2_id == 0){
		char *argv[] = {"cp", from, destination, NULL};
		execv("/usr/bin/cp", argv);
	} else {
		while ((wait(&status)) > 0);
		return;
	}
}
```
Membuat fungsi `move_genre` untuk memindahkan folder png sesuai dengan genre pada title poster dengan membuat path terlebih dahulu untuk mencatat asal file dengan memasukkan nama filenya dan memasukkan genre file sebagai foldernya dan ubah nama file dengan data title dari masing-masing file tersebut. Tetapi disini, saya men-copy dahulu poster dengan command `cp` dari path `from` dan di paste kedalam `destination`

## 2.d
Karena dalam satu foto bisa terdapat lebih dari satu poster maka foto harus dipindah ke masing-masing kategori yang sesuai. Contoh: foto dengan nama “start-up;2020;romance_the-k2;2016;action.png” dipindah ke folder “/drakor/romance/start-up.png” dan “/drakor/action/the-k2.png”.

```sh
int flag1 = 0;
	...
	if(strchr(data[flag].name_file, '_')!= 0) flag1++;
	...
		if(flag1!=0){
			strcpy(data[flag].name_file, data[flag-1].name_file);
			strcpy(data[flag].title, tok);
			flag1=0;
		}
...
```
Seperti yang sudah dijelaskan pada poin b, untuk case ini memakai `strchr` untuk mencari nama file yang menunjukkan dua poster sekaligus, tandai file tersebut dan catat data tersebut dengan nama file yang sama pada array of struct setelahnya. Jadi, setelah didapatkan dua nama file yang sama pada beberapa iterasi yang berurutan, data nama poster tersebut digunakan untuk mencatat path `from` pada poin c sebelumnya dan didapatkan dua file dengan folder genre di masing-masing poster pada file tersebut. 

```sh
...
	for(int i=0; i<flag; i++) move_genre(data, i, path);
...
```
> Setelah itu, fungsi `move_genre` dipanggil pada int main.

## 2.e
Di setiap folder kategori drama korea buatlah sebuah file "data.txt" yang berisi nama dan tahun rilis semua drama korea dalam folder tersebut, jangan lupa untuk sorting list serial di file ini berdasarkan tahun rilis (Ascending).

```sh
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
```
> Membuat fungsi `list_genre` yang digunakan untuk menghapus duplicate genre, mengelist dan mengecek genre, setelah diiterasi cek dan di-list maka selanjutnya akan di tuliskan pada file txt yang nantinya akan berada disetiap folder genre.

```sh
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
        ...
			char *argv[] = {"touch", text_path, NULL};
			execv("/usr/bin/touch", argv);
        ...

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
```
Menentukan destination path file txt `text_path` dimana sesuai dengan genre masing-masing drama korea dan menuliskan text dengan format yang sudah ditentukan `isi_text`, yakni kategori, nama, lalu tahun rilis yang mana nantinya akan diurutkan secara ascending berdasarkan tahun rilisnya. Setelah itu, file txt akan diletakkan ke `text_path` menggunakan `fputs`.

## Dokumentasi Pengerjaan
- Pembuatan script c
![script_c](/uploads/b06fe0354d21dc7991fb6ef2fc3476c2/script_c.jpeg)
- hasil folder_genre
![folder_genre](/uploads/8328e6abbf209f365506f66227e9a755/folder_genre.jpeg)
- hasil move_genre
![isi_folder_genre](/uploads/d553fe077a3dd788665c0e08a0467204/isi_folder_genre.jpeg)
- hasil txt
![isi_text](/uploads/b89c4fbf672663f4c823501958efbd95/isi_text.jpeg)

## Kendala
Kendala-kendala saat progress pengerjaan soal nomor 2 :
1. Pada poin 2b, sulit untuk mengimplementasikan codenya bagaimana untuk membuat folder secara otomatis dengan membaca keseluruhan isi folder.
2. Sempat mengalami error pada poin 2c untuk memindahkan file poster png ke masing-masing folder genre, hal ini disebabkan karena kurangnya ketelitian untuk menambahkan `/` pada `path`

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
- Pengetesan Sleep
![Pengetesan_Sleep](/uploads/89a6fe128247f022cd880f0afd4808a8/Pengetesan_Sleep.jpeg)
- Pengerjaan
![Pengerjaan_3a](/uploads/7c44ba8cc74277594d2e80a38e9b0322/Pengerjaan_3a.jpeg)
### Kendala
- Kendala terjadi pada proses command `find` + `-exec mv`. Hal ini disebabkan kesalahpahaman akan sintaks yang dilihat dari referensi di internet, yakni "'\*darat\*'". Hal ini sangat merepotkan karena kesalahan tadi tidak termasuk dalam sintaks error sehingga tidak ada informasi error. Solusi yang benar adalah menghapus petik satu "\*darat\*".
