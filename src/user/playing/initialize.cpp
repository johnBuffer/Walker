#include "./initialize.hpp"
#include "engine/engine.hpp"

#include "./simulation/simulation.hpp"


namespace playing
{

void registerSystems()
{
    pez::core::registerProcessor<PhysicSolver>(IVec2{480, 480});
    pez::core::registerProcessor<Simulation>();
}

}
