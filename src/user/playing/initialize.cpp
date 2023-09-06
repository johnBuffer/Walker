#include "./initialize.hpp"
#include "engine/engine.hpp"

#include "./simulation/simulation.hpp"
#include "user/common/thread_pool/thread_pool.hpp"


namespace playing
{

void registerSystems()
{
    pez::core::registerSingleton<tp::ThreadPool>(16);

    pez::core::registerProcessor<PhysicSolver>(IVec2{480, 480});
    pez::core::registerProcessor<Simulation>();
}

}
