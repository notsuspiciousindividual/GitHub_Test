#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

int g_var1 = 0;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

void *inc_gv()
{
    int i, j;
    for (i=0; i<10; i++)
    {
        pthread_mutex_lock(&mutex1);
        g_var1++;
        for(j=0; j<5; j++)
        {
            printf(" %d", g_var1);
            //fflush(stdout);
        }
        pthread_mutex_unlock(&mutex1);

    }

}

int main()
{
    /*
        This is to show that if we don't do any form of mutex for our critical section, it will not work
    */

    pthread_t TA, TB;
    int TAret, TBret;

    TAret = pthread_create(&TA, NULL, inc_gv, NULL);
    TBret = pthread_create(&TB, NULL, inc_gv, NULL);

    pthread_join(TA, NULL);
    pthread_join(TB, NULL);

    printf("\nT1 & T2 return: %d, %d\n", TAret, TBret);
    
    return 0;


}