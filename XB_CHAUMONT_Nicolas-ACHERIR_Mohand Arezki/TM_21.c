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
		printf("Usage: ./TM_21 nomdufichier.txt");
		exit(-1);	
	}
	
	TM M;
	
	printf("CONVERSION DE BI-INIFINI A SEMI INFINI A DROITE ET LANCEMENT\n\n");

	FILE* desc = fopen(argv[1], "r");
	
	char word[100];
	int i = 0;
	
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
		
	
	//Partie conversion de Bi inifie a Semi infini a droite

	Trans* tr = M.transition_func;
	FILE* f = fopen("Result_21.txt", "w+");
	
	int num = 1;
	char numb[10];
	sprintf(numb, "%d", num);
	
	char entry[100];
	
	entry[0] = '\0';
	strcpy(entry, M.nom);

	fputs("name: ", f);
	fputs(entry, f);
	fputs("\ninit: q0\naccept: qAccept\n\n", f);
	fputs("q0, 0\nqinit, 0, -\n\n", f);
	fputs("q0, 1\nqinit, 1, -\n\n", f);
	fputs("q0, _\nqOU, _, -\n\n", f);
	fputs("qinit, 0\nqO, S, >\n\n", f);
	fputs("qinit, 1\nqI, S, >\n\n", f);
	fputs("qINIT, 0\nqO, _, >\n\n", f);
	fputs("qINIT, 1\nqI, _, >\n\n", f);
	fputs("qO, 0\nqO, 0, >\n\n", f);
	fputs("qO, 1\nqI, 0, >\n\n",f );
	fputs("qO, _\nqleft, 0, <\n\n", f);
	fputs("qleft, 0\nqleft, 0, <\n\n", f);
	fputs("qleft, 1\nqleft, 1, <\n\n", f);
	fputs("qleft, _\nqshift, _, >\n\n", f);
	fputs("qshift, 0\nqINIT, 0, >\n\n", f);
	fputs("qshift, 1\nqINIT, 1, >\n\n", f);
	fputs("qI, 0\nqO, 1, >\n\n", f);
	fputs("qI, 1\nqI, 1, >\n\n", f);
	fputs("qI, _\nqleft, 1, <\n\n", f);
	fputs("qleft, 0\nqleft, 0, <\n\n", f);
	fputs("qleft, 1\nqleft, 1, <\n\n", f);
	fputs("qleft, _\nqshift, _, >\n\n", f);
	fputs("qleft, S\nqshift, S, >\n\n", f);
	fputs("qinit, _\nqSet, _, <\n\n", f);
	fputs("qINIT, _\nqSet, _, <\n\n", f);
	fputs("qSet, 0\nqSet, 0, <\n\n", f);
	fputs("qSet, 1\nqSet, 1, <\n\n", f);
	fputs("qSet, _\nqSet, _, <\n\n", f);
	fputs("qSet, S\nqOU, S, >\n\n", f);

	fputs("qOU, 0\n", f);
	fputs(M.q_init, f);
	fputs("U, 0, -\n\n", f);

	fputs("qOU, 1\n", f);
	fputs(M.q_init, f);
	fputs("U, 1, -\n\n", f);

	fputs("qOU, _\n", f);
	fputs(M.q_init, f);
	fputs("U, _, -\n\n", f);

	fputs("qOU, S\n", f);
	fputs(M.q_init, f);
	fputs("U, S, -\n\n", f);
	
	while(tr) {
		entry[0] = '\0';
			
		strcat(entry, tr->cinq_uplet[0]);
		strcat(entry, "U, ");
		strcat(entry, tr->cinq_uplet[1]);
		strcat(entry, "\n");
		
		fputs(entry, f);
		
		entry[0] = '\0';
		
		if(tr->cinq_uplet[4][0] == '>') {
				// q0, 0             q0, 0          qright,  // q0, 0
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", >\n\n");											 // q1, 0, >          qright, 0, >
				fputs(entry, f);
				
				entry[0] = '\0';
				//write(f, "qrighti, "+tr->cinq_uplet[3]+", >\n");
				//write(f, "qrighti, X, tr->cinq_uplet[2]U, X, >");
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U, ");
				strcat(entry, "0, >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U, ");
				strcat(entry, "1, >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U, ");
				strcat(entry, "_, >\n\n");
				fputs(entry, f);	
		}
		if(tr->cinq_uplet[4][0] == '<') {
				//write(f, "qlefti, "+tr->cinq_uplet[3]+", <\n");
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				
				//write(f, "qlefti, S, qrighti, S, >");
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", S\n");
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", S, ");
				strcat(entry, ">\n\n");
				fputs(entry, f);
				
				//write(f, "qrighti, X, tr->cinq_uplet[2]L, >");
				entry[0] = '\0';
	
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 0");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
	
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 1");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
	
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", _");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
								
				//write(f, "qlefti, nonS, tr->cinq_uplet[2]U, <");
				entry[0] = '\0';
	
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U");
				strcat(entry, ", 0");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U");
				strcat(entry, ", 1");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U");
				strcat(entry, ", _");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
		}
		if(tr->cinq_uplet[4][0] == '-') {
				//write(f, "tr->cinq_uplet[2]U, tr->cinq_uplet[3], -");
				entry[0] = '\0';
	
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U, ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", -\n\n");
				fputs(entry, f);
		}
		
		//write(f, tr->cinq_uplet[0]"L" + "," + tr->cinq_uplet[1] + "\n");
		entry[0] = '\0';
		strcat(entry, tr->cinq_uplet[0]);
		strcat(entry, "L, ");
		strcat(entry, tr->cinq_uplet[1]);
		strcat(entry, "\n");
		fputs(entry, f);
		

		if(strcmp(tr->cinq_uplet[4], "<") == 0) {
				
				// q0, 0             q0, 0          qright,  // q0, 0
															 // q1, 0, >          qright, 0, >
				
				//write(f, "qrighti, "+tr->cinq_uplet[3]+", >\n");
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", >\n\n");											 // q1, 0, >          qright, 0, >
				fputs(entry, f);				
				
				//write(f, "qrighti, X, tr->cinq_uplet[2]L, X, >");
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", _");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 0");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qright");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 1");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
		}
		if(strcmp(tr->cinq_uplet[4], ">") == 0) {
				//write(f, "qlefti, "+tr->cinq_uplet[3]+", <\n");
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", <\n\n");											 // q1, 0, >          qright, 0, >
				fputs(entry, f);
								
				//write(f, "qlefti, X, qcheckSi, X, <");
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", _");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", 0");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qleft");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", 1");
				strcat(entry, ", <\n\n");
				fputs(entry, f);
				//write(f, "qcheckSi, S, tr->cinq_uplet[2]U, >");
				entry[0] = '\0';
				
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", S\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "U");
				strcat(entry, ", S");
				strcat(entry, ", >\n\n");
				fputs(entry, f);
				//write(f, "qcheckSi, nonS, tr->cinq_uplet[2]L, -");
				entry[0] = '\0';
				
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", _\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", _");
				strcat(entry, ", -\n\n");
				fputs(entry, f);

				entry[0] = '\0';
				
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", 0\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 0");
				strcat(entry, ", -\n\n");
				fputs(entry, f);
				
				entry[0] = '\0';
				
				strcat(entry, "qcheckS");
				strcat(entry, numb);
				strcat(entry, ", 1\n");
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L");
				strcat(entry, ", 1");
				strcat(entry, ", -\n\n");
				fputs(entry, f);
				
		}
		if(strcmp(tr->cinq_uplet[4], "-") == 0) {
				//write(f, "tr->cinq_uplet[2]L, tr->cinq_uplet[3], -");
				entry[0] = '\0';
				
				strcat(entry, tr->cinq_uplet[2]);
				strcat(entry, "L, ");
				strcat(entry, tr->cinq_uplet[3]);
				strcat(entry, ", -\n\n");
				fputs(entry, f);
		}
		tr = tr->suivant;
		num++;
		sprintf(numb, "%d", num);
	}
	
	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "U, 0\nqAccept, 0, -\n\n");
	fputs(entry, f);

	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "U, 1\nqAccept, 1, -\n\n");
	fputs(entry, f);

	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "U, _\nqAccept, _, -\n\n");
	fputs(entry, f);

	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "L, 0\nqAccept, 0, -\n\n");
	fputs(entry, f);

	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "L, 1\nqAccept, 1, -\n\n");
	fputs(entry, f);

	entry[0] = '\0';	
	strcat(entry, M.q_final);
	strcat(entry, "L, _\nqAccept, _, -\n\n");
	fputs(entry, f);
	
	free_every_fucking_thing(&M);
	fclose(desc);
	fclose(f);
	
	return 0;
}
