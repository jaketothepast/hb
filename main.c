#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
 * TODO - daemonize, add in reading of config file,
 *        block sites within a certain time limit.
 */
const char *blockString = "0.0.0.0 ";

FILE *fopenHostsFile(int mode)
{
    switch(mode) 
    {
        case 0:
        return fopen("/etc/hosts", "r");
        case 1:
        return fopen("/etc/hosts", "a");
    }
}

/**
 * Block a hosts using the hostsFile in FILE
 */
void blockHost(FILE *hostsFile, char *host)
{
    // Buffer to hold our block rule.
    char blockRule[256];

    // Copy the blockstring and host to the blockrule.
    strcpy(blockRule, blockString);
    strcat(blockRule, host);
    strcat(blockRule, "\n");

    // Add a null terminator..
    blockRule[strlen(blockRule)] = 0;
    fputs(blockRule, hostsFile);
}

void usage()
{
    fprintf(stdout, "Usage: ");
    fprintf(stdout, "hb [add] <sitename>\n");
}

void showHosts()
{
    pid_t child = fork();
    int rc = 0;

    char *const parmList[] = {"/bin/cat", "/etc/hosts", NULL};

    if (child == 0)
    {
        execv("/bin/cat", parmList);
    }
    else
    {
        rc = wait(NULL);
    }
}

void readToHost(char *host, FILE *hostsFile)
{
  char buf[256];
  char *ptr;

  while ((fgets(buf, 256, hostsFile)) != NULL)
    {
      ptr = strcasestr(host, buf);
      if (ptr != NULL) {
        break;
        printf("FOUND IT\n");
      }
    }
}

/**
 * Entrypoint.
 */
int main(int argc, char **argv)
{
    FILE *hostsFile = NULL;

    pid_t pid;

    pid = fork();

    if (pid > 0) {
      return 0;
    }


    /**
    if (getuid() != 0)
    {
        fprintf(stderr, "hb: Must run as root using sudo!\n");
    }

    for (int i = 0; i < argc; i++) 
    {
        if (strcmp(argv[i], "add") == 0) 
        {
            hostsFile = fopenHostsFile(1);
            blockHost(hostsFile, argv[i+1]);
        }
        else if (strcmp(argv[i], "edit") == 0)
          {
            readToHost(argv[i+1], hostsFile);
          }
        else if (strcmp(argv[i], "delete") == 0)
        {
            fprintf(stdout, "Soon to be implemented!\n");
        }
        else if (strcmp(argv[i], "-h") == 0)
        {
            usage();
            exit(0);
        }
        else if (strcmp(argv[i], "show") == 0)
        {
            showHosts();
        }
    }

    if (hostsFile != NULL)
        fclose(hostsFile);
    **/
}
