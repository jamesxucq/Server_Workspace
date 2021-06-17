/*
 * options.c
 *
 *  Created on: 2010-3-17
 *      Author: David
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <unistd.h>             /* for getopt() */
#include <getopt.h>

#include "macro.h"
#include "logger.h"

#include "options.h"

/*! Show the version information of the program.*/
void
display_version(void) {
    const char* version = VERSION;
    const char* copyright = COPYRIGHT;
    const char* disclaimer = DISCLAIMER;

    printf("Version %s\n", version);
    printf("%s\n", copyright);
    printf("%s\n", disclaimer);
} /* display_version() */

/*! Show the usage of the program.*/
void
display_usage(const char* program_name) {
    const char* function = FUNCTION;

    printf("Function: %s\n", function);
    printf(USAGE, program_name);
} /* void display_usage() */

int parse_args(OUT char *xml_config, int argc, char *argv[]) {
    /* These variables will be used in command-line option parsing */
    /* -- 1 begin --------------------------------------------------------- */
    const int NUM_ARGS = 1; /* Used in checking the number of non-option arguments */
    int opt; /* The return value of getopt() */
    /* -- 1 end ----------------------------------------------------------- */

    /* These variables are ready to get values from option arguments */
    char config[PATH_MAX];
    memset(&config, '\0', PATH_MAX);

    /* Other variable definitions */
    int index;

    static const char *sopt = "f:vh";
    static const struct option lopt[] = {
        {"file", required_argument, NULL, 'f'},
        {"version", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {NULL, 0, NULL, 0}
    };
    /* Parse command-line options */
    /* -- 2 begin -------------------------------------------------------- */
    /* Setting opterr to 0 will disable the "invalid option" error message */
    /* opterr = 0; */
    while ((opt = getopt_long(argc, argv, sopt, lopt, NULL)) != -1) {
        switch (opt) {
            case 'f': /* The option has 1 argument */
                strcpy(config, optarg);
                break;
            case 'v': /* The option has no argument */
                display_version();
                return EXIT_FAILURE;
                break;
            case 'h': /* help */
            case ':': /* invalid option */
            case '?': /* invalid option */
            default:
                display_usage(argv[0]);
                return EXIT_FAILURE;
                break;
        }
    }
    /* -- 2 end ----------------------------------------------------------- */
    /* Output for verifying user input */
    if (strcmp(config, "")) {
        if (valid_file_path(config)) {
            LOG_INFO("the file name is error! %s", config);
            return EXIT_FAILURE;
        }
    } else strcpy(config, CLEAR_BACKUP_DEFAULT);

    /* -- 3 begin (Optional) ---------------------------------------------- */
    /* After the command-line parsing,
       (argc - optind) is the number of non-option arguments and
       argv[optind] points to the first of them.

       Print all the non-option arguments.
     */
    printf("Number of non-option arguments = %d:\n", argc - optind);
    for (index = optind; index < argc; ++index) {
        printf("\t%s\n", argv[index]);
    }
    /* Show the parsed results */
    /* -- 3 end (Optional) ------------------------------------------------ */

    /* Check the number of the required non-option arguments if necessary.
       You can choose between the strict and loose check,
       or disable both of them without the number check. */
    /* -- 4 begin (Optional) ---------------------------------------------- */

    /* -- 4 end (Optional) ----------------------------------------------- */

    /* ... */
    strcpy(xml_config, config);
    //printf( "Program terminated normally.\n" );

    return EXIT_SUCCESS;
} /* int main() */
