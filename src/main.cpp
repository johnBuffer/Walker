#include "user/training/training.hpp"
#include "user/playing/playing.hpp"
#include "user/training/walk.hpp"


int main()
{
    static_assert(std::is_move_constructible<Walk>::value);
    //return Playing::main();
    return Training::main();
}
