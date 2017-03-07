#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

typedef struct{
	long tag;
	int valid;
	int counter;
	int age;
	int id;
} line;

line **cache;
int missPen = 0;
int hitRate = 0;
int *pmissPen = &missPen;
int *phitRate = &hitRate;
unsigned long tag_mask;
unsigned long set_mask;
unsigned long *ptag_mask = &tag_mask;
unsigned long *pset_mask = &set_mask;
unsigned long zero_comp = ~0;

int is_full(int S, int E){
	for (int i = 0; i < S; i++){
		for (int j = 0; j < E; j++){
			if(cache[i][j].valid == 0){
				return 0;
			}
		}
	}
	return 1;
}



void initialize_cache(int S, int E){
	int tmp = 0;
	for (int i = 0; i < S; i++){
		for (int j = 0; j < E; j++){
			cache[i][j].tag = 0;
			cache[i][j].valid = 0;
			cache[i][j].counter = 0;
			cache[i][j].age = 0;
			cache[i][j].id = tmp;
			tmp++;
		}
	}
}

void inc_age(int S, int E, int setIndex, int tmp){
	for (int i = 0; i < S; i++){
		for (int j = 0; j < E; j++){
			if (cache[i][j].id != cache[setIndex][tmp].id){
				cache[i][j].age++;
				}
			}
		}
	}


int replace(char policy[], int set, int E){
	int lineIndex = 0;
	if (strcmp(policy,"LFU") == 0){
			int lfu = 10000;
			for (int i = 0; i < E; i++){
				if (cache[set][i].counter < lfu){
					lfu = cache[set][i].counter;
					lineIndex = i;
				}
			}
			return lineIndex;
		}
	 else if (strcmp(policy,"LRU") ==0){
		int lru = 0;
		for (int i = 0; i < E; i++){
			if (cache[set][i].age > lru){
				lru = cache[set][i].age;
				lineIndex = i;
			}
		}
		return lineIndex;
	} else {
		printf("You did not enter a valid replacement policy\n");
		return -1;
	}
}

int check_hit(long address, line* l){
	int ret_value = (l->valid && (l->tag == (address & tag_mask)));
	if(ret_value){
		l->counter++;
	}
	return ret_value;
}


int main(){

	int S, E, B, m, hitTime, missPenalty;
	char policy[4];
	scanf("%d",&S);
	scanf("%d",&E);
	scanf("%d",&B);
	scanf("%d",&m);
	scanf("%s",policy);
	scanf("%d",&hitTime);
	scanf("%d",&missPenalty);
	printf("\n");


	cache = malloc(sizeof(line*) * S);

	for (int i = 0; i < S; i++){
		cache[i] = malloc(sizeof(line)*E);
	}

	initialize_cache(S,E);
	int b = log2(B);
	int s = log2(S);
	int tagsize = m - (s + b);

	*ptag_mask = ~0 << (s + b);
	*pset_mask = ~(zero_comp << s) << b;

	int hexSize = log2(m);
	char hex[9];
	long address;
	double misses = 0;
	double accesses = 0;

	while(1){
		scanf("%lx",&address);
		if (address == -1){
			break;
		}
		char hit = 0;
		int tmp;
		long setIndex = (address & set_mask) >> b;
		for (int i = 0; i < E; i++){
			if (check_hit(address, &cache[setIndex][i])){
				hit = 1;
				tmp = i;
				break;
			} 
		}
		if (hit){
			printf("%lx  H\n",address);
			*phitRate += hitTime;
			inc_age(S,E,setIndex,tmp);
			accesses++;
		} else {
			printf("%lx  M\n",address);
			int line = replace(policy,setIndex,E);
			cache[setIndex][line].valid = 1;
			cache[setIndex][line].tag = (address & tag_mask);
			*pmissPen += missPenalty + hitTime;
			misses++;
			accesses++;
		}

	}
	
	double missRate = (misses/accesses) * 100;
	int clockCycles = missPen + hitRate;

	printf("Miss rate: %f Clock Cycles %d\n",missRate,clockCycles);

	for (int i = 0; i < S; i++){
		free(cache[i]);
	}
	free(cache);

	return 0;
}





