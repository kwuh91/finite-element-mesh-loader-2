#pragma once

#include "Mesh.h"

class Statistics {
public:
    // amount of finite elements with each material area id 
    // (e.g. map[material area id] = amount)
    std::unordered_map<size_t, size_t> _amountFEareaId;

    // helper flag for ShowData method
    bool _amountFEareaIdIsInitialized = false;


    // amount of boundary elements with each surface area id 
    // (e.g. map[surface area id] = amount)
    std::unordered_map<size_t, size_t> _amountBEareaId;

    // helper flag for ShowData method
    bool _amountBEareaIdIsInitialized = false;


    // how many times was a Node used in all of finite elements 
    // (e.g. map[node id] = amount of occurrences in finite elements)
    std::unordered_map<size_t, size_t> _amountFENode;

    // helper flag for ShowData method
    bool _amountFENodeIsInitialized = false;


    // how many times was a Node used in all of boundary elements 
    // (e.g. map[node id] = amount of occurrences in boundary elements)
    std::unordered_map<size_t, size_t> _amountBENode;

    // helper flag for ShowData method
    bool _amountBENodeIsInitialized = false;


    // the most common Node in finite elements 
    // {id, amount of occurrences}
    std::pair<size_t, size_t> _commonNodeFE;

    // helper flag for ShowData method
    bool _commonNodeFEIsInitialized = false;


    // the most common Node in boundary elements 
    // {id, amount of encounters}
    std::pair<size_t, size_t> _commonNodeBE;

    // helper flag for ShowData method
    bool _commonNodeBEIsInitialized = false;


    // display data
    void ShowData() const;
};

// abstract builder class for loading data from file
class GatherDataBuilder {
public:
    // destructor
    virtual ~GatherDataBuilder() {}

    // fill data
    virtual void load(const std::string&) const = 0;

    // getter
    virtual std::shared_ptr<AneuMeshLoader> GetObject() = 0;

    // reset field
    virtual void Reset() = 0;
};

// derived builder class for loading data from neu file
class GatherDataBuilderNeu : public GatherDataBuilder {
public:
    // constructor
    GatherDataBuilderNeu() { Reset(); }

    // destructor
    // ~GatherDataBuilderNeu() { delete _data; }

    // reset field
    void Reset() override { _data.reset(new AneuMeshLoader()); }

    // fill data
    void load(const std::string& path) const override { _data->loadMesh(path, true); }

    // getter
    std::shared_ptr<AneuMeshLoader> GetObject() override {
        std::shared_ptr<AneuMeshLoader> result = _data;
        Reset();
        return result;
    }

private:
    //AneuMeshLoader* _data;
    std::shared_ptr<AneuMeshLoader> _data;
};

// derived builder class for loading data from aneu file
class GatherDataBuilderAneu : public GatherDataBuilder {
public:
    // constructor
    GatherDataBuilderAneu() { Reset(); }

    // destructor
    // ~GatherDataBuilderAneu() { delete _data; }

    // reset field
    void Reset() override { _data.reset(new AneuMeshLoader()); }

    // fill data
    void load(const std::string& path) const override { _data->loadMesh(path, false); }

    // getter
    std::shared_ptr<AneuMeshLoader> GetObject() override {
        std::shared_ptr<AneuMeshLoader> result = _data;
        Reset();
        return result;
    }

private:
    std::shared_ptr<AneuMeshLoader> _data;
};

class GatherDataDirector {
public:
    // assign a pointer to the field
    void set_builder(std::shared_ptr<GatherDataBuilder> builder) { _builder = builder; }

    // fill data
    void GatherData(const std::string& path) const { _builder->load(path); }

private:
    std::shared_ptr<GatherDataBuilder> _builder;
};

// builder class for gathering statistics
class StatsBuilder {
public:
    // constructor
    StatsBuilder() { Reset(); }

    // destructor
    // ~StatsBuilder() { delete _stats; }

    // reset field
    void Reset() { _stats.reset(new Statistics()); }

    // fill _amountFEareaId
    void CountFEByAreaId(std::shared_ptr<AneuMeshLoader>) const;

    // fill _amountBEareaId
    void CountBEByAreaId(std::shared_ptr<AneuMeshLoader>) const;

    // fill _amountFENode
    void CountNodesFE(std::shared_ptr<AneuMeshLoader>) const;

    // fill _amountBENode
    void CountNodesBE(std::shared_ptr<AneuMeshLoader>) const;

    // fill _commonNodeFE
    void CommonNodeFE(std::shared_ptr<AneuMeshLoader>) const;

    // fill _commonNodeBE
    void CommonNodeBE(std::shared_ptr<AneuMeshLoader>) const;

    // getter
    std::shared_ptr<Statistics> GetStat() {
        std::shared_ptr<Statistics> result = _stats;
        Reset();
        return result;
    }

private:
    std::shared_ptr<Statistics> _stats;
};

class StatsDirector {
public:
    // assign a pointer to the field
    void set_builder(std::shared_ptr<StatsBuilder> builder) { _builder = builder; }

    // count only _amountFEareaId and _amountBEareaId
    void CountAmountOfElementsByAreaId(std::shared_ptr<AneuMeshLoader> obj) const {
        _builder->CountFEByAreaId(obj);
        _builder->CountBEByAreaId(obj);
    };

    // count only _amountFENode and _amountBENode
    void CountNodesInElements(std::shared_ptr<AneuMeshLoader> obj) const {
        _builder->CountNodesFE(obj);
        _builder->CountNodesBE(obj);
    }

    // count only _commonNodeFE and _commonNodeBE
    void CountCommonNodesInElements(std::shared_ptr<AneuMeshLoader> obj) const {
        _builder->CommonNodeFE(obj);
        _builder->CommonNodeBE(obj);
    }

    // count all of the statistics
    void CountAllStatistics(std::shared_ptr<AneuMeshLoader> obj) const {
        _builder->CountFEByAreaId(obj);
        _builder->CountBEByAreaId(obj);
        _builder->CountNodesFE(obj);
        _builder->CountNodesBE(obj);
        _builder->CommonNodeFE(obj);
        _builder->CommonNodeBE(obj);
    }

private:
    std::shared_ptr<StatsBuilder> _builder;
};

inline void ClientCode(GatherDataDirector& gdDirector, 
                       const std::string&  path, 
                       StatsDirector&      sDirector) {
    std::shared_ptr<GatherDataBuilderAneu> builder1(new GatherDataBuilderAneu());
    gdDirector.set_builder(builder1);
    gdDirector.GatherData(path);
    std::shared_ptr<AneuMeshLoader> obj = builder1->GetObject();
    //delete builder1;

    std::shared_ptr<StatsBuilder> builder2 (new StatsBuilder());
    sDirector.set_builder(builder2);
    sDirector.CountAmountOfElementsByAreaId(obj);

    std::shared_ptr<Statistics> stats = builder2->GetStat();
    stats->ShowData();
    std::cout << std::string(30, '-') << std::endl;
    //delete stats;

    sDirector.CountAllStatistics(obj);

    stats = builder2->GetStat();
    stats->ShowData();
    std::cout << std::string(30, '-') << std::endl;
    //delete stats;

    //delete obj;
    //delete builder2;
}

inline size_t digitsFormatingHelper3(size_t num) {
    size_t digits = 0;
    while (num) {
        digits++;
        num /= 10;
    }
    return (3 + 2) - digits;
}

//class Director {
//    // count only _amountFEareaId and _amountBEareaId
//    virtual void CountAmountOfElementsByAreaId(AneuMeshLoader*) const = 0;
//
//    // count only _amountFENode and _amountBENode
//    virtual void CountNodesInElements(AneuMeshLoader*) const = 0;
//
//    // count only _commonNodeFE and _commonNodeBE
//    virtual void CountCommonNodesInElements(AneuMeshLoader*) const = 0;
//
//    // count all of the statistics
//    virtual void CountAllStatistics(AneuMeshLoader*) const = 0;
//
//    // fill data
//    virtual void GatherData(const std::string&) const = 0;
//
//    // set_builder for GatherDataDirector
//    void set_builder(GatherDataBuilder*);
//
//    // set_builder for StatsDirector
//    void set_builder(StatsDirector*);
//};
