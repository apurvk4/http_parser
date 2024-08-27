#include "RequestParser.hpp"
#include "OS.h"
#include <cctype>
#include <cerrno>

using http_parser::Header;
using http_parser::Method;
using http_parser::method_to_string;
using http_parser::ParseState;
using http_parser::Request;
using http_parser::RequestParser;
using http_parser::string_to_method;
using http_parser::string_to_version;
using http_parser::Version;
using http_parser::version_to_string;

RequestParser::RequestParser() : currentParseState(ParseState::METHOD) {}

bool RequestParser::parse(int file_discriptor) {
  // reset all the values
  currentMethod.clear();
  currentUrl.clear();
  currentVersion.clear();
  currentHeaderKey.clear();
  currentHeaderValue.clear();
  currentParseState = ParseState::METHOD;

  processRequest(file_discriptor);
  if (currentParseState == ParseState::DONE) {
    // check if request contains host header, if not then it's a invalid request
    bool hostHeaderFound = false;
    for (const auto &header : request.headers) {
      if (header.key == "host") {
        hostHeaderFound = true;
        break;
      }
    }
    if (!hostHeaderFound) {
      currentParseState = ParseState::PARSE_ERROR;
    }
  }
  return currentParseState == ParseState::DONE;
}

void RequestParser::processRequest(int file_discriptor) {
  char nextChar;
  bool readNextChar = true;
  while (currentParseState != ParseState::DONE ||
         currentParseState != ParseState::PARSE_ERROR) {
    if (readNextChar) {
      readNextChar = false;
      int bytesRead = ::read(file_discriptor, &nextChar, 1);
      if (bytesRead <= 0) {
        perror("Error reading from file discriptor");
        return;
      }
    }
    switch (currentParseState) {
    case ParseState::METHOD:
      parseMethod(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::URL:
      parseUrl(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION:
      parseVersion(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_HTTP_H:
      parseVersionHttpH(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_HTTP_T1:
      parseVersionHttpT1(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_HTTP_T2:
      parseVersionHttpT2(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_HTTP_P1:
      parseVersionHttpP1(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_SLASH:
      parseVersionSlash(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_MAJOR:
      parseVersionMajor(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_DOT:
      parseVersionDot(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::VERSION_MINOR:
      parseVersionMinor(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::REQUEST_LINE_END:
      parseRequestLineEnd(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::HEADER_KEY:
      parseHeaderKey(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::HEADER_DELIMITER:
      parseHeaderDelimiter(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::HEADER_VALUE:
      parseHeaderValue(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::HEADER_LINE_END_CR:
      parseHeaderLineEndCR(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::HEADER_LINE_END_LF:
      parseHeaderLineEndLF(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::END_OF_HEADER_CR:
      parseEndOfHeaderCR(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::END_OF_HEADER_LF:
      parseEndOfHeaderLF(nextChar, currentParseState, readNextChar);
      break;
    case ParseState::DONE:
    case ParseState::PARSE_ERROR:
      return;
    }
  }
}

void RequestParser::parseMethod(char nextChar, ParseState &currentParseState,
                                bool &readNextChar) {
  char space = 32; // ASCII value for space
  if (nextChar == space && currentMethod.size() > 0) {
    Method method = string_to_method(currentMethod);
    if (method == Method::METHOD_UNKOWN) {
      currentParseState = ParseState::PARSE_ERROR;
      return;
    }
    request.method = method;
    currentParseState = ParseState::URL;
    readNextChar = true;
    currentMethod.clear();
  } else if (std::isspace(nextChar) && currentMethod.size() == 0) {
    // ignore leading spaces
    readNextChar = true;
  } else if (std::isalpha(nextChar)) {
    currentMethod += std::toupper(nextChar);
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentMethod.clear();
  }
}

bool RequestParser::isValidUrlChar(char c) {

  if (std::isalnum(c)) {
    return true;
  }
  const char *allowedSpecialChars = "-._~:/?#[]@!$&'()*+,;=";
  for (const char *p = allowedSpecialChars; *p; ++p) {
    if (c == *p) {
      return true;
    }
  }
  return false;
}

void RequestParser::parseUrl(char nextChar, ParseState &currentParseState,
                             bool &readNextChar) {
  char space = 32; // ASCII value for space
  if (nextChar == space && currentUrl.size() > 0) {
    request.url = currentUrl;
    currentParseState = ParseState::VERSION;
    readNextChar = true;
    currentUrl.clear();
  } else if (std::isspace(nextChar) && currentUrl.size() == 0) {
    // ignore leading spaces
    readNextChar = true;
  } else if (isValidUrlChar(nextChar)) {
    currentUrl += nextChar;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentUrl.clear();
  }
}

void RequestParser::parseVersion(char nextChar, ParseState &currentParseState,
                                 bool &readNextChar) {
  char space = 32; // ASCII value for space
  if (std::isspace(nextChar) && nextChar != space) {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  } else if (currentVersion.size() == 0 && nextChar == space) {
    // ignore leading spaces
    readNextChar = true;
  } else if (std::isalnum(nextChar) || nextChar == '.' || nextChar == '/') {
    currentParseState = ParseState::VERSION_HTTP_H;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionHttpH(char nextChar,
                                      ParseState &currentParseState,
                                      bool &readNextChar) {
  if (std::isalpha(nextChar) && std::toupper(nextChar) == 'H') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_HTTP_T1;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionHttpT1(char nextChar,
                                       ParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::isalpha(nextChar) && std::toupper(nextChar) == 'T') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_HTTP_T2;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionHttpT2(char nextChar,
                                       ParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::isalpha(nextChar) && std::toupper(nextChar) == 'T') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_HTTP_P1;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionHttpP1(char nextChar,
                                       ParseState &currentParseState,
                                       bool &readNextChar) {
  if (std::isalpha(nextChar) && std::toupper(nextChar) == 'P') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_SLASH;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionSlash(char nextChar,
                                      ParseState &currentParseState,
                                      bool &readNextChar) {
  if (nextChar == '/') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_MAJOR;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionMajor(char nextChar,
                                      ParseState &currentParseState,
                                      bool &readNextChar) {
  if (std::isdigit(nextChar)) {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_DOT;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionDot(char nextChar,
                                    ParseState &currentParseState,
                                    bool &readNextChar) {
  if (nextChar == '.') {
    currentVersion += nextChar;
    currentParseState = ParseState::VERSION_MINOR;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseVersionMinor(char nextChar,
                                      ParseState &currentParseState,
                                      bool &readNextChar) {
  char space = 32; // ASCII value for space
  if (std::isdigit(nextChar)) {
    currentVersion += nextChar;
    Version version = string_to_version(currentVersion);
    currentVersion.clear();
    if (version == Version::VERSION_UNKOWN) {
      currentParseState = ParseState::PARSE_ERROR;
      return;
    }
    request.version = version;
    currentParseState = ParseState::REQUEST_LINE_END;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentVersion.clear();
  }
}

void RequestParser::parseRequestLineEnd(char nextChar,
                                        ParseState &currentParseState,
                                        bool &readNextChar) {
  char cr = 13; // ASCII value for carriage return
  char lf = 10; // ASCII value for line feed
  if (nextChar == cr) {
    readNextChar = true;
  } else if (nextChar == lf) {
    currentParseState = ParseState::HEADER_KEY;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
  }
}

void RequestParser::parseHeaderKey(char nextChar, ParseState &currentParseState,
                                   bool &readNextChar) {
  char cr = 13; // ASCII value for carriage return
  if (nextChar == cr) {
    currentParseState = ParseState::END_OF_HEADER_CR;
  } else if (nextChar == ':') {
    currentParseState = ParseState::HEADER_DELIMITER;
  } else if (std::isalnum(nextChar) || nextChar == '-') {
    if (std::isalpha(nextChar)) {
      nextChar = std::tolower(nextChar);
    }
    currentHeaderKey += nextChar;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentHeaderKey.clear();
  }
}

void RequestParser::parseHeaderDelimiter(char nextChar,
                                         ParseState &currentParseState,
                                         bool &readNextChar) {
  char space = 32; // ASCII value for space
  if (nextChar == space) {
    readNextChar = true;
  } else if (nextChar == ':') {
    currentParseState = ParseState::HEADER_VALUE;
    readNextChar = true;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
  }
}

void RequestParser::parseHeaderValue(char nextChar,
                                     ParseState &currentParseState,
                                     bool &readNextChar) {
  char cr = 13;         // ASCII value for carriage return
  char whitespace = 32; // ASCII value for space
  if (nextChar == cr) {
    currentParseState = ParseState::HEADER_LINE_END_CR;
  } else if (nextChar == whitespace) {
    readNextChar = true;
  } else {
    currentHeaderValue += nextChar;
    readNextChar = true;
  }
}

void RequestParser::parseHeaderLineEndCR(char nextChar,
                                         ParseState &currentParseState,
                                         bool &readNextChar) {
  char cr = 13; // ASCII value for carriage return
  if (nextChar == cr) {
    readNextChar = true;
    currentParseState = ParseState::HEADER_LINE_END_LF;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
  }
}

void RequestParser::parseHeaderLineEndLF(char nextChar,
                                         ParseState &currentParseState,
                                         bool &readNextChar) {
  char lf = 10; // ASCII value for line feed
  if (nextChar == lf) {
    readNextChar = true;
    request.headers.push_back(Header{currentHeaderKey, currentHeaderValue});
    currentHeaderKey.clear();
    currentHeaderValue.clear();
    currentParseState = ParseState::HEADER_KEY; // read the next header key
  } else {
    currentParseState = ParseState::PARSE_ERROR;
    currentHeaderValue.clear();
    currentHeaderKey.clear();
  }
}

void RequestParser::parseEndOfHeaderCR(char nextChar,
                                       ParseState &currentParseState,
                                       bool &readNextChar) {
  char cr = 13; // ASCII value for carriage return
  if (nextChar == cr) {
    readNextChar = true;
    currentParseState = ParseState::END_OF_HEADER_LF;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
  }
}

void RequestParser::parseEndOfHeaderLF(char nextChar,
                                       ParseState &currentParseState,
                                       bool &readNextChar) {
  char lf = 10; // ASCII value for line feed
  if (nextChar == lf) {
    currentParseState = ParseState::DONE;
  } else {
    currentParseState = ParseState::PARSE_ERROR;
  }
}

void RequestParser::reset() {
  currentMethod.clear();
  currentUrl.clear();
  currentVersion.clear();
  currentHeaderKey.clear();
  currentHeaderValue.clear();
  request = Request();
  currentParseState = ParseState::METHOD;
}

Request RequestParser::get_request() { return request; }

RequestParser::~RequestParser() {}