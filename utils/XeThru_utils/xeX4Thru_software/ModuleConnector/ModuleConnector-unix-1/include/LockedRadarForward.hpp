#ifndef LOCKEDRADARFORWARD_HPP
#define LOCKEDRADARFORWARD_HPP

#include <mutex>

struct RadarInterface;
template<typename, typename> class UniqueLocked;
typedef UniqueLocked<RadarInterface, std::recursive_mutex> LockedRadarInterfacePtr;


#endif
