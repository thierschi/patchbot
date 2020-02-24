#include "pb_path_finding.h"

#include <queue>

/*
	Class int_pair_comparator
*/

bool int_pair_comparator::operator()(const int_pair& ip1, const int_pair& ip2)
{
	return ip1.first < ip2.first;
}




/*
	class dijkstra
*/

void dijkstra::run_path_finding(tile_map& map)
{
	/* Priotity queue  with custom comparator implemented like a min_heap
	Adjacent nodes get pushed here */
	std::priority_queue<int_pair, std::vector<int_pair>, int_pair_comparator> pq;

	/* Vector of distences (local, beacaus we dont need these values later) */
	std::vector<int> dist(map.get_size(), INT_MAX);

	/* Initialize source (patchbot)
	One may notice that I calculate with the values in the vector not with
	(x,y) values */
	coords pb_pos = map.robots.get_patchbots_location();
	pq.push(std::make_pair(0, pb_pos.y * map.get_width() + pb_pos.x));

	while (!pq.empty()) {
		/* Get node/vertex with smallest dist and remove from "heap" */
		int v_dist = pq.top().first;
		int u = pq.top().second;
		pq.pop();

		int y = floor(u / map.get_width());
		int x = u - (y * map.get_width());


		/* Normally one would iterate through all adjacent edges, but this
		is a 2d map and we have 4 edges at max. Hence we go through them
		one by one, so that we dont need to save references to the adjacend
		nodes in each tile, but just calculate the coordinates.
			--> If an edge has value 0, that means it does not exist, e.g. edge
			to a wall */

		/* Skip checking the adjacent edges if a smaller distance already exists */
		if (v_dist > dist[u]) continue;

		/* NORTH */
		if (y > 0) {
			if (map.get_tile(x, y - 1)->get_weight() != -1
				&& map.get_tile(x, y)->predecessor != direction::NORTH && y > 0) {
				if (dist[u - map.get_width()] > dist[u] + map.get_tile(x, y - 1)->get_weight()) {
					dist[u - map.get_width()] = dist[u] + map.get_tile(x, y - 1)->get_weight();
					pq.push(std::make_pair(dist[u - map.get_width()], u - map.get_width()));
					map.get_tile(x, y - 1)->predecessor = direction::SOUTH;
				}
			}
		}

		/* EAST */
		if (x < map.get_width() - 1) {
			if (map.get_tile(x + 1, y)->get_weight() != -1
				&& map.get_tile(x, y)->predecessor != direction::EAST
				&& x < map.get_width() - 1) {
				if (dist[u + 1] > dist[u] + map.get_tile(x + 1, y)->get_weight()) {
					dist[u + 1] = dist[u] + map.get_tile(x + 1, y)->get_weight();
					pq.push(std::make_pair(dist[u + 1], u + 1));
					map.get_tile(x + 1, y)->predecessor = direction::WEST;
				}
			}
		}

		/* SOUTH */
		if (y < map.get_height() - 1) {
			if (map.get_tile(x, y + 1)->get_weight() != -1
				&& map.get_tile(x, y)->predecessor != direction::SOUTH
				&& y < map.get_height() - 1) {
				if (dist[u + map.get_width()] > dist[u] + map.get_tile(x, y + 1)->get_weight()) {
					dist[u + map.get_width()] = dist[u] + map.get_tile(x, y + 1)->get_weight();
					pq.push(std::make_pair(dist[u + map.get_width()], u + map.get_width()));
					map.get_tile(x, y + 1)->predecessor = direction::NORTH;
				}
			}
		}

		/* WEST */
		if (x > 0) {
			if (map.get_tile(x - 1, y)->get_weight() != -1
				&& map.get_tile(x, y)->predecessor != direction::WEST && x > 0) {
				if (dist[u - 1] > dist[u] + map.get_tile(x - 1, y)->get_weight()) {
					dist[u - 1] = dist[u] + map.get_tile(x - 1, y)->get_weight();
					pq.push(std::make_pair(dist[u - 1], u - 1));
					map.get_tile(x - 1, y)->predecessor = direction::EAST;
				}
			}
		}

	}
}
