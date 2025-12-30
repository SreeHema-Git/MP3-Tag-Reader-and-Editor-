#include<stdio.h>
#include"MP3.h"



//function to print help msg
void help_mp3()
{
    printf("--------------------------------------------------\n");
    printf(MAGENTA"                  HELP MENU\n"RESET);
    printf("--------------------------------------------------\n");
    printf(CYAN"Usage:\n"RESET);
    printf(CYAN"  ./a.out [OPTION] [ARGUMENTS]\n\n");

    printf("OPTIONS:\n");
    printf("  -v <filename.mp3>        View MP3 file contents\n");
    printf("  -e <edit_option> \"<text>\" <filename.mp3>\n");
    printf("                           Edit MP3 file contents\n\n");

    printf("EDIT OPTIONS:\n");
    printf("   -t   Edit song title\n");
    printf("   -a   Edit artist name\n");
    printf("   -A   Edit album name\n");
    printf("   -y   Edit year\n");
    printf("   -m   Edit genre/content\n");
    printf("   -c   Edit comment\n");

    printf("--------------------------------------------------\n");
}
//function to print error message
void error_msg()
{
    printf("-------------------------------------------------------\n");
        printf("\n");
        printf(RED"ERROR : INVALID ARGUMENTS\n"RESET);
        printf("USUAGE:\n");
        printf("To view please pass like: ./a.out -v mp3filename\n");
        printf("To edit please pass like: ./a.out -e -t/-a/-A/-m/-y/-c \"changing_text\" mp3filename\n");
        printf("To get help please pass like: ./a.out --help\n");
        printf("-------------------------------------------------------\n");
        return;
}
