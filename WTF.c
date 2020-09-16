// client
// test client

#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <openssl/sha.h>
#include <signal.h>
#include "LL.c"

#define PORT 10001
struct threadNode{
    char* file_name;
    int socket;

};


void * requestFile(void * temp){
    struct threadNode* node = (struct threadNode*)temp;
   // printf("Did it enter this?\n");
    send(node->socket, node->file_name, strlen(node->file_name), 0);

    return NULL;
}

void readConfig(char** address, int* port_no){
    // Open the .configure file, find the size of it and copy it over to a string
   int rz = open(".configure", O_RDONLY, 0600);
   struct stat v;
   stat(".configure", &v);
   int file_size = v.st_size;
   char * configFile = (char*)malloc(file_size*sizeof(char));
   int qz = read(rz, configFile, file_size);


   // read configFile for the address/port
    int i = 0;
    char * addr_size = (char*)malloc(2 * sizeof(char));
    char * port_size = (char*)malloc(2 * sizeof(char));

    // Search for the first semicolon, which will give us the number of bytes for the IP address
   while(configFile[i] != ':'){
       addr_size[i] = configFile[i];
       i++;
   }
    i++;
    
    // Close off the string and convert it to an integer
    addr_size[2] = '\0';
//    printf("This is the addr_size: %s\n", addr_size);
    int x = atoi(addr_size);
  //  printf("This is the addr_size as an integer: %d\n", x);
    
    // Copy the IP from the .config file to use for other protocols
    (*address) = (char*)malloc((x)*sizeof(char));
    int t;
    for(t = 0; t < x; t++){
        (*address)[t] = configFile[i+t];
    }
    (*address)[t] = '\0'; // DELETE THIS MAYBE?
   // printf("This should be the IP: %s\n", *address);

    int b = 0;
    i = i + t; // Our new offset for finding the port
   // printf("This is the char: %c\n", configFile[i]);
    while(configFile[i] != ':'){
        port_size[b] = configFile[i];
        b++;
        i++;
    }
    i++;

    port_size[2] = '\0';
   // printf("This is the port size: %s\n", port_size);
    int y = atoi(port_size);
   // printf("This is the port size as an integer: %d\n", y);
    
    char * portString = (char*)malloc(y*sizeof(char));
    int k;
    for(k = 0; k < y; k++){
        portString[k] = configFile[i+k];
    }
    //printf("This should be the port: %s\n", portString);
    int portNum = atoi(portString);
   // printf("This should be the port as an int: %d", portNum);

    (*port_no) = portNum;

}



void IPConfig(char ** address, char** port_no){
    // Create a .configure file
    int rz = open(".configure", O_RDWR | O_CREAT | O_TRUNC, 0600);
    
    // Addr_Length holds the number of bytes of the address as a char*
    int addr_length_int = strlen(*address);
    int length = snprintf(NULL, 0, "%d", addr_length_int);
    //printf("This is length of address?: '%d'\n", length);
    char * addr_length = malloc(length + 1);
    snprintf(addr_length, length+1, "%d", addr_length_int);
    //printf("This is the address?: '%s'\n", addr_length);
    
    // Write "number of bytes:ip address"
    int asz = write(rz, addr_length, strlen(addr_length));
    int cz = write(rz, ":", sizeof(char));
    int az = write(rz, (*address), strlen((*address)));

   // int nz = write(rz, "\n", sizeof(char));

    // Port_Length holds the number of bytes of the port as a char*
    int port_length_int = strlen(*port_no);
    int lengthp = snprintf(NULL, 0, "%d", port_length_int);
    char * port_length = malloc(lengthp + 1);
    snprintf(port_length, lengthp+1, "%d", port_length_int);

    // Write "number of bytes:port"
    int lpz = write(rz, port_length, strlen(port_length));
    int ccz = write(rz, ":", sizeof(char));
    int pz = write(rz, (*port_no), strlen((*port_no)));

    //printf("IPConfig Finished\n");
    close(rz);
}


void add(char ** project_name, char** file_name){

    
    if(chdir((*project_name)) == -1){
        printf("Add Failed: Project does not exist in the repository\n");
        exit(1);
    }







    

    chdir(".."); // back to working directory
}

void segfault_sigaction(int signal, siginfo_t *si, void *arg){
	printf("Unfortunately something went wrong. Exiting\n");
	exit(0);
}



int main(int argc, char* argv[]){
	// Catch seg faults
	struct sigaction sa;
	memset(&sa, 0,sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_sigaction = segfault_sigaction;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGSEGV, &sa, NULL);	


    char * addr1 = "172.17.11.207";
    int port_no = 10001;



    
    if(strcmp(argv[1], "configure") == 0){ // configure command
        // create config file
        char * port = (char*)malloc(sizeof(argv[3]) * sizeof(char));
        strcpy(port, argv[3]);
        
        char * addr = (char*)malloc(sizeof(argv[2]) * sizeof(char));
        strcpy(addr, argv[2]);
        
        
        //printf("Here is the port: %s and here is the addr: %s\n", port, addr);
        
        IPConfig(&addr, &port);
        exit(1);
    } else{
        // read from config file
        
        readConfig(&addr1, &port_no);

    }







//char* file_name = "proj1";

//createProj(&file_name);


    struct sockaddr_in address;
    int sock = 0; 
    int valread;
    struct sockaddr_in serv_addr;


    char *hello = "temp.txt";
    char buff[50] = {0};


    if( (sock = socket(AF_INET, SOCK_STREAM, 0))< 0 ){
        printf("\n Socket Creation Error\n");
        return -1;

    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    serv_addr.sin_port = htons(port_no);


    if(inet_pton(AF_INET, addr1, &serv_addr.sin_addr)<=0){
        printf("\n Invalid Address \n");
        return -1;
    }


    if(connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0){
        printf("\n Connection Failed\n");
        return -1;
    }






    
    // create
	if(strcmp(argv[1], "create") == 0){
    	// send "create" to server, have the server make the manifest, and send the file back to the client, create client-side    
        // sending over the string with data size followed by data itself
        	char * proj_name = (char*)malloc((strlen(argv[2])+1) * sizeof(char));
        	strcpy(proj_name, argv[2]);

        	//length of the project name
        	int proj_name_size = strlen(argv[2]); // length of project name +1?????????????

        	//turning the project size (int) to a string
        	char * nameSize = (char*)malloc(10*sizeof(char));
        	snprintf(nameSize, 10, "%d", proj_name_size);


        	char * buff = (char*)malloc((proj_name_size + 10) * sizeof(char));
        

        	// concatenating all the elements into one string
        	strcat(buff, nameSize);
        	strcat(buff, ":");
        	strcat(buff, proj_name);

        
        	// send the command to the server
        	int cmd = send(sock, "cre", 3, 0);
        	int proj = send(sock, buff, strlen(buff)+1, 0); 

            
        	char * manifest = (char*)malloc(sizeof(char));

        	int fd = read(sock, manifest, 1);

        	//printf("This is Manifest on client side:%s\n", manifest);
     		struct stat temp2 = {0};

    		// Create the project folder on the client side
    		if(stat(proj_name, &temp2) == -1){
        		mkdir(proj_name, 0700);
   		}else{
        		printf("Project already exists on the client.\n");
        		exit(1);
    		}


    		chdir(proj_name);

    		int fz = open(".Manifest", O_RDWR | O_CREAT | O_TRUNC, 0600);
    		int kz = write(fz, manifest, strlen(manifest));

    		chdir("..");
        	//int fd = read(sock, )
 	
            printf("Create was successful\n");
    
            // add
    } else if(strcmp(argv[1], "add") == 0){
		// the add command will add an entry for the file into the .Manifest of the CLIENT
        	//printf("made it here\n");
		char * proj_name = (char*)malloc((strlen(argv[2])+1) * sizeof(char));
        	strcpy(proj_name, argv[2]);

        	//length of the project name
        	int proj_name_size = strlen(argv[2]); // length of project name +1?????????????

		if(chdir(proj_name) == -1){
			printf("Project doesn't exist on the client.\n");
			exit(1);
		} else { // Director Exists
			//printf("This is argv[3]: '%s'\n", argv[3]);
			// Check if the Manifest file exists
			if(access(".Manifest", F_OK) == -1){
				// If we're here, the Manifest doesn't exist
				printf("Error: This project directory doesn't include a .Manifest file. Please add the project to the repo with ./WTF create <project_name>\n");
				exit(1);
			}

			//printf("Before the LL\n");			

			// Create the Linked List for the .Manifest file
			int manifestVersion = 0;
			struct Node* start = createLL(&manifestVersion);			
			//++manifestVersion; // Increment the verion of the manifestVersion			
			//printf("Made the Linked List\n");			

			// Now that we have to remove the .Manifest file since we already saved all it's data to the LL
			int rem = remove(".Manifest");
			//printf("We removed the manifest\n");
			int mh = open(".Manifest", O_RDWR | O_CREAT | O_APPEND, 0700); // Open the .Manifest to append
			if(mh == -1){
				printf("Manifest doesn't exist on the client.\n");
				exit(1);
			}
			
			//printf("We opened the .Manifest\n");

			int fh = open(argv[3], O_RDONLY); // Read the file they're trying to add
			if(fh == -1){
				printf("File doesn't exist on the client.\n");
				exit(1);
			}

			// Copy over the file to a string data
			struct stat v;
			stat(argv[3], &v);
			int size = v.st_size;
			char *data = malloc(size*sizeof(char));
			int ret = read(fh, data, size);

			//printf("This is the file content: '%s'\n", data); 	

			// Create the hash for the file, it's stored in temp
			unsigned char *d = SHA256(data, strlen(data), 0);
			char *temp = malloc(SHA256_DIGEST_LENGTH*sizeof(char));
			int i;

			sprintf(temp, "%02x", d[0]);
			for(i = 1; i < SHA256_DIGEST_LENGTH; i++){
				sprintf(temp+strlen(temp), "%02x", d[i]);
			}
			putchar('\n');

			//printf("This is the hash 02x: '%02x'\n", d);
			char* insertHash = malloc(strlen(temp) * sizeof(char));
			strcpy(insertHash, temp);
			//printf("This is the hash?: '%s' and this is insertHash: '%s'\n", temp, insertHash);


			// Write the manifest version number to the manifest
			int manLength = snprintf(NULL, 0, "%d", manifestVersion);
			char* newManVer = malloc(manLength + 1);
			snprintf(newManVer, manLength + 1, "%d", manifestVersion);
			int vn = write(mh, newManVer, manLength*sizeof(char));
			int nl = write(mh, "\n", sizeof(char));

			 
			// Write the new file to the manifest
			int vp = write(mh, "1\t", 2*sizeof(char));
			int pn = write(mh, argv[3], strlen(argv[3]));
           		int sz = write(mh, "\t", sizeof(char)); 
			//printf("This is temp: '%s'\n", temp);
			int hz = write(mh, temp, strlen(temp));
			nl = write(mh, "\n", sizeof(char));	

			// Check through all indexes of the LL (Old Manifest). If the file exists already, we don't add it back
			while(start != NULL){
				if(strcmp(start->projName, argv[3]) == 0){ // Skip the node if we found it in our LL
					start = start->next;
					continue;
				}
				
				// Convert the version number to a string and write it (and add a space)
				int len = snprintf(NULL, 0, "%d", start->versionNum);
				char* lenChar = malloc(len + 1);
				snprintf(lenChar, len + 1, "%d", start->versionNum);	
				int vn = write(mh, lenChar, sizeof(char));
				int sn = write(mh, "\t", sizeof(char));

				// Write the file name and a space
				int fn = write(mh, start->projName, strlen(start->projName));
				int s = write(mh, "\t", sizeof(char));
	
				// Write the hash and a new line
				int hn = write(mh, start->hash, strlen(start->hash));
				int g = write(mh, "\n", sizeof(char));				

				// Move to the next node
				//printf("this is insertHash in the loop: '%s'\n", insertHash);
				start = start->next;
			}		 		
			free(start);

			printf("Successfully added %s to the .Manifest file\n", argv[3]);
	
		}



        // remove
	} else if(strcmp(argv[1], "remove") == 0){
		// the remove command will remove an entry for the file into the .Manifest of the CLIENT
        	//printf("Remove\n");
		char * proj_name = (char*)malloc((strlen(argv[2])+1) * sizeof(char));
        	strcpy(proj_name, argv[2]);

        	// length of the project name
        	int proj_name_size = strlen(argv[2]); // length of project name +1?????????????

		if(chdir(proj_name) == -1){
			printf("Project doesn't exist on the client.\n");
			exit(1);
		} else { // Directory Exists
			//printf("This is argv[3]: '%s'\n", argv[3]);
			// Check if the Manifest file exists
			if(access(".Manifest", F_OK) == -1){
				// If we're here, the Manifest doesn't exist
				printf("Error: This project directory doesn't include a .Manifest file. Please add the project to the repo with ./WTF create <project_name>\n");
				exit(1);
			}
			
			// Create the Linked List for the .Manifest file
			int manifestVersion = 0;
			struct Node* start = createLL(&manifestVersion);			
			++manifestVersion; // Increment the verion of the manifestVersion			

			// Now that we have to remove the .Manifest file since we already saved all it's data to the LL
			int rem = remove(".Manifest");

			int mh = open(".Manifest", O_RDWR | O_CREAT | O_APPEND, 0700); // Open the .Manifest to append
			if(mh == -1){
				printf("Manifest doesn't exist on the client.\n");
				exit(1);
			}
				
			// Write the manifest version number to the manifest
			int manLength = snprintf(NULL, 0, "%d", manifestVersion);
			char* newManVer = malloc(manLength + 1);
			snprintf(newManVer, manLength + 1, "%d", manifestVersion);
			int vn = write(mh, newManVer, manLength*sizeof(char));
			int nl = write(mh, "\n", sizeof(char));

			// Check through all indexes of the LL (Old Manifest). If the file exists already, we don't add it back (hence, remove)
			while(start != NULL){
				if(strcmp(start->projName, argv[3]) == 0){ // Skip the node if we found it in our LL
					start = start->next;
					continue;
				}
				
				// Convert the version number to a string and write it (and add a space)
				int len = snprintf(NULL, 0, "%d", start->versionNum);
				char* lenChar = malloc(len + 1);
				snprintf(lenChar, len + 1, "%d", start->versionNum);	
				int vn = write(mh, lenChar, sizeof(char));
				int sn = write(mh, "\t", sizeof(char));

				// Write the file name and a space
				int fn = write(mh, start->projName, strlen(start->projName));
				int s = write(mh, "\t", sizeof(char));
	
				// Write the hash and a new line
				int hn = write(mh, start->hash, strlen(start->hash));
				int g = write(mh, "\n", sizeof(char));				

				// Move to the next node
				//printf("this is insertHash in the loop: '%s'\n", insertHash);
				start = start->next;
			}		 		
			free(start);

			printf("Successfully removed %s from the .Manifest file\n", argv[3]);

		}
	
        // currentversion
    } else if(strcmp(argv[1], "currentversion")== 0){
       
        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "cur", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);


        // read in the size of the buff
        char * temp5 = (char*)malloc(5 * sizeof(char));

        char *x = (char*)malloc(1*sizeof(char));

        int tz;

        // read in until the delimeter is found (:)
        while(1){
            tz = read(sock, x, 1);
        
            if(strcmp(x, ":") == 0){
                break;
            }
        
            strcat(temp5, x);
        }

        // turn the file_size string into an integer
        int buffSize = atoi(temp5);


        char * serverBuff = (char*)malloc(buffSize*sizeof(char));

        tz = read(sock, serverBuff, buffSize);


//        printf("\nThis is serverBuff -> %s\n", serverBuff);
     
        int i = 0;

        char * temp6 = malloc(5*sizeof(char));


        int k = 0;
        while(serverBuff[i] != ':'){
            temp6[k] = serverBuff[i];
            k++;


            i++;
        }
        i++;

        int numFiles = atoi(temp6);



        int j = 0;

        
        while(j < numFiles){
            
            int versionNum = 0;
            char* temp7 = malloc(5*sizeof(char));

                
            k = 0;
            // read versionNum
            while(serverBuff[i]!= ':'){
                temp7[k] = serverBuff[i];
                k++;
                
                i++;
            }
            i++;
            
            versionNum = atoi(temp7);

            printf("%d ", versionNum);

            char* temp8 = malloc(5*sizeof(char));

            // read fileSize
            k = 0;
            while(serverBuff[i] != ':'){
                temp8[k] = serverBuff[i];
                k++;

                i++;
            }

            int fileNameSize = atoi(temp8);

            i++;

            
            // proj_name
            char * projName = malloc(fileNameSize * sizeof(char));
            
            k = 0;
            while(serverBuff[i] != ':'){
                projName[k] = serverBuff[i];
                k++;
                i++;
            }
            i++;
            
            printf("%s\n", projName);
            //printf("This is the project name: %s and this is it's version number: %d\n", projName, versionNum);
        



            j++;
        }

        
        
    } else if(strcmp(argv[1], "update") == 0){
		// the project name
        	char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        	strcpy(proj_name, argv[2]);

        	// get length
        	int proj_name_size = strlen(argv[2]);

        	// turning the project size(int) to a string
        	char * nameSize = (char*)malloc(5 * sizeof(char));
        	snprintf(nameSize, 5, "%d", proj_name_size);
	
        	char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));

        	// turns string into size:name
        	strcat(buff, nameSize);
        	strcat(buff, ":");
        	strcat(buff, proj_name);
       
	        // send size:name to server
	        int cmd = send(sock, "upd", 3, 0);
	        int proj = send(sock, buff, strlen(buff)+1, 0);
		
		
        	// read in the size of the buff
        	char * temp5 = (char*)malloc(6 * sizeof(char));
        	char *x = (char*)malloc(1*sizeof(char));
        	int tz;

        	// read in until the delimeter is found (:)
        	while(1){
        		tz = read(sock, x, 1);
            		if(strcmp(x, ":") == 0){
                		break;
            		}
            		strcat(temp5, x);
        	}

        	// buffSize contains how long the serverRepo's version of the project's Manifest in bytes
        	int buffSize = atoi(temp5);
		//printf("This is the buffSize: '%d'\n", buffSize);

		// serverBuff contains the serverRepo's version of the project's Manifest
        	char * serverBuff = (char*)malloc(buffSize*sizeof(char));
        	tz = read(sock, serverBuff, buffSize);

		//printf("This is in serverBuff: '%s'\n", serverBuff);
		
		// We're gonna create a temporary manifest in the main directory so we can make a LL for it. This .Manifest will be deleted
		int mh = open(".Manifest", O_RDWR | O_CREAT | O_APPEND, 0700); // Open the .Manifest to append
		if(mh == -1){
			printf("Manifest doesn't exist on the client.\n");
			exit(1);
		}
				
		// Write the serverRepo's .Manifest to the main directory
		int srm = write(mh, serverBuff, strlen(serverBuff));

			
		// Create the Linked List for the .Manifest file of the serverRepo
		int serverManVer = 0;
		struct Node* serverLL = createLL(&serverManVer);			
		printList(serverLL);
		// Now we remove the .Manifest file since we already saved all it's data to the LL
		int rem = remove(".Manifest");

		// Now that we have the server's .Manifest, we want to look at the local one
		if(chdir(argv[2]) == -1){
			printf("Project doesn't exist on the client.\n");
			exit(1);
		} else {
			int localManVer = 0;
			struct Node* localLL = createLL(&localManVer);

			// We now have our server's .Manifest stored in a LL and the local .Manifest stored in a LL
			// We want to know if one is greater than the other so we get every node
			struct Node* localDupe = localLL;
			struct Node* serverDupe = serverLL;
			int localCount = 0;
			int serverCount = 0;		

			while(localDupe != NULL){
				++localCount;
				localDupe = localDupe->next;
			}
			while(serverDupe != NULL){
				++serverCount;
				serverDupe = serverDupe->next;
			}
			
			// Prepare the .Update file
			int uz = open(".Update", O_RDWR | O_CREAT | O_TRUNC, 0700);

			// This will keep track of whether or not we found a conflict. 0 -> No conflict, 1 -> Conflict
			int foundConflict = 0;

			// Used for Delete
			struct Node* serv = serverLL;
			struct Node* loc = localLL;

			if(serverManVer != localManVer){ // Server's .Manifest is bigger than the local .Manifest, so our outer loop uses that one
				int foundNode = 0;
				while(serverLL != NULL){
					struct Node* local = localLL;
					while(local != NULL){
						if(strcmp(serverLL->projName, local->projName) == 0){
							foundNode = 1;								
							// According to the documentation: MODIFY for a file both .Manifest have, BUT the versions are different 
							// and the hash is the same
							if(serverLL->versionNum != local->versionNum){
								if(strcmp(serverLL->hash, local->hash) == 0){
									// We are now in the MODIFY case and we want to write the SERVER'S ??????????
									// Write "M " symbolizing Modify
									int az = write(uz, "M\t", 2*sizeof(char));
							
									// Convert the version number to a string and write it (and add a space)
									int len = snprintf(NULL, 0, "%d", serverLL->versionNum);
									char* lenChar = malloc(len + 1);
									snprintf(lenChar, len + 1, "%d", serverLL->versionNum);	
									int vn = write(uz, lenChar, sizeof(char));
									int sn = write(uz, "\t", sizeof(char));
											
									// Write the project name and a space
									int pz = write(uz, serverLL->projName, strlen(serverLL->projName));
									sn = write(uz, "\t", sizeof(char));						

									// Write the hash and a new line
									int hz = write(uz, serverLL->hash, strlen(serverLL->hash)); 
									int nl = write(uz, "\n", sizeof(char));	

									printf("M: '%s'\n", serverLL->projName); 
								} else {
									// If the hashes are different here, we have a conflict case. Conflicts are... bad
									// If we find a conflict, we're supposed to print them out so the user can resolve them
									//  and then remove the .Update file
									printf("Conflict found with '%s'. Please resolve before updating again.\n", serverLL->projName);
									foundConflict = 1;
								}		
                            				}else if(serverLL->versionNum == local->versionNum){
                                				if(strcmp(serverLL->hash, local->hash) == 0){
                                    					// do nothing
                                    					//printf("Manifests are different but file version and hash are the same for the file: %s\n. This is case 4.\n", serverLL->projName);
                                				}else{
                                    					// We are now in the MODIFY case and we want to write the SERVER'S ??????????
									// Write "M " symbolizing Modify
									int az = write(uz, "M\t", 2*sizeof(char));
							
									// Convert the version number to a string and write it (and add a space)
									int len = snprintf(NULL, 0, "%d", serverLL->versionNum);
									char* lenChar = malloc(len + 1);
									snprintf(lenChar, len + 1, "%d", serverLL->versionNum);	
									int vn = write(uz, lenChar, sizeof(char));
									int sn = write(uz, "\t", sizeof(char));
						
									// Write the project name and a space
									int pz = write(uz, serverLL->projName, strlen(serverLL->projName));
									sn = write(uz, "\t", sizeof(char));						

									// Write the hash and a new line
									int hz = write(uz, serverLL->hash, strlen(serverLL->hash)); 
									int nl = write(uz, "\n", sizeof(char));	

									printf("M: '%s'\n", serverLL->projName); 
                                				}

                            				}	
						}
						
						local = local->next;
					}	
					if(foundNode == 0){
						// Accordinng to the documentation: ADD for a file that is in the server's .Manifest but NOT in the client's
						// AND the server's .Manifest is a different version from the client's
						if(localManVer != serverManVer){
							// Write "A " symbolizing Add
							int az = write(uz, "A\t", 2*sizeof(char));
							
							// Convert the version number to a string and write it (and add a space)
							int len = snprintf(NULL, 0, "%d", serverLL->versionNum);
							char* lenChar = malloc(len + 1);
							snprintf(lenChar, len + 1, "%d", serverLL->versionNum);	
							int vn = write(uz, lenChar, sizeof(char));
							int sn = write(uz, "\t", sizeof(char));
						
							// Write the project name and a space
							int pz = write(uz, serverLL->projName, strlen(serverLL->projName));
							sn = write(uz, "\t", sizeof(char));						

							// Write the hash and a new line
							int hz = write(uz, serverLL->hash, strlen(serverLL->hash)); 
							int nl = write(uz, "\n", sizeof(char));	

							printf("A: '%s'\n", serverLL->projName); 
						}
					} else {
						// We found the node and handles the case in the previous while loop. Reset foundNode to 0
						foundNode = 0;
					}
					
					serverLL = serverLL->next;
				}
				
				// For our (D)elete case, we need to check if a project is NOT in the server but is in the client. We can skip this if the server's .Manifest is a different
				// version than the clients
				//printf("ABout to do loc\n");
				while(loc != NULL){
					int foundClient = 0;
					struct Node* serv2 = serv;
					while(serv2 != NULL){
						//printf("Here is the servProj: '%s' being compared to localProj: '%s'\n", serv2->projName, serv->projName); 
						if(strcmp(loc->projName, serv2->projName) == 0){
							foundClient = 1;
						}
						serv2 = serv2->next;
					}
				
					
					if(foundClient == 0){
						// According to the documentation: DELETED for a file that is in the client's .Manifest but NOT in the server's
						// AND the server's .Manifest is a different version from the client's
						// Write "D " symbolizing Add
							int az = write(uz, "D\t", 2*sizeof(char));
							
							// Convert the version number to a string and write it (and add a space)
							int len = snprintf(NULL, 0, "%d", loc->versionNum);
							char* lenChar = malloc(len + 1);
							snprintf(lenChar, len + 1, "%d", loc->versionNum);	
							int vn = write(uz, lenChar, sizeof(char));
							int sn = write(uz, "\t", sizeof(char));
						
							// Write the project name and a space
							int pz = write(uz, loc->projName, strlen(loc->projName));
							sn = write(uz, "\t", sizeof(char));						

							// Write the hash and a new line
							int hz = write(uz, loc->hash, strlen(loc->hash)); 
							int nl = write(uz, "\n", sizeof(char));	

							printf("D: '%s'\n", loc->projName); 
					} else { 
						foundClient = 0;
					}
					
					loc = loc->next;
				}

			} else if(serverManVer == localManVer){
				// manifest version numbers are same
                		int foundNode = 0;
				struct Node* serv = serverLL;
				struct Node* loc = localLL;

				while(serverLL != NULL){
					struct Node* local = localLL;
					while(local != NULL){
						if(strcmp(serverLL->projName, local->projName) == 0){
							foundNode = 1;								
							// According to the documentation: Upload for a file both .Manifest have, file versions different
							if(serverLL->versionNum != local->versionNum){
								printf("File Versions are Different: Error and Exiting. This is case 3.\n");
                                				exit(1);
                            				}else if(serverLL->versionNum == local->versionNum){
                                				if(strcmp(serverLL->hash, local->hash) == 0){
                                    					// do nothing
                                    					printf("The File Version and Hash Number is the same. This is case 1.\n");
                                				}else{
                                    					// We are now in the UPLOAD case. We DON'T need to write this to our .Update file. We just need to tell the user
                                    					// to upload their file
									printf("We found a file in the server that wasn't in the local. That file is '%s'. Please upload.\n", serverLL->projName); 
                                				}

                            				}	
						}
						
						local = local->next;
					}	
					if(foundNode == 0){
						// Accordinng to the documentation: ADD for a file that is in the server's .Manifest but NOT in the client's
						// AND the server's .Manifest is a different version from the client's
						if(localManVer != serverManVer){
							// Write "A " symbolizing Add
							int az = write(uz, "A\t", 2*sizeof(char));
							
							// Convert the version number to a string and write it (and add a space)
							int len = snprintf(NULL, 0, "%d", serverLL->versionNum);
							char* lenChar = malloc(len + 1);
							snprintf(lenChar, len + 1, "%d", serverLL->versionNum);	
							int vn = write(uz, lenChar, sizeof(char));
							int sn = write(uz, "\t", sizeof(char));
						
							// Write the project name and a space
							int pz = write(uz, serverLL->projName, strlen(serverLL->projName));
							sn = write(uz, "\t", sizeof(char));						

							// Write the hash and a new line
							int hz = write(uz, serverLL->hash, strlen(serverLL->hash)); 
							int nl = write(uz, "\n", sizeof(char));	

							printf("A: '%s'\n", serverLL->projName); 
						}
					} else {
						// We found the node and handles the case in the previous while loop. Reset foundNode to 0
						foundNode = 0;
					}
					
					serverLL = serverLL->next;
				}	
				
			}

			// If we found a conflict, remove .Update
			if(foundConflict != 0){
				int r = remove(".Update");
			}
        }else{
            printf("Update was a success\n");
        }	
	

    }else if(strcmp(argv[1], "destroy") == 0){

        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "des", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);


        

    }else if(strcmp(argv[1], "checkout") == 0){
        

        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "che", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);


        mkdir(proj_name, 0700);


        chdir(proj_name);


 
    	// read in the size of the manifest
        char * temp5 = (char*)malloc(6 * sizeof(char));
        char *x = (char*)malloc(1*sizeof(char));
        int tz;

        // read in until the delimeter is found (:)
        while(1){
       		tz = read(sock, x, 1);
           	if(strcmp(x, ":") == 0){
               		break;
            	}
            	strcat(temp5, x);
        }

        // buffSize contains how long the serverRepo's version of the project's Manifest in bytes
        int buffSize = atoi(temp5);
	// printf("This is the buffSize: '%d'\n", buffSize);

	// serverBuff contains the serverRepo's version of the project's Manifest
        char * serverBuff = (char*)malloc(buffSize*sizeof(char));
        tz = read(sock, serverBuff, buffSize);


        int fd = open(".Manifest", O_RDWR | O_CREAT, 0700);
        if(fd == -1){
            perror("Error with creating Manifest on client\n");
		    exit(1);
	    }

        // write the manifest on the client side
        int ret = write(fd, serverBuff, buffSize);

        
       
        // read in the size of the whole concatenated string
        char * temp10 = (char*)malloc(5 * sizeof(char));

        char *z = (char*)malloc(1*sizeof(char));

        int yz;

        // read in until the delimeter is found (:)
        while(1){
            yz = read(sock, z, 1);
        
            if(strcmp(z, ":") == 0){
                break;
            }
        
            strcat(temp10, z);
        }

        // turn the file_size string into an integer
        buffSize = atoi(temp10);


        char * serverString = (char*)malloc(buffSize*sizeof(char));

        yz = read(sock, serverString, buffSize);


        //printf("\nThis is serverString -> %s\n", serverBuff);
     
        int i = 0;

        
        // get the number of files
        char * temp6 = malloc(5*sizeof(char));


        
        int k = 0;
        while(serverString[i] != ':'){
            temp6[k] = serverString[i];
            k++;


            i++;
        }
        i++;

        int numFiles = atoi(temp6);



        int j = 0;


        while(j < numFiles){
            

            // file name size
            char* temp7 = malloc(5*sizeof(char));
            k = 0;

            while(serverString[i] != ':'){
                temp7[k] = serverString[i];
                k++;
                i++;
            }

            int fileNameSize = atoi(temp7);

            i++; // skip colon


            // file name
            char * file_name = malloc(fileNameSize * sizeof(char));

            k = 0;

            while(serverString[i] != ':'){
                file_name[k] = serverString[i];
                k++;
                i++;
            }
            
            i++; // skip colon
            
            int pz = open(file_name, O_RDWR | O_CREAT, 0700);


            
            // file content size
            char * temp8 = malloc(5*sizeof(char));
            k = 0;

            while(serverString[i] != ':'){
                temp8[k] = serverString[i];
                k++;
                i++;
            }
            
            int fileContentSize = atoi(temp8);

            i++; // skip colon

            
            // file contents
            char * file_contents = malloc(fileContentSize+1 * sizeof(char));
            k = 0;


            while(serverString[i] != ':'){
                file_contents[k] = serverString[i];
                k++;
                i++;
            }

            //file_contents[fileContentSize] = '\0';


            i++; // skip colon

            int kz = write(pz, file_contents, fileContentSize+1);

            close(pz);
            j++;
        }



    } else if(strcmp(argv[1], "upgrade") == 0){
	// Upgrade will fail if the project doesn't exist on the server, server cannot be contacted, or there is no .Update file on the client side. We perform
	// these checks first
	if(chdir(argv[2]) == -1){
		printf("Project doesn't exist on the client.\n");
		exit(1);
	} else {
		// Now we're in the client side project, check to see if the .Update file exists
		if(access(".Update", F_OK) == -1){
			// If we're here, the Update doesn't exist
			printf("Error: This project directory doesn't include a .Update file. Please update the project to the repo with ./WTF update <project_name>\n");
			exit(1);
		}

		// Now that the check is done, chdir back to the parent directory
		chdir("..");
	}

	if(chdir(".serverRepo") == -1){
		printf("Server Repo doesn't exist.\n");
		exit(1);
	} else {
		// Now we're in the serverRepo, let's try going into the project
		if(chdir(argv[2]) == -1){
			printf("Error: The server doesn't contain the inputted project.\n");
			exit(1);
		} else {
			// Now that we're here, that means our error conditioning is complete
			chdir(".."); // get out of the project
			chdir(".."); // get out of the serverRepo
		}
	}

	// chdir back to the project directory
	if(chdir(argv[2]) == -1){
		// Realistically, we should never get here because we already did the check, but better safe than sorry
		printf("Project doesn't exist on the client\n");
		exit(1);
	} else {
		// Create the LL containing stuff from the .Update file
		struct upgradeNode* start = createUpgradeLL();
		if(start == NULL){
			// The .Update file is empty. We'll remove it and print error
			int r = remove(".Update");
			printf("The project is up to date.\n");
			exit(1);
		}		

		// We want to loop through each index of the upgradeNode and resolve all everything		
		while(start != NULL){
			if(strcmp(start->mode, "D") == 0){
				// According to the doc, "It will delete the entry from the client's .Manifest for all files tagged with 'D'"
				int clientManVer = 0;
				struct Node* clientMan = createLL(&clientManVer);
				
				// Now that all the contents of the Manifest are stored in clientMan, we can remove the .Manifest and rewrite it without the D node
				int r = remove(".Manifest");
				int mh = open(".Manifest", O_RDWR | O_CREAT | O_TRUNC, 0700);					
				
				// I'm assuming deleting something from the manifest will also increase it's version number, so do that too
				++clientManVer;	

				// Write the manifest version number to the manifest
				int manLength = snprintf(NULL, 0, "%d", clientManVer);
				char* newManVer = malloc(manLength + 1);
				snprintf(newManVer, manLength + 1, "%d", clientManVer);
				int vn = write(mh, newManVer, manLength*sizeof(char));
				int nl = write(mh, "\n", sizeof(char));

				// Now we write everything back into the .Manifest
				while(clientMan != NULL){
					if(strcmp(start->projName, clientMan->projName) != 0){
						// We only write to the file if it's not the same as start		
						// Convert the version number to a string and write it (and add a tab)
						int len = snprintf(NULL, 0, "%d", clientMan->versionNum);
						char* lenChar = malloc(len + 1);
						snprintf(lenChar, len + 1, "%d", clientMan->versionNum);	
						int vn = write(mh, lenChar, sizeof(char));
						int sn = write(mh, "\t", sizeof(char));
	
						// Write the file name and a tab
						int fn = write(mh, clientMan->projName, strlen(clientMan->projName));
						int s = write(mh, "\t", sizeof(char));
		
						// Write the hash and a new line
						int hn = write(mh, clientMan->hash, strlen(clientMan->hash));
						int g = write(mh, "\n", sizeof(char));				

					}				
					clientMan = clientMan->next;
				}
	
			} else if(strcmp(start->mode, "M") == 0 || strcmp(start->mode, "A")){
				// Roshan said that in this case and A, we can just bring the server's .Manifest over and copy the file 	
        			printf("This is the M: '%s'\n", start->projName);
				// We want to get the file from the server
				// We send the file name to the server so we receive the file from the server and write it into the client.
				int len = strlen(start->projName);
            			char * proj_name_len = malloc(len * sizeof(char));
				char * buff = malloc(len * sizeof(char));
				int buffSize = len;
				int buffCount = 0;
	
            			snprintf(proj_name_len, len+1, "%d", len);
				//printf("this is len: '%d'\n", len);        
    
            			strcat(buff, proj_name_len);
            			buffCount+=len;

            			if(buffCount+ 1 > buffSize){
                			buff = realloc(buff, 2*buffSize);
                			buffSize = 2 * buffSize;
            			}
            			strcat(buff, ":");
            			buffCount++;

            			while(buffCount+ (strlen(start->projName)+1) > buffSize){
                			buff = realloc(buff, 2*buffSize);
                			buffSize = 2 * buffSize;
            			}

				// Writes the name of the file. BUFF HOLDS THE ACTUAL FILE NAME
            			strcat(buff, start->projName);		
				//printf("This is buff: '%s'\n", buff);
				
				// Now we also need to send the Project name. Tanuj's code below creates that 
        			char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        			strcpy(proj_name, argv[2]);

       		 		// Get length of proj_name
        			int proj_name_size = strlen(argv[2]);

        			// Turning the project size(int) to a string
        			char * nameSize = (char*)malloc(5 * sizeof(char));
        			snprintf(nameSize, 5, "%d", proj_name_size);

        			char* nameOfProj = (char*)malloc((proj_name_size+5)*sizeof(char));

        			// turns string into size:name
        			strcat(nameOfProj, nameSize);
        			strcat(nameOfProj, ":");
        			strcat(nameOfProj, proj_name);
				// nameOfProj HOLDS THE NAME OF THE GIVEN PROJECT

       				// Tell the server we're on Upgrade
        			int cmd = send(sock, "upg", 3, 0);
				// Send the Project name to the server in the form: <proj_name_size><:><proj_name>
        			int proj = send(sock, nameOfProj, strlen(nameOfProj)+1, 0);
				// Send the File name to the server in the form: <file_name_size><:><file_name>
				int pro = send(sock, buff, strlen(buff)+1, 0);

				// Receive the file 
				char * n = (char*)malloc(6 * sizeof(char));
        			char *o = (char*)malloc(1*sizeof(char));
        			int tz;
 
        			// read in until the delimeter is found (:)
        			while(1){
        				tz = read(sock, o, 1);
            				if(strcmp(o, ":") == 0){
                				break;
            				}
            				strcat(n, o);
        			}
				buffSize = atoi(n);
				char * fileContents = (char*)malloc(buffSize*sizeof(char));
        			tz = read(sock, fileContents, buffSize-1);

				//printf("This is the file Contents: '%s'\n", fileContents);

				// Now we want to write the file contents to the file
				int rem = remove(start->projName);
				int fcd = open(start->projName, O_RDWR | O_CREAT, 0700);
				int fcw = write(fcd, fileContents, strlen(fileContents));				

				// This code is to move the server's .Manifest to the client
				char * temp5 = (char*)malloc(6 * sizeof(char));
        			char *x = (char*)malloc(1*sizeof(char));
 
        			// read in until the delimeter is found (:)
        			while(1){
        				tz = read(sock, x, 1);
            				if(strcmp(x, ":") == 0){
                				break;
            				}
            				strcat(temp5, x);
        			}

        			// buffSize contains how long the serverRepo's version of the project's Manifest in bytes
        			buffSize = atoi(temp5);
				// printf("This is the buffSize: '%d'\n", buffSize);
				//printf("Before read\n");
				// serverBuff contains the serverRepo's version of the project's Manifest
        			char * serverBuff = (char*)malloc(buffSize*sizeof(char));
        			tz = read(sock, serverBuff, buffSize);
				//printf("After read\n");
				int r = remove(".Manifest");
        			int fd = open(".Manifest", O_RDWR | O_CREAT, 0700);
       	 			if(fd == -1){
            				perror("Error with creating Manifest on client\n");
		    			exit(1);
	    			}	
				// Write the .Manifest stored in serverBuff to an actual .Manifest file			
				int ret = write(fd, serverBuff, buffSize);
			


			}

			start = start->next;
		}

		// Once we're done with everything, remove the .Update file
		int removeUpdate = remove(".Update");

	}	

    }else if(strcmp(argv[1], "commit") == 0){
        // commit
    
        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "com", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);


        // receive the server's manifest
        
        // read in the size of the manifest
        char * temp5 = (char*)malloc(6 * sizeof(char));
        char *x = (char*)malloc(1*sizeof(char));
        int tz;

        // read in until the delimeter is found (:)
        while(1){
        	tz = read(sock, x, 1);
           		if(strcmp(x, ":") == 0){
               		break;
           		}
           		strcat(temp5, x);
        }
        	// buffSize contains how long the serverRepo's version of the project's Manifest in bytes
       	int buffSize = atoi(temp5);
	//printf("This is the buffSize: '%d'\n", buffSize);
	// serverBuff contains the serverRepo's version of the project's Manifest
       	char * serverBuff = (char*)malloc(buffSize*sizeof(char));
       	tz = read(sock, serverBuff, buffSize);

       

        remove(".Manifest");
        int fd = open(".Manifest", O_RDWR | O_CREAT, 0700);
        if(fd == -1){
            perror("Error with creating Manifest on client\n");
		    exit(1);
	    }

	//printf("\n\nThis is server buff in commit: '%s'\n\n");

        // write the manifest on the client side
        int ret = write(fd, serverBuff, buffSize);

        int serverManVer = 0;
        struct Node* serverLL = createLL(&serverManVer);
        close(fd);


        if(chdir(proj_name) == -1){
            perror("Chdir into project name failed\n");
            exit(1);
        }

        
        int clientManVer =0;
        struct Node* clientLL = createLL(&clientManVer);


        if(serverManVer != clientManVer){
            perror("The client and server manifests are not on the same version, please update the local project first.\n");
            exit(1);
        }

        struct Node* start = clientLL;
        
    

        int pz = open(".Commit", O_RDWR | O_CREAT | O_APPEND, 0700);
        if(pz == -1){
            printf("Creating the .Commit Failed\n");
            exit(1);
        }

        int qz = write(pz, "1\n", 2);

        struct Node* clientRef = clientLL;
        struct Node* serverRef = serverLL;

        while(start != NULL){
            
            //int hz = open(".SecretHash", O_RDWR | O_CREAT, 0700);
            int fh = open(start->projName, O_RDONLY); // Read the file they're trying to add
			if(fh == -1){
				printf("File doesn't exist on the client.\n");
				exit(1);
			}

			// Copy over the file to a string data
			struct stat v;
			stat(start->projName, &v);
			int size = v.st_size;
			char *data = malloc(size*sizeof(char));
			int ret = read(fh, data, size);

            struct Node* cRef = clientRef;
            struct Node* sRef = serverRef;
            char* mark = malloc(3*sizeof(char));

            int foundClient = 0;
            int foundClientVer = 0;
            while(cRef != NULL){
                if(strcmp(cRef->projName, start->projName) == 0){
                    // Found in Client
                //    printf("Found client: '%s'\n", cRef->projName);
                    foundClient = 1;
                    foundClientVer = cRef->versionNum;
                    break;
                }
                //printf("In Clientver, comparing Client: '%s' to Start: '%s'\n", cRef->projName, start->projName);
                cRef = cRef->next;
            }

            int foundServer = 0;
            int foundServerVer = 0;
            while(sRef != NULL){
                if(strcmp(sRef->projName, start->projName) == 0){
                    // Found in Server
                    //printf("Found Server: '%s'\n", sRef->projName);
                    foundServer = 1;
                    foundServerVer = sRef->versionNum;
                    break;
                }
                //printf("In Serverver, comparing Server: '%s' to Start: '%s'\n", sRef->projName, start->projName);
                sRef = sRef->next;
            }

            if(foundServer == 1 && foundClient == 0){
                mark[0] = 'D';
                mark[1] = ':';
                mark[2] = '\0';
            } else if(foundServer == 0 && foundClient == 1){
                mark[0] = 'A';
                mark[1] = ':';
                mark[2] = '\0';
            } else if(foundServer == 1 && foundClient == 1){
                // Make sure the client's version of the file is greater
                if(foundClientVer > foundServerVer){
                    mark[0] = 'U';
                    mark[1] = ':';
                    mark[2] = '\0';
                } else {
                    // Fails otherwise
                    printf("Client must sync with the repository before committing changes.\n");
                    printf("This is the failure: '%s'\n", start->projName);
                    remove(".Commit");
                    exit(1);
                }
            } else {
                // Something went terribly wrong
                printf("Error\n");
                remove(".Commit");
                exit(1);
            }

            // Create the hash for the file, it's stored in temp
	    unsigned char *d = SHA256(data, strlen(data), 0);
	    char *temp = malloc(SHA256_DIGEST_LENGTH*sizeof(char));
	    int i;

	    sprintf(temp, "%02x", d[0]);
	    for(i = 1; i < SHA256_DIGEST_LENGTH; i++){
	    	sprintf(temp+strlen(temp), "%02x", d[i]);
	    }
           // temp[65] = '\0';
	    putchar('\n');
            
            //int hhz = write(hz, temp, strlen(temp));
           // printf("This is the hash: '%s' for Start: '%s'\n", temp, start->projName);
	    //printf("this is strlen(temp): '%d'\n", strlen(temp));
        
          //  printf("This is the hash again: '%s'\n", temp);
            //printf("This is temp and this is strlen(temp): '%s', '%d'\n", temp, strlen(temp));
            if(strcmp(temp, start->hash) == 0){
                // hashes are the same, do nothing
            }else{
                // hashes are different, write out an entry to .Commit with its version num incremented
                int mn = write(pz, mark, strlen(mark));
                mn = write(pz, "\t", sizeof(char));

                // Convert the version number to a string and write it (and add a space)
				int newVerNum = start->versionNum;
                ++newVerNum;
                int len = snprintf(NULL, 0, "%d", newVerNum);
				
                
                

                
            //    printf("This is the hash again in the if statement: '%s'\n", temp);
                
               
                int pjz = write(pz, start->projName, strlen(start->projName));
		mn = write(pz, "\t", sizeof(char));						

		// Write the NEW hash and a new line
		//printf("this is insertHash before write: '%s'\n", insertHash);
                int haz = write(pz, temp, 64); 
                //printf("this is the hash we wrote: '%s'\n", temp);
                mn = write(pz, "\t", sizeof(char));
                
                char* lenChar = malloc((len+1) * sizeof(char));////////////////////////////////////////
                snprintf(lenChar, len + 1, "%d", newVerNum);	
		mn = write(pz, lenChar, sizeof(char));
		mn = write(pz, "\t", sizeof(char));
				

                int nl = write(pz, "\n", sizeof(char));	
            } 



            start = start -> next;    
        }

        close(pz);
        
        
        // send the .Commit to the server (CommitSize:CommitFile)
        
        int mz = open(".Commit", O_RDWR | O_CREAT | O_APPEND, 0700);

	    struct stat v;
	    stat(".Commit", &v);
	    int size = v.st_size;
	    char *data = malloc(size * sizeof(char));
	    ret = read(mz, data, size);

	    //printf("This is the .Commit file located on the client side: '%s'\n", data);

	    int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	    snprintf(lenChar, len+1, "%d", size);

	    char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	    strcat(returnString, lenChar);
	    strcat(returnString, ":");
	    strcat(returnString, data);

	    //printf("This is the .Commit sent to the server: '%s'\n", returnString);
	
	    // Send .Commit back to the server
	    int rd = send(sock, returnString, strlen(returnString), 0); 

        close(mz);
 
    }else if(strcmp(argv[1], "push") == 0){
        
        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "pus", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);

        
        if(chdir(proj_name)== -1){
           perror("Error entering project_name");
           exit(1);
        }



        int fd = open(".Commit", O_RDONLY, 0);
        if(fd == -1){
           perror("Error with opening .Commit\n");
		   exit(1);
	    }

	    struct stat v;
	    stat(".Commit", &v);
	    int size = v.st_size;
	    char *data = malloc(size * sizeof(char));
	    int ret = read(fd, data, size);

	    //printf("This is the .Commit file located in the serverRepo: '%s'\n", data);

	    int len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	    snprintf(lenChar, len+1, "%d", size);

	    char *returnString = (char*)malloc( (size+len+1) * sizeof(char) );
	    strcat(returnString, lenChar);
	    strcat(returnString, ":");
	    strcat(returnString, data);

	    //printf("This is the Commit sent to the server: '%s'\n", returnString);
	
	    // Send back to the server
	    int rd = send(sock, returnString, strlen(returnString), 0); 

        
        // analyze the Commit File to send the appropriate files back to the server
        int j= 0;
        int i = 2;

        close(fd);

        int pz = open(".Commit", O_RDONLY, 0);
        if(pz == -1){
            perror("Error with opening .Commit\n");
            exit(1);
        }

        struct stat t;
	    stat(".Commit", &t);
	    size = t.st_size;
	    char *temp = malloc(size * sizeof(char));
	    ret = read(fd, temp, size);
        
        char * buff1 = (char*)malloc(sizeof(char));
        int buffSize = 1;
        int buffCount = 0;

        //int count = 0;





        while(j < strlen(temp)){
        

            if(temp[i] == 'A' || temp[i] == 'U'){
                // need to send to the server
                
                // this is the Letter (A, U, D)
                
                char * y = malloc(2 * sizeof(char));
                

                y[0] = temp[i];
                y[1] = '\0';

                j++;
                buffCount++;
                strcat(buff1, y);
                
                
                buffSize = 3 * buffSize;
                buff1 = realloc(buff1, buffSize * sizeof(char));


                strcat(buff1, ":");
                buffCount++;

                i++; // the colon
                j++;


                i++;// the tab
                j++;

                // now we're looking for the file_name
                
                i++;
                j++;
                char* x = (char*)malloc(sizeof(char));
                int xCount = 0;
                int xSize = 1;

                while(temp[i] != '\t'){
                    
                    xCount++;

                    while(xCount > xSize || xCount == xSize){
                        xSize = 2 * xSize;
                        x = realloc(x, xSize * sizeof(char));
                    }
                   // strcpy(x, temp[i]);
                
                    char * z = malloc(2 * sizeof(char));
                

                    z[0] = temp[i];
                    z[1] = '\0';

                    strcat(x, z);
                    i++;
                    j++;
                }

//                buffCount+= xCount;

                while(buffCount > buffSize || buffCount == buffSize){
                        buffSize = 2 * buffSize;
                        buff1 = realloc(buff1, buffSize * sizeof(char));
                    }
                
                // add the length of x to the front first then colon then file name


            int len = strlen(x);
            char * x_len = malloc(len);
            snprintf(x_len, len+1, "%d", len);
            

            strcat(buff1, x_len);
            buffCount+=len;
            j+=len;
            
            if(buffCount+ 1 > buffSize){
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }
            strcat(buff1, ":");
            buffCount++;




            while(buffCount+ (strlen(x)+1) > buffSize){
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }


            strcat(buff1, x);
            buffCount+=(strlen(x)+1); 
            j+=(strlen(x)+1); 


            if(buffCount+ 1 > buffSize){
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff1, ":");
            buffCount++;


            // add size of file contents 
        int xp = open(x, O_RDONLY, 0);
        if(xp == -1){
            perror("Error with start->fileName\n");
		    exit(1);
	    }

	    struct stat v;
	    stat(x, &v);
	    int size = v.st_size;
	    char *data1 = malloc(size * sizeof(char));
	    ret = read(xp, data1, size);

        //	printf("This is the start->projName file contents %s\n", data);


    
        len = snprintf(NULL, 0, "%d", size);
        char* lenChar = malloc(len+1);
	    snprintf(lenChar, len+1, "%d", size);

        while(buffCount+ len > buffSize){
            buff1 = realloc(buff1, 2*buffSize);
            buffSize = 2 * buffSize;
        }

        strcat(buff1, lenChar);
        buffCount+=len;
        j+=len;




            // add colon

            if(buffCount+ 1 > buffSize){
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff1, ":");
            buffCount++;




            // add filecontents

             while(buffCount+ (strlen(data1)+1) > buffSize){ // +1???????????????????????????
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }


            strcat(buff1, data1);
    
    
             if(buffCount+ 1 > buffSize){
                buff1 = realloc(buff1, 2*buffSize);
                buffSize = 2 * buffSize;
            }

            strcat(buff1, ":");
            buffCount++;

 










 while(temp[i] != '\n'){
                    i++;
                }
                i++;



/*    
    strcat(buff1, x);

                if(buffCount+1 > buffSize){
                    buffSize = 2 * buffSize;
                    buff1 = realloc(buff1, buffSize*sizeof(char));
                }

                i++; // the tab
                j++;
                
                // add colon
                
                strcat(buff1, ":");
                buffCount++;
*/

                //printf("This should be the mark and the file name: '%s'\n", buff1);
    

            }else if(temp[i] == 'D'){
                // D, go to next line in .Commit
                while(temp[i] != '\n'){
                    i++;
                }
                i++;
            }


            j++;
        }
        

        
        int buffLen = strlen(buff1);
    	char* buffv2 = (char*)malloc((buffLen+10) * sizeof(char));
    	snprintf(buffv2, buffLen+1, "%d", buffLen);
	
    	strcat(buffv2, ":");
    	strcat(buffv2, buff1);
    	strcat(buffv2, ":");


    	// Send back to client
    	
        //printf("buff before sent back to server:%s\n", buffv2);
        
        int bz = send(sock, buffv2, strlen(buffv2), 0);


        // receive the new .Manifest from the server

        char * temp11 = (char*)malloc(6 * sizeof(char));
        char *f = (char*)malloc(1*sizeof(char));
        int tz;

        // read in until the delimeter is found (:)
        while(1){
       		tz = read(sock, f, 1);
           	if(strcmp(f, ":") == 0){
               		break;
            	}
            	strcat(temp11, f);
        }

        // buffSize contains how long the serverRepo's version of the project's Manifest in bytes
        buffSize = atoi(temp11);
	// printf("This is the buffSize: '%d'\n", buffSize);

	// serverBuff contains the serverRepo's version of the project's Manifest
        char * serverBuff = (char*)malloc(buffSize*sizeof(char));
        tz = read(sock, serverBuff, buffSize);


        remove(".Manifest");

        int pf = open(".Manifest", O_RDWR | O_CREAT, 0700);
        if(pf == -1){
            perror("Error with creating Manifest on client\n");
		    exit(1);
	    }

	//printf("This is server Buff: '%s'\n", serverBuff);

        // write the manifest on the client side
        ret = write(pf, serverBuff, strlen(serverBuff));


        remove(".Commit");


        printf("Push successful\n");


    } else if(strcmp(argv[1], "rollback") == 0){
	char* projName = argv[2];
	int proj_name_size = strlen(argv[2]);
	char* versionNum = argv[3];
	//printf("hello");
	// Go to the serverRepo
	if(chdir(".serverRepo") == -1){
		printf("Error with serverRepo\n");
		exit(1);
	} else {
		// Go to the project folder
		if(chdir(projName) == -1){
			printf("Project doesn't exist on the server!\n");
			exit(1);
		} else {
			// Go to the previous versions folder
			if(chdir(".prevVersions") == -1){
				printf("Error finding previous versions\n");
				exit(1);
			} else {
				// Make sure the version we need is there
				if(chdir(versionNum) == -1){
					printf("Version Number invalid\n");
					exit(1);
				} else {
					// Okay, now we know the version is there. So we have to delete everything in the projdirectory and replace it with 
					// the previous version
					char* tgz = malloc((proj_name_size + 4 + 1) * sizeof(char));
					strcat(tgz, projName);
					strcat(tgz, ".tgz");

					//printf("This is tgz: '%s'\n", tgz);
					if(access(tgz, F_OK) == -1){
						printf("No rollback history available for this version\n");
						exit(1);
					} 
					
					chdir(".."); // Bring us to .prevVersions
					chdir(".."); // Bring us to project folder

					system("rm -r *"); // Remove everything
					chdir(".prevVersions");
					chdir(versionNum);
					
					system("pwd");

				    	char * moving2 = malloc( (3 + proj_name_size + 4 + 1 + 3) * sizeof(char));
    	
    					strcat(moving2, "mv ");
    					strcat(moving2, projName);
    					strcat(moving2, ".tgz ");
    					strcat(moving2, "..");
    
    					//printf("This is moving: '%s'\n", moving2);
    					system(moving2); // Move it to .prevVersions
					chdir("..");
					system(moving2); // Move it to project folder
					chdir("..");					
					system(moving2);
					chdir(".."); // Move it to the .serverRepo since this is where we want to unpack the project

					char* unzip = malloc((4 + 6 + proj_name_size + 4 + 1) * sizeof(char));
					strcat(unzip, "tar ");
					strcat(unzip, "-xvzf ");
					strcat(unzip, projName);
					strcat(unzip, ".tgz");
					//printf("This is unzip: '%s'\n", unzip); 
					
					system(unzip);

					char* remo = malloc((3 + proj_name_size + 4 + 1) * sizeof(char));
					strcat(remo, "rm ");
					strcat(remo, projName);
					strcat(remo, ".tgz");

					system(remo);
					//printf("This is remo: '%s'\n", remo);
					printf("Successfully rolled back to version %s\n", versionNum);

				}

			}
				
		}
	}


    } else if(strcmp(argv[1], "history") == 0){	
        // the project name
        char * proj_name = (char*)malloc(strlen(argv[2]+1) * sizeof(char));
        strcpy(proj_name, argv[2]);


        // get length
        int proj_name_size = strlen(argv[2]);

        // turning the project size(int) to a string
        char * nameSize = (char*)malloc(5 * sizeof(char));
        snprintf(nameSize, 5, "%d", proj_name_size);

        char* buff = (char*)malloc((proj_name_size+5)*sizeof(char));


        // turns string into size:name
        strcat(buff, nameSize);
        strcat(buff, ":");
        strcat(buff, proj_name);
        

        // send size:name to server
        int cmd = send(sock, "his", 3, 0);
        int proj = send(sock, buff, strlen(buff)+1, 0);

	
        // receive the new .History from the server

        char * temp11 = (char*)malloc(6 * sizeof(char));
        char *f = (char*)malloc(1*sizeof(char));
        int tz;

        // read in until the delimeter is found (:)
        while(1){
       		tz = read(sock, f, 1);
           	if(strcmp(f, ":") == 0){
               		break;
            	}
            	strcat(temp11, f);
        }

        // buffSize contains how long the serverRepo's version of the project's History in bytes
        int buffSize = atoi(temp11);
	// printf("This is the buffSize: '%d'\n", buffSize);

	// serverBuff contains the serverRepo's version of the project's History
        char * serverBuff = (char*)malloc(buffSize*sizeof(char));
        tz = read(sock, serverBuff, buffSize);

	printf("This is the History of the given project\n%s\n", serverBuff);


    } 






   //}requestFile(&hello, &sock);
    
   // send(sock, hello, strlen(hello), 0);
    //printf("Message sent from client to server successfully\n");
   //valread = read(sock, buff, 50);
    
  // printf("%s\n", buff);
    
    return 0;


}
