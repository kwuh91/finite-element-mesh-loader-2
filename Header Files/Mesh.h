#pragma once

#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "DataTypes.h"
#include "Exception.h"

// Base abstract class
class MeshLoader abstract {
public:
	// virtual loadMesh method
	virtual void loadMesh(const std::string&, bool) = 0;

	// print method for Node/FiniteElement/BoundaryElement
	template <class T>
	static void print(T, std::ostream&);

	// virtual getter Node
	virtual std::vector<Node> getNodes() const = 0;

	// virtual getter Finite Element
	virtual std::vector<FiniteElement> getFiniteElements() const = 0;

	// virtual getter Surface Finite element
	virtual std::vector<BoundaryElement> getBoundaryElements() const = 0;

	// virtual getter for amount of Nodes
	virtual size_t sizeNodes() const = 0;

	// virtual getter for amount of Finite Elements
	virtual size_t sizeFiniteElements() const = 0;

	// virtual getter for amount of Surface Finite Elements
	virtual size_t sizeBoundaryElements() const = 0;

	// virtual getter for a Space Dimension 
	virtual size_t spaceDim() const = 0;

	// virtual getter for an amount of node in one Finite Element 
	virtual size_t nodesInFE() const = 0;

	// virtual getter for an amount of node in one Surface Finite Element 
	virtual size_t nodesInBE() const = 0;

	// virtual method for finding Finite Elements by 3 vertex Node ids
	virtual std::vector<FiniteElement> findFiniteElementsByVertices(size_t, size_t, size_t) = 0;

	// virtual method for finding Finite Elements by 2 Node ids
	virtual std::vector<FiniteElement> findFiniteElementsByEdges(size_t, size_t) = 0;

	// virtual method for finding Surface Finite Elements by an area ID
	virtual std::vector<BoundaryElement> findBoundaryElementsByAreaID(size_t) const = 0;

	// virtual method for finding Finite Elements by a material ID
	virtual std::vector<FiniteElement> findFiniteElementsByMaterialID(size_t) const = 0;

	// virtual method for finding all unique Nodes of Surface Finite Elements with the same given area ID
	virtual std::vector<Node> findBENodesByAreaID(size_t) = 0;

	// virtual method for adding new nodes to the centers of _FE and _SFE
	virtual void newNodesInEdges() = 0;

	// virtual method for neighbours
	virtual std::unordered_map<size_t, std::unordered_set<Node, Hash>> findNeighbours() const = 0;
};

// Derived class for the files with type *.aneu
class AneuMeshLoader : public MeshLoader {
public:
	// turn *.neu file to *.aneu
	static std::fstream NeuToAneu(std::fstream&, const std::string&);

	// loadMesh method in a derived class for the files with type *.aneu
	void loadMesh(const std::string&, bool);

	// getter Node in a derived class for the files with type *.aneu
	std::vector<Node> getNodes() const;

	// getter Finite Element in a derived class for the files with type *.aneu
	std::vector<FiniteElement> getFiniteElements() const;

	// getter Surface Finite element in a derived class for the files with type *.aneu
	std::vector<BoundaryElement> getBoundaryElements() const;

	// getter for amount of Nodes in a derived class for the files with type *.aneu
	size_t sizeNodes() const { return _nodesAmount; }

	// getter for amount of Finite Elements in a derived class for the files with type *.aneu
	size_t sizeFiniteElements() const { return _finiteElementsAmount; }

	// getter for amount of Surface Finite Elements in a derived class for the files with type *.aneu
	size_t sizeBoundaryElements() const { return _boundaryElementsAmount; }

	// getter for a Space Dimension in a derived class for the files with type *.aneu
	size_t spaceDim() const { return _spaceDimension; }

	// getter for an amount of node in one Finite Element in a derived class for the files with type *.aneu
	size_t nodesInFE() const { return _amountOfNodesInOneFiniteElement; }

	// getter for an amount of node in one Surface Finite Element in a derived class for the files with type *.aneu
	size_t nodesInBE() const { return _amountOfNodesInOneBoundaryElement; }

	// method for finding Finite Elements by 3 vertex Node ids
	std::vector<FiniteElement> findFiniteElementsByVertices(size_t, size_t, size_t);

	// method for finding Finite Elements by 2 Node ids
	std::vector<FiniteElement> findFiniteElementsByEdges(size_t, size_t);

	// method for finding Surface Finite Elements by an area ID
	std::vector<BoundaryElement> findBoundaryElementsByAreaID(size_t) const;

	// method for finding Finite Elements by a material ID
	std::vector<FiniteElement> findFiniteElementsByMaterialID(size_t) const;

	// method for finding all unique Nodes of Surface Finite Elements with the same given area ID
	std::vector<Node> findBENodesByAreaID(size_t);

	// method for adding new nodes to the centers of _FE and _SFE
	void newNodesInEdges();

	// method for neighbours
	std::unordered_map<size_t, std::unordered_set<Node, Hash>> findNeighbours() const;
private:
	size_t _nodesAmount{}; 
	size_t _spaceDimension{};
	size_t _finiteElementsAmount{};
	size_t _amountOfNodesInOneFiniteElement{}; 
	size_t _boundaryElementsAmount{};
	size_t _amountOfNodesInOneBoundaryElement{}; 
	std::unordered_map<size_t, Node> _nodesMap; 
	std::unordered_set<FiniteElement, Hash> _finiteElementsSet; 
	std::unordered_set<BoundaryElement, Hash> _boundaryElementsSet;
};

// definition for print method for Node/FiniteElement/BoundaryElement
template <class T>
void MeshLoader::print(T el, std::ostream& output) {
	if constexpr (std::is_same_v<T, Node>) {
		output << "Node id: " << el._id << std::endl;
		output << "Node _coords: { ";

		switch (el._dim) {
		case 2: { output << "x = " 
			<< el._coords[0] 
			<< "; y = " 
			<< el._coords[1] 
			<< " }" 
			<< std::endl; break; }
		case 3: { output << "x = " 
			<< el._coords[0] << "; y = " 
			<< el._coords[1] << "; z = " 
			<< el._coords[2] << " }" 
			<< std::endl; break; }
		case 4: { output << "x = " 
			<< el._coords[0] << "; y = " 
			<< el._coords[1] << "; z = " 
			<< el._coords[2] << "; t = " 
			<< el._coords[3] << " }" 
			<< std::endl; break; }
		default: {
			std::ranges::copy(el._coords, std::ostream_iterator<double>(output, " "));
			output << " }" << std::endl;
			break;
		}
		}
		output << "Is vertex?: " << el._is_vertex << std::endl;
		return;
	}

	if constexpr (std::is_same_v<T, FiniteElement>) {
		output << "Finite element id: " << el._id << std::endl;
		output << "Material id: ";
		output << el._material_area_id << std::endl;
		output << "Node id's: { ";

		std::ranges::copy(el._nodeIDvec, std::ostream_iterator<size_t>(output, " "));

		output << "}" << std::endl;
		return;
	}

	if constexpr (std::is_same_v<T, BoundaryElement>) {
		output << "Boundary element id: " << el._id << std::endl;
		output << "Area id: ";
		output << el._surface_area_id << std::endl;
		output << "Node id's: { ";

		std::ranges::copy(el._nodeIDvec, std::ostream_iterator<size_t>(output, " "));

		output << "}" << std::endl;
		return;
	}
}

#endif
