# Trabalho 2 -- AOC-II
## Especificações:
### Implementação de um Simulador de Caches -- Deverá ser implementado um simulador de caches, tal que deverá ser parametrizável, quanto ao número de conjuntos, tamanho do bloco, nível de associatividade e política de substituição. Considerando que a cache é endereçada à bytes e o endereço possui 32 bits.

- `<nsets>`: Número de conjuntos na cache.
- `<bsize>`: Tamanho do bloco em bytes.
- `<assoc>`: Grau de associatividade.
- `<substituição>`: Política de substituição (Há somente a implementação da política de substituição Random, R).
- `<flag_saida>`: Flag de saída (0 com labels, 1 para formato padrão).
- `<arquivo_de_entrada>`: Arquivo de entrada contendo os endereços para acesso à cache.

#### Para compilação, o padrão utilizado: gcc main.c -o cache_simulator -Wall -lm
##### Sobre a inclusão do "-lm", após a inserção da biblioteca math.h, foi necessário para compilação estar correta.


#### Na execução, o padrão utilizado: ./cache_simulator < nsets > < bsize > < assoc > < substituicao > <flag_saida> <arquivo_de_entrada>
##### Segue o exemplo:
##### ./cache_simulator 256 4 2 R 1 arquivo_de_teste.bin 


