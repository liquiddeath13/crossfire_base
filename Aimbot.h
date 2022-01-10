#pragma once

class Aimbot
{
	float smooth;
	POINT distoff;
	POINT currDist;
	void MoveCursor(POINT distance)
	{
		mouse_event(MOUSEEVENTF_MOVE, (DWORD)distance.x, (DWORD)distance.y, NULL, NULL);
	}
public:
	Aimbot(float sval, POINT doval) : smooth(sval), distoff(doval)
	{
	}
	void SetSmooth(float sval)
	{
		smooth = sval;
	}
	void SetDistOff(POINT doval)
	{
		distoff = doval;
	}
	POINT Do(POINT distance)
	{
		if (distoff.x > 0 && distoff.y > 0) {
			if ((distance.x < distoff.x && distance.y < distoff.y)) return distance;
		}
		currDist = { static_cast<LONG>(distance.x / smooth), static_cast<LONG>(distance.y / smooth) };
		MoveCursor(currDist);
		return currDist;
	}
};

Aimbot* aim = new Aimbot(1.f, { 0, 0 });