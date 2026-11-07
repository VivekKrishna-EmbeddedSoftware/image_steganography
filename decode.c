#include<stdio.h>
#include"decode.h"
#include<string.h>
#include"types.h"
#include"common.h"


Status read_and_validate_args_for_decoding(char *argv[],int argc,DecodeInfo *decodeInfo)
{
        if(argc==4||argc==3)//proceeds to validation if there are valid no of args only
        {
              //Checks whether stegged image file name is valid or not 
              if (strlen(argv[2]) >= 5 &&
              strncmp(argv[2] + strlen(argv[2]) - 4, ".bmp", 4) == 0) 
              {
                decodeInfo->stegged_image_fname=argv[2];
              }
              else
             {
               printf("INFO : Invalid Stegged Image File name\n");
               return e_failure;
              }
    
             //Checks whether output file argument is given or not and 
             //if not given, prints an info that output file with a default name will be created
             //if given, checks the name is valid or not 
            if(argv[3]!=NULL)
            {
                if (strlen(argv[3]) >= 5  &&
                 strncmp(argv[3] + strlen(argv[3]) - 4, ".txt", 4) == 0) 
                {
                  decodeInfo->output_fname=argv[3];
                }
                else
                {
                  printf("INFO: Invalid Output Text file name\n");
                  return e_failure;
                }
            }
            else
            {
               printf("INFO: Output file not mentioned. Creating 'decoded_file.txt' as default\n");
               decodeInfo->output_fname="decoded_file.txt";
            }

            return e_success;
        }
        else
        {
            printf("INFO : Invalid number of arguments\n");
            return e_failure;
        }

    
}

Status do_decoding(DecodeInfo *decodeInfo)
{
    if(open_files_for_decoding(decodeInfo)==e_success)
     {
        printf("Files are opened successfully\n");
     }
    else
     {
        printf("Failed to open files/file\n");
        return e_failure;
     }

    if(check_magic_string(MAGIC_STRING,decodeInfo)==e_success)
     {
        printf("Found the MAGIC STRING signature in the file\n");
        printf("------DECODING PROCESS STARTED--------\n");
     }
    else
     {
        printf("The MAGIC STRING Signature is not found\n");
        printf("INFO: The file does not have any secret data\n");
        return e_failure;
     }
     
    if(decode_file_extn_size(decodeInfo)==e_success)
     {
        printf("Successfuly decoded extension size|  Size = 4\n");
     }
    else
     {
        printf("Failed to decode extension size\n");
        return e_failure;
     }

    if(decode_file_extn_string(decodeInfo)==e_success)
     {
        printf("Successfully decoded extension string | %s \n",decodeInfo->extn_string);
     }
    else
     {
         printf("Failed to decode extension string\n");
         return e_failure;
     }
     
    if(decode_file_size(decodeInfo)==e_success)
     {
        printf("Successfully decoded file size | File size= %d \n",decodeInfo->secret_file_size);
     }
    else
     {
         printf("Failed to decode File size\n");
         return e_failure;
     }
    
     if(decode_file_content(decodeInfo)==e_success)
     {
        printf("Successfully decoded file content | check the file %s\n",decodeInfo->output_fname);
     }
    else
     {
         printf("Failed to decode File content\n");
         return e_failure;
     }
    

     return e_success;
}

Status open_files_for_decoding(DecodeInfo *decodeInfo)
{
    //opening stegged file in read binary mode
    decodeInfo->fptr_stegged_image=fopen(decodeInfo->stegged_image_fname,"rb");
    if(decodeInfo->fptr_stegged_image==NULL)
    {
        perror("fopen");
        fprintf(stderr,"Error: Unable to open file %s\n",decodeInfo->stegged_image_fname);
        return e_failure;
    }
    
     //opening output file in write mode
    decodeInfo->fptr_output_file=fopen(decodeInfo->output_fname,"w");
    if(decodeInfo->fptr_output_file==NULL)
    {
        perror("fopen");
        fprintf(stderr,"Error: Unable to open file %s\n",decodeInfo->stegged_image_fname);
        return e_failure;
    }

    return e_success;
}

Status check_magic_string(char *magic_string,DecodeInfo *decodeInfo)
{
    //obtaining the length of magic string to be checked
    int size=strlen(magic_string);

    //setting the pointer of stegged image file, to 54th byte since information is encoded from 54th byte
    fseek(decodeInfo->fptr_stegged_image,54,SEEK_SET);
    
    //Stores decoded magic string for comparison
    char decoded_magic_string[size+1];//stores the decoded magic string for comparison

    //Decodes the the image bytes and stores the magic string in "decoded_magic_string" 
    decode_data_from_image(decoded_magic_string,size,decodeInfo);

    printf("Magic string we are looking for: %s\n",magic_string);
    printf("Decoded magic string = %s\n",decoded_magic_string);

    if(strcmp(decoded_magic_string,magic_string)!=0)
    {
        return e_failure;
    }

   return e_success;
}

Status decode_file_extn_size(DecodeInfo *decodeInfo)
{

    char image_buffer_32[32];
    
    if(fread(image_buffer_32,1,32,decodeInfo->fptr_stegged_image)!=32)
    {
    printf("Error: while reading 32 bytes from stegged image file\n");
    return e_failure;    
    }
    
   
    //decoding size and storing in extn_size member of decodeInfo structure variable
    decode_size_from_image_data(&(decodeInfo->extn_size),image_buffer_32);
     
    return e_success;
}

Status decode_file_extn_string(DecodeInfo *decodeInfo)
{
    decode_data_from_image(decodeInfo->extn_string,decodeInfo->extn_size,decodeInfo);
     return e_success;
}

Status decode_file_size(DecodeInfo *decodeInfo)
{
   
    char image_buffer_32[32];
    
    if(fread(image_buffer_32,1,32,decodeInfo->fptr_stegged_image)!=32)
    {
    printf("Error: while reading 32 bytes from stegged image file\n");
    return e_failure;    
    }
    
   
    //decoding size and storing in extn_size member of decodeInfo structure variable
    decode_size_from_image_data(&(decodeInfo->secret_file_size),image_buffer_32);
     
    return e_success;
}

Status decode_file_content(DecodeInfo *decodeInfo)
{
    char secret_file_content[decodeInfo->secret_file_size];
   decode_data_from_image(secret_file_content,decodeInfo->secret_file_size,decodeInfo);
   
   //writing the decoded content to output file
   fwrite(secret_file_content,1,decodeInfo->secret_file_size,decodeInfo->fptr_output_file);
   return e_success;
}

 Status decode_data_from_image(char *decoded_data_buffer,int size,DecodeInfo *decodeInfo)
{
    for(int i=0;i<size;i++)
    {
        //this statement gets 8 bytes from stegged image file into "image_data_buffer"
        if(fread(decodeInfo->image_data_buffer,1,8,decodeInfo->fptr_stegged_image)!=8)
        {
            printf("Error: could not read the required no of bytes for decoding\n");
            return e_failure;
        }

        //this statement decodes 1 byte from 8 bytes of image data, and stores in "decoded_magic_string"
        decode_byte_from_8_bytes(decoded_data_buffer,i,decodeInfo->image_data_buffer);

    }
    decoded_data_buffer[size]='\0';
}

Status decode_byte_from_8_bytes(char *decoded_data_buffer,int i,char *eight_bytes_buff)
{
    decoded_data_buffer[i]=0;
   for(int j=0;j<8;j++)
   {
      decoded_data_buffer[i]=(decoded_data_buffer[i])|((eight_bytes_buff[j]&(1))<<j);
   }

}

//in this size is the argument in which the decoded size is stored
Status decode_size_from_image_data(int *size,char *image_buffer_32)
{
    //decodes size from 32 bytes
    *size=0;
 for(int i=0;i<32;i++)
      {
        *size = (*size)|((image_buffer_32[i]&1)<<i);
      }
}