#ifndef VC_FORMAT_EDIT_MANAGER_H
#define VC_FORMAT_EDIT_MANAGER_H
#include <set>

#include "FormatToken.h"

namespace vc {
  namespace format {
    class EditManager {
      std::set<SourceEdit> SourceEdits;

      public:
      void replaceWhitespace(FormatToken *FT,unsigned NewLines, unsigned Spaces);
    };
  }
}
#endif // !VC_FORMAT_EDIT_MANAGER_H