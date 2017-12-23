#include "utfconverter.h"
#define cse320(msg,msg2) printf("CSE320: %s: %s\n",msg,msg2)


int main(int argc, char *argv[])
{
    int opt,return_code  = EXIT_FAILURE;
    int optV,eFlag,UTFIN=0;
    char *input_path = NULL;
    char *output_path = NULL;
    
    /* open output channel */

   // FILE* standardout = fopen("stdout", "w");

    

    #ifdef CSE320
    char hostname[128];
    gethostname(hostname,sizeof(hostname));
    cse320( "Host",hostname);
    #endif

    /* Parse short options */
    while((opt = getopt(argc, argv, "hveUTF-816")) != -1) {
        switch(opt) {
            case 'h':
                /* The help menu was selected */
                USAGE(argv[0]);
                exit(EXIT_SUCCESS);
                break;
            case'e':
                eFlag =1;       
                break;       
            case'v':
                   optV++;     
                break;   
    
            case '?':
                /* Let this case fall down to default;
                 * handled during bad option.
                 */
                
                 break;
            default:    
                /* A bad option was provided. */
                USAGE(argv[0]);
                exit(EXIT_FAILURE);
                break;
        }
    }

    //select output format
    if(eFlag>0){
        int index;
    for (index = optind; index < argc; index++){
            if(strcmp("UTF-8",argv[index])==0){
            UTFIN=1;    
            optind ++;
            }
            if(strcmp("UTF-16BE",argv[index])==0){
            UTFIN=2;
            optind ++;
            }
            if(strcmp("UTF-16LE",argv[index])==0){
            UTFIN=3;
            optind ++;

            }
  
        }
    }

    else {
        fprintf(stderr, "No output format provided.\n"); 
        USAGE(0[argv]);
        exit(EXIT_FAILURE);
    }
    /* Get position arguments */
    if(optind < argc && (argc - optind) == 2) {
        input_path = argv[optind++];
        output_path = argv[optind++];
    } else {
        if((argc - optind) <= 0) {
            fprintf(stderr, "Missing INPUT_FILE and OUTPUT_FILE.\n");
        } else if((argc - optind) == 1) {
            fprintf(stderr, "Missing OUTPUT_FILE.\n");
        }else if(UTFIN==0){
        	fprintf(stderr, "Missing Proper Output Format.\n");
        } 
        else {
            fprintf(stderr, "Too many arguments provided.\n");
        }
        USAGE(0[argv]);
        exit(EXIT_FAILURE);
    }
    /* Make sure all the arguments were provided */
    if(input_path != NULL || output_path != NULL) {
        int input_fd = -1, output_fd = -1;
        bool success = false;

        switch(validate_args(input_path, output_path)) {

                case VALID_ARGS:
                    /* Attempt to open the input file */
                    if((input_fd = open(input_path, O_RDONLY)) < 0) {
                        fprintf(stderr, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }
                    /* Delete the output file if it exists; Don't care about return code. */
                    unlink(output_path);
                    /* Attempt to create the file */
                    if((output_fd = open(output_path, O_CREAT | O_WRONLY,
                        S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP)) < 0) {
                        /* Tell the user that the file failed to be created */
                        fprintf(stderr, "Failed to open the file %s\n", input_path);
                        perror(NULL);
                        goto conversion_done;
                    }
                    /* Start the conversion */
                    success = convert(input_fd, output_fd,optV, UTFIN);
conversion_done:
                    if(success) {
                        /* We got here so it must of worked right? */
                        return_code = EXIT_SUCCESS;
                        close(input_fd);
                        close(output_fd);

                        fprintf(stderr, "The output file %s was created successfully.\n", output_path);
                        goto done;
                    } else {
                        /* Conversion failed; clean up */
                        if(output_fd < 0 && input_fd >= 0) {
                            close(input_fd);
                        }
                        if(output_fd >= 0) {
                            close(output_fd);
                            unlink(output_path);
                        }
                        /* Just being pedantic... */
                        fprintf(stderr, "The output file %s wasn't created.\n", output_path);
                        return_code = EXIT_FAILURE;
                        goto done;
                    }
                case SAME_FILE:
                    fprintf(stderr, "The output file %s was not created. Same as input file.\n", output_path);
                    break;
                case FILE_DNE:
                    fprintf(stderr, "The input file %s does not exist.\n", input_path);
                    break;
                default:
                    fprintf(stderr, "An unknown error occurred\n");
                    break;
        }
    } else {
        /* Alert the user what was not set before quitting. */
        if((input_path = NULL) == NULL) {
            fprintf(stderr, "INPUT_FILE was not set.\n");
        }
        if((output_path = NULL) == NULL) {
            fprintf(stderr, "OUTPUT_FILE was not set.\n");
        }
        // Print out the program usage
        USAGE(argv[0]);
    }
done:
    return return_code;
}

int validate_args(const char * input_path,const char * output_path)

{


    int return_code = FAILED;
    /* number of arguments */
    //int vargs = 2 ;             ;
    /* create reference */
    //void* pvargs = &vargs;
    /* Make sure both strings are not NULL */
    if(input_path != NULL && output_path != NULL) {
        /* Check to see if the the input and output are two different files. */
        
            /* Check to see if the input file exists */
            struct stat sb;
            struct stat sb2;
            /* zero out the memory of one sb plus another */
            memset(&sb, 0, sizeof(sb) + 1);
            /* increment to second argument */
            //pvargs++;
            /* now check to see if the file exists */
            if((stat(input_path, &sb) == -1 )) {
                /* something went wrong */
                if(errno == ENOENT) {
                    /* File does not exist. */
                    return_code = FILE_DNE;
                    /* No idea what the error is. */
                    perror("NULL");
                }


            } else {

                    #ifdef CSE320
                
                    char str[20];
                    char string[100];

                    strcat(string,input_path);
                    strcat(string,", ");

                    sprintf(str,"%lu",sb.st_dev);
                    strcat(string,str);
                    strcat(string,", ");

                    sprintf(str,"%lu",sb.st_ino);
                    strcat(string,str);
                    strcat(string,", ");

                    sprintf(str,"%lu",sb.st_size);
                    strcat(string,str);
                    strcat(string," Byte(s)");


                    cse320( "Input", string);
                    cse320( "Output", output_path);
                    #endif



                if((sb.st_dev == sb2.st_dev)&&(sb.st_ino==sb2.st_ino)){


                    return_code = SAME_FILE;

                }


                else{

                return_code = VALID_ARGS;
                }
            }
        
    }
    return return_code;
}

bool convert(const int input_fd,const int output_fd, const int optV, const int format)

{
    bool success = false;
    if(input_fd >= 0 && output_fd >= 0) {
        /* UTF-8 encoded text can be @ most 4-bytes */
        unsigned char bytes[4];
        auto unsigned char read_value;
        auto size_t count = 0;
        auto int safe_param = SAFE_PARAM;// DO NOT DELETE, PROGRAM WILL BE UNSAFE //
        void* saftey_ptr = &safe_param;
        auto ssize_t bytes_read;
        bool encode = false;
        bool fencode = false;

     
        
       int inputFormat = BOM_Check(input_fd);


       if(inputFormat<0){

        fprintf(stderr, "Invalid input BOM\n" );
        return false;
       }


        if(!BOM_write(output_fd,format)) {
            fprintf(stderr, "Write to file BOM failed."); 
            return false;                     
        } 


        #ifdef CSE320
         if (format ==1){
            cse320("Output Encoding","UTF-8\n");
         }
         else if (format ==2){
            cse320("Output Encoding","UTF-16BE\n");
         }
         else{
            cse320("Output Encoding","UTF-16LE\n");
         }
        #endif



        if (optV>0){
        	printHeader(optV);
        }



         //if input format == output format , just need to copy byte by byte
        if(inputFormat==format){

        	

        	if(inputFormat>1){
        	unsigned char buf[2];
            unsigned char pair[2];
            	
            bool surrogate =false;
            int temp,temp2=0;
        		while((bytes_read = read(input_fd, &buf, 2)) == 2) {
                temp = ((int)buf[0]<<8)+(int)buf[1];

                if (inputFormat==3){
                	temp = circularShiftTest(temp);
                }

            if(((0xD800<=temp)&&(temp<=0xDBFF))){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    temp2 = ((int)pair[0]<<8)+(int)pair[1];
               if (inputFormat==3){
                	temp2 = circularShiftTest(temp2);
                }    
                    if((0xDC00<=temp2)&&(temp2<=0xDFFF)){
                    	surrogate = true;
                    	
                    }
                if (inputFormat==3){
                	temp = circularShiftTest(temp);
                	temp2 = circularShiftTest(temp2);
                }
                    if(!safe_write(output_fd, &buf, CODE_UNIT_SIZE,5,false)) {
                    	
             			
             		}
             		if(!safe_write(output_fd, &pair, CODE_UNIT_SIZE,5,false)) {
                    	
             		}
                }
			else if((0xDC00<=temp)&&(temp<=0xDFFF)){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    temp2 = ((int)pair[0]<<8)+(int)pair[1];

               if (inputFormat==3){
                	temp2 = circularShiftTest(temp2);
                }  

                    if((0xD800<=temp)&&(temp<=0xDBFF)){
                    	surrogate = true;
                    }
                    if (inputFormat==3){
                	temp = circularShiftTest(temp);
                	temp2 = circularShiftTest(temp2);
               		 }

                 	if(!safe_write(output_fd, &buf, CODE_UNIT_SIZE,5,false)) {
                	
         			
         			}
         			if(!safe_write(output_fd, &pair, CODE_UNIT_SIZE,5,false)) {
                	
         			}
                
            }
           	else{
           		if (inputFormat==3){
                	temp = circularShiftTest(temp);
                }
                	if(!safe_write(output_fd, &buf, CODE_UNIT_SIZE,5,false)){

                	} 
              }

			if (optV>0){
 					if(surrogate==true){
 					printTableBELE(optV,temp,inputFormat,temp2,true);		
 					}else{
            		printTableBELE(optV,temp,inputFormat,0,true);
            		}  
            		
            	}
            	  surrogate = false;   

            }



        	}
            else{
            unsigned char buffer[4];	
            while((bytes_read = read(input_fd, &buffer[0], 1)) == 1) {
                //int realValue = buffer[0] &255;
                if(!safe_write(output_fd, &buffer[0], 1,6,false)) {
                    
                }

                unsigned char masked_value = buffer[0] & 0x80;    
                //ascii char
                if(masked_value==0){
                	
                	printTable( optV,true,1, buffer[0], buffer,true,1);
                }
                else{
                	if((buffer[0] & UTF8_4_BYTE) == UTF8_4_BYTE){

                	bytes_read = read(input_fd, &buffer[1], 3); 	
                	printTable( optV,false,4, calCodePt(4,buffer), buffer,true,1);
                	int temp = buffer[1];
                		if(!safe_write(output_fd, &temp, 1,6,true)) {
                    
               			 }
               		temp = buffer[2];
               			if(!safe_write(output_fd, &temp, 1,6,true)) {
                    
               			 }
               		 temp = buffer[3];	 
               			 if(!safe_write(output_fd, &temp, 1,6,true)) {
                    
               			 }


                	}
                	else if((buffer[0] & UTF8_3_BYTE) == UTF8_3_BYTE){


                	bytes_read = read(input_fd, &buffer[1], 2); 

                	printTable( optV,false,3, calCodePt(3,buffer), buffer,true,1);
                	int temp = buffer[1];
                		if(!safe_write(output_fd, &temp, 1,6,true)) {
                    
               			 }
               		 temp = buffer[2];	 
               			 if(!safe_write(output_fd, &temp, 1,6,true)) {
                    
               			 }
                	} 
                	else {


					bytes_read = read(input_fd, &buffer[1], 1); 
					printTable( optV,false,2, calCodePt(2,buffer), buffer,true,1);
					int temp = buffer[1];
						if(!safe_write(output_fd, &temp, 1,6,true)) {
                  
               			 }
               		 	}                 	
        	        }
            	}

        	}
        			if (optV>0){
			        	printFooter(optV);
			        }	
        
            return true;

        }




        // This case BE <-> LE just need to flip everything
        if((inputFormat==2&&format==3)||(inputFormat==3&&format==2)){
            unsigned char buf[2];
            unsigned char pair[2];
            int temp,temp2=0;
            bool surrogate =false;
            while((bytes_read = read(input_fd, &buf, 2)) == 2) {
                temp = ((int)buf[0]<<8)+(int)buf[1];

                if (inputFormat==3){
                	temp = circularShiftTest(temp);
                }

            if(((0xD800<=temp)&&(temp<=0xDBFF))){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    temp2 = ((int)pair[0]<<8)+(int)pair[1];
               if (inputFormat==3){
                	temp2 = circularShiftTest(temp2);
                }    
                    if((0xDC00<=temp2)&&(temp2<=0xDFFF)){
                    	surrogate = true;
                    	
                    }
                if (inputFormat==3){
                	temp = circularShiftTest(temp);
                	temp2 = circularShiftTest(temp2);
                }
                    if(!safe_write(output_fd, &temp, CODE_UNIT_SIZE,5,true)) {
                    	
             			
             		}
             		if(!safe_write(output_fd, &temp2, CODE_UNIT_SIZE,5,true)) {
                    	
             		}
                }
			else if((0xDC00<=temp)&&(temp<=0xDFFF)){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    temp2 = ((int)pair[0]<<8)+(int)pair[1];

               if (inputFormat==3){
                	temp2 = circularShiftTest(temp2);
                }  

                    if((0xD800<=temp)&&(temp<=0xDBFF)){
                    	surrogate = true;
                    }
                    if (inputFormat==3){
                	temp = circularShiftTest(temp);
                	temp2 = circularShiftTest(temp2);
               		 }

                 	if(!safe_write(output_fd, &temp, CODE_UNIT_SIZE,5,true)) {
                	
         			
         			}
         			if(!safe_write(output_fd, &temp2, CODE_UNIT_SIZE,5,true)) {
                	
         			}
                
            }
           	else{
           		if (inputFormat==3){
                	temp = circularShiftTest(temp);
                }
                	if(!safe_write(output_fd, &temp, CODE_UNIT_SIZE,5,true)){

                	} 
              }

 			if (optV>0){
 					if(surrogate==true){
 					printTableBELE(optV,temp,inputFormat,temp2,false);		
 					}else{
            		printTableBELE(optV,temp,inputFormat,0,false);
            		}  
            		
            	}
            	  surrogate = false;      
            }
            if (optV>0){
            	printFooter(optV);
            }
       
            return true;

        }


        /*Read in UTF 16 Bytes: outputting utf8 format*/
        if(inputFormat>1&&format==1){
        int counter,byteValue,byteValue2,byteValue3,byteValue4 = 0 ;
        unsigned char buf[2];
        unsigned char pair[2];

        while((bytes_read = read(input_fd, buf, 2)) == 2) {


            int realValue = (buf[1]<<8) | buf[0];

            if (inputFormat==2){

                realValue = (buf[0]<<8) | buf[1];

            }

            if((0<=realValue)&&(realValue<=0x7F)){
                counter =1;
            }else if((0x80<=realValue)&&(realValue<=0x7FF)){
                counter =2;
            }else if((0x0800<=realValue)&&(realValue<=0xFFFF)){
                counter =3;

                if((0xD800<=realValue)&&(realValue<=0xDBFF)){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    counter =4;
                }else if((0xDC00<=realValue)&&(realValue<=0xDFFF)){
                    //surrogate pair
                    read(input_fd, pair, 2);
                    counter =4;
                }
            
            }else {

                fprintf(stderr, "Invalid UTF8 encoding\n" );

            }

            
            if(counter ==1){
            byteValue = (realValue);


                    if(!safe_write(output_fd, &byteValue, 1,0,true)) { 

                    }
                    if(optV>0){
                    	printTable16(optV,realValue,byteValue,0,0,0,1,inputFormat);
                    }

            }
            else if (counter ==2){

                    byteValue = ((realValue&0x7ff)>>6)| 0xC0;
                    

                    if(!safe_write(output_fd, &byteValue, 1,0,true)) {         
                     }  

                    byteValue2 = ((realValue&0x3f))|0x80;
 

                    if(!safe_write(output_fd, &byteValue2, 1,0,true)) {         
                     }

                    if(optV>0){
                    	printTable16(optV,realValue,byteValue,byteValue2,0,0,2,inputFormat);
                    }


            }else if(counter ==3){
            
                    byteValue = ((realValue&0xFFFF)>>12)| 0xe0;


                    if(!safe_write(output_fd, &byteValue, 1,0,true)) {         
                     }  

                     byteValue2 = ((realValue&0xfc0)>>6)|0x80;


                    if(!safe_write(output_fd, &byteValue2, 1,0,true)) {         
                     }

                     byteValue3 = (realValue&0x3f)|0x80;


                    if(!safe_write(output_fd, &byteValue3, 1,0,true)) {         
                     }


 					if(optV>0){
                    	printTable16(optV,realValue,byteValue,byteValue2,byteValue3,0,3,inputFormat);
                    }


            }else{

                    //high 10 bits
                    int v1 = ((buf[1]<<8) | buf[0])-0xD800;
                    //low 10 bits
                    int v2 = ((pair[1]<<8) | pair[0])-0xDC00;

                        // this gives us the real code point

                    if (inputFormat==2){

                    //high 10 bits
                     v1 = ((buf[0]<<8) | buf[1])-0xD800;
                    //low 10 bits
                     v2 = ((pair[0]<<8) | pair[1])-0xDC00;


                    }

                    int v = ((v1<<10) | v2) + 0x10000;

                    byteValue = ((v&0x1fffff)>>18)| 0xF0;
                   

                    if(!safe_write(output_fd, &byteValue, 1,0,true)) {         
                     }  

                     byteValue2 = ((v&0x3ffff)>>12)|0x80;
                   

                    if(!safe_write(output_fd, &byteValue2, 1,0,true)) {         
                     }

                     byteValue3 = ((v&0xfff)>>6)|0x80;
                   
                    if(!safe_write(output_fd, &byteValue3, 1,0,true)) {         
                     }

                     byteValue4 = (v&0x3f)|0x80;
                    
                      
                    if(!safe_write(output_fd, &byteValue4, 1,0,true)) {         
                     }
                     if(optV>0){
                    	printTable16(optV,v,byteValue,byteValue2,byteValue3,byteValue4,4,inputFormat);
                    }
           		}

             }
             printFooter(optV);
             return true;

        }

        /* Read in UTF-8 Bytes */
ForceEncode:
        while((((bytes_read = read(input_fd, &read_value, 1)) == 1)&&inputFormat==1)||fencode==true) {
            /* Mask the most significant bit of the byte */
           
            unsigned char masked_value = read_value & 0x80;
        if(fencode==false){
            if(masked_value == 0x80) {
                if((read_value & UTF8_4_BYTE) == UTF8_4_BYTE ||
                   (read_value & UTF8_3_BYTE) == UTF8_3_BYTE ||
                   (read_value & UTF8_2_BYTE) == UTF8_2_BYTE) {
                    // Check to see which byte we have encountered
                    if(count == 000) {
                        count++[bytes] = read_value;
                    } else {
                        /* Set the file position back 1 byte */
                        if(lseek(input_fd, -1, SEEK_CUR) < 0) {
                            /*Unsafe action! Increment! */
                            safe_param = *(int*)++saftey_ptr;
                            /* failed to move the file pointer back */
                            perror("NULL");
                            goto conversion_done;
                        }
                        /* Encode the current values into UTF-16LE */
                        encode = true;
                    }
                }
                 else if((read_value & UTF8_CONT) == UTF8_CONT) {
                    /* continuation byte */
                    bytes[count++] = read_value;
                }
            } else {
                if(count == 000) {
                    /* US-ASCII */
                    bytes[count++] = read_value;
                    encode = true;
                } else {
                    /* Found an ASCII character but theres other characters
                     * in the buffer already.
                     * Set the file position back 1 byte.
                     */
                    if(lseek(input_fd, -1, SEEK_CUR) < 0) {
                        /*Unsafe action! Increment! */
                        safe_param = *(int*) ++saftey_ptr;
                        /* failed to move the file pointer back */
                        perror("NULL");
                        goto conversion_done;
                    }
                    /* Encode the current values into UTF-16LE */
                    encode = true;
                }
            }
        }
            
            
            /* If its time to encode do it here */
            if(encode||fencode) {

                int i, value = 0;
                bool isAscii = false;
                for(i=0; i < count; i++) {
                    if(i == 0) {
                        if((bytes[i] & UTF8_4_BYTE) == UTF8_4_BYTE) {
                            value = bytes[i] & 0x7;
                        } else if((bytes[i] & UTF8_3_BYTE) == UTF8_3_BYTE) {
                            value =  bytes[i] & 0xF;
                        } else if((bytes[i] & UTF8_2_BYTE) == UTF8_2_BYTE) {
                            value =  bytes[i] & 0x1F;
                        } else if((bytes[i] & 0x80) == 0) {
                            /* Value is an ASCII character */
                            value = bytes[i];
                            isAscii = true;
                        } else {
                            /* Marker byte is incorrect */
                            goto conversion_done;
                        }
                    } else {
                        if(!isAscii) {
                            value = (value << 6);
                            value = value|(bytes[i] & 0x3F);
                        } else {
                            /* How is there more bytes if we have an ascii char? */
                            goto conversion_done;
                        }
                    }
                }
                /* Handle the value if its a surrogate pair*/
                if(value >= SURROGATE_PAIR) {
                    int vprime = value - SURROGATE_PAIR;
                    int w1 = (vprime >> 10) + 0xD800;
                    int w2 = (vprime & 0x3FF) + 0xDC00;

                    /* write the surrogate pair to file */

                     if(!safe_write(output_fd, &w1, CODE_UNIT_SIZE,format,true)) {
                        /* Assembly for some super efficient coding 
                        asm("movl   $8, %esi\n\t"
                            "movl   $.LC0, %edi\n"
                            "movl   $0, %eax");
                        goto conversion_done;
                        */
                    }
                    if(!safe_write(output_fd, &w2, CODE_UNIT_SIZE,format,true)) {
                        /* Assembly for some super efficient coding 
                        asm("movl   $8, %esi\n\t"
                            "movl   $.LC0, %edi\n"
                            "movl   $0, %eax");
                        goto conversion_done;
                        */
                    }
                    
                } else {
                    /* write the code point to file */
                    
                    

                    if(!safe_write(output_fd, &value, CODE_UNIT_SIZE,format,true)) {
                        /* Assembly for some super efficient coding 
                        asm("movl   $8, %esi\n"
                            "movl   $.LC0, %edi\n"
                            "movl   $0, %eax");
                        goto conversion_done;
                        
                        */
                    }

                }

  
                printTable(optV,isAscii,count,value,bytes,false,format);
               

                /* Done encoding the value to UTF-16LE */
                encode = false;
                fencode=false;
                count = 0;
            }
        }

        /* If we got here the operation was a success! */
       if(count==0){
        success = true;
        }
        else{
            //clean up remaining
            fencode = true;
            goto ForceEncode;
        }
    }
conversion_done: 
	 printFooter(optV);
    return success;
    }


bool safe_write(int output_fd, void *value,size_t size, int format,bool form)
{
    bool success = true;
    ssize_t bytes_written;
    
    //circular shift
    unsigned int temp = *(int*)value;
    
    if((endianCheck()==true)&& (form==true)){
        temp = reverse(temp);
    }
    
    if(format == 2){  
    temp = circularShift(temp);
    }

    if((bytes_written = write(output_fd, &temp, size)) != size) {
        // The write operation failed// 
        fprintf(stdout, "Write to file failed. Expected %zu bytes but got %zd\n", size, bytes_written);
    }
   
    return success;
}
   
bool BOM_write(int output_fd,int format)
{
    bool success = true;
    ssize_t bytes_written;
    unsigned int BOM;
  
    if(format ==1){     //utf8

        BOM =0xbfbbef;
        if(endianCheck()){
        BOM= 0xEFBBBF;
        BOM=BOM<<8;
        }
        if((bytes_written = write(output_fd, &BOM, 3)) != 3) {
         
        fprintf(stdout, "Write to file EOM failed");
        }

    }
    if(format ==2){     //uft16be
        BOM= 0xFFFE;

        if(endianCheck()){
        BOM= 0xFEFF;
        BOM=BOM<<16;
        }

        if((bytes_written = write(output_fd, &BOM, 2)) != 2) {
         
        fprintf(stdout, "Write to file EOM failed");
        }   
    }
    if(format ==3){     //utf16le
        BOM= 0xFEFF;

        if(endianCheck()){
        BOM= 0xFFFE;
        BOM=BOM<<16;
        }

        if((bytes_written = write(output_fd, &BOM, 2)) != 2) {
         
        fprintf(stdout, "Write to file EOM failed");
     }
    }
    return success;
}

void printHeader(int optV){

    if(optV==1){
    	  fprintf(stderr,"+-------+-------+-----------+\n");
          fprintf(stderr, "| ASCII | Bytes | CodePoint |\n");
        
        
    }else if(optV==2){
    	fprintf(stderr,"+-------+-------+-----------+-----------+\n");
        fprintf(stderr, "| ASCII | Bytes | CodePoint | Input\t|\n");
    }else{
    	fprintf(stderr,"+-------+-------+-----------+-----------+---------------+\n");
        fprintf(stderr,"| ASCII | Bytes | CodePoint | Input\t|   Output\t|\n");
        }
    }
void printTable(int optV, bool isAscii,int count ,unsigned int value,unsigned char bytes[],bool identical,int format){

    
    int i =0;
           if(optV==1){
           		fprintf(stderr,"+-------+-------+-----------+\n");
                if(isAscii){
                	if(value <= 0x20){
                	fprintf(stderr, "| NONE\t| %i\t| U+%.04x%4s|\n",count,value," ");
                	}else{
                		if(value>0x10000){
                		fprintf(stderr, "| %c\t| %i\t| U+%.04x%3s|\n",(char)value,count,value," ");
                		}else{
                		fprintf(stderr, "| %c\t| %i\t| U+%.04x%4s|\n",(char)value,count,value," ");
                			}
               	 }
                }else{

                fprintf(stderr, "| NONE\t| %i\t| U+%.04x%4s|\n",count,value," " );
                    }
                }
            if (optV==2){
                fprintf(stderr,"+-------+-------+-----------+-----------+\n");
                if(isAscii){
                	if(value <=0x20){
                	fprintf(stderr, "| NONE\t| %i\t| U+%.04x%4s| 0x%x\t|\n",count,value," ",bytes[0]);
                	}
                	else {

                	fprintf(stderr, "| %c\t| %i\t| U+%.04x    | 0x%x\t|\n",(char)value,count,value,bytes[0]);
                	
                		}
                
                	}
                else{
	                	if(value>0x10000){
	                	fprintf(stderr, "| NONE\t| %i\t| U+%.04x%3s|0x",count,value," ");	
	                	}
	                	else{	
	               		 fprintf(stderr, "| NONE\t| %i\t| U+%.04x    |0x",count,value );
	               		}
		                for(i =0;i<count;i++){
		                    fprintf(stderr, "%x",bytes[i] );
		                }
		                fprintf(stderr, "\t|\n" );
                    }
                }


            if(optV>=3){

    		int output= value;
        	if(format ==3){
        		output=circularShiftTest(output);
        	}
            	fprintf(stderr,"+-------+-------+-----------+-----------+---------------+\n");
                if(isAscii){
                

                	if(value <= 0x20){
                		if(format==1){
                		fprintf(stderr, "| NONE\t| %i\t| U+%.04x    | 0x%x\t| 0x%x\t\t|\n",count,value,bytes[0],output );
                		}
                		else{

            			fprintf(stderr, "| NONE\t| %i\t| U+%.04x    | 0x%x\t| 0x%.04x\t|\n",count,value,bytes[0],output );
							}

                	}else{

				if(format==1){
					fprintf(stderr, "| %c\t| %i\t| U+%.04x    | 0x%x\t| 0x%x\t\t|\n",(char)value,count,value,bytes[0],output );
				}else{

					fprintf(stderr, "| %c\t| %i\t| U+%.04x    | 0x%x\t| 0x%.04x\t|\n",(char)value,count,value,bytes[0],output );
				}

               		
            		}
                }
                else{
                    //This means there's no surrogate pair
                if(value < SURROGATE_PAIR){
                  if (identical){
                  	 fprintf(stderr, "| NONE\t| %i\t| U+%.04x    |0x",count,value );
	               		
		                for(i =0;i<count;i++){
		                    fprintf(stderr, "%x",bytes[i] );
		                }
		                fprintf(stderr, "\t| 0x" );
		                for(i =0;i<count;i++){
		                    fprintf(stderr, "%x",bytes[i] );
		                }
		                fprintf(stderr, "\t|\n" );
                    }




                  else{  
	                fprintf(stderr,"| NONE\t| %i\t| U+%.04x    | 0x",count,value );

	                for(i =0;i<count;i++){
	                    fprintf(stderr, "%x",bytes[i] );
	                }
		               
		                	fprintf(stderr, "\t| 0x%.04x\t|\n",output );
		                
	                
	                }
            	}
                else{

                int vprime
                 = value - SURROGATE_PAIR;
                int w1 = (vprime >> 10) + 0xD800;
                int w2 = (vprime & 0x3FF) + 0xDC00; 

               if (identical){
                  	 fprintf(stderr, "| NONE\t| %i\t| U+%.04x    |0x",count,value );
	               		
		                for(i =0;i<count;i++){
		                    fprintf(stderr, "%x",bytes[i] );
		                }
		                fprintf(stderr, "| 0x" );
		                for(i =0;i<count;i++){
		                    fprintf(stderr, "%x",bytes[i] );
		                }
		                fprintf(stderr, "\t|\n" );
                    }	
                else{

                
                	fprintf(stderr, "| NONE\t| %i\t| U+%.04x   | 0x",count,value );
				
                for(i =0;i<count;i++){
                    fprintf(stderr, "%x",bytes[i] );
                }
                if(format==3){
                	fprintf(stderr, "| 0x%.02x%.02x\t|\n",circularShiftTest(w1),circularShiftTest(w2));
                }
                else{
                	fprintf(stderr, "| 0x%.02x%.02x\t|\n",w1,w2 );
                }


                	}
				}
       		}


        }


}

void printTable16(int optV,int codePt,int value,int value2,int value3,int value4,int count,int inputFormat){


printFooter(optV);
	
	if(optV>0){
		if(codePt>0x20&&codePt<=127){
		fprintf(stderr, "| %c\t| %i\t| U+%.04x%4s|",(char)value,1,value," ");
			}
			else if(codePt>=0x10000){
			fprintf(stderr, "| NONE\t| %i\t| U+%.04x%3s|",count,codePt," ");	
			}
		else{
		fprintf(stderr, "| NONE\t| %i\t| U+%.04x%4s|",count,codePt," ");
		}
	}
	if(optV>1){
		if(codePt<0x10000){
			if(inputFormat==3){
				fprintf(stderr, " 0x%.04x\t|",circularShiftTest(codePt));
			}else{
				fprintf(stderr, " 0x%.04x\t|",codePt);
			}

		}else{

			 int vprime = codePt - SURROGATE_PAIR;
             int w1 = (vprime >> 10) + 0xD800;
             int w2 = (vprime & 0x3FF) + 0xDC00;


			if(inputFormat==3){
				fprintf(stderr, " 0x%.04x%.04x|",circularShiftTest(w1),circularShiftTest(w2));
			}else{
				fprintf(stderr, " 0x%.04x%.04x|",w1,w2);
			}

		}
	}

	if(optV>2){
		
			if(count==1){
			fprintf(stderr, " 0x%x\t\t|",value);
			}
			else if(count==2){
			fprintf(stderr, " 0x%x%x\t|",value,value2);
			}
			else if(count==3){
			fprintf(stderr, " 0x%x%x%x\t|",value,value2,value3);
			}
			else{
			fprintf(stderr, " 0x%x%x%x%x\t|",value,value2,value3,value4);
			}
		
	}

fprintf(stderr,"\n");


}





void printTableBELE(int optV,int value,int inputFormat,int value2,bool identical){
int count =2;
int codePt =value;
int codePt2 =value2;

if (inputFormat==3){
	codePt=circularShiftTest(codePt);
	codePt2=circularShiftTest(codePt2);
}

printFooter(optV);
if(optV>0){
	if(codePt2==0){
			if(codePt>0x20&&codePt<=127){
		fprintf(stderr, "| %c\t| %i\t| U+%.04x%4s|",(char)codePt,count,codePt," ");
			}else{
		fprintf(stderr, "| NONE\t| %i\t| U+%.04x%4s|",count,codePt," ");
		}
	}else{
		fprintf(stderr, "| NONE\t| 4\t| U+%.04x%.04x|",codePt,codePt2);
	}

	}
if(optV>1&&identical==false){
	if (inputFormat==3){
		codePt=circularShiftTest(codePt);

	codePt2=circularShiftTest(codePt2);
	}
	if(codePt2==0){
		fprintf(stderr, " 0x%.04x%4s|",codePt," ");
		}
	else{
		fprintf(stderr, " 0x%.04x%.04x|",codePt,codePt2);
		}
	}
if(optV>2&&identical==false){
	if(codePt2==0){
	fprintf(stderr, " 0x%.04x%4s\t|",circularShiftTest(codePt)," ");
		}
	else{
	fprintf(stderr, " 0x%.04x%.04x\t|",circularShiftTest(codePt),circularShiftTest(codePt2));	
		}
	}

if(optV>1&&identical ==true){
	if (inputFormat==3){
		codePt=circularShiftTest(codePt);

	codePt2=circularShiftTest(codePt2);
	}
	if(codePt2==0){
		fprintf(stderr, " 0x%.04x%4s|",codePt," ");
		}
	else{
		fprintf(stderr, " 0x%.04x%.04x|",codePt,codePt2);
		}
	}
if(optV>2&&identical ==true){
	if(codePt2==0){
	fprintf(stderr, " 0x%.04x%4s\t|",codePt," ");
		}
	else{
	fprintf(stderr, " 0x%.04x%.04x\t|",codePt,codePt2);	
		}
	}	



fprintf(stderr,"\n");


}



void printFooter(int optV){
	if(optV==1){
	fprintf(stderr,"+-------+-------+-----------+\n");
	}
	if (optV==2){
		fprintf(stderr,"+-------+-------+-----------+-----------+\n");
	}
	if(optV==3){
		fprintf(stderr,"+-------+-------+-----------+-----------+---------------+\n");
	}
}

int circularShift(int value){


unsigned int temp = value;
unsigned int temp2 = temp<<8;
temp = temp >>8;
temp = temp|temp2;

return temp;


}
int circularShiftTest(int value){


unsigned int temp = value&255;
unsigned int temp2 = (value>>8)&255;
temp = temp <<8;
temp = temp|temp2;

return temp;


}

int BOM_Check(int input_fd){

unsigned char buf[3];
  
int bytes_read;
if(( bytes_read=  read(input_fd, &buf, 2))!=2){

fprintf(stderr, "BOM read check failed\n" );

}

//UTF8

    if(buf[0]==0xef&&buf[1]==0xbb){
        bytes_read =  read(input_fd, &buf, 1);

        if(buf[0]==0xbf){

            #ifdef CSE320
            cse320("Input Encoding","UTF-8");
            #endif

            return 1;
        }
        else{
            fprintf(stderr,"NOt valid BOM");
        }
    }
    //UTF16BE
    if(buf[0]==0xfe&&buf[1]==0xff){
        #ifdef CSE320
            cse320("Input Encoding","UTF-16BE");
        #endif
        return 2;
    }
    //UTF16LE
    if(buf[0]==0xff&&buf[1]==0xfe){
        #ifdef CSE320
            cse320("Input Encoding","UTF-16LE");
        #endif
        return 3;
    }




return -1;
}

bool endianCheck(){
    bool return_code = false;//false == little endian
    unsigned int i = 1;
        

        char *c = (char*)&i; // Convert the LSB into a character
        if(*c) {
        //little endian
        } 
        else {
        return_code =true;
        }

return return_code;
}

int reverse(int input){
    unsigned char c1, c2, c3, c4;

   
    c1 = input & 255;
    c2 = (input >> 8) & 255;
    c3 = (input >> 16) & 255;
    c4 = (input >> 24) & 255;

return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;


        
}
int calCodePt(int count, unsigned char bytes[]){
unsigned i =0;
unsigned int value = bytes[0];
if (count==4){
	value = value&0x07;
}
if (count==3){
	value = value&0x0F;
}
if (count==2){
	value = value&0x1F;
}
for(i=1;i<count;i++){
 value = (value << 6);
 value = value|(bytes[i] & 0x3F);
}
return value;
} 


