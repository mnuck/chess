#include "Rendezvous.h"

#include "Logger.h"

void Rendezvous::wait() {
  std::unique_lock<std::mutex> lock(_mutex);
  _waiting ^= true;
  if (_waiting) {
    _cv.wait(lock, [&]() { return !_waiting; });
    lock.unlock();
  } else {
    lock.unlock();
    _cv.notify_one();
  }
}
