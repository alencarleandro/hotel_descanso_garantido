#include <stdio.h>  // Para manipulação de arquivos (FILE, fopen, fwrite, etc.)
#include <stdlib.h> // Para funções como perror
#include <string.h> // Para manipulação de strings (não usado aqui, mas bom incluir por hábito)

// --- Constantes e Tipos Globais ---

// [MELHORIA] Constantes para valores fixos evitam "números mágicos" e "strings mágicas".
#define NOME_ARQUIVO_QUARTOS "quartos.dat"
#define MAX_QUARTOS 100

// [MELHORIA] Constantes para os status dos quartos previnem erros de digitação
// e garantem consistência em todo o programa.
#define STATUS_OCUPADO "ocupado"
#define STATUS_DESOCUPADO "desocupado"

/**
 * @struct Quarto
 * @brief Estrutura de dados para representar um quarto de hotel.
 *
 * @var Quarto::numero_quarto
 * Identificador único do quarto.
 * @var Quarto::quantidade_hospedes
 * Capacidade máxima de hóspedes do quarto.
 * @var Quarto::valor_diaria
 * Custo da diária para este quarto.
 * @var Quarto::status
 * Situação atual do quarto (ex: "ocupado", "desocupado").
 */
typedef struct {
    int numero_quarto;
    int quantidade_hospedes;
    float valor_diaria;
    char status[20];
} Quarto;


// --- Funções de Persistência (Leitura/Gravação em Arquivo) ---

/**
 * @brief Salva o array de quartos em um arquivo binário.
 * @param quartos O array de quartos a ser salvo.
 * @param num_quartos O número de quartos no array.
 */
void salvar_quartos(Quarto quartos[], int num_quartos) {
    // Abre o arquivo para escrita em modo binário ("wb").
    // Usa a constante NOME_ARQUIVO_QUARTOS para o nome do arquivo.
    FILE *file = fopen(NOME_ARQUIVO_QUARTOS, "wb");

    // [ROBUSTEZ] Esta é a melhoria mais importante. Sempre verifique se o arquivo
    // foi aberto com sucesso. Se o fopen() falhar, ele retorna NULL.
    if (file == NULL) {
        // perror() é ideal para erros de sistema, pois imprime a mensagem de erro
        // padrão do sistema operacional (ex: "Permission denied").
        perror("Erro ao abrir o arquivo para salvar os quartos");
        return; // Retorna da função para evitar tentar escrever em um ponteiro nulo.
    }

    // Grava primeiro a quantidade de quartos no arquivo.
    fwrite(&num_quartos, sizeof(int), 1, file);
    // Em seguida, grava os dados de todos os quartos do array.
    fwrite(quartos, sizeof(Quarto), num_quartos, file);

    // Fecha o arquivo para garantir que todos os dados sejam gravados no disco.
    fclose(file);
}

/**
 * @brief Carrega os dados dos quartos de um arquivo binário para um array.
 * @param quartos O array que receberá os dados dos quartos.
 * @param num_quartos Um ponteiro para a variável que armazena o número de quartos.
 * Seu valor será atualizado pela função.
 */
void carregar_quartos(Quarto quartos[], int *num_quartos) {
    // Abre o arquivo para leitura em modo binário ("rb").
    FILE *file = fopen(NOME_ARQUIVO_QUARTOS, "rb");

    // Verifica se o arquivo existe. Se não existir (file == NULL),
    // significa que é a primeira vez que o programa está rodando ou o arquivo foi deletado.
    if (file) {
        // Se o arquivo existe, lê os dados.
        // Primeiro, lê a quantidade de quartos.
        fread(num_quartos, sizeof(int), 1, file);
        // Depois, lê os dados de todos os quartos para o array.
        fread(quartos, sizeof(Quarto), *num_quartos, file);
        // Fecha o arquivo após a leitura.
        fclose(file);
    } else {
        // Se o arquivo não existe, consideramos que não há quartos cadastrados.
        // Isso é um comportamento esperado, não um erro.
        *num_quartos = 0;
    }
}


// --- Função de Exemplo (para demonstrar o uso) ---

/**
 * @brief Lista todos os quartos cadastrados no console.
 * @param quartos O array de quartos.
 * @param num_quartos O número de quartos no array.
 */
void listar_todos_os_quartos(Quarto quartos[], int num_quartos) {
    printf("\n--- Lista de Todos os Quartos (%d) ---\n", num_quartos);
    if (num_quartos == 0) {
        printf("Nenhum quarto cadastrado.\n");
        return;
    }

    // Imprime um cabeçalho para a tabela
    printf("----------------------------------------------------------\n");
    printf("| No. Quarto | Capacidade | Valor Diaria (R$) | Status     |\n");
    printf("----------------------------------------------------------\n");

    for (int i = 0; i < num_quartos; i++) {
        printf("| %-10d | %-10d | %-17.2f | %-10s |\n",
               quartos[i].numero_quarto,
               quartos[i].quantidade_hospedes,
               quartos[i].valor_diaria,
               quartos[i].status);
    }
    printf("----------------------------------------------------------\n");
}

/*
// Exemplo de como a função main poderia interagir com este módulo
int main(void) {
    Quarto quartos[MAX_QUARTOS];
    int num_quartos = 0;

    // Carrega os dados existentes no início
    carregar_quartos(quartos, &num_quartos);

    // Se não houver quartos (primeira execução), cria alguns exemplos
    if (num_quartos == 0) {
        printf("Criando quartos iniciais...\n");
        quartos[0] = (Quarto){101, 2, 150.0, STATUS_DESOCUPADO};
        quartos[1] = (Quarto){102, 3, 200.0, STATUS_DESOCUPADO};
        quartos[2] = (Quarto){201, 4, 250.0, STATUS_DESOCUPADO};
        num_quartos = 3;
    }

    // Mostra os quartos que estão na memória
    listar_todos_os_quartos(quartos, num_quartos);

    // Salva todos os dados antes de sair
    printf("\nSalvando quartos...\n");
    salvar_quartos(quartos, num_quartos);
    printf("Quartos salvos com sucesso.\n");

    return 0;
}
*/