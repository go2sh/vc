#include <filesystem>

#include <Support/ConfigFolders.h>

using namespace vc;

std::string ConfigFolders::getUserDir() { return std::string{}; }

std::string ConfigFolders::getUserConfigDir() { return std::string{}; }

std::string ConfigFolders::getMachineConfigDir() { return std::string{}; }