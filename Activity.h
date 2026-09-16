#pragma once

ref class Activity
{
private:
	int aid;
	int hours;
	int projectId;
public:
	Activity(int aid, int hours, int projectId);
	int GetValue();
};