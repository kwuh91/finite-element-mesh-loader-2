#pragma once

#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#include <type_traits>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <set>
#include <unordered_map>
#include <array>	
#include <memory>
#include <ranges>

// custom DataType for Nodes 
class Node {
public:
	// default constructor
	Node() = default;

	// constructor
	Node(size_t size) : _coords(size), _dim(size) {}

	size_t _id{};
	std::vector<double> _coords;
	size_t _dim{};
	bool _is_vertex{};
};

// Node overloading == for hash struct
bool operator == (Node const&, 
				  Node const&);

// custom DataType for Finite Elements
class FiniteElement {
public:
	// bool _is_FE = true; // if true than it is a _FE else it is a _SFE
	size_t _id{};
	size_t _material_area_id{};
	std::vector<size_t> _nodeIDvec{}; // stores NodeIDs of a finite element
};

class BoundaryElement {
public:
	// bool _is_FE = true; // if true than it is a _FE else it is a _SFE
	size_t _id{};
	size_t _surface_area_id{};
	std::vector<size_t> _nodeIDvec{}; // stores NodeIDs of a surface finite element
};

// Finite Element overloading == for hash struct
bool operator == (FiniteElement const&, 
				  FiniteElement const&);

// Surface Finite Element overloading == for hash struct
bool operator == (BoundaryElement const&,
				  BoundaryElement const&);

// unordered_set functor for hashing
class Hash {
public:
	// Node
	size_t operator() (const Node&) const;

	// Finite Element
	size_t operator() (const FiniteElement&) const;

	// Surface Finite Element
	size_t operator() (const BoundaryElement&) const;
};

// set comparator for sorting
struct Compare final {
	bool operator()(const Node& lhs, 
					const Node& rhs) const noexcept {
		return lhs._id < rhs._id; // comparison logic
	}
};

// function for string splicing
std::vector<std::string> splice(const std::string&);

// function for finding all possible unique combinations size 2
void combinationUtil(std::vector<std::vector<size_t>>&, 
					 std::vector<size_t>&, 
	                 std::vector<size_t>&, 
	                 size_t, 
					 size_t, 
					 size_t);
