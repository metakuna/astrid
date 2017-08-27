#pragma once

#include <cstdlib>
#include <string>

// Name of the application
const char* ASTRID = "ASTRID";
// root (src) directory of the project
std::string AST_ROOT;
static bool _isInit = false;

bool AstInit() {
  AST_ROOT = std::string(getenv(ASTRID));
  _isInit = true;
  return _isInit;
}

bool AstIsInit() { 
  return _isInit;
}

