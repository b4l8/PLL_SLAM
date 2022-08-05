#include "particle.h"
#include "motion.h"


/**
 * Predict on every particle :
 * 1. generate random motion based on specific model
 * 2. particle + motion
 * @param  motion : input motion
 * @param  particles : input/output particles
 */
__global__ void predict(Motion* motion, Particle* particles){
	int idx = threadIdx.x;
	auto& p = particles[idx]; // particle of this thread
	auto m = motion();        // random motion
	p << m;                   // particle + motion
}

/**
 * 应该是二维的。对于每个particle， 每圈	Laser的每个点计算命中
 * PART_NUM x LASER_NUM
 * 拥有part_num个local计数用来统计每圈的weight结果
 * @param particles
 * @param map
 */
__global__ void match(Map* map, Laser* laser,Particle* particles){
	int idx = blockDim.x * blockIdx.x + threadIdx.x;
	__shared__ int cnt[PART_NUM];
	__shared__ int hit[PART_NUM];
	auto& p = particles[blockIdx.x]; // particle of this thread
	auto& hit = laser[threadIdx.x]; // laser hit of this thread
	auto point = convert(p,hit, SCALE);
	// 这里需要检查是否重复同一个格子，会用到相当大的一块内存
	int new_pt = check_repeat(); // 重复的标记为0
	cnt[blockIdx.x]+=new_pt;
	hit[blockIdx.x]+= (new_pt && map->check(point)); // 短路其实没啥意义，会以所有线程最慢的为准。
	__syncthreads();

	particles.set_weight(static_cast<float>(hit[blockIdx.x]) / static_cast<float>(cnt[blockIdx.x]));
}

/**
 * @param particles
 * @param map
 */
Particle correct(Map* map, Laser* laser,Particle* particles){
	match<<<PART_NUM,LASER_NUM>>>(map,laser,particles);
	auto best = max(particles);

	if(need_resample(particles) && best.weight() > 0.6){
		resample(particles);
	}

	// search

	return best;


}

void draw(Map* map, Laser* laser, Pose& pose){
	auto area = check_area(pose, obstacles, view);

	if (area.rect.empty()) return {};

	draw_area(area, map);
}