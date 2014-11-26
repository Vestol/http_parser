/*
 * This is free and unencumbered software released into the public domain.
 * 
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 * 
 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * successors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * For more information, please refer to <http://unlicense.org/>
 */
#include <parser.h>
#include <stdio.h>

/*
 * todo backslash quoting in header values
 */

enum {
  HTTP_TOKEN_CI = 0,
  HTTP_TOKEN_CS,
  HTTP_TOKEN_URI,
  HTTP_TOKEN_VCHAR
};

enum {
  CUR_HEADER_NONE = 0,
  CUR_HEADER_CONNECTION,
  CUR_HEADER_TRANSFER_ENCODING,
  CUR_HEADER_CONTENT_LENGTH,
  CUR_HEADER_EXPECT,
  CUR_HEADER_UPDATE,
  CUR_HEADER_HOST,
};


enum method_e {
  m_nostate,
  
 /*
  * CONNECT */
  m_c,
  m_co,
  m_con,
  m_conn,
  m_conne,
  m_connec,
  m_connect,

 /*
  * DELETE */
  m_d,
  m_de,
  m_del,
  m_dele,
  m_delet,
  m_delete,

 /*
  * GET */
  m_g,
  m_ge,
  m_get,
  
 /*
  * HEAD */
  m_h,
  m_he,
  m_hea,
  m_head,
  
 /*
  * OPTIONS */
  m_o,
  m_op,
  m_opt,
  m_opti,
  m_optio,
  m_option,
  m_options,
  
 /*
  * POST */
  m_p,
  m_po,
  m_pos,
  m_post,

 /*
  * PUT */
  m_pu,
  m_put,
  
 /*
  * TRACE */
  m_t,
  m_tr,
  m_tra,
  m_trac,
  m_trace,
  
 /*
  * Extension methods */
  m_ignore,
};

enum header_field_e {
  hf_nostate,
 /*
  * Connection */
  hf_c,
  hf_co,
  hf_con,
  hf_conn,
  hf_conne,
  hf_connec,
  hf_connect,
  hf_connecti,
  hf_connectio,
  hf_connection,

 /*
  * Content-Length */
  hf_cont,
  hf_conte,
  hf_conten,
  hf_content,
  hf_content_,
  hf_content_l,
  hf_content_le,
  hf_content_len,
  hf_content_leng,
  hf_content_lengt,
  hf_content_length,

 /*
  * Expect */
  hf_e,
  hf_ex,
  hf_exp,
  hf_expe,
  hf_expec,
  hf_expect,

 /*
  * Host */
  hf_h,
  hf_ho,
  hf_hos,
  hf_host,

 /*
  * Transfer-Encoding */
  hf_t,
  hf_tr,
  hf_tra,
  hf_tran,
  hf_trans,
  hf_transf,
  hf_transfe,
  hf_transfer,
  hf_transfer_,
  hf_transfer_e,
  hf_transfer_en,
  hf_transfer_enc,
  hf_transfer_enco,
  hf_transfer_encod,
  hf_transfer_encodi,
  hf_transfer_encodin,
  hf_transfer_encoding,

 /*
  * Update */
  hf_u,
  hf_up,
  hf_upd,
  hf_upda,
  hf_updat,
  hf_update,
  
  hf_ignore,
};

enum {
  s_mfirst_char = 0,
  s_mchars,
  s_msp_after,
  
 /*
  * uri */
  s_ufirst_char,
  s_uchars,
  s_usp_after,
  
 /**
  * HTTP/ */
  s_ph,
  s_pht,
  s_phtt,
  s_phttp,
  s_phttp_slash,

 /*
  * major version */
  s_pmajor,
  
 /*
  * dot between */
  s_pdot,
  
 /*
  * minor version */
  s_pminor,

 /*
  * end of line */
  s_eol_cr,
  s_eol_lf,
  
 /*
  * fields */
  s_ffirst_char,
  s_fchars,
  s_fcolon,
  
  s_veat_wsp,
  s_vfirst_char,
  s_vchars,
  s_vend,
  
 /*
  * end of headers */
  s_eoh_cr,
  s_eoh_lf,
  
  s_headers_done,

 /*
  * chunked transfer-encoding */
  s_chunked,
  s_require_chunklen,
  s_chunklen,
  s_chunkext,
  s_chunk_cr,
  s_chunk_lf,
  
  s_chunkdata,
  s_chunkdata_cont,
  s_chunkdata_cr,
  s_chunkdata_lf,
  
  s_chunk_last_lf,
  
/*
  s_chunked,
  s_require_chunklen,
  s_chunklen,
  s_chunkext,
  s_chunk_cr,
  s_chunk_lf,
  s_chunkdata,
  s_chunkdata_cr,
  s_chunkdata_lf,
*/
  
 /*
  * Content-Length */
  s_before_length,
  s_length,
  
  s_parser_done,
};



enum {
  vs_chunked_c,
  vs_chunked_ch,
  vs_chunked_chu,
  vs_chunked_chun,
  vs_chunked_chunk,
  vs_chunked_chunke,
  vs_chunked_chunked,
  
  vs_chunked_confirmed,
};

enum {
  ex_nostate,
  ex_1,
  ex_10,
  ex_100,
  ex_100_,
  ex_100_c,
  ex_100_co,
  ex_100_con,
  ex_100_cont,
  ex_100_conti,
  ex_100_contin,
  ex_100_continu,
  ex_100_continue,
  ex_ignore,
};

static const uint8_t token[4][256];
static const int8_t hex[256];

__attribute__((const))
static inline uint8_t ci_token(const char chr)
{
  return token[HTTP_TOKEN_CI][(uint8_t) chr];
}

__attribute__((const))
static inline uint8_t cs_token(const char chr)
{
  return token[HTTP_TOKEN_CS][(uint8_t) chr];
}

__attribute__((const))
static inline uint8_t uri_token(const char chr)
{
  return token[HTTP_TOKEN_URI][(uint8_t) chr];
}

__attribute__((const))
static inline uint8_t vchar_token(const char chr)
{
  return token[HTTP_TOKEN_VCHAR][(uint8_t) chr];
}

__attribute__((const)) int8_t hex_token(const char chr) {
  return hex[(uint8_t) chr];
}


static inline int call_on_method(http_parser *p, const char *in, size_t size, const http_parser_settings *s)
{
  int rv;
  
  if (size == 0)
    return 0;
  
  if ( (rv = s->on_method(p, in, size) ) ) {
    p->error = 1;
    return rv;
  }
  
  return 0;
}

static inline int call_on_uri(http_parser *p, const char *in, size_t size, const http_parser_settings *s)
{
  int rv;
  
  if (size == 0)
    return 0;
  
  if ( (rv = s->on_uri(p, in, size) ) ) {
    p->error = 1;
    return rv;
  }
  
  return 0;
}

static inline int call_on_header_field(http_parser *p, const char *in, size_t size, const http_parser_settings *s)
{
  int rv;
  
  if (size == 0)
    return 0;
  
  if ( (rv = s->on_header_field(p, in, size) ) ) {
    p->error = 1;
    return rv;
  }
  
  return 0;
}

static inline int call_on_header_value(http_parser *p, const char *in, size_t size, const http_parser_settings *s)
{
  int rv;
  
  if (size == 0)
    return 0;
  
  if ( (rv = s->on_header_value(p, in, size) ) ) {
    p->error = 1;
    return rv;
  }
  
  return 0;
}

/**
 * Check for chunked transfer-encoding
 * 
 * Chunked encoding must be the last encoding,
 * So we need to generate an error if chunked is listed but 
 * not last.
 */
static inline void check_for_chunked(http_parser *p, char chr)
{
  switch (p->matcher) {
    case vs_chunked_c:
      if (chr == 'c' || chr == 'C') {
        p->matcher = vs_chunked_ch;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
    
    case vs_chunked_ch:
      if (chr == 'h' || chr == 'H') {
        p->matcher = vs_chunked_chu;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
    
    case vs_chunked_chu:
      if (chr == 'u' || chr == 'U') {
        p->matcher = vs_chunked_chun;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
      
    case vs_chunked_chun:
      if (chr == 'n' || chr == 'N') {
        p->matcher = vs_chunked_chunk;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
    
    case vs_chunked_chunk:
      if (chr == 'k' || chr == 'K') {
        p->matcher = vs_chunked_chunke;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
    
    case vs_chunked_chunke:
      if (chr == 'e' || chr == 'E') {
        p->matcher = vs_chunked_chunked;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
      
    case vs_chunked_chunked:
      if (chr == 'd' || chr == 'D') {
        p->matcher = vs_chunked_confirmed;
        p->flags |= HTTP_FLAG_CHUNKED;
        break;
      }
      
      p->matcher = vs_chunked_c;
      break;
      
    case vs_chunked_confirmed:
      if (chr == ' ' || chr == '\t') {
        break;
      }
      
      p->flags &= (~HTTP_FLAG_CHUNKED);
    
    default:
      p->matcher = vs_chunked_c;
  }
}

void http_parser_init(http_parser *out)
{
  *out = (http_parser) {
    .state = 0
  };
}

#define matcher_state(CHR, STATE) if (cs_token(CHR) == CHR) { p->matcher = STATE; break; } p->matcher = m_ignore; break

static inline void parser_method_matcher(http_parser *p, char chr)
{  
  switch (p->matcher)
  {
    case m_nostate:
      switch (cs_token(chr) )
      {
        case 'C': p->matcher = m_c; break;
        case 'D': p->matcher = m_d; break;
        case 'G': p->matcher = m_g; break;
        case 'H': p->matcher = m_h; break;
        case 'O': p->matcher = m_o; break;
        case 'P': p->matcher = m_p; break;
        case 'T': p->matcher = m_t; break;
        default:  p->matcher = m_ignore;
      }
      
      break;
    
    // CONNECT
    case m_c: matcher_state('O', m_co);
    case m_co: matcher_state('N', m_con);
    case m_con: matcher_state('N', m_conn);
    case m_conn: matcher_state('E', m_conne);
    case m_conne: matcher_state('C', m_connec);
    case m_connec:
      if (cs_token(chr) == 'T') {
        p->method = HTTP_METHOD_CONNECT;
        p->matcher = m_connect;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // DELETE
    case m_d: matcher_state('E', m_de);
    case m_de: matcher_state('L', m_del);
    case m_del: matcher_state('E', m_dele);
    case m_dele: matcher_state('T', m_delet);
    case m_delet:
     if (cs_token(chr) == 'E') {
        p->method = HTTP_METHOD_DELETE;
        p->matcher = m_delete;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // GET
    case m_g: matcher_state('E', m_ge);
    case m_ge:
      if (cs_token(chr) == 'T') {
        p->method = HTTP_METHOD_GET;
        p->matcher = m_get;
        break;
      }
      
      p->matcher = m_ignore;
      break;
      
    // HEAD
    case m_h: matcher_state('E', m_he);
    case m_he: matcher_state('A', m_hea);
    case m_hea:
      if (cs_token(chr) == 'D') {
        p->method = HTTP_METHOD_HEAD;
        p->matcher = m_head;
        break;
      }
      
      p->matcher = m_ignore;
      break;

    // OPTIONS
    case m_o: matcher_state('P', m_op);
    case m_op: matcher_state('T', m_opt);
    case m_opt: matcher_state('I', m_opti);
    case m_opti: matcher_state('O', m_optio);
    case m_optio: matcher_state('N', m_option);
    case m_option:
      if (cs_token(chr) == 'S') {
        p->method = HTTP_METHOD_OPTIONS;
        p->matcher = m_options;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // POST | PUT
    case m_p:
      if (cs_token(chr) == 'O') {
        p->matcher = m_po;
        break;
      }
      else if (cs_token(chr) == 'U') {
        p->matcher = m_pu;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // POST
    case m_po:matcher_state('S', m_pos);
    case m_pos:
      if (cs_token(chr) == 'T') {
        p->method = HTTP_METHOD_POST;
        p->matcher = m_post;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // PUT
    case m_pu:
      if (cs_token(chr) == 'T') {
        p->method = HTTP_METHOD_PUT;
        p->matcher = m_put;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    // TRACE
    case m_t: matcher_state('R', m_tr);
    case m_tr: matcher_state('A', m_tra);
    case m_tra: matcher_state('C', m_trac);
    case m_trac:
      if (cs_token(chr) == 'E') {
        p->method = HTTP_METHOD_TRACE;
        p->matcher = m_trace;
        break;
      }
      
      p->matcher = m_ignore;
      break;
    
    
    case m_connect:
    case m_delete:
    case m_get:
    case m_head:
    case m_options:
    case m_post:
    case m_put:
    case m_trace:
      p->method = HTTP_METHOD_EXT;
      p->matcher = m_ignore;
      break;
    
    case m_ignore:
      break;
     
  }
}
#undef matcher_state

#define matcher_state(CHR, STATE) if (ci_token(CHR) == CHR) { p->matcher = STATE; break; } p->matcher = hf_ignore; break

static inline void parser_field_matcher(http_parser *p, char chr)
{
  switch (p->matcher)
  {
    case hf_nostate:
      switch (ci_token(chr) )
      {
        case 'c': p->matcher = hf_c; break;
        case 'e': p->matcher = hf_e; break;
        case 'h': p->matcher = hf_h; break;
        case 't': p->matcher = hf_t; break;
        case 'u': p->matcher = hf_u; break;
        default:  p->matcher = hf_ignore;
      }
      
      break;
    
    // connection:
    case hf_c: matcher_state('o', hf_co);
    case hf_co: matcher_state('n', hf_con);
    case hf_con:
      if (ci_token(chr) == 'n') {
        p->matcher = hf_conn;
        break;
      }
      else if (ci_token(chr) == 't') {
        p->matcher = hf_cont;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    case hf_conn: matcher_state('e', hf_conne);
    case hf_conne: matcher_state('c', hf_connec);
    case hf_connec: matcher_state('t', hf_connect);
    case hf_connect: matcher_state('i', hf_connecti);
    case hf_connecti: matcher_state('o', hf_connectio);
    case hf_connectio:
      if (ci_token(chr) == 'n') {
        p->matcher = hf_connection;
        p->header = CUR_HEADER_CONNECTION;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    
    // Content-Length:
    case hf_cont: matcher_state('e', hf_conte);
    case hf_conte: matcher_state('n', hf_conten);
    case hf_conten: matcher_state('t', hf_content);
    case hf_content: matcher_state('-', hf_content_);
    case hf_content_: matcher_state('l', hf_content_l);
    case hf_content_l: matcher_state('e', hf_content_le);
    case hf_content_le: matcher_state('n', hf_content_len);
    case hf_content_len: matcher_state('g', hf_content_leng);
    case hf_content_leng: matcher_state('t', hf_content_lengt);
    case hf_content_lengt:
      if (ci_token(chr) == 'h') {
        p->matcher = hf_content_length;
        p->header = CUR_HEADER_CONTENT_LENGTH;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    // Expect:
    
    case hf_e: matcher_state('x', hf_ex);
    case hf_ex: matcher_state('p', hf_exp);
    case hf_exp: matcher_state('e', hf_expe);
    case hf_expe: matcher_state('c', hf_expec);
    case hf_expec:
      if (ci_token(chr) == 't') {
        p->matcher = hf_expect;
        p->header = CUR_HEADER_EXPECT; 
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    
    // host:
    case hf_h: matcher_state('o', hf_ho);
    case hf_ho: matcher_state('s', hf_hos);
    case hf_hos:
      if (ci_token(chr) == 't') {
        p->matcher = hf_host;
        p->header = CUR_HEADER_HOST;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    // Transfer-Encoding:
    
    case hf_t: matcher_state('r', hf_tr);
    case hf_tr: matcher_state('a', hf_tra);
    case hf_tra: matcher_state('n', hf_tran);
    case hf_tran: matcher_state('s', hf_trans);
    case hf_trans: matcher_state('f', hf_transf);
    case hf_transf: matcher_state('e', hf_transfe);
    case hf_transfe: matcher_state('r', hf_transfer);
    case hf_transfer: matcher_state('-', hf_transfer_);
    case hf_transfer_: matcher_state('e', hf_transfer_e);
    case hf_transfer_e: matcher_state('n', hf_transfer_en);
    case hf_transfer_en: matcher_state('c', hf_transfer_enc);
    case hf_transfer_enc: matcher_state('o', hf_transfer_enco);
    case hf_transfer_enco: matcher_state('d', hf_transfer_encod);
    case hf_transfer_encod: matcher_state('i', hf_transfer_encodi);
    case hf_transfer_encodi: matcher_state('n', hf_transfer_encodin);
    case hf_transfer_encodin:
      if (ci_token(chr) == 'g') {
        p->matcher = hf_transfer_encoding;
        p->header = CUR_HEADER_TRANSFER_ENCODING;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    // Update:
    
    case hf_u: matcher_state('p', hf_up);
    case hf_up: matcher_state('d', hf_upd);
    case hf_upd: matcher_state('a', hf_upda);
    case hf_upda: matcher_state('t', hf_updat);
    case hf_updat:
      if (ci_token(chr) == 'e') {
        p->matcher = hf_update;
        p->header = CUR_HEADER_UPDATE;
        break;
      }
      
      p->matcher = hf_ignore;
      break;
    
    // ---
    case hf_connection:
    case hf_content_length:
    case hf_expect:
    case hf_host:
    case hf_transfer_encoding:
    case hf_update:
      p->header = CUR_HEADER_NONE;
      p->matcher = hf_ignore;
      break;
    
    case hf_ignore:
      break;
  }
}


#undef matcher_state
#define matcher_state(CHR, STATE) if (ci_token(CHR) == CHR) { p->matcher = STATE; break; } p->matcher = ex_ignore; break

void parser_expect_continue_matcher(http_parser *p, char chr)
{
  switch (p->matcher)
  {
    case ex_nostate: matcher_state('1', ex_1);
    case ex_1: matcher_state('0', ex_10);
    case ex_10: matcher_state('0', ex_100);
    case ex_100: matcher_state('-', ex_100_);
    case ex_100_: matcher_state('c', ex_100_c);
    case ex_100_c: matcher_state('o', ex_100_co);
    case ex_100_co: matcher_state('n', ex_100_con);
    case ex_100_con: matcher_state('t', ex_100_cont);
    case ex_100_cont: matcher_state('i', ex_100_conti);
    case ex_100_conti: matcher_state('n', ex_100_contin);
    case ex_100_contin: matcher_state('u', ex_100_continu);
    case ex_100_continu:
      if (ci_token(chr) == 'e') {
        p->matcher = ex_100_continue;
        p->flags |= HTTP_FLAG_EXPECT_CONTINUE;
        break;
      }
      
      p->matcher = ex_ignore;
      break;
    
    case ex_100_continue:
      if (chr != ' ' && chr != '\t') {
        break;
      }
      
      p->flags &= (~HTTP_FLAG_EXPECT_CONTINUE);
      p->matcher = ex_ignore;
      break;
    
    case ex_ignore:
      break;
  }
}

#undef matcher_state


size_t http_parser_execute(http_parser *p, const char *in, size_t size, const http_parser_settings *settings)
{
  const char *chr;
  const char *endptr;
  const char *marker = in;
  char t;
  
  endptr = in + size;
  
  for (chr = in; chr < endptr; chr++) {
    
    switch (p->state)
    {
      case s_mfirst_char:
        if (ci_token(*chr) ) {
          marker = chr;
          p->state = s_mchars;
          p->matcher = m_nostate;
          parser_method_matcher(p, *chr);
          break;
        }
        
        goto error;
        
            
      case s_mchars:
        if (cs_token(*chr) ) {
          parser_method_matcher(p, *chr);
          break;
        }
        
        
        
//sp_after_label:
      // fall through [SP]
      case s_msp_after:
        call_on_method(p, marker, chr - marker, settings);
        p->matcher = 0;
        
        if (*chr == ' ') {
          p->state = s_ufirst_char;
          break;
        }
        
        goto error;
      
/* fix this: validate the uri format */
/*  the full uri for CONNECT and [*] for options */
      case s_ufirst_char:
        marker = chr;
        if (uri_token(*chr) ) {
          p->state = s_uchars;
          break;
        }
        
        goto error;
      
      case s_uchars:
        if (uri_token(*chr) ) {
          break;
        }
      
      // fall through [SP]
      case s_usp_after:
        call_on_uri(p, marker, chr - marker, settings);
        if (*chr == ' ') {
          p->state = s_ph;
          break;
        }
        
        goto error;
      
      // protocol
      case s_ph:
        if (*chr == 'H') {
          p->state = s_pht;
          break;
        }
        
        goto error;
      
      case s_pht:
        if (*chr == 'T') {
          p->state = s_phtt;
          break;
        }
        
        goto error;
      
      case s_phtt:
        if (*chr == 'T') {
          p->state = s_phttp;
          break;
        }
        
        goto error;
      
      case s_phttp:
        if (*chr == 'P') {
          p->state = s_phttp_slash;
          break;
        }
        
        goto error;
      
      case s_phttp_slash:
        if (*chr == '/') {
          p->state = s_pmajor;
          break;
        }
        
        goto error;
      
      case s_pmajor:
        if (*chr >= '0' && *chr <= '9') {
          p->major = (*chr) - '0';
          p->state = s_pdot;
          break;
        }
        
        goto error;
      
      case s_pdot:
        if (*chr == '.') {
          p->state = s_pminor;
          break;
        }
        
        goto error;
      
      case s_pminor:
        if (*chr >= '0' && *chr <= '9') {
          p->minor = (*chr) - '0';
          p->state = s_eol_cr;
          break;
        }
        
        goto error;
        
      
      // line ending [CRLF]
      
      case s_eol_cr:
        if (*chr == '\r') {
          p->state = s_eol_lf;
          break;
        }
        
        goto error;
      
      case s_eol_lf:
        if (*chr == '\n') {
          p->state = s_ffirst_char;
          break;
        }
        
        goto error;
      
      // if this is \r, we are looking for another CRLF
      /* todo: check for header folding and reject (400 bad request) */
      case s_ffirst_char:
        if (*chr == '\r') {
          p->state = s_eoh_cr;
          break;
        }
        
        marker = chr;
        p->state = s_fchars;
        p->matcher = 0;
        parser_field_matcher(p, *chr);
        break;
            
      case s_fchars:
        if (ci_token(*chr) ) {
          parser_field_matcher(p, *chr);
          break;
        }
        
      // fall through [:]
      case s_fcolon:
        settings->on_header_field(p, marker, chr - marker);
        
        if (*chr == ':') {
          p->state = s_veat_wsp;
          break;
        }
        
        goto error;
      
      // eat preceding white space
      case s_veat_wsp:
        if (*chr == ' ' || *chr == '\t') {
          break;
        }
        
        p->state = s_vfirst_char;
      
      // fall through first header value char
      // change to detect keep-alive, close, 101-continue and so on
      case s_vfirst_char:
        if (vchar_token(*chr) ) { 
          marker = chr;
          p->matcher = 0;
          if (p->header == CUR_HEADER_TRANSFER_ENCODING) {
            check_for_chunked(p, *chr);
            p->state = s_vchars;
            break;
          }
          else if (p->header == CUR_HEADER_EXPECT) {
            parser_expect_continue_matcher(p, *chr);
            p->state = s_vchars;
            break;
          }
          else if (p->header == CUR_HEADER_CONTENT_LENGTH) {
            if (vchar_token(*chr) >= '0' && vchar_token(*chr) <= '9') {
              p->content_length = (*chr - '0');
              p->state = s_vchars;
              break;
            }
            
            goto error;
          }
                    
          p->state = s_vchars;       
          break;
        }
        
        goto error;
      
      case s_vchars:
        if (vchar_token(*chr) ) {
          if (p->header == CUR_HEADER_TRANSFER_ENCODING) {
            check_for_chunked(p, *chr);
            break;
          }
          else if (p->header == CUR_HEADER_EXPECT) {
            parser_expect_continue_matcher(p, *chr);
            break;
          }
          else if (p->header == CUR_HEADER_CONTENT_LENGTH) {
            if (vchar_token(*chr) >= '0' && vchar_token(*chr) <= '9') {
              p->content_length = (p->content_length * 10) + (*chr - '0');
              break;
            }
            else if (vchar_token(*chr) == ' ' || vchar_token(*chr) == '\t') {
              break;
            }
            
            goto error;
          }
          
          break;
        }
        
        p->state = s_vend;
      
      case s_vend:
        p->header = 0;
        settings->on_header_value(p, marker, chr - marker);
        p->state = s_eol_cr;
        chr--;
        break;
        
      
      
      case s_eoh_cr:
        if (*chr != '\n') {
          goto error;
        }
        
        p->state = s_eoh_lf;
      
      case s_eoh_lf:
        p->state = s_headers_done;
        settings->on_headers_finished(p);
      
     /*******************************************
      * Parsing content body
      *******************************************/
      
      case s_headers_done:
        
        if (p->flags & HTTP_FLAG_CHUNKED) {
          p->state = s_chunked;
          break;
        }
        else if (p->content_length) {
          p->state = s_before_length;
          break;
        }
        
        // no body
        else {
          settings->on_finished(p);
          return chr - in + 1;
        }
        
      
      // start chunked
      case s_chunked:
        p->state = s_require_chunklen;
        p->chunk_length = 0;
      
      case s_require_chunklen:
        if (hex_token(*chr) != -1) {
          p->chunk_length = hex_token(*chr);
          p->state = s_chunklen;
          break;
        }
        
        goto error;
      
      case s_chunklen:
        if (hex_token(*chr) != -1) {
          p->chunk_length <<= 4;
          p->chunk_length += hex_token(*chr);
          break;
        }
        else if (*chr == ';') {
          p->state = s_chunkext;
          break;
        }
        else if (*chr == '\r') {
          p->state = s_chunk_lf;
          break;
        }
        
        goto error;
        
      
      case s_chunkext:
        if (*chr != '\r') {
          break;
        }
        
        p->state = s_chunk_lf;
        break;
        
      
      case s_chunk_lf:
        if (*chr == '\n') {
          if (!p->chunk_length) {
            p->state = s_parser_done;
            settings->on_finished(p);
            return chr - in + 1;
          } 
          p->state = s_chunkdata;
          break;
        }
        
        goto error;
      
      case s_chunkdata:
        marker = chr;
        p->state = s_chunkdata_cont;
        p->chunk_length--;
        break;
        
      case s_chunkdata_cont:
        if (p->chunk_length) {
          p->chunk_length--;
          break;
        }
        
        settings->on_body(p, marker, chr - marker);
        
      case s_chunkdata_cr:
        if (*chr == '\r') {
          p->state = s_chunkdata_lf;
          break;
        }
        
        goto error;
      
      case s_chunkdata_lf:
        if (*chr == '\n') {
          p->state = s_chunked;
          break;
        }
        
        goto error;
      
    // read content-length number of bytes
      case s_before_length:
        marker = chr;
        p->state = s_length;
        
      case s_length:
        if ( --p->content_length) {
          break;
        }
        
        p->state = s_parser_done;

      case s_parser_done:
        settings->on_body(p, marker, chr - marker + 1);
        settings->on_finished(p);
        return chr - in + 1;
      
      default:
        
        printf("Error as in [default rule] %c\n\n", *chr);
        goto error;
        
    }
  }
  
  // fix this to make sure
  // callbacks are call for all partial values
  
  if (p->state == s_mchars) {
    call_on_method(p, marker, chr - marker, settings);
  }
  else if (p->state == s_uchars) {
    call_on_uri(p, marker, chr - marker, settings);
  }
  else if (p->state == s_fchars) {
    call_on_header_field(p, marker, chr - marker, settings);
  }
  else if (p->state == s_vchars) {
    call_on_header_value(p, marker, chr - marker, settings);
  }
  else if (p->state == s_length) {
    settings->on_body(p, marker, chr - marker);
  }
  else {
    
    //printf("s_fchars: %d\ns_fupdate: %d\n\n", s_fchars, s_fupdate);
    printf("EndLoop\nState: %u\n\n", p->state);
  }
  
  return size;
  
error:
  printf("Parse error\n\n");
  p->error = 1;
  return chr - in;
}

static const int8_t hex[256] = {
  //NULL SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS  TAB   LF   VT   FF   CR   SO   SI
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  // SP    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
     -1,  10,  11,  12,  13,  14,  15,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //  P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
     -1,  10,  11,  12,  13,  14,  15,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
      
  //  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~  DEL
     -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,
  };

static const uint8_t token[4][256] = {
  [HTTP_TOKEN_CI] = {
  //NULL SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS  TAB   LF   VT   FF   CR   SO   SI
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  // SP    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
      0,  '!',  0,  '#', '$', '%', '&', '\'', 0,   0,  '*', '+',  0,  '-', '.',  0,
      
  //  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0,   0,   0,   0,   0,   0,
      
  //  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
      0,  'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
      
  //  P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
     'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',  0,   0,   0,  '^', '_',
      
  //  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
     '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
      
  //  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~  DEL
     'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',   0
  },
  [HTTP_TOKEN_CS] = {
  //NULL SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS  TAB   LF   VT   FF   CR   SO   SI
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  // SP    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
      0,  '!',  0,  '#', '$', '%', '&', '\'', 0,   0,  '*', '+',  0,  '-', '.',  0,
      
  //  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',  0,   0,   0,   0,   0,   0,
      
  //  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
      0,  'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
      
  //  P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
     'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',  0,   0,   0,  '^', '_',
      
  //  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
     '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
      
  //  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~  DEL
     'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',   0
  },
  [HTTP_TOKEN_URI] = {
  //NULL SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS  TAB   LF   VT   FF   CR   SO   SI
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  // SP    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
      0,  48,   0,  44,  49,  58,  50,  51,  52,  53,  54,  55,  56,  37,  38,  42,
      
  //  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
     27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  41,  57,   0,  58,   0,  43,
      
  //  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
     47,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
      
  //  P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
     16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,  45,   0,  46,   0,  39,
     
  //  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
      0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,  15,
      
  //  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~  DEL
     16,  17,  18,  19,  20,  21,  22,  23,  24,  25,  26,   0,   0,   0,  40,   0
  },
  [HTTP_TOKEN_VCHAR] = {
  //NULL SOH  STX  ETX  EOT  ENQ  ACK  BEL   BS  TAB   LF   VT   FF   CR   SO   SI
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  //DLE  DC1  DC2  DC3  DC4  NAK  SYN  ETB  CAN   EM  SUB  ESC   FS   GS   RS   US
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
      
  // SP    !    "    #    $    %    &    '    (    )    *    +    ,    -    .    /
    ' ',  '!', '"', '#', '$', '%', '&', '\'','(', ')', '*', '+', ',', '-', '.', '/',
      
  //  0    1    2    3    4    5    6    7    8    9    :    ;    <    =    >    ?
     '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', ':', ';', '<', '=', '>', '?',
      
  //  @    A    B    C    D    E    F    G    H    I    J    K    L    M    N    O
     '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O',
      
  //  P    Q    R    S    T    U    V    W    X    Y    Z    [    \    ]    ^    _
     'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\',']', '^', '_',
      
  //  `    a    b    c    d    e    f    g    h    i    j    k    l    m    n    o
     '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o',
      
  //  p    q    r    s    t    u    v    w    x    y    z    {    |    }    ~  DEL
     'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', '~',  0,
  
  // (*TEXT rule) obs-text
     0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
     0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F,
     0xA0, 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8, 0xA9, 0xAA, 0xAB, 0xAC, 0xAD, 0xAE, 0xAF,
     0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9, 0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF,
     0xC0, 0xC1, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8, 0xC9, 0xCA, 0xCB, 0xCC, 0xCD, 0xCE, 0xCF,
     0xD0, 0xD1, 0xD2, 0xD3, 0xD4, 0xD5, 0xD6, 0xD7, 0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
     0xE0, 0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
     0xF0, 0xF1, 0xF2, 0xF3, 0xF4, 0xF5, 0xF6, 0xF7, 0xF8, 0xF9, 0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xFF
  }
};
