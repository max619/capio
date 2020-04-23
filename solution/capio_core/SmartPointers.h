#pragma once
#include "CapioCore.h"
#include <type_traits>

namespace cpio
{
	template<typename T>
	class Deleter
	{
	public:
		void Delete(T* ptr) { delete ptr; };

		template<typename OT>
		typename std::enable_if<std::is_base_of<OT, T>::value || std::is_same<OT, T>::value, cpio::Deleter<OT>>::type Cast()
		{
			return Deleter<OT>();
		}
	};

	template<typename T, typename Dt = cpio::Deleter<T>>
	class UniquePtr final
	{
	public:
		UniquePtr()
		{
			ptr = nullptr;
		}

		UniquePtr(T* value)
		{
			ptr = value;
		}

		UniquePtr(T* value, Dt deleter)
		{
			ptr = value;
			this->deleter = deleter;
		}

		UniquePtr(const UniquePtr<T, Dt>& rhs) = delete;

		UniquePtr(UniquePtr<T, Dt>&& rhs) noexcept
		{
			ptr = rhs.ptr;
			rhs.ptr = nullptr;
			deleter = rhs.deleter;
		}

		~UniquePtr()
		{
			Release();
		}

		UniquePtr<T, Dt>& operator= (const UniquePtr<T, Dt>& rhs) = delete;

		bool operator==(const UniquePtr<T, Dt>& rhs)
		{
			return this->ptr == rhs.ptr;
		}

		bool operator==(void* rhs)
		{
			return this->ptr == rhs;
		}

		T* operator->()
		{
			return ptr;
		}
		const T* operator->() const
		{
			return ptr;
		}

		T& Get()
		{
			if (ptr == nullptr)
				ThrowException(cpio::CapioException, "Pointer is not initialized");

			return *ptr;
		}

		const T& Get() const
		{
			if (ptr == nullptr)
				ThrowException(cpio::CapioException, "Pointer is not initialized");

			return *ptr;
		}

		T& operator*()
		{
			return Get();
		}

		const T& operator*() const
		{
			return Get();
		}

		bool IsEmpty() const
		{
			return ptr == nullptr;
		}

		operator bool() const
		{
			return ptr != nullptr;
		}

		void Release()
		{
			if (ptr != nullptr)
			{
				deleter.Delete(ptr);
				ptr = nullptr;
			}
		}

		template<typename OT, typename ODt>
		typename std::enable_if<std::is_same<OT, T>::value>::type 
			Assign(cpio::UniquePtr<OT, ODt>& rhs)
		{
			if (this != &rhs)
			{
				Release();

				if (rhs)
				{
					ptr = (T*)rhs.ptr;
					rhs.ptr = nullptr;

					deleter = rhs.deleter;
				}
			}
		}
		template<typename OT, typename ODt>
		typename std::enable_if<std::is_base_of<T, OT>::value && !std::is_same<OT, T>::value, cpio::UniquePtr<T, Dt>&>::type 
			Assign(cpio::UniquePtr<OT, ODt>& rhs)
		{
			Release();

			if (rhs)
			{
				ptr = (T*)rhs.ptr;
				rhs.ptr = nullptr;

				deleter = rhs.deleter.Cast<T>();
			}
			return *this;
		}

		template<typename OT, typename ODt>
		UniquePtr<T, Dt>& operator= (UniquePtr<OT, ODt>&& rhs)
		{			
			Assign(rhs);
			return *this;
		}
		template<typename OT, typename ODt>
		operator UniquePtr<OT, ODt>()
		{
			UniquePtr<OT, ODt> res;
			res.Assign(*this);
			return res;
		}

		friend class UniquePtr;

	private:
		T* ptr;
		Dt deleter;
	};


	template<typename T, typename ...Valtys>
	cpio::UniquePtr<T> MakeUnique(Valtys&& ... args)
	{
		T* inst = new T(std::forward<Valtys>(args)...);
		if (inst == nullptr)
			ThrowException(cpio::CapioException, "Out of memory");

		try
		{
			return cpio::UniquePtr<T>(inst);
		}
		catch (std::exception&)
		{
			delete inst;
			throw;
		}
	}
}