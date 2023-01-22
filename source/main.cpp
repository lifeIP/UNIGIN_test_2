// THIS FILE IS STUB FOR INPUT AND OUTPUT DATA
// DO NOT MODIFICATE IT - IT`S PART OF ANOTHER PROJECT

// это файл-заглушка, для демонстрации как метод будет вызван.
// не модифицируйте его - возможно это часть другого проекта

#include <iostream>
#include <fstream>

#include "task.h"

int main()
{
	std::vector<unit> units;
	std::vector<int> result;

	int count = 0;

	// input
	std::ifstream in("input.txt");
	if (in.is_open())
	{
		in >> count;
		unit t;
		for (int i = 0; i < count; i++)
		{
			in >> t.position.x >> t.position.y;
			in >> t.direction.x >> t.direction.y;
			in >> t.fov_deg >> t.distance;
			units.push_back(t);
		}
		in.close();
	}

	// invoke
	Task::checkVisible(units, result);

	// output
	std::ofstream out;
	out.open("output.txt");
	if (out.is_open())
	{
		for (int i = 0; i < count; i++) {
			//std::cout << units.at(result.at(i)).position.x << "\t" << units.at(result.at(i)).position.y << std::endl;
			std::cout << result.at(i) << std::endl;
			out << result.at(i) << std::endl;
		}
		out.close();
	}
	return 0;
}
