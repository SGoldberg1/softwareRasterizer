

inline world_difference
World_Subtract(world_map* world, world_position target, world_position position)
{
    world_difference result;
    v3s chunkDelta = (target.Chunk - position.Chunk) * world->ChunkSideInMeters;
    result.Delta.X = (f32)chunkDelta.X + (target.Offset.X - position.Offset.X);
    result.Delta.Y = (f32)chunkDelta.Y + (target.Offset.Y - position.Offset.Y);
    result.Delta.Z = (f32)chunkDelta.Z + (target.Offset.Z - position.Offset.Z);
    return(result);
}

inline v3
World_MapIntoCameraSpace(engine_state* state, world_position position)
{
	world_difference difference = 
		World_Subtract(state->WorldMap, position, state->Camera.Position);
	v3 result = difference.Delta;
	return(result);
}

internal_function inline world_position
World_MapIntoChunkSpace(world_map* world, world_position position, v3 offset)
{
    position.Offset += offset;
    
    world_position result;
    
    s32 offsetX = Math_RoundF32ToS32((position.Offset.X) / (f32)world->ChunkSideInMeters);
    s32 offsetY = Math_RoundF32ToS32((position.Offset.Y) / (f32)world->ChunkSideInMeters);
    s32 offsetZ = Math_RoundF32ToS32((position.Offset.Z) / (f32)world->ChunkSideInMeters);
    
    result.Chunk.X = position.Chunk.X + offsetX;
    result.Chunk.Y = position.Chunk.Y + offsetY;
    result.Chunk.Z = position.Chunk.Z + offsetZ;
    
    result.Offset.X = position.Offset.X - offsetX * world->ChunkSideInMeters;
    result.Offset.Y = position.Offset.Y - offsetY * world->ChunkSideInMeters;
    result.Offset.Z = position.Offset.Z - offsetZ * world->ChunkSideInMeters;
    
    return(result);
}

internal_function inline b32
World_ValidateEntityPairs(engine_state* state)
{
    b32 result = TRUE;
    
    for(s32 i = 0; i < state->HighEntityCount; ++i)
    {
        world_high_entity* entity = (state->HighEntities + i);
        
        result = result && (state->LowEntities[entity->LowIndex].HighIndex == i);
    }
    
    return(result);
}

internal_function inline world_chunk*
World_GetChunkFrom(world_map* world, 
				   s32 chunkX, s32 chunkY, s32 chunkZ, 
				   memory_block* memory = 0)
{
    Assert(chunkX <  WORLD_INVALID_CHUNK_COORDINATE);
    Assert(chunkX > -WORLD_INVALID_CHUNK_COORDINATE);
    Assert(chunkY <  WORLD_INVALID_CHUNK_COORDINATE);
    Assert(chunkY > -WORLD_INVALID_CHUNK_COORDINATE);
    Assert(chunkZ <  WORLD_INVALID_CHUNK_COORDINATE);
    Assert(chunkZ > -WORLD_INVALID_CHUNK_COORDINATE);
    
	s32 hash = 17 * chunkX + 13 * chunkY + 7 * chunkZ;
	s32 index = hash & (ArrayLength(world->Chunks) - 1);
	Assert(index < ArrayLength(world->Chunks));
	world_chunk* result = world->Chunks + index;
	
    do
    {
        // NOTE(Stephen): First chunk exists
        if((result->Position.X == chunkX) && 
		   (result->Position.Y == chunkY) &&
		   (result->Position.Z == chunkZ))
        {
            break;
        }
        
		if(memory)
		{
			if((result->Position.X != WORLD_INVALID_CHUNK_COORDINATE) &&
			   (result->Next == 0))
			{
				result->Next = MemoryBlock_PushStruct(memory, world_chunk);
				result->Next->Position.X = WORLD_INVALID_CHUNK_COORDINATE;
				result = result->Next;
			}
			
			if(result->Position.X == WORLD_INVALID_CHUNK_COORDINATE)
			{
				result->Position = { chunkX, chunkY, chunkZ };
				result->FirstBlock = {};
				Assert(result->Next == 0);
				break;
			}
		}
		
		result = result->Next;
	}while(result);
	
	return(result);
}

void
World_MakeEntityLow(engine_state* state, s32 lowIndex)
{
	Assert(lowIndex >= 0);
	Assert(lowIndex < ArrayLength(state->LowEntities));
	
	world_low_entity* lowEntity = state->LowEntities + lowIndex;
	s32 highIndex = lowEntity->HighIndex;
	Assert(highIndex >= 0);
	
	if(highIndex)
	{
		s32 lastHighIndex = state->HighEntityCount - 1;
		
		if(highIndex != lastHighIndex)
		{
			world_high_entity* deletingEntity = (state->HighEntities + highIndex);
			world_high_entity* lastEntity = (state->HighEntities + lastHighIndex);;
			*deletingEntity = *lastEntity;
			state->LowEntities[lastEntity->LowIndex].HighIndex = highIndex;
		}
		
		lowEntity->HighIndex = 0;
		--state->HighEntityCount;
	}
}

void
World_MakeEntityHigh(engine_state* state, s32 lowIndex)
{
	Assert(lowIndex >= 0);
	Assert(lowIndex < ArrayLength(state->LowEntities));
	
	world_low_entity* lowEntity = state->LowEntities + lowIndex;
	
	if(lowEntity->HighIndex == 0)
	{
		if(state->HighEntityCount < ArrayLength(state->HighEntities))
		{
			s32 highEntityIndex = state->HighEntityCount++;
			world_high_entity* highEntity = (state->HighEntities + highEntityIndex);
			highEntity->LowIndex = lowIndex;
			lowEntity->HighIndex = highEntityIndex;
			world_difference difference = World_Subtract(state->WorldMap, 
														 lowEntity->Position, state->Camera.Position);
			highEntity->Position = difference.Delta;
		}
		else
		{
			InvalidCodePath;
		}
	}
}

void
World_OffsetAndValidateEntityFrequency(engine_state* state, v3 offset, cube_bounds bounds)
{
	for(s32 index = 1; 
		index < state->HighEntityCount;)
	{
		world_high_entity* entity = state->HighEntities + index;
		entity->Position.X -= offset.X;
		entity->Position.Y -= offset.Y;
		entity->Position.Z -= offset.Z;
		
		if(Math_IsWithinCubeBounds(bounds, entity->Position))
		{
			++index;
		}
		else
		{
			World_MakeEntityLow(state, entity->LowIndex);
		}
	}
}

inline world_low_entity*
World_GetLow(engine_state* state, s32 lowIndex)
{
	Assert(lowIndex < state->LowEntityCount);
	world_low_entity* result = state->LowEntities + lowIndex;
	return(result);
}

inline world_low_entity*
World_AddLowEntity(engine_state* state, world_entity_type type)
{
	s32 index = state->LowEntityCount++;
	Assert(index < ArrayLength(state->LowEntities));
	world_low_entity* result = (state->LowEntities + index);
	result->Type = type;
	result->Position = {};
	result->HighIndex = 0;
	return(result);
}

inline void
World_AddNullEntity(engine_state* state)
{
	World_AddLowEntity(state, type_entity_null);
	World_MakeEntityHigh(state, type_entity_null);
}

inline world_low_entity*
World_AddFloor(engine_state* state, v3 position, v4 color)
{
	world_low_entity* result = World_AddLowEntity(state, type_entity_floor);
	result->Position = World_MapIntoChunkSpace(state->WorldMap, {}, position);
	result->Color = color;
	return(result);
}

inline world_low_entity*
World_AddWall(engine_state* state, v3 position, v4 color)
{
	world_low_entity* result = World_AddLowEntity(state, type_entity_wall);
	result->Position = World_MapIntoChunkSpace(state->WorldMap, {}, position);
	result->Color = color;
	return(result);
}

inline b32
World_InSameChunk(world_position a, world_position b)
{
	b32 result = ((a.Chunk.X == b.Chunk.X) &&
				  (a.Chunk.Y == b.Chunk.Y) &&
				  (a.Chunk.Z == b.Chunk.Z));
	return(result);
}

void
World_ChangeEntityLocation(world_map* world, memory_block* memory, s32 lowEntityIndex,
						   world_position* newPosition, world_position* oldPosition = 0)
{
	if(oldPosition && World_InSameChunk(*newPosition, *oldPosition))
	{
		*oldPosition = *newPosition;
	}
	else
	{
		if(oldPosition)
		{
			world_chunk* chunk = World_GetChunkFrom(world, 
													oldPosition->Chunk.X,
													oldPosition->Chunk.Y,
													oldPosition->Chunk.Z);
			Assert(chunk);
			
			if(chunk)
			{
				world_entity_block* firstBlock = &chunk->FirstBlock;
				
				for(world_entity_block* block = firstBlock; 
					block;
					block = block->Next)
				{
					for(s32 index = 0; index < block->EntityCount; ++index)
					{
						if(block->LowEntityIndex[index] == lowEntityIndex)
						{
							block->LowEntityIndex[index] = 
								firstBlock->LowEntityIndex[--firstBlock->EntityCount];
							
							if(firstBlock->EntityCount == 0)
							{
								if(firstBlock->Next)
								{
									world_entity_block* nextBlock = firstBlock->Next;
									*firstBlock = *nextBlock;
									nextBlock->Next = world->NextFreeEntityBlock;
									world->NextFreeEntityBlock = nextBlock;
								}
								
							}//if(firstBlock->EntityCount == 0)
							
							block = 0;
							break;
						}//if(block->EntityIndex[index] == lowEntityIndex)
					}//for(s32 index = 0; index < block->EntityCount; ++index)
				}//for(world_entity_block* block = firstBlock; block; block = block->Next)
			}//if(chunk)
		}//if(oldPosition)
		
		world_chunk* chunk = World_GetChunkFrom(world, 
												newPosition->Chunk.X,
												newPosition->Chunk.Y,
												newPosition->Chunk.Z,
												memory);
		
		Assert(chunk);
		world_entity_block* block = &chunk->FirstBlock;
		
		if(block->EntityCount == ArrayLength(block->LowEntityIndex))
		{
			world_entity_block* oldBlock = world->NextFreeEntityBlock;
			
			if(oldBlock)
			{
				world->NextFreeEntityBlock = oldBlock->Next;
			}
			else
			{
				
				oldBlock = MemoryBlock_PushStruct(memory, world_entity_block);
			}
			
			*oldBlock = *block;
			block->Next = oldBlock;
			block->EntityCount = 0;
			
		}//if(block->EntityCount == ArrayLength(block->EntityIndex))
		
		Assert(block->EntityCount < ArrayLength(block->LowEntityIndex));
		block->LowEntityIndex[block->EntityCount++] = lowEntityIndex;
	}
}

inline void
World_Initialize(engine_state* state, s32 chunkSideInMeters)
{
	state->WorldMap = MemoryBlock_PushStruct(&state->WorldMemory, world_map);
	state->WorldMap->ChunkSideInMeters = chunkSideInMeters;
	state->WorldMap->NextFreeEntityBlock = 0;
	
	for(s32 i = 0; 
		i < ArrayLength(state->WorldMap->Chunks); 
		++i)
	{
		world_chunk* chunk = (state->WorldMap->Chunks + i);
		chunk->Position = 
		{ 
			WORLD_INVALID_CHUNK_COORDINATE, 
			WORLD_INVALID_CHUNK_COORDINATE, 
			WORLD_INVALID_CHUNK_COORDINATE
		};
		
		chunk->FirstBlock = {};
		chunk->Next = 0;
	}
}

void
World_Build(engine_state* state)
{
	World_AddNullEntity(state);
	
	v3 position;
	position.X = 0;
	position.Y = 0;
	position.Z = 0;
	world_low_entity* entity = World_AddFloor(state, position, { 0.3f, 0.7f, 0.3f, 1 });
	
	World_ChangeEntityLocation(state->WorldMap, &state->WorldMemory, state->LowEntityCount - 1,
							   &entity->Position);
	
	position.X = -1.5;
	position.Y = 2;
	position.Z = 0;
	
	entity = World_AddWall(state, position, {0.3f, 0.3f, 0.7f, 1.0f});
	World_ChangeEntityLocation(state->WorldMap, &state->WorldMemory, state->LowEntityCount - 1,
							   &entity->Position);
	
#if 0	
	position.X = 1.5;
	position.Y = 2;
	position.Z = 0;
	
	entity = World_AddWall(state, position, {0.7f, 0.7f, 0.3f, 1.0f});
	World_ChangeEntityLocation(state->WorldMap, &state->WorldMemory, state->LowEntityCount - 1,
							   &entity->Position);
#endif
	
}