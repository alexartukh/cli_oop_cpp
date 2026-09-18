#pragma once
#include "ActivityPaid.h"
ref class ActivityOvertime : public ActivityPaid
{
public:
	ActivityOvertime(int aid, int hours, int projectId);

	virtual int getHours() override;
	virtual String^ ToString() override;
};

