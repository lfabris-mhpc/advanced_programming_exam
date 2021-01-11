#include <iostream>
#include <iomanip>
#include <sstream>

#include <map>
#include <string>
#include <array>

#include <chrono>
#include <random>
#include <thread>

#include <numeric>
#include <algorithm>

#include "bst.hpp"

template<typename A, typename B>
void profile_insertions(A& container, B& keygen, std::size_t size, bool print=true) {
	std::size_t comps = 0, worst_comps = 0, best_comps = container.size();
	auto start = std::chrono::high_resolution_clock::now();
	std::size_t i = 0, target = container.size() + size;
	while (container.size() != target) {
		auto comp = container.key_comp();
		container[keygen()] = i;
		auto comp2 = container.key_comp();
		++i;

		auto c = comp2.comparisons - comp.comparisons;
		comps += c;
		worst_comps = std::max(worst_comps, c);
		best_comps = std::min(best_comps, c);
	}
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

	if (print) {
    	std::cout << i << " random insertions: " << elapsed.count() << " final size: " << container.size()
			<< " comparisons total: " << comps << " best: " << best_comps << " worst: " << worst_comps << std::endl;
	}
}

template<typename A, typename B>
void profile_find(const A& container, B& keygen, std::size_t searches, bool print=true) {
	std::size_t hits = 0, comps = 0, worst_comps = 0, best_comps = container.size();
	double worst_case = 0;
	auto start = std::chrono::high_resolution_clock::now();
	for (std::size_t i = 0; i < searches; ++i) {
		auto k = keygen();

		auto comp = container.key_comp();

		auto single_start = std::chrono::high_resolution_clock::now();
		auto iter = container.find(k);
		auto single_end = std::chrono::high_resolution_clock::now();

		std::chrono::duration<double> single_elapsed = single_end - single_start;
		auto comp2 = container.key_comp();

		worst_case = std::max(worst_case, single_elapsed.count());

		auto c = comp2.comparisons - comp.comparisons;
		comps += c;
		worst_comps = std::max(worst_comps, c);
		best_comps = std::min(best_comps, c);

		hits += iter != container.end();
	}

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

	if (print) {
    	std::cout << searches << " random searches: " << elapsed.count() << " worst case: " << worst_case
			<< " comparisons total: " << comps << " best: " << best_comps << " worst: " << worst_comps
			<< " (hits " << hits << ")" << std::endl;
	}
}

template<typename T, typename ActualComparator=std::less<T>>
struct counting_comparator {
	mutable std::size_t comparisons = 0;
	ActualComparator comparator;

	counting_comparator(): comparisons{}, comparator{} {
	}

	bool operator()(const T& lhs, const T& rhs) const noexcept {
		++comparisons;
		return comparator(lhs, rhs);
	}
};

int main(int argc, char** argv) {
	std::size_t seed_insert = 123543;
	std::size_t seed_search = 874563;

	std::size_t size = 1000000;
	std::size_t searches = 1000000;

	std::string container_type{"bst"};
	int param = 0;
	if (argc > ++param) {
		container_type = argv[param];

		if (container_type != "stdmap" && container_type != "bst" && container_type != "bst_unbalanced") {
			std::cerr << "first parameter must be either be stdmap or bst" << std::endl;
			exit(EXIT_FAILURE);
		}
	} else {
		std::cerr << "performs random (from a uniform distribution) insertions and lookups in the given container type, monitoring time spent and comparisons performed" << std::endl;
		std::cerr << "usage: " << argv[0] << " stdmap|bst|bst_unbalanced"
			<< " (#random_insertions default: " << size
			<< ") (#searches default: " << searches
			<< ") (seed_insert default: " << seed_insert
			<< ") (seed_search default: " << seed_search
			<< ")" << std::endl;
		exit(EXIT_FAILURE);
	}
	if (argc > ++param) {
		std::size_t pos;
		size = std::stoull(argv[param], &pos);

		if (!pos) {
			std::cerr << "second parameter must be a positive integer for the number of random elements" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (argc > ++param) {
		std::size_t pos;
		searches = std::stoull(argv[param], &pos);

		if (!pos) {
			std::cerr << "third parameter must be a positive integer for the number of random searches performed in each round" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (argc > ++param) {
		std::size_t pos;
		seed_insert = std::stoull(argv[param], &pos);

		if (!pos) {
			std::cerr << "fourth parameter must be a positive integer for the seed used in insertions" << std::endl;
			exit(EXIT_FAILURE);
		}
	}
	if (argc > ++param) {
		std::size_t pos;
		seed_search = std::stoull(argv[param], &pos);

		if (!pos) {
			std::cerr << "fifth parameter must be a positive integer for the seed used in searches" << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	#ifndef KEY_SIZE
	#define KEY_SIZE 1
	#endif
	using K = std::array<std::size_t, KEY_SIZE>;
	std::mt19937 engine{};
	std::uniform_int_distribution<std::size_t> dist{};
	auto keygen = [&dist, &engine]() -> auto {
		K k{0};
		k[std::tuple_size<K>::value - 1] = dist(engine);
		return k;
	};

	std::map<K, std::size_t, counting_comparator<K>> stdmap{};
	bst<K, std::size_t, counting_comparator<K>> tree_unbalanced{};
	bool do_balanced = container_type != "bst_unbalanced";

	std::cout << "test " << container_type << " with keys size_t[" << std::tuple_size<K>::value << "] (uniform distribution)" << std::endl;
	std::cout << "insert seed " << seed_insert << " search seed " << seed_search << std::endl;
	std::cout << "nodes will use " << (double) (sizeof(node<std::pair<const K, std::size_t>>) * size) / (1000 * 1000 * 1000) << " GB" << std::endl;
	if (container_type == "bst") {
		std::cout << "balancing will use " << (double) (sizeof(std::pair<const K, std::size_t>) * size) / (1000 * 1000 * 1000) << " GB" << std::endl;
	}

	std::cout << std::endl;
	std::cout << "size: " << size << std::endl;

	engine.seed(seed_insert);
	if (container_type == "stdmap") {
		std::cout << "stdmap ";
		profile_insertions(stdmap, keygen, size);
	} else {
		std::cout << "bst_unbalanced ";
		profile_insertions(tree_unbalanced, keygen, size);

		std::cout << "bst_unbalanced depth " << tree_unbalanced.depth() << std::endl;
	}

	engine.seed(seed_search);
	if (container_type == "stdmap") {

		std::cout << "stdmap ";
		profile_find(stdmap, keygen, searches);
	} else {
		engine.seed(seed_search);

		std::cout << "bst_unbalanced ";
		profile_find(tree_unbalanced, keygen, searches);

		if (do_balanced) {
			auto tree{tree_unbalanced};
			std::cout << "balancing tree" << std::endl;
			auto start = std::chrono::high_resolution_clock::now();
			tree.balance();
			auto end = std::chrono::high_resolution_clock::now();
			std::chrono::duration<double> elapsed = end - start;
			std::cout << "bst_balanced depth " << tree.depth() << " took " << elapsed.count() << std::endl;

			engine.seed(seed_search);

			std::cout << "bst_balanced ";
			profile_find(tree, keygen, searches);
		}
	}
}
