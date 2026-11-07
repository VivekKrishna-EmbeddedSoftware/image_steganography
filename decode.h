#ifndef DECODE_H
#define DECODE_H
#include "types.h"

/*DecodeInfo structure group file buffers,file names,pointers, and decoded data at one place*/
typedef struct 
{
    //steged image details
    char *stegged_image_fname;
    FILE *fptr_stegged_image;

    //Output file details (Output file is the one that stores the decoded data)
    char *output_fname;
    FILE *fptr_output_file;
     
    //meta data of the secret file encoded in the image
    int extn_size;
    char extn_string[6];
    int secret_file_size;

    //buffer to store the bytes of image file, for decoding
    //stores 8 bytes of image file
    char image_data_buffer[8];


    
}DecodeInfo;

/*---FILE DECLARATIONS--*/

Status read_and_validate_args_for_decoding(char *argv[],int argc,DecodeInfo *decodeInfo);

Status do_decoding(DecodeInfo *decodeInfo);

Status open_files_for_decoding(DecodeInfo *decodeInfo);

Status check_magic_string(char *magic_string,DecodeInfo *decodeInfo);

Status decode_file_extn_size(DecodeInfo *decodeInfo);

Status decode_file_extn_string(DecodeInfo *decodeInfo);

Status decode_file_size(DecodeInfo *decodeInfo);

Status decode_file_content(DecodeInfo *decodeInfo);

Status decode_data_from_image(char *decoded_data_buffer,int size,DecodeInfo *decodeInfo);

Status decode_byte_from_8_bytes(char *decoded_data_buffer,int i,char *eight_bytes_buff);

Status decode_size_from_image_data(int *size,char *image_buffer_32);
#endif