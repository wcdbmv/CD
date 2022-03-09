#pragma once

#include <string_view>


class CharReader {
protected:
	// Sets new string
	void load_(std::string_view string);

	// Returns first character
	char peek_() const;

	// Deletes first character
	void eat_();

	// Deletes first character with check
	// Throws std::invalid_argument if first character and c are different
	void eat_(char c);

	// Deletes and returns first character
	char next_();

	// Returns true if there are no characters left
	bool empty_();

private:
	std::string_view remain_;
};
