/* date = March 16th 2022 8:30 pm */

#ifndef _ENGINE__MEMORY_H
#define _ENGINE__MEMORY_H

struct memory_block
{
	s32 Used;
	s32 Size;
	u8* Base;
};

#endif //_ENGINE__MEMORY_H
