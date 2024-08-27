#pragma once

#include "API.h"
#include <string>
#include <vector>

namespace http_parser {
enum class Method {
  METHOD_GET,
  METHOD_POST,
  METHOD_PUT,
  METHOD_DELETE = 3,
  METHOD_HEAD,
  METHOD_OPTIONS,
  METHOD_PATCH,
  METHOD_TRACE,
  METHOD_CONNECT,
  METHOD_UNKOWN
};

enum class Version {
  HTTP_1_1,
  VERSION_UNKOWN,
};

struct Header {
  std::string key;
  std::string value;
  Header(const std::string &k, const std::string &v) : key(k), value(v) {}
  Header() = default;
};

struct Request {
  Method method;
  std::string url;
  Version version;
  std::vector<Header> headers;
};

std::string PARSER_EXPORT method_to_string(Method m);
std::string PARSER_EXPORT version_to_string(Version v);
Method PARSER_EXPORT string_to_method(const std::string &s);
Version PARSER_EXPORT string_to_version(const std::string &s);

enum class StatusCode {
  OK = 200,
  CREATED = 201,
  ACCEPTED = 202,
  NO_CONTENT = 204,
  MOVED_PERMANENTLY = 301,
  FOUND = 302,
  SEE_OTHER = 303,
  NOT_MODIFIED = 304,
  BAD_REQUEST = 400,
  UNAUTHORIZED = 401,
  FORBIDDEN = 403,
  NOT_FOUND = 404,
  METHOD_NOT_ALLOWED = 405,
  REQUEST_TIMEOUT = 408,
  INTERNAL_SERVER_ERROR = 500,
  NOT_IMPLEMENTED = 501,
  BAD_GATEWAY = 502,
  SERVICE_UNAVAILABLE = 503,
  GATEWAY_TIMEOUT = 504,
  UNKOWN = 0,
};

std::string PARSER_EXPORT status_code_to_string(StatusCode s);
StatusCode PARSER_EXPORT string_to_status_code(const std::string &s);

struct Response {
  Version version;
  StatusCode status_code;
  std::string status_message;
  std::vector<Header> headers;
};

}; // namespace http_parser