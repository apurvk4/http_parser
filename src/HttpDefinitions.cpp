#include "HttpDefinitions.hpp"

using http_parser::Method;
using http_parser::StatusCode;
using http_parser::Version;

std::string http_parser::method_to_string(Method method) {
  switch (method) {
  case Method::METHOD_GET:
    return "GET";
  case Method::METHOD_POST:
    return "POST";
  case Method::METHOD_PUT:
    return "PUT";
  case Method::METHOD_DELETE:
    return "DELETE";
  case Method::METHOD_HEAD:
    return "HEAD";
  case Method::METHOD_OPTIONS:
    return "OPTIONS";
  case Method::METHOD_PATCH:
    return "PATCH";
  case Method::METHOD_TRACE:
    return "TRACE";
  case Method::METHOD_CONNECT:
    return "CONNECT";
    break;
  case Method::METHOD_UNKOWN:
    break;
  }
  return "UNKOWN";
}

Method http_parser::string_to_method(const std::string &s) {
  if (s == "GET") {
    return Method::METHOD_GET;
  } else if (s == "POST") {
    return Method::METHOD_POST;
  } else if (s == "PUT") {
    return Method::METHOD_PUT;
  } else if (s == "DELETE") {
    return Method::METHOD_DELETE;
  } else if (s == "HEAD") {
    return Method::METHOD_HEAD;
  } else if (s == "OPTIONS") {
    return Method::METHOD_OPTIONS;
  } else if (s == "PATCH") {
    return Method::METHOD_PATCH;
  } else if (s == "TRACE") {
    return Method::METHOD_TRACE;
  } else if (s == "CONNECT") {
    return Method::METHOD_CONNECT;
  }
  return Method::METHOD_UNKOWN;
}

std::string http_parser::version_to_string(Version version) {
  switch (version) {
  case Version::HTTP_1_1:
    return "HTTP/1.1";
  case Version::VERSION_UNKOWN:
    break;
  }
  return "VERSION_UNKOWN";
}

Version http_parser::string_to_version(const std::string &s) {
  if (s == "HTTP/1.1") {
    return Version::HTTP_1_1;
  }
  return Version::VERSION_UNKOWN;
}

StatusCode http_parser::string_to_status_code(const std::string &s) {
  if (s == "200") {
    return StatusCode::OK;
  } else if (s == "201") {
    return StatusCode::CREATED;
  } else if (s == "202") {
    return StatusCode::ACCEPTED;
  } else if (s == "204") {
    return StatusCode::NO_CONTENT;
  } else if (s == "301") {
    return StatusCode::MOVED_PERMANENTLY;
  } else if (s == "302") {
    return StatusCode::FOUND;
  } else if (s == "303") {
    return StatusCode::SEE_OTHER;
  } else if (s == "304") {
    return StatusCode::NOT_MODIFIED;
  } else if (s == "400") {
    return StatusCode::BAD_REQUEST;
  } else if (s == "401") {
    return StatusCode::UNAUTHORIZED;
  } else if (s == "403") {
    return StatusCode::FORBIDDEN;
  } else if (s == "404") {
    return StatusCode::NOT_FOUND;
  } else if (s == "405") {
    return StatusCode::METHOD_NOT_ALLOWED;
  } else if (s == "408") {
    return StatusCode::REQUEST_TIMEOUT;
  } else if (s == "500") {
    return StatusCode::INTERNAL_SERVER_ERROR;
  } else if (s == "501") {
    return StatusCode::NOT_IMPLEMENTED;
  } else if (s == "502") {
    return StatusCode::BAD_GATEWAY;
  } else if (s == "503") {
    return StatusCode::SERVICE_UNAVAILABLE;
  } else if (s == "504") {
    return StatusCode::GATEWAY_TIMEOUT;
  }
  return StatusCode::UNKOWN;
}

std::string http_parser::status_code_to_string(StatusCode status_code) {
  switch (status_code) {
  case StatusCode::OK:
    return "200";
  case StatusCode::CREATED:
    return "201";
  case StatusCode::ACCEPTED:
    return "202";
  case StatusCode::NO_CONTENT:
    return "204";
  case StatusCode::MOVED_PERMANENTLY:
    return "301";
  case StatusCode::FOUND:
    return "302";
  case StatusCode::SEE_OTHER:
    return "303";
  case StatusCode::NOT_MODIFIED:
    return "304";
  case StatusCode::BAD_REQUEST:
    return "400";
  case StatusCode::UNAUTHORIZED:
    return "401";
  case StatusCode::FORBIDDEN:
    return "403";
  case StatusCode::NOT_FOUND:
    return "404";
  case StatusCode::METHOD_NOT_ALLOWED:
    return "405";
  case StatusCode::REQUEST_TIMEOUT:
    return "408";
  case StatusCode::INTERNAL_SERVER_ERROR:
    return "500";
  case StatusCode::NOT_IMPLEMENTED:
    return "501";
  case StatusCode::BAD_GATEWAY:
    return "502";
  case StatusCode::SERVICE_UNAVAILABLE:
    return "503";
  case StatusCode::GATEWAY_TIMEOUT:
    return "504";
  case StatusCode::UNKOWN:
    break;
  }
  return 0;
}