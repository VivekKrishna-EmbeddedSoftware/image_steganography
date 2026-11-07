#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "common.h"
#include "decode.h"
int main(int argc,char *argv[])
{

    EncodeInfo FileInfo;
    DecodeInfo decodeInfo;
    /*Code-block-1: Operation check and arguments validation
           1. checking the operation type by calling the check_operation_type()
           2. Based on the return type the arguments(files) will be validated using:
                    ->Status read_and_validate_encode_args(*argv[],filename)
    */
   int ret=check_operation_type(argv);     
    if(ret==e_encode)
    {
          printf("Selected encoding\n");
          if(read_and_validate_encode_args_for_encoding(argv,argc,&FileInfo)==e_success)
          {
              printf("INFO: Read and validate encode arg is success\n");
              
              if(do_encoding(&FileInfo)==e_success)
              {
              printf("Encoding is success\n");
              }
              else
              {
               printf("Failed to encode\n");
               return 1;
              }
            
          }
          else
          {
              printf("INFO: Read and validate encode arg is failed\n");
              return 1;
          }
    }
    else if(ret==e_decode)
    {
         printf("Selected decoding\n");
         
         //starting validation of arguments and then reading them into structure
        if(read_and_validate_args_for_decoding(argv,argc,&decodeInfo)==e_success)
          {
              printf("INFO: Read and validate encode arg is success\n");    
         }
          else
          {
              printf("INFO: Read and validate encode arg is failed\n");
              return 1;
          }
              
          //starting decoding process
         if(do_decoding(&decodeInfo)==e_success)
              {
              printf("Decoding is success\n");
              }
        else
        {
               printf("Failed to decode\n");
               return 1;
        }        
    
    }
    else
    {
         printf("Invalid option\n");
         printf("Usage:\nFor Encoding : ./a.out -e beautiful.bmp secret.txt [stego.bmp]\n");
         printf("For Decoding : ./a.out -d stego.bmp output.txt\n");    
    }
    return 0;
}
