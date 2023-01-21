// this file you need to fill
// этот файл вам нужно заполнить
#define _USE_MATH_DEFINES

#include "task.h"

#include <functional>

#include <thread>
#include <mutex>

#include <cmath>
#include <algorithm>


void Task::checkVisible(const std::vector<unit>& input_units, std::vector<int>& result)
{

	if (input_units.empty()) {
		return;
	}

	if (input_units.size() == 1) {
		result.push_back(0);
		return;
	}

	{
		int i = 0;
		int n = input_units.size();
		for (; i < n - 4; i += 5) {
			result.push_back(0);
			result.push_back(0);
			result.push_back(0);
			result.push_back(0);
			result.push_back(0);
		}
		for (; i < n; ++i) {
			result.push_back(0);
		}
	}
	
	auto check_the_visibility_fields = [&](const std::vector<unit>& data, std::vector<int>& output, const int id) {

		auto distance = [](const unit& A, const vec2& B) {
			float dist = sqrtf(powf(A.position.x - B.x, 2.0) + powf(A.position.y - float(B.y), 2.0));
			return dist;
		};

		auto scalar_product = [](const vec2& norm_vec_A, const vec2& norm_vec_B) {
			float scalar = norm_vec_A.x * norm_vec_B.x + norm_vec_A.y * norm_vec_B.y;
			return scalar;
		};

		vec2 reference_point;
		reference_point.x = -100000;
		reference_point.y = -100000;

		int counter(0);

		for (int i = id - 1; i >= 0; --i) {
			
			float dist = distance(data.at(id), data.at(i).position);

			if (dist > data.at(id).distance) continue; 

			vec2 norm;

			norm.x = (data.at(i).position.x - data.at(id).position.x) / dist;
			norm.y = (data.at(i).position.y - data.at(id).position.y) / dist;
			
			float product = scalar_product(data.at(id).direction, norm);
			
			if (product < 0) continue;

			float angle_res = (acosf(product) * 180.0) / M_PI;

			if (angle_res > data.at(id).fov_deg / 2.0) continue;

			++output.at(id);
		}

		counter = 0;

		for (int i = id + 1; i < data.size(); ++i) {
			
			float dist = distance(data.at(id), data.at(i).position);

			if (dist > data.at(id).distance) continue;

			vec2 norm;

			norm.x = (data.at(i).position.x - data.at(id).position.x) / dist;
			norm.y = (data.at(i).position.y - data.at(id).position.y) / dist;

			float product = scalar_product(data.at(id).direction, norm);

			if (product < 0) continue;

			float angle_res = (acosf(product) * 180.0) / M_PI;

			if (angle_res > data.at(id).fov_deg / 2.0) continue;

			++output.at(id);
		}

	};

	std::function<void(const int start, const int end, const int recursion_depth)> running_parallel_computing;

	running_parallel_computing = [&](const int start, const int end, const int recursion_depth) {
		if (end - start >= 4 && recursion_depth >= 2) {
			std::thread thr([&]() {running_parallel_computing(start, end / 2, recursion_depth - 2);
			return; });
			running_parallel_computing(end / 2 + 1, end, recursion_depth - 2);
			thr.join();
		}
		else
		{
			for (int i = start; i <= end; ++i) {
				check_the_visibility_fields(input_units, result, i);
			}
		}
	};
	
	running_parallel_computing(0, input_units.size() - 1, 6);
}