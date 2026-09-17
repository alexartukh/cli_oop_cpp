#pragma once

#include "Activity.h"

ref class ActivityNotPaid : public Activity
{
public:
	ActivityNotPaid(int aid, int hours, int projectId);
	virtual int getHours() override;
	virtual String^ ToString() override;
};

