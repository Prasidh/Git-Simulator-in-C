#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
struct Node{
	int versionNum;
	char* projName;
	char* hash;

	struct Node *next;
};

struct upgradeNode{
	char* mode;
	int versionNum;
	char* projName;
	char* hash;

	struct upgradeNode *next;
};

struct commitNode{
	char* mode;
	char* projName;
	char* hash;
	int versionNum;

	struct commitNode *next;
};

void printList(struct Node *node){
	int i;
	while(node != NULL){
		printf("Here's the versionNum: '%d', fileName: '%s' and hash: '%s'\n", node->versionNum, node->projName, node->hash);
		node = node->next;
	}
	printf("\n");
}

void printListUpgrade(struct upgradeNode *node){
	int i;
	while(node != NULL){
		printf("Here's the mode: '%s', versionNum: '%d', fileName: '%s' and hash: '%s'\n", node->mode, node->versionNum, node->projName, node->hash);
		node = node->next;
	}
	printf("\n");
}

void printListCommit(struct commitNode *node){
	int i;
	while(node != NULL){
		printf("Here's the mode: '%s', fileName: '%s', hash: '%s' and versionNumL '%d'\n", node->mode, node->projName, node->hash, node->versionNum);
		node = node->next;
	}
	printf("\n");
}
void append(struct Node** head_ref, int *versionNum, char* projName, char* hash){
	// Allocate memory for node
	struct Node* newNode = (struct Node*)malloc(sizeof(struct Node));
	newNode->projName = malloc(strlen(projName) * sizeof(char));
	newNode->hash = malloc(strlen(hash) * sizeof(char));
	
	struct Node* last = *head_ref;
	
	// Put in the data
	newNode->versionNum = *versionNum;
	strcpy(newNode->projName, projName);
	strcpy(newNode->hash, hash);

	// This new node is going to be the last node, so make next of it NULL
	newNode->next = NULL;

	// If LL is empty, make the new node as head
	if(*head_ref == NULL){
		*head_ref = newNode;
		return;
	}

	// Else traverse till the last node
	while(last->next != NULL)
		last = last->next;

	// Change the next of the last node
	last->next = newNode;
	return;

}

void appendUpgrade(struct upgradeNode** head_ref, char* mode, int *versionNum, char* projName, char* hash){
	// Allocate memory for node
	struct upgradeNode* newNode = (struct upgradeNode*)malloc(sizeof(struct upgradeNode));
	newNode->mode = malloc(strlen(mode) * sizeof(char));
	newNode->projName = malloc(strlen(projName) * sizeof(char));
	newNode->hash = malloc(strlen(hash) * sizeof(char));
	
	struct upgradeNode* last = *head_ref;
	
	// Put in the data
	newNode->versionNum = *versionNum;
	strcpy(newNode->mode, mode);
	strcpy(newNode->projName, projName);
	strcpy(newNode->hash, hash);

	// This new node is going to be the last node, so make next of it NULL
	newNode->next = NULL;

	// If LL is empty, make the new node as head
	if(*head_ref == NULL){
		*head_ref = newNode;
		return;
	}

	// Else traverse till the last node
	while(last->next != NULL)
		last = last->next;

	// Change the next of the last node
	last->next = newNode;
	return;

}

void appendCommit(struct commitNode** head_ref, char* mode, char* projName, char* hash, int* versionNum){
	// Allocate memory for node
	struct commitNode* newNode = (struct commitNode*)malloc(sizeof(struct commitNode));
	newNode->mode = malloc(strlen(mode) * sizeof(char));
	newNode->projName = malloc(strlen(projName) * sizeof(char));
	newNode->hash = malloc(strlen(hash) * sizeof(char));
	
	struct commitNode* last = *head_ref;
	
	// Put in the data
	newNode->versionNum = *versionNum;
	strcpy(newNode->mode, mode);
	strcpy(newNode->projName, projName);
	strcpy(newNode->hash, hash);

	// This new node is going to be the last node, so make next of it NULL
	newNode->next = NULL;

	// If LL is empty, make the new node as head
	if(*head_ref == NULL){
		*head_ref = newNode;
		return;
	}

	// Else traverse till the last node
	while(last->next != NULL)
		last = last->next;

	// Change the next of the last node
	last->next = newNode;
	return;

}


struct Node* createLL(int* manVersion){
	struct Node *start = NULL;

	int fd = open(".Manifest", O_RDONLY, 0);
	if(fd == -1){
		printf("Error\n");
		exit(1);
	}

	struct stat v;
	stat(".Manifest", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);
	
	//printf("Here is the size: '%d' and  Manifest: '%s'\n", size, data);

	// This should read the version number of the Manifest + the new Line that follows
	int ManifestVersion = 0;	
	int projVersion = 0;
	char* project = malloc(sizeof(char));
	char* hash = malloc(sizeof(char));
	char delim[] = "\t\n";

	char *ptr = strtok(data, delim);
	ManifestVersion = atoi(ptr);

	int place = 0; // 0 -> We're at a versionNumber, 1 -> We're at a projectName, 2 -> We're at a hash
	while(ptr != NULL){
		ptr = strtok(NULL, delim);
		//printf("This is ptr: '%s' and this is place: '%d'\n", ptr, place);
		if(ptr == NULL){
			break;
		}
		if(place == 0){
			projVersion = atoi(ptr);
			place = 1;
		} else if(place == 1){
			project = malloc( strlen(ptr) * sizeof(char) );
			project = ptr;
			place = 2;
		} else if(place == 2){
			hash = malloc( strlen(ptr) * sizeof(char) );
			hash = ptr;
			
			append(&start, &projVersion, project, hash);
			
			

			// Reset our variables to be used in the next iteration
			project = malloc(sizeof(char));
			hash = malloc(sizeof(char));				
			place = 0;
		}
	}

	//push(&start, k, "Hello", "I'mgay");

	//printf("This is ManifestVersion: '%d'\n", ManifestVersion);
	//printList(start);
	*manVersion = ManifestVersion;
	return start;
}

struct upgradeNode* createUpgradeLL(){
	struct upgradeNode *start = NULL;

	int fd = open(".Update", O_RDONLY, 0);
	if(fd == -1){
		printf("No .Update file exists. Please do an update.\n");
		exit(1);
	}

	struct stat v;
	stat(".Update", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);
	
	//printf("Here is the size: '%d' and  Manifest: '%s'\n", size, data);

	// This should read the version number of the Manifest + the new Line that follows
	char* mode = malloc(sizeof(char));
	int projVersion = 0;
	char* project = malloc(sizeof(char));
	char* hash = malloc(sizeof(char));
	char delim[] = "\t\n";

	char *ptr = strtok(data, delim);
	mode = ptr;	

	int place = 0; // -1 -> We're at the mode, 0 -> We're at a versionNumber, 1 -> We're at a projectName, 2 -> We're at a hash
	while(ptr != NULL){
		ptr = strtok(NULL, delim);
		//printf("This is ptr: '%s' and this is place: '%d'\n", ptr, place);
		if(ptr == NULL){
			break;
		}
		if(place == -1){
			mode = ptr;
			place = 0;
		} else if(place == 0){
			projVersion = atoi(ptr);
			place = 1;
		} else if(place == 1){
			project = malloc( strlen(ptr) * sizeof(char) );
			project = ptr;
			place = 2;
		} else if(place == 2){
			hash = malloc( strlen(ptr) * sizeof(char) );
			hash = ptr;
			
			appendUpgrade(&start, mode, &projVersion, project, hash);
			

			// Reset our variables to be used in the next iteration
			mode = malloc(sizeof(char));
			project = malloc(sizeof(char));
			hash = malloc(sizeof(char));				
			place = -1;
		}
	}

	//push(&start, k, "Hello", "I'mgay");

	//printf("This is ManifestVersion: '%d'\n", ManifestVersion);
	printListUpgrade(start);
	return start;
}

struct commitNode* createCommitLL(){
	struct commitNode *start = NULL;

	int fd = open(".Commit", O_RDONLY, 0);
	if(fd == -1){
		printf("Error\n");
		exit(1);
	}

	struct stat v;
	stat(".Commit", &v);
	int size = v.st_size;
	char *data = malloc(size * sizeof(char));
	int ret = read(fd, data, size);
	
	//printf("Here is the size: '%d' and  Manifest: '%s'\n", size, data);

	// This should read the version number of the Manifest + the new Line that follows
	int ManifestVersion = 0;	
	int projVersion = 0;
	char* mode = malloc(sizeof(char));
	char* project = malloc(sizeof(char));
	char* hash = malloc(sizeof(char));
	char delim[] = ":\t\n";

	char *ptr = strtok(data, delim);
	//mode = ptr;

	int place = -1; // -1 -> We're at a mode, 0 -> We're at a projectName, 1 -> We're at a hash, 2 -> We're at a versionNum
	while(ptr != NULL){
		ptr = strtok(NULL, delim);
		//printf("This is ptr: '%s' and this is place: '%d'\n", ptr, place);
		if(ptr == NULL){
			break;
		}
		if(place == -1){
			mode = ptr;
			printf("This is mode: '%s'\n", mode);
			place = 0;
		} else if(place == 0){
			project = malloc( strlen(ptr) * sizeof(char) );
			project = ptr;
			
			printf("This is projName: '%s'\n", project);
			//projVersion = atoi(ptr);
			place = 1;
		} else if(place == 1){
			hash = malloc( strlen(ptr) * sizeof(char) );
			hash = ptr;

			printf("This is hash: '%s'\n", hash);
			//project = malloc( strlen(ptr) * sizeof(char) );
			//project = ptr;
			place = 2;
		} else if(place == 2){
			projVersion = atoi(ptr);
	
			printf("This is projVersion: '%d'\n", projVersion);			

			appendCommit(&start, mode, project, hash, &projVersion);
			

			// Reset our variables to be used in the next iteration
			mode = malloc(sizeof(char));
			project = malloc(sizeof(char));
			hash = malloc(sizeof(char));				
			place = -1;
		}
	}

	//push(&start, k, "Hello", "I'mgay");

	//printf("This is ManifestVersion: '%d'\n", ManifestVersion);
	printListCommit(start);
	//*manVersion = ManifestVersion;
	return start;
}

