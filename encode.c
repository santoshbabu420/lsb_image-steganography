
#include <stdio.h>
#include "encode.h"
#include "types.h"
#include<string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
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
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    //src image
        if(strcmp(strstr(argv[2],"."),".bmp") == 0)
        {
           encInfo->src_image_fname = argv[2];
        }
        else
        {
            return e_failure;
        }
        //secret data file
    if(strstr(argv[3],".") )
    {
      strcpy(encInfo->extn_secret_file,strstr(argv[3],".") );
      encInfo->secret_fname=argv[3];
    }
    else
    {
        printf("error in the text file");
    }
    //optional file for output image
    if(argv[4] != NULL)
    {
    if(strcmp(strstr(argv[4],"."),".bmp") == 0)
    {
      encInfo->stego_image_fname=argv[4];
    }
    else
    {
      encInfo->stego_image_fname="stego.bmp";

    }
    }
    else
    {
      encInfo->stego_image_fname="stego.bmp";
    }

    return e_success;
}
Status start_encoding(EncodeInfo *encInfo )
{
    if( open_files(encInfo) == e_success ) //for opening files
    {
        printf("files opned sucessfully\n");

        if ( check_capacity(encInfo) == e_success ) // condition for check capacity
        {
            printf("successfully checked the capacity\n");

            if(copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success) //condition for copy header file
            {
                printf("copied bmp header file sucessfully\n");

                if(encode_magic_string(MAGIC_STRING,encInfo) == e_success) //condition for encode magic string
                {
                    printf("magic string encoded sucesfully\n");

                    if(encode_extn_secret_size(strlen(encInfo->extn_secret_file),encInfo) == e_success) // condition for secret file extn size encoding
                    {
                        printf("secret file extension size encode sucessfully\n");

                        if(encode_secret_file_extn(encInfo->extn_secret_file,encInfo) == e_success)//condition for secret file extn
                        {
                            printf("secret file extension encoded sucessfully\n");

                            if(encode_secret_file_size(encInfo->size_secret_file,encInfo) == e_success) //condition for secret file size
                            {
                                printf("secret file size encoded sucessfully\n");

                                if(encode_secret_file_data(encInfo) == e_success) //condition for secret file encoding
                                {
                                    printf("Secret file data encoded sucessfully \n");

                                    if(copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image) == e_success)//condition for copying remaining data
                                    {
                                        printf("Encoding sucessfully completed\n");
                                    }
                                    else
                                        return e_failure;
                                }
                                else
                                    return e_failure;
                            }
                            else
                                return e_failure;
                        }
                        else
                            return e_failure;
                    }
                    else
                        return e_failure;
                }
                else
                    return e_failure;
            }
            else
                return e_failure;
        }
        else
            return e_failure;
    }
    else
        return e_failure;
    return e_success;
}
//check capacity
Status check_capacity(EncodeInfo *encInfo)
{
  encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
  encInfo-> size_secret_file = get_file_size(encInfo->fptr_secret);
  if( encInfo->image_capacity > ( (2 + 4 + strlen(encInfo->extn_secret_file) + 4 + encInfo->size_secret_file ) * 8) )
  {
          return e_success;
  }
  else
  {
          return e_failure;
  }

}
uint get_file_size( FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    uint tell = ftell(fptr);
    rewind(fptr);
    return tell;
}
/*copy bmp image header */
Status copy_bmp_header(FILE *fptr_src_image,FILE *stego_image_fname)
{
    rewind(fptr_src_image);
    char buffer[54];
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,stego_image_fname);
    return e_success;
}
/* store Magic string */
Status encode_magic_string(char *magic_string,EncodeInfo *encInfo)
{
    char buffer[8];
    for(int i=0;i<2;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(magic_string[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
/* function for encode byte to lsb */
Status encode_byte_to_lsb(char data , char *buffer)
{
    int i,flag;
    for(int i=7;i>=0;i--)
    {
      if( data & (1<<i) )
      {
          flag = 1;
      }
      else
      {
          flag = 0;
      }

      buffer[7-i] = buffer[7-i] & (0xfe);
      buffer[7-i] = buffer[7-i] | (flag<<0);
    }

}
/* funtion for encode size to lsb */
uint encode_size_to_lsb(long size,char *buffer)
{
    int i,flag;
    for(i=31;i>=0;i--)
    {
      if(size & (1<<i) )
      {
          flag = 1;
      }
      else
      {
          flag =0;
      }
      buffer[31-i] = buffer[31-i] & (0xfe);
      buffer[31-i]= buffer[31-i] | (flag<<0);
    }
}
/* function to encode secret file extension*/
Status encode_extn_secret_size(int extn_size,EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(extn_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/* encode secret file extension*/
Status encode_secret_file_extn(char *extn,EncodeInfo *encInfo)
{
    char buffer[8];
    int i;
    int len = strlen(extn);
    for(i=0;i<len;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);
        encode_byte_to_lsb(extn[i],buffer);
        fwrite(buffer,8,1,encInfo->fptr_stego_image);
    }
    return e_success;
}
/* Encode secret file size*/
Status encode_secret_file_size(long file_size,EncodeInfo *encInfo)
{

    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(file_size,buffer);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}
/* encode secret file data */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[8];
    char secret_data[encInfo->size_secret_file]; //charcter from the secret file 
    fread(secret_data,encInfo->size_secret_file,1,encInfo->fptr_secret);
    int i;
    for(i=0;i<encInfo->size_secret_file;i++)
    {
        fread(buffer,8,1,encInfo->fptr_src_image);//i will nead character from secret file 
        encode_byte_to_lsb(secret_data[i],buffer); // to use function 8 charcters data afteer coming bcak encoded information in 
        fwrite(buffer,8,1,encInfo->fptr_stego_image);// to stego image....
    }
    return e_success;
}
/* copy remaining image bytes from src image to stego image after encoding*/
Status copy_remaining_img_data(FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char ch;
    while(fread(&ch,1,1,fptr_src_image) > 0) //just compy the fonr file data to another file data remenaing data 
    {
        fwrite(&ch,1,1,fptr_stego_image);
    }
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);













