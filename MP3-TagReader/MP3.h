// view.h
#ifndef MP3_H
#define MP3_H
// Structure to store extracted MP3 metadata fields
typedef struct MP3{
    char title[128];
    char artist[128];
    char album[128];
    char year[10];
    char genre[128];
    char comment[128];
} MP3Tag;


// Color codes for terminal output
#define RESET   "\x1b[0m"   
#define BLACK   "\x1b[30m"
#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define WHITE   "\x1b[37m"

// Converts big-endian to little-endian for frame size
int convert_endianess(int num);
// Prints error message for invalid input or file issues
void error_msg();
// Reads and displays MP3 tag information
void view_mp3(const char *filename,MP3Tag *tag);
// Edits a specific MP3 tag field with new text
void edit_mp3(const char*filename,const char *option,const char *newtext);
// Displays supported commands/options for the program
void help_mp3();

#endif
