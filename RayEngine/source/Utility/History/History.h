#pragma once

#include <vector>
#include <functional>
#include "../Memory/Pointers.h"

namespace Utility
{
	struct ChangeBase
	{
		virtual ~ChangeBase() = default;
		virtual void Do() = 0;
		virtual void Undo() = 0; 
	};
	
	template <class T = int>
	struct Change : ChangeBase
	{
		Change() = default; 
		Change(
			const std::function<void(const T& Data)>& InDo,
			const std::function<void(const T& Data)>& InUndo,
			const T& InData = T()) :
			DoFunc(InDo),
			UndoFunc(InUndo),
			Data(InData) {}

		void Do() override { DoFunc(Data); }
		void Undo() override { UndoFunc(Data); }

	private: 
		std::function<void(const T& Data)> DoFunc = {};
		std::function<void(const T& Data)> UndoFunc = {};
		T Data = {};
	};

	class History 
	{
	public:

		template <class T>
		void AddChange(const T& InChange)
		{
			// Create copy as ptr, to enable inheritance 
			T* ptr = new T(InChange);
			// Quickly wrap ptr in ObjectPtr
			const ObjectPtr basePtr = reinterpret_cast<ChangeBase*>(ptr);
			AddChangeInternal(basePtr);
		}
		
		void Undo();
		void Redo();
		void Clear();
		
	private:
		void AddChangeInternal(const ObjectPtr<ChangeBase>& InChange);
		std::vector<ObjectPtr<ChangeBase>> Changes = {};
		size_t ChangeIndex = 0;
	};
}
