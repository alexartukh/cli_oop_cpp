#include "Activity.h"

Activity::Activity(int aid, int hours, int projectId)
{
	this->aid = aid;
	this->hours = hours;
	this->projectId = projectId;
}

String^ Activity::ToString()
{
	return String::Format(" {0} ", this->hours);
}

int Activity::getHours()
{
	return this->hours;
}
