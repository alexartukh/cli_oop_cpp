#pragma once

#include "Activity.h"

ref class ActivityNotPaid : public Activity
{
public:
	ActivityNotPaid(int aid, int hours, int projectId);
};

