// ROOT/util/Loadable.hpp
#include <atomic>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>

/* Class for encapsulating objects (buffers) that need to be loaded (usually
   from a file) from a separate thread to the one they will be used in.
*/
template <typename Wrapped>
class Loadable {
  std::atomic<bool> loading_;
  std::mutex m_;
  std::condition_variable cv_;
  std::size_t idHash_;
  std::hash<std::thread::id> hasher_;

public:
  Wrapped val; // prefer to use get and set instead
  
  explicit Loadable(Wrapped v) : val(v), loading_(false) {};
  explicit Loadable() : loading_(false) {};

  /* The following functions check the identity of the caller
     using either the thread id or a user defined key */
  // raises the loading flag so only the thread that called it
  // can lower it
  void markLoading();
  template <typename T> void markLoading(T key);
  // lower the loading flag
  void markDone();
  template <typename T> void markDone(T key);  
  // copy the passed value into val
  void set(Wrapped const&);
  template <typename T> void set(Wrapped const&, T key);
  // return a reference to val
  Wrapped& get();
  template <typename T> Wrapped& get(T key);

  bool isLoading() {return loading_;};
};

/* Mark val as 'being loaded' so it can only be accessed by calling with the
   key that was passed to this function (usually the thread id)
*/
template <typename Wrapped>
template <typename T>
void Loadable<Wrapped>::markLoading(T key) {
  std::lock_guard<std::mutex> lk(m_); 
  //std::cout << "Entered markLoading, flag: " << loading_ << " id: " << idHash_ << std::endl;
  if (loading_) {
    throw "Object is being loaded somewhere else";
  }
  loading_ = true;
  // only allow this key to unlock it
  idHash_ = std::hash<T>{}(key);
  //std::cout << "Marked thread as loading, flag: " << loading_ << " id: " << idHash_ << std::endl;
  
}

// use the thread id as a key by default
template <typename Wrapped>
void Loadable<Wrapped>::markLoading() {
  markLoading(std::this_thread::get_id());
}


/* Flag that the loading is complete so other threads can access val
*/
template <typename Wrapped>
template <typename T>
void Loadable<Wrapped>::markDone(T key) {
  // ensure the correct thread is unlocking it
  //std::cout << "Entered markDone, flag: " << loading_ << " id: " << idHash_ << std::endl;
  if (idHash_ == std::hash<T>{}(key)) {
    std::lock_guard<std::mutex> lk(m_);
    loading_ = false;
    cv_.notify_all();
  } else {
    throw "This thread does not have permission to mark the object as loaded";
  }
  // std::cout << "Exiting markDone, flag: " << loading_ << " id: " << idHash_ << std::endl;

}

//use the thread id as a key by default
template <typename Wrapped>
void Loadable<Wrapped>::markDone() {
  markDone(std::this_thread::get_id());
}


/* set the value of val 
*/
template <typename Wrapped>
template <typename T>
void Loadable<Wrapped>::set(Wrapped const& arg, T key) {
  bool canSet = true;
  if (loading_) {
    // if val is being loaded then only the looading thread can set the value
    if (idHash_ != std::hash<T>{}(key))
      canSet = false;
  }
  if (canSet) {
    std::lock_guard<std::mutex> lk(m_);
    val = arg;
  } else {
    throw "This thread does not have permission to access val as it is being loaded by another thread.";
  }
}
template <typename Wrapped>
void Loadable<Wrapped>::set(Wrapped const& arg) {
  set(arg, std::this_thread::get_id());
}


/* Return the value of val
*/
template <typename Wrapped>
template <typename T>
Wrapped& Loadable<Wrapped>::get(T key) {
  bool canGet = true;
  if (loading_) {
    // if val is being loaded then only the loading thread can get the value
    if (idHash_ != std::hash<T>{}(key))
      canGet = false;
  }
  if (canGet) {
    std::lock_guard<std::mutex> lk(m_);
    return val;
  } else {
    throw "This thread does not have permission to access val as it is being loaded by another thread.";
  }
}
// use the thread id as a key by default
template <typename Wrapped>
Wrapped& Loadable<Wrapped>::get() {
  return get(std::this_thread::get_id());
}
