#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

void run_exec(char path[], char *argv[])
{
    pid_t child_id;
    int status;

    child_id = fork();

    if (child_id < 0)
    {
        while (child_id < 0)
            child_id = fork();
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

int main()
{
    pid_t child_wget, child_zip;

    child_wget = fork();

    if (child_wget < 0)
    {
        exit(EXIT_FAILURE);
    }

    if (child_wget == 0)
    {
        char *argv[] = {"curl",
                        "-L",
                        "-o",
                        "characters.zip",
                        "https://drive.google.com/uc?export=download&id=1xYYmsslb-9s8-4BDvosym7R4EmPi6BHp",
                        "-o",
                        "weapon.zip",
                        "https://drive.google.com/uc?export=download&id=1XSkAqqjkNmzZ0AdIZQt_eWGOZ0eJyNlT",
                        NULL};

        execv("/usr/bin/curl", argv);
    }
    else
    {
        child_zip = fork();

        if (child_zip < 0)
        {
            exit(EXIT_FAILURE);
        }

        if (child_zip == 0)
        {
            while (1)
            {
                if (access("./weapon.zip", F_OK) == 0 && access("./characters.zip", F_OK) == 0)
                    break;

                sleep(20);
            }

            char *argv[] = {"unzip", "*.zip", NULL};

            execv("/usr/bin/unzip", argv);
        }
    }

    return 0;
}