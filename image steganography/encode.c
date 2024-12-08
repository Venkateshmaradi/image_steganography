#include <stdio.h>
#include<string.h>
#include "encode.h"
#include "types.h"
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

Status read_and_validate_encode_args(char *argv[],EncodeInfo *encInfo)
{
    if( (argv[2] != NULL) && (strstr(argv[2],".bmp")))
    {
	encInfo -> src_image_fname=argv[2];	
	if( (argv[3] != NULL) && (strchr(argv[3],'.')))
	{
	    strcpy(encInfo ->extn_secret_file,strchr(argv[3],'.')); 
	    encInfo -> secret_fname=argv[3];			
	}
	else
	    return e_failure;
	if( (argv[4] != NULL) && (strstr(argv[4],".bmp")))	
	{
	    encInfo -> stego_image_fname = argv[4];
	}
	else if( argv[4] == NULL)
	{
	    encInfo -> stego_image_fname = "stego.bmp";	
	    return e_success;
	}
	else if( !(strstr(argv[4],".bmp")))
	    return e_failure;
	return e_success;
    }
    else
    {
	puts("ERROR: file should be BMP file\n");
	return e_failure;
    }
}

Status check_capacity( EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if( encInfo->image_capacity > (54+strlen(MAGIC_STRING)+4+4+4+(encInfo -> size_secret_file) *8) )
    {
	return e_success;
    }
    else
	return e_failure;
}

uint get_file_size(FILE *fptr)
{
    fseek(fptr,0,SEEK_END);
    return ftell(fptr);
}

Status copy_bmp_header(FILE*fptr_src_image,FILE*fptr_dest_image)
{
    char buffer[54];
    rewind(fptr_src_image);
    fread(buffer,54,1,fptr_src_image);
    fwrite(buffer,54,1,fptr_dest_image);
    return e_success;
}

Status encode_magic_string(char* magic_string, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_string, strlen(magic_string),encInfo->fptr_src_image,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for(int i=0;i<size;i++)
    {
        fread(buffer,8,1,fptr_src_image);
        encode_byte_to_lsb(data[i],buffer);
        fwrite(buffer,8,1,fptr_stego_image);
    }
    return e_success;
}

Status encode_byte_to_lsb(char data,char *image_buffer)
{
    for(int i=0;i<8;i++)
    {
        image_buffer[i]=((image_buffer[i] & 0xfe) | (data & (1<<i))>>i);
    }
    return e_success;
}

Status encode_extension_size(int size,FILE *fptr_src_image,FILE *fptr_stego_image)
{
    char buffer[32];
    fread(buffer,32,1,fptr_src_image);
    encode_size_to_lsb(buffer,size);
    fwrite(buffer,32,1,fptr_stego_image);
    return e_success;
}
Status encode_size_to_lsb(char *image_buffer,int data)
{
    for(int i=0;i<32;i++)
    {
        image_buffer[i]=((image_buffer[i]&0xfe)|(data & (1<<i))>>i);
    }
}

Status encode_secret_file_extn(char *file_extn,EncodeInfo *encInfo)
{
    if ((encode_data_to_image(file_extn,strlen(file_extn),encInfo->fptr_src_image,encInfo->fptr_stego_image))==e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

Status encode_secret_file_size(long int file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer,32,1,encInfo->fptr_src_image);
    encode_size_to_lsb(buffer,file_size);
    fwrite(buffer,32,1,encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret,0,SEEK_SET);
    char buffer[encInfo->size_secret_file];
    fread(buffer,encInfo->size_secret_file,1,encInfo->fptr_secret);
    if ((encode_data_to_image(buffer,encInfo->size_secret_file,encInfo->fptr_src_image,encInfo->fptr_stego_image))==e_success)
        return e_success;
    else
        return e_failure;
}

Status copy_remaining_img_data(FILE *fptr_src,FILE *fptr_dest)
{
    char ch;
    while (fread(&ch,1,1,fptr_src)>0)
        fwrite(&ch,1,1,fptr_dest);
    return e_success;
}


Status do_encoding(EncodeInfo *encInfo)
{
    if (open_files(encInfo)==e_success)
    {
        printf("--------------------------------------- THE FILES HAVE OPENED SUCCESSFULLY --------------------------------------- \n");
    }
    else
    {
        printf("--------------------------------------- THE OPEN FUNCTION IS FAILED --------------------------------------- \n");
    }

    if(check_capacity(encInfo)==e_success)
    {
        printf("--------------------------------------- CHECK CAPACITY IS EXECUTED --------------------------------------- \n");
    }
    else
    {
        printf("--------------------------------------- CHECK CAPACITY IS FAILED --------------------------------------- \n");
    }

    if (copy_bmp_header(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("--------------------------------------- COPY BMP IS EXECUTED --------------------------------------- \n");
    }
    else
    {
        printf("--------------------------------------- COPY BMP IS FAILED --------------------------------------- \n");
    }

    if (encode_magic_string(MAGIC_STRING,encInfo)==e_success)
    {
        printf("--------------------------------------- ENCODE MAGIC STRING IS SUCCESSFULL --------------------------------------- \n");
        strcpy(encInfo->extn_secret_file,strstr(encInfo->secret_fname,"."));
    }
    else
    {
        printf("--------------------------------------- ENCODE MAGIC STRING IS FAILURE --------------------------------------- \n");
    }

    if (encode_extension_size(strlen(strstr(encInfo->secret_fname,".")),encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("---------------------------------------  ENCODE EXTENSION SIZE IS SUCCESSFULL --------------------------------------- \n");
    }
    else
    {
        printf("---------------------------------------  ENCODE EXTENSION SIZE IS FAILURE --------------------------------------- \n");
    }

    if (encode_secret_file_extn((strstr(encInfo->extn_secret_file,".")),encInfo)==e_success)
    {
        printf("---------------------------------------  ENCODE SECRET FILE EXTENSION IS SUCCESSFULL --------------------------------------- \n");
    }
    else
    {
        printf("---------------------------------------  ENCODE SECRET FILE EXTENSION IS FAILED --------------------------------------- \n");
    }

    if (encode_secret_file_size(encInfo->size_secret_file,encInfo)==e_success)
    {
        printf("---------------------------------------  THE SECRET FILE SIZE IS SUCCESSFULL --------------------------------------- \n");
    }
    else
    {
        printf("--------------------------------------- THE SECRET FILE SIZE IS FAILED --------------------------------------- \n");
    }


    if ((encode_secret_file_data(encInfo))==e_success)
    {
        printf("--------------------------------------- ENCODE SECRET FILE SUCCESSFULL --------------------------------------- \n");
    }
    else
    {
        printf("--------------------------------------- ENCODE SECRET FILE FAILED --------------------------------------- \n");
    }

    if (copy_remaining_img_data(encInfo->fptr_src_image,encInfo->fptr_stego_image)==e_success)
    {
        printf("--------------------------------------- COPY REAMINING IS SUCCESSFULL --------------------------------------- \n");
        return e_success;
    }
    else
    {
        printf("--------------------------------------- COPY REAMINING IS FAILED --------------------------------------- \n");
        return e_failure;
    }


}

