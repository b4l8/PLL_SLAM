#pragma once
#include <cuda_runtime.h>
#include "type.h"

namespace pll_slam{

	class Particle{
	public:
		Particle() noexcept;

		Particle(Pose2DFloat const& pose, float weight) noexcept;


	private:
		Pose2DFloat pose_;
		float       weight_;

	};
}