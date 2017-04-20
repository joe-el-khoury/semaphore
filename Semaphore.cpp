#include <stdexcept>
#include <condition_variable>
#include <thread>
#include <mutex>

#define WITH_LOCK(mu)           \
  if (bool __state__ = false) { \
  } else                        \
    for (std::lock_guard<std::mutex> lock(mu); !__state__; __state__ = true)

class Semaphore {
private:
  int count_;
  std::mutex mutex_;
  std::condition_variable cond_;
public:
  Semaphore (int count)
  {
    if (count < 0) {
      throw std::runtime_error("Semaphore : count cannot be 0.");
    }
    count_ = count;
  }

  void P ()
  {
    if (count_ < 0) {
      throw std::runtime_error("Semaphore : cannot P a negative-count semaphore.");
    }
    if (count_ == 0) {
      std::unique_lock<std::mutex> lk(mutex_);
      cond_.wait(lk);
    }
    WITH_LOCK(mutex_) {
      count_--;
    }
  }

  void V ()
  {
    WITH_LOCK(mutex_) {
      count_++;
    }
    cond_.notify_all();
  }
};