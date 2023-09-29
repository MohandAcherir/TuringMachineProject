#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef struct TM TM;
typedef struct Etat Etat;
typedef struct Trans Trans;
typedef struct Q_A Q_A;

struct Etat {
	char nom[100];
	struct Etat* suivant;	
};

struct Trans {
	char* cinq_uplet[5];
	struct Trans* suivant;
};

struct Q_A {
	char* q_a[2];
	struct Q_A* suivant;
};

struct TM  {
	char nom[100];
	Etat* etats;
	char alphabet[2];
	char alphabet_travail[3];
	char q_init[100];
	char* q_final;
	Trans* transition_func;
	
	
	char etat_courant[10];
	char bande[10000];
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

Q_A* get_tailqa(Q_A* q_a) {
	while(q_a->suivant) {
		q_a = q_a->suivant;
	}
	return q_a;
}

void free_couples(Q_A* c) {
	while(c) {
		free(c->q_a[0]);
		free(c->q_a[1]);
		
		Q_A* tmp = c;
		c = c->suivant;
		free(tmp);
	}
}

void free_every_fucking_thing(TM* M, Q_A* c) {
	free(M->q_final);
	free_etats(M->etats);
	free_trans(M->transition_func);
	free_couples(c);
}

int is_in(Q_A* couples, char* q, char* a) {
	while(couples) {
		if( (strcmp(couples->q_a[0], q) == 0) && (strcmp(couples->q_a[1], a) == 0) ) {
			return 1;
		}
		couples = couples->suivant;
	}
	return 0;
}

int main(int argc, char** argv) {
	
	if(argc < 2) {
		printf("usage: ./launch nomfichier.txt");
		exit(-1);	
	}

	TM M;
	Q_A* couples = NULL;

	printf("MACHINE EXECUTION\n\n");
	
	FILE* desc = fopen(argv[1], "r");
	
	char word[100];
	int i = 0, tr_c = 0;
	
	int v = 0, tour = 0;
	
	int n = 0, q_i = 0, q_f = 0;
	int ti = 0;
	
	int nb_q = 0, nb_t = 0, end_of_trans = 0;
	
	char c;
	
	while((c = getc(desc)) != EOF)	{
		if((isalpha(c)) || isdigit(c) || (c == '_') || (c == '>') || (c == '<') || (c == '-') || (c == '0') || (c == '1')){ // Assemblage du mon de l'etat
			if(c == '<' || c == '>' || c == '-') {                                                            // ou de la lettre
				end_of_trans = 1;                             												  // ou de la direction
			}
			v = 1; //pour dire aux 'if' ci-dessous que les lettres mises dans 'word' ne sont pas autres que 0, 1, _, <, >
				   //  car dans le fichier a lire contient des virgules et des sauts a la ligne
			  
			word[i] = c; // remplir word
			i++;
			word[i] = '\0';
		}
		if(!( (isalpha(c)) || (isdigit(c)) || (c == '_') || (c == '0') || (c == '1') ) || (end_of_trans == 1) ) {
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
								
								if(strcmp(M.q_init, word) == 0) {
									tr_c = 1;
									
									couples = (Q_A*)malloc(sizeof(Q_A));
									couples->suivant = NULL;
								
									couples->q_a[0] = (char*)malloc(sizeof(char)*(strlen(word)+1));
									strcpy(couples->q_a[0], word);
								}
							}
							else {
								Trans* nw = (Trans*)malloc(sizeof(Trans));
									
								nw->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
								strcpy(nw->cinq_uplet[ti], word);
								nw->suivant = NULL;
								
								append_trans(M.transition_func, nw);
								
								
								if(strcmp(M.q_init, word) == 0) {
									tr_c = 1;
									
									Q_A* nnww = (Q_A*)malloc(sizeof(Q_A));
									nnww->q_a[0] = (char*)malloc(sizeof(char)*(strlen(word)+1));
									nnww->suivant = NULL;
									strcpy(nnww->q_a[0], word);
								
									get_tailqa(couples)->suivant = nnww;
								}	
							}
						}		
						else if(ti%5 == 1) {
							ti = 1;
							
							get_tail(M.transition_func)->cinq_uplet[ti] = (char*)malloc(sizeof(char)*(strlen(word)+1));
							strcpy(get_tail(M.transition_func)->cinq_uplet[ti], word);
							
							if(tr_c == 1) {
								get_tailqa(couples)->q_a[1] = (char*)malloc(sizeof(char)*(strlen(word)+1));
								strcpy(get_tailqa(couples)->q_a[1], word);
								tr_c = 0;
							}
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
	

	// Lancement
	M.etat_courant[0] = '\0';
	M.bande[0] = '\0';
	M.bande[9999] = '\0';
	
	char input[1000];
	input[0] = '\0';
	
	printf("Nom : %s\n\n", M.nom);
	printf("Saisissez l'entree : ");
	scanf("%s", input);
	
	strcpy(M.etat_courant, M.q_init);
	strcpy(M.bande, input);

	printf("Input: %s\n", input);
	
	for(int i = 0 ; i < strlen(input) ; i++) {
		if(M.bande[i] != '0' && M.bande[i] != '1')
			M.bande[i] = '_';
	}

	if(strlen(input) < 9999) {
		M.bande[strlen(input)] = 'F';
	}
	
	M.pos_tete = 0;
	
	printf("Etat actuel : %s\n", M.etat_courant);
	printf("Bande : ");
	
	int g = 0;		
	while(g< 9999 && M.bande[g] != 'F') {
		printf("%c", M.bande[g]);
		g++;
	}
	printf("\nPosition : %d\n\n", M.pos_tete);
	
	Trans* tr = NULL;
	
	tr = M.transition_func;
	
	while( (strcmp(M.etat_courant, M.q_final) != 0) && (strcmp(M.etat_courant, "NULL") != 0) ) {
		tr = M.transition_func;
		int fnd = 0;
		
		while(tr && !fnd) {
			if( (strcmp(tr->cinq_uplet[0], M.etat_courant) == 0) && ( (tr->cinq_uplet[1][0] == M.bande[M.pos_tete]) || (M.pos_tete < 9999 && tr->cinq_uplet[1][0] == '_' && M.bande[M.pos_tete] == 'F')) ) {
				if(M.pos_tete+1 < 9999 && M.bande[M.pos_tete] == 'F') {
					M.bande[M.pos_tete+1] = 'F';
				}
				M.bande[M.pos_tete] = tr->cinq_uplet[3][0];
				strcpy(M.etat_courant, tr->cinq_uplet[2]);
				
				if(tr->cinq_uplet[4][0] == '>' && (M.pos_tete < 9999)) {
					M.pos_tete++;
				}
				if((tr->cinq_uplet[4][0] == '<') && (M.pos_tete > 0)) {
					M.pos_tete--;
				}
				fnd = 1;
			}
			tr = tr->suivant;
		}
		
		if(fnd == 0) {
			strcpy(M.etat_courant, "NULL");
		}
		printf("Etat actuel : %s\n", M.etat_courant);
		printf("Bande : ");
		
		g = 0;		
		while(g < 9999 && M.bande[g] != 'F') {
			printf("%c", M.bande[g]);
			g++;
		}

		printf("\nPosition : %d\n\n", M.pos_tete);
	}
	
	if(strcmp(M.etat_courant, "NULL") != 0) {
		printf("ACCEPT\n\n");
	}
	else {
		printf("REJECT\n\n");
	}
	
	free_every_fucking_thing(&M, couples);
	fclose(desc);

	return 0;
}
