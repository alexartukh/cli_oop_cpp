#pragma once

using namespace System;

ref class Activity
{
protected:
	int aid;
	int hours;
	int projectId;
	String^ type;
public:
	Activity(int aid, int hours, int projectId);
	virtual int getHours() override;
	virtual String^ ToString() override;
};
