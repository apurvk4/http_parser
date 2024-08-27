#pragma once

/**
 * @file ResponseParser.hpp
 * @brief simple http parser for parsing http response
 * @version 1.0.0
 * @date 2024-08-19
 *
 *
 * @section LICENSE
 * GNU General Public License v3.0
 */

#include "HttpDefinitions.hpp"
#include <API.h>
#include <string>
#include <vector>

namespace http_parser {

enum class ResponseParseState {
  VERSION,
  VERSION_HTTP_H,
  VERSION_HTTP_T1,
  VERSION_HTTP_T2,
  VERSION_HTTP_P1,
  VERSION_SLASH,
  VERSION_MAJOR,
  VERSION_DOT,
  VERSION_MINOR,
  STATUS_CODE,
  STATUS_CODE_SPACE,
  STATUS_MESSAGE,
  STATUS_MESSAGE_CR,
  STATUS_MESSAGE_LF,
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

class PARSER_EXPORT ResponseParser {
public:
  ResponseParser();
  ~ResponseParser() = default;

  bool parse(int file_descriptor);
  void reset();
  Response get_response() const;

private:
  ResponseParseState currentParseState;
  Response response;
  std::string currentVersion;
  std::string currentStatusCode;
  std::string currentStatusMessage;
  std::string currentHeaderKey;
  std::string currentHeaderValue;
  bool isValidStatusCodeChar(char c);
  bool isValidHeaderKeyChar(char c);
  void processResponse(int file_descriptor);
  void parseVersion(char nextChar, ResponseParseState &currentParseState,
                    bool &readNextChar);
  void parseVersionHttpH(char nextChar, ResponseParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionHttpT1(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionHttpT2(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionHttpP1(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
  void parseVersionSlash(char nextChar, ResponseParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionMajor(char nextChar, ResponseParseState &currentParseState,
                         bool &readNextChar);
  void parseVersionDot(char nextChar, ResponseParseState &currentParseState,
                       bool &readNextChar);
  void parseVersionMinor(char nextChar, ResponseParseState &currentParseState,
                         bool &readNextChar);
  void parseStatusCode(char nextChar, ResponseParseState &currentParseState,
                       bool &readNextChar);
  void parseStatusCodeSpace(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseStatusMessage(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
  void parseStatusMessageCR(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseStatusMessageLF(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseHeaderKey(char nextChar, ResponseParseState &currentParseState,
                      bool &readNextChar);
  void parseHeaderDelimiter(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseHeaderValue(char nextChar, ResponseParseState &currentParseState,
                        bool &readNextChar);
  void parseHeaderLineEndCR(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseHeaderLineEndLF(char nextChar,
                            ResponseParseState &currentParseState,
                            bool &readNextChar);
  void parseEndOfHeaderCR(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
  void parseEndOfHeaderLF(char nextChar, ResponseParseState &currentParseState,
                          bool &readNextChar);
};

} // namespace http_parser