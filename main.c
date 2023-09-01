#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

int OFF;
int IDX;
int TAG;

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


void setStats(CacheStats *stats, int acessos, int hits, int comp_misses, int cap_misses, int conf_misses);
uint32_t reverseAddress(int address);
void loadAdresses(Cache cache, int nsets, int assoc, char *file, int numBitsOffset, int numBitsIndex, CacheStats *stats);
void display ( int flag, CacheStats *cache );


int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Padrao utilizado:\n <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada\n");
        return 1;
    }

    CacheStats stats = {0};
    
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

    loadAdresses( cache, nsets, assoc, file, numBitsOffset, numBitsIndex, &stats );

    float hitRate = (float)stats.hits/(float)stats.acessos;
    float missRate = (float)stats.misses/(float)stats.acessos;
    float compMissRate = (float)stats.comp_misses/(float)stats.misses;
    float capMissRate = (float)stats.cap_misses/(float)stats.misses;
    float confMissRate = (float)stats.conf_misses/(float)stats.misses;

    printf("Acessos: %d\n", stats.acessos);
    printf("Taxa de Hits: %.2f\n", hitRate);
    printf("Taxa de Misses: %.2f\n", missRate);
    printf("Taxa de Misses Compulsorios: %.2f\n", compMissRate);
    printf("Taxa de Misses Capacidade: %.2f\n", capMissRate);
    printf("Taxa de Misses Conflito: %.2f\n", confMissRate);
}

void loadAdresses(Cache cache, int nsets, int assoc, char *file, int numBitsOffset, int numBitsIndex, CacheStats *stats){

    Conj blocos;

    cache.conjs = malloc(sizeof(Conj) * nsets); // nsets
    for (int i = 0; i < nsets; i++){
        cache.conjs[i].blocos = (Bloco*)malloc(sizeof(Bloco) * assoc); // assoc
        for (int j = 0; j < assoc; j++){
            cache.conjs[i].blocos[j].tag = -1;
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
    }

    fclose(arq);
    return;
}



void setStats(CacheStats *stats, int acessos, int hits, int comp_misses, int cap_misses, int conf_misses) {
    stats->acessos = acessos;
    stats->hits = hits;
    stats->comp_misses = comp_misses;
    stats->cap_misses = cap_misses;
    stats->conf_misses = conf_misses;
    stats->misses = comp_misses + cap_misses + conf_misses;
}

uint32_t reverseAddress(int address) {
    uint32_t mask = 0xff000000;
    uint32_t reversed = 0;

    reversed |= ((address<<(24))&mask);
    reversed |= ((address<<(8))&(mask>>(2*4)));
    reversed |= ((address>>(8))&(mask>>(4*4)));
    reversed |= ((address>>(24))&(mask>>(6*4)));

    return reversed;
}

void display ( int flag, CacheStats *cache ) {
    if ( flag == 0 ) {
        printf("Acessos = %d\n", cache->acessos);
        printf("Taxa de hits = %.2f\n", (float)cache->hits / cache->acessos);
        printf("Taxa de miss = %.2f\n", (float)cache->misses / cache->acessos);
        printf("Taxa de miss compulsorio = %.2f\n", (float)cache->comp_misses / cache->acessos);
        printf("Taxa de miss de capacidade = %.2f\n", (float)cache->cap_misses / cache->acessos);
        printf("Taxa de miss de conflito = %.2f\n", (float)cache->conf_misses / cache->acessos);
    } else if ( flag == 1 ) {
        printf( "%d, %.2f, %.2f, %.2f, %.2f, %.2f\n", cache->acessos,
               (float)cache->hits / cache->acessos,
               (float)cache->misses / cache->acessos,
               (float)cache->comp_misses / cache->acessos,
               (float)cache->cap_misses / cache->acessos,
               (float)cache->conf_misses / cache->acessos  );
    }
}
