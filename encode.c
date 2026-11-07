#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
/* Function Definitions */



Status read_and_validate_encode_args_for_encoding(char *argv[],int argc,EncodeInfo *FileInfo)
{
        if(argc==4||argc==5)//proceeds to validation if there are valid no of args only
        {
              //Checks whether image source file name is valid or not 
              if (strlen(argv[2]) >= 5 &&
              strncmp(argv[2] + strlen(argv[2]) - 4, ".bmp", 4) == 0) 
              {
                FileInfo->src_image_fname=argv[2];
              }
              else
             {
               printf("INFO : Invalid Image Source File name\n");
               return e_failure;
              }
    
             //Checks whether secret text file name is valid or not
             if (strlen(argv[3]) >= 5 &&
             strncmp(argv[3] + strlen(argv[3]) - 4, ".txt", 4) == 0) 
             {
                FileInfo->secret_fname=argv[3];
             }
             else
             {
               printf("INFO : Invalid Secret File name\n");
               return e_failure;
             }
         

             //Checks whether output file arg is given or not and 
             //if not given, prints an info that output file with a default name will be created
             //if given, checks the name is valid or not 
            if(argv[4]!=NULL)
            {
                if (strlen(argv[4]) >= 5  &&
                 strncmp(argv[4] + strlen(argv[4]) - 4, ".bmp", 4) == 0) 
                {
                  FileInfo->stego_image_fname=argv[4];
                  
                }
                else
                {
                  printf("INFO: Invalid Output image file name\n");
                  return e_failure;
                }
            }
            else
            {
               printf("INFO: Output file not mentioned. Creating steged_img.bmp as default\n");
               FileInfo->stego_image_fname="steged_img.bmp";
            }
            return e_success;
        }
        else
        {
            printf("INFO : Invalid number of arguments\n");
            return e_failure;
        }
}

Status open_files_for_encoding(EncodeInfo *fileinf)
{
    
     fileinf->fptr_src_image=fopen(fileinf->src_image_fname,"rb");
     if(fileinf->fptr_src_image==NULL)
     {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",fileinf->src_image_fname);
        return e_failure; 
    }
     
     fileinf->fptr_secret=fopen(fileinf->secret_fname,"rb");
     if(fileinf->fptr_secret==NULL)
     {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",fileinf->secret_fname);
        return e_failure; 
    }
     
     fileinf->fptr_stego_image=fopen(fileinf->stego_image_fname,"wb");
     if(fileinf->fptr_stego_image==NULL)
     {
        perror("fopen");
        fprintf(stderr,"ERROR: Unable to open file %s\n",fileinf->stego_image_fname);
        return e_failure; 
    }
     
     return e_success;
}

Status check_capacity(EncodeInfo *fileinf)
{
    fileinf->image_capacity=get_image_size_for_bmp(fileinf->fptr_src_image);
    fileinf->size_secret_file=get_file_size(fileinf->fptr_secret);
    if((fileinf->image_capacity)>(16+32+32+32+(fileinf->size_secret_file)*8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
    

}

Status copy_bmp_header(FILE *fptr_src_image,FILE *fptr_dest_image)
{
     char str[54];
     //setting the postion of fptr_src_image to first byte,
     //since its size might have changed, while using fseek
     rewind(fptr_src_image);

     //reading the bmp header bytes into the buffer "str" 
     if(fread(str,1,54,fptr_src_image)!=54)
     {
        printf("INFO: Copying of BMP header bytes into buffer failed\n");
        return e_failure;
     }
     
     //Copying the bmp header bytes from the buffer "str" to Output file
     if(fwrite(str,1,54,fptr_dest_image)!=54)
     {
        printf("INFO: Copying of BMP header bytes from buffer to Destination failed\n");
        return e_failure;
     }
     

     return e_success;
}

Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    
    encode_data_to_image(magic_string,strlen(magic_string),encInfo);
    return e_success;
}

Status encode_data_to_image(char *data, int size, EncodeInfo *fileinfo)
{ 
  //here we read 8 bytes into image_data buffer of EncodeInfo structure
  //Encode one byte of multi-byte data
  //then write the data to output image
  //Encoding the data to bytes of stego image copied in image_buffer,
  // byte by byte using encode_byte_to_lsb function
 
  for(int i=0;i<size;i++)
  {   
      fread(fileinfo->image_data,1,8,fileinfo->fptr_src_image);
      encode_byte_to_lsb(data[i],fileinfo->image_data);
      fwrite(fileinfo->image_data,1,8,fileinfo->fptr_stego_image);
  }
  return e_success;
}

Status encode_byte_to_lsb(char data, char *image_data)
{
    //here we are encoding bit by bit of data byte to each byte of image buffer
   for(int i=0;i<8;i++)
   {
    //adding ith bit of a data byte to LSB of ith image byte
    image_data[i]=(image_data[i]&0XFE)|((data>>i)&1);
   }
}

Status encode_size_to_lsb(int size,EncodeInfo *fileinf)
{
    char image_buffer_size[32];
    fread(image_buffer_size,1,32,fileinf->fptr_src_image);

     for(int i=0;i<32;i++)
     {
        image_buffer_size[i]=(image_buffer_size[i]&0XFE)|((size>>i)&1);
     }
    fwrite(image_buffer_size,1,32,fileinf->fptr_stego_image);
    
}

Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //printf("%d\n",size);
  encode_size_to_lsb(size,encInfo);
  return e_success;
}

Status encode_secret_file_extn(char *extn_secret,EncodeInfo *fileinf)
{
    encode_data_to_image(extn_secret,strlen(extn_secret),fileinf);
    return e_success;
}

Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    encode_size_to_lsb(file_size,encInfo);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    
    char data[encInfo->size_secret_file];
    fread(data,1,encInfo->size_secret_file,encInfo->fptr_secret);
    encode_data_to_image(data,encInfo->size_secret_file,encInfo);
    return e_success;
}

Status copy_remaining_img_data(EncodeInfo *encInfo)
{
    int no_of_bytes=54+(encInfo->image_capacity)-ftell(encInfo->fptr_stego_image);
   
    char remaining_bytes[no_of_bytes];
    fread(remaining_bytes,1,no_of_bytes,encInfo->fptr_src_image);
    fwrite(remaining_bytes,1,no_of_bytes,encInfo->fptr_stego_image);
    fclose(encInfo->fptr_stego_image);
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    
    
   return e_success;
}

Status do_encoding(EncodeInfo *fileinf)
{
    //checking whther files are successfully opened or not
    //if not opened successfully program is ended
    //if opened successfully next step is performed
    if(open_files_for_encoding(fileinf)==e_success)
    {
        printf("INFO: Files are opened successfully\n");
       
    }
    else
    {
         printf("INFO: Files are not opened successfully\n");
         return e_failure;
    }
    
    //checking capacity of bmp file
    if(check_capacity(fileinf)==e_success)
    {
        printf("INFO: Secret data can fit in given image file\n");
    }
    else
    {
         printf("INFO: Secret data can not fit in given image file\n");
         return e_failure;
    }
    
    
    //copying bmp header
    if(copy_bmp_header(fileinf->fptr_src_image,fileinf->fptr_stego_image)==e_success)
    {
        printf("INFO: Copied bmp header successfully\n");
    }
    else
    {
         printf("INFO: Failed to copy bmp header\n");
         return e_failure;
    }
    
    //This code block encodes magic string to image bytes and then copies them to output image
    if(encode_magic_string(MAGIC_STRING,fileinf)==e_success)
    {
         printf("Encoded magic string successfully\n");           
    }
    else
    {
         printf("INFO: Failed to encode magic string\n");
         return e_failure;
    }


    //This code block encodes secret file 's extension size

    //copying extension part( for example: .txt), of "secret_fname" from EncodeInfo
    //structure into "extn_secret_file" member of the structure 
    strcpy(fileinf->extn_secret_file,strchr(fileinf->secret_fname,'.'));
    if(encode_secret_file_extn_size(strlen(fileinf->extn_secret_file),fileinf)==e_success)
    {
         printf("Encoded file extension size successfully\n");           
    }
    else
    {
         printf("INFO: Failed to file extension size\n");
         return e_failure;
    }

    //This code block encodes secret file's extension characters into .bmp file
    if(encode_secret_file_extn(fileinf->extn_secret_file,fileinf)==e_success)
    {
         printf("Encoded file extension characters successfully\n");           
    }
    else
    {
         printf("INFO: Failed to encode file extension characters\n");
         return e_failure;
    }

    //This code block encodes secret file's size into .bmp file
    if(encode_secret_file_size(fileinf->size_secret_file,fileinf)==e_success)
    {
         printf("Encoded secret file's size successfully\n");           
    }
    else
    {
         printf("INFO: Failed to encode secret file's size\n");
         return e_failure;
    }
    
    //This code block encodes secret file's data into .bmp file
    if(encode_secret_file_data(fileinf)==e_success)
    {
         printf("Encoded secret file's data successfully\n");           
    }
    else
    {
         printf("INFO: Failed to encode secret file's data\n");
         return e_failure;
    }
    
    //This code block copies remaining bytes of source .bmp file into output .bmp file
    if(copy_remaining_img_data(fileinf)==e_success)
    {
         printf("Copied remaining bytes of source image into output image\n");           
    }
    else
    {
         printf("INFO: Failed to copy remaining bytes of source image \n");
         return e_failure;
    }
    

    return e_success;
}

uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height *3;
}

uint get_file_size(FILE *fptr_secret)
{
    long size;
    fseek(fptr_secret, 0, SEEK_END);
    size = ftell(fptr_secret);
    printf("Secret file size: %lu\n",size);
    rewind(fptr_secret);
    return (uint)size;   
}

