#include "Mesh.h"

// Method definitions --- --- ---

// Derived class *.neu
// 
// definition for loadMesh method in a derived class for the files with type *.neu
void AneuMeshLoader::loadMesh(const std::string& path, bool neu) {

	std::fstream filename(path, std::ios_base::in |
		std::ios_base::binary);

	if (!filename.is_open())
		throw Exception("Unable to open file at specified path: " + path);

	if (neu) {
		size_t size{};
		std::vector < std::string > anuenamevec = splice(path);
		std::string aneuname = anuenamevec[anuenamevec.size() - 1];
		size = aneuname.size();
		aneuname.erase(size - 3);
		aneuname += "aneu";
		filename = AneuMeshLoader::NeuToAneu(filename, aneuname);

		if (!filename.is_open())
			throw Exception("Unable to open file at specified path: " + path);
	}

	std::string line;

	// reading nodes
	std::getline(filename, line);
	_nodesAmount = std::stoull(splice(line)[0]);

	size_t curr_id = 1;
	for (size_t i = 0; i < _nodesAmount; ++i) {
		std::getline(filename, line);
		if (i == 0) _spaceDimension = splice(line).size();
		Node currNode(_spaceDimension);
		currNode._id = curr_id;
		currNode._is_vertex = false;
		std::vector<std::string> coords = splice(line);

		std::ranges::transform(coords, begin(currNode._coords),
			[](const std::string& coord) {
				return std::stod(coord);
			});

		_nodesMap[curr_id] = currNode;
		curr_id++;
	}
	curr_id = 1;

	auto loadMeshUtil = [this, &line, &curr_id]<class Element>(std::fstream & filename, Element el) {
		// reading FE/BE elements
		std::getline(filename, line);

		if constexpr (std::is_same_v<Element, FiniteElement>)
			_finiteElementsAmount = std::stoull(splice(line)[0]);
		else
			_boundaryElementsAmount = std::stoull(splice(line)[0]);

		size_t amount{};
		if constexpr (std::is_same_v<Element, FiniteElement>)
			amount = _finiteElementsAmount;
		else
			amount = _boundaryElementsAmount;

		for (size_t i = 0; i < amount; ++i) {
			std::getline(filename, line);
			// getting amount of nodes in one (surface) finite element
			if (i == 0) {
				if constexpr (std::is_same_v<Element, FiniteElement>)
					_amountOfNodesInOneFiniteElement = splice(line).size() - 1;
				else
					_amountOfNodesInOneBoundaryElement = splice(line).size() - 1;
			}
			// finite elements
			if constexpr (std::is_same_v<Element, FiniteElement>) {
				FiniteElement currFiniteElement{};
				currFiniteElement._id = curr_id;
				std::vector<std::string> ids = splice(line);
				currFiniteElement._material_area_id = std::stoull(ids[0]);

				for (size_t j = 1; j < ids.size(); ++j) {
					size_t currNodeID = std::stoull(ids[j]);
					currFiniteElement._nodeIDvec.push_back(currNodeID);
					if (_nodesMap.contains(currNodeID)) _nodesMap[currNodeID]._is_vertex = true;
				}

				_finiteElementsSet.insert(currFiniteElement);
			}
			// surface finite elements
			else {
				BoundaryElement currSurfaceFiniteElement{};
				currSurfaceFiniteElement._id = curr_id;
				std::vector<std::string> ids = splice(line);
				currSurfaceFiniteElement._surface_area_id = std::stoull(ids[0]);

				for (size_t j = 1; j < ids.size(); ++j) {
					size_t currNodeID = std::stoull(ids[j]);
					currSurfaceFiniteElement._nodeIDvec.push_back(currNodeID);
					if (_nodesMap.contains(currNodeID)) _nodesMap[currNodeID]._is_vertex = true;
				}

				_boundaryElementsSet.insert(currSurfaceFiniteElement);
			}
			curr_id++;
		}
	};

	FiniteElement FEblank{}; BoundaryElement SFEblank{};
	loadMeshUtil(filename, FEblank);
	loadMeshUtil(filename, SFEblank);

	filename.close();
}

// definition for getter Node in a derived class for the files with type *.neu
std::vector<Node> AneuMeshLoader::getNodes() const {
	std::vector<Node> res;

	std::ranges::transform(_nodesMap, std::back_inserter(res),
		[](const auto& el) {
			return el.second;
		});

	std::ranges::sort(res, {}, &Node::_id);
	return res;
}

// definition for getter Finite Element in a derived class for the files with type *.neu
std::vector<FiniteElement> AneuMeshLoader::getFiniteElements() const {
	std::vector<FiniteElement> res;
	std::ranges::copy(_finiteElementsSet, std::back_inserter(res));
	std::ranges::sort(res, {}, &FiniteElement::_id);
	return res;
}

// definition for getter Surface Finite element in a derived class for the files with type* .neu
std::vector<BoundaryElement> AneuMeshLoader::getBoundaryElements() const {
	std::vector<BoundaryElement> res;
	std::ranges::copy(_boundaryElementsSet, std::back_inserter(res));
	std::ranges::sort(res, {}, &BoundaryElement::_id);
	return res;
}

// definition for method for finding Finite Elements by 3 vertex Node ids
std::vector<FiniteElement> AneuMeshLoader::findFiniteElementsByVertices(size_t node1id, 
																		size_t node2id, 
																		size_t node3id) {
	if (!(_nodesMap.contains(node1id) && 
		  _nodesMap.contains(node2id) && 
		  _nodesMap.contains(node3id)))
		throw Exception("One or more nodes are not present in the loaded data");

	Node& node1 = _nodesMap[node1id];
	Node& node2 = _nodesMap[node2id];
	Node& node3 = _nodesMap[node3id];

	if (!(node1._is_vertex && node2._is_vertex && node3._is_vertex)) 
		throw Exception("Not all nodes are vertices");

	std::vector<FiniteElement> res{};

	std::ranges::copy_if(_finiteElementsSet, std::back_inserter(res),
		[&node1id, &node2id, &node3id](const FiniteElement& el) {
			return 
			std::ranges::find(el._nodeIDvec, node1id) != end(el._nodeIDvec) &&
			std::ranges::find(el._nodeIDvec, node2id) != end(el._nodeIDvec) &&
			std::ranges::find(el._nodeIDvec, node3id) != end(el._nodeIDvec);
		});

	std::ranges::sort(res, {}, &FiniteElement::_id);
	return res;
}

// definition for method for finding Finite Elements by 2 Node ids
std::vector<FiniteElement> AneuMeshLoader::findFiniteElementsByEdges(size_t node1id, 
																	 size_t node2id) {
	if (!(_nodesMap.contains(node1id) && 
		  _nodesMap.contains(node2id))) 
		throw Exception("One or more nodes are not present in the loaded data");

	Node& node1 = _nodesMap[node1id];
	Node& node2 = _nodesMap[node2id];
	std::vector<FiniteElement> res{};

	std::ranges::copy_if(_finiteElementsSet, std::back_inserter(res),
		[&node1id, &node2id](const FiniteElement& el) {
			return
			std::ranges::find(el._nodeIDvec, node1id) != end(el._nodeIDvec) &&
			std::ranges::find(el._nodeIDvec, node2id) != end(el._nodeIDvec);
		});

	std::ranges::sort(res, {}, &FiniteElement::_id);
	return res;
}

// definition for method for finding Surface Finite Elements by an area ID
std::vector<BoundaryElement> AneuMeshLoader::findBoundaryElementsByAreaID(size_t areaid) const {
	std::vector<BoundaryElement> res{};
	std::ranges::copy(_boundaryElementsSet, std::back_inserter(res));
	std::ranges::sort(res, {}, &BoundaryElement::_id);
	return res;
}

// definition for method for finding Finite Elements by a material ID
std::vector<FiniteElement> AneuMeshLoader::findFiniteElementsByMaterialID(size_t materialid) const {
	std::vector<FiniteElement> res{};
	std::ranges::copy(_finiteElementsSet, std::back_inserter(res));
	std::ranges::sort(res, {}, &FiniteElement::_id);
	return res;
}

// definition for method for finding all unique Nodes of Surface Finite Elements with the same given area ID
std::vector<Node> AneuMeshLoader::findBENodesByAreaID(size_t areaid) {
	std::vector<Node> res{}; std::set<Node, Compare> uninodes{};

	for (const BoundaryElement& el : _boundaryElementsSet) {
		if (el._surface_area_id == areaid) {
			for (const size_t& id : el._nodeIDvec) uninodes.insert(_nodesMap[id]);
		}
	}
	std::ranges::move(uninodes, std::back_inserter(res));
	return res;
}

// definition for method for adding new nodes to the centers of _FE and _SFE
void AneuMeshLoader::newNodesInEdges() {
	size_t newNodesAmount = _nodesAmount;
	size_t NodeId = _nodesAmount + 1;
	bool FE = true, BE = true;

	for (FiniteElement el : _finiteElementsSet) {
		std::vector<size_t> nodes{};

		std::ranges::copy(el._nodeIDvec, std::back_inserter(nodes));

		std::vector<std::vector<size_t>> combinations{};
		std::vector<size_t> data(2);
		size_t n = nodes.size();
		combinationUtil(combinations, nodes, data, 0, n - 1, 0);

		for (const std::vector<size_t>& pairOfNodes : combinations) {
			Node newnode(_spaceDimension);
			newnode._id = NodeId;
			std::vector<double>& coords1 = _nodesMap[pairOfNodes[0]]._coords;
			std::vector<double>& coords2 = _nodesMap[pairOfNodes[1]]._coords;

			for (size_t i = 0; i < _spaceDimension; ++i) {
				double coord = (coords1[i] + coords2[i]) / 2;
				newnode._coords[i] = coord;
			}

			_nodesMap[NodeId] = newnode;
			el._nodeIDvec.push_back(NodeId);

			NodeId++;
		}

		if (FE) { _amountOfNodesInOneFiniteElement = el._nodeIDvec.size();
				  FE = false; }
	}

	for (BoundaryElement el : _boundaryElementsSet) {
		std::vector<size_t> nodes{};

		std::ranges::copy(el._nodeIDvec, std::back_inserter(nodes));

		std::vector<std::vector<size_t>> combinations{};
		std::vector<size_t> data(2);
		size_t n = nodes.size();
		combinationUtil(combinations, nodes, data, 0, n - 1, 0);

		for (const std::vector<size_t>& pairOfNodes : combinations) {
			Node newnode(_spaceDimension);
			newnode._id = NodeId;
			std::vector<double>& coords1 = _nodesMap[pairOfNodes[0]]._coords;
			std::vector<double>& coords2 = _nodesMap[pairOfNodes[1]]._coords;

			for (size_t i = 0; i < _spaceDimension; ++i) {
				double coord = (coords1[i] + coords2[i]) / 2;
				newnode._coords[i] = coord;
			}

			_nodesMap[NodeId] = newnode;
			el._nodeIDvec.push_back(NodeId);

			NodeId++;
		}

		if (BE) { _amountOfNodesInOneBoundaryElement = el._nodeIDvec.size();
			       BE = false;
		}
	}

	_nodesAmount = NodeId - 1;
}

// method for neighbours
std::unordered_map<size_t, std::unordered_set<Node, Hash>> AneuMeshLoader::findNeighbours() const {
	std::unordered_map <size_t, std::unordered_set<Node, Hash>> res;

	for (const FiniteElement& el : _finiteElementsSet) {
		size_t size = el._nodeIDvec.size();
		for (const size_t& i : el._nodeIDvec) {
			for (const size_t& j : el._nodeIDvec) {
				if (i != j) {
					if (res.contains(j)) res[i].insert(_nodesMap.at(j));
					else {
						std::unordered_set<Node, Hash> temp{ _nodesMap.at(j) };
						res[i] = temp;
					}
				}
			}
		}
	}

	return res;
}

// Derived class *.aneu
// 
// definition for NeuToAneu method (turn *.neu file to *.aneu)
std::fstream AneuMeshLoader::NeuToAneu(std::fstream&       filename, 
									   const std::string&  aneuFileName) {

	std::fstream newfile(aneuFileName, std::ios_base::out  | 
								       std::ios_base::in   |
									   std::fstream::trunc |
									   std::ios_base::binary);

	if (!newfile.is_open()) 
		throw Exception("Unable to create file");

	auto NeuToAneuUtil = [](std::fstream& neu, 
							std::fstream& aneu, bool flag) {
		std::string line;
		size_t data, count;

		// reading first line of the block
		std::getline(neu, line);
		count = std::stoull(line);

		// saving a position 
		std::streampos oldpos = neu.tellg();

		// reading line with info
		std::getline(neu, line);
		data = flag ? splice(line).size() - 1 : splice(line).size();

		// write to the *.aneu file
		line = std::to_string(count) + " " + std::to_string(data) + "\r";
		aneu << line << std::endl;

		// go back to old position
		neu.seekg(oldpos);

		// iterating through the block
		for (size_t i = 0; i < count; ++i) {
			std::getline(neu, line);
			aneu << line << std::endl;
		}
	};

	size_t flag(0);
	while (filename.peek() != EOF) NeuToAneuUtil(filename, newfile, flag++);

	filename.close();
	newfile.seekg(0, std::ios::beg);
	return newfile;
}
