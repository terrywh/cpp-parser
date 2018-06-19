#ifndef SEPARATOR_PARSER_H
#define SEPARATOR_PARSER_H

#include <functional> // for std::functional
#include <cctype>
#include "concepts.hpp"

namespace parser {
/**
 * 针对单个字符作为分割符，包裹符号的解析器；
 * @tparam VALUE 解析数据存储类型, 需要满足 value_type 以及 is_move_constructible 需求；
 */
template <class VALUE,
	class = typename std::enable_if<concepts::value_type<VALUE>::value, void>::type,
	class = typename std::enable_if<std::is_move_constructible<VALUE>::value, void>::type>
class separator_parser {
public:
	typedef VALUE value_type;
	typedef std::pair<value_type, value_type> entry_type;
	
	/**
	 * 构建一个能够解析如下形式文本的解析器；并在成功解析出一对 KEY/VAL 数据后调用回调；
	 * 	{WHITESPACE}kb{KEY}ka{WHITESPACE}op{WHITESPACE}vb{VAL}va{WHITESPACE}sp{WHITESPACE}
	 * 若不存在对应位置符号，请见其指定为 '\0'；
	 * @param cb 回调函数，原型如下：
	 * 	void callback(entry_type e);
	 */
	separator_parser(char kb, char ka, char op, char vb, char va, char sp, std::function<void (entry_type)> cb)
	: kb_(kb), ka_(ka), op_(op), vb_(vb), va_(va), sp_(sp)
	, stat_(STATUS_WHITESPACE_BEFORE_KEY)
	, cb_(cb) {

	}
	/**
	 * 构建一个能够解析如下形式文本的解析器；并在成功解析出一对 KEY/VAL 数据后将其写入对应容器中；
	 * 	{WHITESPACE}kb{KEY}ka{WHITESPACE}op{WHITESPACE}vb{VAL}va{WHITESPACE}sp{WHITESPACE}
	 * 若不存在对应位置符号，请见其指定为 '\0'；
	 * @param ctr 容器，需要满足 container_type_1 或 container_type_2 需求；
	 */
	template <class CONTAINER,
		class = typename std::enable_if<(concepts::container_type_1<CONTAINER, entry_type>::value || concepts::container_type_2<CONTAINER, entry_type>::value), void>::type>
	separator_parser(char kb, char ka, char op, char vb, char va, char sp, CONTAINER* ctr)
	: kb_(kb), ka_(ka), op_(op), vb_(vb), va_(va), sp_(sp)
	, stat_(STATUS_WHITESPACE_BEFORE_KEY)
	, cb_(std::bind(&separator_parser::on_emit<CONTAINER>, std::placeholders::_1, ctr)) {

	}
	/// 重置复用解析器状态，以重新开始解析过程
	void reset() {
		pair_.first.clear();
		pair_.second.clear();
		stat_ = STATUS_WHITESPACE_BEFORE_KEY;
	}
	/// 解析指定文本内容（在成功解析出一对 KEY/VAL 后，调用回调或写入容器）
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
private:
	// 容器包裹回调 ( container_type_1 )
	template <class CONTAINER>
	static auto on_emit(entry_type data, CONTAINER* ctr) -> decltype(ctr->insert(std::move(data)), void()) {
		ctr->insert(std::move(data));
	}
	// 容器包裹回调 ( container_type_2 )
	template <class CONTAINER>
	static auto on_emit(entry_type data, CONTAINER* ctr) -> decltype(ctr->push_back(std::move(data)), void()) {
		ctr->push_back(std::move(data));
	}

	char kb_, ka_, op_, vb_, va_, sp_, stat_;
	// 回调函数
	std::function<void (entry_type)> cb_;
	// 解析状态
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
	// 当前正在操作整理的数据容器
	entry_type pair_;
};

}

#endif // SEPARATOR_PARSER_H