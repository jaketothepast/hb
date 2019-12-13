#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Macro for status checks.
#define ONFAILED(status, fn) if(status > fn)

/**
 * TODO: Add config file reading for daemonization.
 * TODO: Allow daemonized process to call file reading/editing routines at will.
 * TODO: Generalized routine for reading through a file and replacing a line.
 * TODO: Generalized routine for reading a file.
 */

// Our rule we use to blackhole domains
const char *blockString = "0.0.0.0 ";

// The current hardcoded location of the hosts file.
static const char *HOSTFILE = "/etc/hosts";

/**
 * SIGINT handler, cleans up resources on Ctrl-C
 * @param signal
 */
void int_handler(int signal) {
    // Clean up any resources.
    fprintf(stderr, "SIGINT received, cleaning up...\n");
}

void replacehost(char *oldhost, char *newhost, FILE *hostsFile);

/**
 * Return an open handle to the hosts file.
 *
 * @param mode whichever mode host file you want to open
 * @return FILE *
 */
FILE *fopenHostsFile(int mode)
{
    switch(mode)
    {
        case 0:
            return fopen(HOSTFILE, "r");
        case 1:
            return fopen(HOSTFILE, "a");
        case 2:
            return fopen(HOSTFILE, "r+");

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
  char *ptr, *f;

  printf("Starting to read!\n");
  printf("Looking for host:%s\n", host);
  fseek(hostsFile, 0, SEEK_SET);
  while (fgets(buf, 256, hostsFile) != NULL)
    {
      /**
      printf("%s\n", ptr);
      printf("%s\n", host);
      **/

      printf("Reading!\n");
      f = strcasestr(buf, host);
      if (f != NULL) {
        printf("FOUND IT\n");
        break;
      }
      memset(buf, 0, sizeof(buf));
    }
}

/**
 * Daemonize the process, to be run like this ONLY if there were no arguments
 * provided.
 *
 * TODO:
 * read the config file.
 */
void daemonize() {
    pid_t mypid = fork();
    if (mypid != 0) {
        // I am the parent, kill myself
        fprintf(stderr, "Parent exiting");
        exit(1);
    }

    else {
        // DO THE THING.
    }
}

/**
 * Entrypoint.
 */
int main(int argc, char **argv)
{
    // Install a sigint handler to help us clean up.
    signal(SIGINT, int_handler);

    FILE *hostsFile;
    if (getuid() != 0)
    {
        fprintf(stderr, "hb: Must run as root using sudo!\n");
    }

    // Process our command line arguments.
    for (int i = 0; i < argc; i++)
    {
        // Opens a hosts file in append mode, adding a host.
        if (strcmp(argv[i], "add") == 0) 
        {
	        if (argc < 3) {
                printf("Please provide a host!\n");
		        exit(1);
	        }
            hostsFile = fopenHostsFile(1);
            blockHost(hostsFile, argv[i+1]);
        }
        // Replaces a host
        // TODO: this currently duplicates the whole file and appends it to the end.
        else if (strcmp(argv[i], "edit") == 0)
          {
            hostsFile = fopenHostsFile(2);
            replacehost(argv[i+1], argv[i+2], hostsFile);
          }
        // Deletes a host.
        else if (strcmp(argv[i], "delete") == 0)
        {
            fprintf(stdout, "Soon to be implemented!\n");
        }
        // Shows usage.
        else if (strcmp(argv[i], "-h") == 0)
        {
            usage();
            exit(0);
        }
        // Show the entire hosts file.
        else if (strcmp(argv[i], "show") == 0)
        {
            showHosts();
        }
        // Daemonize this process, allowing for hosts file to be automagically managed.
        else {
//            daemonize();
        }
    }

    if (hostsFile != NULL)
        fclose(hostsFile);
}

/**
 * Replace a host in the hosts file with the NEW host.
 *
 * @param oldhost
 * @param newhost
 * @param hostsFile
 */
void replacehost(char *oldhost, char *newhost, FILE *hostsFile) {
    char buf[256];
    char *ptr, *f;

    char newHostsFile[4096];
    memset(newHostsFile, 0, 4096);

    printf("Starting to read!\n");
    printf("Looking for host:%s\n", oldhost);
    fseek(hostsFile, 0, SEEK_SET);
    while (fgets(buf, 256, hostsFile) != NULL)
    {
        /**
        printf("%s\n", ptr);
        printf("%s\n", host);
        **/

        printf("Reading!\n");
        f = strcasestr(buf, oldhost);
        if (f != NULL) {
            printf("FOUND IT\n");
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%s %s\n", blockString, newhost);
            printf("New buf: %s", buf);
        }

        strcat(newHostsFile, buf);
        memset(buf, 0, sizeof(buf));
    }

    // Ensure we have an EOF at the end of the file.
    newHostsFile[strlen(newHostsFile)] = EOF;

    // If this failed, write an error message to stderr
    ONFAILED(0, (fwrite(newHostsFile, sizeof(char), sizeof(newHostsFile), hostsFile))) {
        fprintf(stderr, "Did not write anything!\n");
    }
}
