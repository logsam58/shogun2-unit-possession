#pragma once
#include "input174.h"
#include <algorithm>
#include <cmath>

namespace deck216 {
struct FineIntent {
    deck174::Vec2 move{};
    float turn{};
    bool active{}, began{}, released{}, turning{}, cancel{};
};

class FinePad {
    // Both gestures are latched at press so crossing the zone cannot change the order.
    static constexpr float center_radius = .75f;
    static constexpr float drag_deadband = .07f;
    static constexpr float degrees_per_axis = 150.f;
    bool held_{}, turning_{};
    deck174::Vec2 direction_{};
    float origin_x_{}, angle_{};

    static deck174::Vec2 edge_direction(deck174::Vec2 pad) {
        // The boundary sits halfway between each cardinal and diagonal.
        constexpr float tan_half_sector = .41421356f; // tan(22.5 degrees)
        constexpr float diagonal = .70710678f;
        const float x = std::fabs(pad.x), y = std::fabs(pad.y);
        if(y <= x * tan_half_sector) return {std::copysign(1.f,pad.x),0.f};
        if(x <= y * tan_half_sector) return {0.f,std::copysign(1.f,pad.y)};
        return {std::copysign(diagonal,pad.x),std::copysign(diagonal,pad.y)};
    }

public:
    void reset() { *this = {}; }

    FineIntent sample(const deck174::Frame& frame, uint64_t) {
        FineIntent out;
        const bool clicked = (frame.buttons & deck174::bit(deck174::Button::LeftPadClick)) != 0;
        if (!deck174::unit(frame.left_pad)) {
            out.cancel = held_;
            reset();
            return out;
        }

        if (clicked && !held_) {
            held_ = out.began = true;
            const auto& pad = frame.left_pad;
            const float radius = std::hypot(pad.x, pad.y);
            turning_ = radius <= center_radius;
            origin_x_ = pad.x;
            if (!turning_) direction_ = edge_direction(pad);
        }
        if (!held_) return out;

        if (clicked && turning_) {
            const float dx = frame.left_pad.x - origin_x_;
            const float degrees = std::max(0.f, std::fabs(dx) - drag_deadband) * degrees_per_axis;
            angle_ = std::copysign(std::min(180.f, degrees), dx);
        }
        out.active = clicked;
        out.released = !clicked;
        out.turning = turning_;
        out.move = direction_;
        out.turn = angle_;
        if (!clicked) reset();
        return out;
    }
};
}
