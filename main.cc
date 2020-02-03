#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>

#include <float.h>
#include <stdint.h>

#include <vector>
#include <chrono>
#include <iostream>

#include <unistd.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846264338327950288
#endif // M_PI

struct Vec4 {
	float x;
	float y;
	float z;
	float w;
};

void read_csv_point_cloud(const char* path, std::vector<Vec4>& points);

void transform_coordinates_to_depth(const std::vector<Vec4>& points);

int main(int argc, char** argv)
{
	assert(argc == 2);

	std::vector<Vec4> points;

	read_csv_point_cloud(argv[1], points);

	auto start = std::chrono::steady_clock::now();

	transform_coordinates_to_depth(points);

	auto end = std::chrono::steady_clock::now();

 	std::cout << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << '\n';

	return 0;
}

void read_csv_point_cloud(const char* path, std::vector<Vec4>& points)
{
	FILE* fp = fopen(path, "r");

	int ret;
	Vec4 v;

	while (EOF != fscanf(fp, "%f,%f,%f,%f", &v.x, &v.y, &v.z, &v.w)) {
		points.push_back(v);
	}

	fclose(fp);
}

void transform_coordinates_to_depth(const std::vector<Vec4>& points)
{
	int depth_width = 640;
	int depth_height = 480;

	float* depth_image = new float[depth_width * depth_height];
	memset(depth_image, 0, depth_width * depth_height * sizeof(float));

	static const float h_fov = 56.559f * M_PI / 180.0f;
	static const float v_fov = atan2f(depth_height * tanf(h_fov * 0.5) * 2.0, depth_width);

	static const float tan_h_fov_half = tanf(h_fov * 0.5f);
	static const float tan_v_fov_half = tanf(v_fov * 0.5f);

	static const float half_width = depth_width * 0.5f;
	static const float half_height = depth_height * 0.5f;

	static const float inv_half_width = 1.0f / half_width;
	static const float inv_half_height = 1.0f / half_height;

	static const float alpha = tan_h_fov_half * inv_half_width;
	static const float beta = tan_v_fov_half * inv_half_height;

	for (const auto& point : points) {
		if (point.z < FLT_EPSILON) {
			continue;
		}

		const float k_x = alpha * point.z; // TODO: Test if preprocessing z is better than this
		const float k_y = beta * point.z;

		const float cc = (half_width * k_x - point.x) / k_x;
		const float cr = (half_height * k_y - point.y) / k_y;
		const float cd = sqrtf(point.x * point.x + point.y * point.y + point.z * point.z);

		const int ccs[] = {
			(int)(cc), (int)(cc), (int)(cc + 1), (int)(cc + 1)
		};
		const int crs[] = {
			(int)(cr), (int)(cr + 1), (int)(cr), (int)(cr + 1)
		};

		for (int k = 0; k < 4; ++k) {
			const int tc = ccs[k];
			const int tr = crs[k];

			if (tr < 0 || tc < 0 || tr >= depth_height || tc >= depth_width)
				continue;

			const int idx = tr * depth_height + tc;
			float& depth_pixel = depth_image[idx];

			if (depth_pixel == 0) {
				depth_pixel = cd;
			} else if (depth_image[idx] > 0) {
				depth_pixel = depth_pixel < cd ? depth_pixel : cd;
			}
		}
	}

	delete[] depth_image;
	depth_image = nullptr;
}
