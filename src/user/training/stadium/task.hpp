#pragma once


struct Task
{
    /// Prepare for the task before its execution
    virtual void initialize() = 0;

    /// The update to perform at each timestep of the task's execution
    virtual void update(float dt) = 0;

    /// Compute the resulting score
    virtual void computeScore() = 0;

    /// Check if the the task is finished
    [[nodiscard]]
    virtual bool done() const = 0;
};
