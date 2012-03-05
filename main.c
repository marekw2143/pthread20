#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

//
// Number of threads.
//
#define N 2
#define START_BALANCE 100
#define MAX_PAYMENT 50
#define MAX_PER_USER_ITERATIONS 1000000


int balance; // total amount of money
int money_takers;
pthread_mutex_t mutex;
pthread_cond_t cond;
void add_money(int amount, int thread_id)
{
    printf("thread %d enters add_money with amount=%d\n", thread_id, amount);
    
    pthread_mutex_lock(&mutex);
    balance += amount;
    pthread_mutex_unlock(&mutex);
    
    printf("balance: %d\n", balance);

    printf("before thread %d signaling money added\n", thread_id);

    pthread_cond_signal(&cond);

    printf("after thread %d signaling money added\n", thread_id);
}

void take_money(int amount, int thread_id)
{
    printf("thread %d wants to take %d money\n", thread_id, amount);

    pthread_mutex_lock(&mutex);
    while(!enoutht_payment(amount))
    {
        printf("thread %d must wait for enought money, now there's: %d, and thread wants %d\n", thread_id, balance, amount);
        pthread_cond_wait(&cond, &mutex);
    }
    
    balance -= amount;
    pthread_mutex_unlock(&mutex);
    
    printf("thread %d removed %d from balance. now balance is: %d\n", thread_id, amount, balance);
}

void bank_user(void *args)
{
    int i = MAX_PER_USER_ITERATIONS;
    int amount;
    int thread_number = *((int *)args);

    printf("here's thread %d\n", thread_number);
    while(i--)
    {
        //pthread_mutex_lock(&mutex);

        amount = rand() % MAX_PAYMENT;
        if(rand()%2)
            add_money(amount, thread_number);
        else
        {
            printf("money takers: %d\n", money_takers);
            // if all other threads are waiting for more money - just add money or there'll be deadlock
            if(money_takers == N-1)
            {
                printf("thread %d wanted to take money but all other threads were waiting on adding money, so add_money will be called\n", thread_number);
                add_money(amount, thread_number);
                continue;
            }
            money_takers++;
            take_money(amount + 20, thread_number);
            money_takers--;
        }

    }
}
int enoutht_payment(int amount)
{
    return balance >= amount ? 1 : 0;
}

void init()
{
    balance = START_BALANCE;
    money_takers = 0;

    srand((unsigned)time(NULL));  
    
    if(pthread_mutex_init(&mutex, NULL) != 0)
    {
        printf("error pthread_mutex_init");
        exit(1);
    }

    if(pthread_cond_init(&cond, NULL) != 0)
    {
        printf("error pthread_cond_init");
        exit(1);
    }
}


int main(int argc, char **argv)
{
    int i;
    int *t_num;
    pthread_t threads[N];
    init();
    for(i=0;i<N;i++)
    {
        t_num = (int *)malloc(sizeof(int));
        *t_num = i;
        pthread_create(&threads[i], NULL, (void *)bank_user, (void *)t_num);
    }

    for(i=0;i<N;i++)
    {
        pthread_join(threads[i], NULL);
    }
    return 0;
}
