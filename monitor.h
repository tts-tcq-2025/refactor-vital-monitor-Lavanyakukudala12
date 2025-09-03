#pragma once
#include <string>
#include <map>
#include <vector>

// Types of vitals we track
enum class VitalType {
    Temperature,
    PulseRate,
    Spo2
};

// Allowed range for each vital
struct VitalRange {
    float minValue;
    float maxValue;
};

// Info about a violation
struct VitalAlert {
    VitalType vital;
    float actualValue;
    VitalRange expected;
    std::string status; // "too-low" or "too-high"
};

// Check readings against safe ranges
std::vector<VitalAlert> checkVitals(
    const std::map<VitalType, float>& readings,
    const std::map<VitalType, VitalRange>& limits
);

// Store and manage safe ranges
class VitalLimits {
public:
    void setLimit(VitalType vital, float min, float max);
    void changeMin(VitalType vital, float newMin);
    void changeMax(VitalType vital, float newMax);
    const std::map<VitalType, VitalRange>& getAll() const;

private:
    std::map<VitalType, VitalRange> ranges;
};

// Patient info (for age-based adjustments)
class Patient {
public:
    Patient(int ageYears);
    void tuneLimits(VitalLimits& registry) const;

private:
    int age;
};

// Alert/Logging utilities
namespace alerts {
    void show(const std::vector<VitalAlert>& violations);
}
