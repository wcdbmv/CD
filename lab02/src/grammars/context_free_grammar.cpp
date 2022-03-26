#include <grammars/context_free_grammar.hpp>

#include <stdexcept>

#include <graph/graph.hpp>


ContextFreeGrammar::ContextFreeGrammar(
	Alphabet non_terminal_symbols,
	Alphabet terminal_symbols,
	ProductionRules production_rules,
	Symbol start_symbol
)
	: Grammar{
		std::move(non_terminal_symbols),
		std::move(terminal_symbols),
		std::move(production_rules),
		std::move(start_symbol),
	}
{
	checkContextFree_();
}

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

void ContextFreeGrammar::greibachNormalForm(std::optional<String> mb_order) {
	if (mb_order && mb_order->size() != non_terminal_symbols_.size()) {
		throw std::invalid_argument("[ContextFreeGrammar::greibachNormalForm] mb_order must be full");
	}

	if (non_terminal_symbols_.size() <= 1) {
		return;
	}

	auto order = mb_order ? std::move(*mb_order) : calcBestLinearOrder_();

	for (auto i = order.size() - 2; i != 0; --i) {
		const auto& Ai = order[i];
		for (auto j = i + 1; j < order.size(); ++j) {
			const auto Aj = order[j];

			auto P = production_rules_[{Ai}];

			for (auto&& Aj_alpha : production_rules_[{Ai}]) {
				if (Aj_alpha.empty() || Aj_alpha.front() != Aj) {
					continue;
				}

				P.erase(Aj_alpha);

				auto alpha = String(Aj_alpha.begin() + 1, Aj_alpha.end());

				for (auto&& beta : production_rules_[{Aj}]) {
					auto beta_alpha = beta;
					beta_alpha.insert(beta_alpha.end(), alpha.begin(), alpha.end());
					P.insert(std::move(beta_alpha));
				}
			}

			production_rules_[{Ai}] = std::move(P);
		}
	}

	auto old_non_terminal_symbols = non_terminal_symbols_;
	for (auto&& A : old_non_terminal_symbols) {
		auto P = production_rules_[{A}];
		for (auto&& aXX : production_rules_[{A}]) {
			if (aXX.size() <= 2) {
				continue;
			}
			bool is_continue = true;
			for (auto&& Xj : aXX) {
				if (terminal_symbols_.contains(Xj)) {
					is_continue = false;
					break;
				}
			}
			if (is_continue) {
				continue;
			}

			P.erase(aXX);

			auto aX1X1 = aXX;
			for (auto Xj = aX1X1.begin() + 1; Xj != aX1X1.end(); ++Xj) {
				if (!terminal_symbols_.contains(*Xj)) {
					continue;
				}

				auto Xj1 = *Xj + '\'';
				non_terminal_symbols_.insert(Xj1);
				production_rules_[{Xj1}].insert({*Xj});
				*Xj = Xj1;
			}

			P.insert(aX1X1);
		}
		production_rules_[{A}] = std::move(P);
	}
}
