#include <unistd.h>

#include <memory>

#include "gtest/gtest.h"
#include "aos/common/queue.h"
#include "aos/testing/test_shm.h"
#include "aos/common/actions/actor.h"
#include "y2015/actors/stack_action.q.h"
#include "y2015/actors/stack_actor.h"
#include "y2015/control_loops/fridge/fridge.q.h"

#include "aos/common/controls/control_loop_test.h"
#include "frc971/control_loops/team_number_test_environment.h"

using ::aos::time::Time;

namespace frc971 {
namespace actors {
namespace testing {

class StackActionTest : public ::testing::Test {
 protected:
  StackActionTest() {
    frc971::actors::stack_action.goal.Clear();
    frc971::actors::stack_action.status.Clear();
    control_loops::fridge_queue.status.Clear();
    control_loops::fridge_queue.goal.Clear();
  }

  virtual ~StackActionTest() {
    frc971::actors::stack_action.goal.Clear();
    frc971::actors::stack_action.status.Clear();
    control_loops::fridge_queue.status.Clear();
    control_loops::fridge_queue.goal.Clear();
  }

  // Bring up and down Core.
  ::aos::testing::TestSharedMemory my_shm_;
};

// Tests that cancel stops not only the stack action, but the underlying profile
// action.
TEST_F(StackActionTest, StackCancel) {
  StackActor stack(&frc971::actors::stack_action);

  frc971::actors::stack_action.goal.MakeWithBuilder().run(true).Send();

  // tell it the fridge is zeroed
  control_loops::fridge_queue.status.MakeWithBuilder()
      .zeroed(true)
      .angle(0.0)
      .height(0.0)
      .Send();

  // do the action and it will post to the goal queue
  stack.WaitForActionRequest();

  // the action has started, so now cancel it and it should cancel
  // the underlying profile
  frc971::actors::stack_action.goal.MakeWithBuilder().run(false).Send();

  // let the action start running, if we return from this call it has worked.
  StackParams s;
  stack.RunAction(s);

  SUCCEED();
}

}  // namespace testing
}  // namespace actors
}  // namespace frc971
