#include "ActivityNotPaid.h"

ActivityNotPaid::ActivityNotPaid(int aid, int hours, int projectId) : Activity(aid, hours, projectId)
{

}
String^ ActivityNotPaid::ToString()
{
	return String::Format("({0})", this->hours);
}

int ActivityNotPaid::getHours()
{
	return 0;
}
