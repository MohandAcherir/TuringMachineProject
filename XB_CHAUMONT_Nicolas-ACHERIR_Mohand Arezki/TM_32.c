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
		exit(-1);	
	}
	TM M;
	Q_A* couples = NULL;

	printf("SUPPRESSION DES TRANSITIONS INATTEIGNABLES\n\n");
	
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
		
	
	//Suppression des transitions inatteignables

	Trans* tr = M.transition_func;

	
	Q_A* curr_l = couples;
	Q_A* Start = couples;
	
	while(curr_l) {
		Start = couples;
		tr = M.transition_func;
		
		while(tr) {
			Start = couples;
			
			if( (strcmp(curr_l->q_a[0], tr->cinq_uplet[0]) == 0) && (strcmp(curr_l->q_a[1], tr->cinq_uplet[1]) == 0) ) {
					if( (strcmp(tr->cinq_uplet[4], ">") == 0) || (strcmp(tr->cinq_uplet[4], "<") == 0) ) {
				
					
						if(is_in(couples, tr->cinq_uplet[2], "0") == 0) {
							Q_A* nnww = (Q_A*)malloc(sizeof(Q_A));
							nnww->q_a[0] = (char*)malloc(sizeof(char)*(strlen(tr->cinq_uplet[2])+1));
							nnww->q_a[1] = (char*)malloc(sizeof(char)+1);
							strcpy(nnww->q_a[0], tr->cinq_uplet[2]);
							strcpy(nnww->q_a[1], "0");
							nnww->suivant = NULL;
							get_tailqa(Start)->suivant = nnww;
						}
						if(is_in(couples, tr->cinq_uplet[2], "1") == 0) {
							Q_A* nnww = (Q_A*)malloc(sizeof(Q_A));
							nnww->q_a[0] = (char*)malloc(sizeof(char)*(strlen(tr->cinq_uplet[2])+1));
							nnww->q_a[1] = (char*)malloc(sizeof(char)+1);
							strcpy(nnww->q_a[0], tr->cinq_uplet[2]);
							strcpy(nnww->q_a[1], "1");
							nnww->suivant = NULL;
							get_tailqa(Start)->suivant = nnww;
						}
						if(is_in(couples, tr->cinq_uplet[2], "_") == 0) {
							Q_A* nnww = (Q_A*)malloc(sizeof(Q_A));
							nnww->q_a[0] = (char*)malloc(sizeof(char)*(strlen(tr->cinq_uplet[2])+1));
							nnww->q_a[1] = (char*)malloc(sizeof(char)+1);
							strcpy(nnww->q_a[0], tr->cinq_uplet[2]);
							strcpy(nnww->q_a[1], "_");
							nnww->suivant = NULL;
							get_tailqa(Start)->suivant = nnww;
						}
					}
					if( strcmp(tr->cinq_uplet[4], "-") == 0 ) {
						if(is_in(couples, tr->cinq_uplet[2], tr->cinq_uplet[3]) == 0) {
							Q_A* nnww = (Q_A*)malloc(sizeof(Q_A));
							nnww->q_a[0] = (char*)malloc(sizeof(char)*(strlen(tr->cinq_uplet[2])+1));
							nnww->q_a[1] = (char*)malloc(sizeof(char)+1);
							strcpy(nnww->q_a[0], tr->cinq_uplet[2]);
							strcpy(nnww->q_a[1], tr->cinq_uplet[3]);
							nnww->suivant = NULL;
							get_tailqa(Start)->suivant = nnww;
						}					
					}
			}
			tr = tr->suivant;
		}
		curr_l = curr_l->suivant;
	}
	

	tr = M.transition_func;

	
	FILE* r_32 = fopen("Result_32.txt", "w+");
	
	fputs("name: ", r_32);
	fputs(M.nom, r_32);
	fputs("\n", r_32);

	fputs("init: ", r_32);
	fputs(M.q_init, r_32);
	fputs("\n", r_32);

	fputs("accept: ", r_32);
	fputs(M.q_final, r_32);
	fputs("\n\n", r_32);

	tr = M.transition_func;
	
	while(tr) {
		if(is_in(couples, tr->cinq_uplet[0], tr->cinq_uplet[1]) == 1) {		
			char* stat1 = tr->cinq_uplet[0];
			char* stat2 = tr->cinq_uplet[2];
			char* letter1 = tr->cinq_uplet[1];
			char* letter2 = tr->cinq_uplet[3];
			char* direction = tr->cinq_uplet[4];
			fprintf(r_32, "%s, %s\n%s, %s, %s\n\n", stat1, letter1, stat2, letter2, direction);
		}
		tr = tr->suivant;
	}
	/*
transition jamais utlisee <==> non atteignable depuis les transitions intiales: q0, 0 ; q0, 1 ; q0, _ <==>  q0->qi/j....?

*/
		
	free_every_fucking_thing(&M, couples);
	fclose(desc);
	fclose(r_32);
	
	return 0;
}
