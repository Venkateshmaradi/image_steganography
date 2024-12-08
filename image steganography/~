#include <stdio.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "decode.h"

int i,j;


Status decode_open_file(DecodeInfo *decinfo)
{

    decinfo->fptr_stego_image=fopen(decinfo->stego_image_fname,"r");
    if (decinfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->stego_image_fname);

        return e_failure;
    }
    return e_success;
}


Status read_and_validate_decode_args(char *argv[],DecodeInfo *decinfo)
{

    if( (argv[2] != NULL) && (strstr(argv[2],".bmp")))
    {
        decinfo -> stego_image_fname=argv[2];
        if( (argv[3] != NULL) && (strchr(argv[3],'.')))
        {
            return e_success;
        }
        else if( (argv[3] !=NULL) && (strstr(argv[3],".") == NULL))
            return e_failure;
        else
            return e_success;
    }
    else
    {
        puts("ERROR: Not a .bmp file");
        return e_failure;
    }
}



Status decode_magic_string(char *magic_string,DecodeInfo *decinfo)
{
    fseek(decinfo->fptr_stego_image,54,SEEK_SET);
    int len=strlen(magic_string);
    unsigned char buffer1[8];
    unsigned char buffer2[len+1];
    j=0;
    while(j<len)
    {
        buffer2[j]=0x00;
        fread(buffer1,8,1,decinfo->fptr_stego_image);	
        for(i=0;i<8;i++)
        {
        
            buffer2[j]= buffer2[j] | ( (buffer1[i] & 1) << i) ;
        }
        j++;
    }
    buffer2[j]='\0';  
    if( (strcmp(buffer2,MAGIC_STRING)) == 0 )
        return e_success;
    else
        return e_failure;
}


Status decode_secret_file_extn_size(DecodeInfo *decinfo,int *n)
{
    char buffer1[32];
    fread(buffer1,32,1,decinfo->fptr_stego_image);	
    decode_size(buffer1,n);

    return e_success;
}



Status decode_secret_file_extn(DecodeInfo *decinfo,int size)
{
    char ch;
    char buffer[8];
    char *dest=decinfo->extn_secret_file;
    for(int i=0;i<size;i++)
    {
 
        fread(&buffer,8,1,decinfo->fptr_stego_image);
        decode_data_from_lsb(buffer,&ch);
        dest[i]=ch;		
    }
    return e_success;
}


Status decode_size(char *data,int *n)
{
    int num;
    for(i=0;i<32;i++)
    {
      
        num =num|( (data[i] & 1)<<i);
    }
    *n=num;
}



Status decode_data_from_lsb(char *source,char *dest)
{
    char ch;
    for(i=0;i<8;i++)
    {
        ch= ch | ( (source[i] & 1<<0) << i) ;
    }
    *dest=ch; 
}



Status decode_new_sec_file_open(DecodeInfo *decinfo,char *name)
{
    if(name != NULL && strstr(name,"." ))
    {
        char sec[50]="h";
        i=0;
        while (name[i] != '.')		
        {
            sec[i]=name[i];
            i++;
        }
        sec[i] == '\0';
        decinfo -> secret_fname= strcat(sec,decinfo->extn_secret_file);	
        decinfo -> fptr_secret = fopen(decinfo->secret_fname,"w");	
        if (decinfo->fptr_secret == NULL)		
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->secret_fname);
            return e_failure;
        }
    }
    else 				
    {
        char sec[50]="def_sec";
        decinfo -> secret_fname= strcat(sec,decinfo->extn_secret_file);	
        decinfo -> fptr_secret = fopen(decinfo->secret_fname,"w");
        if (decinfo->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", decinfo->secret_fname);
            return e_failure;
        }
    }
    return e_success;
}



Status decode_secret_file_size(DecodeInfo *decinfo)
{ 
    int n=decinfo->size_secret_file;
    char buffer1[32];
    fread(buffer1,32,1,decinfo->fptr_stego_image); 
    decode_size(buffer1,&n);				

    decinfo->size_secret_file=n;	
    return e_success;

}



Status decode_secret_data(DecodeInfo *decinfo)
{
    char buffer[8];
    char ch;
    int size=decinfo->size_secret_file;			
    while(size>0)
    {
        fread(buffer,8,1,decinfo->fptr_stego_image);

        decode_data_from_lsb(buffer,&ch);		
        fwrite(&ch,1,1,decinfo->fptr_secret);
        size--;
    }
    return e_success;
}



Status do_decoding( DecodeInfo *decinfo,char *argv[])
{
    char *name=NULL;
    int size=0;

    if( decode_open_file(decinfo) == e_success)	
    {
        printf("---------------------------------------- OPENING THE FILE IS SUCCESSFULL ---------------------------------------- \n");


        if( decode_magic_string(MAGIC_STRING,decinfo) == e_success) 
        {
            printf("---------------------------------------- DECODING THE MAGIC STRING IS SUCCESSFULL ---------------------------------------- \n");


            if( decode_secret_file_extn_size(decinfo,&size)==e_success)  
            {
                printf("---------------------------------------- DECODE SECRET FILE EXTN SIZE SUCCESSFULL ---------------------------------------- \n");


                if( decode_secret_file_extn(decinfo,size) == e_success) 
                {
                    printf("---------------------------------------- DECODE SECRET FILE EXTN SUCCESSFULL ----------------------------------------\n");
                    if(argv[3] != NULL)
                    {
                        name=argv[3];
                        printf("INFO: Opening c%s\n",argv[3]);
                    }
                    else
                        printf("---------------------------------------- AS THERE IS FILE PASSED OPENNG DEFAULT DEF_SEC%s ----------------------------------------\n",decinfo->extn_secret_file);



                    if( decode_new_sec_file_open(decinfo,name) == e_success) 
                    {
                        printf("---------------------------------------- DECODE NEW SECRET FILE OPEN SUCCESSFULL ----------------------------------------\n");
        
                        if( decode_secret_file_size(decinfo) ==e_success ) 
                        {
                            printf("---------------------------------------- DECODE SECRET FILE SIZE SUCCESSFULL ----------------------------------------\n");
                            if( decode_secret_data(decinfo) ==e_success)  
                            {
                                printf("---------------------------------------- DECODE SECRET DATA SUCCESSFULL ----------------------------------------\n");
                                printf("---------------------------------------- DECODING SUCCESSFULL --------------------\n");
                                return e_success;
                            }
                            else
                            {
                                printf("---------------------------------------- DECODING SECRET DATA FAILURE ----------------------------------------\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("---------------------------------------- DECODING SECRET FILE SIZE FAILURE ----------------------------------------\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("---------------------------------------- OPEN DEFAULT SECRET FILE FAILURE ----------------------------------------\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("---------------------------------------- DECODING THE SECRET FILE EXTENSION FAILURE ----------------------------------------\n");
                    return e_failure;
                }
            }	
            else
            {
                printf("---------------------------------------- DECODING THE SECRET FILE EXTENSION SIZE FAILURE ----------------------------------------\n");
                return e_failure;
            }
        }
        else
        {
            printf("---------------------------------------- DECODING MAGIC STRING FAILURE ----------------------------------------\n");
            return e_failure;
        }
    }
    else
    {
        printf("---------------------------------------- OPENING THE FILE IS FAILURE ----------------------------------------\n");
        return e_failure;
    }
}

