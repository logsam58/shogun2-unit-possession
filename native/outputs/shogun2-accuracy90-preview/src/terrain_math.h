#pragma once
#include <algorithm>
#include <cmath>
// Empire.Retail RVA3aa605..3aa651: grade is rise/run, not angle in degrees.
// Constants are read from the verified engine; these are not tuning settings.
inline float native_grade_factor(float grade,float uphill_weight,float downhill_cap){
    return grade>0 ? 1.0f/(1.0f+grade*uphill_weight) :
        (grade<0 ? std::min(1.0f-grade,downhill_cap) : 1.0f);
}
inline float native_grade_probe(float previous_speed,float acceleration,float dt,float radius,float minimum){
    return std::clamp((previous_speed+acceleration)*dt,minimum,radius);
}
