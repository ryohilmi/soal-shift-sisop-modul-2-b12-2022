#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <json-c/json.h>
#include <dirent.h>
#include <string.h>

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
    char *argv[] = {"curl",
                    "-L",
                    "-o",
                    filename,
                    url,
                    NULL};

    run_exec("/usr/bin/curl", argv);
}

void extract_zip(char filename[], char output[])
{
    char *arg[] = {"unzip", "-n", filename, "-d", output, NULL};
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

void gacha(GachaItem items[], int *item_count, int *gacha_count)
{
    int random = rand() % *item_count;

    char path[100];
    char count[10];

    if (*gacha_count % 10 == 0)
    {
        sprintf(count, "%d", *gacha_count + 10);
        strcpy(path, "gacha_gacha/total_gacha_");
        strcat(path, count);
        make_dir(path);
    }
}

int main()
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
            gacha(characters, characters_count, gacha_count);
        }
        else
        {
            gacha(weapons, weapons_count, gacha_count);
        }

        (*gacha_count)++;
        primogems -= 160;
    }

    return 0;
}