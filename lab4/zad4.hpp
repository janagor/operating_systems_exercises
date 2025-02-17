#pragma once
#include "monitor.h"
#include <array>
#include <cassert>
#include <cstring>
#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include <vector>

#define N 10

class PBuffer : public Monitor {
public:
  PBuffer(int buffer_size);
  PBuffer(int buffer_size, unsigned _buf_id);
  void log_producer(int id);
  void log_consumer(int id);
  void produce(int producer_id);
  void consume(int producer_id);
  bool can_produce();
  bool can_consume();
  inline int getIndex() { return index; };

public:
  unsigned type;

private:
  Condition full;
  Condition empty;
  int index;
  std::array<int, N> buffer;
};

void producer(PBuffer &pBuf, int producer_id);
void consumer(std::vector<PBuffer> &production, int id, int consumer_id);
