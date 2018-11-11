
#ifndef DOOR_HPP
#define DOOR_HPP

#include "../math.hpp"

class Texture;
class Model3D;
class DynamicTransform3D;

class Door {
public:

static float DOOR_DEPTH;
static float DOOR_HEIGHT;
static float DOOR_WIDTH;

static float OPEN_DISTANCE;
static float OPEN_DISTANCE_SQUARED;

static float TIME_TO_OPEN;
static float TIME_TO_CLOSE;

Door(Vec3 position, Vec3 openPosition, Texture* texture);

void open();
Vec2 getDoorSize();

void update();
void render() const;

DynamicTransform3D* transform;

private:
Texture* texture;
static Model3D* mesh;

public:
bool rotated = false;
float offset = 0;

bool isOpening = false;
float openingStartTime;
float openTime;
float closingStartTime;
float closeTime;

Vec3 openPosition;
Vec3 closePosition;
};

#endif
