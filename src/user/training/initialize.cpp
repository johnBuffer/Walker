#include "./initialize.hpp"
#include "engine/engine.hpp"

#include "user/common/thread_pool/thread_pool.hpp"

#include "user/training/stadium.hpp"
#include "user/training/genome.hpp"
#include "user/training/agent.hpp"
#include "user/training/target_sequence.hpp"


namespace training
{

void registerSystems()
{
    pez::core::registerSingleton<tp::ThreadPool>(16);

    pez::core::registerProcessor<Stadium>();

    pez::core::registerDataEntity<Genome>();
}

}
