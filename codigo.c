#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>

// Numero de filosofos
#define N 5

// Numero de ciclos de simulacion
#define RONDAS 5

// Estados de los filosofos
#define THINKING 0
#define HUNGRY 1
#define EATING 2

// Variables globales
int state[N];

// Mutex para proteger los estados
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// Variable de condicion para cada filosofo
pthread_cond_t s[N];

// Prototipos
void test(int i);
void take_forks(int i);
void put_forks(int i);
void think(int i, unsigned int *seed);
void eat(int i, unsigned int *seed);
void *philosopher(void *arg);

// Programa principal
int main() {

	pthread_t philosophers[N];
	int id[N];

	// Inicializar estados y variables de condicion
	for (int i = 0; i < N; i++) {
		state[i] = THINKING;
		pthread_cond_init(&s[i], NULL);
	}

	printf("====================================\n");
	printf(" PROBLEMA DE LA CENA DE LOS FILOSOFOS\n");
	printf("====================================\n\n");

    // Crear los cinco hilos
	for (int i = 0; i < N; i++) {
		id[i] = i;
		if (pthread_create(&philosophers[i], NULL, philosopher, &id[i]) != 0) {
			perror("Error al crear hilo");
			return 1;
		}
	}

	// Esperar a que terminen los filosofos
	for (int i = 0; i < N; i++) {
		pthread_join(philosophers[i], NULL);
	}

	// Liberar recursos
	for (int i = 0; i < N; i++) {
		pthread_cond_destroy(&s[i]);
	}

	pthread_mutex_destroy(&mutex);
	printf("\nTodos los filosofos han terminado.\n");
	return 0;
}

// Funcion para comprobar si un filosofo puede comer
void test(int i) {

	if (state[i] == HUNGRY && state[(i + N - 1) % N] != EATING && state[(i + 1) % N] != EATING) {
		state[i] = EATING;
		pthread_cond_signal(&s[i]);
	}
}

// El filosofo solicita los tenedores
void take_forks(int i) {

	pthread_mutex_lock(&mutex);
	state[i] = HUNGRY;
	printf("Filosofo %d tiene hambre.\n", i);
	test(i);

	// Espera hasta que pueda comer
	while (state[i] != EATING) {
		pthread_cond_wait(&s[i], &mutex);
	}
	pthread_mutex_unlock(&mutex);
}

// El filosofo libera los tenedores
void put_forks(int i) {

	pthread_mutex_lock(&mutex);
	state[i] = THINKING;
	printf("Filosofo %d deja de comer.\n", i);

	// Comprobar si los vecinos pueden comer
	test((i + N - 1) % N);
	test((i + 1) % N);
	pthread_mutex_unlock(&mutex);
}

// Simulacion de pensamiento
void think(int i, unsigned int *seed) {

	printf("Filosofo %d esta pensando.\n", i);
	sleep(rand_r(seed) % 3 + 1);			// Tiempo aleatorio entre 1 y 3 segundos
}

// Simulacion de comida
void eat(int i, unsigned int *seed) {

	printf("Filosofo %d esta comiendo.\n", i);
	sleep(rand_r(seed) % 3 + 1);			// Tiempo aleatorio entre 1 y 3 segundos
}

// Proceso de cada filosofo
void *philosopher(void *arg) {

	int i = *(int *)arg;
	unsigned int seed = (unsigned int)time(NULL) + i * 1234;

	for (int j = 0; j < RONDAS; j++) {
		think(i, &seed);
		take_forks(i);
		eat(i, &seed);
		put_forks(i);
	}

	printf("Filosofo %d termino sus ciclos.\n", i);
	return NULL;
}
