#ifndef VC_FORMAT_FORMATLINEFORMATER_H
#define VC_FORMAT_FORMATLINEFORMATER_H
#include <Format/FormatTypes.h>
#include <Format/Format.h>

#include "FormatLineParser.h"
#include "EditManager.h"


namespace vc {
namespace format {

class FormatLineFormater {
  const FormatStyle &Style;
  EditManager &Manager;

public:
  FormatLineFormater(EditManager &Manager, const FormatStyle &Style)
      : Manager(Manager), Style(Style) {}
  
  Penalty format(const std::vector<FormatLine *> &Lines);
};
} // namespace format
} // namespace vc

#endif // !VC_FORMAT_FORMATLINEFORMATER_H
