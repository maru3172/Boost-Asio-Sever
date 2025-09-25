#pragma once

enum
{
	SLIST_ALIGNMENT = 16
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
struct MemoryHeader : public SLIST_ENTRY
{
	int32 allocSize;

	MemoryHeader(int32 size) : allocSize(size) { }
	
	static void* AttachHeader(MemoryHeader* header, int32 size)
	{
		new(header)MemoryHeader(size);
		return reinterpret_cast<void*>(++header);
	}
	static MemoryHeader* DetachHeader(void* ptr)
	{
		MemoryHeader* header = reinterpret_cast<MemoryHeader*>(ptr) - 1;
		return header;
	}
};

DECLSPEC_ALIGN(SLIST_ALIGNMENT)
class MemoryPool
{
	SLIST_HEADER _header;
	int32 _allocSize = 0;
	std::atomic<int32> _useCount = 0;
	std::atomic<int32> _reserveCount = 0;
public:
	MemoryPool(int32 allocSize);
	~MemoryPool();

	void Push(MemoryHeader* ptr);
	MemoryHeader* Pop();
};
