#pragma once

#include "pb_map.h"

//typedef std::pair<int, int> int_pair;

class int_pair_comparator {
private:
	typedef std::pair<int, int> int_pair;

public:
	bool operator() (const int_pair& ip1, const int_pair& ip2);
};

class dijkstra
{
private:
	typedef std::pair<int, int> int_pair;

public:
	static void run_path_finding(tile_map& map);
};

