#include <stdio.h>//
#include <stdlib.h>
#include<unistd.h>
#include <string.h>
#include "mp3.h"

char* get_frame_id(const char* option)//to get which tag to edit based on tag given in CLA
{
    if (strcmp(option, "-t") == 0) return "TIT2";// If user passed -t (title),Return ID3 frame ID for Title
    if (strcmp(option, "-a") == 0) return "TPE1";// If user passed -a (artist),Return ID3 frame ID for Artist
    if (strcmp(option, "-A") == 0) return "TALB";// If user passed -A (album),Return ID3 frame ID for Album
    if (strcmp(option, "-y") == 0) return "TYER";// If user passed -y (year),Return ID3 frame ID for Year
    if (strcmp(option, "-m") == 0) return "TCON";// If user passed -m (genre/content),Return ID3 frame ID for Genre
    if (strcmp(option, "-c") == 0) return "COMM";// If user passed -c (comment),Return ID3 frame ID for Comment
    return NULL;// If no match → invalid option
}


void edit_mp3(const char* filename,const char* option,const char* new_text)//song.mp3 -t/-a/-A/-y "new changable text"
{
    const char* stored_option=get_frame_id(option);// Convert user option (-t/-a/...) to actual ID3 frame
    if(stored_option==NULL)// If invalid option was passed
    {
        printf("Invalid tag given for edit\n");// Show error
        return;//terminate
    }
    else{
        int len=strlen(filename);// Get length of filename string
        // Validate extension: must be at least 4 chars and end with ".mp3"
        if ((len<4)||strcmp(filename + len - 4, ".mp3") != 0)
        {
            printf("Error: Not an MP3 file.\n");// File doesn't end in .mp3
            return;//terminate
        }
        else
        {
            printf("Opening file for edit...\n");
            FILE *fptr=fopen(filename,"rb");//can also use r,Open existing MP3 in binary read mode
            if(fptr==NULL)
            {
                printf("Error: Cannot open file %s\n", filename);
                return;
            }

            FILE *dest_ptr=fopen("dest.mp3","wb");// Open a new file to write edited content
            if(dest_ptr==NULL)
            {
                printf("Error: Cannot open file for edit\n");
                fclose(fptr);// Close original file
                return;
            }
            


            char header[10];
            fread(header,1,10,fptr);//read 1st 10 bytes from file to header

             // to verify that the file contains an ID3 tag which says that its an ID3V2

            if (strncmp(header, "ID3", 3) != 0)
            {
                printf(" No ID3 tag found in file.\n");
                fclose(fptr);
                fclose(dest_ptr);
                return;
            }
             // Write ID3 header to destination file
            if(fwrite(header,10,1,dest_ptr)!=1)
            {
                printf("The header is not read properly\n");
                return;
            }
            
            while(1)// Infinite loop to read each ID3 frames until break
            {
                char frame_id[5]={0};// Infinite loop to read each ID3 frame until break
                int frame_size;// Size of frame data
                char flags[2];// 2-byte flags

                // Read 4 bytes frame ID
                if(fread(frame_id,1,4,fptr)!=4)
                {
                    printf("Error: Unable to read frame_id\n");
                    break;
                }
               // printf("%s read frame_id correctly\n",frame_id);
                if(frame_id[0]==0)// If frame ID is 0, metadata section ended
                {
                    printf("Reached end of metadata\n");
                    break;
                }
                // Read next 4 bytes → frame size
                if(fread(&frame_size,4,1,fptr)!=1)
                {
                    printf("Error: Unable to read frame_size\n");
                    break;

                }
                frame_size=convert_endianess(frame_size);// Convert from big-endian to Little endianness
                //printf("%s read frame_size %d correctly\n",frame_id,frame_size);
                
                if(fread(flags,1,2,fptr)!=2)// Read 2 bytes flags
                {
                    printf("Error: Unable to read flags\n");
                    break;

                }
                //printf("%s read flags correctly\n",frame_id);
                if(frame_size<=0) // Safety check: invalid size
                {
                    printf("%s framesize is 0\n",frame_id);
                    break;
                }
                // Allocate memory to store frame data
                char *data=malloc(frame_size+1);
                if(data==NULL)
                {
                    printf("Memory allocation for storing data is failed for %s\n",frame_id);
                    fclose(fptr);
                    fclose(dest_ptr);
                    return;
                }
                fread(data,1,frame_size,fptr);// Read actual tag data
                data[frame_size]='\0';// Null terminate (for safe string usage)

                if(strcmp(frame_id,stored_option)==0)// If this is the frame we want to edit
                {
                    printf(YELLOW"Editing frame %s with new text \"%s\"\n"RESET,frame_id,new_text);

                    if (strcmp(frame_id, "COMM") == 0)// Special case: COMMENT frame
                    {
                        const char *lang = "eng";  // Default language code = "eng"
                        int desc_len = 1;              // 1 byte for empty short description + null terminator

                        if (frame_size >= 4)// If old data is valid
                        {
                            lang = data + 1; // Skip encoding byte and point to 3-byte language
                        }
                        // Calculate how big the new COMM frame data will be-->1 = encoding byte,language (3 bytes),short description + null terminator,actual comment text
                        int new_size = 1 + 3 + desc_len + strlen(new_text); 
                        // 1 = encoding, 3 = lang, desc_len = desc + '\0', strlen = comment text
                        int new_size_big_endian = convert_endianess(new_size);
                        if(fwrite(frame_id,4,1,dest_ptr)!=1)// Write frame ID
                        {
                            printf("Failed to copy %s frame id into destination\n",frame_id);
                            break;

                        }
                        //printf("Copied %s frameid correctly into destination\n",frame_id);// Write 'COMM'
                        if(fwrite(&new_size_big_endian, 4, 1, dest_ptr)!=1)// Write new frame size
                        {
                            printf("Failed to copy %s frame size into destination\n",frame_id);
                            break;

                        }    // Write frame sizew
                        //printf("Copied %s framesize correctly into destination\n",frame_id);
                        if(fwrite(flags, 2, 1, dest_ptr)!=1)// Write flags
                        {
                            printf("Failed to copy %s frame flags into destination\n",frame_id);
                            break;
                        }   
                        //printf("Copied %s frame flags correctly into destination\n",frame_id);// Write flags
                        fputc(0x00, dest_ptr);             // Encoding = ISO-8859-1 (normal text)
                        fwrite(lang, 1, 3, dest_ptr);  // Write language (usually "eng")
                        fputc(0x00, dest_ptr);             // Null short description (empty)
                        fwrite(new_text, 1, strlen(new_text), dest_ptr); // Write comment text
                        printf(GREEN"New text copied successfully for %s\n"RESET, new_text);


                    }
                    // Normal text frames (TIT2, TPE1, TALB, TYER, TCON)
                    else{
                    int new_size=strlen(new_text)+1;// 1 extra for encoding byte
                    int new_size_big_endian=convert_endianess(new_size);

                    if(fwrite(frame_id,4,1,dest_ptr)!=1)// Write frame ID
                    {
                        printf("Failed to copy %s frame id into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s frameid correctly into destination\n",frame_id);
                    if(fwrite(&new_size_big_endian,4,1,dest_ptr)!=1)// Write new size
                    {
                        printf("Failed to copy %s frame size into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s framesize correctly into destination\n",frame_id);
                    if(fwrite(flags,2,1,dest_ptr)!=1)// Write frame flags
                    {
                        printf("Failed to copy %s frame flags into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s frame flags correctly into destination\n",frame_id);
                    fputc(0x00,dest_ptr); // Text encoding byte
                    fwrite(new_text, 1, strlen(new_text), dest_ptr);// Write the new text
                    printf(GREEN"New text copied successfully for %s\n"RESET,new_text);
                }
                }
                else{
                    if(fwrite(frame_id,4,1,dest_ptr)!=1)// Copy frame ID unchanged
                    {
                        printf("Failed to copy %s frame id into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s frameid correctly into destination\n",frame_id);
                    int frame_size_big_endian=convert_endianess(frame_size);
                    if(fwrite(&frame_size_big_endian,4,1,dest_ptr)!=1)// Copy size
                    {
                        printf("Failed to copy %s frame size into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s framesize correctly into destination\n",frame_id);
                    if(fwrite(flags,2,1,dest_ptr)!=1)// Copy flags
                    {
                        printf("Failed to copy %s frame flags into destination\n",frame_id);
                        break;

                    }
                    //printf("Copied %s frame flags correctly into destination\n",frame_id);
                    fwrite(data, 1,frame_size,dest_ptr);// Copy original data
                    //printf("text copied successfully for %s\n",frame_id);
                }
                free(data);// Free allocated memory

            }

            

            //copy remaining audio data after meta data

            int ch;
            while ((ch = fgetc(fptr)) != EOF)// Read audio data till EOF
            fputc(ch, dest_ptr); // Write same to destination
            printf(YELLOW"All data copied successfully...\n"RESET);
            fclose(fptr);// Close original file
            fclose(dest_ptr);// Close new file
            remove(filename);//song.mp3 del
            rename("dest.mp3", filename);//dest.mp3-->song.mp3 -->rename
            printf(GREEN"Edited successfully\n"RESET);
            


        }

        

    }
    

}


    
