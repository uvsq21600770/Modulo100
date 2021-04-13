#include <stdio.h> 
#include <stdlib.h> 
#include <sys/types.h>
#include <unistd.h>

#define RM ((double)((1L<<31) - 1))

double unif()
{
    return (double)random()/RM;
}

int Bernoulli(float p)
{
    printf("p = %f --- ", p);

    double res_double= unif();

    if(res_double < p)
    {
        return 1;
    }

    return 0;
}

int BernoulliGratuit(float p)
{
    return unif()<p;
}

int arriveGratuit(float p0, float p2)
{
    double U = unif();

    if(U < p0) return 0;
    if(U < p0 + p2) return 2;
    else return 1;
}

int nb_arrivees(float p0, float p2)
{
    printf("p = %f --- p1 = %f --- p2 = %f  => ", p0, 1-p0-p2, p2);

    long int res;
    res = random()%100;
    double res_double= (double)res / 100;

    if(res_double < 1 - p0 - p2)
    {
        return 0;
    }
    else if(res_double < p0)
    {
        return 1;
    }
    else return 2;

}

int main()
{
    long int res = 0;
    unsigned int seed = getpid();
    printf("Seed: <%d>\n", seed);

    //srandom(seed);
    srandom(1);
    for(int i = 0; i < 10; i++)
    {
        res = random();
        printf("%ld\n", res);
    }

    printf("\nDans [0..99]\n");
    for(int i = 0; i < 10; i++)
    {
        res = random()%100;
        printf("%ld\n", res);
    }

    printf("\nDans avec unif [0..99]\n");
    for(int i = 0; i < 10; i++)
    {
        res = unif()*100;
        printf("%ld\n", res);
    }

    printf("\nP(0) = 0.7 et P(1) = 0.3\n");
    for(int i = 0; i < 10; i++)
    {
        res = random()%10;
        if(res < 7)
        {
            printf("0\n");
        }
        else{
             printf("1\n");
        }
    }

    printf("\nTest Bernoulli\n");
    float p = 0;
    for(int i = 0; i < 10; i++)
    {
        p = (float)i / 10;
        printf("%d\n", Bernoulli(p));
    }

    printf("\nTest nb_arrivee\n");
    float p0 = 0.3;
    float p2 = 0.5;
    printf("<%d>\n",nb_arrivees(p0, p2));
    

    printf("\nRepartition de 10000 pulls\n");
    int T[10];
    for(int i = 0; i < 10; i++)
    {
        T[i] =0;
    }
    double res_d;
    int j = 0;
    for(int i = 0; i < 10000; i++)
    {
        res_d = unif();
        j = 10*res_d;
        (T[j])++;
    }

    for(int i = 0; i < 10; i++)
    {
        printf("T[%d] = %d\n", i, T[i]);
    }
}