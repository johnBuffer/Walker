#include "./initialize.hpp"
#include "engine/engine.hpp"

#include "user/training/stadium.hpp"
#include "user/training/genome.hpp"
#include "user/training/target_sequence.hpp"
#include "user/training/training_state.hpp"
#include "user/training/walk.hpp"
#include "user/training/demo.hpp"

#include "user/training/render/renderer.hpp"


namespace training
{

void registerSystems()
{
    pez::core::registerSingleton<TrainingState>();

    pez::core::registerProcessor<Stadium>();
    pez::core::registerProcessor<Demo>();

    pez::core::registerDataEntity<Genome>();
    pez::core::registerDataEntity<Walk>();
    pez::core::registerDataEntity<TargetSequence>();

    pez::core::registerRenderer<Renderer>();
}

}
