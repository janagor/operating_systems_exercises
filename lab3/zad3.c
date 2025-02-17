#include "zad3.h"

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *PRODUKTY[] = {"Ciasto", "Mieso", "Ser", "Kapusta"};

Buffer Buffers[BUFNUM];
sem_t empties[BUFNUM];
sem_t fulls[BUFNUM];
pthread_mutex_t mutexes[BUFNUM] = {
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER,
    PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};

void init_buffers() {
  for (int i = 0; i < BUFNUM; ++i) {
    Buffers[i].index = 0;
    memset(Buffers[i].values, 0, sizeof(Buffers[i].values));
  }
}

void log_producent(int producent_id, int id, int index) {
  printf("Producent %s:\n\t ID producenta: %d\n\t INDEX utworzonego "
         "produktu: %d\n",
         PRODUKTY[producent_id], id, index);
}

void log_konsument(int konsument_id, int id_ciasto, int id_nadzienie,
                   int index_ciasto, int index_nadzienie) {
  printf("Konsument %s:\n\t ID producenta ciasta: %d\n\t ID producenta "
         "nadzienia: %d\n\t INDEX wykorzystanego ciasta: %d\n\t INDEX "
         "wykorzystanego nadzienia: %d\n",
         PRODUKTY[konsument_id], id_ciasto, id_nadzienie, index_ciasto,
         index_nadzienie);
}

void *producent(void *arg) {
  ProdArgs *args = (ProdArgs *)arg;
  int type = args->type;
  int id = args->id;
  while (1) {

    sem_wait(&empties[type]);
    pthread_mutex_lock(&mutexes[type]);

    assert(Buffers[type].index >= 0);
    Buffers[type].values[Buffers[type].index] = id;
    log_producent(type, id, Buffers[type].index);
    Buffers[type].index++;
    assert(Buffers[type].index < N + 1);

    pthread_mutex_unlock(&mutexes[type]);
    sem_post(&fulls[type]);
    sleep(1);
  }
  return NULL;
}

void *konsument(void *arg) {
  ProdArgs *args = (ProdArgs *)arg;
  int type = args->type;
  int id = args->id;

  while (1) {
    // ciasto
    sem_wait(&fulls[0]);

    pthread_mutex_lock(&mutexes[0]);

    --Buffers[0].index;
    assert(Buffers[0].index < N);
    int id_ciasto = Buffers[0].values[Buffers[0].index];
    int idx_ciasto = Buffers[0].index;
    assert(Buffers[0].index >= 0);

    pthread_mutex_unlock(&mutexes[0]);

    sem_post(&empties[0]);

    // farsz

    sem_wait(&fulls[type]);

    pthread_mutex_lock(&mutexes[type]);

    --Buffers[type].index;
    assert(Buffers[type].index < N);
    int id_nadzienie = Buffers[type].values[Buffers[type].index];
    int idx_nadzienie = Buffers[type].index;
    log_konsument(type, id_ciasto, id_nadzienie, idx_ciasto, idx_nadzienie);
    assert(Buffers[type].index >= 0);

    pthread_mutex_unlock(&mutexes[type]);

    sem_post(&empties[type]);

    sleep(1);
  }
  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(
        stderr,
        "Użycie: %s <liczba_producentów_na_typ> <liczba_konsumentów_na_typ>\n",
        argv[0]);
    return EXIT_FAILURE;
  }

  int PRODUCENCI = atoi(argv[1]);
  int KONSUMENCI = atoi(argv[2]);

  if (PRODUCENCI <= 0 || KONSUMENCI <= 0) {
    fprintf(stderr, "Liczba producentów i konsumentów musi być większa od 0\n");
    return EXIT_FAILURE;
  }

  pthread_t **producenci = malloc(BUFNUM * sizeof(pthread_t *));
  pthread_t **konsumenci = malloc(BUFNUM * sizeof(pthread_t *));

  for (int i = 0; i < BUFNUM; i++) {
    producenci[i] = malloc(PRODUCENCI * sizeof(pthread_t));
    konsumenci[i] = malloc(KONSUMENCI * sizeof(pthread_t));

    if (!producenci[i] || !konsumenci[i]) {
      perror("Błąd alokacji pamięci dla wątków");
      return EXIT_FAILURE;
    }
  }

  for (int i = 0; i < BUFNUM; ++i) {
    sem_init(&empties[i], 0, N);
    sem_init(&fulls[i], 0, 0);
  }

  for (int i = 0; i < PRODUCENCI; ++i) {
    for (int j = 0; j < BUFNUM; ++j) {
      ProdArgs args;
      args.id = i;
      args.type = j;
      pthread_create(&producenci[j][i], NULL, producent, &args);
      usleep(100000);
    }
  }

  for (int i = 0; i < KONSUMENCI; i++) {

    for (int j = 1; j < BUFNUM; ++j) {
      ProdArgs args;
      args.id = i;
      args.type = j;
      pthread_create(&producenci[j - 1][i], NULL, konsument, &args);
      usleep(100000);
    }
  }

  for (int i = 0; i < PRODUCENCI; i++)
    for (int j = 0; j < BUFNUM; j++)
      pthread_join(producenci[j][i], NULL);

  for (int i = 0; i < PRODUCENCI; i++)
    for (int j = 1; j < BUFNUM; j++)
      pthread_join(konsumenci[j - 1][i], NULL);

  for (int i = 0; i < BUFNUM; i++) {
    free(producenci[i]);
    free(konsumenci[i]);
  }
  free(producenci);
  free(konsumenci);

  for (int i = 0; i < BUFNUM; ++i) {
    sem_destroy(&empties[0]);
    sem_destroy(&fulls[0]);
  }

  return 0;
}
