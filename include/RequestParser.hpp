#pragma once

/**
 * @file http_parser.hpp
 * @author apurv kumar (apurvkumar5555@gmail.com)
 * @brief simple http parser for parsing http request
 * @version 1.0.0
 * @date 2024-08-19
 *
 *
 * @section LICENSE
 * GNU General Public License v3.0
 * @copyright Copyright (c) 2024
 *
 */

#include "HttpDefinitions.hpp"
#include "API.h"
#include <string>
#include <vector>

namespace http_parser {

enum class ParseState {
  METHOD,
  URL,
  VERSION,
  VERSION_HTTP_H,
  VERSION_HTTP_T1,
  VERSION_HTTP_T2,
  VERSION_HTTP_P1,
  VERSION_SLASH,
  VERSION_MAJOR,
  VERSION_DOT,
  VERSION_MINOR,
  REQUEST_LINE_END,
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

class PARSER_EXPORT RequestParser {
public:
  RequestParser();
  ~RequestParser();
  bool parse(int file_descriptor);
  void reset();
  Request get_request();

private:
  ParseState currentParseState;
  Request request;
  std::string currentMethod;
  std::string currentUrl;
  std::string currentVersion;
  std::string currentHeaderKey;
  std::string currentHeaderValue;

  bool isValidUrlChar(char c);

  void processRequest(int file_descriptor);
  void parseMethod(char nextChar, ParseState &currentParseState,
                   bool &readNextChar);
  void parseUrl(char nextChar, ParseState &currentParseState,
                bool &readNextChar);
  void parseVersion(char nextChar, ParseState &currentParseState,
                    bool &readNextChar);
  void parseVersionHttpH(char nextChar, ParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionHttpT1(char nextChar, ParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionHttpT2(char nextChar, ParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionHttpP1(char nextChar, ParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionSlash(char nextChar, ParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionMajor(char nextChar, ParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionDot(char nextChar, ParseState &currentParseState,
                       bool &readNextChar);
  void parseVersionMinor(char nextChar, ParseState &currentParseState,
                         bool &readNextChar);
  void parseRequestLineEnd(char nextChar, ParseState &currentParseState,
                           bool &readNextChar);
  void parseHeaderKey(char nextChar, ParseState &currentParseState,
                      bool &readNextChar);
  void parseHeaderDelimiter(char nextChar, ParseState &currentParseState,
                            bool &readNextChar);
  void parseHeaderValue(char nextChar, ParseState &currentParseState,
                        bool &readNextChar);
  void parseHeaderLineEndCR(char nextChar, ParseState &currentParseState,
                            bool &readNextChar);
  void parseHeaderLineEndLF(char nextChar, ParseState &currentParseState,
                            bool &readNextChar);
  void parseEndOfHeaderCR(char nextChar, ParseState &currentParseState,
                          bool &readNextChar);
  void parseEndOfHeaderLF(char nextChar, ParseState &currentParseState,
                          bool &readNextChar);
};
}; // namespace http_parser