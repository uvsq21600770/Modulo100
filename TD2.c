#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
/*
double exp(double lambda)
{
    return -log(unif())/lambda;
}

int arriveGratuit(float p0, float p2)
{
    double U = unif();

    if(U < p0) return 0;
    if(U < p0 + p2) return 2;
    else return 1;
}*/

// Simulation d'une file en TD
// A chaque top - 1 service
//              - 0, 1 ou 2 arrivees

// Generateur aleatoire d'arrivees
#define RM ((double)((1L<<31) - 1))

double unif()
{
    return (double)random()/RM;
}

int generer_arrivees(double p0, double p1, double p2)
{
    double X = unif();

    if(X < p0) return 0;
    if(X < p0 + p1) return 1;
    else return 2;
}

// Fonction de gestion de l'echeancier
#define AC 1
struct event{
    double date;
    int nom_event; //AC
    struct event* suiv;
};
typedef struct event* ECHEANCIER;

struct event *premier_evenement(ECHEANCIER *E)
{
    if (*E==NULL) exit(2);

    struct event *premier = *E;
    *E = (*E)->suiv;

    return premier;
}

ECHEANCIER ajouter_evenement(ECHEANCIER E, double date, int nom_event)
{
    if ((E==NULL) || (date < E->date))
    {
        struct event* e = malloc(sizeof(struct event));
        if (e==NULL) exit(3);

        e->date = date;
        e->nom_event = nom_event;
        e->suiv = E;
        E = e;

        return E;
    }

    E = ajouter_evenement(E->suiv, date, nom_event);
    return E;
}

#define DANSFIC
//Fonction de simulation
#define TMAX 1e5
double simul (double p0, double p1, double p2)
{
#ifdef DANSFIC    
    FILE *F = fopen("file.data", "w");
    if(F == NULL) exit(4);
#endif

    // Les variables
    unsigned long int T = 0; // Date de simulation
    unsigned long int N = 0; // Nombre de client dans la file
    double S = 0.0; // Somme du nombre client

    ECHEANCIER E = NULL;
    E = ajouter_evenement(E, 0, AC);
    struct event *e;

    while(T < TMAX)
    {        
        e = premier_evenement(&E);
        T = e->date;
#ifdef DANSFIC  
fprintf(F, "%lu %lu %lf\n", T, N, S/(T+1));
#endif
        if (e->nom_event == AC) {
            N = N + generer_arrivees(p0, p1, p2); // Les arrivees
            S = S + N;
            if(N > 0) N = N-1; // Le service

            E = ajouter_evenement(E, T+1, 1);
        }
    }
#ifdef DANSFIC      
    fclose(F);
#endif
    return S/(T+1);
}

int main()
{
    unsigned int seed = getpid();
    srandom(seed);
    double nbmoy;
    double p0, p1, p2;
    p0 = 0.5;
    for(p2 = 0.0; p2 < 0.9; p2+=0.05)
    {
        p1 = 1.0 - p0 - p2;
        nbmoy = simul(p0, p1, p2);
    printf("%lf %lf %lf\n",p2 , p1+2*p2, nbmoy);
    }
    
    return 0;
}