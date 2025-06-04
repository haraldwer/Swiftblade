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
			const std::function<void(const T& InData)>& InDo,
			const std::function<void(const T& InData)>& InUndo,
			const T& InData = T()) :
			doFunc(InDo),
			undoFunc(InUndo),
			data(InData) {}

		void Do() override { doFunc(data); }
		void Undo() override { undoFunc(data); }

	private: 
		std::function<void(const T& InData)> doFunc = {};
		std::function<void(const T& InData)> undoFunc = {};
		T data = {};
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
		std::vector<ObjectPtr<ChangeBase>> changes = {};
		size_t changeIndex = 0;
	};
}
