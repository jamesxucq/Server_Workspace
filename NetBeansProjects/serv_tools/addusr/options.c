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

int parse_args(OUT char *user_name, OUT char *password, OUT char *xml_config, int argc, char *argv[]) {
    /* These variables will be used in command-line option parsing */
    /* -- 1 begin --------------------------------------------------------- */
    const int NUM_ARGS = 1; /* Used in checking the number of non-option arguments */
    int opt; /* The return value of getopt() */
    /* -- 1 end ----------------------------------------------------------- */

    /* These variables are ready to get values from option arguments */
    char config[PATH_MAX];
    char uname[MID_TEXT_LEN];
    char paswod[MID_TEXT_LEN];
    memset(config, '\0', PATH_MAX);
    memset(uname, '\0', MID_TEXT_LEN);
    memset(paswod, '\0', MID_TEXT_LEN);

    /* Other variable definitions */
    int index;

    static const char *sopt = "u:p:f:vh";
    /* Parse command-line options */
    /* -- 2 begin -------------------------------------------------------- */
    /* Setting opterr to 0 will disable the "invalid option" error message */
    /* opterr = 0; */
    while ((opt = getopt(argc, argv, sopt)) != -1) {
        switch (opt) {
            case 'u': /* The option has 1 argument */
                strcpy(uname, optarg);
                break;
            case 'p': /* The option has 1 argument */
                strcpy(paswod, optarg);
                break;
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
    } else strcpy(config, AUTH_PATH_DEFAULT);

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
    strcpy(user_name, uname);
printf("user_name:%s\n", user_name);
    strcpy(password, paswod);
printf("password:%s\n", password);
    strcpy(xml_config, config);
printf("xml_config:%s\n", xml_config);
    //printf( "Program terminated normally.\n" );

    return EXIT_SUCCESS;
} /* int main() */
