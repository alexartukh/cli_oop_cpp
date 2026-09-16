#include "Activity.h"

Activity::Activity(int aid, int hours, int projectId)
{
	this->aid = aid;
	this->hours = hours;
	this->projectId = projectId;
}

// смысл активности в том, чтобы она возвращала количество потраченных часов
int Activity::GetValue()
{
	return this->hours;
}
