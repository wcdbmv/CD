#include <char_reader.hpp>

#include <stdexcept>
#include <string>


void CharReader::load_(std::string_view string) {
	remain_ = string;
}

char CharReader::peek_() const {
	return remain_[0];
}

void CharReader::eat_() {
	remain_ = std::string_view{std::next(remain_.begin()), remain_.end()};
}

void CharReader::eat_(char c) {
	if (peek_() != c) {
		// const auto message = std::format("[CharReader::eat_] Expected '{}', got '{}'", c, peek_());

		using namespace std::string_literals;
		const auto message = "[CharReader::eat_] Expected '"s + c + "', got '" + peek_() + "'.";

		throw std::invalid_argument(message);
	}

	eat_();
}

char CharReader::next_() {
	auto c = peek_();
	eat_();
	return c;
}

bool CharReader::empty_() {
	return remain_.empty();
}
