#define _GNU_SOURCE
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <json_tokener.h>

// Macro for status checks.
#define ONFAILED(status, fn) if(status > fn)

/**
 * TODO: Add config file reading for daemonization.
 * TODO: Allow daemonized process to call file reading/editing routines at will.
 * TODO: Generalized routine for reading through a file and replacing a line.
 * TODO: Generalized routine for reading a file.
 */

/** GLOBALS **/
// Our rule we use to blackhole domains
const char *blockString = "0.0.0.0 ";
// The current hardcoded location of the hosts file.
static const char *HOSTFILE = "/etc/hosts";
// Our configuration
struct json_object *config;

/**
 * SIGINT handler, cleans up resources on Ctrl-C
 * @param signal
 */
void int_handler(int signal) {
    // Clean up any resources.
    fprintf(stderr, "SIGINT received, cleaning up...\n");
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
 * @return 1 if successful, 0 if otherwise.
 */
int read_config_file() {
    config = json_tokener_parse(CONFIG);
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
        while(1) {
            // read config
            read_config_file();
            // make adjustments

            // profit??
            // JK, sleep.
        }
    }
}

/**
 * Entrypoint.
 */
int main(int argc, char **argv)
{
    // Install a sigint handler to help us clean up.
    signal(SIGINT, int_handler);

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
            blockHost(argv[i+1]);
        }
        // Replaces a host
        // TODO: this currently duplicates the whole file and appends it to the end.
        else if (strcmp(argv[i], "edit") == 0)
        {
            replacehost(argv[i+1], argv[i+2]);
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
