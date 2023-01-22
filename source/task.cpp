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

	/*{
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
	}*/
	
	for (int i = 0; i < input_units.size(); ++i) {
		result.push_back(0);
	}


	
	auto check_the_visibility_fields = [&](const std::vector<unit>& data, std::vector<int>& output, const int id) {

			auto distance = [](const unit& A, const vec2& B) {
				float dist = sqrtf(powf(A.position.x - B.x, 2.0) + powf(A.position.y - B.y, 2.0));
				return dist;
			};

			for (int i = id - 1; i >= 0; --i) {
				if (data.at(id).position.x + data.at(id).distance < data.at(id).position.x ||
					data.at(id).position.x - data.at(id).distance > data.at(id).position.x ||
					data.at(id).position.y + data.at(id).distance < data.at(id).position.y ||
					data.at(id).position.y - data.at(id).distance > data.at(id).position.y
					) continue;

				float dist = distance(data.at(id), data.at(i).position);

				if (dist > data.at(id).distance) continue;

				float product = data.at(id).direction.x * (data.at(i).position.x - data.at(id).position.x) / dist + 
					data.at(id).direction.y * (data.at(i).position.y - data.at(id).position.y) / dist;
		
				if (product < 0) continue;

				float angle_res = (acosf(product) * 180.0) / M_PI;

				if (angle_res > data.at(id).fov_deg / 2.0) continue;

				++output.at(id);
			}

			for (int i = id + 1; i < data.size(); ++i) {

				if (data.at(id).position.x + data.at(id).distance < data.at(id).position.x ||
					data.at(id).position.x - data.at(id).distance > data.at(id).position.x ||
					data.at(id).position.y + data.at(id).distance < data.at(id).position.y ||
					data.at(id).position.y - data.at(id).distance > data.at(id).position.y
					) continue;

				float dist = distance(data.at(id), data.at(i).position);

				if (dist > data.at(id).distance) continue;

				float product = data.at(id).direction.x * (data.at(i).position.x - data.at(id).position.x) / dist +
					data.at(id).direction.y * (data.at(i).position.y - data.at(id).position.y) / dist;

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

	running_parallel_computing(0, input_units.size() - 1, 8);
}