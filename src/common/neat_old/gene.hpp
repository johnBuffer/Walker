#pragma once
#include "neat_id.hpp"


namespace nt
{

// Describes network's connections
struct Gene
{
    // Connection
    ID id_from = ID{0};
    ID id_to   = ID{0};
    // NN weight
    float weight;
    // NEAT attributes
    ID   innovation_id = ID{0};
    bool active        = true;
    Gene() = default;
    Gene(ID from, ID to, float w, ID inn_id)
        : id_from(from)
        , id_to(to)
        , weight(w)
        , innovation_id(inn_id)
    {}
};

}