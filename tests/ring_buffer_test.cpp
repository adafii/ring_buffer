#include "ring_buffer.h"
#include <gtest/gtest.h>
#include <ranges>
#include <thread>

TEST(ring_buffer, write_read_one_thread) {
    auto buffer = ring_buffer::ring_buffer<uint32_t, 32>{};

    for (const auto test_size : {0uz, 1uz, buffer.size()}) {
        const auto test_data = std::views::iota(0uz, test_size) | std::ranges::to<std::vector<uint32_t>>();

        for (auto element : test_data) {
            buffer.write(element);
        }

        for (auto element : test_data) {
            ASSERT_EQ(buffer.read(), element);
        }
    }
}

TEST(ring_buffer, one_writer_one_reader_thread) {
    auto buffer = ring_buffer::ring_buffer<uint32_t, 32>{};

    for (const auto test_size : {0uz, 1uz, buffer.size() * 2, buffer.size() * 8}) {
        const auto test_data = std::views::iota(0uz, test_size) | std::ranges::to<std::vector<uint32_t>>();

        auto writer = std::jthread{[&buffer, &test_data]() {
            for (auto element : test_data) {
                buffer.write(element);
            }
        }};

        auto reader = std::jthread{[&buffer, &test_data]() {
            for (auto element : test_data) {
                ASSERT_EQ(buffer.read(), element);
            }
        }};
    }
}