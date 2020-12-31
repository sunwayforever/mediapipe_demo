
#ifndef __LOCKS_H__
#define __LOCKS_H__

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

typedef boost::shared_mutex BLock;
typedef boost::unique_lock<BLock> WriteLock;
typedef boost::shared_lock<BLock> ReadLock;

#endif
