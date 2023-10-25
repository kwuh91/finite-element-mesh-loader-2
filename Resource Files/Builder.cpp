#include "Builder.h"

// display data
void Statistics::ShowData() const {
    if (_amountFEareaIdIsInitialized) {
        std::cout << "amount of finite elements with each material area id" << std::endl;
        std::cout << "e.g. map[material area id] = amount" << std::endl;
        for (const auto& el : _amountFEareaId) {
            std::cout << el.first 
                      << std::setw(digitsFormatingHelper3(el.first)) 
                      << " = " 
                      << el.second 
                      << std::endl;
        }
        std::cout << std::endl;
    }

    if (_amountBEareaIdIsInitialized) {
        std::cout << "amount of boundary elements with each surface area id" << std::endl;
        std::cout << "e.g. map[surface area id] = amount" << std::endl;
        for (const auto& el : _amountBEareaId) {
            std::cout << el.first 
                      << std::setw(digitsFormatingHelper3(el.first)) 
                      << " = " 
                      << el.second 
                      << std::endl;
        }
        std::cout << std::endl;
    }

    if (_amountFENodeIsInitialized) {
        std::cout << "how many times was a Node used in all of finite elements" << std::endl;
        std::cout << "e.g. map[node id] = amount of occurrences in finite elements" << std::endl;
        for (const auto& el : _amountFENode) {
            std::cout << el.first 
                      << std::setw(digitsFormatingHelper3(el.first)) 
                      << " = " 
                      << el.second 
                      << std::endl;
        }
        std::cout << std::endl;
    }

    if (_amountBENodeIsInitialized) {
        std::cout << "how many times was a Node used in all of boundary elements" << std::endl;
        std::cout << "e.g. map[node id] = amount of occurrences in boundary elements" << std::endl;
        for (const auto& el : _amountBENode) {
            std::cout << el.first 
                      << std::setw(digitsFormatingHelper3(el.first)) 
                      << " = " 
                      << el.second 
                      << std::endl;
        }
        std::cout << std::endl;
    }

    if (_commonNodeFEIsInitialized) {
        std::cout << "the most common Node in finite elements" << std::endl;
        std::cout << "{id, amount of occurrences}" << std::endl;
        std::cout << "{" 
                  << _commonNodeFE.first << ", " 
                  << _commonNodeFE.second << "}" 
                  << std::endl;
        std::cout << std::endl;
    }

    if (_commonNodeBEIsInitialized) {
        std::cout << "the most common Node in boundary elements" << std::endl;
        std::cout << "{id, amount of occurrences}" << std::endl;
        std::cout << "{" 
                  << _commonNodeBE.first << ", " 
                  << _commonNodeBE.second << "}" 
                  << std::endl;
        std::cout << std::endl;
    }

    if (!(_amountFEareaIdIsInitialized ||
        _amountBEareaIdIsInitialized   ||
        _amountFENodeIsInitialized     ||
        _amountBENodeIsInitialized     ||
        _commonNodeFEIsInitialized     ||
        _commonNodeBEIsInitialized)) std::cout << "No statistics were loaded" << std::endl;
}

// fill _amountFEareaId
void StatsBuilder::CountFEByAreaId(std::shared_ptr<AneuMeshLoader> obj) const {
    for (const FiniteElement& FE : obj->getFiniteElements()) {
        if (_stats->_amountFEareaId.count(FE._material_area_id))
            _stats->_amountFEareaId[FE._material_area_id]++;
        else _stats->_amountFEareaId[FE._material_area_id] = 1;
    }
    _stats->_amountFEareaIdIsInitialized = true;
}

// fill _amountBEareaId
void StatsBuilder::CountBEByAreaId(std::shared_ptr<AneuMeshLoader> obj) const {
    for (const BoundaryElement& BE : obj->getBoundaryElements()) {
        if (_stats->_amountBEareaId.count(BE._surface_area_id))
            _stats->_amountBEareaId[BE._surface_area_id]++;
        else _stats->_amountBEareaId[BE._surface_area_id] = 1;
    }
    _stats->_amountBEareaIdIsInitialized = true;
}

// fill _amountFENode
void StatsBuilder::CountNodesFE(std::shared_ptr<AneuMeshLoader> obj) const {
    for (const FiniteElement& FE : obj->getFiniteElements()) {
        for (const size_t& id : FE._nodeIDvec) {
            if (_stats->_amountFENode.count(id))
                _stats->_amountFENode[id]++;
            else _stats->_amountFENode[id] = 1;
        }
    }
    _stats->_amountFENodeIsInitialized = true;
}

// fill _amountBENode
void StatsBuilder::CountNodesBE(std::shared_ptr<AneuMeshLoader> obj) const {
    for (const BoundaryElement& BE : obj->getBoundaryElements()) {
        for (const size_t& id : BE._nodeIDvec) {
            if (_stats->_amountBENode.count(id))
                _stats->_amountBENode[id]++;
            else _stats->_amountBENode[id] = 1;
        }
    }
    _stats->_amountBENodeIsInitialized = true;
}

// fill _commonNodeFE
void StatsBuilder::CommonNodeFE(std::shared_ptr<AneuMeshLoader> obj) const {
    if (_stats->_amountFENodeIsInitialized) {
        using pair_type = decltype(_stats->_amountFENode)::value_type;
        const auto pair = std::ranges::max_element(_stats->_amountFENode,
            [](const pair_type& p1,
                const pair_type& p2) {
                    return p1.second < p2.second;});
        _stats->_commonNodeFE = { pair->first, pair->second };
        _stats->_commonNodeFEIsInitialized = true;
        return;
    }

    size_t maxim = std::numeric_limits<size_t>::min();
    size_t maxNodeId;
    std::unordered_map<size_t, size_t> temp_umap;

    for (const FiniteElement& FE : obj->getFiniteElements()) {
        for (const size_t& id : FE._nodeIDvec) {
            if (temp_umap.count(id)) {
                temp_umap[id]++;
                if (temp_umap[id] > maxim) {
                    maxim = temp_umap[id];
                    maxNodeId = id;
                }
            }
            else temp_umap[id] = 1;
        }
    }

    _stats->_commonNodeFE = { maxim, maxNodeId };
    _stats->_commonNodeFEIsInitialized = true;
}

// fill _commonNodeBE
void StatsBuilder::CommonNodeBE(std::shared_ptr<AneuMeshLoader> obj) const {
    if (_stats->_amountBENodeIsInitialized) {
        using pair_type = decltype(_stats->_amountBENode)::value_type;
        const auto pair = std::ranges::max_element(_stats->_amountBENode,
            [](const pair_type& p1,
                const pair_type& p2) {
                    return p1.second < p2.second;});
        _stats->_commonNodeBE = { pair->first, pair->second };
        _stats->_commonNodeBEIsInitialized = true;
        return;
    }

    size_t maxim = std::numeric_limits<size_t>::min();
    size_t maxNodeId;
    std::unordered_map<size_t, size_t> temp_umap;

    for (const BoundaryElement& BE : obj->getBoundaryElements()) {
        for (const size_t& id : BE._nodeIDvec) {
            if (temp_umap.count(id)) {
                temp_umap[id]++;
                if (temp_umap[id] > maxim) {
                    maxim = temp_umap[id];
                    maxNodeId = id;
                }
            }
            else temp_umap[id] = 1;
        }
    }

    _stats->_commonNodeBE = { maxim, maxNodeId };
    _stats->_commonNodeBEIsInitialized = true;
}
