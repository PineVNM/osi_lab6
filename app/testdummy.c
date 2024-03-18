#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MYFILE "/dev/etx_device"
int main()
{
   char value[10];
   FILE *fptr;

   // Open the device for reading and writting
   fptr = fopen(MYFILE,"r+");

   if(fptr == NULL)
   {
       printf("Error! opening file %s\n", MYFILE);

       // Program exits if the file pointer returns NULL.
       exit(1);
   }

   // Write the string "Hello" to the device using the pointer fptr
   fprintf(fptr, "Hello");
   
   // Read from the device for a count of 1 byte
   fread(&value, sizeof(value), 1, fptr);
   
   printf("%s\n", value);
   fclose(fptr); 
  
   return 0;
   
}

