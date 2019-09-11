#ifndef VC_SUPPORT_CONFIGFOLDERS
#define VC_SUPPORT_CONFIGFOLDERS
#include <string>
#include <optional>

namespace vc {
class ConfigFolders {
  std::string ComponentName;

public:
  ConfigFolders(const std::string &ComponentName)
      : ComponentName(ComponentName) {}
  
  /** Get the user folder of the current user */
  std::string getUserDir();
  /** Get the user config dir for the component */
  std::string getUserConfigDir();
  /** Get the machine config dir for the component */
  std::string getMachineConfigDir();
};
} // namespace vc
#endif // !VC_SUPPORT_CONFIGFOLDERS