#pragma once
#include <iostream>
#include <queue>

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

/* Abstract class for controlling a thread which has access to a shared
   resource. It is used to wrap the 'thread unsafe' resource so it can
   be accessed asynchronously 
*/
template <typename Msg, typename Derived>
class ThreadSingleton {
public:
  // return the singleton
  static Derived* Instance() {
    static Derived instance;
    return &instance;
  }

protected:
  // constructor and destructor
  ThreadSingleton();
  virtual ~ThreadSingleton();
  
  // called by the client thread
  void post_message(Msg&);

  // called by the thread loop in the message thread
  virtual bool process_message(Msg&) = 0;
   
private:
  ThreadSingleton(ThreadSingleton const&); // delete copy constructor
  void operator=(ThreadSingleton const&); // delete assignement operator

  // loops over the message queue
  void msg_loop();
  // message queue checked by the worker thread
  std::queue<Msg> msgQueue_;
  // the thread that will actually do all the work
  std::thread worker_;

  std::mutex m;
  std::condition_variable cv_;
  std::atomic<bool> killThread_;
  std::atomic<bool> hasMessage_;
};

template <typename Msg,typename Derived>
ThreadSingleton<Msg,Derived>::ThreadSingleton() : killThread_(false) {
  //std::cout << "Instance created with type: "; // DEBUG
  //std::cout << typeid(Msg).name() << std::endl;

  worker_ = std::thread(&ThreadSingleton<Msg,Derived>::msg_loop, this);
}

template <typename Msg,typename Derived>
ThreadSingleton<Msg,Derived>::~ThreadSingleton() {
  //std::cout << "destructor called" <<std::endl;
  {
    std::lock_guard<std::mutex> lk(m);
    killThread_ = true;
  }
  cv_.notify_one(); // wake up the thread if it is waiting
  //std::cout << "waiting for thread" <<std::endl;
  worker_.join();
  //std::cout << "exiting destructor" << std::endl;
}

template <typename Msg,typename Derived>
void ThreadSingleton<Msg,Derived>::msg_loop() {
  std::queue<Msg> tempQ;
  while (!killThread_) {
    //std::cout << "in message loop" << std::endl;
    //sleep(1);

    std::unique_lock<std::mutex> lk(m); // lock the mutex
    cv_.wait(lk, [&]{return hasMessage_.load() or killThread_.load();});
    //std::cout << msgQueue_.size() << "messages to be processed" << std::endl;

    // copy all the messages waiting in the queue and release the
    // mutex so more messages can be posted
    while (!msgQueue_.empty()) {
      tempQ.push(msgQueue_.front());
      msgQueue_.pop();
    }
    hasMessage_ = false;
    lk.unlock(); // unlock the mutex

    // process all the messages
    while (!tempQ.empty() and !killThread_) { // check if class is being destroyed as process_message is virtual
      process_message(tempQ.front()); //TODO log if the processing fails
      tempQ.pop();
    }
  }
}

template <typename Msg,typename Derived>
void ThreadSingleton<Msg,Derived>::post_message(Msg& msg) {
  {
    std::lock_guard<std::mutex> lk(m);
    msgQueue_.push(msg);
    hasMessage_ = true;
    //std::cout << "posted message" << std::endl;
  }
  cv_.notify_one();
}
/*
template <typename Msg,typename Derived>
bool ThreadSingleton<Msg,Derived>::process_message(Msg& msg) {
  std::cout << "entered process_message function" << std::endl;
  return true;
}
*/
