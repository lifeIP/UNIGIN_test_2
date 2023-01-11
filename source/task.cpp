// this file you need to fill
// этот файл вам нужно заполнить
#include "task.h"

#include <functional>
#include <future>
#include <iostream>
#include <algorithm>

void Task::checkVisible(const std::vector<unit> &input_units, std::vector<int> &result)
{
	if (input_units.empty()) {
		return;
	}

	std::vector<int> IDs;
	IDs.reserve(input_units.size());
	result.reserve(input_units.size());
	
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
	auto fillResult = std::async(std::launch::async, [&]() {
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
	});


	// Fast multithreaded sorting.
	std::function<void(const std::vector<unit>& input, std::vector<int>::iterator begin,
		std::vector<int>::iterator end)> quick_sort;

	quick_sort = [&quick_sort](const std::vector<unit>&input, std::vector<int>::iterator IDs_begin, 
		std::vector<int>::iterator IDs_end)->void {
			
			auto const sz = IDs_end - IDs_begin;
			if (sz <= 1) return;

			auto pivot = IDs_begin + sz / 2;
			auto const pivot_v = *pivot;

			std::swap(*pivot, *(IDs_end - 1));
			
			auto p = std::partition(IDs_begin, IDs_end, [&](const int a) {
				return sqrtf(powf(input.at(a).position.x, 2) + powf(input.at(a).position.y, 2)) <
					sqrtf(powf(input.at(pivot_v).position.x, 2) + powf(input.at(pivot_v).position.y, 2));
			});

			std::swap(*p, *(IDs_end - 1));

			if (sz > 10000) {
				auto left = std::async(std::launch::async, [&]() {
					return quick_sort(input, IDs_begin, p);
					});
				quick_sort(input, p + 1, IDs_end);
			}
			else {
				quick_sort(input, IDs_begin, p);
				quick_sort(input, p + 1, IDs_end);
			}
	};
	
	std::function<bool(const std::vector<unit>& data, const std::vector<int>& input_ID, 
		std::vector<int>& output, const int id)> check_the_visibility_fields;

	check_the_visibility_fields = [](const std::vector<unit>& data,
		const std::vector<int>& input, std::vector<int>& output, const int id)->bool {

			// Creating and normalizing a vector by 2 points.
			auto normilize = [](const vec2& point_1, const vec2& point_2)->const vec2& {
				vec2 res;

				res.x = point_2.x - point_1.x;
				res.y = point_2.y - point_1.y;
	
				float tmp = sqrtf(powf(res.x, 2) + powf(res.y, 2));

				res.x = res.x / tmp;
				res.y = res.y / tmp;

				return res;
			};

			// The angle between two normalized vectors.
			auto angle = [](const vec2& norm_vect_1, const vec2& norm_vect_2)->const float& {
				return acosf(norm_vect_1.x * norm_vect_2.x + norm_vect_1.y * norm_vect_2.y);
			};

			// The scalar product of two vectors.
			auto scalar = [](const vec2& norm_vect_1, const vec2& norm_vect_2)->const float& {
				return norm_vect_1.x * norm_vect_2.x + norm_vect_1.y * norm_vect_2.y;
			};
			
			// The distance between the points.
			auto distance = [](const vec2& point_1, const vec2& point_2)->const float& {
				return sqrtf(powf(point_1.x - point_2.x, 2) + powf(point_1.y - point_2.y, 2));
			};

			vec2 tested;
			tested.x = 1;
			tested.y = 1;
			
			for (int i = id - 1; i > 0; --i) {// left
				if (distance(data.at(input.at(id)).position, 
					data.at(input.at(i)).position) > data.at(id).distance) break;
				if ()
				++output.at(id);
			}
			for (int i = id + 1; i < data.size() - 1; ++i) {// right
				if (distance(data.at(input.at(id)).position,
					data.at(input.at(i)).position) > data.at(id).distance) break;

			}
			
			
			
	};

	quick_sort(input_units, IDs.begin(), IDs.end());


	result = IDs;
	
}
