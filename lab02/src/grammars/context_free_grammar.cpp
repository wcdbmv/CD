#include <grammars/context_free_grammar.hpp>

#include <stdexcept>

#include <graph/graph.hpp>


ContextFreeGrammar::ContextFreeGrammar(const Grammar& other)
	: Grammar{other}
{
	checkContextFree_();
}

ContextFreeGrammar::ContextFreeGrammar(Grammar&& other)
	: Grammar{other}
{
	checkContextFree_();
}

void ContextFreeGrammar::checkContextFree_() {
	for (auto&& [from, to] : production_rules_) {
		if (from.size() != 1 || !non_terminal_symbols_.contains(from.front())) {
			throw std::invalid_argument("[ContextFreeGrammar::ContextFreeGrammar] Left part of the production rule can have only one non terminal");
		}
	}
}

void ContextFreeGrammar::eliminateLeftRecursion() {
	auto new_non_terminal_symbols = non_terminal_symbols_;
	auto new_production_rules = production_rules_;

	// TODO :
	// Если ε присутствовал в языке исходной грамматики,
	// добавим новый начальный символ S′ и правила S′ → S ∣ ε

	for (auto i = non_terminal_symbols_.begin(); i != non_terminal_symbols_.end(); ++i) {
		const auto& Ai = *i;
		for (auto j = non_terminal_symbols_.begin(); j != i; ++j) {
			const auto& Aj = *j;

			auto Ai_rule = production_rules_[{Ai}];

			for (auto&& Aj_gamma : production_rules_[{Ai}]) {
				if (!(Aj_gamma.size() >= 2 && Aj_gamma.front() == Aj)) {
					continue;
				}

				Ai_rule.erase(Aj_gamma);

				for (auto&& xi : new_production_rules[{Aj}]) {
					if (xi.empty()) {
						continue;
					}

					auto xi_gamma = xi;
					xi_gamma.insert(xi_gamma.end(), Aj_gamma.begin() + 1, Aj_gamma.end());
					Ai_rule.insert(xi_gamma);
				}
			}
			production_rules_[{Ai}] = std::move(Ai_rule);
		}
		eliminateImmediateLeftRecursion_(Ai, new_non_terminal_symbols, new_production_rules);
	}
	non_terminal_symbols_ = std::move(new_non_terminal_symbols);
	production_rules_ = std::move(new_production_rules);
}

bool ContextFreeGrammar::hasImmediateLeftRecursionProductionRules_(const Symbol& A) {
	for (auto&& p : production_rules_[{A}]) {
		if (p.size() >= 2 && p.front() == A) {
			return true;
		}
	}
	return false;
}

void ContextFreeGrammar::eliminateImmediateLeftRecursion_(
	const Symbol& A,
	Alphabet& new_non_terminal_symbols,
	ProductionRules& new_production_rules
) {
	if (!hasImmediateLeftRecursionProductionRules_(A)) {
		return;
	}

	auto A1 = A + '\'';
	while (new_non_terminal_symbols.contains(A1)) {
		A1 += '\'';
	}
	new_non_terminal_symbols.insert(A1);

	new_production_rules[{A}].clear();
	for (auto&& p : production_rules_[{A}]) {
		if (p.size() >= 2 && p.front() == A) {
			auto alpha = String(p.begin() + 1, p.end());
			auto alpha_A1 = alpha;
			alpha_A1.push_back(A1);

			new_production_rules[{A1}].insert(std::move(alpha_A1));
			new_production_rules[{A1}].insert(kEpsilon);
		} else if (!p.empty() && p.front() != A || p.empty()) {
			const auto& beta = p;
			auto beta_A1 = beta;
			beta_A1.push_back(A1);

			new_production_rules[{A}].insert(std::move(beta_A1));
		}
	}
}

String ContextFreeGrammar::calcBestLinearOrder_() const {
	String order;

	if (non_terminal_symbols_.empty()) {
		return order;
	}

	Vertices vertices = non_terminal_symbols_;
	Edges edges;

	UMap<Vertex, bool> used;

	std::function<void(const Symbol& A, const Symbol& A1)> dfs;
	dfs = [&](const Symbol& A, const Symbol& A1) {
		used[A1] = true;
		if (auto adj = production_rules_.find({A1}); adj != production_rules_.end()) {
			for (auto&& to : adj->second) {
				if (to.empty() || !non_terminal_symbols_.contains(to.front())) {
					continue;
				}

				const auto& B = to.front();
				edges[A].insert(B);

				if (!used[B]) {
					dfs(A, B);
				}
			}
		}
	};

	for (auto&& A : non_terminal_symbols_) {
		used.clear();
		dfs(A, A);
	}

	Graph graph{std::move(vertices), std::move(edges)};

	return graph.topologicalSort();
}
