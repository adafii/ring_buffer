#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <mutex>

namespace ring_buffer {

template <typename element_t, std::size_t buffer_size>
class ring_buffer {
    static_assert(buffer_size > 0, "Buffer size has to be greater than 0");

public:
    constexpr void write(element_t data) {
        auto local_head = head_.load();
        while (!head_.compare_exchange_strong(local_head, (local_head + 1) % buffer_size)) {
        }

        buffer_[local_head].state.wait(state_t::writing);
        buffer_[local_head].state.wait(state_t::written);
        buffer_[local_head].state.wait(state_t::reading);

        buffer_[local_head].state.store(state_t::writing);
        buffer_[local_head].state.notify_all();

        buffer_[local_head].data = data;

        buffer_[local_head].state.store(state_t::written);
        buffer_[local_head].state.notify_all();
    }

    [[nodiscard]] constexpr auto read() {
        auto local_tail = tail_.load();
        while (!tail_.compare_exchange_strong(local_tail, (local_tail + 1) % buffer_size)) {
        }

        buffer_[local_tail].state.wait(state_t::reading);
        buffer_[local_tail].state.wait(state_t::read);
        buffer_[local_tail].state.wait(state_t::writing);

        buffer_[local_tail].state.store(state_t::reading);
        buffer_[local_tail].state.notify_all();

        auto read_data = buffer_[local_tail].data;

        buffer_[local_tail].state.store(state_t::read);
        buffer_[local_tail].state.notify_all();

        return read_data;
    }

    [[nodiscard]] auto size() const {
        return buffer_size;
    }

private:
    enum class state_t : uint8_t {
        read,
        writing,
        written,
        reading
    };

    struct cell_t {
        element_t data;
        std::atomic<state_t> state;
    };

    std::array<cell_t, buffer_size> buffer_{};

    std::atomic<std::size_t> head_{0};
    std::atomic<std::size_t> tail_{0};
};

}  // namespace ring_buffer