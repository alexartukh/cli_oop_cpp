#include "ActivityOvertime.h"

ActivityOvertime::ActivityOvertime(int aid, int hours, int projectId) : ActivityPaid(aid, hours, projectId)
{

}
String^ ActivityOvertime::ToString()
{
	return String::Format("[[{0}]]", this->hours);
}

int ActivityOvertime::getHours()
{
	int v = this->Activity::getHours() * 2;
	return v;
}

