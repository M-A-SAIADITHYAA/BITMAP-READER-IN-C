#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdint.h>

//provides fixed size irrespective of the system

#define MAXPATH 260 
//maximum for windows and linux path length

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
    
    char outputPath[MAXPATH];
    
    char *dotPos = strrchr(filepath, '.');
    if (dotPos != NULL) {
        size_t baseLength = dotPos - filepath;
        strncpy(outputPath, filepath, baseLength); //copy thr str till the baselength  
        outputPath[baseLength] = '\0';               
        strcat(outputPath, "_gray.bmp");            
    } else {
        
        snprintf(outputPath, sizeof(outputPath), "%s_gray.bmp", filepath);
    }
     if (fseek(file, 10, SEEK_SET) != 0){
     	perror("Error seeking in file");
     	fclose(file);
        return 1;
     	
	 }
    
    uint32_t offset  = read(file);
   
    

     if (fseek(file, 14, SEEK_SET) != 0) { //14 refers to the offset position and seek_set refers to the start seek_end refers to the end of the file 
        perror("Error seeking in file");//if it not succesful a non zero number is returned ,14 refer to the initial header
        fclose(file);
        return 1;
    }

    uint32_t headerSize = read(file);//the first 4 bytes represents the headersize
      if (fseek(file, 18, SEEK_SET) != 0) {
        perror("Error seeking ");//move from 14 to 18 while make sure that there are bytes
        fclose(file);
        return 1;
    }
    uint32_t width = read(file);
      if (fseek(file, 22, SEEK_SET) != 0) {
        perror("Error seeking to width");//move from 18 to 22 while make sure that there are bytes
        fclose(file);
        return 1;
    }

   uint32_t height = read(file);
   if (fseek(file, 28, SEEK_SET) != 0) {
        perror("Error seeking ");//move from 22 to 28 while make sure that there are bytes
        fclose(file);
        return 1;
    }
    uint32_t bits_per_pixel = read(file);
   
     printf("Width: %u pixels\n", bits_per_pixel);
     int rowSize = ((width * bits_per_pixel + 31) / 32) * 4;
     
     if (fseek(file, offset , SEEK_SET) != 0) {
        perror("Error seeking ");
        fclose(file);
        return 1;
    }
      uint8_t *imageData = (uint8_t *)malloc(height * rowSize);//allocating memory to store the imageData
    if (imageData == NULL) {
        perror("Error allocating memory");
        fclose(file);
        return;
    }
    fread(imageData, 1, height * rowSize, file);
    fclose(file);	
    
    
    //creating an output file for grayscale
     FILE *outputFile = fopen(outputPath, "wb");
    if (outputFile == NULL) {
        perror("Error opening output file");
        free(imageData);
        return;
    }
    
    FILE *inputHeaderFile = fopen(filepath, "rb");//opening the input file
    if (inputHeaderFile == NULL) {
        perror("Error opening input header file");
        fclose(outputFile);
        free(imageData);
        return;
    }
    uint8_t header[headerSize ];
    fread(header, 1, headerSize , filepath);//reading and writting the header details from input to grayscale file
    fwrite(header, 1, headerSize , outputFile);
    
    
    uint8_t *grayData = (uint8_t *)malloc(height * rowSize);//graydata is intialized to write in the 
    if (grayData == NULL) {
    perror("Error allocating memory for grayscale data");
    fclose(outputFile);
    free(imageData);
    return;
} int i,j;
  for ( i = 0; i < height; ++i) {
    for ( j = 0; j < width; ++j) {
        int pixelOffset = i * rowSize + j * 3;
        uint8_t r = imageData[pixelOffset + 2];
        uint8_t g = imageData[pixelOffset + 1];
        uint8_t b = imageData[pixelOffset];

        uint8_t gray = (uint8_t)(0.299 * r + 0.587 * g + 0.114 * b);//formula based on ntsc standard

        grayData[pixelOffset] = gray;   // in place of blue
        grayData[pixelOffset + 1] = gray; // Green 
        grayData[pixelOffset + 2] = gray; // Red
    }
} 
     

   

    fwrite(grayData, 1, height * rowSize, outputFile);
    fclose(outputFile);
    
    free(imageData);
    free(grayData);
    return 0;


}
