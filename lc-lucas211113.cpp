#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>

FILE *fp;					// ponteiro para o arquivo
void leitor_carta();		// protótipo da função para Leitura da Carta
void leitor_secao_solida();
void leitor_secao_material();
void leitor_secao_step();

// Variaveis globais
int cont = 0; 				// contador temporario de nós
int flag3d = 0;				// Se 'flag3D=0' então o modelo é 2D, se 'flag3D=1' então o modelo é 3D

// SECTION
struct section {
	char type[20]; // até 40 caracteres por campo da tabela 4.5 do "algoritmo_SysOpti_02.docx"
	char elset[20];
	char mat[20];
};

struct material{
	char type[20];
	char name[20];
	char text[55];
};

struct step{
	char load_step_text;
	char type[20];
};

struct cload{
	int id;
	int dof;
	float valor_forca;
};

struct boundary{
	int id;
	int dof1;
	int dof2;
	float valor_desloc;
};

struct section *ptr_section;			// ponteiro para estrutura section
struct material *ptr_material;			// ponteiro para estrutura material
struct step *ptr_step;				// ponteiro para estrutura step
struct cload *ptr_cload;
struct boundary *ptr_boundary;
char *step_text;
int *controle_boundary;
int *controle_cload;

double *section_num;				// ponteiro para o escalar da SECTION ???
int cont_section = 0;
int cont_material = 0;
int cont_boundary = 0;
int cont_cload = 0;
int cont_cont = 0;
int cont_cont2 = 0;
//unsigned long cont_step = 0;
int cont_step = 0;

int main(int argc, char *argv[])
{
	printf("\nLeitor de Carta - v1");

	fp = fopen("carta-lucas.inp","r");
	//fp = fopen(argv[1],"r");

	if (fp == NULL)
		printf("\nErro! Arquivo inexistente! - Uso: teste1 nome_da_carta");
	else
	{
		leitor_carta();
		fclose(fp);
	}
	printf("\nFinal - Leitor de Carta - v1\n");

	getchar();
	return 0;
}

void leitor_carta()
{
	char ch, inst[4], ch2=' ';
	int i;

	ch=getc(fp);
	while ( ch != EOF )
	{
		printf("\nLeitor Carta - Pressione 'q' para sair");
		printf("%c",ch);
		scanf("%c",&ch2);
		if (ch2 == 'q')
			break;
		printf("%c",ch2);
		// Se começa com um '*' é uma nova "instrução" da carta - Primeiro Byte
		if (ch == '*')
		{
			// Ler os 4 próximos bytes depois do '*' para identificar a "instrução"
			for(i=0; i < 4;i++)
				inst[i] = getc(fp);
		}
		// Leitura da SOLID SECTION
		if ((inst[0] == 'S') && (inst[1] == 'O') && (inst[2] == 'L') && (inst[3] == 'I'))
			leitor_secao_solida();

		if ((inst[0] == 'M') && (inst[1] == 'A') && (inst[2] == 'T') && (inst[3] == 'E'))
			leitor_secao_material();

		if ((inst[0] == 'S') && (inst[1] == 'T') && (inst[2] == 'E') && (inst[3] == 'P'))
			leitor_secao_step();
		// Limpar detector de "instrução"
		for(i=0; i < 4;i++)
			inst[i] = ' ';
	}
}

void leitor_secao_solida()
{
	int i=0;
	char ch; // Pegar somente os 2 bytes iniciais -> Tamanho máximo para o tipo do elemento DCOUPD3 = 7 bytes
	char tempchar[20];
	while( ch != '*' )
	{
		//ptr_section = (struct section *) realloc(ptr_section,((cont_section+1)*sizeof(struct section)));
		// Tratamento da alocação dinâmica
		if (cont_section == 0)
			ptr_section = (struct section *) malloc(sizeof(struct section));
		else
			ptr_section = (struct section *) realloc(ptr_section,((cont_section+1)*sizeof(struct section)));

		strcpy(ptr_section[cont_section].type, "SOLID");

		strcpy(tempchar, "                    "); // preencher com 20 espacos em branco

		printf("\nCOMECO SOLID SECTION\n");

		while (getc(fp) != '='); // ate achar o primeiro sinal de igual
	//	printf("\nelset");
		ch = getc(fp);
			for(i=0; ch!=',';i++){
				ptr_section[cont_section].elset[i] = ch;
				putchar(ptr_section[cont_section].elset[i]);
				ch = getc(fp);
			}
			printf("\n");
			while (getc(fp) != '=');
			ch = getc(fp);
			for(i=0; ch!=13 && ch!=10;i++){
				ptr_section[cont_section].mat[i] = ch;
				putchar(ptr_section[cont_section].mat[i]);
				ch = getc(fp);
			}

		printf("\nch -> H = %x D = %d",ch,ch);
		ch=getc(fp);
		printf("\nch -> H = %x D = %d",ch,ch);
	}
	cont_section++;
	printf("\ncont_section = %d",cont_section);
	getchar();
}

void leitor_secao_material()
{
	int i=0;
	int a=0;
	char ch;
	char tempchar[20];
	while( ch != '*' )
	{
		if (cont_material == 0)
			ptr_material = (struct material *) malloc(sizeof(struct material));
		else
			ptr_material = (struct material *) realloc(ptr_material,((cont_material+1)*sizeof(struct material)));

		strcpy(ptr_material[cont_material].type, "MATERIAL");

		strcpy(tempchar, "                    "); // preencher com 20 espacos em branco

		printf("\nCOMECO MATERIAL\n");
		while (getc(fp) != '='); // ate achar o primeiro sinal de igual
		ch = getc(fp);

		for(i=0;ch!='*';i++){
			ptr_material[cont_material].name[i] = ch;
			putchar(ptr_material[cont_material].name[i]);
			ch = getc(fp);
		}
		putchar(ch);
		ch = getc(fp); //pular asterisco do elastic

		for(i=0; ch!= '*';i++){
			ptr_material[cont_material].text[i] = ch;
			putchar(ptr_material[cont_material].text[i]);
			ch = getc(fp);
		}
}
}

void leitor_secao_step()
{
	int ch=0,i=0,a=0,cont_virgula=0;
	char tipo_analise[22],x[10],y[5],z[5],w[10],c[10],d[5],e[18],verif_boundary[3],verif_end[8];
	
	strcpy(tipo_analise, "                      ");
	strcpy(x, "          ");
	strcpy(y, "     ");
	strcpy(z, "     ");
	strcpy(w, "          ");
	strcpy(c, "          ");
	strcpy(d, "     ");
	strcpy(e, "                  ");
	strcpy(verif_boundary, "   ");
	strcpy(verif_end, "        ");
	
	printf("\nSECAO STEP\n");
	if(getc(fp)==','){    //verificar se existe uma segunda palavra depois de STEP
	do{
		if (cont_step == 0)
			step_text = (char *) malloc(sizeof(char *));
		else
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char *)));
			
	ch = getc(fp);
	step_text[cont_step] = ch;
	printf("\nstep_text[%d] = %c",cont_step,step_text[cont_step]);
	cont_step++;
	}while(step_text[cont_step-1]!= '*');
	printf("\nxxx tipo analise =\n");
	tipo_analise[i] = step_text[cont_step-1];
	putchar(tipo_analise[i]);
	i++;
}
	
	do{
		if (cont_step == 0){
			step_text = (char *) malloc(sizeof(char));
		
		}else{
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
				
		}


	step_text[cont_step] = getc(fp);
	tipo_analise[i] = step_text[cont_step];
	printf("\nAAA - step_text[%d] = %c",cont_step,step_text[cont_step]);
	getchar();

	cont_step++;
	i++;
	}while(i<4);

	// STA
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'S') && (tipo_analise[2] == 'T') && (tipo_analise[3] == 'A')){
		
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// BUC
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'B') && (tipo_analise[2] == 'U') && (tipo_analise[3] == 'C')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// FRE
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'F') && (tipo_analise[2] == 'R') && (tipo_analise[3] == 'E')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// COM
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'C') && (tipo_analise[2] == 'O') && (tipo_analise[3] == 'M')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// MOD
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'M') && (tipo_analise[2] == 'O') && (tipo_analise[3] == 'D')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// STE
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'S') && (tipo_analise[2] == 'T') && (tipo_analise[3] == 'E')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

	// DYN
	if ((tipo_analise[0] == '*') && (tipo_analise[1] == 'D') && (tipo_analise[2] == 'Y') && (tipo_analise[3] == 'N')){
	  for(;;cont_step++){	
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      tipo_analise[i] = step_text[cont_step];
      printf("\nSTA - step_text[%d] = %c",cont_step,step_text[cont_step]);
      getchar();
      i++;
      	if(step_text[cont_step]=='*')
   			break;	   
      }
  }

    printf("\nfim tipo_analise\n");
	for(;;){
    
	while(step_text[cont_step-1]!='*'){           //BOUNDARY
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      
      if(step_text[cont_step]==','){
		for(a=0;a<5;a++){
      	step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
		step_text[cont_step] = getc(fp);
		printf("\nBBB - step_text[%d] = %c",cont_step,step_text[cont_step]);
   		cont_step++;
   		getchar();
    	}
      }
      
      if(step_text[cont_step]==32){

      	for(a=0;;a++){
      		//printf("\nID\n");
			x[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = x[a];
      		cont_step++;			
			if(a[x]==','){
				a[x]=' ';
				break;
			}
			//printf("x = %c",x[a]);
      	}
      	
      	for(a=0;;a++){
      		//printf("\nDOF1\n");
			y[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = y[a];
      		cont_step++;			
			if(y[a]==','){
				y[a] = ' ';
				cont_virgula++;
			}
			if(cont_virgula==1)
				break;
			//printf("y = %c",y[a]);
      	}	
	
		for(a=0;;a++){
			cont_virgula=0;
      		//printf("\nDOF2\n");
			z[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = z[a];
      		cont_step++;			
			if(z[a]==','){
				z[a] = ' ';
				cont_virgula++;
			}
			if(cont_virgula==1)
				break;
			//printf("y = %c",z[a]);
      	}
      	
		for(a=0;;a++){
      		//printf("\nVALOR DESLOC\n");
			w[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = w[a];
      		cont_step++;			
			if(w[a]==10)
				break;
			if(w[a]=='*')
				break;	
			//printf("w = %c",w[a]);
      	}		  
		  		
      	
      	if (cont_boundary == 0)
			ptr_boundary = (struct boundary *) malloc(sizeof(struct boundary));
		else
			ptr_boundary = (struct boundary *) realloc(ptr_boundary,((cont_boundary+1)*sizeof(struct boundary)));
			
  		ptr_boundary[cont_boundary].id = atoi(x);	// converter caracteres para INT	
		printf("\nPTR BOUNDARY ID[%d] -> %d",cont_boundary,ptr_boundary[cont_boundary].id);
  		ptr_boundary[cont_boundary].dof1 = atoi(y);	// converter caracteres para INT	
		printf("\nPTR BOUNDARY DOF1[%d] -> %d",cont_boundary,ptr_boundary[cont_boundary].dof1);	
		ptr_boundary[cont_boundary].dof2 = atoi(z);	// converter caracteres para INT	
		printf("\nPTR BOUNDARY DOF2[%d] -> %d",cont_boundary,ptr_boundary[cont_boundary].dof2);	
		ptr_boundary[cont_boundary].valor_desloc = atof(w);	// converter caracteres para INT	
		printf("\nPTR BOUNDARY VALOR DESLOC[%d] -> %f",cont_boundary,ptr_boundary[cont_boundary].valor_desloc);			
		cont_boundary++;
  	}	
    printf("\nBBB - step_text[%d] = %c",cont_step,step_text[cont_step]);
    cont_step++;
    getchar();
    }
    
    //Controle BOUNDARY
    if(cont_cont ==0)
    	controle_boundary = (int *) malloc(sizeof(int *));
    else
    	controle_boundary = (int *) realloc(controle_boundary,((cont_cont+1)*sizeof(int)));
    
    controle_boundary[cont_cont] = cont_boundary - 1;
    cont_cont++;
	 
    while(step_text[cont_step]!='*'){           //CLOAD	
      cont_step++;
      step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      step_text[cont_step] = getc(fp);
      printf("\nCLOAD - step_text[%d] = %c\n",cont_step,step_text[cont_step]);
      
    if(step_text[cont_step]==','){
		for(a=0;a<7;a++){
		cont_step++;
      	step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
		step_text[cont_step] = getc(fp);
		printf("\nCLOAD VIRGULA - step_text[%d] = %c",cont_step,step_text[cont_step]);
   		getchar();
    	}
      }
      
      if(step_text[cont_step]==32){

      	for(a=0;;a++){
      		//printf("\nID\n");
			c[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = c[a];
      		cont_step++;
			if(c[a]==','){
				c[a]=' ';
				break;
			}
			//printf("c = %c",c[a]);
      	}
      	
      	for(a=0;;a++){
      		cont_virgula=0;
      		//printf("\nDOF1\n");
			d[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = d[a];
      		cont_step++;			
			if(d[a]==','){
				d[a] = ' ';
				cont_virgula++;
			}
			if(cont_virgula==1)
				break;
			//printf("d = %c",d[a]);
      		}
      	
		for(a=0;;a++){
      		//printf("\nVALOR FORCA\n");
			e[a]=getc(fp);
			step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
      		step_text[cont_step] = e[a];
      		cont_step++;			
			if(e[a]==10)
				break;
			if(e[a]=='*')
				break;	
			//printf("w = %c",e[a]);
      	}		  
		  		
      	
      	if (cont_cload == 0)
			ptr_cload = (struct cload *) malloc(sizeof(struct cload));
		else
			ptr_cload = (struct cload *) realloc(ptr_cload,((cont_cload+1)*sizeof(struct cload)));
			
  		ptr_cload[cont_cload].id = atoi(c);	// converter caracteres para INT	
		printf("\nPTR CLOAD ID[%d] -> %d",cont_cload,ptr_cload[cont_cload].id);
  		ptr_cload[cont_cload].dof = atoi(d);	// converter caracteres para INT	
		printf("\nPTR CLOAD DOF[%d] -> %d",cont_cload,ptr_cload[cont_cload].dof);	
		ptr_cload[cont_cload].valor_forca = atof(e);	// converter caracteres para INT	
		printf("\nPTR CLOAD VALOR FORCA [%d] -> %-1f",cont_cload,ptr_cload[cont_cload].valor_forca);			
		cont_cload++;
		
      	getchar();
      }
      
	}
	
	//Controle CLOAD
    if(cont_cont2 ==0)
    	controle_cload = (int *) malloc(sizeof(int *));
    else
    	controle_cload = (int *) realloc(controle_cload,((cont_cont2+1)*sizeof(int)));
    
    controle_cload[cont_cont2] = cont_cload - 1;
    cont_cont2++;
	cont_step++;
	
	for(a=0;a<3;a++){
	step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
	verif_boundary[a] = getc(fp);
	step_text[cont_step] = verif_boundary[a];
	printf("step_text[%d] -> %c\n",cont_step,step_text[cont_step]);
	cont_step++;
	//putchar(step_text[cont_step]);
	}
	
	
	if((verif_boundary[0]=='B') && (verif_boundary[1]=='O') && (verif_boundary[2]=='N'));
	
	if((verif_boundary[0]=='C') && (verif_boundary[1]=='O') && (verif_boundary[2]=='N'))
		break;
	if((verif_boundary[0]=='O') && (verif_boundary[1]=='U') && (verif_boundary[2]=='T'))
		break;
	
	}
	
	
	do{
	step_text = (char *) realloc(step_text,((cont_step+1)*sizeof(char)));
	step_text[cont_step] = getc(fp);
	ch = step_text[cont_step];
	printf("\nstep_text[%d] -> %c",cont_step,step_text[cont_step]);
	cont_step++;
	}while(ch != EOF);
}
	/*while(step_text[cont_step-1]!='*'){
	step_text[cont_step] = getc(fp);
	printf("\nstep_text[%d] -> %c",cont_step,step_text[cont_step]);
	cont_step++;
	}
	
	for(a=0;a<8;a++){
	verif_end[a] = getc(fp);
	step_text[cont_step] = verif_end[a];
	printf("\nstep_text[%d] -> %c",cont_step,step_text[cont_step]);
	cont_step++;
	}
	
	if((verif_end[0]=='E') && (verif_end[1]=='N') && (verif_end[2]=='D'));
	
	else{
		while(step_text[cont_step-1]!='*'){
		step_text[cont_step] = getc(fp);
		printf("\nstep_text[%d] -> %c",cont_step,step_text[cont_step]);
		cont_step++;
		}
	}
	
	for(a=0;a<8;a++){
	verif_end[a] = getc(fp);
	step_text[cont_step] = verif_end[a];
	printf("\nstep_text[%d] -> %c",cont_step,step_text[cont_step]);
	cont_step++;
	}
	
	if((verif_end[0]=='E') && (verif_end[1]=='N') && (verif_end[2]=='D')){
		printf("\n");
		getch();
	}
	}*/
	


    //Exemplo de conversão de int e float
    // char id[10],x[17];					// ESPACOS TEMPORARIOS
    // ptr_node[cont_nos].id = atoi(id);	// converter caracteres para INT
	// ptr_node[cont_nos].x  = atof(x);		// converter caracteres para DOUBLE
	// Para editar o codigo fonte KWrite
	// Para compilar no Linux: gcc lc-lucas.c -o lc-lucas.out
	// Para executar: ./lc-lucas.out

