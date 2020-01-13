#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "linkedlist.h"

// Macro for status checks.
#define ONFAILED(status, fn) if(status > fn)

#define ARG_IS(argname) (strcmp(argv[i], argname) == 0)

/**
 * TODO: Add config file reading for daemonization.
 * TODO: Allow daemonized process to call file reading/editing routines at will.
 * TODO: Generalized routine for reading through a file and replacing a line.
 * TODO: Generalized routine for reading a file.
 */

/** GLOBALS **/
// Our rule we use to blackhole domains
const char *blockString = "0.0.0.0 ";
LinkedList *hosts = NULL;
int HB_PERIOD = 60;

// The current hardcoded location of the hosts file.
static char *HOSTFILE = "/etc/hosts";
static char *CONFIG;
// Our configuration

/**
 * SIGINT handler, cleans up resources on Ctrl-C
 * @param signal
 */
void int_handler(int signal) {
    // Clean up any resources.
    fprintf(stderr, "SIGINT received, cleaning up...\n");

    // This should be the final decrement to the config object.
}

/**
 * SIGHUP handler, will reload the configuration file.
 * @param signal Signo passed by system.
 */
void hup_handler(int signal) {

}

void replacehost(char *oldhost, char *newhost);

/**
 * Return an open handle to the hosts file.
 *
 * @param mode whichever mode host file you want to open
 * @return FILE *
 */
FILE *fopenHostsFile(char *mode)
{
    return fopen(HOSTFILE, mode);
}

/**
 * Block a hosts using the hostsFile in FILE
 */
void blockHost(char *host)
{
    FILE *hostsFile = fopenHostsFile("a");
    char blockRule[256];
    sprintf(blockRule, "%s %s\n", blockString, host);
    blockRule[strlen(blockRule)] = 0;

    ONFAILED(EOF, (fputs(blockRule, hostsFile))) {
        fprintf(stderr, "Failed to write block rule to file\n");
    }

    fclose(hostsFile);
}

void usage()
{
    fprintf(stdout, "Usage: ");
    fprintf(stdout, "hb [add] <sitename>\n");
    fprintf(stdout, "hb [edit] <oldsite> <newsite>\n");
    fprintf(stdout, "hb [show]\n");
    fprintf(stdout, "hb [delete] <sitename>\n");
}

void showHosts()
{
    pid_t child = fork();
    int rc = 0;

    char *const parmList[] = {"/bin/cat", HOSTFILE, NULL};

    if (child == 0)
    {
        execv("/bin/cat", parmList);
    }
    else
    {
        rc = wait(NULL);
    }
}

/**
 * Read the configuration file and give a return code to indicate any changes.
 * TODO: overwrites hosts, but doesn't handle if the list shrinks.
 * @return 1 if successful, 0 if otherwise.
 */
int read_config_file() {
    // increment the refcount for tmp.
    FILE *config = fopen(CONFIG, "r");
    LinkedList *ptr = hosts;

    char buf[1024];

    if (config == NULL) {
        fprintf(stderr, "Error trying to open config file: %s\n", strerror(errno));
    }
    else {
        // Add each line from the file to the linked list.
        while (fgets(buf, 1024, config) != NULL) {

            buf[strlen(buf) - 1] = 0;

            // We should add a method that overwrites the linked list node at an index.
            fprintf(stderr, "Read %s from config file\n", buf);

            // overwrite data in these nodes.
            if (ptr != NULL && strcmp(buf, ptr->data) != 0) {
                if (strlen(ptr->data) < strlen(buf)) {
                    if ((ptr->data = realloc(ptr->data, strlen(buf))) == NULL) {
                        fprintf(stderr, "Failed to realloc pointer\n");
                        exit(1);
                    }
                }
                strcpy(ptr->data, buf);
                ptr = ptr->next;
            } else {
                linkedlist_add(&hosts, buf);
            }
        }
    }
}

/**
 * Wake up on SIGALRM to do our thing.
 */
void run_loop(int signal) {
    read_config_file();

    alarm(HB_PERIOD);
}

/**
 * Entrypoint.
 */
int main(int argc, char **argv)
{
    signal(SIGINT, int_handler);
    if (getuid() != 0)
    {
        fprintf(stderr, "hb: Must run as root using sudo!\n");
    }

    // Process our command line arguments.
    for (int i = 0; i < argc; i++)
    {
        // Opens a hosts file in append mode, adding a host.
        if (ARG_IS("add"))
        {
	        if (argc < 3) {
                printf("Please provide a host!\n");
		        exit(1);
	        }
            blockHost(argv[++i]);
        }
        // Replaces a host
        // TODO: this currently duplicates the whole file and appends it to the end.
        else if (ARG_IS("edit"))
        {
            replacehost(argv[++i], argv[++i]);
        }
        // Deletes a host.
        else if (ARG_IS("delete"))
        {
            fprintf(stdout, "Soon to be implemented!\n");
        }
        // Shows usage.
        else if (ARG_IS("-h"))
        {
            usage();
            exit(0);
        }
        // Show the entire hosts file.
        else if (ARG_IS("show"))
        {
            showHosts();
        }
        else if (ARG_IS("-config")) {
            CONFIG = argv[++i];
            read_config_file();
        }
        else if (ARG_IS("-period")) {
            HB_PERIOD = atoi(argv[++i]);
        }

        // Daemonize this process, allowing for hosts file to be automagically managed.
        else {
//            daemonize();
        }
    }

    free_list(&hosts);
}

/**
 * Replace a host in the hosts file with the NEW host.
 *
 * @param oldhost
 * @param newhost
 * @param hostsFile
 */
void replacehost(char *oldhost, char *newhost) {
    FILE *hostsFile = fopenHostsFile("w+");
    char buf[256];
    char *ptr, *f;

    char newHostsFile[4096];
    memset(newHostsFile, 0, 4096);

    printf("Starting to read!\n");
    printf("Looking for host:%s\n", oldhost);
    fseek(hostsFile, 0, SEEK_SET);

    while (fgets(buf, 256, hostsFile) != NULL)
    {
        f = strcasestr(buf, oldhost);
        if (f != NULL) {
            memset(buf, 0, sizeof(buf));
            sprintf(buf, "%s %s\n", blockString, newhost);
        }

        strcat(newHostsFile, buf);
        memset(buf, 0, sizeof(buf));
    }

    // Ensure we have an EOF at the end of the file.
    newHostsFile[strlen(newHostsFile)] = EOF;

    // Seek to 0
    fseek(hostsFile, 0, SEEK_SET);

    // If this failed, write an error message to stderr
    ONFAILED(0, (fwrite(newHostsFile, sizeof(char), sizeof(newHostsFile), hostsFile))) {
        fprintf(stderr, "Did not write anything!\n");
    }
    fclose(hostsFile);
}
