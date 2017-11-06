#ifndef KV_PARSER_H
#define KV_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <ctype.h>

typedef struct {
	void* data; // user data pointer
	int   status;
} kv_parser;

typedef int (*kv_data_cb)(kv_parser*, const char*, size_t);
typedef int (*kv_notify_cb)(kv_parser*);

typedef struct {
	kv_data_cb on_key;
	kv_notify_cb on_key_end;
	kv_data_cb on_val;
	kv_notify_cb on_val_end;
	// {ws*}{w1?}key{w1?}{ws*}{s1}{ws*}{w2?}val{w2?}{ws*}{s2}
	char s1;
	char s2;
	char w1;
	char w2; // not used
} kv_parser_settings;


extern void kv_parser_init(kv_parser* parser);
extern size_t kv_parser_execute(kv_parser* parser, kv_parser_settings* settings, const char* data, size_t size);

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif // KV_PARSER_H
