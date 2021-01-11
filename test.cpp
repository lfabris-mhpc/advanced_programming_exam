#include <iostream>
#include <map>
#include <string>

#include "bst.hpp"

template<typename K, typename V>
std::ostream& operator<<(std::ostream& os, const std::map<K, V>& m) {
	os << "map(" << m.size() << ") {";
	for (auto p : m) {
		os << "(" << p.first << ": " << p.second << "), ";
	}
	return os << "}";
}

template<typename A, typename B, typename C>
void print(const A& stdmap, const B& tree, const C& rtree) {
	std::cout << "stdmap: " << stdmap << std::endl;
	std::cout << "tree: " << tree << std::endl;
	std::cout << "rtree: " << rtree << std::endl;
}

template<typename A, typename B, typename C>
void clear(A& stdmap, B& tree, C& rtree) {
	stdmap.clear();
	tree.clear();
	rtree.clear();
}

template<typename B, typename C>
void balance(B& tree, C& rtree) {
	tree.clear();
	rtree.clear();
}

template<typename A, typename B, typename C>
void insertBrackets(A& stdmap, B& tree, C& rtree, int from, int to) {
	for (auto i = from; i <= to; ++i) {
		stdmap[std::to_string(i)] = i;
		tree[std::to_string(i)] = i;
		rtree[std::to_string(i)] = i;
	}
}

template<typename A, typename B, typename C>
void insert(A& stdmap, B& tree, C& rtree, int from, int to) {
	for (auto i = from; i <= to; ++i) {
		stdmap.insert(std::make_pair(std::to_string(i), i));
		tree.insert(std::make_pair(std::to_string(i), i));
		rtree.insert(std::make_pair(std::to_string(i), i));
	}
}

template<typename A, typename B, typename C>
void insertInverted(A& stdmap, B& tree, C& rtree, int from, int to) {
	for (auto i = from; i <= to; ++i) {
		stdmap.insert(std::make_pair(std::to_string(i), -i));
		tree.insert(std::make_pair(std::to_string(i), -i));
		rtree.insert(std::make_pair(std::to_string(i), -i));
	}
}

template<typename A, typename B, typename C>
void emplace(A& stdmap, B& tree, C& rtree, int from, int to) {
	for (auto i = from; i <= to; ++i) {
		stdmap.emplace(std::to_string(i), i);
		tree.emplace(std::to_string(i), i);
		rtree.emplace(std::to_string(i), i);
	}
}

template<typename A, typename B, typename C>
void invertValues(A& stdmap, B& tree, C& rtree) {
	for (auto& p : stdmap) {
		p.second = -p.second;
	}
	for (auto& p : tree) {
		p.second = -p.second;
	}
	for (auto& p : rtree) {
		p.second = -p.second;
	}
}

int main() {
	std::map<std::string, int> stdmap{};
	bst<std::string, int> tree{};
	bst<std::string, int, std::greater<std::string>> rtree{};
	
	std::cout << "inserting from 0 to 10 into originals" << std::endl;
	insert(stdmap, tree, rtree, 0, 10);
	
	std::cout << std::endl;
	std::cout << "originals:" << std::endl;
	print(stdmap, tree, rtree);

	std::cout << "copy constructors from originals" << std::endl;
	auto stdmap2{stdmap};
	auto tree2{tree};
	auto rtree2{rtree};
	
	std::cout << "insertingBrackets from 20 to 30 into originals" << std::endl;
	insertBrackets(stdmap, tree, rtree, 20, 30);
	
	std::cout << std::endl;
	std::cout << "originals:" << std::endl;
	print(stdmap, tree, rtree);

	std::cout << std::endl;
	std::cout << "copy-constructed:" << std::endl;
	print(stdmap2, tree2, rtree2);
	
	std::cout << "copy assignments from copy constructed" << std::endl;
	auto stdmap3 = std::map<std::string, int>{stdmap2};
	auto tree3 = bst<std::string, int>{tree2};
	auto rtree3 = bst<std::string, int, std::greater<std::string>>{rtree2};
	
	std::cout << "emplacing from 40 to 50 into copy-constructed" << std::endl;
	emplace(stdmap2, tree2, rtree2, 40, 50);
	
	std::cout << std::endl;
	std::cout << "copy-constructed:" << std::endl;
	print(stdmap2, tree2, rtree2);

	std::cout << std::endl;
	std::cout << "copy-assigned:" << std::endl;
	print(stdmap3, tree3, rtree3);
	
	std::cout << "move constructors from copy-assigned" << std::endl;
	std::map<std::string, int> stdmap4{std::move(stdmap3)};
	bst<std::string, int> tree4{std::move(tree3)};
	bst<std::string, int, std::greater<std::string>> rtree4{std::move(rtree3)};

	std::cout << std::endl;
	std::cout << "move-constructed from copy-assigned:" << std::endl;
	print(stdmap4, tree4, rtree4);

	std::cout << "move assignments to copy-constructed from move-constructed" << std::endl;
	stdmap2 = std::move(stdmap4);
	tree2 = std::move(tree4);
	rtree2 = std::move(rtree4);

	std::cout << std::endl;
	std::cout << "move-assigned" << std::endl;
	print(stdmap2, tree2, rtree2);
	
	std::cout << "insertingInverted from 40 to 50 into move-assigned" << std::endl;
	insertInverted(stdmap2, tree2, rtree2, 40, 50);

	std::cout << std::endl;
	std::cout << "move-assigned" << std::endl;
	print(stdmap2, tree2, rtree2);
	
	std::cout << "inserting from 40 to 50 into move-assigned" << std::endl;
	insert(stdmap2, tree2, rtree2, 40, 50);

	std::cout << std::endl;
	std::cout << "move-assigned" << std::endl;
	print(stdmap2, tree2, rtree2);
	
	int search = 10;
	std::cout << std::endl;
	std::cout << "stdmap.find(" << search << ") != stdmap.end(): " << (stdmap2.find(std::to_string(search)) != stdmap2.end()) << std::endl;
	std::cout << "tree.find(" << search << ") != tree.end(): " << (tree2.find(std::to_string(search)) != tree2.end()) << std::endl;
	std::cout << "rtree.find(" << search << ") != rtree.end(): " << (rtree2.find(std::to_string(search)) != rtree2.end()) << std::endl;
	
	search = -123;
	std::cout << std::endl;
	std::cout << "stdmap.find(" << search << ") != stdmap.end(): " << (stdmap2.find(std::to_string(search)) != stdmap2.end()) << std::endl;
	std::cout << "tree.find(" << search << ") != tree.end(): " << (tree2.find(std::to_string(search)) != tree2.end()) << std::endl;
	std::cout << "rtree.find(" << search << ") != rtree.end(): " << (rtree2.find(std::to_string(search)) != rtree2.end()) << std::endl;
	
	std::cout << std::endl;
	std::cout << "tree depth: " << tree2.depth() << " rtree depth: " << rtree2.depth() << std::endl;
	
	std::cout << "balancing" << std::endl;
	tree2.balance();
	rtree2.balance();
	
	std::cout << "tree depth: " << tree2.depth() << " rtree depth: " << rtree2.depth() << std::endl;
		
	std::cout << std::endl;
	std::cout << "balanced" << std::endl;
	print(stdmap2, tree2, rtree2);
	
	std::cout << std::endl;
	std::cout << "invert the sign of the values" << std::endl;
	invertValues(stdmap2, tree2, rtree2);
	print(stdmap2, tree2, rtree2);
	
	stdmap.clear();
	tree.clear();
	rtree.clear();
	
	stdmap2.clear();
	tree2.clear();
	rtree2.clear();
	
	stdmap3.clear();
	tree3.clear();
	rtree3.clear();
	
	stdmap4.clear();
	tree4.clear();
	rtree4.clear();
}
