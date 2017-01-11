#pragma once
#include "GameObject.h"
#include <Windows.h>
#include <vector>
#include <memory>
#include <random>
#include <map>
#include <set>

// Implementation from https://github.com/munificent/hauberk/blob/db360d9efa714efb6d937c31953ef849c7394a39/lib/src/content/dungeon.dart

struct Tile;

enum Direction {
	NONE,
	NORTH,
	WEST,
	EAST,
	SOUTH
};

enum TileType {
	FLOOR,
	WALL
};

struct Tile
{
	TileType tileType;
	std::shared_ptr<GameObject> tile;
};

typedef struct Rect
{
	int x, y, width, height;
	Rect(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {};
	bool intersects(Rect &otherRect) const
	{
		if (this->x > otherRect.x + otherRect.width || otherRect.x > this->x + this->width)
			return false;

		if (this->y > otherRect.y + otherRect.height || otherRect.y > this->y + this->height)
			return false;

		return true;
	}
};

class Stage
{
protected:
	int height, width;
	std::vector<std::vector<Tile>> tiles;
public:
	Stage(int height, int width)
	{
		// Must be odd size
		if (height % 2 == 0)
		{
			MessageBox(nullptr, "Stage must be of odd size", "Error!", MB_OK);
		}

		tiles.resize(height);
		for (int i = 0; i < height; i++)
		{
			tiles[i].resize(width);
		}

		this->height = height;
		this->width = width;
	};

	// Fill the tiles in
	void fillTiles(const TileType &tileType)
	{
		for (GLfloat y = 0; y < static_cast<GLfloat>(height); y++)
		{
			for (GLfloat x = 0; x < static_cast<GLfloat>(width); x++)
			{
				setTileType(glm::vec3(x, 0.0f, y), tileType);
			}
		}
	}

	TileType getTileType(glm::vec3 &pos) const
	{
		return tiles[pos.x][pos.z].tileType;
	}

	void setTileType(glm::vec3 &pos, const TileType &tileType)
	{
		tiles[static_cast<int>(pos.x)][static_cast<int>(pos.y)].tileType = tileType;
	}
};

class Dungeon : public Stage
{
private:
	// Tries to create room
	const int roomTries = 20;
	// Chance to connect rooms
	const int extraRoomConnectionChance = 20;

	// Increasing gives chance to make rooms larger
	const int roomExtraSize = 0;
	const int windingPercent = 0;

	std::vector<Rect> rooms;

	// Index of the connected region
	std::vector<std::vector<int>> regions;

	int currentRegion = -1;

public:
	Dungeon() : Stage(51, 51) {};
	Dungeon(int height, int width) : Stage(height, width) {};

	bool canCreateTile(int x, int y, Direction dir)
	{
		switch (dir)
		{
		case NORTH:
		{
			if (y - 3 < 0)
				return false;
		}
		case SOUTH:
		{
			if (y + 3 < height)
				return false;
		}
		case WEST:
		{
			if (y - 3 < width)
				return false;
		}
		case EAST:
		{
			if (x + 3 < width)
				return false;
		}
		default:
			break;
		}
		return true;
	}

	void createTile(int x, int y, Direction dir, int numTimesInDir = 1, TileType tileType = TileType::FLOOR)
	{
		switch (dir)
		{
		case NORTH:
		{
			y -= numTimesInDir;
			break;
		}
		case SOUTH:
		{
			y += numTimesInDir;
			break;
		}
		case WEST:
		{
			x -= numTimesInDir;
			break;
		}
		case EAST:
		{
			x += numTimesInDir;
			break;
		}
		default:
			break;
		}
		setTileType(glm::vec3(x, 0, y), tileType);
		regions[x][y] = currentRegion;
	}

	void createTile(int x, int y, TileType tileType = TileType::FLOOR)
	{
		setTileType(glm::vec3(x, 0, y), tileType);
		regions[x][y] = currentRegion;
	}

	void startRegion()
	{
		currentRegion++;
	}

	void generateDungeon()
	{
		// Fill the tiles
		fillTiles(TileType::WALL);

		regions.resize(height);
		for (int i = 0; i < height; i++)
		{
			regions[i].resize(width);
		}

		addRooms();

		for (int y = 1; y < height; y += 2)
		{
			for (int x = 1; x < width; x += 2)
			{
				if (getTileType(glm::vec3(x, 0, y)) != WALL) continue;
				growMaze(glm::vec3(x, 0, y));
			}
		}

		connectRegions();

		removeDeadEnds();
	}

	void addRooms()
	{
		for (int i = 0; i < roomTries; i++)
		{
			// makes sure rooms are odd-sized
			// avoids rooms that are too rectangular
			int size = generateARandomNumberBetween(1, 3 + roomExtraSize) * 2 + 1;
			int rectagnularity = generateARandomNumberBetween(0, 1 + size / 2) * 2;
			int roomWidth = size;
			int roomHeight = size;
			if (generateARandomNumberBetween(1, 2) == 1)
			{
				roomWidth += rectagnularity;
			}
			else
			{
				roomHeight += rectagnularity;
			}

			int x = generateARandomNumberBetween(0, (this->width - roomWidth) / 2) * 2 + 1;
			int y = generateARandomNumberBetween(0, (this->height - roomHeight) / 2) * 2 + 1;

			Rect room = Rect(x, y, roomWidth, roomHeight);

			bool overlappingRoom = false;
			for (auto &otherRoom : rooms)
			{
				if (room.intersects(otherRoom))
				{
					overlappingRoom = true;
					break;
				}
			}

			if (overlappingRoom)
				continue;

			rooms.push_back(room);

			startRegion();

			for (int b = y; b < y + roomHeight; b++)
			{
				for (int a = x; a < x + roomWidth; a++)
				{
					createTile(b, a);
				}
			}
		}
	}

	void growMaze(glm::vec3 pos)
	{
		std::vector<glm::vec3> cells;

		Direction lastDir;

		startRegion();
		growMaze(pos);
		cells.push_back(pos);

		while (!cells.empty())
		{
			glm::vec3 cell = cells.back();

			std::vector<Direction> unmadeCells;
			for (int dir = Direction::NORTH ; dir != Direction::SOUTH; dir++)
			{
				if (canCreateTile(pos.x, pos.z, static_cast<Direction>(dir)))
					unmadeCells.push_back(static_cast<Direction>(dir));
			}

			if (!unmadeCells.empty())
			{
				Direction dir;
				if (std::find(unmadeCells.begin(), unmadeCells.end(), dir) != unmadeCells.end()
					&& generateARandomNumberBetween(0, 100) > windingPercent)
				{
					dir = lastDir;
				}
				else
				{
					dir = unmadeCells[generateARandomNumberBetween(0, static_cast<int>(unmadeCells.size()) - 1)];
				}

				createTile(static_cast<int>(cell.x), static_cast<int>(cell.z), dir, 1);
				createTile(static_cast<int>(cell.x), static_cast<int>(cell.z), dir, 2);

				GLfloat cellx = cell.x;
				GLfloat celly = cell.z;
				switch (dir)
				{
				case NORTH:
				{
					celly -= 1.0f;
					break;
				}
				case SOUTH:
				{
					celly += 1.0f;
					break;
				}
				case WEST:
				{
					cellx -= 1.0f;
					break;
				}
				case EAST:
				{
					cellx += 1.0f;
					break;
				}
				default:
					break;
				}

				cells.push_back(glm::vec3(cellx, cell.y, celly));
				lastDir = dir;
			}
			else
			{
				cells.pop_back();

				lastDir = Direction::NONE;
			}
		}
	}

	void connectRegions()
	{
		// find regions that can connect
		std::map<glm::vec3, std::set<int>> connectorRegions;
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				if(getTileType(glm::vec3(static_cast<GLfloat>(x), 0.0f, static_cast<GLfloat>(y)), tileType) 
					!= TileType::Wall) continue;
				std::set<int> regions;
				
				//STOPPED HERE. NEED TO INIT INTS TO -1 for NULL.
				for (int dir = Direction::NORTH ; dir != Direction::SOUTH; dir++)
				{
					switch (dir)
					{
					case NORTH:
					{
						if (y - 3 < 0)
							return false;
					}
					case SOUTH:
					{
						if (y + 3 < height)
							return false;
					}
					case WEST:
					{
						if (y - 3 < width)
							return false;
					}
					case EAST:
					{
						if (x + 3 < width)
							return false;
					}
					default:
						break;
					}
					return true;
				}
			}
		}
	}

	void removeDeadEnds()
	{

	}

	template <typename T>
	T generateARandomNumberBetween(const T min, const T max)
	{
		static std::random_device rdev;
		static std::default_random_engine re(rdev());
		typedef typename std::conditional<
			std::is_floating_point<T>::value,
			std::uniform_real_distribution<T>,
			std::uniform_int_distribution<T>>::type dist_type;
		dist_type uni(min, max);
		return static_cast<T>(uni(re));
	}
};
