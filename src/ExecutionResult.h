#pragma once

struct ExecutionResult
{
    float TimeMillis;
    
    union PartOne
    {
        float FloatResult;
        int IntResult;
    } PartOne;

    union PartTwo
    {
        float FloatResult;
        int IntResult;
    } PartTwo;
};
