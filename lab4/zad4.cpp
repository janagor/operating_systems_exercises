#include "zad4.hpp"
#include "monitor.h"
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>

PBuffer::PBuffer(int buffer_size) : index(0), type(-1), buffer() {}
PBuffer::PBuffer(int buffer_size, unsigned type)
    : index(0), type(type), buffer() {}

bool PBuffer::can_consume() { return index > 0; }

bool PBuffer::can_produce() { return index < N; }

void PBuffer::log_producer(int id) {
  std::ostringstream b;

  b << "write to: ";
  b << type;
  b << ", index after: ";
  b << index;
  b << ", written: ";
  b << buffer.at(index - 1);
  b << ", by: ";
  b << id;
  b << "\n";
  std::cout << b.str();
}
void PBuffer::log_consumer(int id) {
  std::ostringstream b;

  b << "read from: ";
  b << type;
  b << ", index after: ";
  b << index;
  b << ", read: ";
  b << buffer.at(index);
  b << ", by: ";
  b << id;
  if (type) {
    b << ", pierog stworzony";
  }
  b << "\n";
  std::cout << b.str();
}

void PBuffer::produce(int producer_id) {
  std::ostringstream b;
  enter();

  if (!can_produce())
    wait(full);
  buffer.at(index) = producer_id;
  ++index;
  log_producer(producer_id);
  if (can_consume())
    signal(empty);

  leave();
}

void PBuffer::consume(int consumer_id) {
  enter();

  if (!can_consume()) {
    wait(empty);
  }

  --index;
  log_consumer(consumer_id);

  if (can_produce())
    signal(full);

  leave();
}

void producer(PBuffer &p, int producer_id) {
  while (true) {
    p.produce(producer_id);
    sleep(1);
  }
}

void consumer(std::vector<PBuffer> &production, int type, int consumer_id) {
  while (true) {
    production.at(0).consume(consumer_id);
    production.at(type).consume(consumer_id);
    sleep(1);
  }
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    std::cerr << "Użycie: " << argv[0]
              << " <liczba_producentów_na_typ> <liczba_konsumentów_na_typ>"
              << std::endl;
    return EXIT_FAILURE;
  }
  int PRODUCENCI = std::atoi(argv[1]);
  int KONSUMENCI = std::atoi(argv[2]);

  std::vector<PBuffer> pbuffers = {PBuffer(N, 0), PBuffer(N, 1), PBuffer(N, 2),
                                   PBuffer(N, 3)};

  std::vector<std::thread> producers(PRODUCENCI * 4);
  std::vector<std::thread> consumers(KONSUMENCI * 3);

  for (int i = 0; i < KONSUMENCI; ++i)
    for (int j = 1; j < 4; ++j) {
      consumers.at(i * 3 + (j - 1)) =
          std::thread(consumer, std::ref(pbuffers), j, i);
    }

  for (int i = 0; i < PRODUCENCI; ++i)
    for (int j = 0; j < 4; j++) {
      producers.at(i * 4 + j) =
          std::thread(producer, std::ref(pbuffers.at(j)), i);
    }

  for (int i = 0; i < PRODUCENCI * 4; ++i) {
    producers[i].join();
  }

  for (int i = 0; i < KONSUMENCI * 3; ++i) {
    consumers[i].join();
  }

  return 0;
}
