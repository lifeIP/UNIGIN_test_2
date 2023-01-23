// this file you need to fill
// этот файл вам нужно заполнить
#define _USE_MATH_DEFINES

#include "task.h"

#include <functional>
#include <thread>
#include <cmath>


void Task::checkVisible(const std::vector<unit>& input_units, std::vector<int>& result)
{

	if (input_units.empty()) {
		return;
	}

	if (input_units.size() == 1) {
		result.push_back(0);
		return;
	}
	
	for (int i = 0; i < input_units.size(); ++i) {
		result.push_back(0);
	}
	
	// Filling in the vector "output" using data from the vector "data" for the element "id".
	auto check_the_visibility_fields = [&](const std::vector<unit>& data, std::vector<int>& output, const int& id)
	{
		const unit dir_id(data.at(id));

		for (int i = id - 1; i >= 0; --i) {
			
			float difference_x = data.at(i).position.x - dir_id.position.x;
			float difference_y = data.at(i).position.y - dir_id.position.y;

			float dist = powf(difference_x, 2.0) + powf(difference_y, 2.0);
			
			if (dist > powf(data.at(id).distance, 2.0)) continue;

			dist = sqrtf(dist);

			float product = dir_id.direction.x * (difference_x) / dist +
				dir_id.direction.y * (difference_y) / dist;

			if (product < 0) continue;

			float angle_res = acos(product) * 180.0 / M_PI;

			if (angle_res > dir_id.fov_deg / 2.0) continue;

			++output.at(id);
		}

		for (int i = id + 1; i < data.size(); ++i) {

			float difference_x = data.at(i).position.x - dir_id.position.x;
			float difference_y = data.at(i).position.y - dir_id.position.y;

			float dist = powf(difference_x, 2.0) + powf(difference_y, 2.0);

			if (dist > powf(data.at(id).distance, 2.0)) continue;

			dist = sqrtf(dist);

			float product = dir_id.direction.x * (difference_x) / dist +
				dir_id.direction.y * (difference_y) / dist;

			if (product < 0) continue;

			float angle_res = acos(product) * 180.0 / M_PI;

			if (angle_res > dir_id.fov_deg / 2.0) continue;

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

	running_parallel_computing(0, input_units.size() - 1, 2);
}