#include "History.h"

void Utility::History::AddChangeInternal(const ObjectPtr<ChangeBase>& InChange)
{
	if (!InChange)
		return; 
	if (changeIndex > changes.size())
		changeIndex = changes.size() - 1;
	while (changeIndex > 0)
	{
		changeIndex--;
		if(!changes.empty())
			changes.pop_back(); 
	}
	changeIndex = 0;
	changes.emplace_back(InChange);
	InChange->Do(); 
}

void Utility::History::Undo()
{
	const size_t index = changes.size() - 1 - changeIndex;
	if (index < changes.size())
	{
		if (ChangeBase* change = changes[index].Get())
			change->Undo();
		changeIndex++;
	}
}

void Utility::History::Redo()
{
	if (changeIndex > changes.size())
		changeIndex = changes.size();
	changeIndex--;
	const size_t index = changes.size() - 1 - changeIndex;
	if (index < changes.size())
	{
		if (ChangeBase* change = changes[index].Get())
			change->Do();
	}
	else
	{
		changeIndex = 0;
	}
}

void Utility::History::Clear()
{
	changes.clear();
	changeIndex = 0;
}
