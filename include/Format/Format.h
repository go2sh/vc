#ifndef VC_FORMAT_FORMAT_H
#define VC_FORMAT_FORMAT_H

namespace vc {
  namespace format {
    struct FormatStyle {

      enum UseTabsStyle {
        UTS_Never,
        UTS_ForIdention
      };
      UseTabsStyle IndentionChar;

      unsigned TabSize;
      unsigned IndentionWidth;

    };
  }
}

#endif // !VC_FORMAT_FORMAT_H
