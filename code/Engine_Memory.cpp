

inline void
MemoryBlock_Initialize(memory_block* block, s32 size, void* base)
{
	block->Used = 0;
	block->Size = size;
	block->Base = (u8*)base;
}

inline void
MemoryBlock_Reset(memory_block* block)
{
	block->Used = 0;
}

#define MemoryBlock_PushSize(block, size) MemoryBlock_PushSize_(block, size)
#define MemoryBlock_PushArray(block, count, type) \
(type*)MemoryBlock_PushSize_(block, (sizeof(type) * (count)))
#define MemoryBlock_PushStruct(block, type) \
(type*)MemoryBlock_PushSize_(block, sizeof(type))

inline void*
MemoryBlock_PushSize_(memory_block* block, s32 size)
{
	Assert((block->Used + size) <= block->Size);
	void* result = (block->Base + block->Used);
	block->Used += size;
	return(result);
}

#define MemoryBlock_PopSize(block, size) (type*)MemoryBlock_PopSize_(block, size)
#define MemoryBlock_PopArray(block, count, type) MemoryBlock_PopSize_(block, (sizeof(type) * (count)))
#define MemoryBlock_PopStruct(block, type) (type*)MemoryBlock_PopSize_(block, sizeof(type))

inline void*
MemoryBlock_PopSize_(memory_block* block, s32 size)
{
	Assert((block->Used - size) >= 0);
	block->Used -= size;
	void* result = (block->Base + block->Used);
	return(result);
}
