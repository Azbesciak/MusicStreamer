#include <iostream>
#include <iomanip>
#include <mutex>


#ifndef MUSICSTREAMER_SYNCH_H
#define MUSICSTREAMER_SYNCH_H

#define synchronized(m) \
    for (std::unique_lock<std::recursive_mutex> lk(m);lk;lk.unlock())

#endif //MUSICSTREAMER_SYNCH_H
