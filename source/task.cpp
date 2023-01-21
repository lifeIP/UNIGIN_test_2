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
				return (input.at(a).position.x + input.at(a).position.y) <
					(input.at(pivot_v).position.x + input.at(pivot_v).position.y);
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

	auto check_the_visibility_fields = [&](const std::vector<unit>& data, const std::vector<int>& input_ID, std::vector<int>& output, const int id) {

		auto distance = [](const unit& A, const vec2& B) {
			float dist = sqrtf(powf(A.position.x - B.x, 2.0) + powf(A.position.y - float(B.y), 2.0));
			return dist;
		};

		auto scalar_product = [](const vec2& norm_vec_A, const vec2& norm_vec_B) {
			float scalar = norm_vec_A.x * norm_vec_B.x + norm_vec_A.y * norm_vec_B.y;
			return scalar;
		};


		for (int i = id - 1; i >= 0; --i) {
					
			float dist = distance(data.at(input_ID.at(id)), data.at(input_ID.at(i)).position);
			
			if (dist > data.at(input_ID.at(id)).distance) continue;

			vec2 norm;

			norm.x = (data.at(input_ID.at(i)).position.x - data.at(input_ID.at(id)).position.x) / dist;
			norm.y = (data.at(input_ID.at(i)).position.y - data.at(input_ID.at(id)).position.y) / dist;
			
			float product = scalar_product(data.at(input_ID.at(id)).direction, norm);
			
			if (product < 0) continue;

			float angle_res = (acosf(product) * 180.0) / M_PI;

			if (angle_res > data.at(input_ID.at(id)).fov_deg / 2.0) continue;

			++output.at(input_ID.at(id));
		}

		for (int i = id + 1; i < data.size(); ++i) {

			float dist = distance(data.at(input_ID.at(id)), data.at(input_ID.at(i)).position);

			if (dist > data.at(input_ID.at(id)).distance) continue;

			vec2 norm;

			norm.x = (data.at(input_ID.at(i)).position.x - data.at(input_ID.at(id)).position.x) / dist;
			norm.y = (data.at(input_ID.at(i)).position.y - data.at(input_ID.at(id)).position.y) / dist;

			float product = scalar_product(data.at(input_ID.at(id)).direction, norm);

			if (product < 0) continue;

			float angle_res = (acosf(product) * 180.0) / M_PI;

			if (angle_res > data.at(input_ID.at(id)).fov_deg / 2.0) continue;

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

	std::thread thr([&]()
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
	);

	quick_sort(input_units, IDs.begin(), IDs.end());
	thr.join();
	running_parallel_computing(0, input_units.size() - 1, 6);
}