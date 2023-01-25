// this file you need to fill
// этот файл вам нужно заполнить


#define _USE_MATH_DEFINES

#include "task.h"


#if defined(_MSC_VER)
	 /* Microsoft C/C++-compatible compiler */
#include <intrin.h>
#elif defined(__GNUC__) && (defined(__x86_64__) || defined(__i386__))
	 /* GCC-compatible compiler, targeting x86/x86-64 */
#include <x86intrin.h>
#endif

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
	for (int i = 0; i < input_units.size(); ++i) {
		result.at(0) = 0;
	}

	//The array in which the ids of the elements of the sorted array will be stored.
	std::vector<int> IDs(input_units.size());
	for (int i = 0; i < input_units.size(); ++i) {
		IDs.at(i) = i;
	}

	// Filling in the vector "output" using data from the vector "data" for the element "id".
	auto check_the_visibility_fields = [&](const std::vector<unit>& data, const std::vector<int>& input_ID, std::vector<int>& output, const int& id)
	{
		const unit dir_id(data.at(input_ID.at(id)));
		const float angle(cos(dir_id.fov_deg * 0.00872664625972f));
		const float distance2 = powf(dir_id.distance, 2.0);

		const float pos_plus_dist_x = dir_id.position.x + dir_id.distance;
		const float pos_min_dist_x = dir_id.position.x - dir_id.distance;

		int count = 0;
		// Processing of array data located to the left of the element with the number "id".

		for (int i = id - 1; i >= 0; --i) {

			if (pos_min_dist_x > data.at(input_ID.at(i)).position.x) break;

			float difference_x = data.at(input_ID.at(i)).position.x - dir_id.position.x;
			float difference_y = data.at(input_ID.at(i)).position.y - dir_id.position.y;

			float dist = difference_x * difference_x + difference_y * difference_y;

			if (dist > distance2 || (dir_id.direction.x * difference_x +
				dir_id.direction.y * difference_y) / sqrtf(dist) < angle) continue;

			++count;
		}

		//Processing of array data located to the right of the element with the number "id".
		for (int i = id + 1; i < data.size(); ++i) {

			if (pos_plus_dist_x < data.at(input_ID.at(i)).position.x) break;

			float difference_x = data.at(input_ID.at(i)).position.x - dir_id.position.x;
			float difference_y = data.at(input_ID.at(i)).position.y - dir_id.position.y;

			float dist = difference_x * difference_x + difference_y * difference_y;

			if (dist > distance2 || (dir_id.direction.x * difference_x +
				dir_id.direction.y * difference_y) / sqrtf(dist) < angle) continue;

			++count;
		}
		output.at(input_ID.at(id)) = count;
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
				check_the_visibility_fields(input_units, IDs, result, i);
			}
		}
	};

	std::sort(IDs.begin(), IDs.end(), [&](const int& left, const int& right) {
		return input_units.at(left).position.x < input_units.at(right).position.x;
		});
	running_parallel_computing(0, input_units.size() - 1, 2);
}