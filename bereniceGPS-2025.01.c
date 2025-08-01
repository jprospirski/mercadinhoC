/*Programa desenvolvido pelos GAROTOS DE PROGRAMA.
Integrantes: 1. João Rospirski 2. João Tedesqui 3. Cauã Domingues 4. Thiago Rocha
Atendendo os requisitos da entregra mensal. Contém os seguintes sistemas no código:
Alocação dinamica de memória (malloc e realloc).
Implementação de Árvore binária (conceito).
Implementação de Lista Encadeada (conceito).
Implementação de ordenação por QuickSort (qsort).*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <time.h>

// --- DEFINES ---
// Define nomes de arquivos para persistência de dados.
#define ARQPRODUTOS "produtos.txt"                   // Arquivo de estoque de produtos
#define ARQCAIXA "caixa.txt"                         // Arquivo com o saldo do caixa
#define ARQ_VDETALHADAS "vendas_detalhadas.txt" // Arquivo com o registro de cada item vendido
#define ARQLOG "log.txt"                             // Arquivo de log para eventos e erros do sistema
#define MAX_TAMANHO 100                              // Tamanho máximo padrão para strings (nomes, categorias, etc.)

// --- VARIÁVEIS GLOBAIS ---
// Variáveis que mantêm o estado atual da sessão de vendas.
float totalpendente = 0; // Armazena o valor total dos itens no carrinho da compra atual.
int caixa_aberto = 0;    //(0 ou 1) que indica se o caixa está aberto ou fechado.
float saldo_caixa = 0;   // Armazena o saldo monetário atual do caixa.

// --- ESTRUTURAS ---
// Struct feita para os produtos.
typedef struct produto
{
    char categoria[MAX_TAMANHO];
    int codigo;
    char nome[MAX_TAMANHO];
    float preco;
    int quantidade;
} produto;

// Struct feita para a arvore binaria.
typedef struct no
{
    produto p;
    struct no *esquerda;
    struct no *direita;
} no;

// Struct feita para o carrinho (lista encadeada)
typedef struct item_carrinho
{
    produto p_vendido;
    int quantidade_vendida;
    struct item_carrinho *proximo;
} item_carrinho;

// Struct feita para armazenar informações de vendas detalhadas
typedef struct
{
    char data_hora[20];
    int codigo;
    char nome[MAX_TAMANHO];
    int quantidade;
    float preco_unitario;
} venda_detalhada;

item_carrinho *carrinho_atual = NULL;//Criação de um ponteiro do tipo "item_carrinho" iniciado com NULL. Utilizado para começar encadear a lista

// --- FUNÇÕES AUXILIARES ---

//Função para limpar o buff do teclado evitando possiveis erros.
void limpar_buffer() {

    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

//Registra uma mensagem com data e hora no arquivo de log. Útil para rastrear eventos importantes e erros do sistema.
void registrar_log(const char *mensagem) {

    FILE *arquivo = fopen(ARQLOG, "a");
    if (arquivo == NULL) {
        printf("Erro ao abrir arquivo de log"); // Se não conseguir abrir o log, imprime o erro no console.
        return;
    }

    // Bloco para obter e formatar a data e hora atuais.
    time_t tempo_atual;
    char buffer_tempo[26];
    struct tm *info_tempo;

    time(&tempo_atual);
    info_tempo = localtime(&tempo_atual);
    strftime(buffer_tempo, sizeof(buffer_tempo), "%Y-%m-%d %H:%M:%S", info_tempo);

    fprintf(arquivo, "[%s] %s\n", buffer_tempo, mensagem); //Recebe apenas a mensagem a ser gravada e grava na formatação correta.
    fclose(arquivo);
}

// --- FUNÇÕES DA ÁRVORE BINÁRIA ---

// Função: Aloca memória e cria um novo nó para a árvore de produtos.
// Recebe: um produto (variavel do tipo struct "produto")
// Retorna: Um ponteiro para o nó recém-criado, ou NULL em caso de falha de alocação. 
no *criar_no(produto p) {

    no *novo_no = (no *)malloc(sizeof(no)); // 'malloc' retorna NULL se não houver memória suficiente disponível.
    if (novo_no == NULL) {
        registrar_log("ERRO: Falha ao alocar memória para novo nó da árvore."); // Exemplo de registro no log, passando somente a mensagem de ERRO
        return NULL; // Retorna NULL em caso de erro.
    }

    novo_no->p = p; // Atribui o produto 'p' (passado como argumento) ao campo 'p' da estrutura do novo nó.
    novo_no->esquerda = NULL; // Inicializa o ponteiro da esquerda como NULL, pois ainda está sem um valor.
    novo_no->direita = NULL; // Inicializa o ponteiro da direita como NULL, pois ainda está sem um valor.

    return novo_no; // Retorna o ponteiro para o nó recém-criado e inicializado.
}

 // Função: Insere um novo produto na Árvore Binária (A inserção é baseada no código do produto e feito de forma recursiva).
 // Parametros: 'raiz' = O nó da raiz da arvore (ou subarvore). 'p' = O produto a ser inserido.
 // Retorna: O nó da raiz da arvore (ou subarvore) atualizado.
no *inserir_na_arvore(no *raiz, produto p) {
    
    if (raiz == NULL) { // No caso da árvore estar vazia, o novo nó se torna a raiz.
        return criar_no(p);
    }

    // Navegação na arvore já existente para inserção do novo produto.
    if (p.codigo < raiz->p.codigo) { // Comparação pra ver se o código inserido é MENOR que o código já existente.
        raiz->esquerda = inserir_na_arvore(raiz->esquerda, p); //Nesse caso, insere os dados do produto na ESQUERDA.
    }
    else if (p.codigo > raiz->p.codigo) { // Comparação pra ver se o código inserido é MAIOR que o código já existente.
        raiz->direita = inserir_na_arvore(raiz->direita, p); //Nesse caso, insere os dados do produto na DIREITA.
    }

    return raiz; // Se o código já existe, nada é feito, mantendo o produto original.
}

 // Função: Busca um produto na arvore pelo seu código de forma recursiva.
 // Parametros: 'raiz' = O nó ou raiz da arvore que a busca será realizada. 'codigo' = O código a ser encontrado.
 // Retorna: Um ponteiro para o nó encontrado, ou NULL se o produto não existir.
no *buscar_na_arvore(no *raiz, int codigo) {

    if (raiz == NULL || raiz->p.codigo == codigo) {// No caso da árvore estár vazia ou o produto foi encontrado na raiz atual.
        return raiz;
    }

    // Navega recursivamente para a sub-árvore apropriada.
    if (codigo < raiz->p.codigo) {
        return buscar_na_arvore(raiz->esquerda, codigo);
    }
    else {
        return buscar_na_arvore(raiz->direita, codigo);
    }
}

// Função: Salva os dados dos nós da arvore em um arquivo de forma recursiva. (Ordenados por código)
// Parametros: 'arq' = Arquivo aonde será salvo os dados. 'raiz' = O nó ou raiz da arvore a ser salvo.
void salvar_arvore_recursivo(FILE *arq, no *raiz) {

    if (raiz != NULL) { // Comparação para ver se a arvore não está vazia, ou se chegou em algum galho
        salvar_arvore_recursivo(arq, raiz->esquerda); // Recursividade para os galhos da esquerda.
        fprintf(arq, "%s;%d;%s;%.2f;%d\n", raiz->p.categoria, raiz->p.codigo, raiz->p.nome, raiz->p.preco, raiz->p.quantidade); // Salva o conteúdo do nó  atual no arquivo.
        salvar_arvore_recursivo(arq, raiz->direita); //Recursiviade para os galhos da direita.
    } 
}

// Função: Abre o arquivo "produtos.txt" e inicia o processo de salvar os dados recursivamente. utilizando a função acima.
// Parametros: 'nomeArquivo' = nome do arquivo a ser salvo (produtos.txt), 'raiz' = A raiz a arvore binária.
// !!!: Essa função é separada em duas pois a abertura do arquivo é feita separadamente, e não podemos coloca-la em uma função recursiva.
void salvar_produtos(const char *nomeArquivo, no *raiz) {

    FILE *arquivo = fopen(nomeArquivo, "w"); // Abre o arquivo no modo de escrita.
    if (arquivo == NULL) {
        registrar_log("ERRO: Falha ao abrir arquivo de produtos para escrita.");
        return;
    }
    salvar_arvore_recursivo(arquivo, raiz); //Chama a função de recursividade para salvar as infomações.
    fclose(arquivo);
}

// Função: Carrega os dados em TEXTO do arquivo para a arvore binária.
// Parametros: 'raiz' = raiz da árvore.
// Retorna: A raiz da árvore após a atualização. 
no *carregar_produtos(no *raiz) {

    FILE *arquivo = fopen(ARQPRODUTOS, "r");
    if (arquivo == NULL) {
        arquivo = fopen(ARQPRODUTOS, "w");// Se o arquivo não existe, tenta criá-lo.
        if (arquivo != NULL) {
            fclose(arquivo);
            registrar_log("Arquivo de produtos não encontrado. Um novo foi criado.");
        }
        return NULL;
    }

    char linha[MAX_TAMANHO * 2];
    produto prod;

    while (fgets(linha, sizeof(linha), arquivo)) {// Lê o arquivo linha por linha.
        linha[strcspn(linha, "\n")] = 0;// Remove o caractere de nova linha ('\n').

        if (sscanf(linha, "%[^;];%d;%[^;];%f;%d", prod.categoria, &prod.codigo, prod.nome, &prod.preco, &prod.quantidade) == 5) {//Extrai os dados da linha com sscanf caso '=5'(numero de informações).
            raiz = inserir_na_arvore(raiz, prod); //Insere os dados recém coletados na árvore.
        }
    }
    fclose(arquivo);
    registrar_log("Produtos carregados do arquivo para a árvore.");
    return raiz;
}

// Função: Liberar toda arvore binária de forma recursiva.
// Parametros: 'raiz' = Raiz da arvore binária.
void liberar_arvore(no *raiz) {
    if (raiz != NULL) {
        liberar_arvore(raiz->esquerda);
        liberar_arvore(raiz->direita);
        free(raiz);
    }
}

// --- FUNÇÕES DO SISTEMA ---

// Função: Adiciona um item novo ao carrinho, utilizando lista encadeada alocada dinamicamente.
// Parametros: 'p' = O produto a ser adicionado ao carrinho. 'quantidade' = quantidade do produto.
void carrinho_add_item(produto p, int quantidade) {

    item_carrinho *novo_item = (item_carrinho *)malloc(sizeof(item_carrinho)); // cria um ponteiro chamado 'novo_item' para apontar para este espaço de memória recém-criado.
    
    novo_item->p_vendido = p; //Transfere as informações do produto para a nova struct (item_carrinho).
    novo_item->quantidade_vendida = quantidade; //Transfere as informações de quantidade do produto para a nova struct (item_carrinho).
    novo_item->proximo = carrinho_atual; //Engata o ponteiro 'proximo' do item recém adicionado em um novo endereço de memória 
    carrinho_atual = novo_item; // Atualiza o ponteiro 'carrinho_atual' para que ele aponte para o 'novo_item', tornando ele o primeiro da lista.
}

// Função: Libera a memória da lista encadeada usada no carrinho de compras.
void limpar_carrinho() {

    item_carrinho *atual = carrinho_atual;
    item_carrinho *proximo;
    while (atual != NULL) {
        proximo = atual->proximo;
        free(atual);
        atual = proximo;
    }
    carrinho_atual = NULL;
}

// Função: Finaliza a compra, registrando a venda.
// Parametros: 'raiz' = A raiz da arvore binaria.
void finalizar_compra(no *raiz) {

    if (!caixa_aberto) {
        printf("O caixa está fechado. Abra o caixa para finalizar a compra.\n");
        Sleep(1000);
        return;
    }
    if (totalpendente <= 0 || carrinho_atual == NULL) {
        printf("Nenhum item no carrinho para finalizar.\n");
        Sleep(1000);
        return;
    }

    int opc;
    printf("Forma de pagamento:\n1 - Dinheiro\n2 - Cartão\nOpção: ");
    if (scanf("%d", &opc) != 1 || (opc != 1 && opc != 2)) {
        limpar_buffer();
        printf("Forma de pagamento inválida.\n");
        Sleep(1000);
        return;
    }
    limpar_buffer();

    FILE *arquivo = fopen(ARQ_VDETALHADAS, "a"); // Abre o arquivo de vendas detalhadas para adicionar os novos registros.
    if (arquivo == NULL) {
        perror("Erro ao registrar a venda.");
        registrar_log("ERRO: Falha ao abrir arquivo de vendas detalhadas.");
        Sleep(1000);
        return;
    }

    // Pega a data e hora atuais para registrar na venda.
    time_t tempo_atual;
    char buffer_tempo[20];
    time(&tempo_atual);
    strftime(buffer_tempo, sizeof(buffer_tempo), "%Y-%m-%d %H:%M:%S", localtime(&tempo_atual));

    item_carrinho *item = carrinho_atual; // Percorre o carrinho e salva cada item vendido no arquivo.
    while (item != NULL) {
        fprintf(arquivo, "%s;%d;%s;%d;%.2f\n", buffer_tempo, item->p_vendido.codigo, item->p_vendido.nome, item->quantidade_vendida, item->p_vendido.preco);
        item = item->proximo;
    }
    fclose(arquivo);

    // Atualiza o saldo do caixa e registra no log.
    saldo_caixa += totalpendente;
    char log_msg[100];
    sprintf(log_msg, "Venda finalizada no valor de R$ %.2f.", totalpendente);
    registrar_log(log_msg);

    // Limpa o carrinho e reseta o total pendente para a próxima compra.
    limpar_carrinho();
    totalpendente = 0;

    // Salva o estado atualizado do estoque no arquivo de produtos.
    salvar_produtos(ARQPRODUTOS, raiz);
    printf("Compra finalizada com sucesso!\n");
    system("pause");
}

/**
 * @brief Interface para o usuário adicionar um produto ao carrinho.
 * @param raiz A raiz da árvore de produtos para busca.
 * @param categoria A categoria de produtos que está sendo visualizada.
 */

// Função: Interface para o usuario adicionar um novo produto ao carrinho.
// Parametros: 'raiz' = A raiz da arvore binaria. 'categoria' = A categoria de produtos.
void carrinho(no *raiz, char *categoria) {

    if (!caixa_aberto) {
        printf("O caixa está fechado. Abra o caixa para realizar compras.\n");
        Sleep(1000);
        return;
    }

    int opc, quantidade;
    printf("--------------------------------------------------------\n");
    printf("Insira o código do produto para adicionar ao carrinho, ou insira 0 para voltar: ");
    if (scanf("%d", &opc) != 1) {
        limpar_buffer();
        printf("Opção inválida.\n");
        Sleep(1000);
        return;
    }
    if (opc == 0) {
        return;
    }
    limpar_buffer();

    no *no_encontrado = buscar_na_arvore(raiz, opc); // Busca o produto na árvore.

    if (no_encontrado != NULL && strcmp(no_encontrado->p.categoria, categoria) == 0) { // Verifica se o produto foi encontrado e se pertence à categoria correta.
        printf("Adicionando \"%s\" ao carrinho.\n", no_encontrado->p.nome);
        printf("Insira a quantidade: ");
        if (scanf("%d", &quantidade) != 1 || quantidade <= 0) { // Verifica se a quantidade é válida.
            limpar_buffer();
            printf("Quantidade inválida.\n");
            Sleep(1000);
            return;
        }

        if (quantidade > no_encontrado->p.quantidade) { // Verifica se há estoque suficiente.
            printf("Quantidade fora do estoque. Disponível: %d\n", no_encontrado->p.quantidade);
            Sleep(1000);
            return;
        }

        
        no_encontrado->p.quantidade -= quantidade; // Atualiza o estoque na árvore.
        totalpendente += quantidade * no_encontrado->p.preco; // Atualiza o total do carrinho.
        carrinho_add_item(no_encontrado->p, quantidade); // Adiciona o item à lista do carrinho.

        printf("Foram adicionados %d %s ao carrinho. Total parcial: %.2f\n", quantidade, no_encontrado->p.nome, totalpendente);
        system("pause");
    } else {
        printf("Produto não encontrado ou não pertence a categoria %s.\n", categoria);
        Sleep(1000);
    }
}

// Função: Zedra o estoque da padaria de forma recursiva.
// Parametros: 'raiz' = Raiz da arvore binaria.
void zerar_estoque_padaria_arvore(no *raiz) {
    if (raiz != NULL) {
        zerar_estoque_padaria_arvore(raiz->esquerda);
        if (strcmp(raiz->p.categoria, "PADARIA") == 0) { // Se o produto for da categoria PADARIA, zera sua quantidade.
            raiz->p.quantidade = 0;
        }
        zerar_estoque_padaria_arvore(raiz->direita);
    }
}

// Função: Gerencia a abertura e o fechamento do caixa. E zera o estoque da padaria.
// Parametros: 'raiz' A raiz da arvore binária.
void abrir_fechar_caixa(no *raiz) {
    if (!caixa_aberto) { // Se o caixa estiver FECHADO.
        // ABRIR CAIXA
        FILE *arquivo = fopen(ARQCAIXA, "r");

        if (arquivo != NULL) { // Se o arquivo existe, lê o saldo salvo.
            fscanf(arquivo, "%f", &saldo_caixa);
            fclose(arquivo);
        } else { // Se não existe, o saldo inicial é zero.
            saldo_caixa = 0;
        }
        caixa_aberto = 1; // O caixa foi aberto.
        printf("Caixa aberto com saldo de R$ %.2f\n", saldo_caixa);
        registrar_log("Caixa aberto.");
    } else {
        // FECHAR CAIXA
        FILE *arquivo = fopen(ARQCAIXA, "w");
        if (arquivo != NULL) {
            fprintf(arquivo, "%.2f\n", saldo_caixa); // Salva o saldo atual no arquivo.
            fclose(arquivo);
        }
        caixa_aberto = 0; // O caixa foi fechado.
        printf("Caixa fechado. Saldo salvo em %s\n", ARQCAIXA);
        registrar_log("Caixa fechado.");
    }

    // Requerimento do programa: ao fechar o caixa, o estoque da padaria é zerado.
    zerar_estoque_padaria_arvore(raiz);
    salvar_produtos(ARQPRODUTOS, raiz);
    printf("Estoque da padaria foi zerado.\n");
    registrar_log("Estoque da padaria zerado.");
    system("pause");
}

// Função: Interface para adicionar um novo produto ao sistema.
// Parametros: 'p_raiz' = Um ponteiro para o ponteiro da raiz da árvore, para que a raiz possa ser modificada.
void adicionar_produto(no **p_raiz) {
    produto novo;
    int opc;

    system("cls");
    printf("Escolha a categoria do novo produto:\n1 - Limpeza\n2 - Alimentos\n3 - Padaria\nOpção: ");
    if (scanf("%d", &opc) != 1) {
        limpar_buffer();
        printf("Opção inválida.\n");
        Sleep(1000);
        return;
    }

    switch (opc) {
    case 1:
        strcpy(novo.categoria, "LIMPEZA");
        break;
    case 2:
        strcpy(novo.categoria, "ALIMENTOS");
        break;
    case 3:
        strcpy(novo.categoria, "PADARIA");
        break;
    default:
        printf("Categoria inválida.\n");
        Sleep(1000);
        return;
    }
    limpar_buffer();

    printf("Insira o código do produto: ");
    if (scanf("%d", &novo.codigo) != 1) {
        limpar_buffer();
        printf("Código inválido.\n");
        Sleep(1000);
        return;
    }
    limpar_buffer();

    if (buscar_na_arvore(*p_raiz, novo.codigo) != NULL) { // Verifica se o código já está em uso para evitar duplicatas.
        printf("Erro: Código de produto já existente.\n");
        Sleep(1000);
        return;
    }

    printf("Insira o nome do produto: "); //Coleta do NOME
    fgets(novo.nome, sizeof(novo.nome), stdin);
    novo.nome[strcspn(novo.nome, "\n")] = 0; // Remove o '\n'

    printf("Insira o preço do produto: "); // Coleta do PREÇO   
    if (scanf("%f", &novo.preco) != 1 || novo.preco <= 0) {
        limpar_buffer();
        printf("Preço inválido.\n");
        Sleep(1000);
        return;
    }
    limpar_buffer();

    printf("Insira a quantidade do produto: "); //Coleta da QUANTIDADE
    if (scanf("%d", &novo.quantidade) != 1 || novo.quantidade < 0) {
        limpar_buffer();
        printf("Quantidade inválida.\n");
        Sleep(1000);
        return;
    }
    limpar_buffer();

    // Insere o novo produto na árvore e salva no arquivo.
    *p_raiz = inserir_na_arvore(*p_raiz, novo);
    salvar_produtos(ARQPRODUTOS, *p_raiz);

    system("cls");
    printf("Produto adicionado com sucesso.\n");
    char log_msg[150];
    sprintf(log_msg, "Produto adicionado: Cod %d, Nome %s, Categoria %s", novo.codigo, novo.nome, novo.categoria); //Função sprintf serve para montar strings.
    registrar_log(log_msg);
    printf("Categoria: %s\nCódigo: %d\nNome: %s\nPreço: %.2f\nQuantidade: %d\n", novo.categoria, novo.codigo, novo.nome, novo.preco, novo.quantidade);
    system("pause");
}

// Função: Exibe os produtos na tela (Ordenação por código).
// Parametros: 'raiz' = raiz da arvore binaria. 'categoria' = categoria a ser exibida.
void exibir_produtos_da_arvore(no *raiz, const char *categoria) {
    if (raiz != NULL) {
        exibir_produtos_da_arvore(raiz->esquerda, categoria);
        if (strcmp(raiz->p.categoria, categoria) == 0) { // Exibe o produto apenas se a categoria corresponder.
            printf("%-5d %-30s R$%-9.2f %-10d\n", raiz->p.codigo, raiz->p.nome, raiz->p.preco, raiz->p.quantidade);
        }
        exibir_produtos_da_arvore(raiz->direita, categoria);
    }
}

// Função: Monta a interface de exibição de produtos.
// Parametros: 'raiz' = A raiz da arvore binaria. 'categoria' = categoria a ser exibida.
void exibir_produtos(no *raiz, char *categoria) {
    system("cls");
    printf("CATEGORIA: %s.\n", categoria);
    printf("%-5s %-30s %-10s %-10s\n", "ID", "NOME", "PREÇO", "QUANTIDADE");
    printf("---------------------------------------------------------------\n");
    exibir_produtos_da_arvore(raiz, categoria);
    
    carrinho(raiz, categoria);// Após exibir, chama a função do carrinho para permitir a compra.
}

// Função: Permite a atualização manual do estoque da padaria de forma recursiva.
// Parametros: 'raiz' = A raiz da árvore binaria.
void atualizar_estoque_padaria_recursivo(no *raiz) {
    if (raiz != NULL) {
        atualizar_estoque_padaria_recursivo(raiz->esquerda);
        
        if (strcmp(raiz->p.categoria, "PADARIA") == 0) { // Se o produto for da categoria PADARIA, pede a nova quantidade.
            int nova_quantidade;
            printf("Produto: %s | Qtd atual: %d | Nova quantidade: ", raiz->p.nome, raiz->p.quantidade);
            if (scanf("%d", &nova_quantidade) != 1 || nova_quantidade < 0) { // Verifica se a quantidade é válida
                printf("Inválido. Pulando.\n");
                limpar_buffer();
            } else {
                raiz->p.quantidade = nova_quantidade; // Atualiza o estoque atual do produto.
                limpar_buffer();
            }
        }
        atualizar_estoque_padaria_recursivo(raiz->direita);
    }
}

// Função: Interface principal para atualizar o estoque da padaria.
// Parametros: 'raiz' = A raiz da arvore binaria.
void atualizar_estoque_padaria(no *raiz) {
    if (!caixa_aberto) {
        printf("O caixa está fechado. Abra o caixa para atualizar o estoque.\n");
        Sleep(1000);
        return;
    }
    printf("Atualizando o estoque da padaria...\n");
    atualizar_estoque_padaria_recursivo(raiz);
    salvar_produtos(ARQPRODUTOS, raiz);
    printf("Estoque da padaria atualizado com sucesso.\n");
    registrar_log("Estoque da padaria atualizado manualmente.");
    system("pause");
}

// --- FUNÇÕES DE RELATÓRIO ---
/* Para ordenar as informações da nossa arvore binária de outras maneiras (alfabeticamente) no nosso caso, como a arvore foi ordenada pelo código e não pelo nome, 
A ideia dessa ordenação é: criar um array temporario e comparamos ele com a função 'qsort' (QICKSORT). */

// Função: Comparar e preparar os produtos para o utilizar a função qsort, para ordenar os PRODUTOS por nome.
// Parametros: 'a' = ponteiro para o produto 1. 'b' = ponteiro para o produto 2.
// Retorna: NEGATIVO se 'pA' < 'pB' (em ordem alfabética), retorna NULO se forem iguais, e retorna POSITIVO se 'pA' > 'pB'. 
int cmp_prod_por_nome(const void *a, const void *b) {

    produto *pA = (produto *)a; // Recebe um ponteiro de void, e anexa ele a 'pA' para ser possivel a comparação.
    produto *pB = (produto *)b; // Recebe um ponteiro de void, e anexa ele a 'pB' para ser possivel a comparação.
    return strcmp(pA->nome, pB->nome); // strcmp faz a comparação dos dois ponteiros apontando para o nome.
}

// Função: Comparar e preparar os produtos para o utilizar a função qsort, para ordenar os VENDAS por nome.
// Parametros: 'a' = ponteiro para o produto 1. 'b' = ponteiro para o produto 2.
// Retorna: NEGATIVO se 'vA' < 'vB' (em ordem alfabética), retorna NULO se forem iguais, e retorna POSITIVO se 'vA' > 'vB'. 
int comparar_vendas_por_nome(const void *a, const void *b) {

    venda_detalhada *vA = (venda_detalhada *)a;
    venda_detalhada *vB = (venda_detalhada *)b;
    return strcmp(vA->nome, vB->nome); // strcmp faz a comparação das duas strings apontandas.
}

// Função: Comparar e preparar os produtos para o utilizar a função qsort, para ordenar os VENDAS por código.
// Parametros: 'a' = ponteiro para o produto 1. 'b' = ponteiro para o produto 2.
// Retorna: NEGATIVO se 'vA' < 'vB' (em ordem alfabética), retorna NULO se forem iguais, e retorna POSITIVO se 'vA' > 'vB'. 
int comparar_vendas_por_codigo(const void *a, const void *b) {

    venda_detalhada *vA = (venda_detalhada *)a;
    venda_detalhada *vB = (venda_detalhada *)b;
    return vA->codigo - vB->codigo; // faz a comparação (aritimetica) dos dois codigos apontandos.
}

// Função: Copia todos os produtos da arvore para um array para ordenar por nome (de forma recursiva).
// Parametros: 'raiz' = raiz da arvore binária. 'array' = array que recebera os produtos. 'contador' = contador de quantidade de produtos do array.
void arvore_para_array(no *raiz, produto **array, int *contador) {

    if (raiz == NULL) {
        return;
    }
    arvore_para_array(raiz->esquerda, array, contador);
    (*array)[*contador] = raiz->p; //Insere o valor de 'raiz->p' dentro do '(*array)' na posição '[contador]'
    (*contador)++; // Incrementa o contador de quantidade de produtos.
    arvore_para_array(raiz->direita, array, contador);
}

// Função: Exibe todos os produtos da arvore binaria, ordenada por código (não usa a qsort).
void exibir_produtos_ordenados(no *raiz) {

    if (raiz != NULL) {
        exibir_produtos_ordenados(raiz->esquerda);
        printf("%-5d %-30s R$%-9.2f %-10d %-15s\n", raiz->p.codigo, raiz->p.nome, raiz->p.preco, raiz->p.quantidade, raiz->p.categoria);
        exibir_produtos_ordenados(raiz->direita);
    }
}

// Função: Interface para exibição dos produtos ordenados.
// Parametros: 'raiz' = A raiz da arvore binaria. 'por_nome' = 1 para ordenar por nome, 0 para ordenar por código.
void relatorio_produtos_por_ordem(no *raiz, int por_nome) {
    system("cls");
    if (por_nome) {
        printf("--- Relatório de Produtos (Ordem Alfabética) ---\n");
    } else {
        printf("--- Relatório de Produtos (Ordem de Código) ---\n");
    }
    printf("%-5s %-30s %-10s %-10s %-15s\n", "ID", "NOME", "PREÇO", "QTD", "CATEGORIA");
    printf("----------------------------------------------------------------------\n");

    if (por_nome) {

        produto *produtos_array = (produto *)malloc(sizeof(produto) * 1000); // Cria um array alocado dinamicamente do tipo 'produto' para utilização do qsort
        int contador = 0;

        arvore_para_array(raiz, &produtos_array, &contador);
        qsort(produtos_array, contador, sizeof(produto), cmp_prod_por_nome);

        for (int i = 0; i < contador; i++) {
            printf("%-5d %-30s R$%-9.2f %-10d %-15s\n", produtos_array[i].codigo, produtos_array[i].nome, produtos_array[i].preco, produtos_array[i].quantidade, produtos_array[i].categoria);
        }
        free(produtos_array);
    } else {
        exibir_produtos_ordenados(raiz);// Para ordenar por código, basta percorrer a árvore em ordem.
    }
    printf("----------------------------------------------------------------------\n");
    system("pause");
}

// Função: Interface para exibição de vendas ordenadas.
// Parametros: 'por_nome' = 1 para ordenar por nome do produto, 0 para ordenar por código.
// INFORMAÇÕES: As informações exibidas pelo relatorio de vendas são mais detalhadas, indicando data/hora.
void relatorio_vendas(int por_nome) {

    system("cls");
    if (por_nome) {
        printf("--- Relatório Detalhado de Vendas (Ordenado por Nome do Produto) ---\n");
    } else {
        printf("--- Relatório Detalhado de Vendas (Ordenado por Código do Produto) ---\n");
    }

    // Imprime o cabeçalho da tabela do relatório com alinhamento e espaçamento definidos.
    printf("%-20s %-5s %-25s %-5s %-10s %-10s\n", "DATA/HORA", "ID", "NOME", "QTD", "PREÇO UN.", "SUBTOTAL");
    printf("====================================================================================\n");

    FILE *arq = fopen(ARQ_VDETALHADAS, "r"); // Tenta abrir o arquivo de vendas detalhadas ("vendas_detalhadas.txt") em modo de leitura ("r")
    if (arq == NULL) { 
        printf("Nenhuma venda registrada ainda.\n"); // 
        printf("====================================================================================\n");
        system("pause");
        return;
    }

    // Declaração das variáveis de controle. (fundamental para o uso do quicksort)
    venda_detalhada *vendas = NULL;         // Cria um ponteiro para o array dinâmico, começa como nulo. 
    int capacidade = 100;                   // Capacidade atual alocada para o array 'vendas'. Limitada em 100.
    int n_vendas = 0;                       // Número de vendas efetivamente lidas do arquivo. 
    char linha[256];                        // Buffer para armazenar cada linha lida do arquivo. 
    float total_geral_vendas = 0.0;         // Acumulador para o valor total das vendas. 

    while (fgets(linha, sizeof(linha), arq)) { // lê o arquivo de vendas linha por linha até o final.

        // Extrai as informações do arquivo 'vendas_detalhadas' e as coloca na pozição livre do array
        sscanf(linha, "%19[^;];%d;%[^;];%d;%f", vendas[n_vendas].data_hora, &vendas[n_vendas].codigo, vendas[n_vendas].nome, &vendas[n_vendas].quantidade, &vendas[n_vendas].preco_unitario); 
        n_vendas++; // Incrementa o contador de vendas lidas.
    }
    fclose(arq); // Fecha o arquivo após terminar a leitura.

    if (n_vendas > 0) { // Verifica se alguma venda foi carregada para o array.
        if (por_nome) {
            /* -------------------- Explicação da função 'qsort' -------------------------------*/
            qsort(                         // FORMATAÇÃO : void qsort(void *base, size_t num, size_t size, int (*compar)(const void *, const void *));
                vendas,                    // 'void *base' = Posição inicial do array que vai ser ordenado.
                n_vendas,                  // 'size_t num' = Numero de elementos do array.
                sizeof(venda_detalhada),   // 'size_t size' = Tamanho de cada elemento do array.
                comparar_vendas_por_nome); // 'int (*compar)(const void *, const void *)' = fução que retorna formato de comparação dos elementos (-1, 0 ou 1).

        } else {
            qsort(vendas, n_vendas, sizeof(venda_detalhada), comparar_vendas_por_codigo); // Se for por código, usa a outra função de comparação.
        }

        for (int i = 0; i < n_vendas; i++) { // Loop para exibir os dados já ordenados.

            float subtotal = vendas[i].quantidade * vendas[i].preco_unitario; // Calcula o subtotal para cada item de venda.
            // Imprime uma linha formatada da tabela com os dados da venda.
            printf("%-20s %-5d %-25s %-5d R$%-9.2f R$%-9.2f\n", vendas[i].data_hora, vendas[i].codigo, vendas[i].nome, vendas[i].quantidade, vendas[i].preco_unitario, subtotal);
            total_geral_vendas += subtotal; // Adiciona o subtotal ao total geral.
        }
        free(vendas); // Libera a memória alocada
    } else { // Caso não tenha nenhuma venda lida.
        printf("Nenhuma venda registrada ainda.\n");
    }

    printf("====================================================================================\n");
    printf("TOTAL GERAL DAS VENDAS: R$ %.2f\n", total_geral_vendas); // 
    printf("====================================================================================\n");
    system("pause");
}

// Função: Exibe a interface de relatorios.
// Parametros: 'raiz' = raiz da arvore binaria.
void menu_relatorios(no *raiz) {
    int opc;
    do {
        system("cls");
        printf("--- MENU DE RELATÓRIOS ---\n");
        printf("=========================================\n");
        printf("Listagem de Produtos:\n");
        printf("  1 - Produtos (ordenado por nome)\n");
        printf("  2 - Produtos (ordenado por código)\n");
        printf("-----------------------------------------\n");
        printf("Listagem de Vendas:\n");
        printf("  3 - Vendas Detalhadas (ordenado por nome do produto)\n");
        printf("  4 - Vendas Detalhadas (ordenado por código do produto)\n");
        printf("=========================================\n");
        printf("  0 - Voltar ao menu principal\n");
        printf("=========================================\n");
        printf("Selecione uma opção: ");

        if (scanf("%d", &opc) != 1) {
            limpar_buffer();
            return;
        }
        limpar_buffer();

        switch (opc) {
        case 0: // volta para o menu principal.
            break; 
        case 1:
            relatorio_produtos_por_ordem(raiz, 1); // Exibe o relatorio de produtos, passando 1 como valor para exibir ordenado por nome
            break;
        case 2:
            relatorio_produtos_por_ordem(raiz, 0);  // Exibe o relatorio de produtos, passando 0 como valor para exibir ordenado por codigo
            break;
        case 3:
            relatorio_vendas(1);  // Exibe o relatorio de produtos, passando 1 como valor para exibir ordenado por nome
            break;
        case 4:
            relatorio_vendas(0);  // Exibe o relatorio de produtos, passando 0 como valor para exibir ordenado por codigo
            break;
        default:
            printf("Opção inválida.\n");
            Sleep(1000);
            break;
        }
    } while (opc != 0);
}

// --- FUNÇÃO PRINCIPAL ---

// Função: função main do programa, ponto de entrada do usuario
int main() {
    
    SetConsoleOutputCP(65001); // Define o code page do console para UTF-8 para exibir acentos corretamente.
    no *raiz = NULL;
    int opc;

    registrar_log("-----------------------------------------");
    registrar_log("Programa iniciado.");
    
    raiz = carregar_produtos(raiz); // Carrega os dados dos produtos do arquivo para a árvore em memória.

    
    do {// Loop principal do menu.
        system("cls");
        printf("Mercadinho\n");
        printf("===========================================================\n");
        printf("COMPRAR:                                   Total no carrinho: R$ %.2f\n", totalpendente);
        printf("  1 - Limpeza\n  2 - Alimentos\n  3 - Padaria\n");
        printf("===========================================================\n");
        printf("CAIXA:                                     Status: %s\n", caixa_aberto ? "ABERTO" : "FECHADO");
        printf("  4 - Finalizar compra\n  5 - Abrir/Fechar caixa\n");
        printf("===========================================================\n");
        printf("ADM:\n  6 - Adicionar produto\n  7 - Atualizar estoque da Padaria\n  8 - Relatórios\n  9 - Encerrar programa\n");
        printf("===========================================================\n");
        printf("Selecione uma opção: ");

       
        if (scanf("%d", &opc) != 1) { // Validação da entrada do usuário.
            limpar_buffer();
            printf("Opção inválida.\n");
            Sleep(1000);
            continue; // Volta para o início do loop.
        }

        switch (opc) {
        case 1:
            exibir_produtos(raiz, "LIMPEZA");
            break;
        case 2:
            exibir_produtos(raiz, "ALIMENTOS");
            break;
        case 3:
            exibir_produtos(raiz, "PADARIA");
            break;
        case 4:
            finalizar_compra(raiz);
            break;
        case 5:
            abrir_fechar_caixa(raiz);
            break;
        case 6:
            adicionar_produto(&raiz);
            break;
        case 7:
            atualizar_estoque_padaria(raiz);
            break;
        case 8:
            menu_relatorios(raiz);
            break;
        case 9:
            printf("Encerrando programa.\n");
            break;
        default:
            limpar_buffer();
            printf("Opção inválida.\n");
            Sleep(1000);
            break;
        }
    } while (opc != 9);

    // --- Finalização do Programa ---
    // Salva o estado final do estoque.
    salvar_produtos(ARQPRODUTOS, raiz);
    // Libera toda a memória alocada dinamicamente.
    liberar_arvore(raiz);
    limpar_carrinho();
    raiz = NULL;

    registrar_log("Programa encerrado.");
    registrar_log("-----------------------------------------\n");

    return 0;
}