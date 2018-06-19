#ifndef MULTIPART_PARSER_H
#define MULTIPART_PARSER_H

#include <functional> // for std::functional
#include <cctype>
#include "concepts.hpp"

namespace parser {
/**
 * 针对 multipart/form-data 等 multipart 形式的解析器
 * @tparam VALUE 存储解析数据，满足 value_type 要求
 */
template <class VALUE,
	class = typename std::enable_if<concepts::value_type<VALUE>::value, void>::type,
	class = typename std::enable_if<std::is_move_constructible<VALUE>::value, void>::type>
class multipart_parser {
public:
	typedef VALUE value_type;
	typedef std::pair<value_type, value_type> entry_type;
	/**
	 * 创建一个解析器，并将成功解析出的 KEY/VAL 对回调指定函数；
	 * @param cb 回调函数，原型如下：
	 * 	void callback(entry_type entry);
	 */
	multipart_parser(const std::string& boundary, std::function<void (entry_type)> cb)
	: boundary_(boundary)
	, stat_(STATUS_BEFORE_BOUNDARY_1)
	, offset_(0) 
	, cb_(cb) {

	}
	/**
	 * 创建一个解析器，并将成功解析出的 KEY/VAL 对写入指定容器；
	 * @param ctr 容器，需要满足 container_type_1 或 container_type_2 需求；
	 */
	template <class CONTAINER,
		class = typename std::enable_if<(concepts::container_type_1<CONTAINER, entry_type>::value || concepts::container_type_2<CONTAINER, entry_type>::value), void>::type>
	multipart_parser(const std::string& boundary, CONTAINER* ctr)
	: boundary_(boundary)
	, stat_(STATUS_BEFORE_BOUNDARY_1)
	, offset_(0)
	, cb_(std::bind(&multipart_parser::on_emit<CONTAINER>, std::placeholders::_1, ctr)) {

	}
	/// 解析指定数据（允许二进制数据）并调用回调函数或写入容器
	std::size_t parse(const char* data, std::size_t size) {
		char* p = const_cast<char*>(data), c;
		while(p < data + size) {
			c = *p;
			switch(stat_) {
			case STATUS_BEFORE_BOUNDARY_1:
				if(c == '-') {
					++stat_;
				}else{
					entry_.second.push_back('\r');
					entry_.second.push_back('\n');
					stat_ = STATUS_DATA;
					continue;
				}
				break;
			case STATUS_BEFORE_BOUNDARY_2:
				if(c == '-') {
					++stat_;
				}else{
					entry_.second.push_back('\r');
					entry_.second.push_back('\n');
					entry_.second.push_back('-');
					stat_ = STATUS_DATA;
					continue;
				}
				break;
			case STATUS_BOUNDARY:
				if(offset_ >= boundary_.size()) {
					// 非首个 boundary 会含有数据需要抛出 (此时 KEY 应为空)
					if(entry_.second.size() > 0) { // DATA KEY/VAL
						cb_(std::move(entry_));
					}
					offset_ = 0;
					++stat_;
					continue;
				}else if(boundary_[offset_++] == c) {

				}else{
					entry_.second.push_back('\r');
					entry_.second.push_back('\n');
					entry_.second.push_back('-');
					entry_.second.push_back('-');
					for(int i=0; i<offset_; ++i) {
						entry_.second.push_back(boundary_[i]); // 前面经过的若干 boundary 符号属于数据区
					}
					offset_ = 0;
					stat_ = STATUS_DATA;
					continue;
				}
				break;
			case STATUS_AFTER_BOUNDARY_1:
				if(c == '\r') {
					++stat_;
				}else if(c == '-') {
					stat_ = STATUS_FINISH_1;
					continue;
				}else{
					goto PARSE_FAILED;
				}
				break;
			case STATUS_AFTER_BOUDDARY_2:
				if(c == '\n') {
					++stat_;
				}else{
					goto PARSE_FAILED;
				}
				break;
			case STATUS_HEADER_FIELD:
				if(c == '\r') {
					stat_ = STATUS_BEFORE_DATA_1;
					continue;
				}else if(c == ':') {
					++stat_;
					continue;
				}else{
					entry_.first.push_back(c);
				}
				break;
			case STATUS_HEADER_COLON:
				++stat_;
				break;
			case STATUS_HEADER_WHITESPACE_BEFORE_VALUE:
				if(!std::isspace(c)) {
					++stat_;
					continue;
				}
				break;
			case STATUS_HEADER_VALUE:
				if(c == '\r') {
					cb_(std::move(entry_)); // HEADER FIELD/VALUE
					++stat_;
					continue;
				}else{
					entry_.second.push_back(c);
				}
				break;
			case STATUS_AFTER_HEADER_1:
				++stat_; // \r
				break;
			case STATUS_AFTER_HEADER_2:
				if(c == '\n') {
					++stat_;
				}else{
					goto PARSE_FAILED;
				}
				break;
			case STATUS_BEFORE_DATA_1:
				if(c == '\r') {
					++stat_;
				}else{
					stat_ = STATUS_HEADER_FIELD;
					continue;
				}
				break;
			case STATUS_BEFORE_DATA_2:
				if(c == '\n') {
					++stat_;
				}else{
					goto PARSE_FAILED;
				}
				break;
			case STATUS_DATA:
				if(c == '\r') {
					++stat_;
					continue;
				}else{
					entry_.second.push_back(c);
				}
				break;
			case STATUS_AFTER_DATA_1:
				++stat_;
				break;
			case STATUS_AFTER_DATA_2:
				if(c == '\n') {
					stat_ = STATUS_BEFORE_BOUNDARY_1;
				}else{
					entry_.second.push_back('\r');
					stat_ = STATUS_DATA;
					continue;
				}
				break;
			case STATUS_FINISH_1:
				if(c == '-') {
					++stat_;
				}else{
					goto PARSE_FAILED;
				}
				break;
			case STATUS_FINISH_2:
				if(c == '-') {
					// 本次解析结束，重置后可复用
				}else{
					goto PARSE_FAILED;
				}
				break;
			}
			++p;
		}
PARSE_FAILED:
		return p - data;
	}
	/// 重置解析器状态 ( 重复使用 )
	void reset() {
		stat_   = STATUS_BEFORE_BOUNDARY_1;
		offset_ = 0;
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
	// 边界
	std::string boundary_;
	int stat_;
	// 解析在边界 boundary 中的偏移位置
	int offset_;
	// 回调
	std::function<void (entry_type)> cb_;
	enum {
		STATUS_BEFORE_BOUNDARY_1,
		STATUS_BEFORE_BOUNDARY_2,
		STATUS_BOUNDARY,
		STATUS_AFTER_BOUNDARY_1,
		STATUS_AFTER_BOUDDARY_2,
		STATUS_HEADER_FIELD,
		STATUS_HEADER_COLON,
		STATUS_HEADER_WHITESPACE_BEFORE_VALUE,
		STATUS_HEADER_VALUE,
		STATUS_AFTER_HEADER_1,
		STATUS_AFTER_HEADER_2,
		STATUS_BEFORE_DATA_1,
		STATUS_BEFORE_DATA_2,
		STATUS_DATA,
		STATUS_AFTER_DATA_1,
		STATUS_AFTER_DATA_2,
		STATUS_FINISH_1,
		STATUS_FINISH_2,
	};
	// 正在处理的数据缓存
	entry_type entry_;
};

}

#endif // MULTPART_PARSER_H