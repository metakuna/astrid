// ROOT/test/file/FileThreadTest.cpp

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "sim/State.hpp"
#include "file/FileThread.hpp"
#include "file/EphLoader.hpp"

using namespace std;

// mock ephemereis loader class to be called by file thread
class MockSubLoader : public file::EphLoader {
public:
  MOCK_METHOD6(load_state, bool(string target, string rel,
                double start, double timeStep, int nPoints, sim::State* stateArr));
  MOCK_METHOD6(load_state_range, bool(string target, string rel,
                double start, double end, int nPoints, sim::State* stateArr));
  
};

TEST(FileThreadTest,call_eph_loader) {

}

int main(int argc, char** argv) {
  //testing::InitGoogleTest(&argc, char** argv);
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
