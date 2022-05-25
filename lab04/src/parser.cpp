#include <parser.hpp>

#include <map>
#include <set>


namespace {

using namespace std::string_literals;

template <typename T>
using Set = std::set<T>;

template <typename T>
using Vector = std::vector<T>;

template <typename Key, typename T>
using Map = std::map<Key, T>;

using Token = std::string;
using Tokens = Set<Token>;

using Precedence = Map<Token, int>;

using Relations = Map<Token, Map<Token, Token>>;


const auto kMarker = "$"s;

const Vector<Tokens> kOperators = {
	{"**", "abs", "not"},
	{"*", "/", "mod", "rem"},
	{"+'", "-'"},
	{"+\"", "-\"", "&"},
	{"<", "<=", "=", "/>", ">", ">="},
	{"and", "or" , "xor"},
};

const Tokens kPrefixes = {"abs", "not", "+'", "-'"};
const Tokens kIdentifiers = {"a", "b", "c", "d"};
const Tokens kConstants = {"0", "1", "2"};

const Tokens kRightAssociative = {"**"};


Precedence sFillPrecedence() {
	Precedence precedence;
	for (auto i = static_cast<int>(kOperators.size()) - 1; i >= 0; --i) {
		for (auto&& op : kOperators[i]) {
			precedence[op] = i;
		}
	}
	return precedence;
}

void sInsert(Tokens& set, const Tokens& values) {
	std::copy(values.begin(), values.end(), std::inserter(set, set.end()));
}

void sAppend(Vector<Token>& vector, const Tokens& values) {
	std::copy(values.begin(), values.end(), std::back_inserter(vector));
}

Tokens sGetAllTokens() {
	Tokens all_tokens;
	for (auto&& ops : kOperators) {
		sInsert(all_tokens, ops);
	}
	sInsert(all_tokens, kPrefixes);
	sInsert(all_tokens, kIdentifiers);
	sInsert(all_tokens, kConstants);
	all_tokens.insert({"(", ")", kMarker});
	return all_tokens;
}

auto sMakeRelations() {
	Relations relations = {
		{kMarker, {{"(", "<"}}},
		{"(",     {{")", "="}, {"(", "<"}}},
		{")",     {{"$", ">"}, {")", ">"}}},
	};

	Vector<Token> variables;
	variables.reserve(kIdentifiers.size() + kConstants.size());
	sAppend(variables, kIdentifiers);
	sAppend(variables, kConstants);

	for (const auto& var : variables) {
		relations[kMarker][var] = relations["("][var] = "<";
		relations[var][kMarker] = relations[var][")"] = ">";
	}

	// Here 'relations[op]' can be stored to speedup
	auto precedence = sFillPrecedence();
	for (const auto& [op, _] : precedence) {
		relations[op][kMarker] = ">";
		relations[kMarker][op] = "<";

		relations[op]["("] = relations["("][op] = "<";
		relations[op][")"] = relations[")"][op] = ">";

		for (const auto& var : variables) {
			relations[op][var] = "<";
			relations[var][op] = ">";
		}

		if (kPrefixes.contains(op)) {
			for (const auto& [op_, pr_] : precedence) {
				relations[op_][op] = "<";
				relations[op][op_] = precedence[op] > precedence[op_] ? ">" : "<";
			}
		} else {
			for (const auto& [op_, _] : precedence) {
				const auto prOp = precedence[op];
				const auto prOp_ = precedence[op_];

				const auto isRightAssociativeOp = kRightAssociative.contains(op);
				const auto isRightAssociativeOp_ = kRightAssociative.contains(op_);

				if (prOp < prOp_ || (prOp == prOp_ && isRightAssociativeOp && isRightAssociativeOp_)) {
					relations[op][op_] = "<";
				} else if (prOp > prOp_ || (prOp == prOp_ && !isRightAssociativeOp && !isRightAssociativeOp_)) {
					relations[op][op_] = ">";
				}
			}
		}
	}

	return relations;
}

}  // namespace


std::pair<bool, std::string> parse(Vector<Token> tokens) {
	auto all_tokens = sGetAllTokens();
	auto relations = sMakeRelations();
	tokens.push_back(kMarker);

	std::string result;
	auto next_token = tokens.begin();
	Vector<Token> stack_tail;
	auto stack_head = kMarker;

	do {
		if (all_tokens.contains(*next_token)) {
			if (const auto& relation = relations[stack_head][*next_token]; relation == "<" || relation == "=") {
				stack_tail.push_back(stack_head);
				stack_head = *next_token;
				++next_token;
			} else if (relation == ">") {
				Token prev_stack_head;
				do {
					if (stack_head != "(" && stack_head != ")") {
						result.append(stack_head);
						result.push_back(' ');
					}

					prev_stack_head = stack_head;
					stack_head = stack_tail.back();
					stack_tail.pop_back();
				} while (relations[stack_head][prev_stack_head] != "<");
			} else {
				return {false, {}};
			}
		} else {
			return {false, {}};
		}
	} while (stack_head != kMarker || *next_token != kMarker);

	result.pop_back();

	return {true, result};
}
