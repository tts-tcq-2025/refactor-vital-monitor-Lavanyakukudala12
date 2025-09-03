#include "monitor.h"
#include <iostream>
#include <thread>
#include <chrono>

using std::cout;
using std::flush;
using std::this_thread::sleep_for;
using std::chrono::seconds;

// Compare actual readings with allowed ranges
std::vector<VitalAlert> checkVitals(
    const std::map<VitalType, float>& readings,
    const std::map<VitalType, VitalRange>& limits
) {
    std::vector<VitalAlert> results;

    for (const auto& [vital, value] : readings) {
        auto it = limits.find(vital);
        if (it == limits.end()) continue; // skip if not in limits

        const auto& range = it->second;
        if (value < range.minValue) {
            results.push_back({vital, value, range, "too-low"});
        } else if (value > range.maxValue) {
            results.push_back({vital, value, range, "too-high"});
        }
    }
    return results;
}

// Manage safe ranges
void VitalLimits::setLimit(VitalType vital, float min, float max) {
    ranges[vital] = {min, max};
}
void VitalLimits::changeMin(VitalType vital, float newMin) {
    if (ranges.count(vital)) ranges[vital].minValue = newMin;
}
void VitalLimits::changeMax(VitalType vital, float newMax) {
    if (ranges.count(vital)) ranges[vital].maxValue = newMax;
}
const std::map<VitalType, VitalRange>& VitalLimits::getAll() const {
    return ranges;
}

// Patient adjustments (based on age group)
Patient::Patient(int ageYears) : age(ageYears) {}

void Patient::tuneLimits(VitalLimits& registry) const {
    if (age < 13) { // children
        registry.changeMin(VitalType::PulseRate, 70);
        registry.changeMax(VitalType::Temperature, 100);
    } else if (age > 65) { // seniors
        registry.changeMax(VitalType::PulseRate, 90);
    }
}

// Alerts
namespace alerts {
    void show(const std::vector<VitalAlert>& violations) {
        if (violations.empty()) {
            cout << "Vitals are stable.\n";
            return;
        }

        for (const auto& v : violations) {
            switch (v.vital) {
                case VitalType::Temperature: cout << "Temperature"; break;
                case VitalType::PulseRate:   cout << "Pulse Rate"; break;
                case VitalType::Spo2:        cout << "SpO2"; break;
            }
            cout << " is " << v.status << " (" << v.actualValue << ")\n";

            // blinking effect
            for (int i = 0; i < 2; i++) {
                cout << "\r* " << flush;
                sleep_for(seconds(1));
                cout << "\r *" << flush;
                sleep_for(seconds(1));
            }
            cout << "\n";
        }
    }
}
