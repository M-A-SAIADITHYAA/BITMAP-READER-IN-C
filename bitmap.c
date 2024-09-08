#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

//provides fixed size irrespective of the system

#define MAXPATH 260 //maximum for windows and linux path length

uint32_t read(FILE *file){
    uint32_t value; //variable to store the final value
    uint8_t bytes[4]; // array to store the byte value of size 8 
    if(fread(bytes,sizeof(bytes),1,file)!=1){ // fread takes four arguments bytes:pointer from where read data is stored
    // 1 number of element to be read file is the pointer to the file we are expecting one element to be read if not one something not good so exit
        fprintf(stderr,"error\n");
        exit(1);
    }

        value = (uint32_t)bytes[0] | ((uint32_t)bytes[1] << 8) | //we are converting the four bytes array into 32 bit as it is in little endian format the left most is the least significant
            ((uint32_t)bytes[2] << 16) | ((uint32_t)bytes[3] << 24);
    return value;
  
    
}

int main(){
    char filepath[260];
    printf("enter the file path");
    if(fgets(filepath,sizeof(filepath),stdin)==NULL){
        fprintf(stderr,"error while reading the file path\n");
        return 1;//return 
    }

    //if there is any newline
    filepath[strcspn(filepath,"\n")]=0; //if the newline charater is in 19 index,filepath[19]= 0

    FILE *file = fopen(filepath, "rb");
    if (file == NULL) {
        perror("Error opening file");
        return 1;
    }

     if (fseek(file, 14, SEEK_SET) != 0) { //14 refers to the offset position and seek_set refers to the start seek_end refers to the end of the file 
        perror("Error seeking in file");//if it not succesful a non zero number is returned ,14 refer to the initial header
        fclose(file);
        return 1;
    }

    uint32_t headerSize = read(file);//the first 4 bytes represents the headersize
      if (fseek(file, 18, SEEK_SET) != 0) {
        perror("Error seeking to width");//move from 14 to 18 while make sure that there are bytes
        fclose(file);
        return 1;
    }
    uint32_t width = read(file);

    printf("Header Size: %u bytes\n", headerSize);
    printf("Width: %u pixels\n", width);

    fclose(file);
    return 0;


}


