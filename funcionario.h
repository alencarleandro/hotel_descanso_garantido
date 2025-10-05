#include <stdio.h>
#include <string.h> // Para usar strcmp() e strcspn()
#include <stdlib.h> // Para usar exit()

// --- Constantes e Tipos Globais ---

// [MELHORIA] Uso de constantes para valores fixos melhora a manutenção e legibilidade.
#define NOME_ARQUIVO_FUNCIONARIOS "funcionarios.dat"
#define MAX_FUNCIONARIOS 100

// [MELHORIA] Enum para opções de pesquisa torna o código mais claro do que usar "números mágicos" 1 e 2.
typedef enum {
    PESQUISA_FUNC_POR_CODIGO = 1,
    PESQUISA_FUNC_POR_NOME = 2
} TipoPesquisaFuncionario;

// Estrutura de dados para representar um funcionário.
typedef struct {
    int codigo;
    char nome[100];
    char telefone[20];
    char cargo[50];
    float salario;
} Funcionario;


// --- Funções Auxiliares ---

/**
 * @brief Lê uma string do teclado de forma segura para prevenir buffer overflow.
 * @param buffer Onde a string será armazenada.
 * @param tamanho O tamanho máximo do buffer.
 */
void ler_string_segura(char *buffer, int tamanho) {
    // [SEGURANÇA] fgets é a função recomendada para ler strings em C de forma segura.
    fgets(buffer, tamanho, stdin);
    // Remove o caractere '\n' (Enter) que o fgets captura no final da string.
    buffer[strcspn(buffer, "\n")] = 0;
}

/**
 * @brief Imprime os dados de um funcionário de forma formatada.
 * @param func O funcionário a ser impresso.
 */
void imprimir_funcionario(const Funcionario* func) {
    // [DRY] Centraliza a impressão dos dados, evitando repetição de código.
    printf("\n--- Funcionario Encontrado ---\n");
    printf("Codigo:   %d\n", func->codigo);
    printf("Nome:     %s\n", func->nome);
    printf("Telefone: %s\n", func->telefone);
    printf("Cargo:    %s\n", func->cargo);
    printf("Salario:  R$%.2f\n", func->salario);
    printf("----------------------------\n");
}


// --- Lógica de Negócio (Core) ---

/**
 * @brief Busca um funcionário pelo código no array.
 * @param funcionarios O array de funcionários.
 * @param num_funcionarios O número atual de funcionários.
 * @param codigo O código a ser pesquisado.
 * @return O índice do funcionário no array se encontrado, ou -1 caso contrário.
 */
int buscar_funcionario_por_codigo(Funcionario funcionarios[], int num_funcionarios, int codigo) {
    // [ARQUITETURA] Função com responsabilidade única de buscar, sem interagir com o usuário.
    // [LEITURA] Loop 'for' é mais idiomático para percorrer um array de tamanho conhecido.
    for (int i = 0; i < num_funcionarios; i++) {
        if (funcionarios[i].codigo == codigo) {
            return i; // Retorna o índice do funcionário encontrado.
        }
    }
    return -1; // Sinaliza que não encontrou.
}


// --- Funções de Interface com o Usuário (UI) ---

/**
 * @brief Gerencia o fluxo de cadastro de um novo funcionário.
 * @param funcionarios O array de funcionários.
 * @param num_funcionarios Ponteiro para o número atual de funcionários.
 */
void cadastrar_funcionario(Funcionario funcionarios[], int *num_funcionarios) {
    if (*num_funcionarios >= MAX_FUNCIONARIOS) {
        printf("Erro: Limite maximo de funcionarios atingido.\n");
        return;
    }
    
    Funcionario novo_funcionario;
    int codigo_digitado;

    printf("\n--- Cadastro de Novo Funcionario ---\n");
    printf("Digite o codigo do funcionario: ");
    scanf("%d", &codigo_digitado);
    getchar(); // Limpa o buffer do teclado (o '\n' deixado pelo scanf).

    // [ARQUITETURA] A UI chama a lógica de negócio (a função de busca) para validar os dados.
    if (buscar_funcionario_por_codigo(funcionarios, *num_funcionarios, codigo_digitado) != -1) {
        printf("Erro: Ja existe um funcionario com esse codigo.\n");
        return;
    }
    novo_funcionario.codigo = codigo_digitado;

    printf("Digite o nome do funcionario: ");
    ler_string_segura(novo_funcionario.nome, sizeof(novo_funcionario.nome));
    
    printf("Digite o telefone do funcionario: ");
    ler_string_segura(novo_funcionario.telefone, sizeof(novo_funcionario.telefone));
    
    printf("Digite o cargo do funcionario: ");
    ler_string_segura(novo_funcionario.cargo, sizeof(novo_funcionario.cargo));
    
    printf("Digite o salario do funcionario: ");
    scanf("%f", &novo_funcionario.salario);
    getchar(); // Limpa o buffer novamente.

    // Adiciona o novo funcionário ao final do array
    funcionarios[*num_funcionarios] = novo_funcionario;
    (*num_funcionarios)++; // Incrementa o contador de funcionários
    
    printf("\nFuncionario cadastrado com sucesso!\n");
}

/**
 * @brief Gerencia o fluxo de pesquisa de um funcionário.
 * @param funcionarios O array de funcionários.
 * @param num_funcionarios O número atual de funcionários.
 */
void pesquisar_funcionario(Funcionario funcionarios[], int num_funcionarios) {
    int opcao;
    printf("\n--- Pesquisa de Funcionario ---\n");
    printf("Pesquisar por: %d. Codigo ou %d. Nome: ", PESQUISA_FUNC_POR_CODIGO, PESQUISA_FUNC_POR_NOME);
    scanf("%d", &opcao);
    getchar(); // Limpa o buffer

    if (opcao == PESQUISA_FUNC_POR_CODIGO) {
        int codigo_busca;
        printf("Digite o codigo do funcionario: ");
        scanf("%d", &codigo_busca);
        getchar();

        int indice = buscar_funcionario_por_codigo(funcionarios, num_funcionarios, codigo_busca);
        if (indice != -1) {
            imprimir_funcionario(&funcionarios[indice]);
        } else {
            printf("Funcionario com codigo %d nao encontrado.\n", codigo_busca);
        }

    } else if (opcao == PESQUISA_FUNC_POR_NOME) {
        char nome_busca[100];
        printf("Digite o nome do funcionario: ");
        ler_string_segura(nome_busca, sizeof(nome_busca));

        int encontrado = 0;
        for (int i = 0; i < num_funcionarios; i++) {
            // strcmp retorna 0 se as strings forem iguais.
            if (strcmp(funcionarios[i].nome, nome_busca) == 0) {
                imprimir_funcionario(&funcionarios[i]);
                encontrado = 1;
            }
        }
        if (!encontrado) {
            printf("Nenhum funcionario com o nome '%s' foi encontrado.\n", nome_busca);
        }

    } else {
        printf("Opcao invalida.\n");
    }
}


// --- Funções de Persistência (Leitura/Gravação em Arquivo) ---

/**
 * @brief Salva o array de funcionários em um arquivo binário.
 * @param funcionarios O array de funcionários.
 * @param num_funcionarios O número atual de funcionários.
 */
void salvar_funcionarios(Funcionario funcionarios[], int num_funcionarios) {
    FILE *file = fopen(NOME_ARQUIVO_FUNCIONARIOS, "wb");

    // [ROBUSTEZ] É crucial verificar se o arquivo foi aberto com sucesso antes de escrever nele.
    if (file == NULL) {
        perror("Erro ao abrir o arquivo para salvar funcionarios");
        return;
    }

    // Grava primeiro a quantidade de registros, para facilitar a leitura depois.
    fwrite(&num_funcionarios, sizeof(int), 1, file);
    // Grava todos os dados do array de uma vez.
    fwrite(funcionarios, sizeof(Funcionario), num_funcionarios, file);
    
    fclose(file);
}

/**
 * @brief Carrega os funcionários de um arquivo binário para o array.
 * @param funcionarios O array onde os funcionários serão carregados.
 * @param num_funcionarios Ponteiro que será atualizado com a quantidade de funcionários lidos.
 */
void carregar_funcionarios(Funcionario funcionarios[], int *num_funcionarios) {
    FILE *file = fopen(NOME_ARQUIVO_FUNCIONARIOS, "rb");

    // Se o arquivo não existe (ex: primeira vez rodando), fopen retorna NULL.
    if (file) {
        fread(num_funcionarios, sizeof(int), 1, file);
        fread(funcionarios, sizeof(Funcionario), *num_funcionarios, file);
        fclose(file);
    } else {
        // Se não há arquivo, simplesmente começamos com um contador zerado.
        *num_funcionarios = 0;
    }
}