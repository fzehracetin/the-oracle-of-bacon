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
	q->queue = (int*) calloc(M, sizeof(int));
	q->parent = (int*) calloc(M, sizeof(int));
	q->rear = 0;
    q->front = 0;
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

unsigned long long int find_key (char *str) {
	int i = 0;
	unsigned long long int key = 0;
	int N = strlen(str);
	
	while (i < N) {
		key += str[i] * pow(R, N - i - 1);
		i++;
	}
	return key;
}

int insert_to_table (unsigned long long int key, char* token, ht* hashTable, unsigned long long int M, int* count) {
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
		*count = *count + 1;
		strcpy(hashTable[adr].name, token);
		hashTable[adr].head = NULL;
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

void tokenizer(char* line, ht* hashTable, unsigned long long int M, int* movie_count, int* artist_count) {
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
			movie_adr = insert_to_table(key, token, hashTable, M, movie_count);
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
			artist_adr = insert_to_table(key, buffer2, hashTable, M, artist_count);
			if (artist_adr != -1) {
				insert_node(hashTable, movie_adr, artist_adr);
				insert_node(hashTable, artist_adr, movie_adr);
			}
		}
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

void bfs (QUEUE *q, int start, int wanted, ht* hashTable, unsigned long long int M, int select) {
    int i = 0, flag = 0, j = 0, count = 0, v = start;
	node* current;
    int *yol = (int*) calloc(M, sizeof(int));
	int* visited = (int*) calloc(M, sizeof(int));
	
	q->front = 0; //initialize attributes
	q->rear = 0;
	
	q->parent[v] = -1;
    visited[v] = 1;
	enqueue(q, v);
    while ((!flag) && (q->front < q->rear)) {
        v = dequeue(q);
        if (hashTable[v].head) {
			current = hashTable[v].head;
			while (current) {
				if (visited[current->adr] != 1) {
					q->parent[current->adr] = v;
					visited[current->adr] = 1;
					enqueue(q, current->adr);
					if (current->adr == wanted) {
						flag = 1;
					}
				}
				current = current->next;
			}
        }
    }
	if (flag == 1) {
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
		} 
		if (!select) {
			if (count > 6) 
				printf("\nKevin Bacon Sayisi %d oldugundan Kevin Bacon ile baglanti yoktur. \n", count);
			else
				printf("\n%s Kevin Bacon Sayisi: %d\n", hashTable[wanted].name, count);
		}
		else if (select)
			printf("\n%s - %s: %d\n", hashTable[start].name, hashTable[wanted].name, count);
	}
}

void kevin_bacon (char* artist1, char* artist2, ht* hashTable, QUEUE *q, unsigned long long int M, int select) {
	int adr1, adr2;
	
	adr1 = search(artist1, hashTable, M);
	adr2 = search(artist2, hashTable, M);
	if ((adr1 == -1) || (adr2 == -1))
		return;
	printf("\n%s %d nolu adreste.\n", artist1, adr1);
	printf("%s %d nolu adreste.\n", artist2, adr2);
	bfs(q, adr1, adr2, hashTable, M, select);
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
void print_connections (ht* hashTable, char* element, long long int M) {
	int i;
	node* current;
	i = search(element, hashTable, M);
	if (i == -1 )
		return;
	if (hashTable[i].name[0] != '\0') {
		printf("%s: ", hashTable[i].name);
		current = hashTable[i].head;
		if (hashTable[i].head != NULL) {
			printf("%s", hashTable[current->adr].name);
			current = current->next;
			while (current) {
				printf(", ");
				printf("%s", hashTable[current->adr].name);
				current = current->next;
			} printf("\n");
		}
	}
	
}

void file_reader(ht* hashTable, unsigned long long int M) 
{
	FILE *fp;
	char line[SIZE];
	int movie_count = 0, artist_count = 0;
	
	fp = fopen("input-mpaa.txt", "r");
	if (fp == NULL) {
		printf("File could not opened !!\n");
		return;
	}
	while ( fgets(line, SIZE - 1, fp) ) {
		tokenizer(line, hashTable, M, &movie_count, &artist_count);
	}
	printf("\n%d film, %d artist ve tum baglantilari yerlestirildi. \n", movie_count, artist_count);
	printf("Toplam unique eleman sayisi: %d \n", movie_count + artist_count);
}

int main(int argc, char **argv)
{
	char artist1[100], artist2[100];
	unsigned long long int M = 323567;
	ht* hashTable = (ht*) calloc(M, sizeof(ht));
	
	QUEUE q;

	initializeHash(hashTable, M);
	initiliazeQueue(&q, M);
	
	int fonk, out; 
	do {
		out = 1;
		system("cls");
		printf("_________KEVIN BACON SAYISINI BULMA__________ \n");
		printf("0 - GRAF YAPISINI OLUSTURMA \n");
		printf("1 - BIR ARTISTIN KEVIN BACON SAYISINI BULMA  \n");
		printf("2 - IKI ARTIST ARASINDAKI UZAKLIGI BULMA \n");
		printf("3 - BIR ARTIST YA DA FILMIN BAGLANTILARINI GOSTER \n");
		printf("4 - GRAF YAPISINI GORUNTULE \n");
		printf("5 - EXIT \n");
		printf("Islem yapmak istediginiz fonksiyonu seciniz : ");
		scanf("%d", &fonk);
		switch(fonk) {
			case 0:
				system("cls");
				do {
					printf("____________________GRAF YAPISINI OLUSTURMA____________________\n");
					file_reader(hashTable, M);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 1: 
				system("cls");
				do {
					printf("____________________KEVIN BACON SAYISINI BULMA____________________ \n");
					strcpy(artist1, "Kevin Bacon");
					printf("**UYARI** : Artist adi Firstname Lastname sirasi ile girilecektir! \n");
					printf("            Ornek input: Kevin Bacon\n");
					printf("\nKevin Bacon'a uzakligi bulunacak artistin adini giriniz: ");
					scanf(" %[^\n]s", artist2);
					kevin_bacon(artist1, artist2, hashTable, &q, M, 0);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 2: 
				system("cls");
				do {
					printf("____________________IKI ARTIST ARASINDAKI UZAKLIGI BULMA____________________\n");
					printf("**UYARI** : Artist adi Firstname Lastname sirasi ile girilecektir... \n");
					printf("            Ornek input: Kevin Bacon\n");
					printf("\nBirinci artistin adini giriniz: ");
					scanf(" %[^\n]s", artist1);
					printf("Ikinci artistin adini giriniz: ");
					scanf(" %[^\n]s", artist2);
					kevin_bacon(artist1, artist2, hashTable, &q, M, 1);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 3: 
				system("cls");
				do {
					printf("____________________BIR ARTIST YA DA FILMIN BAGLANTILARINI GOSTER____________________\n");
					printf("**UYARI** : Artist adi Firstname Lastname sirasi ile girilecektir! \n");
					printf("            Ornek input: Kevin Bacon\n");
					printf("**UYARI** : Film isimlerinin yaninda parantez icerisinde cikis yili yazmalidir! \n");
					printf("            Ornek input: Fight Club (1999)\n");
					printf("\nBaglantilari bulunacak artist ya da filmin adini giriniz: ");
					scanf(" %[^\n]s", artist1);
					print_connections(hashTable, artist1, M);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
			case 4: 
				system("cls");
				do {
					printf("____________________GRAF YAPISINI GORUNTULE____________________\n");
					print_hash(hashTable, M);
					printf("\nPress 0 to return the menu: ");
					scanf("%d", &out);
				} while(out != 0);
				break;
		}
	} while(fonk != 5);
	return 0;
}
