#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

#define MAX_UINT 4294967295

unsigned int OFF;
unsigned int IDX;
unsigned int TAG;


typedef struct Bloco {
    unsigned int val;
    unsigned int tag;
} Bloco;

typedef struct Conj {
    Bloco* blocos;
} Conj;

typedef struct Cache {
    int nsets;
    int bsize;
    int assoc;
    char subst;
    Conj* conjs;
} Cache;

typedef struct CacheStats {
    int acessos;
    int hits;
    int misses;
    int comp_misses;
    int cap_misses;
    int conf_misses;
} CacheStats;


unsigned int reverseAddress(int address);
void loadAdresses(Cache cache, int nsets, int assoc, char *file, int numBitsOffset, int numBitsIndex, CacheStats *stats);
void printResults(int flag, CacheStats stats);
int randInt(int max);



int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Padrao utilizado: <nsets> <bsize> <assoc> <substituição> <flag_saida> <arquivo_de_entrada>\n");
        return 1;
    }
    if (argv[4][0] != 'R'){
        printf("Apenas a politica de substituicao RANDOM foi implementada.");
        return 1;
    }

    CacheStats stats = {0};
    srand(time(NULL));
    
    int nsets = atoi(argv[1]);
    int bsize = atoi(argv[2]);
    int assoc = atoi(argv[3]);
    char subst = argv[4][0];
    int flag_saida = atoi(argv[5]);
    char *file = argv[6];


    Cache cache;
    cache.nsets = nsets;
    cache.bsize = bsize;
    cache.assoc = assoc;
    cache.subst = subst;

    int numBitsOffset = log2(bsize);
    int numBitsIndex = log2(nsets);
    int numBitsTag = 32 - numBitsOffset - numBitsIndex;

    loadAdresses(cache, nsets, assoc, file, numBitsOffset, numBitsIndex, &stats);
    printResults(flag_saida, stats);

}

void loadAdresses(Cache cache, int nsets, int assoc, char *file, int numBitsOffset, int numBitsIndex, CacheStats *stats){

    Conj blocos;
    int ranIdx;
    int tagMissFlag = 1;
    int valCount = 0;
    int blockAmount = nsets * assoc;

    cache.conjs = malloc(sizeof(Conj) * nsets); // nsets
    for (int i = 0; i < nsets; i++){
        cache.conjs[i].blocos = (Bloco*)malloc(sizeof(Bloco) * assoc); // assoc
        for (int j = 0; j < assoc; j++){
            cache.conjs[i].blocos[j].tag = MAX_UINT;
            cache.conjs[i].blocos[j].val = 0;
        }
    }


    FILE *arq = fopen(file, "rb");
    if (!arq) {
        printf("Erro\n");
        return;
    }

    unsigned int address;

    while (fread(&address, 4, 1, arq) == 1){
        tagMissFlag = 1;
        address = reverseAddress(address);
        TAG = address >> (numBitsOffset + numBitsIndex);
        IDX = (address >> numBitsOffset) & ((int)(pow(2, numBitsIndex)-1));
        stats->acessos++;

        if (assoc == 1){        // Mapeamento Direto
            if (cache.conjs[IDX].blocos[0].val == 1){
                if (cache.conjs[IDX].blocos[0].tag == TAG){  // TAG bateu
                    stats->hits++;
                }
                else{  // TAG não bateu
                    stats->conf_misses++;
                    stats->misses++;
                    cache.conjs[IDX].blocos[0].tag = TAG;
                }
            }
            else{       // Validade = 0
                stats->comp_misses++;
                stats->misses++;
                cache.conjs[IDX].blocos[0].val = 1;
                cache.conjs[IDX].blocos[0].tag = TAG;
            }
        }
        
        else{         // Conjunto Associativo
            for (int i = 0; i < assoc; i++){
                if (cache.conjs[IDX].blocos[i].val == 1){
                    if (cache.conjs[IDX].blocos[i].tag == TAG){   // TAG bateu
                        stats->hits++;
                        tagMissFlag = 0;
                        break;
                    }
                }
                else{   // Validade = 0, primeira validade 0, para direita está tudo 0 também
                    stats->comp_misses++;
                    stats->misses++;
                    valCount++;
                    cache.conjs[IDX].blocos[i].val = 1;
                    cache.conjs[IDX].blocos[i].tag = TAG;
                    tagMissFlag = 0;
                    break;
                }

            }
            if (tagMissFlag){       // Nenhuma TAG bateu, sai aleatório
                ranIdx = randInt(assoc);
                cache.conjs[IDX].blocos[ranIdx].tag = TAG;
                stats->misses++;
                if (nsets == 1){            // Total assoc só miss de capacidade
                    stats->cap_misses++;
                }
                else{
                    if (valCount == blockAmount){       // cache cheia
                        stats->cap_misses++;
                    }
                    else{
                        stats->conf_misses++;
                    }
                }
            }
        }
    
    }

    fclose(arq);
    return;
}


void printResults(int flag, CacheStats stats){

    float hitRate = (float)stats.hits/(float)stats.acessos;
    float missRate = (float)stats.misses/(float)stats.acessos;
    float compMissRate = (float)stats.comp_misses/(float)stats.misses;
    float capMissRate = (float)stats.cap_misses/(float)stats.misses;
    float confMissRate = (float)stats.conf_misses/(float)stats.misses;

    if (flag == 0){
        printf("Acessos: %d\n", stats.acessos);
        printf("Taxa de Hits: %.4f\n", hitRate);
        printf("Taxa de Misses: %.4f\n", missRate);
        printf("Taxa de Misses Compulsorios: %.2f\n", compMissRate);
        printf("Taxa de Misses Capacidade: %.2f\n", capMissRate);
        printf("Taxa de Misses Conflito: %.2f\n", confMissRate);
    }
    else{
        printf("%d %.4f %.4f %.2f %.2f %.2f", stats.acessos, hitRate, missRate, compMissRate, capMissRate, confMissRate);
    }
}

int randInt(int max){

    return (rand() % max);
}


unsigned int reverseAddress(int address) {
    unsigned int mask = 0xff000000;
    unsigned int reversed = 0;

    reversed |= ((address<<(24))&mask);
    reversed |= ((address<<(8))&(mask>>(2*4)));
    reversed |= ((address>>(8))&(mask>>(4*4)));
    reversed |= ((address>>(24))&(mask>>(6*4)));

    return reversed;
}
