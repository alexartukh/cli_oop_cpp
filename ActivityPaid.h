#pragma once

#include "Activity.h"

ref class ActivityPaid : public Activity
{
public:
	ActivityPaid(int aid, int hours, int projectId);
	virtual String^ ToString() override;
};

