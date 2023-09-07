#pragma once

struct Vec2 {
	float x, y;
};
struct Vec1
{
	float x;
};
struct Vec3
{
	float x, y, z;
	Vec3 operator+(Vec3 d) {
		return { x + d.x, y + d.y, z + d.z };
	}
	Vec3 operator-(Vec3 d) {
		return { x - d.x, y - d.y, z - d.z };
	}
	Vec3 operator*(float d) {
		return { x * d, y * d, z * d };
	}
	void normalize() {
		while (y < -180)
		{
			y = 360;
		}
		while (y >180)
		{
			y = -360;
		}
		while (x > 89)
		{
			x = 89;
		}
		while (x < -89)
		{
			x = -89;
		}


	}
};

struct Vec4
{
	float x, y, z, w;
};

struct BoneMatrix
{
	byte pad1[12];
	float x;
	byte pad2[12];
	float y;
	byte pad3[12];
	float z;
};
