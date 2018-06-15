#ifndef KV_PARSER_H
#define KV_PARSER_H

#include <utility>
#include <functional>
#include <cctype>

class keyval_parser {
public:
	typedef std::string key_type;
	typedef std::string val_type;
	typedef std::pair<key_type, val_type> value_type;
	

	keyval_parser(char kb, char ka, char op, char vb, char va, char sp, std::function<void (value_type)> cb)
	: kb_(kb), ka_(ka), op_(op), vb_(vb), va_(va), sp_(sp)
	, stat_(STATUS_WHITESPACE_BEFORE_KEY)
	, cb_(cb) {

	}
	template <class Container>
	keyval_parser(char kb, char ka, char op, char vb, char va, char sp, Container* ctr)
	: kb_(kb), ka_(ka), op_(op), vb_(vb), va_(va), sp_(sp)
	, stat_(STATUS_WHITESPACE_BEFORE_KEY)
	, cb_(std::bind(&keyval_parser::on_emit<Container>, std::placeholders::_1, ctr)) {

	}
	void reset() {
		pair_.first.clear();
		pair_.second.clear();
		stat_ = STATUS_WHITESPACE_BEFORE_KEY;
	}
	std::size_t parse(const char* data, std::size_t size) {
		char *p = const_cast<char*>(data), c;
		bool  d = false;
		while(p < data + size) {
			c = *p;
			switch(stat_) {
			case STATUS_WHITESPACE_BEFORE_KEY:
				if(kb_ == c || !std::isspace(c)) {
					++stat_;
					continue;
				}
				break;
			case STATUS_WRAPPER_BEFORE_KEY:
				if(kb_ != c) {
					++stat_;
					continue;
				}
				break;
			case STATUS_KEY:
				if(ka_ == '\0') {
					if(op_ == c || std::isspace(c)) {
						++stat_;
						continue;
					}
				}else{
					if(ka_ == c || op_ == c) {
						++stat_;
						continue;
					}
				}
				pair_.first.push_back(c);
				break;
			case STATUS_WRAPPER_AFTER_KEY:
				if(ka_ != c) {
					++stat_;
					continue;	
				}
				break;
			case STATUS_WHITESPACE_AFTER_KEY:
				if(op_ == c || !std::isspace(c)) {
					++stat_;
					continue;
				}
				break;
			case STATUS_OPERATOR:
				if(op_ != c) {
					++stat_;
					continue;
				}
				break;
			case STATUS_WHITESPACE_BEFORE_VAL:
				if(vb_ == c || !std::isspace(c)) {
					++stat_;
					continue;
				}
				break;
			case STATUS_WRAPPER_BEFORE_VAL:
				if(vb_ != c) {
					++stat_;
					continue;
				}
				break;
			case STATUS_VAL:
				if(va_ == '\0') {
					if(sp_ == c || std::isspace(c)) {
						cb_(std::move(pair_));
						++stat_;
						continue;
					}
				}else{
					if(va_ == c || sp_ == c) {
						cb_(std::move(pair_));
						++stat_;
						continue;
					}
				}
				pair_.second.push_back(c);
				break;
			case STATUS_WRAPPER_AFTER_VAL:
				if(va_ != c) {
					++stat_;
					continue;
				}
				break;
			case STATUS_WHITESPACE_AFTER_VAL:
				if(sp_ == c || !std::isspace(c)) {
					++stat_;
					continue;
				}
				break;
			case STATUS_SEPERATOR:
				if(sp_ != c) {
					stat_ = STATUS_WHITESPACE_BEFORE_KEY;
					continue;
				}
				break;
			};
			++p;
		}
		return p - data;
	}
	template <class Container>
	static auto on_emit(value_type data, Container* ctr) -> decltype(ctr->emplace(std::move(data)), void()) {
		ctr->emplace(std::move(data));
	}
	template <class Container>
	static auto on_emit(value_type data, Container* ctr) -> decltype(ctr->emplace_back(std::move(data)), void()) {
		ctr->emplace_back(std::move(data));
	}
private:
	char kb_, ka_, op_, vb_, va_, sp_, stat_;
	std::function<void (value_type)> cb_;
	enum {
		STATUS_WHITESPACE_BEFORE_KEY,
		STATUS_WRAPPER_BEFORE_KEY,
		STATUS_KEY,
		STATUS_WRAPPER_AFTER_KEY,
		STATUS_WHITESPACE_AFTER_KEY,
		STATUS_OPERATOR,
		STATUS_WHITESPACE_BEFORE_VAL,
		STATUS_WRAPPER_BEFORE_VAL,
		STATUS_VAL,
		STATUS_WRAPPER_AFTER_VAL,
		STATUS_WHITESPACE_AFTER_VAL,
		STATUS_SEPERATOR,
	};
	value_type pair_;
};


#endif // KV_PARSER_H