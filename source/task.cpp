// this file you need to fill
// этот файл вам нужно заполнить
#define _USE_MATH_DEFINES

#include "task.h"

#include <algorithm>
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
	std::vector<int> IDs;
	{
		int i = 0;
		int n = input_units.size();
		for (; i < n - 4; i += 5) {
			IDs.push_back(i);
			IDs.push_back(i + 1);
			IDs.push_back(i + 2);
			IDs.push_back(i + 3);
			IDs.push_back(i + 4);
		}
		for (; i < n; ++i) {
			IDs.push_back(i);
		}
	}

	std::function<void(const std::vector<unit>& input, std::vector<int>::iterator begin,
		std::vector<int>::iterator end)> quick_sort; // Fast multithreaded sorting.

	quick_sort = [&quick_sort](const std::vector<unit>& input, std::vector<int>::iterator IDs_begin,
		std::vector<int>::iterator IDs_end)->void {

			auto const sz = IDs_end - IDs_begin;
			if (sz <= 1) return;

			auto pivot = IDs_begin + sz / 2;
			auto const pivot_v = *pivot;

			std::swap(*pivot, *(IDs_end - 1));

			auto p = std::partition(IDs_begin, IDs_end, [&](const int a) {
				return input.at(a).position.x < input.at(pivot_v).position.x ||
					(input.at(a).position.x == input.at(pivot_v).position.x &&
						input.at(a).position.y < input.at(pivot_v).position.y);
				});

			std::swap(*p, *(IDs_end - 1));

			if (sz > 512) {
				std::thread thr([&]() {
					return quick_sort(input, IDs_begin, p);
					});
				quick_sort(input, p + 1, IDs_end);
				thr.join();
			}
			else {
				quick_sort(input, IDs_begin, p);
				quick_sort(input, p + 1, IDs_end);
			}
	};

	// Filling in the vector "output" using data from the vector "data" for the element "id".
	auto check_the_visibility_fields = [&](const std::vector<unit>& data, const std::vector<int>& input_ID, std::vector<int>& output, const int id)
	{
		const unit dir_id(data.at(input_ID.at(id)));

		for (int i = id - 1; i >= 0; --i) {

			if (dir_id.position.x - dir_id.distance > data.at(input_ID.at(i)).position.x) break;
			
			float difference_x = data.at(input_ID.at(i)).position.x - dir_id.position.x;
			float difference_y = data.at(input_ID.at(i)).position.y - dir_id.position.y;

			float dist = powf(difference_x, 2.0) + powf(difference_y, 2.0);

			if (dist > powf(dir_id.distance, 2.0)) continue;

			dist = sqrtf(dist);

			float product = dir_id.direction.x * (difference_x) / dist +
				dir_id.direction.y * (difference_y) / dist;

			if (product < 0) continue;

			float angle_res = acos(product) * 180.0 / M_PI;

			if (angle_res > dir_id.fov_deg / 2.0) continue;

			++output.at(input_ID.at(id));
		}

		for (int i = id + 1; i < data.size(); ++i) {

			if (dir_id.position.x + dir_id.distance < data.at(input_ID.at(i)).position.x) break;

			float difference_x = data.at(input_ID.at(i)).position.x - dir_id.position.x;
			float difference_y = data.at(input_ID.at(i)).position.y - dir_id.position.y;

			float dist = powf(difference_x, 2.0) + powf(difference_y, 2.0);

			if (dist > powf(dir_id.distance, 2.0)) continue;

			dist = sqrtf(dist);

			float product = dir_id.direction.x * (difference_x) / dist +
				dir_id.direction.y * (difference_y) / dist;

			if (product < 0) continue;

			float angle_res = acos(product) * 180.0 / M_PI;

			if (angle_res > dir_id.fov_deg / 2.0) continue;

			++output.at(input_ID.at(id));
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
				check_the_visibility_fields(input_units, IDs, result, i);
			}
		}
	};

	quick_sort(input_units, IDs.begin(), IDs.end());
	running_parallel_computing(0, input_units.size() - 1, 2);
}