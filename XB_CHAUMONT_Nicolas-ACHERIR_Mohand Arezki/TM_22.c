#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TM TM;
typedef struct Etat Etat;
typedef struct Trans Trans;

struct Etat {
	char nom[100];
	struct Etat* suivant;	
};

struct Trans {
	char* cinq_uplet[5];
	struct Trans* suivant;
};

struct TM  {
	char nom[100];
	Etat* etats;
	char alphabet[2];
	char alphabet_travail[3];
	char q_init[100];
	char* q_final;
	Trans* transition_func;
	
	
	char etat_courant;
	char* bande;
	int pos_tete;
};

void append_etat(Etat* head, Etat* e) {
	while(head->suivant != NULL) {
		head = head->suivant;
	}
	
	head->suivant = e;
}

void append_trans(Trans* head, Trans* e) {
	while(head->suivant != NULL) {
		head = head->suivant;
	}
	head->suivant = e;
}

void free_etats(Etat* e) {
	if(e == NULL) {
		return;
	}
	
	Etat* next = e->suivant;
	
	free(e);
	
	free_etats(next);
}

void free_trans(Trans* t) {
	if(t == NULL) {
		return;
	}
	
	Trans* next = t->suivant;
	
	for(int i = 0 ; i < 5 ; i++) {
		free(t->cinq_uplet[i]);
	}
	
	free(t);
	
	free_trans(next);
}

Trans* get_tail(Trans* f) {
	while(f->suivant != NULL) {
		f = f->suivant;
	}
	return f;
}

void free_every_fucking_thing(TM* M) {
	free(M->q_final);
	free_etats(M->etats);
	free_trans(M->transition_func);
}

int main(int argc, char** argv) {
	if(argc < 2) {
		printf("Usage: ./TM_22 nomdufichier.txt\n");
		exit(-1);	
	}

	printf("CONVERSION DE {a, b, c, d} VERS {0, 1, _}\n\n");
	TM* MMM = (TM*)malloc(sizeof(TM));
	MMM->transition_func = NULL;

	TM M = *MMM;
	
	FILE* desc = fopen(argv[1], "r");
	
	char word[100];
	int i = 0;
	
	int v = 0, tour = 0;
	
	int n = 0, q_i = 0, q_f = 0;
	int ti = 0;
	
	int nb_q = 0, nb_t = 0, end_of_trans = 0;
	
	char c;
	
	while((c = getc(desc)) != EOF)	{
		if((isalpha(c)) || isdigit(c) ||(c == '_') || (c == '>') || (c == '<') || (c == '-') || (c == '0') || (c == '1')){ // Assemblage du mon de l'etat
			if(c == '<' || c == '>' || c == '-') {                                                            // ou de la lettre
				end_of_trans = 1;                             												  // ou de la direction
			}
			v = 1; //pour dire aux 'if' ci-dessous que les lettres mises dans 'word' ne sont pas autres que 0, 1, _, <, >
				   //  car dans le fichier a lire contient des virgules et des sauts a la ligne
			  
			word[i] = c; // remplir word
			i++;
			word[i] = '\0';
		}
		if(!( (isalpha(c)) || isdigit(c)  || (c == '_') || (c == '0') || (c == '1') ) || (end_of_trans == 1) ) {
			end_of_trans = 0; 
			if((v == 1) && (tour < 6) && (tour%2 == 0)) {
				v = 0;
				tour++;
				
				if(strcmp(word, "name") == 0) {
					n = 1;
				}
				
				else if(strcmp(word, "init") == 0) {
					q_i = 1;	
				}
				else if(strcmp(word, "accept") == 0) {
					q_f = 1;
				}
			}
				
			else if(v == 1 && ( ((tour < 6) && (tour%2 == 1)) || (tour >= 6)) ){
				
				v = 0;
				
				if(tour < 6) {
					tour++;
				}	
					
				if(n == 1) { //intiliser le nom de notre TM
					strcpy(M.nom, word);
					
					n = 0;
				}
				else if(q_i == 1) { //initialiser l'etat initial
					strcpy(M.q_init, word);
					
					q_i = 0;
				}
				else if(q_f == 1) { //initialiser l'etat final
					M.q_final = (char*) malloc(sizeof(char)*(strlen(word)+1));
					strcpy(M.q_final, word);
					
					q_f = 0;
				}
				else { //Si non, outre le nom, les etats initial et final, on stocke les transitions
						if(ti%5 == 0) {
							ti = 0;
							
							if(nb_q == 0) {
								nb_q = 1;
								
								M.etats = (Etat*) malloc(sizeof(Etat));
								
								strcpy(M.etats->nom, word);
								M.etats->suivant = NULL;
							}
							else {
								Etat *nw = (Etat*)malloc(sizeof(Etat));
								
								strcpy(nw->nom, word);
								nw->suivant = NULL;
								
								append_etat(M.etats, nw);
							}
							
							if(nb_t == 0) {
								nb_t = 1;
								
								M.transition_func = (Trans*) malloc(sizeof(Trans));
								
								M.transition_func->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
								strcpy(M.transition_func->cinq_uplet[ti], word);
								
								M.transition_func->suivant = NULL;
							}
							else {
								Trans* nw = (Trans*)malloc(sizeof(Trans));
									
								nw->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
								strcpy(nw->cinq_uplet[ti], word);
								nw->suivant = NULL;
								
								append_trans(M.transition_func, nw);
							}
						}		
						else if(ti%5 == 1) {
							ti = 1;
							
							get_tail(M.transition_func)->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
							strcpy(get_tail(M.transition_func)->cinq_uplet[ti], word);						
						}
						else if(ti%5 == 2) {
							ti = 2;
							
							Etat *nw = (Etat*)malloc(sizeof(Etat));
								
							strcpy(nw->nom, word);
							nw->suivant = NULL;
								
							append_etat(M.etats, nw);
																
							get_tail(M.transition_func)->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));	
							strcpy(get_tail(M.transition_func)->cinq_uplet[ti], word);
						}
						else if(ti%5 == 3) {
							ti = 3;
							
							get_tail(M.transition_func)->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
							strcpy(get_tail(M.transition_func)->cinq_uplet[ti], word);
						}
						else if(ti%5 == 4) {
							ti = 4;
							
							get_tail(M.transition_func)->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
							strcpy(get_tail(M.transition_func)->cinq_uplet[ti], word);
						}
						ti++;
					}
				}
				i = 0;
				word[i] = '\0';
			}
		}
		
	
	//!!!!!!!!! fin partie 1 !!!!!!!!

	Trans* tr = M.transition_func;
	
	while(tr) {
		Trans* tr2 = tr->suivant;
		int skip = 0;
		
		while(tr2) {
			char* stat1 = tr->cinq_uplet[0];
			char* stat2 = tr2->cinq_uplet[0];
			char* letter1 = tr->cinq_uplet[1];
			char* letter2 = tr2->cinq_uplet[1];
			
			if( (strcmp(stat1, stat2)==0) && (   (strcmp(letter1, "a") == 0 && strcmp(letter2, "b") == 0)
											  || (strcmp(letter1, "b") == 0 && strcmp(letter2, "a") == 0)
											  || (strcmp(letter1, "c") == 0 && strcmp(letter2, "d") == 0)
											  || (strcmp(letter1, "d") == 0 && strcmp(letter2, "c") == 0)	) )
			{	
				if(tr2 != tr->suivant) {
					Trans* TR = tr;
					
					while(TR->suivant != tr2) {
						TR = TR->suivant;
					}
					TR->suivant = NULL;
					
					Trans* med = tr->suivant;
					tr->suivant = tr2;
				
					while(tr2->suivant) {
						tr2 = tr2->suivant;
					}
					tr2->suivant = med;
				}
				tr2 = NULL;
				skip = 1;
			}
			else {
				tr2 = tr2->suivant;
			}	
		}
		if(skip == 1) {
			tr = tr->suivant;
			skip = 0;
		}
		tr = tr->suivant;
	}
	
	tr = M.transition_func;

	FILE* f = fopen("Result_22.txt", "w+");
	
	int num = 1, trr = 2;
	char numb[100], trn[100];
	sprintf(numb, "%d", num);
	sprintf(trn, "%d", trr);
	
	char entry[100];
	
	entry[0] = '\0';
	strcat(entry, M.nom);
	fputs("name: ", f);
	fputs(entry, f);
	fputs("\ninit: ", f);
	entry[0] = '\0';
	strcat(entry, M.q_init);
	fputs(entry, f);
	fputs("\naccept: ", f);
	entry[0] = '\0';
	strcat(entry, M.q_final);
	fputs(entry, f);
	fputs("\n\n", f);	
	
	entry[0] = '\0';

	int a = 0;
	char d = '*';
	
	int skip  = 0;
	tr = M.transition_func;
	
	while(tr) {
		if( strcmp(tr->cinq_uplet[1], "a") == 0 ) {
			a = 1;
		}
		if( strcmp(tr->cinq_uplet[1], "b") == 0 ) {
			a = 2;
		}
		if( strcmp(tr->cinq_uplet[1], "c") == 0 ) {
			a = 3;
		}
		if( strcmp(tr->cinq_uplet[1], "d") == 0 ) {
			a = 4;
		}
		if( strcmp(tr->cinq_uplet[1], "_") == 0 ) {
			a = 5;
		}
		
		if( strcmp(tr->cinq_uplet[4], ">") == 0 ) {
			d = '>';
		}
		if( strcmp(tr->cinq_uplet[4], "<") == 0 ) {
			d = '<';
		}
		if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
			d = '-';
		}
		
		if( strcmp(tr->cinq_uplet[1], "a") == 0 ) {
			fputs(tr->cinq_uplet[0], f);
			fputs(", 0\n", f);
			fputs("ab", f);
			fputs(numb, f);
			fputs(", 0, >\n\n", f);

			fputs("ab", f);
			fputs(numb, f);
			fputs(", 0\n", f);
			fputs("qwrite1", f);
			fputs(numb, f);
			fputs(", 0, <\n\n", f);
				
			if(strcmp(tr->cinq_uplet[3], "a") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "b") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "c") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "d") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "_") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", _, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, -\n\n", f);
						
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, -\n\n", f);

				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, -\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			}
			if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);
			}
			if(tr->suivant && (strcmp(tr->suivant->cinq_uplet[0], tr->cinq_uplet[0]) == 0) && (strcmp(tr->suivant->cinq_uplet[1], "b") == 0)) {
				skip = 1;
				
				fputs("ab", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite1", f);
				fputs(trn, f);
				fputs(", 1, <\n\n", f);
				
				if(strcmp(tr->suivant->cinq_uplet[3], "a") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "b") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "c") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "d") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "_") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", _, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", _, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], ">") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, -\n\n", f);
						
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, -\n\n", f);

					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, -\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "<") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "-") == 0) {
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);
				}
			}
		}
		if( strcmp(tr->cinq_uplet[1], "b") == 0 ) {
			fputs(tr->cinq_uplet[0], f);
			fputs(", 0\n", f);
			fputs("ab", f);
			fputs(numb, f);
			fputs(", 0, >\n\n", f);

			fputs("ab", f);
			fputs(numb, f);
			fputs(", 1\n", f);
			fputs("qwrite1", f);
			fputs(numb, f);
			fputs(", 1, <\n\n", f);
			
			if(strcmp(tr->cinq_uplet[3], "a") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "b") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "c") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "d") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "_") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", _, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, -\n\n", f);
						
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, -\n\n", f);

				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, -\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			}
			if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);
			}
			if(tr->suivant && (strcmp(tr->suivant->cinq_uplet[0], tr->cinq_uplet[0]) == 0) && (strcmp(tr->suivant->cinq_uplet[1], "a") == 0)) {
				skip = 1;
				
				fputs("ab", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite1", f);
				fputs(trn, f);
				fputs(", 0, <\n\n", f);
				
				if(strcmp(tr->suivant->cinq_uplet[3], "a") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "b") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "c") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "d") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "_") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", _, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", _, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], ">") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, -\n\n", f);
						
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, -\n\n", f);

					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, -\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "<") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "-") == 0) {
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(numb, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);
				}
			}
		}
		
		///////////////// c et d
		
		if( strcmp(tr->cinq_uplet[1], "c") == 0 ) {
			fputs(tr->cinq_uplet[0], f);
			fputs(", 1\n", f);
			fputs("ab", f);
			fputs(numb, f);
			fputs(", 1, >\n\n", f);

			fputs("ab", f);
			fputs(numb, f);
			fputs(", 0\n", f);
			fputs("qwrite1", f);
			fputs(numb, f);
			fputs(", 0, <\n\n", f);
				
			if(strcmp(tr->cinq_uplet[3], "a") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "b") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "c") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "d") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "_") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", _, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, -\n\n", f);
						
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, -\n\n", f);

				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, -\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			}
			if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);
			}
			if(tr->suivant && (strcmp(tr->suivant->cinq_uplet[0], tr->cinq_uplet[0]) == 0) && (strcmp(tr->suivant->cinq_uplet[1], "d") == 0)) {
				skip = 1;
				
				fputs("ab", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite1", f);
				fputs(trn, f);
				fputs(", 1, <\n\n", f);
				
				if(strcmp(tr->suivant->cinq_uplet[3], "a") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "b") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "c") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "d") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "_") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", _, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", _, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], ">") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, -\n\n", f);
						
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, -\n\n", f);

					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, -\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "<") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "-") == 0) {
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);
				}
			}
		}
		if( strcmp(tr->cinq_uplet[1], "d") == 0 ) {
			fputs(tr->cinq_uplet[0], f);
			fputs(", 1\n", f);
			fputs("ab", f);
			fputs(numb, f);
			fputs(", 1, >\n\n", f);

			fputs("ab", f);
			fputs(numb, f);
			fputs(", 1\n", f);
			fputs("qwrite1", f);
			fputs(numb, f);
			fputs(", 1, <\n\n", f);
				
			if(strcmp(tr->cinq_uplet[3], "a") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "b") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "c") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "d") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "_") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", _, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, -\n\n", f);
						
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, -\n\n", f);

				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, -\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			}
			if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);
			}
			if(tr->suivant && (strcmp(tr->suivant->cinq_uplet[0], tr->cinq_uplet[0]) == 0) && (strcmp(tr->suivant->cinq_uplet[1], "c") == 0)) {
				skip = 1;
				
				fputs("ab", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qwrite1", f);
				fputs(trn, f);
				fputs(", 0, <\n\n", f);
				
				if(strcmp(tr->suivant->cinq_uplet[3], "a") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "b") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "c") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "d") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[3], "_") == 0) {
					fputs("qwrite1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", _, >\n\n", f);
					fputs("qwrite2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("check",f );
					fputs(trn, f);
					fputs(", _, >\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], ">") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, -\n\n", f);
						
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, -\n\n", f);

					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, -\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "<") == 0) {
					fputs("check",f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("qleft2", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);

					fputs("qleft3", f);
					fputs(trn, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
				}
				if(strcmp(tr->suivant->cinq_uplet[4], "-") == 0) {
					fputs("check",f );
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1, <\n\n", f);
			
					fputs("check",f );
					fputs(trn, f);
					fputs(", _\n", f);
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(numb, f);
					fputs(", _\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", _, <\n\n", f);
			
					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 0\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 0, <\n\n", f);

					fputs("qleft1", f);
					fputs(trn, f);
					fputs(", 1\n", f);
					fputs(tr->suivant->cinq_uplet[2], f);
					fputs(", 1, <\n\n", f);
				}
			}
		}
		
		//// Blank _
		
		if( strcmp(tr->cinq_uplet[1], "_") == 0 ) {
			fputs(tr->cinq_uplet[0], f);
			fputs(", _\n", f);
			fputs("ab", f);
			fputs(numb, f);
			fputs(", _, >\n\n", f);

			fputs("ab", f);
			fputs(numb, f);
			fputs(", _\n", f);
			fputs("qwrite1", f);
			fputs(numb, f);
			fputs(", _, <\n\n", f);
				
			if(strcmp(tr->cinq_uplet[3], "a") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "b") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "c") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "d") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[3], "_") == 0) {
				fputs("qwrite1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _, >\n\n", f);
				fputs("qwrite2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("check",f );
				fputs(numb, f);
				fputs(", _, >\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, -\n\n", f);
						
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, -\n\n", f);

				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, -\n\n", f);
			}
			if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				fputs("check",f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("qleft2", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);

				fputs("qleft3", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			}
			if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
				fputs("check",f );
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1, <\n\n", f);
			
				fputs("check",f );
				fputs(numb, f);
				fputs(", _\n", f);
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", _\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", _, <\n\n", f);
			
				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 0\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 0, <\n\n", f);

				fputs("qleft1", f);
				fputs(numb, f);
				fputs(", 1\n", f);
				fputs(tr->cinq_uplet[2], f);
				fputs(", 1, <\n\n", f);
			}
		}
		
		if(skip == 1) {
			tr = tr->suivant;
			skip = 0;
		}
		if(tr != NULL) {
			tr = tr->suivant;
		}	
		num = num+2;
		trr = trr+2;
		sprintf(numb, "%d", num);
		sprintf(trn, "%d", trr);		
	}
	
		
	free_every_fucking_thing(&M);
	free(MMM);
	fclose(desc);
	fclose(f);
	
	return 0;
}
