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

            printf("%ld ", ep->d_ino);
            printf("%s ", items[i].name);
            printf("%d ", items[i].rarity);
            printf("%d\n", i);
            *count = i;
        }

        (void)closedir(dp);
    }
}

int main()
{
    GachaItem characters[50];
    GachaItem weapons[150];

    int *characters_count = malloc(sizeof(int));
    int *weapons_count = malloc(sizeof(int));

    parse_json(weapons, WEAPON, weapons_count);
    parse_json(characters, CHARACTER, characters_count);

    printf("Weapon: %d\n", *weapons_count);
    printf("Character: %d\n", *characters_count);
}