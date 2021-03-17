#include "displayobject.hpp"
#include <cassert>
#include <chrono>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <unistd.h>
#include <vector>

namespace {
// Original coordinates before animation begins (if any).
constexpr int BAKERY_X = 1, BAKERY_Y = 85, EGG_BARN_X = 6, EGG_BARN_Y = 20,
              SUGAR_BARN_X = 1, SUGAR_BARN_Y = 34, FARMER1_X = 22,
              FARMER1_Y = 3, CHICKEN1_X = 16, CHICKEN1_Y = 1, CHICKEN2_X = 16,
              CHICKEN2_Y = 22, NEST1_X = 20, NEST1_Y = 1, NEST3_X = 20,
              NEST3_Y = 25, NEST2_X = 20, NEST2_Y = 13, CHILD1_X = 13,
              CHILD1_Y = 122, CHILD2_X = 18, CHILD2_Y = 122, CHILD3_X = 23,
              CHILD3_Y = 122, CHILD4_X = 28, CHILD4_Y = 122, CHILD5_X = 33,
              CHILD5_Y = 122, INSIDE_STORE_X = 8, INSIDE_STORE_Y = 120,
              CONVEYOR_BELT_BEGIN_Y = 86, CONVEYOR_BELT_END_Y = 98,
              RIGHT_BOUNDARY_Y = 130, MIXER_CENTER_X = 18, MIXER_CENTER_Y = 107,
              CUPCAKE_CENTER_X = 4, CUPCAKE_CENTER_Y = 106, INTERSECT_X = 6,
              INTERSECT_Y = 46, TRUCK1_X = 6, TRUCK1_Y = 30, TRUCK2_X = 1,
              TRUCK2_Y = 44;

DisplayObject intersection("\
  __|   |_________________#\
                    #\
  --|   |-----------------",
                           0);

DisplayObject nest1[4] = {
    DisplayObject("\
\\       /#\
 -------",
                  0),

    DisplayObject("\
\\   O   /#\
 -------",
                  0),

    DisplayObject("\
\\  OO   /#\
 -------",
                  0),

    DisplayObject("\
\\  OOO  /#\
 -------",
                  0),
};

DisplayObject nest2[4] = {
    DisplayObject("\
\\       /#\
 -------",
                  0),

    DisplayObject("\
\\   O   /#\
 -------",
                  0),

    DisplayObject("\
\\  OO   /#\
 -------",
                  0),

    DisplayObject("\
\\  OOO  /#\
 -------",
                  0),
};

DisplayObject nest3[4] = {
    DisplayObject("\
\\       /#\
 -------",
                  0),

    DisplayObject("\
\\   O   /#\
 -------",
                  0),

    DisplayObject("\
\\  OO   /#\
 -------",
                  0),

    DisplayObject("\
\\  OOO  /#\
 -------",
                  0),
};

DisplayObject nest4[4] = {
    DisplayObject("\
\\       /#\
 -------",
                  0),

    DisplayObject("\
\\   O   /#\
 -------",
                  0),

    DisplayObject("\
\\  OO   /#\
 -------",
                  0),

    DisplayObject("\
\\  OOO  /#\
 -------",
                  0),
};

DisplayObject chicken1("\
     O>#\
   ^(1)#\
    = =",
                       2);

DisplayObject chicken2("\
     O>#\
   ^(2)#\
    = =",
                       2);

DisplayObject cow("\
   ____ \"#\
 /(____)U#\
  !! !!",
                  3);

DisplayObject farmer("\
   _#\
  O#\
/(~)\\#\
 ! !",
                     1);

DisplayObject child1("\
  * *#\
 /(1)\\#\
  ! !",
                     1);

DisplayObject child2("\
  * *#\
 /(2)\\#\
  ! !",
                     1);

DisplayObject child3("\
  * *#\
 /(3)\\#\
  ! !",
                     1);

DisplayObject child4("\
  * *#\
 /(4)\\#\
  ! !",
                     1);

DisplayObject child5("\
  * *#\
 /(5)\\#\
  ! !",
                     1);

DisplayObject egg_barn("\
   __ ^#\
  /  /  \\#\
 |  | _  |#\
 |  |[ ] |",
                       3);

DisplayObject sugar_barn("\
   __ ^#\
  /  /  \\#\
 |  | _  |#\
 |  |[ ] |",
                         3);

DisplayObject bakery("\
-----------------------------------|#\
|                                  |#\
                                   |#\
                                   |#\
                                   -----------#\
 =================                           |#\
|o   o   o   o   o                           |#\
                      _________              |#\
                       ^^^^^^^               |#\
 =================                  #\
|o   o   o   o   o    |_______|    -----------#\
                        oven       |#\
                                   |#\
 =================                 |#\
|o   o   o   o   o                 |#\
                                   |#\
                                   |#\
 =================   (     )       |#\
|o   o   o   o   o   (_____)       |#\
|                     mixer        |#\
|                                  |#\
|                                  |#\
-----------------------------------|#\
      Anne's Patisserie",
                     0);

DisplayObject truck1("\
   _______#\
  |       |__#\
  -OO----OO-O|",
                     3);

DisplayObject truck2("\
   _______#\
  |       |__#\
  -OO----OO-O|",
                     3);

DisplayObject flour1("\
\\~~~~/#\
(flour)",
                     2);

DisplayObject flour2("\
\\~~~~/#\
(flour)",
                     2);

DisplayObject sugar1("\
\\~~~~/#\
(sugar)",
                     2);

DisplayObject sugar2("\
\\~~~~/#\
(sugar)",
                     2);

DisplayObject butter1("[butter]", 2);
DisplayObject butter2("[butter]", 2);

DisplayObject eggs1("[OOO]", 2);
DisplayObject eggs2("[OOO]", 2);

DisplayObject batter("[ccc]", 2);

std::string mixer_string;
DisplayObject mixer_contents(mixer_string, 3);

DisplayObject cupcakes[7] = {DisplayObject("   ________#\
   /       /#\
  /       /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   /       /#\
  /     c /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   /       /#\
  /   c c /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   /       /#\
  / c c c /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   /     c /#\
  / c c c /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   /   c c /#\
  / c c c /#\
  _______",
                                           2),
                             DisplayObject("   ________#\
   / c c c /#\
  / c c c /#\
  _______",
                                           2)};

int EggsLaid = 0, EggsCollected = 0, EggCratesUsed = 0, EggsInBarn = 0,
    ButterInBarn = 0, ButterProduced = 0, ButterUsed = 0, SugarProduced = 0,
    SugarUsed = 0, FlourProduced = 0, FlourUsed = 0, CakesProduced = 0,
    CakesSold = 0, NestOneEggCount = 0, NestTwoEggCount = 0,
    NestThreeEggCount = 0, TruckEggCrates = 0, TruckButter = 0, TruckSugar = 0,
    TruckFlour = 0, EggCratesInMixture = 0, FlourInMixture = 0,
    SugarInMixture = 0, ButterInMixture = 0, NumberOfCupcakesAvailable = 0,
    BatchesAvailable = 0, CupcakesWanted = 0, Truck1_X = TRUCK1_X,
    Truck1_Y = TRUCK1_Y, Truck2_X = TRUCK2_X, Truck2_Y = TRUCK2_Y,
    Chicken1_X = CHICKEN1_X, Chicken1_Y = CHICKEN1_Y, Chicken2_X = CHICKEN2_X,
    Chicken2_Y = CHICKEN2_Y, Farmer_X = FARMER1_X, Farmer_Y = FARMER1_Y;

bool Child1Completed = false, Child2Completed = false, Child3Completed = false,
     Child4Completed = false, Child5Completed = true, nest1_is_open = true,
     nest2_is_open = true, nest3_is_open = true, intersection_is_open = true;

std::condition_variable barn_has_enough_eggs, truck1_emptied, truck2_emptied,
    move_single_egg_crate, move_single_sugar, move_single_flour,
    move_single_butter, batches_are_available, cupcake_stand_capacity_available,
    enough_cupcakes_available, child1_done, child2_done, child3_done,
    child4_done, child5_done, nest1_open, nest2_open, nest3_open,
    intersection_open;

void redisplay() {
  DisplayObject::redisplay();
  std::cout << "Eggs: Laid=" << EggsLaid << ", Used=" << EggCratesUsed
            << std::endl
            << "Butter: Sold=" << ButterProduced << ", Used=" << ButterUsed
            << std::endl
            << "Sugar: Sold=" << SugarProduced << ", Used=" << SugarUsed
            << std::endl
            << "Flour: Sold=" << FlourProduced << ", Used=" << FlourUsed
            << std::endl
            << "Cakes: Baked=" << CakesProduced << ", Sold=" << CakesSold
            << ", Wanted=" << CupcakesWanted << std::endl;
}

} // namespace

int main(int argc, char **argv) {
  std::cout << "\nWelcome to Farmville! Choose a speed-mode: "
               "\n[1] Super Speed Mode"
               "\n[2] Okay Mode\n";
  int hyperspeed_input;
  std::cin >> hyperspeed_input;
  const bool hyperspeed = hyperspeed_input == 1 ? true : false;

  using namespace std::chrono_literals;
  // Latency between animation movement in x- or y-directions.
  const std::chrono::milliseconds ANIMATION_LATENCY(hyperspeed ? 10ms : 100ms);
  const std::chrono::milliseconds SLOWER_ANIMATION_LATENCY(hyperspeed ? 20ms
                                                                      : 200ms);
  // How often the re-display refreshes.
  const std::chrono::milliseconds REFRESH_RATE = hyperspeed ? 10ms : 100ms;
  // Baking period for one batch in the oven.
  const std::chrono::milliseconds BAKING_TIME = hyperspeed ? 20ms : 3000ms;
  // Egg-laying and farmer collection delay periods.
  const std::chrono::milliseconds CHICKEN_DELAY(
      hyperspeed ? 10 : 1000 + std::rand() % 1000);
  const std::chrono::milliseconds FARMER_DELAY(
      hyperspeed ? 10 : 1000 + std::rand() % 1000);
  const std::chrono::milliseconds MILK_COW_DELAY(
      hyperspeed ? 10 : 1000 + std::rand() % 1000);

  std::mutex refresh_mutex;
  std::vector<std::thread> threads;

  auto Chicken1GoToNest2 = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      ++Chicken1_Y;
      const bool reached_nest2 = (Chicken1_Y == NEST2_Y - 1);
      chicken1.draw(Chicken1_X, Chicken1_Y);
      if (!reached_nest2)
        continue;
      if (NestTwoEggCount == 3)
        return;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      const int egg_count = std::min(3 - NestTwoEggCount, 1 + std::rand() % 3);
      if (egg_count + NestTwoEggCount <= 3) {
        NestTwoEggCount += egg_count;
        EggsLaid += egg_count;
      }
      return;
    }
  };

  auto Chicken1GoToNest1 = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      --Chicken1_Y;
      const bool reached_nest1 = (Chicken1_Y == NEST1_Y);
      chicken1.draw(Chicken1_X, Chicken1_Y);
      if (!reached_nest1)
        continue;
      if (NestOneEggCount == 3)
        return;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      const int egg_count = std::min(3 - NestOneEggCount, 1 + std::rand() % 3);
      if (egg_count + NestOneEggCount <= 3) {
        NestOneEggCount += egg_count;
        EggsLaid += egg_count;
      }
      return;
    }
  };

  auto Chicken1ChillOut = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      --Chicken1_Y;
      chicken1.draw(Chicken1_X, Chicken1_Y);
      if (!(Chicken1_Y == NEST2_Y - 7))
        continue;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      return;
    }
  };

  auto Chicken2GoToNest2 = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      --Chicken2_Y;
      const bool reached_nest2 = (Chicken2_Y == NEST2_Y);
      chicken2.draw(Chicken2_X, Chicken2_Y);
      if (!reached_nest2)
        continue;
      if (NestTwoEggCount == 3)
        return;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      const int egg_count = std::min(3 - NestTwoEggCount, 1 + std::rand() % 3);
      if (egg_count + NestTwoEggCount <= 3) {
        NestTwoEggCount += egg_count;
        EggsLaid += egg_count;
      }
      return;
    }
  };

  auto Chicken2GoToNest3 = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      ++Chicken2_Y;
      const bool reached_nest3 = (Chicken2_Y == NEST3_Y);
      chicken2.draw(Chicken2_X, Chicken2_Y);
      if (!reached_nest3)
        continue;
      if (NestThreeEggCount == 3)
        return;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      const int egg_count = std::min(3 - NestThreeEggCount, std::rand() % 3);
      if (egg_count + NestThreeEggCount <= 3) {
        NestThreeEggCount += egg_count;
        EggsLaid += egg_count;
      }
      return;
    }
  };

  auto Chicken2ChillOut = [&]() -> void {
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      ++Chicken2_Y;
      chicken2.draw(Chicken2_X, Chicken2_Y);
      if (!(Chicken2_Y == NEST3_Y - 5))
        continue;
      lock.unlock();
      std::this_thread::sleep_for(CHICKEN_DELAY);
      lock.lock();
      return;
    }
  };

  auto FarmerSearchLoop = [&]() -> void {
    bool forward = false;
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      Farmer_Y = forward ? Farmer_Y - 1 : Farmer_Y + 1;
      const bool reached_nest1 = (Farmer_Y == NEST1_Y),
                 reached_nest2 = (Farmer_Y == NEST2_Y + 1),
                 reached_nest3 = (Farmer_Y == NEST3_Y + 1);
      farmer.draw(Farmer_X, Farmer_Y);
      if (reached_nest1) {
        forward = false;
        if (NestOneEggCount == 0)
          continue;
        if (!nest1_is_open)
          nest1_open.wait(lock, [&]() { return nest1_is_open; });
        nest1_is_open = false;
        lock.unlock();
        std::this_thread::sleep_for(FARMER_DELAY);
        lock.lock();
        EggsCollected += NestOneEggCount;
        NestOneEggCount = 0;
        nest1_is_open = true;
        nest1_open.notify_one();

      } else if (reached_nest2 && NestTwoEggCount) {
        if (!nest2_is_open)
          nest2_open.wait(lock, [&]() { return nest2_is_open; });
        nest2_is_open = false;
        lock.unlock();
        std::this_thread::sleep_for(FARMER_DELAY);
        lock.lock();
        EggsCollected += NestTwoEggCount;
        NestTwoEggCount = 0;
        nest2_is_open = true;
        nest2_open.notify_one();

      } else if (reached_nest3) {
        forward = true;
        if (NestThreeEggCount == 0)
          continue;
        if (!nest3_is_open)
          nest3_open.wait(lock, [&]() { return nest1_is_open; });
        nest3_is_open = false;
        lock.unlock();
        std::this_thread::sleep_for(FARMER_DELAY);
        lock.lock();
        EggsCollected += NestThreeEggCount;
        NestThreeEggCount = 0;
        nest3_is_open = true;
        nest3_open.notify_one();
      }
      if (EggsCollected >= 9)
        return;
    }
  };

  auto FarmerGoToBarn = [&]() -> void {
    bool right = true, left = false, up = false, outside_of_nests = false;
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      Farmer_Y += right;
      Farmer_Y -= left;
      Farmer_X -= up;
      if (Farmer_Y == NEST3_Y + 10) {
        right = false;
        up = true;
        outside_of_nests = true;
      }
      if (Farmer_X == EGG_BARN_X + 4) {
        up = false;
        left = true;
      }
      farmer.draw(Farmer_X, Farmer_Y);
      if (outside_of_nests && Farmer_Y == EGG_BARN_Y + 3) {
        lock.unlock();
        std::this_thread::sleep_for(MILK_COW_DELAY);
        lock.lock();
        ButterInBarn += 3;
        EggsInBarn += EggsCollected;
        EggsCollected = 0;
        barn_has_enough_eggs.notify_one();
        return;
      }
    }
  };

  auto FarmerGoToNests = [&]() -> void {
    bool right = true, left = false, down = false, outside_of_barn = false;
    for (;;) {
      std::this_thread::sleep_for(SLOWER_ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      Farmer_Y += right;
      Farmer_Y -= left;
      Farmer_X += down;
      if (Farmer_Y == NEST3_Y + 10) {
        right = false;
        down = true;
        outside_of_barn = true;
      }
      if (Farmer_X == FARMER1_X) {
        down = false;
        left = true;
      }
      farmer.draw(Farmer_X, Farmer_Y);
      if (outside_of_barn && Farmer_Y == EGG_BARN_Y + 3)
        return;
    }
  };

  auto Truck1GoEastThroughIntersection = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck1.draw(Truck1_X, ++Truck1_Y);
      if (Truck1_Y > INTERSECT_Y + 14)
        return;
    }
  };

  auto Truck1GoWestThroughIntersection = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck1.draw(Truck1_X, --Truck1_Y);
      if (Truck1_Y < 30)
        return;
    }
  };

  auto Truck1GoEastToBakery = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck1.draw(Truck1_X, ++Truck1_Y);
      if (Truck1_Y > BAKERY_Y - 14) {
        TruckEggCrates = 3;
        TruckButter = 3;
        truck1_emptied.wait(
            lock, [&]() { return TruckEggCrates == 0 && TruckButter == 0; });
        return;
      }
    }
  };

  auto Truck1GoWestToIntersection = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck1.draw(Truck1_X, --Truck1_Y);
      if (Truck1_Y < INTERSECT_Y + 14)
        return;
    }
  };

  auto Truck2Down = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck2.draw(++Truck2_X, Truck2_Y);
      if (Truck2_X > INTERSECT_X - 4)
        return;
    }
  };

  auto Truck2GoSouthThroughIntersection = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck2.draw(++Truck2_X, Truck2_Y);
      if (Truck2_X > INTERSECT_X + 8)
        return;
    }
  };

  auto Truck2GoRightToBakery = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck2.draw(Truck2_X, ++Truck2_Y);
      if (Truck2_Y <= BAKERY_Y - 15)
        continue;
      TruckFlour = 3;
      TruckSugar = 3;
      truck2_emptied.wait(lock,
                          [&]() { return TruckFlour == 0 && TruckSugar == 0; });
      return;
    }
  };
  auto Truck2GoBackToIntersection = [&]() -> auto {
    bool left = true, up = false;
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      Truck2_X -= up;
      Truck2_Y -= left;
      truck2.draw(Truck2_X, Truck2_Y);
      if (Truck2_Y <= 44) {
        left = false;
        up = true;
      }
      if (Truck2_X <= INTERSECT_X + 3)
        return;
    }
  };

  auto Truck2GoNorthThroughIntersection = [&]() -> auto {
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      truck2.draw(--Truck2_X, Truck2_Y);
      if (Truck2_X < 3)
        return;
    }
  };

  const auto MoveSingleAcrossConveyorBelt =
      [&](DisplayObject &object, int X, int &update, int &decrement) -> void {
    int y = CONVEYOR_BELT_BEGIN_Y;
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      object.draw(X, ++y);
      if (y == CONVEYOR_BELT_END_Y) {
        object.draw(-1, -1);
        --decrement;
        ++update;
        return;
      }
    }
  };

  const auto GoIntoStore = [&](DisplayObject &child, int X, int Y,
                               int cupcakes_wanted) -> void {
    bool left = false, right = true, up = false, reached_store = false;
    int x = X, y = Y;
    const int ORIGINAL_Y = Y;
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      y += right;
      y -= left;
      x -= up;
      if (y == RIGHT_BOUNDARY_Y) {
        right = false;
        up = true;
      }
      if (x == INSIDE_STORE_X) {
        reached_store = true;
        up = false;
        left = true;
      }
      if (y == ORIGINAL_Y && reached_store) {
        child.draw(x, y);
        int cupcakes_taken = 0;
        while (cupcakes_taken < cupcakes_wanted) {
          enough_cupcakes_available.wait(
              lock, [&]() { return NumberOfCupcakesAvailable > 0; });
          --NumberOfCupcakesAvailable;
          ++cupcakes_taken;
          if (6 - NumberOfCupcakesAvailable >= 3)
            cupcake_stand_capacity_available.notify_one();
        }
        CakesSold += cupcakes_wanted;
        CupcakesWanted = 0;
        cupcake_stand_capacity_available.notify_one();
        return;
      }
      child.draw(x, y);
    }
  };

  const auto ExitFromStore = [&](DisplayObject &child, int ChildPositionX,
                                 int ChildPositionY) -> void {
    bool left = false, down = false, right = true, hit_boundary = false;
    int x = INSIDE_STORE_X, y = INSIDE_STORE_Y;
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      y += right;
      y -= left;
      x += down;
      if (y == RIGHT_BOUNDARY_Y) {
        right = false;
        down = true;
        hit_boundary = true;
      }
      if (hit_boundary && x == ChildPositionX) {
        down = false;
        left = true;
      }
      if (hit_boundary && y == ChildPositionY) {
        child.draw(x, y);
        return;
      }
      child.draw(x, y);
    }
  };

  threads.emplace_back([&]() -> auto { // Chicken 1.
    std::unique_lock<std::mutex> lock(refresh_mutex);
    lock.unlock();
    for (;;) {
      chicken1.draw(Chicken1_X, Chicken1_Y);
      lock.lock();
      nest2_open.wait(lock, [&]() { return nest2_is_open; });
      lock.unlock();
      nest2_is_open = false;
      Chicken1GoToNest2();
      nest2_is_open = true;
      nest2_open.notify_one();

      Chicken1ChillOut();

      lock.lock();
      nest1_open.wait(lock, [&]() { return nest1_is_open; });
      lock.unlock();
      nest1_is_open = false;
      Chicken1GoToNest1();
      nest1_is_open = true;
      nest1_open.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Chicken 2.
    std::unique_lock<std::mutex> lock(refresh_mutex);
    lock.unlock();
    for (;;) {
      chicken2.draw(Chicken2_X, Chicken2_Y);
      lock.lock();
      nest2_open.wait(lock, [&]() { return nest2_is_open; });
      lock.unlock();
      nest2_is_open = false;
      Chicken2GoToNest2();
      nest2_is_open = true;
      nest2_open.notify_one();

      Chicken2ChillOut();

      lock.lock();
      nest3_open.wait(lock, [&]() { return nest3_is_open; });
      lock.unlock();
      nest3_is_open = false;
      Chicken2GoToNest3();
      nest3_is_open = true;
      nest3_open.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Nest 1.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::scoped_lock<std::mutex> lock(refresh_mutex);
      nest1[NestOneEggCount % 4].draw(NEST1_X, NEST1_Y);
    }
  });
  threads.emplace_back([&]() -> auto { // Nest 2.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::scoped_lock<std::mutex> lock(refresh_mutex);
      nest2[NestTwoEggCount % 4].draw(NEST2_X, NEST2_Y);
    }
  });
  threads.emplace_back([&]() -> auto { // Nest 3.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::scoped_lock<std::mutex> lock(refresh_mutex);
      nest3[NestThreeEggCount % 4].draw(NEST3_X, NEST3_Y);
    }
  });

  threads.emplace_back([&]() -> auto { // Farmer.
    for (;;) {
      FarmerSearchLoop();
      FarmerGoToBarn();
      FarmerGoToNests();
    }
  });

  threads.emplace_back([&]() -> auto { // Truck 1.
    std::unique_lock<std::mutex> lock(refresh_mutex);
    lock.unlock();
    for (;;) {
      lock.lock();
      if (EggsInBarn < 9 || ButterInBarn < 3) {
        truck1.draw(Truck1_X, Truck1_Y);
        barn_has_enough_eggs.wait(
            lock, [&] { return EggsInBarn >= 9 && ButterInBarn >= 3; });
      }
      EggsInBarn -= 9;
      ButterInBarn -= 3;
      intersection_open.wait(lock, [&]() { return intersection_is_open; });
      intersection_is_open = false;
      lock.unlock();
      Truck1GoEastThroughIntersection();
      intersection_is_open = true;
      intersection_open.notify_one();
      Truck1GoEastToBakery();
      Truck1GoWestToIntersection();
      lock.lock();
      intersection_open.wait(lock, [&]() { return intersection_is_open; });
      intersection_is_open = false;
      lock.unlock();
      Truck1GoWestThroughIntersection();
      intersection_is_open = true;
      intersection_open.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Truck 2.
    std::unique_lock<std::mutex> lock(refresh_mutex);
    lock.unlock();
    for (;;) {
      FlourProduced += 3;
      SugarProduced += 3;
      Truck2Down();
      lock.lock();
      intersection_open.wait(lock, [&]() { return intersection_is_open; });
      intersection_is_open = false;
      lock.unlock();
      Truck2GoSouthThroughIntersection();
      intersection_is_open = true;
      intersection_open.notify_one();
      Truck2GoRightToBakery();
      Truck2GoBackToIntersection();
      lock.lock();
      intersection_open.wait(lock, [&]() { return intersection_is_open; });
      intersection_is_open = false;
      lock.unlock();
      Truck2GoNorthThroughIntersection();
      intersection_is_open = true;
      intersection_open.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Butter.
    for (;;) {
      std::unique_lock<std::mutex> lock(refresh_mutex);
      move_single_butter.wait(
          lock, [&]() { return ButterInMixture < 2 && TruckButter > 0; });
      lock.unlock();
      MoveSingleAcrossConveyorBelt(butter1, 5, ButterInMixture, TruckButter);
      if (TruckButter == 0 && TruckEggCrates == 0)
        truck1_emptied.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Egg Crate.
    for (;;) {
      std::unique_lock<std::mutex> lock(refresh_mutex);
      move_single_egg_crate.wait(
          lock, [&]() { return EggCratesInMixture < 2 && TruckEggCrates > 0; });
      lock.unlock();
      MoveSingleAcrossConveyorBelt(eggs1, 9, EggCratesInMixture,
                                   TruckEggCrates);
      if (TruckButter == 0 && TruckEggCrates == 0)
        truck1_emptied.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Sugar.
    for (;;) {
      std::unique_lock<std::mutex> lock(refresh_mutex);
      move_single_sugar.wait(
          lock, [&]() { return SugarInMixture < 2 && TruckSugar > 0; });
      lock.unlock();
      MoveSingleAcrossConveyorBelt(sugar1, 13, SugarInMixture, TruckSugar);
      if (TruckSugar == 0 && TruckFlour == 0)
        truck2_emptied.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Flour.
    for (;;) {
      std::unique_lock<std::mutex> lock(refresh_mutex);
      move_single_flour.wait(
          lock, [&]() { return FlourInMixture < 2 && TruckFlour > 0; });
      lock.unlock();
      MoveSingleAcrossConveyorBelt(flour1, 17, FlourInMixture, TruckFlour);
      if (TruckSugar == 0 && TruckFlour == 0)
        truck2_emptied.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Mixer.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      if (EggCratesInMixture == 2 && FlourInMixture == 2 &&
          SugarInMixture == 2 && ButterInMixture == 2) {
        EggCratesInMixture = 0;
        FlourInMixture = 0;
        SugarInMixture = 0;
        ButterInMixture = 0;
        EggCratesUsed += 2;
        FlourUsed += 2;
        SugarUsed += 2;
        ButterUsed += 2;
        mixer_string = "c c c#c c c";
        BatchesAvailable = 2;
      } else if (BatchesAvailable == 1) {
        mixer_string = "c c c";
      } else if (BatchesAvailable == 2) {
        mixer_string = "c c c#c c c";
      } else {
        if (TruckEggCrates > 0 && EggCratesInMixture <= 1) {
          assert(EggCratesInMixture <= 1);
          move_single_egg_crate.notify_one();
        }
        if (TruckButter > 0 && ButterInMixture <= 1) {
          assert(ButterInMixture <= 1);
          move_single_butter.notify_one();
        }
        if (TruckSugar > 0 && SugarInMixture <= 1) {
          assert(SugarInMixture <= 1);
          move_single_sugar.notify_one();
        }
        if (TruckFlour > 0 && FlourInMixture <= 1) {
          assert(FlourInMixture <= 1);
          move_single_flour.notify_one();
        }
        mixer_string = "    ";
        assert(EggCratesInMixture <= 2 && FlourInMixture <= 2 &&
               SugarInMixture <= 2 && ButterInMixture <= 2);
        mixer_string[0] =
            EggCratesInMixture == 0 ? ' ' : EggCratesInMixture == 1 ? 'e' : 'E';
        mixer_string[1] =
            FlourInMixture == 0 ? ' ' : FlourInMixture == 1 ? 'f' : 'F';
        mixer_string[2] =
            SugarInMixture == 0 ? ' ' : SugarInMixture == 1 ? 's' : 'S';
        mixer_string[3] =
            ButterInMixture == 0 ? ' ' : ButterInMixture == 1 ? 'b' : 'B';
      }
      if (BatchesAvailable > 0)
        batches_are_available.notify_one();
      assert(BatchesAvailable <= 2);
      mixer_contents.update_contents(mixer_string);
      mixer_contents.draw(MIXER_CENTER_X, MIXER_CENTER_Y);
    }
  });

  threads.emplace_back([&]() -> auto { // Oven.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      batches_are_available.wait(lock, [&]() { return BatchesAvailable > 0; });
      cupcake_stand_capacity_available.wait(
          lock, [&]() { return NumberOfCupcakesAvailable + 3 <= 6; });
      assert(BatchesAvailable > 0 && BatchesAvailable < 3);
      assert(NumberOfCupcakesAvailable + 3 <= 6);
      batter.update_contents("[ccc]");
      batter.draw(10, 109);
      BatchesAvailable -= 1;
      lock.unlock();
      std::this_thread::sleep_for(BAKING_TIME);
      lock.lock();
      CakesProduced += 3;
      NumberOfCupcakesAvailable += 3;
      batter.update_contents("");
      batter.draw(10, 109);
      enough_cupcakes_available.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Cupcakes.
    for (;;) {
      std::this_thread::sleep_for(ANIMATION_LATENCY);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      cupcakes[NumberOfCupcakesAvailable].draw(CUPCAKE_CENTER_X,
                                               CUPCAKE_CENTER_Y);
    }
  });

  threads.emplace_back([&]() -> auto { // Child 1.
    for (;;) {
      child1.draw(CHILD1_X, CHILD1_Y);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      child5_done.wait(lock, [&] { return Child5Completed; });
      Child5Completed = false;
      lock.unlock();

      CupcakesWanted = 1 + (std::rand() % 6);
      GoIntoStore(child1, CHILD1_X, CHILD1_Y, CupcakesWanted);
      ExitFromStore(child1, CHILD1_X, CHILD1_Y);

      Child1Completed = true;
      child1_done.notify_one();
    }
  });
  threads.emplace_back([&]() -> auto { // Child 2.
    for (;;) {
      child2.draw(CHILD2_X, CHILD2_Y);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      child1_done.wait(lock, [&] { return Child1Completed; });
      Child1Completed = false;
      lock.unlock();

      CupcakesWanted = 1 + (std::rand() % 6);
      GoIntoStore(child2, CHILD2_X, CHILD2_Y, CupcakesWanted);
      ExitFromStore(child2, CHILD2_X, CHILD2_Y);

      Child2Completed = true;
      child2_done.notify_one();
    }
  });
  threads.emplace_back([&]() -> auto { // Child 3.
    for (;;) {
      child3.draw(CHILD3_X, CHILD3_Y);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      child2_done.wait(lock, [&] { return Child2Completed; });
      Child2Completed = false;
      lock.unlock();

      CupcakesWanted = 1 + (std::rand() % 6);
      GoIntoStore(child3, CHILD3_X, CHILD3_Y, CupcakesWanted);
      ExitFromStore(child3, CHILD3_X, CHILD3_Y);

      Child3Completed = true;
      child3_done.notify_one();
    }
  });
  threads.emplace_back([&]() -> auto { // Child 4.
    for (;;) {
      child4.draw(CHILD4_X, CHILD4_Y);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      child3_done.wait(lock, [&] { return Child3Completed; });
      Child3Completed = false;
      lock.unlock();

      CupcakesWanted = 1 + (std::rand() % 6);
      GoIntoStore(child4, CHILD4_X, CHILD4_Y, CupcakesWanted);
      ExitFromStore(child4, CHILD4_X, CHILD4_Y);

      Child4Completed = true;
      child4_done.notify_one();
    }
  });
  threads.emplace_back([&]() -> auto { // Child 5.
    for (;;) {
      child5.draw(CHILD5_X, CHILD5_Y);
      std::unique_lock<std::mutex> lock(refresh_mutex);
      child4_done.wait(lock, [&] { return Child4Completed; });
      Child4Completed = false;
      lock.unlock();

      CupcakesWanted = 1 + (std::rand() % 6);
      GoIntoStore(child5, CHILD5_X, CHILD5_Y, CupcakesWanted);
      ExitFromStore(child5, CHILD5_X, CHILD5_Y);

      Child5Completed = true;
      child5_done.notify_one();
    }
  });

  threads.emplace_back([&]() -> auto { // Non-animated objects.
    std::scoped_lock<std::mutex> lock(refresh_mutex);
    egg_barn.draw(EGG_BARN_X, EGG_BARN_Y);
    sugar_barn.draw(SUGAR_BARN_X, SUGAR_BARN_Y);
    bakery.draw(BAKERY_X, BAKERY_Y);
    cow.draw(EGG_BARN_X + 5, EGG_BARN_Y - 5);
    intersection.draw(INTERSECT_X, INTERSECT_Y);
  });

  threads.emplace_back([&]() -> auto { // Display
    for (;;) {
      std::this_thread::sleep_for(REFRESH_RATE);
      std::scoped_lock<std::mutex> lock(refresh_mutex);
      redisplay();
    }
  });

  for (auto &t : threads)
    t.join();
}