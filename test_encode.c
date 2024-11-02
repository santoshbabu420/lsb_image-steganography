/*#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "string.h"



int main(int argc,char *argv[]) //to pass the command line argumernts 
{
   EncodeInfo encInfo;
    uint img_size;
    if(argc >= 3)
   {
	int ret = check_operation_type(argv); // decoding
	if (ret == e_encode)
	{
	    printf("selected encoding....\n");
	
	
	    //emcoding operation
	   if(argc >= 4)
	   {
	          	if(read_and_validate_encode_args(argv,&encInfo) == e_success) // .bmp file or not to check...
		          {
		                printf("read_valdiate_suscessfully:\n");
		                  if(start_encoding(&encInfo) == e_success)
		                   {
		                	puts("encoding succesful");
		                    }
		                    else
		                    {
			              puts("encoding unsuccesful");
			               return e_failure;
		                      }
			    }  
			//
	   } 
		   
		else
		{
	         return e_failure;
		}
	    	
	}
	else if(ret == e_decode)
	{
	    //decoding operation
	    printf("selected decoding ....\n");
	}
  //  }
    else
    {
	puts("error in to pass  the command line arguments");
	//ERROR MESSAGE

    }


}
}
    
OperationType check_operation_type(char *argv[])
{
    //check argv[1] is -e or not
    //yes ----> return e_ncode
    //no --->
    //check argv[1]
    //yes--> return e_decode
    //no -->
    //return e_unsupported
    if(strcmp(argv[1], "-e") == 0)
    {
	return e_encode;
    }
    else if(strcmp(argv[1], "-d") == 0)
    {
	return e_decode;
    }
    else
    {
	return e_unsupported;
    }


}
*/
#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
#include "decode.h"
//#include "decode.h"
OperationType check_operation_type(char *argv[])
{
    if(strcmp(argv[1],"-e") == 0) //checking operator for encoding
    {
        return e_encode;
    }
    else if (strcmp(argv[1],"-d") == 0) //decoding
    {
        return e_decode;
    }
    else
    {
        return e_unsupported; //unsupported
    }
}
int main(int argc , char *argv[])
{
    if(argc >= 3) //checking command line arguments are greater than 3 or not
    {
        int res = check_operation_type(argv); //calling check operator function

           if( res == e_encode)
           {
               printf("selected encoding\n");

               EncodeInfo encinfo; //encoding structure varriable declaration

               if(read_and_validate_encode_args(argv,&encinfo) == e_success)
               {
                  if( start_encoding(&encinfo) == e_success ) //encoding successful
                  {
                      printf("Encoding sucessfull\n");
                  }
               }
               else
               {
               puts("error in encoding operator");
               }
           }
           else if(res == e_decode)
           {
               printf("selected decoding\n");

               DecodeInfo decInfo; //declaring decoding structure variable

               if(read_and_validate_decode_argc(argv,&decInfo) == e_success)
               {
                   if( do_decoding(&decInfo) == e_success) //decoding succesfull
                   {
                       puts("Decoding sucessfully complted");
                   }
               }
               else
               {
               puts("error in decoding operator");

               }
           }
           else
           {
               puts("Error in the command line arguments"); //printing error in the cmd line
           }
    }
}
