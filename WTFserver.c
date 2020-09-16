// this is the server
// test server

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>
#include "LL.c"
#include <dirent.h>
#include <errno.h>

struct threadNode{
    char* file_name;
    int socket;

};


pthread_mutex_t cre_lock;
pthread_mutex_t cur_lock;
pthread_mutex_t upd_lock;
pthread_mutex_t des_lock;
pthread_mutex_t che_lock;
pthread_mutex_t upg_lock;
pthread_mutex_t com_lock;
pthread_mutex_t pus_lock;

// new execution context for accepting commands
void * acceptCommands(void * temp){
    
    struct threadNode* node = (struct threadNode*)temp;

    int new_socket = node -> socket;
    
   //read in the command
    char * command = (char*)malloc(3 * sizeof(char));

    int cmd = read(new_socket, command, 3);

    //printf("This is the command:%s\n", command);



    // do appropriate stuff for each command

    if(strcmp(command, "cre") == 0){
        // create the .Manifest and send the contents of it back to the client
    
      //  printf("Did this enter?\n");
        chdir(".serverRepo");


        // read in the size of the project_name
        char * temp5 = (char*)malloc(20 * sizeof(char));

        char *x = (char*)malloc(1*sizeof(char));

        int tz;

        // read in until the delimeter is found (:)
        while(1){
            tz = read(new_socket, x, 1);
        
            if(strcmp(x, ":") == 0){
                break;
            }
        
            strcat(temp5, x);
        }

        // turn the file_size string into an integer
        int proj_name_size = atoi(temp5);

        

        // read the amount of bytes found above (size of the project_name)
        char * project_name = (char*)malloc(proj_name_size * sizeof(char)); // CHANGE THISSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSSS

        tz = read(new_socket, project_name, proj_name_size);
    


        //printf("This is project_name in the server:%s\n", project_name);        



        // make the project folder on the server side 
        struct stat temp = {0};
        // create new project folder on the server side
        if(stat(project_name, &temp) == -1 ){
            mkdir(project_name, 0700);
        }else{
            printf("Project folder already exists! Exiting.\n");
            //return -1;
        }


        pthread_mutex_lock(&cre_lock);
        
        chdir(project_name);

        // create .Manifest
        int rz = open(".Manifest", O_RDWR| O_CREAT| O_TRUNC, 0600);
        int iz = write(rz, "1", sizeof(char)); // .Manifest starts out with version number (1) and nothing else

        close(rz);

        
        // send manifest to client
        send(new_socket, "1", sizeof(char), 0);

	// Create the .History file
	int his = open(".History", O_RDWR | O_CREAT | O_APPEND, 0700);
	int crea = write(his, "create\n0\n\n", 10*sizeof(char));
	close(his);

        pthread_mutex_unlock(&cre_lock);
        chdir("..");
        chdir("..");

    } else if(strcmp(command,"cur")==0){
        // do currentversion

        chdir(".serverRepo");


        //read in project name size
        char* num = (char*)malloc(5 * sizeof(char));

        char* x = (char*)malloc(1*sizeof(char));


        int tz;


        // read until delimeter found (:)
        
        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turn file size string to int
        int proj_name_size = atoi(num);


        // read amount of bytes found above (size of project name) for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);

        //printf("This is the project name received by server: '%s'", project_name);

        
        pthread_mutex_lock(&cur_lock);

        if(chdir(project_name)== -1){
            perror("Error entering project_name");
            
            pthread_mutex_unlock(&upd_lock);
            exit(1);
        }
        


        int manNum = 0;

        // create linked list for the manifest file
        struct Node* start = createLL(&manNum);

        struct Node* front = start;

        // num files
        int count = 0;
        

        // buff is fully concatenated string
        char * buff = (char*)malloc(1*sizeof(char));
        int buffCount= 0;
        int buffSize = 1;


        while(front != NULL){
            count++;
            front = front->next;
        }


        // concatenates version numbers, file names, and num files into one big string
        // Format of this is as given in the project description -> <numberOfFiles><:><versionNumber><:><projectNameLength><:><projectName>
        while(buffCount+(count+1)> buffSize){
            buff = realloc(buff, 2*buffSize);
            buffSize = 2*buffSize;
        }

        char * countChar = malloc(count * sizeof(char));
        snprintf(countChar, count+1, "%d", count);

        strcat(buff, countChar);
        buffCount+=(count+1);


        
         if(buffCount+ 1 > buffSize){
            buff = realloc(buff, 2*buffSize);
            buffSize = 2 * buffSize;
         }

         strcat(buff, ":");
         buffCount++;



        while(start != NULL){

            // versionNum
            int len = snprintf(NULL, 0, "%d", start->versionNum);
            char* lenChar = malloc(len+1);
            snprintf(lenChar, len+1, "%d", start->versionNum);


            while(buffCount+ len > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff, lenChar);
            buffCount+= len;


            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }
            strcat(buff, ":");
            buffCount++;

            
            char* proj_name = (char*)malloc(strlen(start->projName)*sizeof(char));
            strcpy(proj_name, start->projName);


            
            while(buffCount+ len > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            //add the proj_name size

            len = strlen(proj_name);
            char * proj_name_len = malloc(len);
            snprintf(proj_name_len, len+1, "%d", len);
            

            strcat(buff, proj_name_len);
            buffCount+=len;

            
            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }
            strcat(buff, ":");
            buffCount++;




            while(buffCount+ (strlen(proj_name)+1) > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }


            strcat(buff, proj_name);
            buffCount+=(strlen(proj_name)+1); 
            


            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff, ":");
            buffCount++;


//            printf("Added %s to buffer string \n", start->projName);
//            printf("This is buff:%s\n", buff);
            start = start->next;
        }
    

    
    

    	// add the length of buff to the front and send to the socket
    	// Final version of this string is -> <lengthOfTHISBuffer><:><numberOfFiles><:><versionNumber><:><projectNameLength><:><projectName>
    	int buffLen = strlen(buff);
    	char* buffv2 = (char*)malloc((buffLen+10) * sizeof(char));
    	snprintf(buffv2, buffLen+1, "%d", buffLen);
	
    	strcat(buffv2, ":");

    	strcat(buffv2, buff);
    	strcat(buffv2, ":");

    	//printf("This is buffv2:%s\n", buffv2);

    	// Send back to client
    	int bz = send(new_socket, buffv2, strlen(buffv2), 0);


        pthread_mutex_unlock(&cur_lock);
    	// chdir back out
    	chdir("..");
    	chdir("..");

    } else if(strcmp(command, "upd") == 0){
	if(chdir(".serverRepo") == -1){
		perror("chdir into serverrepo failed\n");
        pthread_mutex_unlock(&upd_lock);
        exit(1);
	}	
	
	// Read in the Project Name Size
	char* num = (char*)malloc(5 * sizeof(char));
        char* x = (char*)malloc(1*sizeof(char));
        int tz;

        // read until delimeter found (:)
        while(1){
            tz = read(new_socket, x, 1);
            if(strcmp(x, ":") == 0){
                break;
            }
            strcat(num, x);
        }

        // turn file size string to int
        int proj_name_size = atoi(num);
	
        // read amount of bytes found above (size of project name) for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);

        //printf("This is the project name received by server: '%s'", project_name);


        
        pthread_mutex_lock(&upd_lock);

        if(chdir(project_name)== -1){
            perror("Error entering project_name");
            pthread_mutex_unlock(&upd_lock);
            exit(1);
        }
        
	int fd = open(".Manifest", O_RDONLY, 0);
	if(fd == -1){
		perror("Error with .Manifest\n");
        pthread_mutex_unlock(&upd_lock);
        exit(1);
	}

	struct stat v;
	stat(".Manifest", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);

	//printf("This is the .Manifest file located in the serverRepo: '%s'\n", data);

	int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

	char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	strcat(returnString, lenChar);
	strcat(returnString, ":");
	strcat(returnString, data);

	//printf("This is the final return string: '%s'\n", returnString);
	
	// Send back to the client
	int rd = send(new_socket, returnString, strlen(returnString), 0); 

    

    pthread_mutex_unlock(&upd_lock);
    chdir("..");
    chdir("..");


    } else if(strcmp(command, "des") == 0){
        

        
        pthread_mutex_lock(&des_lock);
        chdir(".serverRepo");

        
        // read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));

        int tz;

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);

        
        // delete everything

        //printf("This is the project_name before destroying: %s\n", project_name);

	

	char * moving2 = malloc( (7 + proj_name_size) * sizeof(char));
        
    	
    	strcat(moving2, "rm ");
    	strcat(moving2, "-r ");
    	strcat(moving2, project_name);
    	//strcat(moving2, manVer);
    
    	//printf("This is moving: '%s'\n", moving2);
    	system(moving2);
    
	


/*
        DIR* projName = opendir(project_name);
        struct dirent* file;
        char pathName[256];


        while( (file = readdir(projName))!= NULL){

        //printf("Did this enter?\n");
            if(strcmp(file->d_name, "..") ==0 || strcmp(file->d_name, ".") == 0){ 
               continue;  
           } 

           sprintf(pathName, "%s/%s", project_name, file->d_name);
            
           //printf("This is the pathName:%s", pathName);
           
            int x = remove(pathName);
           if(x == -1){
               
               printf("Remove failed\n");
               pthread_mutex_unlock(&des_lock);
               exit(1);
           }
        }

        closedir(projName);
        rmdir(project_name);
*/

        //printf("Successfully destroyed %s\n", project_name);



        pthread_mutex_unlock(&des_lock);
        chdir("..");





    } else if(strcmp(command, "che") == 0){
        
        
        chdir(".serverRepo");
        
        // read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));

        int tz;

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);


        //printf("This is the project name on the server side:%s\n", project_name);
        

        // now send manifest and all files and directories under project_name back to the client
       

        pthread_mutex_lock(&che_lock);
        
        if(chdir(project_name)== -1){
            perror("Error entering project_name");
            
            pthread_mutex_unlock(&che_lock);
            exit(1);
        }



        int fd = open(".Manifest", O_RDONLY, 0);
        if(fd == -1){
            perror("Error with .Manifest\n");
		    pthread_mutex_unlock(&che_lock);
            exit(1);
	    }

	struct stat v;
	stat(".Manifest", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);

	//printf("This is the .Manifest file located in the serverRepo: '%s'\n", data);

	int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

	char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	strcat(returnString, lenChar);
	strcat(returnString, ":");
	strcat(returnString, data);

	//printf("This is the Manifest sent to the client: '%s'\n", returnString);
	
	// Send back to the client
	int rd = send(new_socket, returnString, strlen(returnString), 0); 

    
    int serverManVer = 0;
    struct Node* serverLL = createLL(&serverManVer);

    // now analyze the linked list into a buffer with all the files in the project 
    



    struct Node* front =  serverLL;
    int count = 0;


   // buff is fully concatenated string
        char * buff = (char*)malloc(1*sizeof(char));
        int buffCount= 0;
        int buffSize = 1;


        while(front != NULL){
            count++;
            front = front->next;
        }


        // concatenates version numbers, file names, and num files into one big string
        // Format of this is as given in the project description -> <numberOfFiles><:><versionNumber><:><projectNameLength><:><projectName>
        while(buffCount+(count+1)> buffSize){
            buff = realloc(buff, 2*buffSize);
            buffSize = 2*buffSize;
        }

        char * countChar = malloc(count * sizeof(char));
        snprintf(countChar, count+1, "%d", count);

        strcat(buff, countChar);
        buffCount+=(count+1);


        
         if(buffCount+ 1 > buffSize){
            buff = realloc(buff, 2*buffSize);
            buffSize = 2 * buffSize;
         }

         strcat(buff, ":");
         buffCount++;


        struct Node* start = serverLL;

            while(start != NULL){

           
            char* proj_name = (char*)malloc(strlen(start->projName)*sizeof(char));
            strcpy(proj_name, start->projName);


            
            while(buffCount+ len > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            //add the file_name_size

            len = strlen(proj_name);
            char * proj_name_len = malloc(len);
            snprintf(proj_name_len, len+1, "%d", len);
            

            strcat(buff, proj_name_len);
            buffCount+=len;

            
            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }
            strcat(buff, ":");
            buffCount++;




            while(buffCount+ (strlen(proj_name)+1) > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }


            strcat(buff, proj_name);
            buffCount+=(strlen(proj_name)+1); 
            


            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff, ":");
            buffCount++;


            // add size of file contents 
        int fd = open(start->projName, O_RDONLY, 0);
        if(fd == -1){
            perror("Error with start->fileName\n");

            pthread_mutex_unlock(&che_lock);
		    exit(1);
	    }

	struct stat v;
	stat(start->projName, &v);
	int size = v.st_size-1;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);

//	printf("This is the start->projName file contents %s\n", data);


      


    int len = snprintf(NULL, 0, "%d", size);
    char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

    while(buffCount+ len > buffSize){
            buff = realloc(buff, 2*buffSize);
            buffSize = 2 * buffSize;
    }

    strcat(buff, lenChar);
    buffCount+=len;




            // add colon

            if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff, ":");
            buffCount++;




            // add filecontents

             while(buffCount+ (strlen(data)) > buffSize){ // +1???????????????????????????
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }


            strcat(buff, data);
            buffCount+=(strlen(data)); // +1?????????????????? 
            
            
            // add colon

              if(buffCount+ 1 > buffSize){
                buff = realloc(buff, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff, ":");
            buffCount++;




//            printf("Added %s to buffer string \n", start->projName);
//            printf("This is buff:%s\n", buff);
            start = start->next;
        }
    
        //printf("buff:%s\n", buff);
       

        int buffLen = strlen(buff);
    	char* buffv2 = (char*)malloc((buffLen+10) * sizeof(char));
    	snprintf(buffv2, buffLen+1, "%d", buffLen);
	
    	strcat(buffv2, ":");
    	strcat(buffv2, buff);
    	strcat(buffv2, ":");

    	//printf("This is buffv2:%s\n", buffv2);

    	// Send back to client
    	
        //printf("buff before sent back to client:%s\n", buffv2);
        
        int bz = send(new_socket, buffv2, strlen(buffv2), 0);



        pthread_mutex_unlock(&che_lock);

        chdir("..");
        chdir("..");
    }else if(strcmp(command, "upg") == 0){ // This is upgrade 
        chdir(".serverRepo");

	// READING THE PROJECT
        // read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));
        int tz;

        // read until delimter (:) found
        while(1){
            tz = read(new_socket, x, 1);
            if(strcmp(x, ":") == 0){
                break;
            }
            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);

        //printf("This is the project name on the server side:%s\n", project_name);
        

	// READING THE FILE
	// read in file name size
        char * numb = (char*)malloc(5 * sizeof(char));
        char* y =(char*)malloc(sizeof(char));

        // read until delimter (:) found
        while(1){
            tz = read(new_socket, y, 1);
            if(strcmp(y, ":") == 0){
                break;
            }
            strcat(numb, y);
        }

        // turning file size string into int
        int file_name_size = atoi(numb);

        // read amount of bytes found above from the socket for the project name
        char * file_name = (char*)malloc(file_name_size * sizeof(char));

        tz = read(new_socket, file_name, file_name_size);

      //  printf("This is the file name on the server side:%s\n", file_name);
        
        

        
        pthread_mutex_lock(&upg_lock);
	
        // Now we chdir to the projName	
        if(chdir(project_name)== -1){
            perror("Error entering project_name");
            pthread_mutex_unlock(&upg_lock);
            exit(1);
        }

	// Now let's open the file 
	int fd = open(file_name, O_RDONLY, 0);
        if(fd == -1){
            perror("Error with file name\n");
            pthread_mutex_unlock(&upg_lock);
		    exit(1);
	}

	struct stat v;
	stat(file_name, &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);

	//printf("This is the file_name file located in the serverRepo: '%s'\n", data);

	int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

	char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	strcat(returnString, lenChar);
	strcat(returnString, ":");
	strcat(returnString, data);

	//printf("This is the file_name sent to the client: '%s'\n", returnString);
	
	// Send back to the client
	int rd = send(new_socket, returnString, strlen(returnString), 0); 

	
	// Now we want to work with the Manifest
        fd = open(".Manifest", O_RDONLY, 0);
        if(fd == -1){
            perror("Error with .Manifest\n");
            pthread_mutex_unlock(&upg_lock);
		    exit(1);
	    }

	struct stat u;
	stat(".Manifest", &u);
	int sizeM = v.st_size;
	char *dataM = malloc(size * sizeof(char));
	int retM = read(fd, dataM, sizeM);

	//printf("This is the .Manifest file located in the serverRepo: '%s'\n", dataM);

	int lenM = snprintf(NULL, 0, "%d", sizeM);
        char* lenCharM = malloc(lenM+1);
	snprintf(lenCharM, lenM+1, "%d", sizeM);

	char *returnStringM = (char*)malloc( (sizeM+lenM+1) * sizeof(char) );
	strcat(returnStringM, lenChar);
	strcat(returnStringM, ":");
	strcat(returnStringM, dataM);

	//printf("This is the Manifest sent to the client: '%s'\n", returnStringM);
	
	// Send back to the client
	rd = send(new_socket, returnStringM, strlen(returnStringM), 0); 

    
        pthread_mutex_unlock(&upg_lock);
	
        chdir("..");
        chdir("..");


    }else if(strcmp(command, "com") == 0){
        // commit 
        
        chdir(".serverRepo");
        
        // read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));

        int tz;

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);


        //printf("This is the project name on the server side:%s\n", project_name);
       
        
        
        pthread_mutex_lock(&com_lock);
        
        
        if(chdir(project_name)== -1){
           perror("Error entering project_name");
           pthread_mutex_unlock(&com_lock);
           exit(1);
        }



        int fd = open(".Manifest", O_RDONLY, 0);
        if(fd == -1){
           perror("Error with .Manifest\n"); 
           pthread_mutex_unlock(&com_lock);
		   exit(1);
	    }

	    struct stat v;
	    stat(".Manifest", &v);
	    int size = v.st_size;
	    char *data = malloc(size * sizeof(char));
	    int ret = read(fd, data, size);

	    //printf("This is the .Manifest file located in the serverRepo: '%s'\n", data);

	    int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	    snprintf(lenChar, len+1, "%d", size);

	    char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	    strcat(returnString, lenChar);
	    strcat(returnString, ":");
	    strcat(returnString, data);

	    //printf("This is the Manifest sent to the client: '%s'\n", returnString);
	
	    // Send back to the client
	    int rd = send(new_socket, returnString, strlen(returnString), 0); 



        // READ in .Commit, and write a 1 at the beginning

        char * num1 = (char*)malloc(5 * sizeof(char));
        char* z =(char*)malloc(sizeof(char));

        

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, z, 1);

            if(strcmp(z, ":") == 0){
                break;
            }

            strcat(num1, z);
        }

        // turning file size string into int
        int commit_file_size = atoi(num1);

        // read amount of bytes found above from the socket for the project name
        char * commit_contents = (char*)malloc(commit_file_size * sizeof(char));

        tz = read(new_socket, commit_contents, commit_file_size);


        //printf("This is the Commit file on the server side:%s\n", commit_contents);


        int pz = open(".Commit", O_RDWR | O_CREAT, 0700);
        

        int kz = write(pz, commit_contents, commit_file_size);




        close(pz);
        printf("Commit was a success!\n");



        pthread_mutex_unlock(&com_lock);

        chdir("..");
        chdir("..");

    }else if(strcmp(command, "pus") == 0){
        
        chdir(".serverRepo");
        
        // read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));

        int tz;

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);



        //printf("This is the project name on the server side:%s\n", project_name);
        


        pthread_mutex_lock(&pus_lock);
        
        if(chdir(project_name) == -1){
            perror("Error chdir into project name\n");
            pthread_mutex_unlock(&pus_lock);
            exit(1);
        }
        
        // READ in .Commit, and write a 1 at the beginning

        char * num1 = (char*)malloc(5 * sizeof(char));
        char* z =(char*)malloc(sizeof(char));

        

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, z, 1);

            if(strcmp(z, ":") == 0){
                break;
            }

            strcat(num1, z);
        }

        // turning file size string into int
        int commit_file_size = atoi(num1);

        // read amount of bytes found above from the socket for the project name
        char * commit_contents = (char*)malloc(commit_file_size * sizeof(char));

        tz = read(new_socket, commit_contents, commit_file_size);


        //printf("This is the Commit file on the server side:%s\n", commit_contents);


        int pz = open(".Commit", O_RDWR | O_CREAT, 0700);
        

        int kz = write(pz, commit_contents, strlen(commit_contents));


        chdir(".."); // Back in .serverRepo

	
        // read in the size of the whole concatenated string
        char * temp10 = (char*)malloc(5 * sizeof(char));

        char *l = (char*)malloc(1*sizeof(char));

        int yz;

        // read in until the delimeter is found (:)
        while(1){
            yz = read(new_socket, l, 1);
        
            if(strcmp(l, ":") == 0){
                break;
            }
        
            strcat(temp10, l);
        }

        // turn the file_size string into an integer
        int buffSize = atoi(temp10);


        char * serverString = (char*)malloc(buffSize*sizeof(char));

        yz = read(new_socket, serverString, buffSize);

	//printf("This is serverString: '%s'\n", serverString);
	




        // this is to make the system call to zip previous version of proj_name
        char* systemCall = malloc((8 + proj_name_size + 5 + proj_name_size+1) * sizeof(char));

        strcat(systemCall, "tar czf ");
        strcat(systemCall, project_name);
        strcat(systemCall, ".tgz ");
        strcat(systemCall, project_name);

        //printf("This is systemCall: '%s'\n", systemCall);


        system(systemCall);
        

        char * moving = malloc( (3 + proj_name_size + 4 + 1 + proj_name_size+1) * sizeof(char));
        
        strcat(moving, "mv ");
        strcat(moving, project_name);
        strcat(moving, ".tgz ");
        strcat(moving, project_name);

        //printf("This is moving: '%s'\n", moving);

        system(moving);


        if(chdir(project_name) == -1){
            //printf("Error entering project_name folder\n");
            pthread_mutex_unlock(&pus_lock);
            exit(1);
        }


        // get man ver num
        int mz = open(".Manifest", O_RDONLY);
        char * manVer = (char*)malloc(sizeof(char));
        int ret = read(mz, manVer, 1);
        //printf("This is manVer: '%s'\n", manVer);



        // make the project folder on the server side 
        /* struct stat temp = {0};
        // create new project folder on the server side
        if(0 != access(".prevVersions", F_OK)){
		if(ENOENT == errno){
			mkdir(".prevVersions", 0700);
		}
	}
	//if(stat(".prevVersions", &temp) == -1 ){
        //   mkdir(".prevVersions", 0700);
        //}
	*/
        if(chdir(".prevVersions") == -1){
                mkdir(".prevVersions", 0700);
		if(chdir(".prevVersions") == -1){
			printf("Error entering .prevVersions\n");
                	pthread_mutex_unlock(&pus_lock);
                	exit(1);
                }
      	}

	chdir("..");      

        char * moving1 = malloc( (3 + proj_name_size + 4 + 1 + 14) * sizeof(char));
        
        strcat(moving1, "mv ");
        strcat(moving1, project_name);
        strcat(moving1, ".tgz ");
        strcat(moving1, ".prevVersions");

       	//printf("This is moving: '%s'\n", moving1);
            
        system(moving1);
            
            
        if(chdir(".prevVersions") == -1){
        	printf("Error entering .prevVersions\n");
                pthread_mutex_unlock(&pus_lock);
                exit(1);
       	}
        


    	mkdir(manVer, 0700);
    
    	char * moving2 = malloc( (3 + proj_name_size + 4 + 1 + 2) * sizeof(char));
        
    	
    	strcat(moving2, "mv ");
    	strcat(moving2, project_name);
    	strcat(moving2, ".tgz ");
    	strcat(moving2, manVer);
    
    	//printf("This is moving: '%s'\n", moving2);
    	system(moving2);
    

    	chdir(".."); // In the project
	
	// Case 'D'
	// We have now finished copying the previous version of the project. We're now going to read through the .Commit file and act on the changes
	struct commitNode* start1 = createCommitLL();	
 	struct commitNode* historyCopy = start1;

	// We have to go through the LL and do those changes
	while(start1 != NULL){
		if(strcmp(start1->mode, "D") == 0){
			// The easiest case, delete the file if it's marked with D
			int d = remove(start1->projName);
		}		
		start1 = start1->next;
	}	
	
	// (Re)create the new 'U' and 'A' files
	// On the alternative, U and A are not that easy. We read through the server string that has the content of everything in the Commit
	// It's in the formate <Mode><:><Bytes of Project Name><:><Project Name><:><Bytes of Content><:><Content><...>
	char delim[] = ":";	
	char *ptr = strtok(serverString, delim); 

	while(ptr != NULL){
		if(strcmp(ptr, "A") == 0 || strcmp(ptr, "U") == 0){
			ptr = strtok(NULL, delim); // This bring us to <Bytes of Project Name>
			int nameOfProjBytes = atoi(ptr);
			ptr = strtok(NULL, delim); // This brings us to <Project Name>
			char* projectName = ptr;
			
			// Since we're overwriting a file potentially, let's just remove it first
			remove(projectName);
			// Open a new file
			int nfz = open(projectName, O_RDWR | O_CREAT, 0700);

			ptr = strtok(NULL, delim); // This brings us to <Bytes of Content>
			int contentBytes = atoi(ptr);
			ptr = strtok(NULL, delim); // This brins us to <Content>
			char* content = ptr;

			int cw = write(nfz, content, strlen(content)); // Write the content to the file
			
			close(nfz); // Close the file descriptor

			//printf("Bytes: '%d' and Proj: '%s' and contentBytes: '%d' and content: '%s'\n", nameOfProjBytes, projectName, contentBytes, content);
		}

		ptr = strtok(NULL, delim);	
	}	

	// We need to update our manifest file so compare both the changes in Commit and the actual .Manifest
	int manVersion = 0;
	struct Node* originalManifest = createLL(&manVersion);
	struct commitNode* start = createCommitLL(); // Reference Linked List for the commit file
	++manVersion; // Increase the version of the .Manifest

	int rem = remove(".Manifest"); // We're rewriting the .Manifest file so we can remove it
	if(rem == 0){
		printf("Removed .Manifest\n");
	}
	int nman = open(".Manifest", O_RDWR | O_CREAT | O_APPEND, 0700);
	

	// Convert the new Manifest version
	int le = snprintf(NULL, 0, "%d", manVersion);
	char* leChar = malloc(le + 1);
	snprintf(leChar, le + 1, "%d", manVersion);
	int manVersionWrite = write(nman, leChar, le*sizeof(char));
	int nl = write(nman, "\n", sizeof(char));	

	//printf("This is leChar: '%s'\n", leChar);
	
	// This is case U for updating files to the Manifest as well as adding any irrelevant entries
	while(originalManifest != NULL){
		int found = 0;
		int aCase = 0;
		struct commitNode* copy = start;
		while(copy != NULL){
			if(strcmp(copy->projName, originalManifest->projName) == 0){
				if(strcmp(copy->mode, "U") == 0){ // We're going to update the file if it's marked with U
					found = 1;
					break;
				}
				if(strcmp(copy->mode, "A") == 0){
					aCase = 1;
					break;
				}
			}
			copy = copy->next;
		}

		if(aCase == 1){
			aCase = 0;
			continue;
		}

		if(found == 1){ // We found something in the original Manifest that was in the commit file as well. Here we use commit's content
			// Convert the version number to a string and write it (and add a space)
			
			int len = snprintf(NULL, 0, "%d", copy->versionNum);
			char* lenChar = malloc(len + 1);
			snprintf(lenChar, len + 1, "%d", copy->versionNum);	
			int vn = write(nman, lenChar, sizeof(char));
			int sn = write(nman, "\t", sizeof(char));

			// Write the file name and a space
			int fn = write(nman, copy->projName, strlen(copy->projName));
			int s = write(nman, "\t", sizeof(char));
	
			// Write the hash and a new line
			int hn = write(nman, copy->hash, strlen(copy->hash));
			int g = write(nman, "\n", sizeof(char));			
			
			//printf("We updated verNum: '%s', projName: '%s', hash: '%s'\n", lenChar, copy->projName, copy->hash);

			found = 0;
		} else { // The file wasn't in the commit so we just copy 
			// Convert the version number to a string and write it (and add a space)
			int len = snprintf(NULL, 0, "%d", originalManifest->versionNum);
			char* lenChar = malloc(len + 1);
			snprintf(lenChar, len + 1, "%d", originalManifest->versionNum);	
			int vn = write(nman, lenChar, sizeof(char));
			int sn = write(nman, "\t", sizeof(char));

			// Write the file name and a space
			int fn = write(nman, originalManifest->projName, strlen(originalManifest->projName));
			int s = write(nman, "\t", sizeof(char));
	
			// Write the hash and a new line
			int hn = write(nman, originalManifest->hash, strlen(originalManifest->hash));
			int g = write(nman, "\n", sizeof(char));				

			//printf("We added garbage verNum: '%s', projName: '%s', hash: '%s'\n", lenChar, originalManifest->projName, originalManifest->hash);


		}

		originalManifest = originalManifest->next;
	}	

	// Case 'A' for any remaining files that need to be added to the .Manifest
	while(start != NULL){
		if(strcmp(start->mode, "A") == 0){ // If a file was added, add the contents to the .Manifest
			int len = snprintf(NULL, 0, "%d", start->versionNum);
			char* lenChar = malloc(len + 1);
			snprintf(lenChar, len + 1, "%d", start->versionNum);	
			int vn = write(nman, lenChar, sizeof(char));
			int sn = write(nman, "\t", sizeof(char));

			// Write the file name and a space
			int fn = write(nman, start->projName, strlen(start->projName));
			int s = write(nman, "\t", sizeof(char));
	
			// Write the hash and a new line
			int hn = write(nman, start->hash, strlen(start->hash));
			int g = write(nman, "\n", sizeof(char));			
			
			//printf("We added verNum: '%s', projName: '%s', hash: '%s'\n", lenChar, start->projName, start->hash);

		
		}		

		start = start->next;
	}

	close(nman);


	int fd = open(".Manifest", O_RDONLY, 0);
	if(fd == -1){
		perror("Error with .Manifest\n");
        pthread_mutex_unlock(&pus_lock);
        exit(1);
	}

	struct stat v;
	stat(".Manifest", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	ret = read(fd, data, size);

	//printf("This is the .Manifest file located in the serverRepo: '%s'\n", data);

	int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

	char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	strcat(returnString, lenChar);
	strcat(returnString, ":");
	strcat(returnString, data);

	//printf("This is the final return string: '%s'\n", returnString);
	
	// Send back to the client
	int rd = send(new_socket, returnString, strlen(returnString), 0); 

	// Write to the .History file
	// Write all of historyCopy
	int hiz = open(".History", O_RDWR | O_APPEND, 0700);
	if(hiz < 0){
		printf("Error writing to History file\n");
	    	pthread_mutex_unlock(&pus_lock);
		exit(1);
	}
	int pu = write(hiz, "push\n", 5*sizeof(char));

	while(historyCopy != NULL){
		int mo = write(hiz, historyCopy->mode, strlen(historyCopy->mode));
		int sp = write(hiz, " ", sizeof(char));

		// Convert the version number to a string and write it (and add a space)
		int len = snprintf(NULL, 0, "%d", historyCopy->versionNum);
		char* lenChar = malloc(len + 1);
		snprintf(lenChar, len + 1, "%d", historyCopy->versionNum);	
		int vn = write(hiz, lenChar, sizeof(char));
		int sn = write(hiz, " ", sizeof(char));
											
		int pn = write(hiz, historyCopy->projName, strlen(historyCopy->projName));

		int hn = write(hiz, historyCopy->hash, strlen(historyCopy->hash));
		sp = write(hiz, " ", sizeof(char));

		historyCopy = historyCopy->next;
	}


    pthread_mutex_unlock(&pus_lock);


    } else if(strcmp(command, "his") == 0){
	chdir(".serverRepo");

	// read in project name size
        char * num = (char*)malloc(5 * sizeof(char));
        char* x =(char*)malloc(sizeof(char));

        int tz;

        // read until delimter (:) found
        

        while(1){
            tz = read(new_socket, x, 1);

            if(strcmp(x, ":") == 0){
                break;
            }

            strcat(num, x);
        }

        // turning file size string into int
        int proj_name_size = atoi(num);

        // read amount of bytes found above from the socket for the project name
        char * project_name = (char*)malloc(proj_name_size * sizeof(char));

        tz = read(new_socket, project_name, proj_name_size);



        //printf("This is the project name on the server side:%s\n", project_name); 


        pthread_mutex_lock(&pus_lock);
        
        if(chdir(project_name) == -1){
            perror("Error chdir into project name\n");
            pthread_mutex_unlock(&pus_lock);
            exit(1);
        }
        	
	int fd = open(".History", O_RDONLY, 0);
	if(fd == -1){
		perror("Error with .History\n");
        	pthread_mutex_unlock(&pus_lock);
        	exit(1);
	}

	struct stat v;
	stat(".History", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);

	//printf("This is the .History file located in the serverRepo: '%s'\n", data);

	int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	snprintf(lenChar, len+1, "%d", size);

	char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	strcat(returnString, lenChar);
	strcat(returnString, ":");
	strcat(returnString, data);

	//printf("This is the final return string: '%s'\n", returnString);
	
	// Send back to the client
	int rd = send(new_socket, returnString, strlen(returnString), 0); 

	chdir(".."); // Chdir to .serverRepo
	chdir(".."); //Chdir back to main

	pthread_mutex_unlock(&pus_lock);

    }
       
    return NULL;
}



int main(int argc,char**argv){

    struct stat temp = {0};

    // hidden server repo
    if(stat(".serverRepo", &temp)== -1){
        mkdir(".serverRepo", 0700);
    }



    int port = atoi(argv[1]);

    int server_fd, new_socket, valread;

    struct sockaddr_in address;

    int opt = 1;
    int addrlen = sizeof(address);

    char buff[50] = {0};
    char* hello = "This message was sent from the server";



    /*
     socket()
     bind()
     listen()
     accept()
    */

    // create the socket file descriptor

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0){
        perror("Socket failed to create\n");
        exit(EXIT_FAILURE);
    }


    // setsockopt (attach socket to port)
    
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))){
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    //binding socket

    if(bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0){
        perror("bind failed");
        exit(EXIT_FAILURE);

    }

    // server listen

    if(listen(server_fd, 3)< 0){
        perror("listen error");
        exit(EXIT_FAILURE);
    }

    // accept
    
    do{
    if( (new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen) )< 0){
        perror("accept error");
        exit(EXIT_FAILURE);
    } 




    pthread_mutex_init(&cre_lock, NULL);
    pthread_mutex_init(&cur_lock, NULL);
    pthread_mutex_init(&upd_lock, NULL);
    pthread_mutex_init(&des_lock, NULL);
    pthread_mutex_init(&che_lock, NULL);
    pthread_mutex_init(&upg_lock, NULL);
    pthread_mutex_init(&com_lock, NULL);
    pthread_mutex_init(&pus_lock, NULL);

    struct threadNode* node = (struct threadNode*)malloc(sizeof(struct threadNode));
   // node -> file_name = (char *)malloc(file_size * sizeof(char));

    node -> socket = new_socket;
   // strcpy(node->file_name, buff);

    pthread_t thread;

    pthread_create(&thread, NULL, acceptCommands, (void*)node);

    pthread_join(thread, NULL);



    pthread_mutex_destroy(&cre_lock);
    pthread_mutex_destroy(&cur_lock);
    pthread_mutex_destroy(&upd_lock);
    pthread_mutex_destroy(&des_lock);
    pthread_mutex_destroy(&che_lock);
    pthread_mutex_destroy(&upg_lock);
    pthread_mutex_destroy(&com_lock);
    pthread_mutex_destroy(&pus_lock);




/*   
    valread = read(new_socket, buff, 50);
   // printf("%s\n", buff);


    int fd = open(buff, O_RDONLY);
    int rfd;
    //printf("This is fd size: %d\n", fd);

    struct stat buf;
    
    if(stat("temp.txt", &buf) == -1){
        printf("There was an error in the size of the file\n");
        return -1;
    }
    ssize_t file_size = (ssize_t)buf.st_size;

    rfd = read(fd, buff, file_size);
*/


/*
    struct threadNode* node = (struct threadNode*)malloc(sizeof(struct threadNode));
    node -> file_name = (char *)malloc(file_size * sizeof(char));

    node -> socket = new_socket;
    strcpy(node->file_name, buff);

    pthread_t thread;

    pthread_create(&thread, NULL, requestFile, (void*)node);

    pthread_join(thread, NULL);
  */  
 

 //   send(new_socket, buff, strlen(buff), 0); // new_socket
   
 


    close(new_socket);
    }while(1);

   
        
    printf("Message was sent successfully to server\n");
    return 0;


}
