#include<stdio.h>// Standard I/O functions
#include<stdlib.h>// For malloc(), free()
#include<string.h>// For strcmp(), strlen(), strcpy()
#include "MP3.h"// Custom header with structures, macros, prototypes


// Converting 4-byte integer from Big Endian → Little Endian or Little Endian → Big Endian since we perform swapping it works fine for both
 int convert_endianess(int num)
{
    char *ptr=(char *)&num;
    char temp;
    for(int i=0;i<4/2;i++)// Swap first byte with last, second with third
    {
        temp=ptr[i];
        ptr[i]=ptr[4-i-1];
        ptr[4-i-1]=temp; 
    }
    return num;// Return converted value
}

void view_mp3(const char *filename,MP3Tag *tag)
{
    
    int len=strlen(filename);// Get filename length
    if (strcmp(filename + len - 4, ".mp3") != 0)// Check file extension is .mp3
    {
        printf(RED"Error: Not an MP3 file.\n"RESET);
        return ;
    }
    FILE *fp=fopen(filename,"rb");//to open file in read binary mode since ourfile is not .txt/.csv we are using rb
    if(fp==NULL)
    {
        printf(RED"Error: Cannot open file %s\n"RESET, filename);
        return;
    }

    char header[10];// ID3 header is 10 bytes to store that we need header
    fread(header,1,10,fp);//read 1st 10 bytes from file to header

    // to verify that the file contains an ID3 tag which says that its an ID3V2

    if (strncmp(header, "ID3", 3) != 0)
    {
        printf(RED"Error:No ID3 tag found in file.\n"RESET);
        fclose(fp);
        return;
    }
    // Loop to read frames until end of metadata section
    // to read all ID3 frames
    while(1)
    {
        char frame_id[5]={0};// Frame ID (4 bytes + null character at end)
        int frame_size; // Frame size (4 bytes)
        char flags[2]; // Frame flags (2 bytes)

        // Read frame ID
        if(fread(frame_id,1,4,fp)!=4)// If less than 4 bytes read, stop
            break;
        if(frame_id[0]==0)// Stop if frame ID is empty (end of frames) if we completed meta data there will be padding which comprises of 00000 so it fails here and terminates out of loop
            break;
        // Read frame size (4 bytes) directly into int
        fread(&frame_size, 4, 1, fp);
        // Swap bytes to convert big-endian to little-endian
        frame_size = convert_endianess(frame_size);
        // Read frame flags (2 bytes, can be ignored)
        fread(flags, 1, 2, fp);
        if (frame_size <= 0) // If size invalid, stop reading
            break;

        char *data=malloc(frame_size +1); //  buffer for frame data storing
        if (data == NULL)
        {
            printf(RED"Error:Memory not allocated to store %s data\n"RESET,frame_id);
            fclose(fp);
              return;
        }
        fread(data, 1, frame_size, fp);// reading frame data into data location
        data[frame_size] = '\0';// Add null terminator

        // First byte is text encoding (ISO standards(english,latin) can be UTF also) so skipping that
        char *text = data + 1;

        // Match frame IDs and store data in structure
        if (strcmp(frame_id, "TIT2") == 0)// Song title
        {
            strcpy(tag->title, text);//storing into structure member
        }
        else if (strcmp(frame_id, "TPE1") == 0)// Artist
        {
            strcpy(tag->artist, text);//storing into structure member
            
        }
        else if (strcmp(frame_id, "TALB") == 0)// Album
        {
            strcpy(tag->album, text);//storing into structure member
            
        }
        else if (strcmp(frame_id, "TYER") == 0)// Year
        {
            strcpy(tag->year, text);//storing into structure member
           
        }
        else if (strcmp(frame_id, "TCON") == 0)// Genre
        {
            strcpy(tag->genre, text);//storing into structure member
            
        }
        else if (strcmp(frame_id, "COMM") == 0)// Comment frame
        {
            //since COMM has lang+\0+short description+\0+actual data...wee need to skip until short description \0
            text=text+3;//skips eng+\0
            while(*(text)!='\0')//skips short description
            {
                text++;
            }
            text++;//skips \0 followed by short description
            strcpy(tag->comment, text);//storing into structure member
            
        }
        else
        {
            // Unknown frame, skiping this
        }

        free(data);// Free allocated memory
        data=NULL;//prevents from dangling pointer
        //loop ends after reading all
    }
    fclose(fp);// Close file after reading

    
     // Print extracted metadata
    printf(CYAN"\n----- MP3 Metadata -----\n"RESET);
    printf("Title   : %s\n", tag->title);
    printf("Artist  : %s\n", tag->artist);
    printf("Album   : %s\n", tag->album);
    printf("Year    : %s\n", tag->year);
    printf("Genre   : %s\n", tag->genre);
    printf("Comment : %s\n", tag->comment);
    printf(CYAN"-------------------------\n"RESET );

}


