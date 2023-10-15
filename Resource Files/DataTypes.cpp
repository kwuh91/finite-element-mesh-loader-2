#include "DataTypes.h"

// Node overloading == for hash struct
bool operator == (Node const& lhs, 
				  Node const& rhs) {

	return (lhs._id == rhs._id) && 
		   (lhs._coords == rhs._coords) && 
		   (lhs._is_vertex == rhs._is_vertex);
}

// Finite Element overloading == for hash struct
bool operator == (FiniteElement const& lhs, 
				  FiniteElement const& rhs) {

	return (lhs._id == rhs._id) && 
		   (lhs._material_area_id == rhs._material_area_id) && 
		   (lhs._nodeIDvec == rhs._nodeIDvec);
}

// Surface Finite Element overloading == for hash struct
bool operator == (BoundaryElement const& lhs,
				  BoundaryElement const& rhs) {

	return (lhs._id == rhs._id) &&
		   (lhs._surface_area_id == rhs._surface_area_id) &&
		   (lhs._nodeIDvec == rhs._nodeIDvec);
}

// Node
size_t Hash::operator() (const Node& node) const {
	return node._id + static_cast<size_t>(
		std::floor(
			   std::accumulate(
				   begin(node._coords), end(node._coords), 0) +  
		   node._is_vertex));
}

// Finite Element
size_t Hash::operator() (const FiniteElement& FE) const {
	return FE._id +
		   FE._material_area_id +
	       FE._nodeIDvec.size();
}

// Surface Finite Element
size_t Hash::operator() (const BoundaryElement& BE) const {
	return BE._id +
		BE._surface_area_id +
		BE._nodeIDvec.size();
}

// function for string splicing
std::vector<std::string> splice(const std::string& str) {
	std::vector<std::string> res;
	std::string temp;

	for (size_t i = 0; i < str.size(); ++i) {
		if (str[i] != ' ' && 
			str[i] != '\r' && 
			str[i] != '\\') 
				temp.push_back(str[i]);

		else if (temp.size() > 0) {
			res.push_back(temp);
			temp.clear();
		}
	}
	if (temp.size() > 0) res.push_back(temp);
	return res;
}

// function for finding all possible unique combinations size 2
void combinationUtil(std::vector<std::vector<size_t>>& res, 
	                 std::vector<size_t>&              arr, 
	                 std::vector<size_t>&              data, 
	                 size_t                            start, 
	                 size_t                            end, 
	                 size_t                            index) {

	if (index == 2) {
		std::vector<size_t> temp{};
		for (size_t i = 0; i < 2; ++i) temp.push_back(data[i]);
		res.push_back(temp);
		return;
	}

	for (size_t i = start; i <= end && end - i + 1 >= 2 - index; ++i) {
		data[index] = arr[i];
		combinationUtil(res, arr, data, i + 1, end, index + 1);
	}
}
