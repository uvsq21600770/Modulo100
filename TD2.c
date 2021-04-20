#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

/*
int arriveGratuit(float p0, float p2)
{
    double U = unif();

    if(U < p0) return 0;
    if(U < p0 + p2) return 2;
    else return 1;
}*/


// Simulation d'une file en M/M/1



// Generateur aleatoire d'arrivees
#define RM ((double)((1L<<31) - 1))

double unif()
{
    return (double)random()/RM;
}

double loi_exp(double lambda)
{
    return -log(unif())/lambda;
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
#define DS 2
#define FS 3
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

    E->suiv = ajouter_evenement(E->suiv, date, nom_event);
    return E;
}

// Les variables globales, ont a pas le time
    double T = 0; // Date de simulation
    unsigned long int N = 0; // Nombre de client dans la file
    ECHEANCIER E = NULL;     // Echenancier

    double last;
    double S;

void traiter_AC(double t, double lambda)
{

    S = S + (t-last)*N;
    last = t;
    N++; // Les arrivees
    E = ajouter_evenement(E, t+loi_exp(lambda), AC);
    if(N == 1) E = ajouter_evenement(E, t, DS);
}

void traiter_DS(double t, double mu)
{

    E = ajouter_evenement(E, t+loi_exp(mu), FS);
}

void traiter_FS(double t)
{
    S = S + (t-last)*N;
    last = t;
    N = N - 1;
    if(N > 0) E = ajouter_evenement(E, t, DS);
}


//Fonction de simulation
#define TMAX 1e6
void simul (double lambda, double mu)
{

    // Initialisation
    T = 0.0; // Date de simulation
    N = 0; // Nombre de client dans la file
    E = ajouter_evenement(E, 0, AC);
    struct event *e;

    last = 0.0;
    S = 0.0;
    unsigned long int i = 0;
    while(T < TMAX)
    {   
        e = premier_evenement(&E);
        T = e->date;
        if (e->nom_event == AC) traiter_AC(T, lambda);
        if (e->nom_event == DS) traiter_DS(T, mu);
        if (e->nom_event == FS) traiter_FS(T);
        if(!i && T) printf("%lf %lf\r", T, S/T);
        fflush(stdout);
        free(e);
        i = (i+1) % 10000;
    }
    printf("\n%10.2le %lf\r", T, S/T);

}

int main()
{
    unsigned int seed = getpid();
    //srandom(seed);
    
    double lambda, mu;
    lambda = 0.8;
    mu = 1.0;
    simul(lambda, mu);
    
    return 0;
}