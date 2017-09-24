// ROOT/test/util/LoadableTest.cpp
#include <iostream>
#include <chrono>
#include <future>
#include <functional>

#include <gtest/gtest.h>

#include "util/Loadable.hpp"

const int VALUE = 5;

using namespace std;

void sleep_millis(int i) {
  std::this_thread::sleep_for(std::chrono::milliseconds(i));
}

/* Basic test to ensure a value can be loaded by another thread */
TEST(LoadableTest, by_thread_id) {
  Loadable<int> l;
  std::thread( [&l] { 
      // cout << "Entered other thread" << endl;
      l.markLoading(); // this thread is loading data
      sleep_millis(100); // busy loading
      l.val = 3; // without setter
      l.set(VALUE); // use setter
      l.markDone(); // data is ready for use
  }).detach();
  sleep_millis(10);
  while (l.isLoading()) {
    // cout << "Waiting for the object to be loaded" << endl;
    sleep_millis(10);
  }
  ASSERT_EQ(VALUE, l.get()) << "The value was not loaded" << endl;
}

/* Ensure exceptions are thrown if the wong thread tries to get, set or
   take ownership of the object while it is being loaded by another thread
*/
TEST(LoadableTest, exceptions) {
  Loadable<int> l(VALUE);
  // launch a thread which takes control of l
  std::thread( [&l] {
      l.markLoading();
      sleep_millis(100);
      l.markDone();
  }).detach();
  sleep_millis(10); // wait for the thread to take control of l

  ASSERT_ANY_THROW(l.markLoading());
  ASSERT_ANY_THROW(l.get());
  ASSERT_ANY_THROW(l.set(VALUE));
  ASSERT_ANY_THROW(l.markDone());
  sleep_millis(200); // the other thread should have finished by now
  ASSERT_NO_THROW(l.markLoading());
  ASSERT_NO_THROW(l.get());
  ASSERT_NO_THROW(l.set(VALUE));
  ASSERT_NO_THROW(l.markDone());
}

/* Test that the value can be loaded using a single key from multiple
   threads
*/
void spawn_load_threads(Loadable<int>& l, unsigned key) {
  function<void(unsigned)> controller = [&] (unsigned key) {
    l.markLoading(key);
    
    thread([&] (unsigned key) {
      l.set(VALUE, key); 
      sleep_millis(100);
      l.markDone(key);
    }, key).detach();
  };
  thread(controller, key).detach();
}

TEST(LoadableTest, by_key) {
  Loadable<int> l;
  unsigned key = 124334213;
  spawn_load_threads(l, key);
  sleep_millis(10);
  while (l.isLoading()) {
    // cout << "Waiting for the object to be loaded" << endl;
    sleep_millis(10);
  }
  ASSERT_EQ(VALUE, l.get()) << "The value was not loaded" << endl;
  
}

int main(int argc, char** argv) {
  const string key = "helloe";
  cout << std::hash<string>{}(key) << std::endl;
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
