// Make use of the CMake build system or compile manually, e.g. with:
// g++ -std=c++11 example.cc -lsweep

#include <chrono>
#include <cstdlib>
#include <iostream>
#include <thread>

#include <sweep/sweep.hpp>

class Timer {
public:
  Timer() : beg_(clock_::now()) {}
  void reset() { beg_ = clock_::now(); }
  double elapsed() const { return std::chrono::duration_cast<second_>(clock_::now() - beg_).count(); }

private:
  typedef std::chrono::high_resolution_clock clock_;
  typedef std::chrono::duration<double, std::ratio<1>> second_;
  std::chrono::time_point<clock_> beg_;
};

bool reset_defaults(sweep::sweep& device) try {
  device.set_sample_rate(500);
  device.set_motor_speed(5);
  return true;
} catch (const sweep::device_error& e) {
  std::cerr << "\tError: " << e.what() << std::endl;
  return false;
}

bool test_motor_speeds(sweep::sweep& device, int speed_from, int speed_to) try {
  std::cout << "Performing test: " << speed_from << "Hz to " << speed_to << "Hz..." << std::endl;
  Timer tmr;
  device.set_motor_speed(speed_from);
  while (!device.get_motor_ready()) {
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
  }
  double duration = tmr.elapsed();
  std::cout << "\tReset to " << speed_from << "Hz, took " << duration << " seconds to stabilize & calibrate." << std::endl;

  tmr.reset();
  device.set_motor_speed(speed_to);
  device.start_scanning();
  duration = tmr.elapsed();
  std::cout << "\tAdjusting from " << speed_from << "Hz to " << speed_to << "Hz, took " << duration
            << " seconds to stabilize, calibrate & start scanning." << std::endl;
  device.stop_scanning();

  return true;
} catch (const sweep::device_error& e) {
  std::cerr << "\tError: " << e.what() << std::endl;
  return false;
}

bool test_scanning(sweep::sweep& device) try {
  std::cout << "\tInitiating scanning. Will commence after stabilization and calibration is complete..." << std::endl;
  device.start_scanning();
  for (auto n = 0; n < 10; ++n) {
    const sweep::scan scan = device.get_scan();
    std::cout << "\t\tScan #" << n << " has " << scan.samples.size() << " samples" << std::endl;
  }

  std::cout << "\tAllowing scans to queue up for 10 seconds..." << std::endl;
  std::this_thread::sleep_for(std::chrono::seconds(10));
  std::cout << "\tStopping scanning..." << std::endl;
  device.stop_scanning();
  return true;
} catch (const sweep::device_error& e) {
  std::cerr << "\tError: " << e.what() << std::endl;
  return false;
}

bool test_sample_rates(sweep::sweep& device) try {
  std::vector<int> rates = {500, 750, 1000};
  for (const int rate : rates) {
    std::cout << "\tSetting sample rate setting to: " << rate << "Hz" << std::endl;
    device.set_sample_rate(rate);
    std::cout << "\tConfirming sample rate setting is: " << device.get_sample_rate() << "Hz" << std::endl;
  }
  std::cout << "\tResetting defaults..." << std::endl;
  reset_defaults(device);
  return true;
} catch (const sweep::device_error& e) {
  std::cerr << "\tError: " << e.what() << std::endl;
  return false;
}

int main(int argc, char* argv[]) try {
  if (argc != 2) {
    std::cerr << "Usage: ./test-c++ device\n";
    return EXIT_FAILURE;
  }

  std::cout << "Constructing sweep device..." << std::endl;
  sweep::sweep device{argv[1]};

  std::cout << "Performing Sample Rate Test..." << std::endl;
  std::cout << (test_sample_rates(device) ? "Passed" : "Failed") << std::endl;

  std::cout << "Performing Scanning Test..." << std::endl;
  std::cout << (test_scanning(device) ? "Passed" : "Failed") << std::endl;

  std::cout << "Performing Motor Speed Tests..." << std::endl;
  for (auto from = 0; from < 10; ++from) {
    for (auto to = 1; to < 10; ++to) {
      std::cout << (test_motor_speeds(device, from, to) ? "Passed" : "Failed") << std::endl;
    }
  }
} catch (const sweep::device_error& e) {
  std::cerr << "Error: " << e.what() << std::endl;
}
