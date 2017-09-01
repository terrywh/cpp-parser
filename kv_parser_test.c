#include "kv_parser.h"
#include "string.h"
#include "stdio.h"


int on_key(kv_parser* parser, const char* at, size_t len) {
	printf("%.*s", len, at);
	return 0;
}
int on_key_end(kv_parser* parser) {
	printf(" -> ");
	return 0;
}
int on_val(kv_parser* parser, const char* at, size_t len) {
	printf("%.*s", len, at);
	return 0;
}
int on_val_end(kv_parser* parser) {
	printf("\n");
	return 0;
}

int main(int argc, char* argv[]) {
	char* data1 = "reject_id=222&accept_id=1&type=xy";

	char* data2_0 = "reject_id = ";
	char* data2_1 = "222 & acc";
	char* data2_2 = "ept_id =1&&";
	char* data2_3 = "type= x";
	char* data2_4 = "y";

	char* data3 = "reject_id=[222]&[accept_id]=1&[type]=[xy]";

	kv_parser_settings settings;
	memset(&settings, 0, sizeof(kv_parser_settings));
	settings.s1 = '=';
	settings.s2 = '&';
	settings.w1 = '[';
	settings.w2 = ']';
	settings.on_key = on_key;
	settings.on_key_end = on_key_end;
	settings.on_val = on_val;
	settings.on_val_end = on_val_end;

	kv_parser parser1;
	kv_parser_init(&parser1, &settings);
	kv_parser_execute(&parser1, &settings, data1, strlen(data1));
	printf("\n\n");
	kv_parser parser2;
	kv_parser_init(&parser2, &settings);
	kv_parser_execute(&parser2, &settings, data2_0, strlen(data2_0));
	kv_parser_execute(&parser2, &settings, data2_1, strlen(data2_1));
	kv_parser_execute(&parser2, &settings, data2_2, strlen(data2_2));
	kv_parser_execute(&parser2, &settings, data2_3, strlen(data2_3));
	kv_parser_execute(&parser2, &settings, data2_4, strlen(data2_4));
	printf("\n\n");
	kv_parser parser3;
	kv_parser_init(&parser3, &settings);
	kv_parser_execute(&parser3, &settings, data3, strlen(data3));
	printf("\n\n");
	return 0;
}
