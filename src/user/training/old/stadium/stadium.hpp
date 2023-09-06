#pragma once

#include "engine/common/index_vector.hpp"

#include "./selector.hpp"
#include "./agent.hpp"
#include "./task.hpp"

#include "user/common/neat_old/genome.hpp"
#include "user/common/thread_pool/thread_pool.hpp"


/** The Stadium class is responsible for agents selection
 * The process consists of successive iterations. Each iteration is divided in multiple steps:
 *  - Initialization:
 *    - Prepare agents for the step
 *    - Prepare task
 *
 *  - Task execution and evaluation
 *    - Let agents execute the task
 *    - Monitor their performances and compute their score
 *
 *  - Selection and evolution
 *    - Best agents are selected
 *    - Random mutations are applied
 */

template<typename TTaskContext>
class Stadium
{
public:
    /** User interface **/

    /// Default constructor
    explicit
    Stadium(std::vector<TTaskContext>& tasks)
        : m_tasks{tasks}
        , m_tread_pool{16}
    {
        static_assert(std::is_convertible<TTaskContext*, Task*>::value , "Provided class must derive from Task");
    }

    /// Initializes the iteration
    void initializeIteration()
    {
        m_time = 0.0f;
        for (auto& task : m_tasks) {
            task.initialize();
        }
    }

    /// Runs the task
    void executeIteration(float dt)
    {
        initializeIteration();
        m_tread_pool.dispatch(to<uint32_t>(m_tasks.size()), [this, dt](uint32_t start, uint32_t end) {
            float t = 0.0f;
            while (t < m_task_max_time) {
                bool done = true;
                for (uint32_t i{start}; i < end; ++i) {
                    if (!m_tasks[i].done()) {
                        m_tasks[i].update(dt);
                        done = false;
                    }
                }
                if (done) {
                    break;
                }
                t += dt;
            }
        });

        updateScores();
    }

    void updateScores()
    {
        for (auto& task : m_tasks) {
            task.computeScore();
        }
    }

    float getTaskTime() const
    {
        return m_task_max_time;
    }

private:
    /** Implementation **/

    /// Thread pool used to parallelize tasks execution
    tp::ThreadPool m_tread_pool;

    /// Reference to the tasks container
    std::vector<TTaskContext>& m_tasks;

    /// Task timeout
    float m_task_max_time = 100.0f;
    float m_time          = 0.0f;

    /// Check if the current iteration is over
    [[nodiscard]]
    bool done() const
    {
        if (m_time > m_task_max_time) {
            return true;
        }

        for (const auto& task : m_tasks) {
            if (!task.done()) {
                return false;
            }
        }
        return true;
    }
};
