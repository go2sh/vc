#include <cstdlib>
#include <filesystem>
#include <optional>

/* For home dir detection */
#include <pwd.h>
#include <sys/types.h>
#include <unistd.h>

#include <Support/ConfigFolders.h>

using namespace vc;
namespace fs = std::filesystem;

static std::optional<std::string> getEnv(const std::string &Name) {
  const char *EnvValue = std::getenv(Name.data());

  if (EnvValue) {
    return std::optional<std::string>{std::string{EnvValue}};
  }

  return std::optional<std::string>{};
}

std::string ConfigFolders::getUserDir() {
  auto HomeVar = getEnv("HOME");

  if (HomeVar) {
    return *HomeVar;
  }

  struct passwd *pw = getpwuid(getuid());
  if (pw) {
    return std::string{pw->pw_dir};
  }

  throw false;
}

std::string ConfigFolders::getUserConfigDir() {
  auto XdgDirEnv = getEnv("XDG_CONFIG_HOME");

  if (XdgDirEnv) {
    return *XdgDirEnv;
  }
  
  fs::path XdgPath = fs::path{getUserDir()} / ".config" / ComponentName;
  return XdgPath.string();
}

std::string ConfigFolders::getMachineConfigDir() { return std::string{}; }