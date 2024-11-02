/*
Name:
Date:
Description:
Sample i/p:
Sample o/p:
*/
/*
NAME:
DATE:
DESCRIPTION:
SAMPLE I/P:
SAMPLE O/P:
 */
#include<stdio.h>
#include"decode.h"
#include"types.h"
#include<string.h>
#include"common.h"
Status read_and_validate_decode_argc( char *argv[] , DecodeInfo *decInfo)
{
    if(argv[2] != NULL)
    {
	if(strcmp(strstr(argv[2],"."),".bmp") == 0)
	{
	    strcpy(decInfo->stego,argv[2]);
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	return e_failure;
    }
    if(argv[3] != NULL)
    {
	if(strcmp(strstr(argv[3],"."),".txt") == 0)
	{
	    strcpy(decInfo->text_file,argv[3]);
	}
	else
	{
	    strcpy(decInfo->text_file,"text_data");
	}
    }
    else
    {
	strcpy(decInfo->text_file,"text_data");
    }

    return e_success;
}
Status do_decoding( DecodeInfo *decInfo)
{
    if(open_files_decode(decInfo) == e_success)
    {
	puts("files opened successfully ");

	if(decode_magic_string(decInfo) == e_success) 
	{
	    puts("magic string decoded successfully");

	    if(decode_extn_size(decInfo)==e_success)
	    {
		puts("extension size sucessfully completed");

		if(decode_extension(decInfo,decInfo->extn_size) == e_success)
		{
		    puts("extension completed");

		    if(decode_secret_file_size(decInfo) == e_success)
		    {
			puts("secret size sucessful");

			if(decode_secret_data(decInfo,decInfo->secret_file_size) == e_success)
			{
			    puts("decoding succesffuly complted");
			}
			else
			{
			    return e_failure;
			}
		    }
		    else
		    {
			return e_failure;
		    }
		}
		else
		{
		    return e_failure;
		}
	    }
	    else
	    {
		return e_failure;
	    }
	}
	else
	{
	    return e_failure;
	}
    }
    else
    {
	return e_failure;
    }

    return e_success;

}
Status open_files_decode( DecodeInfo *decInfo)
{
    decInfo->fptr_stego_file = fopen(decInfo->stego,"r");
    if(decInfo->fptr_stego_file == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR : unable to open file %s\n", decInfo->stego);
	return e_failure;
    }
    return e_success;
}
Status decode_lsb_data( char *data , char *buffer , int n)
{
    int flag=0, i;
    for(i=7;i>=0;i--)
    {
	if(buffer[i] & (1<<0) )
	{
	    flag =1;
	}
	else
	{
	    flag = 0;
	}
	data[n] = data[n] | (flag << (7 - i) );
    }
    return e_success;
}

Status decode_magic_string( DecodeInfo *decInfo)
{
    fseek(decInfo->fptr_stego_file,54,SEEK_CUR);
    char buffer[8];
    int i=0;
    decInfo->magic_char[i] = 0;
    for(i=0;i<2;i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_file);
	decode_lsb_data(decInfo->magic_char,buffer,i);
    }
    decInfo->magic_char[2] = '\0';
    printf("%s\n",decInfo->magic_char);
    if(strcmp(MAGIC_STRING,decInfo->magic_char) == 0)
    {	
	return e_success;
    }
    else
    {
	return e_failure;
    }
}

Status decode_extn_size(DecodeInfo *decInfo)
{
    char buffer[32];
    decInfo->extn_size=0;
    fread(buffer,32,1,decInfo->fptr_stego_file); 
    int i,flag=0;
    for(i=0;i<=31;i++)
    {
	if(buffer[i]  & (1<<0))
	{
	    flag=1;
	}
	else
	{
	    flag=0;
	}

	decInfo->extn_size = decInfo->extn_size |( flag << (31-i) );
    }
    printf("%d\n",decInfo->extn_size);
    return e_success;
}
Status decode_extension(DecodeInfo *decInfo,int extn_size)
{
    char buffer[8];
    int i=0;
    for(i=0;i<extn_size;i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_file);
	decode_lsb_data(decInfo->extn,buffer,i);
    }
    decInfo->extn[i]='\0';
    printf("%s\n",decInfo->extn);
    strcat(decInfo->text_file,decInfo->extn);
    decInfo->fptr_text_file = fopen(decInfo->text_file,"w");
    if(decInfo->fptr_text_file == NULL)
    {
	perror("fopen");
	fprintf(stderr,"ERROR:unable to open file %s\n",decInfo->text_file);
	return e_failure;
    }
    printf("Output File Created: %s\n", decInfo->text_file);
    return e_success;
}
Status decode_secret_file_size(DecodeInfo *decInfo)
{
    decInfo->secret_file_size=0;
    char buffer[32];
    fread(buffer,32,1,decInfo->fptr_stego_file);
    decode_size_data(&decInfo->secret_file_size,buffer);
    printf("%d\n",decInfo->secret_file_size);
    return e_success;
}
Status decode_secret_data(DecodeInfo *decInfo,int sz)
{
    char buffer[8];
    char ch=0;
    int flag=0,i,j;
    for(i=0;i<sz;i++)
    {
	fread(buffer,8,1,decInfo->fptr_stego_file);

	for(j=0;j < 8;j++)
	{
	    if(buffer[j] & (1<<0) )
	    {
		flag =1;
	    }
	    else
	    {
		flag = 0;
	    }
	    ch = ch | (flag << (7 - j) );
	}
	fwrite(&ch,1,1,decInfo->fptr_text_file);
	ch=0;
    }
    return e_success;
}
uint decode_size_data(int *data, char *buffer)
{
    int flag=0, i;
    for(i=0;i<=31;i++)
    {
	if(buffer[i] & (1<<0) )
	{
	    flag =1;
	}
	else
	{
	    flag = 0;
	}
	*data = *data| (flag << (31 - i) );

    }
    return *data;
}
