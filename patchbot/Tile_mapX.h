#pragma once
#include <iostream>
#include <vector>
#include "Tile.h"
#define SIZE height*width

using namespace std;

class Tile_mapX {
protected:
	int height;
	int width;
	vector<Tile> i_map;

public:
	Tile_map(int w = 1, int h = 1) {
		height = h;
		width = w;

		i_map.insert(i_map.begin(), SIZE, Tile());
	}

	void printMap() {
		for (int i = 0; i < SIZE; i++) {
			cout << (int)i_map[i].get_terrain();
			if ((i + 1) % width == 0)
				cout << "\n";
		}
	}

	// Getter
	int getSize() {
		return SIZE;
	}

	int getHeight() {
		return height;
	}

	int getWidth() {
		return width;
	}

	Tile getTile(int x, int y) {
		if (x >= width || y >= height) {
			throw invalid_argument("Coordinates out of range.");
		}
		return i_map[y * width + x];
	}

	//Setter
	void setHeight(int h) {
		if (h <= 0) {
			throw invalid_argument("New height of Tile_map must be 1 or greater.");
		}
		else if (h < height) {
			i_map.erase(i_map.begin() + h * width, i_map.end());
			height = h;
		}
		else if (h > height) {			
			i_map.insert(i_map.end(), (h - height) * width, Tile());
			height = h;
		}
	}

	void setWidth(int w) {
		if (w <= 0) {
			throw invalid_argument("New width of Tile_map must be 1 or greater");
		}
		else if (w < width) {
			for (int i = height; i > 0; i--) {
				i_map.erase(i_map.begin() + ((i - 1) * width) + w , i_map.begin() + i * width);
			}
			width = w;
		}
		else if (w > width) {
			i_map.push_back(Tile());
			for (int i = height; i > 0; i--) {
				i_map.insert(i_map.begin() + i * width, w - width, Tile());
			}
			i_map.pop_back();
			width = w;
		}
	}

	void setTile(Tile t, int x, int y) {
		if (x >= width || y >= height) {
			throw invalid_argument("Coordinates invalid.");
		}
		i_map[y * width + x] = t;
	}
};