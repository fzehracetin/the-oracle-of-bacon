#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#define SIZE 4000
#define MAX 200
#define R 31

typedef struct node {
	int adr;
	struct node* next;
}node;

typedef struct {
	char *name;
	node* head;
}ht;

typedef struct {
    int *queue;
	int *parent;
    int rear;
    int front;
}QUEUE;

void initializeHash( ht* hashTable, unsigned long long int M) {
	int i;	
	for (i = 0; i < M; i++) {
		hashTable[i].name = (char*) calloc(MAX, sizeof(char));
		hashTable[i].name[0] = '\0';
	}
}

void initiliazeQueue(QUEUE *q, unsigned long long int M) {
    q->rear = 0;
    q->front = 0;
	q->queue = (int*) calloc(M, sizeof(int));
	q->parent = (int*) calloc(M, sizeof(int));
}

void enqueue(QUEUE *q, int x) {
    q->queue[q->rear++] = x;
}

int dequeue(QUEUE *q) {
    return q->queue[q->front++];
}

node* createNode(int adr) {
	node* myNode = (node*) calloc(1, sizeof(node));
	if (myNode == NULL) {
		printf("Node could not allocated \n");
	}
	myNode->adr = adr;
	myNode->next = NULL;
	return myNode;
}

int double_hashing (unsigned long long int key, int i, unsigned long long int M) {
	int h1 = key % M;
	int h2 = 1 + (key % (M - 1));
	return (h1 + i * h2) % M;
}

unsigned long long int find_key(char *str) {
	int i = 0;
	unsigned long long int key = 0;
	int N = strlen(str);
	
	while (i < N) {
		key += str[i] * pow(R, N - i - 1);
		i++;
	}
	return key;
}

int insert_to_table (unsigned long long int key, char* token, ht* hashTable, unsigned long long int M) {
	int adr, i = 0;
	int same = 0;
	adr = double_hashing(key, i, M);
	
	while( (!same) && (hashTable[adr].name[0] != '\0')  && (i < M) ) {
		if (strcmp(token, hashTable[adr].name) == 0)
			same = 1;
		else {
			i++;
			adr = double_hashing(key, i, M);
		}
	}
	if (same) {
		return adr;
	}
	else if (hashTable[adr].name[0] == '\0') {
		strcpy(hashTable[adr].name, token);
		hashTable[adr].head = NULL;
		//printf("%s inserted to %d.. \n", hashTable[adr].name, adr);
		return adr;
	} 
	else if (hashTable[adr].name[0] != '\0') {
		printf("Table is full! \n");
		return -1;
	}
	return -1;
}

void insert_node (ht* hashTable, int adr1, int adr2) {
	node* newNode = createNode(adr2);
	node* current;
	if (hashTable[adr1].head == NULL) {
		hashTable[adr1].head = newNode;
	}
	else {
		current = hashTable[adr1].head;
		while (current->next) {
			current = current->next;
		}
		current->next = newNode;
	}
}

void tokenizer(char* line, ht* hashTable, unsigned long long int M) {
	char *token, buffer1[200], buffer2[200];
	int i, j, artist_adr = 0, movie_adr = 0;
	unsigned long long int key = 0;
	
	for (token = strtok(line, "/"); token != NULL; token = strtok(NULL, "/")) {
		if (strcmp(token, "S\n") == 0) {
			continue;
		}
		i = 0;
		while((i < strlen(token) && ((token[i] < '0') || (token[i] > '9')))) {
			i++;
		} 
		if (i < strlen(token)) {
			key = find_key(token);
			//printf("%s key: %llu \n", token, key); //film
			movie_adr = insert_to_table(key, token, hashTable, M);
		}
		else {
			i = 0;
			while (token[i] != ',') {
				buffer1[i] = token[i];
				i++;
			} buffer1[i] = '\0';
			
			i+= 2;
			j = 0;
			while ( (i < strlen(token)) && (token[i] != '\n')) {
				buffer2[j] = token[i];
				i++;
				j++;
			} buffer2[j] = '\0';
			strcat(buffer2, " ");
			strcat(buffer2, buffer1);
			key = find_key(buffer2);
			//printf("%s key: %llu \n", buffer2, key); //oyuncu
			artist_adr = insert_to_table(key, buffer2, hashTable, M);
			if (artist_adr != -1) {
				insert_node(hashTable, movie_adr, artist_adr);
				insert_node(hashTable, artist_adr, movie_adr);
			}
		}
	}
}

void file_reader(ht* hashTable, unsigned long long int M) 
{
	FILE *fp;
	char line[SIZE];
	
	fp = fopen("D://input-mpaa.txt", "r");
	if (fp == NULL) {
		printf("File could not opened !!\n");
		return;
	}
	while ( fgets(line, SIZE - 1, fp) ) {
		tokenizer(line, hashTable, M);
	}
}

int search (char* artist, ht* hashTable, unsigned long long int M) {
	int i = 0;
	
	while ((i < M) && (strcmp(hashTable[i].name, artist) != 0)) {
		i++;
	}
	if (i < M)
		return i;
	else {
		printf("%s isimli bir artist bulunamadi!!", artist);
		return -1;
	}
}

void bfs (QUEUE *q, int start, int wanted, ht* hashTable, int *visited, unsigned long long int M) {
    int i = 0, flag = 0, j = 0, count = 0, v = start;
	node* current;
    int *yol = (int*) calloc(M, sizeof(int));
	
	q->parent[i] = -1;
    visited[v] = 1;
	enqueue(q, v);
    while ((q->front < q->rear) && (!flag)) {
        v = dequeue(q);
        if (hashTable[v].head) {
			current = hashTable[v].head;
			while (current) {
				if (visited[current->adr] != 1) {
					q->parent[current->adr] = v;
					visited[current->adr] = 1;
					enqueue(q, current->adr);
					if (current->adr == wanted)
						flag = 1;
				}
				current = current->next;
			}
        }
    }
	if (flag == 1) {
		printf("Yol var. \n");
		i = wanted;
		j = 0;
		while (i != start) {
			yol[j++] = i;
			i = q->parent[i];
		}
		yol[j] = start;
		printf("\n");
		while (j > 0) {
			printf("%s - ", hashTable[yol[j]].name);
			j = j - 2;
			printf("%s '", hashTable[yol[j++]].name);
			printf("%s'\n", hashTable[yol[j--]].name);
			count++;
		} printf("Kevin Bacon Sayisi: %d \n", count);
	}
}

void kevin_bacon (char* artist1, char* artist2, ht* hashTable, QUEUE *q, int* visited, unsigned long long int M) {
	int adr1, adr2;
	
	adr1 = search(artist1, hashTable, M);
	adr2 = search(artist2, hashTable, M);
	if ((adr1 == -1) || (adr2 == -1))
		return;
	printf("%s %d nolu adreste.\n", artist1, adr1);
	printf("%s %d nolu adreste.\n", artist2, adr2);
	bfs(q, adr1, adr2, hashTable, visited, M);
}

void print_hash (ht* hashTable, long long int M) {
	int i;
	node* current;
	int count = 0;
	for(i = 0; i < M; i++) {
		if (hashTable[i].name[0] != '\0') {
			count++;
			printf("********%s-> ", hashTable[i].name);
			current = hashTable[i].head;
			if (hashTable[i].head != NULL) {
				printf("%s", hashTable[current->adr].name);
				current = current->next;
				while (current) {
					printf("-> ");
					printf("%s", hashTable[current->adr].name);
					current = current->next;
				} printf("\n");
			}
		}
	}
	printf("%d elements inserted to hash table. \n", count);
}

int main(int argc, char **argv)
{
	unsigned long long int M = 323567;
	char artist1[100] = "Leonardo DiCaprio", artist2[100] = "Kevin Spacey";
	ht* hashTable = (ht*) calloc(M, sizeof(ht));
	int* visited;
	QUEUE q;
	
	visited = (int*) calloc(M, sizeof(int));
	
	initializeHash(hashTable, M);
	initiliazeQueue(&q, M);
	
	/*file_reader(hashTable, M);
	printf("Elemanlar yerlestirildi. \n");

	kevin_bacon(artist1, artist2, hashTable, &q, visited, M);*/
	
	int fonk, out; 
	do {
		out = 1;
		system("cls");
		printf("_________KEVIN BACON SAYISINI BULMA__________ \n");
		printf("0 - Hash Tablosunu Olusturma \n");
		printf("1 - Artistler Arasinda Kevin Bacon Sayisini Bulma \n");
		printf("2 - Hash Tablosunu Goruntule \n");
		printf("3 - EXIT \n");
		printf("Islem yapmak istediginiz fonksiyonu seciniz : ");
		scanf("%d", &fonk);
		switch(fonk) {
			case 0:
				do {
					printf("_________________________________\n");
					printf("0 - Hash Tablosunu Olusturma \n");
					file_reader(hashTable, M);
					printf("Elemanlar yerlestirildi. \n");
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 1: 
				do {
					printf("_________________________________\n");
					printf("1 - Artistler Arasinda Kevin Bacon Sayisini Bulma \n");
					printf("Birinci artistin adini giriniz: ");
					scanf(" %[^\n]s", artist1);
					printf("Ikinci artistin adini giriniz: ");
					scanf(" %[^\n]s", artist2);
					kevin_bacon(artist1, artist2, hashTable, &q, visited, M);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 2: 
			do {
				printf("_________________________________\n");
				printf("2 - Hash Tablosunu Goruntule \n");
				print_hash(hashTable, M);
				
				printf("\nPress 0 to return the menu: ");
				scanf("%d", &out);
			} while(out != 0);
			break;
		}
	} while(fonk != 3);
	return 0;
}
