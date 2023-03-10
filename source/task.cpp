#define _USE_MATH_DEFINES

#include "task.h"

#pragma GCC target("avx2")
#pragma GCC optimize("O3")

#include <algorithm>
#include <functional>
#include <thread>
#include <cmath>

using namespace std;

void Task::checkVisible(const std::vector<unit>& input_units, std::vector<int>& result)
{

	if (input_units.empty()) {
		return;
	}

	if (input_units.size() == 1) {
		result.push_back(0);
		return;
	}

	result = std::vector<int>(input_units.size());

	//The array in which the ids of the elements of the sorted array will be stored.
	std::vector<int> IDs(input_units.size());
	#pragma loop(hint_parallel(4))
	for (int i = 0; i < input_units.size(); ++i) {
		IDs.at(i) = i;
	}

	auto check_the_visibility_fields = [&](const int& id)
	{
		const unit dir_id(input_units.at(IDs.at(id)));
		const float angle(powf(cos(dir_id.fov_deg * 0.008726646f), 2.0));
		const float distance2 = powf(dir_id.distance, 2.0);

		int count = 0;
		// Processing of array data located to the left of the element with the number "id".
		for (int i = id - 1; i >= 0; --i) {

			float difference_x = input_units.at(IDs.at(i)).position.x - dir_id.position.x;

			if (-dir_id.distance > difference_x) break;

			float difference_y = input_units.at(IDs.at(i)).position.y - dir_id.position.y;

			float norm = dir_id.direction.x * difference_x + dir_id.direction.y * difference_y;
			if (norm < 0) continue;

			float dist = difference_x * difference_x + difference_y * difference_y;
			if (dist > distance2) continue;
			if (powf(norm, 2.0) / dist < angle) continue;

			++count;
		}

		//Processing of array data located to the right of the element with the number "id".
		for (int i = id + 1; i < input_units.size(); ++i) {

			float difference_x = input_units.at(IDs.at(i)).position.x - dir_id.position.x;

			if (dir_id.distance < difference_x) break;

			float difference_y = input_units.at(IDs.at(i)).position.y - dir_id.position.y;
			
			float norm = dir_id.direction.x * difference_x + dir_id.direction.y * difference_y;
			if (norm < 0) continue;

			float dist = difference_x * difference_x + difference_y * difference_y;
			if (dist > distance2) continue;

			if (powf(norm, 2.0) / dist < angle) continue;

			++count;
		}
		result.at(IDs.at(id)) = count;
	};

	std::function<void(const int& start, const int& end, const int& recursion_depth)> running_parallel_computing;
	// Runs parallel computations using recursion
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
				check_the_visibility_fields(i);
			}
		}
	}; 

	if (IDs.size() >= 4) {
		std::thread thr_1([&]() {
			std::sort(IDs.begin(), IDs.begin() + (IDs.size() / 4), [&](const int& left, const int& right) {
				return input_units.at(left).position.x < input_units.at(right).position.x;
				});
			});
		std::thread thr_2([&]() {
			std::sort(IDs.begin() + (IDs.size() / 4 + 1), IDs.begin() + (IDs.size() / 2), [&](const int& left, const int& right) {
				return input_units.at(left).position.x < input_units.at(right).position.x;
				});
			});
		std::thread thr_3([&]() {
			std::sort(IDs.begin() + (IDs.size() / 2 + 1), IDs.begin() + (IDs.size() / 2 + IDs.size() / 4), [&](const int& left, const int& right) {
				return input_units.at(left).position.x < input_units.at(right).position.x;
				});
			});
		std::sort(IDs.begin() + (IDs.size() / 2 + IDs.size() / 4 + 1), IDs.end(), [&](const int& left, const int& right) {
			return input_units.at(left).position.x < input_units.at(right).position.x;
			});
		thr_1.join();
		thr_2.join();
		thr_3.join();
		
		std::sort(IDs.begin(), IDs.end(), [&](const int& left, const int& right) {
			return input_units.at(left).position.x < input_units.at(right).position.x;
			});
	}
	else {
		std::sort(IDs.begin(), IDs.end(), [&](const int& left, const int& right) {
			return input_units.at(left).position.x < input_units.at(right).position.x;
			});
	}
	running_parallel_computing(0, input_units.size() - 1, 2);
}