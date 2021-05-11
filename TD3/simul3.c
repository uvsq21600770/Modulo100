#include "stdio.h"
#include "stdlib.h"
#include "math.h"

// ## 1. Les générateurs aléatoires
double unif() {
	return (double)random()/((double)RAND_MAX);
}	
	

double expo (double L) {
	return -log(unif())/L;
}	

// ## 2. Gestion de l'échéancier
struct evenement {
	int le_type;
	double la_date;
	struct evenement * suiv;
};
typedef struct evenement * ECHEANCIER;


// ## 3. La simulation
#define AC 1
#define DS 2
#define FS 3
// ### EXO 3
#define NB_EVENT_MAX 1e8
// ### EXO 4
//#define NB_EVENT_MAX 1e9
#define DEBUG1
#define EPSILON 1e-4
struct evenement * nouveau_evenement (int le_type, double la_date) {
	struct evenement *e;
	e = malloc(sizeof(struct evenement));
	if (!e) exit(1);
	e->le_type = le_type;
	e->la_date = la_date;
	e->suiv = NULL;
	return e;
}

struct evenement * recup_premier_evenement (ECHEANCIER *E) {
	struct evenement *e = *E;
	*E = (*E)->suiv;
	return e;
}

ECHEANCIER inserer_evenement(struct evenement *e, ECHEANCIER E) {
	if ((E==NULL) || (e->la_date < E->la_date)) {
		e->suiv = E;
		return e;
	}
	E->suiv = inserer_evenement(e,E->suiv);
	return E;
}

double simul_MM1 (double lambda, double mu, int *converge) {
	ECHEANCIER E = nouveau_evenement (AC,0.0);
	unsigned long int N = 0; // Nombre de clients dans la file
	double T = 0.0; // La date courante
	unsigned long int nb_event = 0;
	double S = 0.0;
	double lastT = 1e-6;
	unsigned long int nb_e = 0;
	double max = 0.0;
	double min = 0.0;
	double nbmoy = 0;
	double L_MANCHON = 1e3;
	*converge=0;
	printf("### SIMUL %.3lf %.3lf\n",lambda,mu);
	while ((nb_event < NB_EVENT_MAX) && (!(*converge))) {
		struct evenement *e = recup_premier_evenement (&E);
		if (e->le_type==AC) {
			T = e->la_date;
			S = S + N*(T-lastT);
			lastT = T;
			N++;
			E = inserer_evenement(nouveau_evenement(AC,T+expo(lambda)),E);
			if (N==1) E = inserer_evenement(nouveau_evenement(DS,T),E);
		}
		if (e->le_type==DS) {
			T = e->la_date;
			E = inserer_evenement(nouveau_evenement(FS,T+expo(mu)),E);
		}
		if (e->le_type==FS) {
			T = e->la_date;
			S = S + N*(T-lastT);
			lastT = T;
			N--;
			if (N>0) E = inserer_evenement(nouveau_evenement(DS,T),E);
		}
		nb_event++;
		nb_e++;
		nbmoy = S/lastT;
		if (nbmoy>max) max = nbmoy;
		if (nbmoy<min) min = nbmoy;
		if (nb_e>L_MANCHON) { // on a atteint la fin du manchon
			if (max-min < nbmoy*EPSILON) *converge = 1;
			else {	nb_e = 0; max = min = nbmoy;
					L_MANCHON = 1e3*nbmoy;
				}
		}
		free(e);
#ifdef DEBUG1
if (nb_event % 1000000==0) {
	printf("%.3le %lu %.3le || %lf < %lf < %lf\r",T,N,(double)nb_event,min,S/lastT,max);
	fflush(stdout);
	}
#endif
	}
#ifdef DEBUG1
printf("%.3le %lu %.3le || %lf < %lf < %lf\r",T,N,(double)nb_event,min,S/lastT,max);
printf("\n");
#endif
	return S/lastT;
}

int main () {
	FILE *F;
	F = fopen("mm1_3.data","w");
	double lambda;
	double mu = 1.0;
	double nbmoy;
	int converge = 0;
// ### EXO 3
	for (lambda = 0.025 ; lambda < 1.2; lambda += 0.05) {
		nbmoy = simul_MM1 (lambda,mu,&converge);
		if (converge) fprintf(F,"%lf %lf\n",lambda/mu,nbmoy);
			else printf("Pas de convergence\n");
	}
// ### EXO 4
/*
	int i;
 	lambda = 0.1;
	for (i=0 ; i<10 ; i++) {
		printf("### lambda = %lf\n",lambda);
		nbmoy = simul_MM1 (lambda,mu,&converge);
		if (converge) fprintf(F,"%lf %lf\n",lambda/mu,nbmoy);
			else printf("Pas de convergence\n");
		lambda = lambda + (1.0-lambda)/2.0;
	}
*/
	fclose(F);
}

