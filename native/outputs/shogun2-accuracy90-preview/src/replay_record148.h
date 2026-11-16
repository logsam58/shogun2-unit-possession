#pragma once
#include "exact_replay_carrier147.h"

struct ReplaySquad148 {
    uint32_t key{},proof{};
};
static ReplaySquad148 capture_squad148(uintptr_t native_unit,uintptr_t env,uintptr_t squad) noexcept;
static uint32_t membership_proof148(ReplaySquad148 before,ReplaySquad148 target) noexcept;
static void record_formation148(uintptr_t actor,exactreplay147::Kind kind,ReplaySquad148 target={},
                                uint32_t source=0,uint16_t argument=0) noexcept;
static uint16_t replay_clock_unit148(uintptr_t native_unit) noexcept;
static void replay_unit_scope148(uintptr_t native_unit,bool after,uint16_t occurrence) noexcept;
static void replay_apply_unit148(uintptr_t native_unit,bool after,uint16_t occurrence) noexcept;
static bool replay_detached148(uintptr_t actor,uintptr_t* native_unit=nullptr,uintptr_t* env=nullptr) noexcept;

