#ifndef HTTP_PARSER_VESTOL
#define HTTP_PARSER_VESTOL

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

enum {
  HTTP_METHOD_EXT = 0,
  HTTP_METHOD_CONNECT,
  HTTP_METHOD_DELETE,
  HTTP_METHOD_GET,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_OPTIONS,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_TRACE,
};

enum {
  HTTP_FLAG_CLOSE = 1,
  HTTP_FLAG_CHUNKED = 2,
  HTTP_FLAG_EXPECT_CONTINUE = 4,
};

typedef struct http_parser http_parser;
typedef struct http_parser_settings http_parser_settings;

typedef int (*http_event_f)(http_parser *);
typedef int (*http_data_f)(http_parser *, const char *, size_t);

struct http_parser {
  unsigned state : 8;
  unsigned matcher : 8; 
  unsigned flags : 8;
  
  unsigned method : 8;
  
  unsigned error : 4;
  unsigned major : 4;
  unsigned minor : 4;
  unsigned header : 4;
  
  uint64_t content_length;
  uint64_t chunk_length;
};

struct http_parser_settings {
  http_data_f  on_method;
  http_data_f  on_uri;
  http_data_f  on_header_field;
  http_data_f  on_header_value;
  http_event_f on_headers_finished;
  http_data_f  on_body;
  http_event_f on_finished;
};

void http_parser_init(http_parser *out);
size_t http_parser_execute(http_parser *p, const char *in, size_t size, const http_parser_settings *settings);

// bool http_parser_is_chunked(const http_parser *p);
// bool http_parser_should_keep_alive(const http_parser *p);

#endif
