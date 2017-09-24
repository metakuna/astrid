#include "util/ThreadSingleton.hpp"
#include <gtest/gtest.h>
#include <unistd.h>
#include <atomic>
#include <chrono>
#include <vector>

static int THREAD_WAIT;
const int MAIN_WAIT = 100;

const int NUM_FAST_MESSAGES = 1000;
const float MS_PER_MESSAGE = 0.1;

class TestThread : public ThreadSingleton<int,TestThread> {
public:

  std::atomic<int> num;
  std::vector<int> arr;
  void dispatch(int i) {
    post_message(i);
  }  

protected:

  bool process_message(int&);

};

bool TestThread::process_message(int& i) {
  //std::cout << "received message with number: " << i << std::endl;
  std::this_thread::sleep_for(std::chrono::milliseconds(THREAD_WAIT));
  num = i;
  arr.push_back(i);
  return true;
}

// pass 9 messages and wait for the worker thread to update a member variable
TEST(ThreadSingletonTest, slow_message_test) {
  THREAD_WAIT = 50;
  TestThread* t = TestThread::Instance();
  for (int num = 1; num < 10; num++) {
     t->dispatch(num);
     ASSERT_NE(num, t->num); // wait for delay due to process_message
     std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_WAIT));
     ASSERT_EQ(num, t->num); // the message should have been processed by now
  }
}

// pass a large number of messages quickly and ensure they are all processed
TEST(ThreadSingletonTest, fast_message_test) {
  THREAD_WAIT = 0;
  TestThread* t = TestThread::Instance();
  int size1 = t->arr.size();
  std::cout << "Initial size of arr: " << size1 << std::endl;
  for (int i = 0; i < NUM_FAST_MESSAGES; i++) {
    t->dispatch(i);
  }
  // allow set amount of time per message
  std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(NUM_FAST_MESSAGES*MS_PER_MESSAGE)));
  ASSERT_EQ(size1 + NUM_FAST_MESSAGES, t->arr.size());
}

int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
