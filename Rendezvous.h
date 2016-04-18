#ifndef __RENDEZVOUS_H__
#define __RENDEZVOUS_H__

#include <condition_variable>
#include <mutex>

// From https://en.wikipedia.org/wiki/Rendezvous_(Plan_9)
// can only handle two entities. if a third entity
// tries to call .wait(), behavior is undefined.
class Rendezvous {
 public:
  Rendezvous() : _waiting(false) {}
  ~Rendezvous() {}

  void wait();

 private:
  std::condition_variable _cv;
  std::mutex _mutex;
  bool _waiting;
};

#endif  // __RENDEZVOUS_H__
