#ifndef __BST_HPP__
#define __BST_HPP__

#include <iostream>
#include <utility>
#include <memory>

#include <vector>
#include <algorithm>
#include <numeric>

#include <cassert>

template<typename pair_type>
struct node {
	node* parent;
	std::unique_ptr<node> left;
	std::unique_ptr<node> right;
	pair_type data;

	node() = default;

	explicit node(node* parent): parent{parent}, left{}, right{}, data{} {
	}

	node(node* parent, const pair_type& d): parent{parent}, left{}, right{}, data{d} {
	}

	node(node* parent, pair_type&& d): parent{parent}, left{}, right{}, data{std::move(d)} {
	}

	//deep copy semantics
	node(const node& other): node{nullptr, other.data} {
		if (other.left) {
			left.reset(new node{*other.left});
			left->parent = this;
		}
		if (other.right) {
			right.reset(new node{*other.right});
			right->parent = this;
		}
	}

	node& operator=(const node& other) {
		auto tmp = node{other};
		tmp.parent = parent;
		return *this = std::move(tmp);
	}

	//take ownership of subtree
	node(node&& other) noexcept = default;

	//take ownership of subtree, retaining parent
	node& operator=(node&& other) noexcept {
		auto tmp = node{std::move(other)};
		tmp.parent = parent;
		return *this = std::move(tmp);
	}

	virtual ~node() noexcept = default;

	std::size_t depth() const noexcept;

	void clear_children() noexcept {
		left.reset();
		right.reset();
	}

	node* leftmost() noexcept {
		return _leftmost(this);
	}

	const node* leftmost() const noexcept {
		return _leftmost(this);
	}

	node* first_right_ancestor() noexcept {
		return _first_right_ancestor(this);
	}

	const node* first_right_ancestor() const noexcept {
		return _first_right_ancestor(this);
	}
private:
	template<typename node_type>
	static node_type* _leftmost(node_type* root) noexcept;

	template<typename node_type>
	static node_type* _first_right_ancestor(node_type* root) noexcept;
};

template<typename P>
std::size_t node<P>::depth() const noexcept {
	std::size_t depth = 1;
	auto current = this;
	while (current->parent) {
		current = current->parent;
		assert(current != this);
		++depth;
	}

	return depth;
}

template<typename P>
template<typename node_type>
node_type* node<P>::_leftmost(node_type* root) noexcept {
	if (!root) {
		return nullptr;
	}

	node_type* current{root};
	while (current->left) {
		current = current->left.get();
		assert(current != root);
	}

	return current;
}

template<typename P>
template<typename node_type>
node_type* node<P>::_first_right_ancestor(node_type* root) noexcept {
	if (!root) {
		return nullptr;
	}

	node_type* current{root};
	while (current->parent && current == current->parent->right.get()) {
		current = current->parent;
		assert(current != root);
	}

	assert(current->parent != root);
	return current->parent;
}

template<typename node_type, typename ref_type>
class node_iterator {
	node_type* current;
public:
	using value_type = ref_type;
	using reference = value_type&;
	using pointer = value_type*;
	using difference_type = std::ptrdiff_t;
	using iterator_category = std::forward_iterator_tag;

	explicit node_iterator(node_type* n) noexcept: current{n} {
	}

	reference operator*() const noexcept {
		return current->data;
	}

	pointer operator->() const noexcept {
		return &**this;
	}

	node_iterator& operator++() noexcept;

	node_iterator operator++(int) noexcept {
		auto tmp(*this);
		++(*this);
		return tmp;
	}

	friend
	bool operator==(const node_iterator& lhs, const node_iterator& rhs) noexcept {
		return lhs.current == rhs.current;
	}

	friend
	bool operator!=(const node_iterator& lhs, const node_iterator& rhs) noexcept {
		return !(lhs == rhs);
	}

	std::size_t depth() const noexcept {
		return current ? current->depth() : 0;
	}
};

template<typename N, typename R>
node_iterator<N, R>& node_iterator<N, R>::operator++() noexcept {
	if (current->right) {
		current = current->right->leftmost();
	} else if (current->parent) {
		current = current->first_right_ancestor();
	} else {
		current = nullptr;
	}

	return *this;
}

enum class KeyLocation {PARENT, LEFT, RIGHT};

template<typename key_type, typename value_type, typename Comparator = std::less<key_type>>
class bst {
public:
	using pair_type = std::pair<const key_type, value_type>;

	using node_type = node<pair_type>;
	using iterator = node_iterator<node_type, pair_type>;
	using const_iterator = node_iterator<node_type, const pair_type>;

	bst() = default;

	//deep copy semantics, retaining structure, through node constructor; recursive, pre-order traversal
	bst(const bst& other): _size{other._size}, root{}, comparator{other.comparator} {
		if (other.root) {
			root.reset(new node_type{*(other.root.get())});
		}
	}

	bst& operator=(const bst& other) {
		clear();
		auto tmp = bst{other};
		return *this = std::move(tmp);
	}

	//move semantics through root's move
	bst(bst&& other) noexcept = default;

	bst& operator=(bst&& other) noexcept = default;

	virtual ~bst() noexcept {
		//iterative deletion of nodes
		clear();
	}

	Comparator key_comp() const {
		return comparator;
	}

	iterator begin() noexcept {
		return iterator{root ? root.get()->leftmost() : nullptr};
	}

	iterator end() noexcept {
		return iterator{nullptr};
	}

	const_iterator cbegin() const noexcept {
		return const_iterator{root ? root.get()->leftmost() : nullptr};
	}

	const_iterator cend() const noexcept {
		return const_iterator{nullptr};
	}

	const_iterator begin() const noexcept {
		return cbegin();
	}

	const_iterator end() const noexcept {
		return cend();
	}

	std::size_t size() const noexcept {
		return _size;
	}

	iterator find(const key_type& key) noexcept {
		return iterator{_find(key)};
	}

	const_iterator find(const key_type& key) const noexcept {
		return const_iterator{_find(key)};
	}

	std::pair<iterator, bool> insert(const pair_type& x) {
		return _insert(x);
	}

	std::pair<iterator, bool> insert(pair_type&& x) {
		return _insert(std::move(x));
	}

	template<typename... Types>
	std::pair<iterator, bool> emplace(Types&&...args) {
		return _insert(pair_type{std::forward<Types>(args)...});
	}

	void clear() noexcept;

	void balance();

	value_type& operator[](const key_type& key) {
		return _square_brackets(key);
	}

	value_type& operator[](key_type&& key) {
		return _square_brackets(std::move(key));
	}

	std::size_t depth() const noexcept;

	friend
	std::ostream& operator<<(std::ostream& os, const bst& tree) {
		os << "bst(" << tree.size() << ") {";
		for (const auto& p : tree) {
			os << "(" << p.first << ": " << p.second << "), ";
		}

		return os << "}";
	}
private:
	std::size_t _size;
	std::unique_ptr<node_type> root;
	Comparator comparator;

	std::pair<node_type*, KeyLocation> find_parent_candidate(node_type* root, const key_type& key) const;

	node_type* _find(const key_type& key) const;

	template<typename O>
	std::pair<iterator, bool> _insert(O&& x);

	template<typename O>
	value_type& _square_brackets(O&& key) {
		return _insert(pair_type{std::forward<O>(key), {}}).first->second;
	}

	using veciter = typename std::vector<pair_type>::iterator;
	void move_balanced(std::vector<pair_type>& vec, veciter b, veciter e);
};

template<typename K, typename V, typename C>
void bst<K, V, C>::clear() noexcept {
	//the recursive deletion of nodes can result in stack overflow in degenerate cases
	if (root) {
		auto current = root->leftmost();
		while (current) {
			if (current->right) {
				current = current->right->leftmost();
			} else if (current->parent) {
				while (current->parent && current == current->parent->right.get()) {
					current->clear_children();
					current = current->parent;
				}

				current->clear_children();
				current = current->parent;
			} else {
				current->clear_children();
				current = nullptr;
			}
		}
	}

	root.reset();
	_size = 0;
}

template<typename K, typename V, typename C>
void bst<K, V, C>::balance() {
	std::vector<pair_type> vec{};
	vec.reserve(_size);

	for (auto& p : *this) {
		vec.emplace_back(std::move(p));
	}

	clear();

	move_balanced(vec, vec.begin(), vec.end());
}

template<typename K, typename V, typename C>
std::size_t bst<K, V, C>::depth() const noexcept {
	if (!root) {
		return 0;
	}

	std::size_t depth = 0;
	for (auto iter = cbegin(); iter != cend(); ++iter) {
		depth = std::max(depth, iter.depth());
	}

	return depth;
}

template<typename K, typename V, typename C>
std::pair<typename bst<K, V, C>::node_type*, KeyLocation> bst<K, V, C>::find_parent_candidate(bst<K, V, C>::node_type* root, const K& key) const {
	assert(root);
	auto current = root;
	std::size_t i = 0;
	while (i != _size) {
		if (comparator(key, current->data.first)) {
			if (!current->left) {
				return std::make_pair(current, KeyLocation::LEFT);
			}
			current = current->left.get();
		} else if (comparator(current->data.first, key)) {
			if (!current->right) {
				return std::make_pair(current, KeyLocation::RIGHT);
			}
			current = current->right.get();
		} else {
			return std::make_pair(current, KeyLocation::PARENT);
		}

		++i;
	}

	return std::make_pair(nullptr, KeyLocation::PARENT);
}

template<typename K, typename V, typename C>
typename bst<K, V, C>::node_type* bst<K, V, C>::_find(const K& key) const {
	if (root) {
		auto search = find_parent_candidate(root.get(), key);
		assert(search.first);
		if (search.second == KeyLocation::PARENT) {
			return search.first;
		}
	}

	return nullptr;
}

template<typename K, typename V, typename C>
template<typename O>
std::pair<typename bst<K, V, C>::iterator, bool> bst<K, V, C>::_insert(O&& x) {
	if (!root) {
		root.reset(new node_type{nullptr, std::forward<O>(x)});
		++_size;

		return std::make_pair(iterator{root.get()}, true);
	}

	auto searched = find_parent_candidate(root.get(), x.first);
	auto parent = searched.first;
	assert(parent);
	switch (searched.second) {
		case KeyLocation::PARENT:
			return std::make_pair(iterator{parent}, false);
		case KeyLocation::LEFT:
			parent->left.reset(new node_type{parent, std::forward<O>(x)});
			++_size;

			return std::make_pair(iterator{parent->left.get()}, true);
		case KeyLocation::RIGHT:
			parent->right.reset(new node_type{parent, std::forward<O>(x)});
			++_size;

			return std::make_pair(iterator{parent->right.get()}, true);
		default:
			assert(false);
	}

	return std::make_pair(end(), false);
}

template<typename K, typename V, typename C>
void bst<K, V, C>::move_balanced(std::vector<bst<K, V, C>::pair_type>& vec, bst<K, V, C>::veciter b, bst<K, V, C>::veciter e) {
	if (b == e) {
		return;
	}

	auto mid = b + (std::distance(b, e) / 2);
	_insert(std::move(*mid));

	move_balanced(vec, b, mid);
	move_balanced(vec, mid + 1, e);
}

#endif
