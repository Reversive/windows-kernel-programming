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
	explicit SpinLock(PKSPIN_LOCK slock, PKIRQL old_irql) : _slock(slock), _old_irql(old_irql) { }

	void Lock()
	{
		KdPrint(("[+] Acquiring spinlock, current irql is %d\n", KeGetCurrentIrql()));
		KeAcquireSpinLock(_slock, _old_irql);
		
	}

	void Unlock()
	{
		KeReleaseSpinLock(_slock, *_old_irql);
		KdPrint(("[+] Released spin lock, irql set back to %d\n", *_old_irql));
	}

private:
	PKSPIN_LOCK _slock;
	PKIRQL _old_irql;
};