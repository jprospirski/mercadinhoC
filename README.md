# Sistema de Gestão de Supermercado em C

> Aplicação de linha de comando (CLI) desenvolvida em C para simular um sistema de ponto de venda (PDV) e gestão de estoque de um pequeno mercado.

![Demonstração do Sistema](demo.gif)

## 📖 Sobre o Projeto

Este projeto foi desenvolvido como um trabalho acadêmico para a disciplina de Algoritmos e Estruturas de Dados. O objetivo era aplicar conceitos fundamentais da Ciência da Computação para criar um sistema funcional e robusto, com foco em gerenciamento de memória, manipulação de dados e persistência de informações em arquivos.

O sistema gerencia um catálogo de produtos, controla o estoque, processa vendas em um carrinho de compras dinâmico e gera relatórios detalhados, tudo através de uma interface interativa no terminal.

## ✨ Funcionalidades Principais

* **Gestão de Produtos:** Adição de novos produtos e carregamento do estoque inicial a partir de um arquivo (`produtos.txt`). Os produtos são organizados em uma **Árvore Binária de Busca** para performance otimizada em buscas.
* **Carrinho de Compras:** Implementação de um carrinho de compras dinâmico usando uma **Lista Encadeada Simples**, permitindo adicionar múltiplos produtos antes de finalizar a compra.
* **Sistema de Caixa:** Funcionalidades para abrir e fechar o caixa, com o saldo sendo persistido entre as sessões (`caixa.txt`).
* **Regras de Negócio Específicas:** Implementação de regras como o zeramento automático do estoque da padaria ao fechar o caixa.
* **Relatórios Avançados:** Geração de relatórios de produtos e de vendas detalhadas, com opções de ordenação por nome ou por código, utilizando a função `qsort` da biblioteca padrão do C.
* **Persistência de Dados:** Todas as transações, produtos e eventos importantes são salvos em arquivos de texto (`vendas_detalhadas.txt`, `log.txt`), garantindo que os dados não sejam perdidos.

## 🛠️ Tecnologias e Conceitos Aplicados

* **Linguagem:** C (Puro)
* **Estruturas de Dados:** Árvore Binária de Busca, Lista Encadeada Simples.
* **Algoritmos:** `qsort` (Quicksort) para ordenação de relatórios, Algoritmos de busca e inserção recursiva em árvores.
* **Gerenciamento de Memória:** Alocação dinâmica com `malloc` e liberação com `free`.
* **Manipulação de Arquivos:** Leitura e escrita em arquivos (`fopen`, `fprintf`, `fscanf`, `fgets`).

## 🚀 Como Executar o Projeto

É necessário ter um compilador C (como o GCC/MinGW no Windows) instalado.

```bash
# 1. Clone o repositório
git clone [https://github.com/jprospirski/nome-do-seu-repositorio.git](https://github.com/jprospirski/nome-do-seu-repositorio.git)

# 2. Navegue até o diretório do projeto
cd nome-do-seu-repositorio

# 3. Compile o código-fonte
# O nome do seu arquivo principal é bereniceGPS-2025.01.c
# Vamos compilar para um executável chamado "mercado.exe"
gcc bereniceGPS-2025.01.c -o mercado.exe -Wno-deprecated-declarations

# 4. Execute o programa
./mercado.exe
