#pragma once

#include "Activity.h"

ref class ActivityPaid : public Activity
{
public:
	ActivityPaid(int aid, int hours, int projectId);

	virtual int getHours() override;
	virtual String^ ToString() override;
};
