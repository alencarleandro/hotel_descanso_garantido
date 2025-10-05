#include <stdio.h>
#include <string.h>
#include <time.h>   // Para lidar com as datas e horas (struct tm)
#include <stdlib.h> // Para usar exit()

// --- Constantes e Definições de Tipos ---

// [MELHORIA] Constantes para nomes de arquivos e limites de arrays.
#define NOME_ARQUIVO_ESTADIAS "estadias.dat"
#define MAX_ESTADIAS 100

// [MELHORIA] Constantes para os status dos quartos evitam erros de digitação
// e facilitam a manutenção. ("ocupado" vs "OCUPADO" vs "Ocupado").
#define STATUS_OCUPADO "ocupado"
#define STATUS_DESOCUPADO "desocupado"

// Definições de struct que foram usadas mas não declaradas no código original.
// Adicionadas para que o código seja compilável e completo.
typedef struct {
    int numero_quarto;
    int quantidade_hospedes;
    float valor_diaria;
    char status[20]; // "ocupado" ou "desocupado"
} Quarto;

typedef struct {
    int codigo;
    char nome[100];
} Cliente;

// Estrutura principal para uma estadia.
typedef struct {
    int codigo_estadia;
    struct tm data_entrada; // struct tm é o tipo padrão em C para data/hora
    struct tm data_saida;
    int quantidade_diarias;
    int codigo_cliente;
    int numero_quarto;
} Estadia;


// --- Funções Auxiliares (Lógica de Negócio e Helpers) ---

/**
 * @brief Imprime os detalhes de uma estadia de forma organizada.
 * @param estadia A estadia a ser impressa.
 */
void imprimir_detalhes_estadia(const Estadia* estadia) {
    // [DRY] Centraliza a impressão dos dados da estadia, evitando repetição.
    printf("\n----------------------------------\n");
    printf("Codigo da Estadia: %d\n", estadia->codigo_estadia);
    printf("Quarto:            %d\n", estadia->numero_quarto);
    printf("Cliente (Cod):     %d\n", estadia->codigo_cliente);
    // As datas em struct tm precisam de ajustes para exibição:
    // tm_mon é 0-11, então somamos 1.
    // tm_year é o número de anos desde 1900, então somamos 1900.
    printf("Data de Entrada:   %02d/%02d/%04d %02d:%02d\n",
           estadia->data_entrada.tm_mday, estadia->data_entrada.tm_mon + 1, estadia->data_entrada.tm_year + 1900,
           estadia->data_entrada.tm_hour, estadia->data_entrada.tm_min);
    printf("Data de Saida:     %02d/%02d/%04d %02d:%02d\n",
           estadia->data_saida.tm_mday, estadia->data_saida.tm_mon + 1, estadia->data_saida.tm_year + 1900,
           estadia->data_saida.tm_hour, estadia->data_saida.tm_min);
    printf("Total de Diarias:  %d\n", estadia->quantidade_diarias);
    printf("----------------------------------\n");
}

/**
 * @brief Encontra o índice de uma estadia no array a partir do seu código.
 * @return O índice da estadia se encontrada, ou -1 caso contrário.
 */
int buscar_indice_estadia(Estadia estadias[], int num_estadias, int codigo_estadia) {
    // [ARQUITETURA] Uma função focada apenas em buscar, sem interagir com o usuário.
    for (int i = 0; i < num_estadias; i++) {
        if (estadias[i].codigo_estadia == codigo_estadia) {
            return i; // Encontrado!
        }
    }
    return -1; // Não encontrado
}

/**
 * @brief Remove uma estadia do array a partir do seu índice.
 * @param estadias O array de estadias.
 * @param num_estadias Ponteiro para o número total de estadias.
 * @param indice_para_remover O índice do elemento a ser removido.
 */
void remover_estadia_do_array(Estadia estadias[], int *num_estadias, int indice_para_remover) {
    // [ARQUITETURA] Encapsula a lógica de remoção, que pode ser complexa.
    // Move todos os elementos após o índice uma posição para a esquerda.
    for (int i = indice_para_remover; i < (*num_estadias - 1); i++) {
        estadias[i] = estadias[i + 1];
    }
    (*num_estadias)--; // Decrementa o contador total.
}

/**
 * @brief Gera um novo código de estadia seguro (maior ID existente + 1).
 */
int gerar_novo_codigo_estadia(Estadia estadias[], int num_estadias) {
    // [CORREÇÃO DE BUG] Usar 'num_estadias + 1' como ID é perigoso, pois a remoção
    // de estadias pode levar a IDs duplicados. Esta abordagem é mais segura.
    if (num_estadias == 0) {
        return 1; // Se for a primeira estadia
    }
    int max_codigo = estadias[0].codigo_estadia;
    for (int i = 1; i < num_estadias; i++) {
        if (estadias[i].codigo_estadia > max_codigo) {
            max_codigo = estadias[i].codigo_estadia;
        }
    }
    return max_codigo + 1;
}

// --- Funções de Persistência (Leitura/Gravação em Arquivo) ---

void salvar_estadias(Estadia estadias[], int num_estadias) {
    FILE *file = fopen(NOME_ARQUIVO_ESTADIAS, "wb");
    // [ROBUSTEZ] Sempre verifique o retorno do fopen para garantir que o arquivo abriu.
    if (file == NULL) {
        perror("Erro ao abrir arquivo para salvar estadias");
        return;
    }
    fwrite(&num_estadias, sizeof(int), 1, file);
    fwrite(estadias, sizeof(Estadia), num_estadias, file);
    fclose(file);
}

void carregar_estadias(Estadia estadias[], int *num_estadias) {
    FILE *file = fopen(NOME_ARQUIVO_ESTADIAS, "rb");
    if (file) {
        fread(num_estadias, sizeof(int), 1, file);
        fread(estadias, sizeof(Estadia), *num_estadias, file);
        fclose(file);
    } else {
        // Se o arquivo não existe (primeira execução), inicia com zero estadias.
        *num_estadias = 0;
    }
}


// --- Funções de Interface com o Usuário (UI) ---

void listar_estadias_cliente(Estadia estadias[], int num_estadias, int codigo_cliente) {
    printf("\n--- Listando Estadias do Cliente (Cod: %d) ---\n", codigo_cliente);
    int encontradas = 0;

    // [LEITURA] Loop 'for' é mais adequado para percorrer o array.
    for (int i = 0; i < num_estadias; i++) {
        if (estadias[i].codigo_cliente == codigo_cliente) {
            // Chama a função auxiliar para imprimir os detalhes.
            imprimir_detalhes_estadia(&estadias[i]);
            encontradas++;
        }
    }

    if (encontradas == 0) {
        printf("Nenhuma estadia encontrada para este cliente.\n");
    }
}

void dar_baixa_estadia(Estadia estadias[], int *num_estadias, Quarto quartos[], int num_quartos) {
    printf("\n--- Dar Baixa em Estadia (Check-out) ---\n");
    int codigo_estadia;
    printf("Digite o codigo da estadia para finalizar: ");
    scanf("%d", &codigo_estadia);

    int indice_estadia = buscar_indice_estadia(estadias, *num_estadias, codigo_estadia);

    if (indice_estadia == -1) {
        printf("Erro: Estadia com codigo %d nao encontrada.\n", codigo_estadia);
        return;
    }

    // Lógica para encontrar o quarto e calcular o valor
    int numero_quarto = estadias[indice_estadia].numero_quarto;
    int diarias = estadias[indice_estadia].quantidade_diarias;
    float valor_diaria = 0.0f;
    int quarto_encontrado = 0;

    for (int i = 0; i < num_quartos; i++) {
        if (quartos[i].numero_quarto == numero_quarto) {
            valor_diaria = quartos[i].valor_diaria;
            strcpy(quartos[i].status, STATUS_DESOCUPADO); // Usa a constante
            quarto_encontrado = 1;
            break;
        }
    }

    if (!quarto_encontrado) {
        // Isso indica uma inconsistência nos dados (estadia aponta para um quarto que não existe)
        printf("Erro critico: O quarto %d associado a esta estadia nao foi encontrado!\n", numero_quarto);
        return;
    }

    float valor_total = diarias * valor_diaria;
    printf("Valor total a ser pago: R$%.2f\n", valor_total);

    // Remove a estadia do array usando a função auxiliar
    remover_estadia_do_array(estadias, num_estadias, indice_estadia);
    printf("Estadia finalizada e quarto liberado com sucesso.\n");
}

void cadastrar_estadia(Estadia estadias[], int *num_estadias, Cliente clientes[], int num_clientes, Quarto quartos[], int num_quartos) {
    // Esta função é longa por natureza, mas a lógica de busca foi simplificada.
    if (*num_estadias >= MAX_ESTADIAS) {
        printf("Erro: Limite maximo de estadias atingido.\n");
        return;
    }
    
    printf("\n--- Cadastrar Nova Estadia (Check-in) ---\n");
    
    // 1. Validar Cliente
    int codigo_cliente;
    printf("Digite o codigo do cliente: ");
    scanf("%d", &codigo_cliente);
    // (Em um sistema real, aqui chamaríamos uma função 'buscar_cliente_por_codigo')
    int cliente_valido = 0;
    for(int i = 0; i < num_clientes; i++) { if(clientes[i].codigo == codigo_cliente) { cliente_valido = 1; break; } }
    if (!cliente_valido) {
        printf("Erro: Cliente com codigo %d nao cadastrado.\n", codigo_cliente);
        return;
    }

    // 2. Encontrar Quarto Disponível
    int quantidade_hospedes;
    printf("Digite a quantidade de hospedes: ");
    scanf("%d", &quantidade_hospedes);
    
    int indice_quarto_disponivel = -1;
    for (int i = 0; i < num_quartos; i++) {
        if (quartos[i].quantidade_hospedes >= quantidade_hospedes && strcmp(quartos[i].status, STATUS_DESOCUPADO) == 0) {
            indice_quarto_disponivel = i;
            break; // Encontrou o primeiro quarto que serve
        }
    }

    if (indice_quarto_disponivel == -1) {
        printf("Erro: Nao ha quartos desocupados que comportem %d hospedes.\n", quantidade_hospedes);
        return;
    }

    // 3. Coletar Dados e Criar Estadia
    Estadia nova_estadia;
    nova_estadia.codigo_cliente = codigo_cliente;
    nova_estadia.numero_quarto = quartos[indice_quarto_disponivel].numero_quarto;
    
    printf("Digite a data de entrada (dd mm aaaa hh mm): ");
    scanf("%d %d %d %d %d", &nova_estadia.data_entrada.tm_mday, &nova_estadia.data_entrada.tm_mon, &nova_estadia.data_entrada.tm_year, &nova_estadia.data_entrada.tm_hour, &nova_estadia.data_entrada.tm_min);
    nova_estadia.data_entrada.tm_mon -= 1;
    nova_estadia.data_entrada.tm_year -= 1900;

    printf("Digite a data de saida (dd mm aaaa hh mm): ");
    scanf("%d %d %d %d %d", &nova_estadia.data_saida.tm_mday, &nova_estadia.data_saida.tm_mon, &nova_estadia.data_saida.tm_year, &nova_estadia.data_saida.tm_hour, &nova_estadia.data_saida.tm_min);
    nova_estadia.data_saida.tm_mon -= 1;
    nova_estadia.data_saida.tm_year -= 1900;

    // Calcular diferença de tempo para as diárias
    time_t entrada = mktime(&nova_estadia.data_entrada);
    time_t saida = mktime(&nova_estadia.data_saida);
    if (entrada == -1 || saida == -1) {
        printf("Erro: Data invalida inserida.\n");
        return;
    }
    double segundos = difftime(saida, entrada);
    // Arredonda para cima, considerando que a fração de um dia conta como uma diária completa.
    nova_estadia.quantidade_diarias = (int)(segundos / (60 * 60 * 24)) + 1;
    if(nova_estadia.quantidade_diarias <= 0) nova_estadia.quantidade_diarias = 1;

    // 4. Salvar Alterações
    nova_estadia.codigo_estadia = gerar_novo_codigo_estadia(estadias, *num_estadias);
    strcpy(quartos[indice_quarto_disponivel].status, STATUS_OCUPADO); // Atualiza status do quarto
    
    estadias[*num_estadias] = nova_estadia; // Adiciona ao array
    (*num_estadias)++;
    
    printf("Estadia cadastrada com sucesso! Codigo: %d, Quarto: %d.\n", nova_estadia.codigo_estadia, nova_estadia.numero_quarto);
}