#include "History.h"

void Utility::History::AddChangeInternal(const ObjectPtr<ChangeBase>& InChange)
{
	if (!InChange)
		return; 
	if (ChangeIndex > Changes.size())
		ChangeIndex = Changes.size() - 1;
	while (ChangeIndex > 0)
	{
		ChangeIndex--;
		if(!Changes.empty())
			Changes.pop_back(); 
	}
	ChangeIndex = 0;
	Changes.emplace_back(InChange);
	InChange->Do(); 
}

void Utility::History::Undo()
{
	const size_t index = Changes.size() - 1 - ChangeIndex;
	if (index < Changes.size())
	{
		if (ChangeBase* change = Changes[index].Get())
			change->Undo();
		ChangeIndex++;
	}
}

void Utility::History::Redo()
{
	if (ChangeIndex > Changes.size())
		ChangeIndex = Changes.size();
	ChangeIndex--;
	const size_t index = Changes.size() - 1 - ChangeIndex;
	if (index < Changes.size())
	{
		if (ChangeBase* change = Changes[index].Get())
			change->Do();
	}
	else
	{
		ChangeIndex = 0;
	}
}

void Utility::History::Clear()
{
	Changes.clear();
	ChangeIndex = 0;
}
