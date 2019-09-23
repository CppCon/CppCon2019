#pragma once

#include "Norm3.h"
#include "OrthoNormalBasis.h"

[[nodiscard]] Norm3 coneSample(const Norm3 &direction, double coneTheta,
                               double u, double v) noexcept;

[[nodiscard]] Norm3 hemisphereSample(const OrthoNormalBasis &basis, double u,
                                     double v) noexcept;
