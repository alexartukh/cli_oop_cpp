#include "ActivityPaid.h"

ActivityPaid::ActivityPaid(int aid, int hours, int projectId) : Activity(aid, hours, projectId)
{

}
String^ ActivityPaid::ToString()
{
	return String::Format("[{0}]", this->hours);
}
