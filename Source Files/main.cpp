#include "Mesh.h"

auto main(int argc, char** argv) -> int {
	AneuMeshLoader obj;
	std::string path{};
	bool neu;
	size_t choice{};
	std::cout << "1 - neu file" << std::endl;
	std::cout << "2 - aneu file" << std::endl;
	std::cin >> choice;
	switch (choice){
	case 1: { neu = true; break; }
	case 2: { neu = false; break; }
	default: { std::cout << "Invalid input" << std::endl; return 0; }
	}

	std::cout << "1 - classic launch" << std::endl;
	std::cout << "2 - console launch" << std::endl;
	std::cin >> choice;
	switch (choice) {
		case 1: {
			std::cout << "Enter file path: ";
			std::cin >> path;
			obj.loadMesh(path, neu);
			break;
		}
		case 2: {
			obj.loadMesh(argv[1], neu);
			break;
		}
		default: {
			std::cout << "Invalid input" << std::endl;
			return 0;
		}
	}	

	std::cout << std::endl << std::string(50, '-') << std::endl << std::endl;

	std::cout << "Nodes amount = " << obj.sizeNodes() << std::endl;
	std::cout << "Space Dimension = " << obj.spaceDim() << std::endl;
	std::cout << "Finite Elements amount = " << obj.sizeFiniteElements() << std::endl;
	std::cout << "Amount of Nodes in one Finite Element = " << obj.nodesInFE() << std::endl;
	std::cout << "Boundary Elements amount = " << obj.sizeBoundaryElements() << std::endl;
	std::cout << "Amount of Nodes in one Boundary Element = " << obj.nodesInBE() << std::endl;

	std::vector<Node> myNodes = obj.getNodes();
	std::vector<FiniteElement> myFiniteElements = obj.getFiniteElements();
	std::vector<BoundaryElement> mySurfaceFiniteElements = obj.getBoundaryElements();

	std::cout << std::endl;
	obj.print(*myNodes.begin(), std::cout);
	std::cout << std::endl;
	obj.print(*myFiniteElements.begin(), std::cout);
	std::cout << std::endl;
	obj.print(*mySurfaceFiniteElements.begin(), std::cout);
	std::cout << std::endl;

	obj.newNodesInEdges();

	std::cout << std::string(50,'-') << std::endl << std::endl;

	std::cout << "Nodes amount = " << obj.sizeNodes() << std::endl;
	std::cout << "Space Dimension = " << obj.spaceDim() << std::endl;
	std::cout << "Finite Elements amount = " << obj.sizeFiniteElements() << std::endl;
	std::cout << "Amount of Nodes in one Finite Element = " << obj.nodesInFE() << std::endl;
	std::cout << "Boundary Elements amount = " << obj.sizeBoundaryElements() << std::endl;
	std::cout << "Amount of Nodes in one Boundary Element = " << obj.nodesInBE() << std::endl;

	myNodes = obj.getNodes();
	myFiniteElements = obj.getFiniteElements();
	mySurfaceFiniteElements = obj.getBoundaryElements();

	//std::cout << std::endl;
	//obj.print(*myNodes.begin(), std::cout);
	//std::cout << std::endl;
	//obj.print(*myFiniteElements.begin(), std::cout);
	//std::cout << std::endl;
	//obj.print(*mySurfaceFiniteElements.begin(), std::cout);
	//std::cout << std::endl;

	std::cout << std::string(50, '-') << std::endl << std::endl;

	std::unordered_map<size_t, std::unordered_set<Node, Hash>> exp = obj.findNeighbours();
	for (const auto& pair : exp) {
		std::cout << pair.first << " = " << "{ ";
		for (const Node& node : pair.second) std::cout << node._id << "; ";
		std::cout << "}" << std::endl;
	}

	//for (const auto& node : myNodes) {
	//	obj.print(node, std::cout);
	//	std::cout << std::endl;
	//}

	return 0;
}
