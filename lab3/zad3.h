#ifndef ZAD3_H
#define ZAD3_H

#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFNUM 4
#define N 10

typedef struct {
  int type;
  int id;
} ProdArgs;

// NOTE: index is and index of the array buffer `values` and it points to the
// next EMPTY cell - cell to be written to
typedef struct Buffer {
  int index;
  int values[N];
} Buffer;

void init_buffers();
void log_producent(int producent_id, int id, int index);
void log_konsument(int konsument_id, int id_ciasto, int id_nadzienie,
                   int index_ciasto, int index_nadzienie);
void *producent(void *arg);
void *konsument(void *arg);

#endif
