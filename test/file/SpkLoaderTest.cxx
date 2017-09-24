// ROOT/test/file/SpkLoaderTest.cpp

#include <iostream>

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <SpiceUsr.h>

#include "file/SpkLoader.hpp"
#include "sim/State.hpp"
#include "AstridConfig.hpp"

using namespace std;

#ifdef _IGNORE_

// max number of object ids for spice to load
const SpiceInt MAXOBJ = 1000; 

string eph_path;
string lsk_path;

void init() {
  AstInit();
  eph_path = AST_ROOT + RES + EPH + PLANETS_EPH_FILE;
  lsk_path = AST_ROOT + RES + LSK_FILE;
}

int main(int argc, char** argv, char** penv) {
  SPICEINT_CELL ( ids, MAXOBJ );

  init();
  cout << "ephemerides file: " << eph_path << endl;
  cout << "leap second file: " << lsk_path << endl;
  const int nTests = 10;
  
  timestamp_t t1 = get_timestamp();

  for (int i = nTests; i > 0; i--) {
    //furnsh_c(eph_path.c_str());
    //unload_c(eph_path.c_str());
  }
  furnsh_c(eph_path.c_str());
  cout << "Time to load and unload " << nTests << " times: " << (get_timestamp() - t1) << " us" << endl;

  //cout << "not yet loaded files" << endl;
  //sleep(10);

  furnsh_c(lsk_path.c_str());
  furnsh_c(eph_path.c_str());

  //cout << "Loaded files" << endl;
  //sleep(10);


  ConstSpiceChar* targ = static_cast<ConstSpiceChar*>("MOON");
  ConstSpiceChar* obs = static_cast<ConstSpiceChar*>("EARTH");
  ConstSpiceChar* abcorr = static_cast<ConstSpiceChar*>("NONE");
  ConstSpiceChar* ref = static_cast<ConstSpiceChar*>("J2000");

  SpiceDouble res[6];
  SpiceDouble lt;

  ofstream dump;
  dump.open("eph_dump.txt", ios::app);
  dump << "ephemeris for the moon wrt earth, with lsk\n\n";

  t1 = get_timestamp();
  SpiceDouble epoch = 0;
  for (int i = 0; i < nTests; i++) {
    spkezr_c(targ, epoch, ref, abcorr, obs, res, &lt);
    epoch += 1000000;
    if (epoch > 100000000)
      epoch = 0;
    dump << "Time: " << epoch << endl;
    dump << "Pos: " << setprecision(10) << fixed << res[0] << " " << res[1] << " " << res[2] << endl;
    dump << "Vel: " << setprecision(10) << fixed << res[3] << " " << res[4] << " " << res[5] << endl;
  } 
  dump.close();
  cout << "Time to get moon state " << nTests << " times: " << (get_timestamp() - t1) << " us" << endl;

  cout << "finished getting moon state" << endl;
  //sleep(10);

  unload_c(eph_path.c_str());
  unload_c(lsk_path.c_str());
  cout << "unloaded files" << endl;
  //sleep(10);
}

#endif

const std::string LSK_PATH = "naif0012.tls";
const std::string EPH_PATH = "ephemerides/de430.bsp";

TEST(SpkLoaderTest, initial) {
/*  SpkLoader spk(LSK_PATH);
  spk.register_file(EPH_PATH);

  State* points[10];
  double rS, rI;
  spk.load_state("EARTH", NULL, 0.0, 1000.0, 10, points, &rS, &rI);
  spk.force_unload_files();
*/
  const int MAXOBJ = 1000;
  const int WINSIZ = 2000;
 
  string lsk_full = astrid::INSTALL_DIR + astrid::RESOURCES_DIR + LSK_PATH;
  //ConstSpiceChar* lsk = string(astrid::INSTALL_DIR + astrid::RESOURCES_DIR + LSK_PATH).c_str();
  const char* lsk_c = string(astrid::INSTALL_DIR + astrid::RESOURCES_DIR + LSK_PATH).c_str();
  ConstSpiceChar* lsk = lsk_full.c_str();
  ConstSpiceChar* eph = (astrid::INSTALL_DIR + astrid::RESOURCES_DIR + EPH_PATH).c_str();

  SpiceChar time[1000];

  SPICEINT_CELL(ids, MAXOBJ);
  SPICEDOUBLE_CELL(cover, WINSIZ);

  cout << lsk_full << endl;
  cout << lsk_c << endl;
  cout << lsk << endl;
  cout << eph << endl;

  furnsh_c(lsk);
  furnsh_c(eph);

  spkobj_c(eph, &ids);
  
  SpiceInt obj = 0;
  SpiceInt niv = 0;

  SpiceDouble begin;
  SpiceDouble end;
  for (SpiceInt i = 0; i < card_c(&ids); i++) {
    obj = SPICE_CELL_ELEM_I(&ids, i);

    scard_c(0, &cover);
    spkcov_c(eph, obj, &cover);
    
    niv = wncard_c(&cover);
  
    cout << "Coverage for object: " << static_cast<int>(obj) << endl;
    
    for (SpiceInt j = 0; j < niv; j++) {
      wnfetd_c(&cover, j, &begin, &end);
   
      timout_c ( begin, "YYYY MON DD HR:MN:SC.### (TDB) ::TDB",  
                              1000, time);
      cout << "Interval beginning: " << time << endl;

      timout_c ( end, "YYYY MON DD HR:MN:SC.### (TDB) ::TDB",  
                              1000, time);
      cout << "Interval ending: " << time << endl;
    }
  }
}
/*
  ConstSpiceChar* targ = static_cast<ConstSpiceChar*>("MOON");
  ConstSpiceChar* obs = static_cast<ConstSpiceChar*>("EARTH");
  ConstSpiceChar* abcorr = static_cast<ConstSpiceChar*>("NONE");
  ConstSpiceChar* ref = static_cast<ConstSpiceChar*>("J2000");

  SpiceDouble res[6];
  SpiceDouble lt;

  ofstream dump;
  dump.open("eph_dump.txt", ios::app);
  dump << "ephemeris for the moon wrt earth, with lsk\n\n";

  t1 = get_timestamp();
  SpiceDouble epoch = 0;
  for (int i = 0; i < nTests; i++) {
    spkezr_c(targ, epoch, ref, abcorr, obs, res, &lt);
  
}
*/
int main(int argc, char** argv) {
  testing::InitGoogleMock(&argc, argv);
  return RUN_ALL_TESTS();
}
