#pragma once
#include <ntddk.h>


struct CriticalRegion
{
    void Enter()
    {
        KeEnterCriticalRegion();
    }

    void Leave()
    {
        KeLeaveCriticalRegion();
    }

    const char* RegionName() const
    {
        return "Critical";
    }
};

struct GuardedRegion
{
    void Enter()
    {
        KeEnterGuardedRegion();
    }

    void Leave()
    {
        KeLeaveGuardedRegion();
    }

    const char* RegionName() const
    {
        return "Guarded";
    }
};

// skipping rule of 0/3/5 for this snippet
template <typename IRegion>
struct Region
{
    explicit Region(IRegion* _region) : _region(_region)
    {
        // Added logging just for showing entry/exit from region
        if (_region != nullptr)
        {
            KdPrint(("[+] Entering %s region\n", _region->RegionName()));
            _region->Enter();
        }
    }
    ~Region()
    {
        if (_region != nullptr)
        {
            _region->Leave();
            KdPrint(("[+] Left %s region\n", _region->RegionName()));
        }
    }

private:
    IRegion* _region;
};