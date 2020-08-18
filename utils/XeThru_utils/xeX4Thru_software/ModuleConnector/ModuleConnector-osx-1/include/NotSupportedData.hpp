#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "datatypes.h"
#include <vector>
#include <functional>

namespace XeThru {






/**
 * @class RawNormalizedData
 *
 * Not supported
 */
struct RawNormalizedData
{

    uint32_t get_frame_counter() {
        return frame_counter;
    }

    std::vector<float> & get_data() {
        return data;
    }

    uint32_t frame_counter;
    std::vector<float> data;
};



} // namespace
#endif
