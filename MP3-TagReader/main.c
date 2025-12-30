/* 
Name        : S. SRI HEMAMBICA
Date        : 20-11-2025
Project     : MP3 Tag Reader and Editor 
Description : This MP3 Tag Reader & Editor is a console-based application that allows users to 
              view and modify metadata stored inside MP3 files using the ID3v2 tagging format.

              It consists of the following functionalities:

              Viewing Metadata (-v):
                  → Displays all available MP3 metadata such as:
                        • Title
                        • Artist
                        • Album
                        • Year
                        • Genre
                        • Comment
                  → Reads ID3v2 headers and extracts information frame-by-frame.
                  → Parses tag frames like TIT2 (Title), TPE1 (Artist), TALB (Album), TYER (Year), etc.
                  → Ensures correct handling of big-endian frame sizes.

              Editing Metadata (-e):
                  → Allows modification of specific MP3 tag fields.
                  → Supported edit options:
                        -t : Edit Title
                        -a : Edit Artist
                        -A : Edit Album
                        -y : Edit Year
                        -m : Edit Genre
                        -c : Edit Comment
                  → Updates frame size and rewrites the tag without corrupting audio data.
                  → Works safely on the ID3v2 header and leaves MP3 audio content intact.

              Command-Line Arguments (CLA):
                  → Accepts user inputs in the format:
                        For Viewing : ./a.out -v <filename.mp3>
                        For Editing : ./a.out -e <option> "<new_text>" <filename.mp3>
                        For Help    : ./a.out --help
                  → Displays error messages for incorrect or missing arguments.

              Core Concepts Used:
                  → Binary file handling (reading & writing MP3 data)
                  → ID3v2 frame parsing and metadata extraction
                  → Big-endian to little-endian conversion for frame sizes
                  → Structures and modular programming in C
                  → Safe string operations and buffer handling

              Additional Features:
                  → Clean and user-friendly color-coded output using ANSI color macros
                  → Error handling for invalid files, missing tags, and incorrect usage
                  → Modular design with separate files for viewing, editing, and help menus

              This project demonstrates how metadata is stored inside MP3 files and how it can be 
              safely read or modified without affecting the original audio content. It showcases 
              practical use of binary file processing, structured design, and understanding of  
              the ID3v2 tagging standard used in modern media applications.
*/






#include<stdio.h>                     // Standard I/O header for input/output functions
#include<string.h>                   // Header for string operations like strcmp()
#include "MP3.h"                     // Custom header containing MP3 function declarations & macros
int main(int argc, char *argv[])// Main function with command-line arguments
{
    MP3Tag tag={"","","","","",""};// initialising all metadata fields with empty string
    if(argc<2)// Check if minimum arguments are provided
    {
        error_msg();// Display error message
        return 0;// Exit program
    }
    if((strcmp(argv[1],"-v")==0))// Check if user passed -v option for viewing metadata
    {
        if(argc<3||argc>3)
        {
            error_msg();// Display error message
            return 0;// Exit program

        }
        view_mp3(argv[2],&tag);// Call function to view MP3 metadata
        return 0;// Exit after viewing
    }
    else if(strcmp(argv[1],"--help")==0)// Check for --help option
    {
        help_mp3();// Display help information
        return 0; // Exit program

    }
    else if(strcmp(argv[1],"-e")==0)// Check for -e option for editing metadata
    {
        if (argc < 5||argc>5)// Ensure enough arguments are provided for editing
        {
            
            error_msg();// Display error message
            return 0;// Exit program
        }
        edit_mp3(argv[4],argv[2],argv[3]);// Call edit function with field, tag, and value
    }
    else
    {
        printf(YELLOW"Invalid option\n"RESET); // Show invalid option message
        error_msg();// Display error message
        return 0;// Exit program

    }
    return 0;// Successful program termination
}