#include "ResponseParser.hpp"
#include "HttpDefinitions.hpp"
#include "OS.h"
#include <cctype>

using http_parser::Header;
using http_parser::Response;
using http_parser::ResponseParser;
using http_parser::ResponseParseState;
using http_parser::StatusCode;
using http_parser::Version;

ResponseParser::ResponseParser()
    : currentParseState(ResponseParseState::VERSION) {}

bool ResponseParser::parse(int file_descriptor) {
  reset();
  processResponse(file_descriptor);
  return currentParseState == ResponseParseState::DONE;
}

void ResponseParser::reset() {
  currentParseState = ResponseParseState::VERSION;
  response = Response();
  currentVersion = "";
  currentStatusCode = "";
  currentStatusMessage = "";
  currentHeaderKey = "";
  currentHeaderValue = "";
}

Response ResponseParser::get_response() const { return response; }

bool ResponseParser::isValidStatusCodeChar(char c) { return isdigit(c); }

bool ResponseParser::isValidHeaderKeyChar(char c) {
  return isalnum(c) || c == '-';
}

void ResponseParser::processResponse(int file_descriptor) {
  char nextChar;
  bool readNextChar = true;
  while (currentParseState != ResponseParseState::DONE &&
         currentParseState != ResponseParseState::PARSE_ERROR) {
    if (readNextChar) {
      readNextChar = false;
      int byteRead = ::read(file_descriptor, &nextChar, 1);
      if (byteRead <= 0) {
        perror("Error reading from file discriptor");
        return;
      }
    }
    switch (currentParseState) {
    case ResponseParseState::VERSION:
      parseVersion(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_HTTP_H:
      parseVersionHttpH(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_HTTP_T1:
      parseVersionHttpT1(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_HTTP_T2:
      parseVersionHttpT2(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_HTTP_P1:
      parseVersionHttpP1(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_SLASH:
      parseVersionSlash(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_MAJOR:
      parseVersionMajor(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_DOT:
      parseVersionDot(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::VERSION_MINOR:
      parseVersionMinor(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::STATUS_CODE:
      parseStatusCode(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::STATUS_CODE_SPACE:
      parseStatusCodeSpace(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::STATUS_MESSAGE:
      parseStatusMessage(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::STATUS_MESSAGE_CR:
      parseStatusMessageCR(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::STATUS_MESSAGE_LF:
      parseStatusMessageLF(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::END_OF_HEADER_CR:
      parseEndOfHeaderCR(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::END_OF_HEADER_LF:
      parseEndOfHeaderLF(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::HEADER_KEY:
      parseHeaderKey(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::HEADER_DELIMITER:
      parseHeaderDelimiter(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::HEADER_VALUE:
      parseHeaderValue(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::HEADER_LINE_END_CR:
      parseHeaderLineEndCR(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::HEADER_LINE_END_LF:
      parseHeaderLineEndLF(nextChar, currentParseState, readNextChar);
      break;
    case ResponseParseState::DONE:
      readNextChar = false;
      break;
    case ResponseParseState::PARSE_ERROR:
      readNextChar = false;
      break;
    }
  }
}

void ResponseParser::parseVersion(char nextChar,
                                  ResponseParseState &currentParseState,
                                  bool &readNextChar) {
  if (nextChar == 'H') {
    currentParseState = ResponseParseState::VERSION_HTTP_H;
  } else if (std::isspace(nextChar)) {
    // Ignore leading whitespace
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionHttpH(char nextChar,
                                       ResponseParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::toupper(nextChar) == 'H') {
    currentVersion += std::toupper(nextChar);
    currentParseState = ResponseParseState::VERSION_HTTP_T1;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionHttpT1(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  if (std::toupper(nextChar) == 'T') {
    currentVersion += std::toupper(nextChar);
    currentParseState = ResponseParseState::VERSION_HTTP_T2;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionHttpT2(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  if (std::toupper(nextChar) == 'T') {
    currentVersion += std::toupper(nextChar);
    currentParseState = ResponseParseState::VERSION_HTTP_P1;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionHttpP1(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  if (std::toupper(nextChar) == 'P') {
    currentVersion += std::toupper(nextChar);
    currentParseState = ResponseParseState::VERSION_SLASH;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionSlash(char nextChar,
                                       ResponseParseState &currentParseState,
                                       bool &readNextChar) {
  if (nextChar == '/') {
    currentVersion += nextChar;
    currentParseState = ResponseParseState::VERSION_MAJOR;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionMajor(char nextChar,
                                       ResponseParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::isdigit(nextChar)) {
    currentVersion += nextChar;
    currentParseState = ResponseParseState::VERSION_DOT;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionDot(char nextChar,
                                     ResponseParseState &currentParseState,
                                     bool &readNextChar) {
  if (nextChar == '.') {
    currentVersion += nextChar;
    currentParseState = ResponseParseState::VERSION_MINOR;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseVersionMinor(char nextChar,
                                       ResponseParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::isdigit(nextChar)) {
    currentVersion += nextChar;
    response.version = http_parser::string_to_version(currentVersion);
    currentParseState = ResponseParseState::STATUS_CODE;
    readNextChar = true;
    currentVersion.clear();
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseStatusCode(char nextChar,
                                     ResponseParseState &currentParseState,
                                     bool &readNextChar) {
  char whitespace = 32; // ASCII value for space
  if (isValidStatusCodeChar(nextChar) && currentStatusCode.size() < 3) {
    currentStatusCode += nextChar;
    readNextChar = true;
  } else if (nextChar == whitespace && !currentStatusCode.empty()) {
    response.status_code =
        http_parser::string_to_status_code(currentStatusCode);
    currentParseState = ResponseParseState::STATUS_CODE_SPACE;
    currentStatusCode.clear();
  } else if (nextChar == whitespace && currentStatusCode.empty()) {
    // ignore whitespace
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseStatusCodeSpace(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char whitespace = 32; // ASCII value for space
  if (nextChar == whitespace) {
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::STATUS_MESSAGE;
  }
}

void ResponseParser::parseStatusMessage(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn && !currentStatusMessage.empty()) {
    currentParseState = ResponseParseState::STATUS_MESSAGE_CR;
  } else if (std::isprint(nextChar) && nextChar >= 32 && nextChar <= 126) {
    currentStatusMessage += nextChar;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseStatusMessageCR(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn) {
    currentParseState = ResponseParseState::STATUS_MESSAGE_LF;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseStatusMessageLF(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char lineFeed = 10; // ASCII value for line feed
  if (nextChar == lineFeed) {
    response.status_message = currentStatusMessage;
    currentParseState = ResponseParseState::HEADER_KEY;
    currentStatusMessage.clear();
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseEndOfHeaderCR(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn) {
    currentParseState = ResponseParseState::END_OF_HEADER_LF;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseEndOfHeaderLF(char nextChar,
                                        ResponseParseState &currentParseState,
                                        bool &readNextChar) {
  char lineFeed = 10; // ASCII value for line feed
  if (nextChar == lineFeed) {
    currentParseState = ResponseParseState::DONE;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseHeaderKey(char nextChar,
                                    ResponseParseState &currentParseState,
                                    bool &readNextChar) {
  char whitespace = 32;     // ASCII value for space
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn) {
    currentParseState = ResponseParseState::END_OF_HEADER_CR;
  } else if (isValidHeaderKeyChar(nextChar)) {
    currentHeaderKey += std::tolower(nextChar);
    readNextChar = true;
  } else if (nextChar == whitespace) {
    // ignore whitespace
    readNextChar = true;
  } else if (nextChar == ':') {
    currentParseState = ResponseParseState::HEADER_DELIMITER;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseHeaderDelimiter(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char whitespace = 32; // ASCII value for space
  if (nextChar == whitespace) {
    // ignore whitespace
    readNextChar = true;
  } else if (nextChar == ':') {
    currentParseState = ResponseParseState::HEADER_VALUE;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseHeaderValue(char nextChar,
                                      ResponseParseState &currentParseState,
                                      bool &readNextChar) {
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn) {
    currentParseState = ResponseParseState::HEADER_LINE_END_CR;
  } else if (std::isprint(nextChar) && nextChar >= 32 && nextChar <= 126) {
    currentHeaderValue += nextChar;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseHeaderLineEndCR(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char carriageReturn = 13; // ASCII value for carriage return
  if (nextChar == carriageReturn) {
    currentParseState = ResponseParseState::HEADER_LINE_END_LF;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}

void ResponseParser::parseHeaderLineEndLF(char nextChar,
                                          ResponseParseState &currentParseState,
                                          bool &readNextChar) {
  char lineFeed = 10; // ASCII value for line feed
  if (nextChar == lineFeed) {
    Header header = {currentHeaderKey, currentHeaderValue};
    response.headers.push_back(header);
    currentHeaderKey.clear();
    currentHeaderValue.clear();
    currentParseState = ResponseParseState::HEADER_KEY;
    readNextChar = true;
  } else {
    currentParseState = ResponseParseState::PARSE_ERROR;
  }
}