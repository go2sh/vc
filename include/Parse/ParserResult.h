#ifndef VC_PARSER_RESULT_H
#define VC_PARSER_RESULT_H

namespace vc {
template <typename T> class ParserResult {

  T *Result;
  bool isError;

public:
  void setIsParseError() { isError = true; };
  bool isParseError() { return isError; };

  T *get() { return Result; };
};

template <typename T>
static inline ParserResult<T> makeParserResult(T *Result) {
  return ParserResult<T>(Result);
}

template <typename T>
static inline ParserResult<T> makeParserErrorResult(T *Result = nullptr) {
  ParserResult<T> PR;
  if (Result)
    PR = ParserResult<T>(Result);
  PR.setIsParseError();
  return PR;
};
};
#endif
