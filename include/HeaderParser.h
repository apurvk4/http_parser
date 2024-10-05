#pragma once

namespace http_parser {

enum class HeaderParseState {
  HEADER_KEY,
  HEADER_DELIMITER,
  HEADER_VALUE,
  HEADER_LINE_END_CR,
  HEADER_LINE_END_LF,
  END_OF_HEADER_CR,
  END_OF_HEADER_LF,
  DONE,
  PARSE_ERROR,
};

class HeaderParser{
public:
    bool parse(int fd);
};

};