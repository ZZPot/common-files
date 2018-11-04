#pragma once
#include <vector>
#include <set>
#include <opencv2/core.hpp>

typedef std::vector<cv::Point> contour_type;

typedef int TAG_TYPE;

struct Obj2d
{
	cv::Rect rect;
	cv::RotatedRect r_rect;
	std::vector<std::vector<contour_type>> contours;
	double square;
	TAG_TYPE tag;
	std::string name;
};

struct Marker2d
{
	cv::Point2d point;
	TAG_TYPE tag;
	std::string name;
};

struct Path2d
{
	std::vector<cv::Point> points;
	TAG_TYPE tag;
};

enum OBJECT_TYPE
{
	OBJECT_NONE = 0,
	OBJECT_OBJ2D = 1,
	OBJECT_MARKER2D,
	OBJECT_PATH2D
};