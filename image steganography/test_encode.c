/*
Name:VENKATESH MARADI
Date:23/03/2023
Descrition:Steganography: Hiding the data in the least significant bit

venky@venky-Dell-G15-5511:~/ECEP/C/project/C-Project-1/4-SkeletonCode$ ./a.out -e beautiful.bmp secret.txt stego.bmp 
---------------------------------------- ENCODING IS SELECTED ----------------------------------------
---------------------------------------- READ AND VALIDATE IS SUCCESSFULL ----------------------------------------
--------------------------------------- THE FILES HAVE OPENED SUCCESSFULLY ---------------------------------------
width = 1024
height = 768
--------------------------------------- CHECK CAPACITY IS EXECUTED ---------------------------------------
--------------------------------------- COPY BMP IS EXECUTED ---------------------------------------
--------------------------------------- ENCODE MAGIC STRING IS SUCCESSFULL ---------------------------------------
---------------------------------------  ENCODE EXTENSION SIZE IS SUCCESSFULL ---------------------------------------
---------------------------------------  ENCODE SECRET FILE EXTENSION IS SUCCESSFULL ---------------------------------------
---------------------------------------  THE SECRET FILE SIZE IS SUCCESSFULL ---------------------------------------
--------------------------------------- ENCODE SECRET FILE SUCCESSFULL ---------------------------------------
--------------------------------------- COPY REAMINING IS SUCCESSFULL ---------------------------------------
---------------------------------------- ENCODING IS SUCCESSFULL ----------------------------------------


venky@venky-Dell-G15-5511:~/ECEP/C/project/C-Project-1/4-SkeletonCode$ ./a.out -d stego.bmp
---------------------------------------- DECODING IS SELECTED ----------------------------------------
---------------------------------------- READ AND VALIDATE IS SUCCESSFULL ----------------------------------------
---------------------------------------- OPENING THE FILE IS SUCCESSFULL ----------------------------------------
---------------------------------------- DECODING THE MAGIC STRING IS SUCCESSFULL ----------------------------------------
---------------------------------------- DECODE SECRET FILE EXTN SIZE SUCCESSFULL ----------------------------------------
---------------------------------------- DECODE SECRET FILE EXTN SUCCESSFULL ----------------------------------------
---------------------------------------- AS THERE IS FILE PASSED OPENNG DEFAULT DEF_SEC.txt ----------------------------------------
---------------------------------------- DECODE NEW SECRET FILE OPEN SUCCESSFULL ----------------------------------------
---------------------------------------- DECODE SECRET FILE SIZE SUCCESSFULL ----------------------------------------
---------------------------------------- DECODE SECRET DATA SUCCESSFULL ----------------------------------------
---------------------------------------- DECODING SUCCESSFULL --------------------
---------------------------------------- DECODING IS SUCCESSFULL ----------------------------------------
*/


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"


int main(int argc,char **argv)
{
    if(argc<2)
    {
        return 0;
    }
    int operation=check_operation_type(argv); 
    if(operation == e_encode)
    {
        printf("---------------------------------------- ENCODING IS SELECTED ---------------------------------------- \n");
        EncodeInfo encInfo;				
        if(read_and_validate_encode_args(argv,&encInfo)==e_success)	
        {
            printf("---------------------------------------- READ AND VALIDATE IS SUCCESSFULL ---------------------------------------- \n");
            if( do_encoding (&encInfo) == e_success)	
                printf("---------------------------------------- ENCODING IS SUCCESSFULL ---------------------------------------- \n");
            else
                printf("---------------------------------------- ENCODING IS FAILURE ---------------------------------------- \n");
        }
        else
        {
            printf("---------------------------------------- READ AND VALIDATE WAS FAILED ---------------------------------------- \n");

        }
    }
    else if(operation == e_decode)
    {
        printf("---------------------------------------- DECODING IS SELECTED ---------------------------------------- \n");
        DecodeInfo decinfo;	
        if(read_and_validate_decode_args(argv,&decinfo)==e_success)
        {
            printf("---------------------------------------- READ AND VALIDATE IS SUCCESSFULL ---------------------------------------- \n");
            if( do_decoding (&decinfo,argv) == e_success)	
                printf("---------------------------------------- DECODING IS SUCCESSFULL ---------------------------------------- \n");
            else
                printf("---------------------------------------- DECODING IS FAILURE ---------------------------------------- \n");
        }
        else
        {
            printf("---------------------------------------- READ AND VALIDATE WAS FAILURE ---------------------------------------- \n");

        }
    }
    else
    {
        printf("--------------------------------------- UNSUPPORTED OPERATION TYPE ----------------------------------------\n ");
    }
    return 0;
} 
OperationType check_operation_type(char **argv)
{
    if(!(strcmp(argv[1],"-e")))		
    {
        return e_encode;
    }
    else if(!(strcmp(argv[1],"-d")))
    {
        return e_decode;
    }
    else
    {
        return e_unsupported;	
    }
}
