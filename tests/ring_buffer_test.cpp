#include "ring_buffer.h"
#include <gtest/gtest.h>
#include <ranges>

TEST(ring_buffer, write_read_one_thread) {
    auto buffer = ring_buffer::ring_buffer<uint32_t, 8>{};

    for (auto test_size : {0uz, 1uz, buffer.size()}) {
        auto test_vector = std::views::iota(0uz, test_size) | std::ranges::to<std::vector<uint32_t>>();

        for (auto element : test_vector) {
            buffer.write(element);
        }

        for (auto element : test_vector) {
            auto read = buffer.read();
            ASSERT_EQ(read, element);
        }
    }
}
