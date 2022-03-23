
struct random_linear_congruent
{
    u32 Value;
};

inline random_linear_congruent
Random_GetLinearCongruent(u32 seed)
{
    random_linear_congruent result;
    result.Value = seed;
    return(result);
}

inline u32
Random_LinearCongruentNumber(random_linear_congruent* random)
{
    local_persist u64 multiplier = 1103515245UL;//a
    local_persist u64 increment = 12345UL;//c
    local_persist u64 modulus = 2147483648UL;//m
    u32 result = (u32)(((u64)random->Value * multiplier + increment) % modulus);
    random->Value = result;
    return(result);
}

inline u32
Random_LinearCongruentNumber(random_linear_congruent* random, u32 min, u32 max)
{
    u32 result = Random_LinearCongruentNumber(random);
    result = (result % (max - min + 1) + min);
    return(result);
}
