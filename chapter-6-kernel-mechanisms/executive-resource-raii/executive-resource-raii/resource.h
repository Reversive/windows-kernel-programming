#pragma once
#include <wdm.h>

struct ExclusiveResource
{
	ExclusiveResource(PERESOURCE resource) : _resource(resource) {}

	void Acquire()
	{
		ExEnterCriticalRegionAndAcquireResourceExclusive(_resource);
	}

	void Release()
	{
		ExReleaseResourceAndLeaveCriticalRegion(_resource);
	}

	const char* Name() const
	{
		return "Exclusive";
	}

private:
	PERESOURCE _resource;
};

struct SharedResource
{
	SharedResource(PERESOURCE resource) : _resource(resource) {}

	void Acquire()
	{
		ExEnterCriticalRegionAndAcquireResourceShared(_resource);
	}

	void Release()
	{
		ExReleaseResourceAndLeaveCriticalRegion(_resource);
	}

	const char* Name() const
	{
		return "Shared";
	}

private:
	PERESOURCE _resource;
};

template<typename IResource>
struct Resource
{
	Resource(IResource* _resource) : _resource(_resource)
	{
		if (_resource != nullptr)
		{
			KdPrint(("Entering critical region and acquiring %s resource\n", _resource->Name()));
			_resource->Acquire();
		}
	}

	~Resource()
	{
		if (_resource != nullptr)
		{
			_resource->Release();
			KdPrint(("Leaving critical region and releasing %s resource\n", _resource->Name()));
		}
	}

private:
	IResource* _resource;
};