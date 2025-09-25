#pragma once

class RefCountable
{
protected:
	std::atomic<int32> _refCount;
public:
	RefCountable() : _refCount(1) { }
	virtual ~RefCountable() { }

	int32 GetRefCount() { return _refCount; }
	int32 AddRef() { return ++_refCount; }
	int32 ReleaseRef()
	{
		int32 refCount = --_refCount;
		if (refCount == 0)
			delete this;
		return refCount;
	}
};

template<typename T>
class TSharedPtr
{
	T* _ptr = nullptr;

	inline void Set(T* ptr)
	{
		_ptr = ptr;
		if (ptr)
			ptr->AddRef();
	}

	inline void Release()
	{
		if (_ptr != nullptr)
		{
			_ptr->ReleaseRef();
			_ptr = nullptr;
		}
	}
public:
	TSharedPtr() { }
	TSharedPtr(T* ptr) { Set(ptr); }
	
	// 복사생성
	TSharedPtr(const TSharedPtr& rhs) { Set(rhs._ptr); }
	// 이동생성
	TSharedPtr(TSharedPtr&& rhs) noexcept { _ptr = rhs._ptr; rhs._ptr = nullptr; }
	// 상속 관계 복사 생성
	template<typename U>
	TSharedPtr(const TSharedPtr<U>& rhs) { Set(static_cast<T*>(rhs._ptr)); }

	~TSharedPtr() { Release(); }

	// 복사대입연산자
	TSharedPtr& operator=(const TSharedPtr& rhs)
	{
		if (_ptr != rhs._ptr)
		{
			Release();
			Set(rhs._ptr);
		}
		return *this;
	}

	// 이동대입연산자
	TSharedPtr& operator=(TSharedPtr&& rhs) noexcept
	{
		Release();
		_ptr = rhs._ptr;
		rhs._ptr = nullptr;
		return *this;
	}

	// 연산자 오버로딩 - 같은가?
	bool operator==(const TSharedPtr& rhs) const { return _ptr == rhs._ptr; }
	bool operator==(T* ptr) const { return _ptr == ptr; }
	// 연산자 오버로딩 - 달라?
	bool operator!=(const TSharedPtr& rhs) const { return _ptr != rhs._ptr; }
	bool operator!=(T* ptr) const { return _ptr != ptr; }
	// 연산자 오버로딩 - 현재 주소가 오른쪽 주소보다 작아?
	bool operator<(const TSharedPtr& rhs) const { return _ptr < rhs._ptr; }
	// 만든 sharedptr을 빼고 진짜 포인터에다가 넣어줘야 할 때, 스마트 포인터랑 비슷한 개념
	// 포인터가 움직이고 존재하는 객체처럼 존재
	T* operator*() { return _ptr; }
	const T* operator*() const { return _ptr; }
	operator T* () const { return _ptr; }
	T* operator->() { return _ptr; }
	const T* operator->() const { return _ptr; }

	bool InNull() { return _ptr == nullptr; }
};