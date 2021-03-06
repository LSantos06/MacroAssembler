#include "montador.h"

char *tokens_linha[10];

/*
 *  seleciona_operacao()
 *
 *  Funcao responsavel pela execucao de uma operacao de acordo
 * com o segundo argumento
 *
 *  Erros: Terminal (arquivo a ser ligado nao contem extensao '.o'!)
 *         Terminal (arquivo a ser pre-processado nao contem extensao '.asm'!)
 *         Terminal (arquivo a ser montado nao contem extensao '.pre' ou '.asm'!)
 */
void seleciona_operacao(int argc, char* argv[]){
	Operacoes_t operacao;
	operacao = validacao_argumentos(argc, argv);
	int erro_passagem_1 = 0;

	// Se a operacao eh ligacao -l
	if(operacao == LIGACAO){
		// Variaveis que checam as extensoes
		char *validade_entrada_o = ".o";
		int contador = 2;

		while(contador < (argc-1)){
			// Se o arquivo de entrada nao contem a extensao valida, ERROR -4
			if((strstr(argv[contador], validade_entrada_o))==NULL){
				printf("Erro Terminal: %d Arquivo a ser ligado nao contem extensao '.o'!\n", contador-1);
				exit(-4);
			}
			contador++;
		}

		// Ligacao
		ligador(contador-2, argc, argv);

	} // ligacao

	// Se a operacao eh -p ou -o
	else{
		// Variaveis que checam as extensoes
		char *validade_entrada_pre = ".pre";
		char *validade_entrada_asm = ".asm";
		char *nome_arq;

		//Cria as tabelas de simbolos e de definicoes vazias
		inicializa_tabelas();

		// Se a operacao eh pre processamento -p
		if(operacao == PRE_PROC){
			// Se o arquivo de entrada nao contem a extensao valida, ERROR -4
			if((strstr(argv[2], validade_entrada_asm))==NULL){
				printf("Erro Terminal: Arquivo a ser pre-processado nao contem extensao '.asm'!\n");
				exit(-4);
			}

			// Abertura do arquivo para leitura
			FILE* fp = fopen(argv[2], "r");
			// Se o arquivo nao conseguiu ser aberto, ERROR -5
			if(fp == NULL){
				printf("Erro Terminal: Erro na abertura do arquivo!\n");
				exit(-5);
			}

			// Pre processamento
			pre_processamento(fp, argv[3]);

		} // preproc

		// Se a operacao eh montagem -o
		if(operacao == MONTAGEM){
			// Se o arquivo de entrada nao contem as extensoes validas, ERROR -4
			if((strstr(argv[2], validade_entrada_pre)==NULL && strstr(argv[2], validade_entrada_asm)==NULL)){
				printf("Erro Terminal: Arquivo a ser montado nao contem extensao '.pre' ou '.asm'!\n");
				exit(-4);
			}

			// Abertura do arquivo para leitura
			FILE* fp = fopen(argv[2], "r");
			// Se o arquivo nao conseguiu ser aberto, ERROR -5
			if(fp == NULL){
				printf("Erro Terminal: Erro na abertura do arquivo!\n");
				exit(-5);
			}
			//Copia nome do arquivo (pre_processamento altera argv[3])
			nome_arq = strdup(argv[3]);
			// Se a extensao for .asm
			if(strstr(argv[2], validade_entrada_pre)==NULL){
				// Pre-processamento seguido da passagem 1
				erro_passagem_1 = !(passagem1(pre_processamento(fp, argv[3])));
			}
			// Se for .pre
			else if (strstr(argv[2], validade_entrada_asm)==NULL){
				// Passagem 1
				erro_passagem_1 = !(passagem1(fp));
			}
			//Passa arquivo (ja com rewind) e nome do arquivo que tera extensao .o
			passagem2(fp, nome_arq, erro_passagem_1);

		} // montagem
	} // preproc ou montagem
}

/*
 *  scanner()
 *
 *  Verifica erros lexicos no programa
 *
 *  Erros: Lexico (token '%s' inicia com digito)
 *         (excedeu numero de tokens)
 */
int scanner(char *linha, int contador_linha, char *delimitador, int passagem){
	if(linha == NULL){
		return 0;
	}

	char *token;
	int erro = 0, i = 0;

	//Espacao como caractere limitador
	token = strtok(linha, delimitador);

	while(token!=NULL){
		tokens_linha[i] = remove_espacos(strdup(token));
		//Se token comeca com numero e n vem depois de uma diretiva SPACE ou CONST: ERROR -6
		if(i>0 && token[0]>='0' && token[0]<='9' && (strcmp(tokens_linha[i-1], "SPACE") && strcmp(tokens_linha[i-1], "CONST")) ){
			if(passagem == 1){
				printf("Erro lexico (linha %d): token '%s' inicia com digito\n", contador_linha, token);
			}
			erro = 1;
		}
		//Mesma condicao da anterior, mas sem acessar fora dos limites do vetor
		else if(i==0 && token[0]>='0' && token[0]<='9'){
			if(passagem == 1){
				printf("Erro lexico (linha %d): token '%s' inicia com digito\n", contador_linha, token);
			}
			erro = 1;
		}
		if(i>8){
			if(passagem == 1){
				printf("\nExcedeu numero de tokens! (linha %d)\n", contador_linha);
			}
			erro = 1;
			break;
		}
		// printf("<%s>\n", token);
		i++;
		//Avanca pro proximo token
		token = strtok(NULL, delimitador);
	}
	tokens_linha[i] = "\0";

	if(erro == 1){
		return 0;
	}
	return 1;

}

/*
 *  pre_processamento()
 *
 *  Funcao responsavel pelo pre processamento de um arquivo .asm de entrada
 * avaliando as diretivas:
 *   - EQU, 1 operando, sempre no inicio, cria um sinonimo textual para um simbolo;
 *   - IF, 1 operando, inclui a linha seguinte somente se o operando == 1.
 *
 *  Erros: Terminal (erro na abertura do arquivo!)
 *         Sintatico (operando do EQU possui tipo invalido!)
 *         Semantico (label '%s' ja definida!)
 *
 *  Warning: (EQU não está no início do código)
 */
FILE* pre_processamento(FILE *entrada, char *nome_arquivo_pre){
  // while !EOF{
  // verificar se o inicio da linha eh um label e se a proxima palavra eh um EQU
  //  se for EQU, associar o label ao operando
  // verificar se o inicio da linha eh um IF
  //  se o valor do operando eh 1
  //   deixar a proxima linha
  //  se o valor do operando eh 0
  //   desconsiderar a proxima linha
  // }
  // rewind arquivo .asm
  // retornar novo arquivo .pre

	printf("\n:::::::::::::PRE PROCESSAMENTO\n");

  // Variaveis para verificacao das linhas
  char linha[TLINHA];
  char escrita[TLINHA];
	char aux_escrita_diretiva[TLINHA];
	char *aux_escrita;
	char *aux_escrita2;
  char *instrucao;
  char *token;

  // Variaveis para armazenar a lista de EQUs
  lista_t *lista_equs = (lista_t *) malloc(sizeof(lista_t));
  inicializa_lista(lista_equs);
  char *label;
  lista_t *resultado_busca;

	// Variaveis para criacao do arquivo pre processado
	FILE *pre;
	char *arquivo_saida;

	// Elaborando o nome do arquivo .pre
	arquivo_saida = strcat(nome_arquivo_pre, ".pre");
	//printf("%s\n", arquivo_saida);

	// Abertura do arquivo para escrita
  pre = fopen(arquivo_saida, "w");
	// Se o arquivo nao conseguiu ser aberto, ERROR -4
  if(pre == NULL){
    printf("Erro Terminal: Erro na abertura do arquivo!\n");
    exit(-4);
 	}

  // Variaveis para escrita das linhas e erros
	int linha_anterior_if = 0;
	int linha_atual_if = 0;
	int escreve = 0;
	int contador_linha = 0;
	int flag_equ = 0;

	// Variavel para verificacao de erros
	int flag_erro = 0;

  while(!feof(entrada)){
    // Funcao fgets() lê até TLINHA caracteres ou até o '\n'
    instrucao = fgets(linha, TLINHA, entrada);
		contador_linha++;

		// fgets() retorna NULL, fim do pre processamento
		if(instrucao == NULL)
			break;

    // Se a linha nao eh nula
    if(instrucao[0] != '\n' && instrucao[0] != ';'){

			// Retirando os comentarios
			instrucao = strtok(instrucao, ";");

			// char c = instrucao[strlen(instrucao)-1];
			// printf("%c %d\n", c, c);

			// Formatando o arquivo .pre de acordo com a tabela ASCII
			if(instrucao[strlen(instrucao)-1] < 48
			|| instrucao[strlen(instrucao)-1] > 122){
				 instrucao[strlen(instrucao)-1] = '\n';
			}

			// Armazena a linha para escrita futura
			strcpy(escrita, instrucao);

      // 1 token, que pode ser um "IF" ou qualquer outra coisa
      // Funcao strtok() corta uma string em tokens a partir de um separador
      token = strtok(instrucao, " 	");

			// Primeiro Token
			// Remove os espacos do token
			token = remove_espacos(token);
			// Passando o token para caixa alta, para fins de comparacao
			string_alta(token);

      //// Se o 1 token eh "IF"
      if(!strcmp(token,"IF")){
        //printf("IF\n");

				// Variavel para avaliacao da escrita da linha apos o IF
				linha_anterior_if = linha_atual_if;
				linha_atual_if = 1;

        // Pega o 2 token para decidir se a linha continua ou eh desconsiderada
        if(token!=NULL){
          token = strtok(NULL, "	 ");
          //printf("IF %s\n", token);

					// Segundo token
					// Remove os espacos do token
					token = remove_espacos(token);
					// Passando o token para caixa alta, para fins de comparacao
					string_alta(token);

					//exibe_lista(lista_equs);

          // Busca o argumento do IF na lista de EQUs
          resultado_busca = busca_elemento(lista_equs, token);

					// Se o argumento do IF eh definido por EQU
					if(resultado_busca != NULL){
	          // IF 1, proxima linha eh escrita
	          if(!strcmp(resultado_busca->valor, "1")){
	            printf("IF: escreve prox\n");
							escreve = 1;
	          }
	          // IF != 1, proxima linha nao eh escrita
	          else{
							printf("IF: nao escreve prox\n");
							escreve = 0;
	          }
					}
					// Operando eh 1
					else if(!strcmp(token,"1")){
						printf("IF: escreve prox\n");
						escreve = 1;
					}
					// Operando eh 0
					else{
						printf("IF: nao escreve prox\n");
						escreve = 0;
					}
        } // 2 token
      } // if

			// Se o 1 token eh "STOP"
			else if(!strcmp(token,"STOP")){
        //printf("STOP\n");

				// Variavel para avaliacao da escrita da linha apos o IF
				linha_anterior_if = linha_atual_if;
				linha_atual_if = 0;

        printf("escreve atual (depende se tem IF antes)\n");

				// Depende do IF para saber se a linha sera escrita
				if(linha_anterior_if == 1){
					// So escreve quando a avaliacao do if for 1
					if(escreve == 1){
						fputs(escrita, pre);
					}
				}
				// Linha que nao possui diretivas e independe do IF
				else{
					fputs(escrita, pre);
				}
			} // stop

			// Se o 1 token eh "END"
			else if(!strcmp(token,"END")){
				//printf("END\n");

				// Variavel para avaliacao da escrita da linha apos o IF
				linha_anterior_if = linha_atual_if;
				linha_atual_if = 0;

        printf("escreve atual (depende se tem IF antes)\n");

				// Depende do IF para saber se a linha sera escrita
				if(linha_anterior_if == 1){
					// So escreve quando a avaliacao do if for 1
					if(escreve == 1){
						fputs(escrita, pre);
					}
				}
				// Linha que nao possui diretivas e independe do IF
				else{
					fputs(escrita, pre);
				}
			} // end

      //// Senao eh "IF", nem "STOP", nem "END"
      else{
        //printf("OUTRO\n");

				// Variavel para avaliacao da escrita da linha apos o IF
				linha_anterior_if = linha_atual_if;
				linha_atual_if = 0;

        // Armazena o label para guardar na lista de Equs
        label = token;
				// Retira o : do label para insercao na lista
				label[strlen(label)-1] = '\n';

        // Pega o 2 token para ver se eh um EQU ou alguma label que esta na lista de EQUS
        if(token!=NULL){
          token = strtok(NULL, "	 ");
          //printf("OUTRO %s", token);

					// Segundo token
					// Remove os espacos do token
					token = remove_espacos(token);
					// Passando o token para caixa alta, para fins de comparacao
					string_alta(token);

          // LABEL: EQU, Eh um EQU, linha nao eh escrita no .pre
					if(!strcmp(token,"EQU")){
						printf("nao escreve atual\n");

						// Se as EQUs nao estao no comeco do codigo
						if(flag_equ == 1){
							printf("Warning (Linha %d): EQU não está no início do código\n", contador_linha);
						}

            // Pega o 3 token, que eh o operando de EQU
            if(token!=NULL){
              token = strtok(NULL, "	 ");

							// Se o operando do EQU nao for um numero
							if(*token < 48 ||
							   *token > 57){
								 printf("Erro Sintático (Linha %d): operando do EQU possui tipo inválido!\n\n", contador_linha);
								 flag_erro = 1;
							}

              // Insere na lista de equs
							if(busca_elemento(lista_equs, label) == NULL){
								insere_elemento(lista_equs, label, token);
								// exibe_lista(lista_equs);
							}
							else{
								label[strlen(label)-1] = '\0';
								printf("Erro Semântico (Linha %d): Label %s ja definida!\n\n", contador_linha, label);
								flag_erro = 1;
							}
            } // 3 token
          } // token == EQU

          // INSTRUCAO OP || LABEL: DIRETIVA OP, linha eh escrita, dependendo se tem IF antes
          else{
						flag_equ = 1;

						// Pega o 3 token, se ele nao for nulo, eh uma diretiva
						if(token!=NULL){
							token = strtok(NULL, "	 ");

							// LABEL: DIRETIVA OP, diretivas podem conter operandos com elemento terminal
							if(token!=NULL){

								// Terceiro token
								// Remove os espacos do token
								token = remove_espacos(token);
								// Passando o token para caixa alta, para fins de comparacao
								string_alta(token);

								// Buscando o token na lista de EQUS
								resultado_busca = busca_elemento(lista_equs, token);

								// Operando esta na lista de EQUS
								if(resultado_busca != NULL){
									// Separa o id associado da escrita
									aux_escrita = strtok(escrita, " 	");
									aux_escrita2 = strtok(NULL, " 	");
									strcpy(aux_escrita_diretiva, aux_escrita2);

									// Acrescenta o valor associado na escrita
									strcat(aux_escrita, " ");
									strcat(aux_escrita, aux_escrita_diretiva);
									strcat(aux_escrita, " ");
									strcat(aux_escrita, resultado_busca->valor);
									strcat(aux_escrita, "\n");
									strcpy(escrita, aux_escrita);
								} // resultado_busca != NULL
							} // diretiva
						} // 3 token

            printf("escreve atual (depende se tem IF antes)\n");

						//printf("%d, %d\n", linha_anterior_if, linha_atual_if);

						// Depende do IF para saber se a linha sera escrita
						if(linha_anterior_if == 1){

							// So escreve quando a avaliacao do if for 1
							if(escreve == 1){
								fputs(escrita, pre);
							}
						}
						// Linha que nao possui diretivas e independe do IF
						else{
							fputs(escrita, pre);
						}
          }
        } // 2 token
      } // else

    } // instrucao != NULL

  } // while (!feof)

	// Fecha o arquivo .pre
	fclose(pre);

	// Apaga arquivo de saida se encontra erro
	if(flag_erro == 1){
		printf("\n##############Erros, arquivo pre-processado nao foi gerado!\n");
		remove(arquivo_saida);
	}
	else{
		printf("\n##############Arquivo pre-processado %s gerado!\n", arquivo_saida);
	}

	// Fecha o arquivo .asm de entrada
	fclose(entrada);

	//exibe_equ(lista_equs);

	// Libera a memoria alocada para a lista de EQUS
	libera_lista(lista_equs);

	//Abre pra passar pra passagem1
	pre = fopen(arquivo_saida, "r");
	return pre;
}


/*
 *  passagem1()
 *
 *  Funcao responsavel pela 1 passagem do montador, recebendo um arquivo .pre ou .o
 *
 *  Passagem 1: informacoes de definicoes de rotulos, simbolos, etc. Armazena os
 * dados na Tabela de Simbolos e Tabela de definicoes (subconjunto da TS)
 *
 *  Estruturas de dados a serem usadas:
 *    - Contador de linhas : indica qual linha do arquivo esta (envio de erros)
 *		- Contador de posicao: endereco de memoria a ser colocado simbolo
 *		- Tabela de simbolos : rotulos e simbolos declarados no programa
 *		- Tabela de diretivas: instrucoes e diretivas reconhecidas pelo montador
 *		- Tabela de definicoes: subconjunto da TS, para o ligador usar
 *
 *  Erros: Sintatico (diretiva '%s' com argumento invalido!)
 *         Semantico ('SECTION DATA' antes de 'SECTION TEXT')
 *         Semantico (múltiplas diretivas 'SECTION TEXT')
 *         Semantico (múltiplas diretivas 'SECTION DATA')
 *         Sintatico (diretiva '%s' com argumento invalido!)
 *         Lexico (rótulo Inválido! ':' no meio do token)
 *         Semantico (simbolo redefinido)
 *         Sintatico (Múltiplos labels na mesma linha)
 *         Semantico (instrucao na secao de dados)
 *				 Sintatico (SECTION Invalido)
 *         Sintatico (operando para diretiva SPACE invalido (numero menor ou igual a 0)!)
 *         Sintatico (diretiva CONST espera 1 argumento!)
 *         Sintatico (operacao nao identificada)
 *         Semantico ('SECTION TEXT' faltando no programa)
 */
int passagem1(FILE *pre_processado){
	if(pre_processado == NULL){
	    printf("Erro na abertura do arquivo!\n");
	    exit(-4);
 	}

	printf("\n:::::::::::::PASSAGEM 1\n");
	int contador_posicao = 0;
	int contador_linha = 1;
	int i = 0;
	int pulo = 0;
	int def_sec_text = 0, def_sec_data = 0;
	int def_label;
	int op_space;
	int flag_erro = 0;

	//Cria as tabelas de simbolos e de definicoes vazias
	inicializa_tabelas();
	instancia_listas_globais();

	char linha[TLINHA];
	char *instrucao;
	char *label;
	char *elemento;
	char *valor = "xx";

	while(!feof(pre_processado)){
			//Zera definicoes de diretivas, instrucoes e labels a cada linha
			def_label = 0;

			// Funcao fgets() lê até TLINHA caracteres ou até o '\n'
	    instrucao = fgets(linha, TLINHA, pre_processado);

	    if(instrucao==NULL){
	    	break;
	    }
	    //Se a linha nao for nula
	    if(instrucao[0]!='\n' && instrucao[1]!='\0'){

		    //Scanner coloca em tokens_linha um vetor com os tokens da linha
		    if(!scanner(instrucao, contador_linha, " \t\n", 1)){
					flag_erro = 1;
				}

		    //Analisar todos os tokens da linha
		    for(i=0; tokens_linha[i]!="\0"; i++){
		    	elemento = tokens_linha[i];
					string_alta(elemento);

					//Se for diretiva PUBLIC
					if(!strcmp(elemento, "PUBLIC")){
						string_alta(tokens_linha[i+1]);
						//Insere token seguinte na tabela de definicoes
						if(strcmp(tokens_linha[i+1], "\0")){
								//Insere apenas o label
								//insere(tabela, instrucao, posicao, externo, eh_dado?)
								insere_tabela(tabela_definicoes, tokens_linha[i+1], 0, 0, def_sec_data);
						}
						else{
							printf("\nErro Sintatico (linha %d): Diretiva '%s' com argumento invalido!", contador_linha, elemento);
							flag_erro = 1;
						}
						break;
					}
					//Se for uma diretiva section
					if(!strcmp(elemento, "SECTION")){
							string_alta(tokens_linha[i+1]);
							//Se token seguinte for TEXT
							if (!strcmp(tokens_linha[i+1], "TEXT")) {
								if(def_sec_data){
									printf("\nErro Semantico (linha %d): 'SECTION DATA' antes de 'SECTION TEXT'", contador_linha);
									flag_erro = 1;
								}
								if(def_sec_text){
									printf("\nErro Semantico (linha %d): Múltiplas diretivas 'SECTION TEXT'", contador_linha);
									flag_erro = 1;
								}
								def_sec_text = 1;
								break;
							}
							else if (!strcmp(tokens_linha[i+1], "DATA")) {
								if(def_sec_data){
									printf("\nErro Semantico (linha %d): Múltiplas diretivas 'SECTION DATA'", contador_linha);
									flag_erro = 1;
								}
								def_sec_data = 1;
								break;
							}
							//Se proximo elemento n for DATA nem TEXT
							else{
								printf("\nErro Sintatico (linha %d): Diretiva '%s' com argumento invalido!", contador_linha, elemento);
								flag_erro = 1;
								break;
							}
					}
		    	//Se ṕossuir : eh label
		    	if(strstr(elemento, ":")!=NULL){
		    		//Verifica se : esta no fim do token
		    		if(elemento[strlen(elemento)-1]!=':'){
		    			printf("\nErro Lexico (linha %d): Rótulo Inválido! ':' no meio do token\n", contador_linha);
							flag_erro = 1;
		    		}
		    		else{
		    			label = elemento;
		    			//Retira ':'
		    			label[strlen(label)-1] = '\0';
		    			if(pertence_tabela(tabela_simbolos, label)){
		    				printf("\nErro Semantico (linha %d): Simbolo redefinido!\n", contador_linha);
								flag_erro = 1;
		    			}
		    			else{
								if(def_label){
									printf("\nErro Sintático (linha %d): Múltiplos labels na mesma linha!\n", contador_linha);
									flag_erro = 1;
								}
								else{
									string_alta(tokens_linha[i+1]);
									//Se possuir um extern, coloca 0 absoluto e indicacao de simbolo externo
									if(!strcmp(tokens_linha[i+1], "EXTERN")){
										//insere(tabela, instrucao, posicao, externo, eh_dado?)
										insere_tabela(tabela_simbolos, label, 0, 1, def_sec_data);
									}
									//Se n for externo
									else{
			    					insere_tabela(tabela_simbolos, label, contador_posicao, 0, def_sec_data);
									}
									def_label = 1;
								} //else def_label
		    			} //else pertence_tabela()
		    		} //else elemento :
		    	} //if(strstr(elemento, ":")!=NULL)

		    	//Se n for label, SECTION ou PUBLIC, pode ser operacao ou diretiva
		    	else{
		    		//Se achou na tabela de instrucoes, retorna diferente de -1
		    		pulo = tamanho_instrucao(elemento);
		    		if(pulo!=-1){
							//Verifica se instrucao nao esta no section data
							if(def_sec_data){
								printf("\nErro Semantico (linha %d): Instrucao na secao de dados!\n", contador_linha);
								flag_erro = 1;
							}
		    			contador_posicao += pulo + 1;
							break;
		    		}
		    		//Se n tiver achado na Tab Instrucoes, procura na de diretivas
		    		else{
		    		pulo = tamanho_diretiva(elemento);
		    		//Se tiver achado na tabela de diretivas
			    		if(pulo != -1){
								//Se tiver dentro do SECTION TEXT (antes do DATA)
								if(!def_sec_data && def_sec_text){
									printf("\nErro Semantico (linha %d): Diretiva na Secao errada!\n", contador_linha);
									flag_erro = 1;
								}
									string_alta(tokens_linha[i+1]);
									//Se for space
									if(!strcmp(elemento, "SPACE")){
										//Se n for "\0", possui operando
										if(strcmp(tokens_linha[i+1], "\0")){
												op_space = atoi(tokens_linha[i+1]);
												if(op_space<=0){
													printf("\nErro Sintático (linha %d): Operando para diretiva SPACE invalido (numero menor ou igual a 0)!\n", contador_linha);
													flag_erro = 1;
												}	//if op_space <=0
												else{
													insere_elemento(lista_tamanhos_vetores,
														pega_primeiro_simbolo(tabela_simbolos), tokens_linha[i+1]);
													contador_posicao += op_space;
												} //else
										} // if stcmp(tokens_linha[i+1])

										//Se n possuir operando, aloca 1 espaco
										else{
											insere_elemento(lista_tamanhos_vetores,
												pega_primeiro_simbolo(tabela_simbolos), "1");
											contador_posicao++;
										}
									} //if SPACE
									//Se for const
									if(!strcmp(elemento, "CONST")){
										//Se n tiver operando
										if(!strcmp(tokens_linha[i+1], "\0")){
											printf("\nErro Sintatico (linha %d): Diretiva '%s' espera 1 argumento!\n", contador_linha, elemento);
											flag_erro = 1;
										}
										//Se tiver operando, pula 1 casa
										else{
											//Se for CONST
											insere_elemento(lista_ctes,
												pega_primeiro_simbolo(tabela_simbolos), valor);
											contador_posicao++;
											//Se for CONST 0
											if(atoi(tokens_linha[i+1])==0){
												insere_elemento(lista_zeros
													, pega_primeiro_simbolo(tabela_simbolos), valor);
											}
										}
									}
									break;
			    		} //Se for Diretiva
			    	//Se n tiver achado em nenhuma das 2 tabelas, erro
			    		else {
								//Pulo pode ser -2, de um operando de diretiva invalido
								if(pulo == -1){
			    			printf("\nErro Sintatico (linha %d): Operacao nao identificada (nao eh instrucao nem diretiva)\n", contador_linha);
								flag_erro = 1;
							}
			    			break;
			    		} // else (pulo > -1) (diretiva)
		    		} //else (pulo!=-1) (instrucao)
		    	} //else (strstr(elemento, ";"))
		    } //for
		} // if instrucao nao eh nula
		contador_linha = contador_linha + 1;
  } // while (!feof)
	if(!def_sec_text){
		printf("\nErro Semantico! 'SECTION TEXT' faltando no programa\n");
		flag_erro = 1;
	}
	//Apos o fim do programa, copia os valores da tabela de simbolos
	//para a tabela de definicoes
	copia_para_definicoes();

  rewind(pre_processado);

	if(flag_erro){
		return 0;
	}
	return 1;
} //passagem1()

/*
 *  passagem2()
 *
 *  Funcao responsavel pela 2 passagem do montador, recebendo um arquivo .pre ou .o
 *
 *  Passagem 2: Enderecos dos simbolos ja sao conhecidos, declaracoes podem ser "montadas"
 *
 *  Estruturas de dados a ser usadas:
 * 		- Contador de linhas : indica qual linha do arquivo esta (envio de erros)
 *		- Contador de posicao: endereco de memoria a ser colocado simbolo
 *		- Tabela de simbolos : rotulos e simbolos declarados no programa
 *		- Tabela de diretivas: instrucoes e diretivas reconhecidas pelo montador
 *		- Tabela de definicoes: subconjunto da TS, para o ligador usar
 *    - Tabela de uso: estrutura contendo enderecos com simbolos externos
 *		- Arquivo de leitura do codigo
 *		- Arquivo para escrita do objeto
 *		- Lista de realocacao para o ligador
 *				-> Fazer uma lista para armazenar mapa de bits e posicao de memoria,
 *					para imprimir no arquivo objeto posteriormente
 *
 *  Erros: Semantico (modificacao de um valor constante)
 *         Sintatico (tentativa de divisao por zero)
 *         Semantico (multiplas diretivas BEGIN no codigo!)
 *         Semantico (diretiva END declarada antes de BEGIN)
 *         Semantico (diretiva END sem um BEGIN correspondente!)
 *         Semantico (multiplas diretivas END no codigo!)
 *         Sintatico (label ja definido na linha)
 *         Semantico (diretiva BEGIN sem um END correspondente)
 *         Semantico (falta de uma instrucao STOP no programa!)
 *
 */
FILE* passagem2(FILE *arq_intermediario, char* nome_arquivo_obj, int erro_passagem_1){
		// while !feof
		//	pega linha
		//	ignora label e comentarios
		//  if operando eh simbolo
		//		procura na TS
		//			se for externo, anota na tabela de uso
		//		se nao achou, erro (simbolo indefinido)
		//  procura operacao na TS
		//		if achou
		//			muda contador posicao
		//			if numero de operandos e tipo corretos => imprime no arquivo
		//			senao operando invalido
		//		se n achou, procura diretivas
		//			if achou diretiva => execucao diretiva, muda contador_posicao
		//		se n achou nd, operacao n identificada
		//	contador_linha++

		int contador_linha = 1;
		//contador_posicao eh ponteiro para ser passado para funcao checa_tipo()
		int* cont_pos_ptr;
		int contador_posicao = 0;
		cont_pos_ptr = &contador_posicao;

		//Propagacao do erro da passagem 1
		int flag_erro = erro_passagem_1;
		int i, checa_instrucao, checa_diretiva;
		int tamanho;
		//Flag que indica se ja houve label na linha
		int def_label = 0;
		//Flags indicando se houve Begin e End no codigo
		int def_begin = 0;
		int def_end = 0;
		int def_stop = 0;
		char* arquivo_saida;

		char *instrucao;
		char linha[TLINHA];
		char* prox_elemento;
		char *arquivo_provisorio_nome = "arquivo_provisorio";
		FILE* obj_provisorio;
		FILE* obj;

		arquivo_saida = strcat(nome_arquivo_obj, ".o");
		obj = fopen(arquivo_saida, "w");
		//Abre arquivo pra escrita
		obj_provisorio = fopen(arquivo_provisorio_nome, "w");

		//1- Escreve primeiro o codigo
		//2- rewind
		//3- Tabelas e mapa de bits
		fprintf(obj_provisorio, "CODE\n");

		printf("\n:::::::::::::PASSAGEM 2\n");
		while(!feof(arq_intermediario)){
			// Funcao fgets() lê até TLINHA caracteres ou até o '\n'
			instrucao = fgets(linha, TLINHA, arq_intermediario);

			if(instrucao==NULL){
	    	break;
	    }
	    //Se a linha nao for nula
	    if(instrucao[0]!='\n' && instrucao[1]!='\0'){
				//Como mudou de linha, zera definicao do label
				def_label = 0;

		    //Scanner coloca em tokens_linha um vetor com os tokens da linha
		    if(!scanner(instrucao, contador_linha, " ,\t\n", 2)){
					flag_erro = 1;
				}

		    //Analisar todos os tokens da linha
		    for(i=0; tokens_linha[i]!="\0" && tokens_linha[i]!=NULL; i++){
					//printf("\ntoken= <%s>\n", tokens_linha[i]);
					string_alta(tokens_linha[i]);

					if(!strcmp(tokens_linha[i], "COPY")){
						if(busca_elemento(lista_ctes, tokens_linha[i+2])!=NULL){
							printf("\nErro Semantico (linha %d): Modificacao de um valor constante\n",
						contador_linha);
						flag_erro = 1;
						}
					}
					if(!strcmp(tokens_linha[i], "STORE") || !strcmp(tokens_linha[i], "INPUT")){
						if(busca_elemento(lista_ctes, tokens_linha[i+1])!=NULL){
							printf("\nErro Semantico (linha %d): Modificacao de um valor constante\n",
						contador_linha);
						flag_erro = 1;
						}
					}
					if(!strcmp(tokens_linha[i], "DIV")){
						if(busca_elemento(lista_zeros, tokens_linha[i+1])!=NULL){
							printf("\nErro Sintatico (linha %d): Tentativa de divisao por zero\n",
						contador_linha);
						flag_erro = 1;
						}
					}
					//Se for STOP
					if(!strcmp(tokens_linha[i], "STOP")){
						def_stop = 1;
					}
					//Se for BEGIN
					if(!strcmp(tokens_linha[i], "BEGIN")){
						if(def_begin){
							printf("\nErro Semântico! Multiplas diretivas BEGIN no codigo!\n");
							flag_erro = 1;
						}
						if(def_end){
							printf("\nErro Semântico! Diretiva END declarada antes de BEGIN\n");
							flag_erro = 1;
						}
						def_begin = 1;
						continue;
					}
					//Se for END
					if(!strcmp(tokens_linha[i], "END")){
						if(!def_begin){
							printf("\nErro Semântico! Diretiva END sem um BEGIN correspondente!\n");
							flag_erro = 1;
						}
						if(def_end){
							printf("\nErro Semântico! Multiplas diretivas END no codigo!\n");
							flag_erro = 1;
						}
						def_end = 1;
						continue;
					}
					//Se for label, ignora
					if(strstr(tokens_linha[i], ":")){
							if(def_label){
								printf("\nErro Sintatico (linha %d): Label ja definido na linha\n", contador_linha);
								flag_erro = 1;
							}
							else{
								def_label = 1;
							}
							continue;
					}

					//Se for instrucao, verifica tipos com o tamanho da operacao
					tamanho = tamanho_instrucao(tokens_linha[i]);
					if(tamanho!=-1){
						*cont_pos_ptr = *cont_pos_ptr + tamanho;

						checa_instrucao = checa_tipo_instrucao(obj_provisorio, i, contador_linha, cont_pos_ptr);
						//checa_tipo_instrucao retorna -1 se houve erro
						if(checa_instrucao == -1){
							flag_erro = 1;
						}
						//Soma indice do vetor com o checa_tipo, para n ler
						//elementos da lista 2 vezes
						else{
							i += checa_instrucao;
						} //else
						continue;
					} //if INSTRUCAO
					//Se for diretiva (sem ser BEGIN ou END), retorna >= 0
					tamanho = tamanho_diretiva(tokens_linha[i]);
					if(tamanho >= 0){
						//Incrementa 1 se for CONST, variavel se for SPACE, 0 Caso Contrario
						checa_diretiva = checa_tipo_diretiva(obj_provisorio, i, contador_linha, cont_pos_ptr);
						//Se retornar -1, houve erro
						if(checa_diretiva == -1){
							flag_erro = 1;
						}
						else{
							i += checa_diretiva;
						}
						continue;
					} //if Diretiva

				} //For (!fim da linha)
			} //Se a linha n for nula

			contador_linha++;
		} //while (!feof)
		//Inverte para mandar ordem certa
		insere_lista(mapa_bits, mapa_provisorio);
		fclose(obj_provisorio);

		if(flag_erro){
			//Exclui arquivo provisorio, pois nao sera mais utilizado
			printf("\nErros, arquivo objeto nao foi gerado!\n");
			remove(arquivo_provisorio_nome);
			return NULL;
		}
		if(!def_end && def_begin){
			printf("\nErro Semântico! Diretiva BEGIN sem um END correspondente!\n");
		}
		else if(def_end && def_begin){
			imprime_tabelas_arquivo(1, obj, arquivo_provisorio_nome, mapa_provisorio);
			printf("\nArquivo %s criado!\n", arquivo_saida);
		}
		else{
			if(!def_stop){
				printf("\nErro Semântico! Falta de uma instrucao STOP no programa!\n");
			}
			else{
				imprime_tabelas_arquivo(0, obj, arquivo_provisorio_nome, mapa_provisorio);
				printf("\nArquivo %s criado!\n", arquivo_saida);
			}
		}
		//Exclui arquivo provisorio, pois nao sera mais utilizado
		remove(arquivo_provisorio_nome);
}

/*
 *  imprime_tokens()
 *
 *  Funcao que exibe os tokens de uma determinada linha
 */
void imprime_tokens(){
	int i=0;

	for(i=0; tokens_linha[i]!="\0"; i++){
		printf("\nToken <%s>\n", tokens_linha[i]);

		if(strstr(tokens_linha[i],"\n")!=NULL){
			return;
		}
	}
}

/*
 *  checa_tipo_instrucao()
 *
 *  Funcao de checagem de tipos das instrucoes
 *
 *  Erros: Semantico (pulo para rotulo invalido!)
 *         Sintatico (argumento nao declarado!)
 *         Sintatico (argumento invalido!)
 *         Sintatico ('STOP' nao recebe argumentos!)
 */
int checa_tipo_instrucao(FILE* obj, int i, int contador_linha, int *contador_posicao){
	// Recebe indice atual da posicao na linha, numero da linha (para msg de erro)
	// e contador posicao (para colocar na tabela de uso)
	// Se os tipos estiverem
	// certos, escreve no arquivo
	// Caso contrario, retorna erro
	// Retorna posicao do vetor tokens_linha apos a operacao e seus operandos

	char *elemento = tokens_linha[i];
	char *antes_mais1, *antes_mais2;
	int depois_mais_num1, depois_mais_num2;
	int indice_retorno, erro = 0;
	int externo1 = 0, externo2 = 0;
	int flag_dado;

	//Coloca em Upper Case para fazer comparacao
	string_alta(tokens_linha[i+1]);
	string_alta(tokens_linha[i+2]);

	//Se for instrucao de desvio
	if(!strcmp(tokens_linha[i], "JMP") || !strcmp(tokens_linha[i], "JMPN")
	|| !strcmp(tokens_linha[i], "JMPP") || !strcmp(tokens_linha[i], "JMPZ")){
		//Se for um simbolo externo, coloca na tabela de uso
		if(eh_externo(tokens_linha[i+1])){
			externo1 = 1;
			//insere(tabela, instrucao, posicao, externo, eh_dado?)
			insere_tabela(tabela_uso, tokens_linha[i+1], *contador_posicao, 0, 0);
		}
		//Se for dado
		else if(eh_dado(tokens_linha[i+1])==1){
			printf("\nErro Semantico (linha %d): Pulo para rotulo invalido!\n", contador_linha);
			erro = 1;
		}
		//Se n tiver achado na tabela
		else if(eh_dado(tokens_linha[i+1])==-1){
			printf("\nErro Sintatico (linha %d): Argumento nao declarado!\n", contador_linha);
			erro = 1;
		}
		//Se argumento for valido, imprime
		if(!erro){
			//imprime opcode e operador no arquivo
			fprintf(obj, "%d %d ", opcode(tokens_linha[i]), busca_posicao_memoria(tabela_simbolos, tokens_linha[i+1]));
			insere_elemento(mapa_bits, "x", "0");
			if(externo1){
				insere_elemento(mapa_bits, "x", "0");
			}
			else{
				insere_elemento(mapa_bits, "x", "1");
			}
		}
	}
	//Se for copy
	else if(!strcmp(tokens_linha[i], "COPY")){
				//TRATAMENTO TOKEN 1
				depois_mais_num1 = (tokens_linha[i+1], contador_linha, *contador_posicao-1);
					//Se for 0, n tem +
					if(!depois_mais_num1){
							flag_dado = eh_dado(tokens_linha[i+1]);
							antes_mais1 = tokens_linha[i+1];
							//Se for um simbolo externo, coloca na tabela de uso
							if(eh_externo(tokens_linha[i+1])){
								externo1 = 1;
								//insere(tabela, instrucao, posicao, externo, eh_dado?)
								insere_tabela(tabela_uso, tokens_linha[i+1], *contador_posicao-1, 0, 0);
							}
							//Se 1 argumento n for dado
							else if(flag_dado!=1){
								if(flag_dado == -1){
									printf("\nErro Semantico (linha %d): Argumento 1 de 'COPY' nao declarado!\n", contador_linha);
								}
								else{
								printf("\nErro Sintatico (linha %d): Argumento 1 de 'COPY' invalido!\n", contador_linha);
								}
								erro = 1;
							} //else if
					} // if(!depois_mais_num1)
					else if(depois_mais_num1 == -1){
						erro = 1;
					}
					//Dando 1, copia so a primeira parte
					else if (depois_mais_num1){
						antes_mais1 = pega_antes_mais(tokens_linha[i+1]);
						if(eh_externo(antes_mais1)){
							externo1 = 1;
						}
					}

				//TRATAMENTO TOKEN 2
				depois_mais_num2 = pega_elemento_vetor(tokens_linha[i+2], contador_linha, *contador_posicao);
					//Se for 0, n tem +
					if(!depois_mais_num2){
							flag_dado = eh_dado(tokens_linha[i+2]);
							antes_mais2 = tokens_linha[i+2];
							//Se for um simbolo externo, coloca na tabela de uso
							if(eh_externo(tokens_linha[i+2])){
								externo2 = 1;
								//insere(tabela, instrucao, posicao, externo, eh_dado?)
								insere_tabela(tabela_uso, tokens_linha[i+2], *contador_posicao, 0, 0);
							}
							//Se 2 argumento n for dado
							else if(flag_dado!=1){
								if(flag_dado == -1){
									printf("\nErro Semantico (linha %d): Argumento 2 de 'COPY' nao declarado!\n", contador_linha);
								}
								else{
								printf("\nErro Sintatico (linha %d): Argumento 2 de 'COPY' invalido!\n", contador_linha);
								}
								erro = 1;
							} //else if
					} // if(!depois_mais_num1)
					else if(depois_mais_num2 == -1){
						erro = 1;
					}
					//Dando 1, copia so a primeira parte
					else if (depois_mais_num2){
						antes_mais2 = pega_antes_mais(tokens_linha[i+2]);
						if(eh_externo(antes_mais2)){
							externo2 = 1;
						}
					}

				//Se os 2 forem argumentos validos
				if(!erro){
					int n1 = busca_posicao_memoria(tabela_simbolos, antes_mais1)+depois_mais_num1;
					int n2 = busca_posicao_memoria(tabela_simbolos, antes_mais2)+depois_mais_num2;
					//Imprime opcode + posicao dos operandos
					fprintf(obj, "%d %d %d ", opcode(tokens_linha[i]), n1, n2);
					insere_elemento(mapa_bits, "x", "0");
					if(externo1){
						insere_elemento(mapa_bits, "x", "0");
					}
					else{
						insere_elemento(mapa_bits, "x", "1");
					}
					if(externo2){
						insere_elemento(mapa_bits, "x", "0");
					}
					else{
						insere_elemento(mapa_bits, "x", "1");
					}
				}
	} //else if (COPY)
	else if(!strcmp(tokens_linha[i], "STOP")){
			//Se n for ultimo token da linha
			if(strcmp(tokens_linha[i+1], "\0")){
				printf("\nErro Sintatico (linha %d): 'STOP' nao recebe argumentos!\n", contador_linha);
				erro = 1;
			}
			if(!erro)
			fprintf(obj, "%d ", opcode(tokens_linha[i]));
			insere_elemento(mapa_bits, "x", "0");
	}
	//Se n for nem desvio, copy ou stop, vai ter 1 operando da area de dados
	else{
			depois_mais_num1 = pega_elemento_vetor(tokens_linha[i+1], contador_linha, *contador_posicao);
			//Se n possuir +, considera todo o token como label
			if(depois_mais_num1 == 0){
					flag_dado = eh_dado(tokens_linha[i+1]);
					//Pega o label td para impressao
					antes_mais1 = tokens_linha[i+1];
					//Se for um simbolo externo, coloca na tabela de uso
					if(eh_externo(tokens_linha[i+1])){
						externo1 = 1;
						//insere(tabela, instrucao, posicao, externo, eh_dado?)
						insere_tabela(tabela_uso, tokens_linha[i+1], *contador_posicao, 0, 0);
					}
					else if(flag_dado != 1){
						if(flag_dado == -1){
							printf("\nErro Semantico (linha %d): Argumento nao declarado!\n", contador_linha);
						}
						else{
							printf("\nErro Sintatico (linha %d): Argumento invalido!\n", contador_linha);
						}
						erro = 1;
					} //if
			}
			else if (depois_mais_num1 == -1){
				erro = 1;
			}
			//Dando 1, copia so a primeira parte
			else if (depois_mais_num1){
				//Pega ate o mais para tratar como label
				antes_mais1 = pega_antes_mais(tokens_linha[i+1]);
				if(eh_externo(antes_mais1)){
					externo1 = 1;
				}
			}
			//Se for valido
			if(!erro){
					int n1 = busca_posicao_memoria(tabela_simbolos, antes_mais1)+depois_mais_num1;
					// Imprime opcode + posicao dos operandos
					fprintf(obj, "%d %d ", opcode(tokens_linha[i]), n1);
					insere_elemento(mapa_bits, "x", "0");
					if(externo1){
						insere_elemento(mapa_bits, "x", "0");
					}
					else{
						insere_elemento(mapa_bits, "x", "1");
					}
				} //if (!erro)
	} //else if(!COPY & !DESVIO)

	if(erro){
		return -1;
	}
	*contador_posicao = *contador_posicao + 1;
	return tamanho_instrucao(tokens_linha[i]);
} //checa_tipo()

/*
 *  checa_tipo_diretiva()
 *
 *  Funcao de checagem de tipos de diretivas
 *
 *  Erros: Sintatico (Esperado um numero como argumento!)
 */
int checa_tipo_diretiva(FILE* obj, int i, int contador_linha, int *contador_posicao){
	// Recebe indice atual da posicao na linha, numero da linha (para msg de erro)
	// e contador posicao (para incrementar posicao)
	// Se os tipos estiverem
	// certos, escreve no arquivo
	// Caso contrario, retorna erro
	// Retorna posicao do vetor tokens_linha apos a operacao e seus operandos
	int erro = 0;
	int op_space;
	int op_const;
	char* elemento;
	int tamanho_elemento = 0;

	elemento = tokens_linha[i];
	string_alta(tokens_linha[i+1]);
	//Se for space
	if(!strcmp(elemento, "SPACE")){
		//Se n for "\0", possui operando
		if(strcmp(tokens_linha[i+1], "\0")){
				op_space = atoi(tokens_linha[i+1]);
				if(op_space<=0){
					erro = 1;
				}	//if op_space <=0
				else{
					//Coloca 0 para cada espaco reservado
					for(int j=0; j<op_space; j++){
						fprintf(obj, "0 ");
						insere_elemento(mapa_bits, "x", "0");
					}
					contador_posicao += op_space;
				} //else
		} // if stcmp(tokens_linha[i+1])

		//Se n possuir operando, aloca 1 espaco
		else{
			op_space = 1;
			fprintf(obj, "0 ");
			insere_elemento(mapa_bits, "x", "0");
			contador_posicao +=1;
		}
	} //if SPACE
	//Se for const
	if(!strcmp(elemento, "CONST")){
		//Se n tiver operando
		if(!strcmp(tokens_linha[i+1], "\0")){
			erro = 1;
		}
		//Se tiver operando, pula 1 casa
		else{
				//Se for hexadecimal
				if(eh_numero(tokens_linha[i+1], 'h')){
					op_const = strtol(tokens_linha[i+1], NULL, 16);
					fprintf(obj, "%d ", op_const);
					insere_elemento(mapa_bits, "x", "0");
					contador_posicao++;
				}
				//Se for decimal
				else if(eh_numero(tokens_linha[i+1], 'd')){
					op_const = atoi(tokens_linha[i+1]);
					fprintf(obj, "%d ", op_const);
					insere_elemento(mapa_bits, "x", "0");
					contador_posicao++;
				}
				//Se n for decimal nem hexa
				else{
					printf("Erro Sintático (linha %d): Esperado um numero como argumento!\n"
					,contador_linha);
					contador_posicao++;
				}
		}
	}

	if(erro){
		return -1;
	}
	return tamanho_diretiva(tokens_linha[i]);
}
