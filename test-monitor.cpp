#include <gtest/gtest.h>
#include "monitor.h"

// ✅ Functional tests
TEST(VitalCheck, NormalValues) {
    std::map<VitalType, float> readings = {
        {VitalType::Temperature, 98.5},
        {VitalType::PulseRate, 75},
        {VitalType::Spo2, 95}
    };
    std::map<VitalType, VitalRange> limits = {
        {VitalType::Temperature, {95, 102}},
        {VitalType::PulseRate, {60, 100}},
        {VitalType::Spo2, {90, 100}}
    };

    auto issues = checkVitals(readings, limits);
    EXPECT_TRUE(issues.empty());
}

TEST(VitalCheck, SingleViolation) {
    std::map<VitalType, float> readings = {
        {VitalType::Temperature, 104}
    };
    std::map<VitalType, VitalRange> limits = {
        {VitalType::Temperature, {95, 102}}
    };

    auto issues = checkVitals(readings, limits);
    ASSERT_EQ(issues.size(), 1);
    EXPECT_EQ(issues[0].status, "too-high");
    EXPECT_EQ(issues[0].vital, VitalType::Temperature);
}

TEST(VitalCheck, MultipleViolations) {
    std::map<VitalType, float> readings = {
        {VitalType::Temperature, 94},
        {VitalType::PulseRate, 55},
        {VitalType::Spo2, 85}
    };
    std::map<VitalType, VitalRange> limits = {
        {VitalType::Temperature, {95, 102}},
        {VitalType::PulseRate, {60, 100}},
        {VitalType::Spo2, {90, 100}}
    };

    auto issues = checkVitals(readings, limits);
    ASSERT_EQ(issues.size(), 3);
}

// ✅ OO Tests
TEST(PatientAdjustments, ChildProfile) {
    VitalLimits limits;
    limits.setLimit(VitalType::PulseRate, 60, 100);
    limits.setLimit(VitalType::Temperature, 95, 102);

    Patient child(8);
    child.tuneLimits(limits);

    EXPECT_EQ(limits.getAll().at(VitalType::PulseRate).minValue, 70);
    EXPECT_EQ(limits.getAll().at(VitalType::Temperature).maxValue, 100);
}

TEST(PatientAdjustments, SeniorProfile) {
    VitalLimits limits;
    limits.setLimit(VitalType::PulseRate, 60, 100);

    Patient senior(70);
    senior.tuneLimits(limits);

    EXPECT_EQ(limits.getAll().at(VitalType::PulseRate).maxValue, 90);
}
