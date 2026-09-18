#pragma once

using namespace System;

ref class Activity
{
private:
	int aid;
	int hours;
	int projectId;

public:
	Activity(int aid, int hours, int projectId);

	virtual int getHours() override;
	virtual String^ ToString() override;
};
