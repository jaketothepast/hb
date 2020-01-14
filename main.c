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
 * @return 1 if successful, 0 if otherwise.
 */
int read_config_file() {
    // increment the refcount for tmp.
    FILE *config = fopen(CONFIG, "r");
    LinkedList *ptr = hosts, *prev = NULL;

    char buf[1024];

    if (config == NULL) {
        fprintf(stderr, "Error trying to open config file: %s\n", strerror(errno));
        fclose(config);
        return 0;
    }
    else {
        // Add each line from the file to the linked list.
        while (fgets(buf, 1024, config) != NULL) {

            buf[strlen(buf) - 1] = 0;

            // overwrite data in these nodes.
            if (ptr != NULL) {
                if (strlen(ptr->data) < strlen(buf)) {
                    if ((ptr->data = realloc(ptr->data, strlen(buf))) == NULL) {
                        fprintf(stderr, "Failed to realloc pointer\n");
                        return 0;
                    }
                }
                strcpy(ptr->data, buf);
                prev = ptr;
                ptr = ptr->next;
            } else {
                linkedlist_add(&hosts, buf);
            }
        }

        // In this case, the list got chopped off, we need to reset pointers and free the rest of our list.
        if (ptr != NULL) {
            prev->next = NULL;
        }

        // If we didn't exhaust the linked list, then we need to free any remaining nodes because the list
        // shortened.
        while (ptr != NULL) {
            prev = ptr;
            ptr = ptr->next;
            free(prev->data);
            free(prev);
        }
        fclose(config);
        return 1;
    }
}

/**
 * Wake up on SIGALRM to do our thing.
 */
void run_loop() {
    sigset_t sigset;

    sigemptyset(&sigset);
    sigaddset(&sigset, SIGALRM);
    sigaddset(&sigset, SIGINT);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

    int signo;
    int stat;

    alarm(HB_PERIOD);
    for (;;){
        stat = sigwait(&sigset, &signo);
        if (stat == EINVAL) {
            fprintf(stderr, "We have an invalid signo\n");
        }

        if (signo == SIGINT) {
            break;
        }
        else if (signo == SIGALRM) {
            fprintf(stderr, "Waking up and reading config..\n");
            read_config_file();
            alarm(HB_PERIOD);
        }
    }
}

void usage()
{
    fprintf(stdout, "Usage: ");
    fprintf(stdout, "hb [add] <sitename>\n");
    fprintf(stdout, "hb [edit] <oldsite> <newsite>\n");
    fprintf(stdout, "hb [show]\n");
    fprintf(stdout, "hb [delete] <sitename>\n");
    fprintf(stdout, "hb -period <wakeup period> -config <config file> -daemon\n");
}

/**
 * Entrypoint.
 */
int main(int argc, char **argv)
{
    int run_as_daemon = 0;
    if (getuid() != 0)
    {
        fprintf(stderr, "hb: Must run as root using sudo!\n");
        exit(1);
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
        }
        else if (ARG_IS("-period")) {
            HB_PERIOD = atoi(argv[++i]);
        }
        else if (ARG_IS("-daemon")) {
            run_as_daemon = 1;
        }

    }

    if (run_as_daemon) {
        run_loop();
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
