#pragma once
#include <wdm.h>
template<typename TLock>
struct Locker {
	explicit Locker(TLock& lock) : _lock(lock) {
		_lock.Lock();
	}
	~Locker() {
		_lock.Unlock();
	}
private:
	TLock& _lock;
};


struct SpinLock
{
	void Init()
	{
		KeInitializeSpinLock(&_slock);
	}

	void Lock()
	{
		KeAcquireSpinLock(&_slock, &_old_irql);
		KdPrint(("[+] Acquiring spinlock, original irql is %d\n", _old_irql));
	}

	void Unlock()
	{
		KeReleaseSpinLock(&_slock, _old_irql);
		KdPrint(("[+] Released spin lock, irql set back to %d\n", _old_irql));
	}

private:
	KSPIN_LOCK _slock;
	KIRQL _old_irql;
};