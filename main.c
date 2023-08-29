#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int Off;
int Ind;
int Tag;

typedef struct Bloco {
    unsigned int val;
    unsigned int tag;
} Bloco;

typedef struct Conj {
    Bloco* conjunto;
} Conj;

typedef struct Cache {
    int nsets;
    int bsize;
    int assoc;
    char subst;
    Conj* Conjs;
} Cache;

typedef struct CacheStats {
    int acessos;
    int hits;
    int misses;
    int comp_misses;
    int cap_misses;
    int conf_misses;
} CacheStats;

int main(int argc, char *argv[]) {
    if (argc != 7) {
        printf("Padrao utilizado: <nsets> <bsize> <assoc> <substituição> <flag_saida> arquivo_de_entrada\n");
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


    FILE *arq = fopen(file, "rb");
    if (!arq) {
        printf("Erro\n");
        return 1;
    }
}

void Set_Stats(CacheStats *stats, int total_accesses, int hits, int comp_misses, int cap_misses, int conf_misses) {
    stats->acessos = acessos;
    stats->hits = hits;
    stats->comp_misses = comp_misses;
    stats->cap_misses = cap_misses;
    stats->conf_misses = conf_misses;
    stats->misses = comp_misses + cap_misses + conf_misses;
}
