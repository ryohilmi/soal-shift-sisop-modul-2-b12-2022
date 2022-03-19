#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <string.h>
#include <json-c/json.h>
#include <dirent.h>
#include <time.h>

#define CHARACTER 1
#define WEAPON 2

struct
{
    char name[100];
    int rarity;
} typedef GachaItem;

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

int main()
{
    pid_t pid, sid;

    pid = fork();

    if (pid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (pid > 0)
    {
        exit(EXIT_SUCCESS);
    }

    umask(0);

    sid = setsid();
    if (sid < 0)
    {
        exit(EXIT_FAILURE);
    }

    if ((chdir("/home/ryohilmi/Sisop/soal-shift-sisop-modul-2-b12-2022/soal1")) < 0)
    {
        exit(EXIT_FAILURE);
    }

    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    while (1)
    {
        struct tm *local;
        time_t t = time(NULL);
        local = localtime(&t);

        if (local->tm_mday == 30 && local->tm_mon == 2 && local->tm_hour == 4 && local->tm_min == 44)
        {
            int primogems = 79000;

            GachaItem characters[50];
            GachaItem weapons[150];

            int *characters_count = malloc(sizeof(int));
            int *weapons_count = malloc(sizeof(int));
            int *gacha_count = malloc(sizeof(int));
            *gacha_count = 0;

            download("characters.zip", "https://drive.google.com/uc?export=download&id=1xYYmsslb-9s8-4BDvosym7R4EmPi6BHp");
            download("weapon.zip", "https://drive.google.com/uc?export=download&id=1XSkAqqjkNmzZ0AdIZQt_eWGOZ0eJyNlT");
            make_dir("gacha_gacha");
            extract_zip("characters.zip", "gacha_gacha/");
            extract_zip("weapon.zip", "gacha_gacha/");

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
        }

        if (local->tm_mday == 30 && local->tm_mon == 2 && local->tm_hour == 7 && local->tm_min == 44)
        {
            zip();
            remove_files("gacha_gacha/");
            remove_files("characters.zip");
            remove_files("weapon.zip");
        }

        sleep(30);
    }
}