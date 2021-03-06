//===--- dcqueue.h - Geant-V ------------------------------------*- C++ -*-===//
//
//                     Geant-V Prototype
//
//===----------------------------------------------------------------------===//
/**
 * @file dcqueue.h
 * @brief Implementation of data concurrent queue for Geant-V prototype
 * @author Andrei Gheata
 */
//===----------------------------------------------------------------------===//

#ifndef GEANT_DCQUEUE
#define GEANT_DCQUEUE
#include <deque>
#include <cassert>
#include <atomic>
#include <condition_variable>
#include <mutex>
#include <thread>

/**
 * @brief Templated class dcqueue
 * @details
 *
 */
template <typename T>
class dcqueue {
  std::deque<T> the_queue; /** Double-ended queue */
  mutable std::mutex the_mutex;
  std::condition_variable the_condition_variable;
  std::atomic<size_t> nobjects;  /** Number of objects in the queue */
  std::atomic<size_t> npriority; /** Number of prioritized objects */
  size_t nop;                    /** Number of ops */
public:
  /** @brief Dcqueue constructor */
  dcqueue() : the_queue(), the_mutex(), the_condition_variable(), nobjects(0), npriority(0), nop(0) {}

  /** @brief Dcqueue destructor */
  ~dcqueue() {}

  /**
   * @brief Push function
   *
   * @param data Data that should be pushed in queue
   * @param priority Priority of data
   */
  void push(T const &data, bool priority = false);

  /**
   * @brief Function that provides asynchronized number of objects in queue
   * @return Atomic parameter of number of objects in queue
   */
  size_t size_async() const { return nobjects.load(std::memory_order_relaxed); }

  /**
   * @brief Function of cleanup asynchronized objects in  queue
   * @return Boolean value that shows if queue is empty or not
   */
  bool empty_async() const { return (nobjects.load(std::memory_order_relaxed) == 0); }

  /**
   * @brief Function that provides atomic parameter of size priority objects in queue
   * @return Atomic parameter of size of priority objects in queue
   */
  size_t size_priority() const { return npriority.load(std::memory_order_relaxed); }

  /**
   * @brief Function that provides atomic parameter of size of objects in queue
   * @return Atomic parameter of size of objects in queue
   */
  size_t size_objects() const { return nobjects.load(std::memory_order_relaxed); }

  /**
   * @brief n_ops function
   * @return nop  Number of ops
   */
  size_t n_ops() const { return nop; }

  /** @brief Templated sizing function for queue */
  size_t size() const;

  /** @brief Function that check if the queue is empty */
  bool empty() const;

  /** @brief Templated try_pop function */
  bool try_pop(T &data);

  /** @brief Templated wait_and_pop function */
  void wait_and_pop(T &data);

  /** @brief Templated wait_and_pop_max function */
  void wait_and_pop_max(size_t nmax, size_t &n, T *array);
};

/**
 * @details Push an pointer of type T* in the queue. If pushed with priority, the pointer
 * is put at the back of the queue, otherwise to the front.
 *
 * @tparam T Type of objects in queue
 */
template <typename T>
void dcqueue<T>::push(T const &data, bool priority)
{
  the_mutex.lock();
  if (priority) {
    the_queue.push_back(data);
    npriority++;
  } else
    the_queue.push_front(data);
  nobjects++;
  nop++;
  the_condition_variable.notify_one();
  the_mutex.unlock();
}

/**
 * @tparam T Type of objects in queue
 * @return Returns the number of objects in the queue.
 */
template <typename T>
size_t dcqueue<T>::size() const
{
  the_mutex.lock();
  size_t the_size = the_queue.size();
  the_mutex.unlock();
  return the_size;
}

/**
 * @tparam T type of objects in queue
 */
template <typename T>
bool dcqueue<T>::empty() const
{
  the_mutex.lock();
  bool is_empty = the_queue.empty();
  the_mutex.unlock();
  return is_empty;
}

/**
 * @details Gets the back object from the queue. Wait if none is available.
 *
 * @param data Data that should be popped in queue
 * @tparam T type of objects in queue
 */
template <typename T>
void dcqueue<T>::wait_and_pop(T &data)
{
  the_mutex.lock();
  while (the_queue.empty()) {
    std::unique_lock<std::mutex> lck(the_mutex, std::adopt_lock);
    the_condition_variable.wait(lck);
  }
  data = the_queue.back();
  the_queue.pop_back();
  nobjects--;
  nop++;
  if (npriority > 0) npriority--;
  the_mutex.unlock();
}

/**
 * @details Gets the back object from the queue. Returns 0 if none is available. Fast return...
 *
 * @param data Data that should be popped in queue
 * @tparam T Type of objects in queue
 * @todo We have to check again for robbery...
 */
template <typename T>
bool dcqueue<T>::try_pop(T &data)
{
  if (empty_async()) return false;
  the_mutex.lock();
  if (the_queue.empty()) {
    the_mutex.unlock();
    return false;
  }
  data = the_queue.back();
  the_queue.pop_back();
  nobjects--;
  nop++;
  if (npriority > 0) npriority--;
  the_mutex.unlock();
  return true;
}

/**
 * @details  Pop many objects in one go, maximum nmax. Will return the number of requested
 * objects, or the number available. The wait time for the client is minimal (at least one object in the queue).
 *
 * @param nmax Maximum of objects that can be poped in queue
 * @param n Objects that should be poped
 * @param array Array of elements that should be popped to queue
 * @tparam T Type of objects in queue
 */
template <typename T>
void dcqueue<T>::wait_and_pop_max(size_t nmax, size_t &n, T *array)
{
  std::unique_lock<std::mutex> lk(the_mutex);
  while (the_queue.empty()) {
    the_condition_variable.wait(lk);
  }
  n                    = the_queue.size();
  if (n > nmax) n      = nmax;
  unsigned int npopped = 0;
  while (npopped < n) {
    array[npopped++] = the_queue.back();
    the_queue.pop_back();
    nobjects--;
    if (npriority) npriority--;
  }
  the_mutex.unlock();
}
#endif
