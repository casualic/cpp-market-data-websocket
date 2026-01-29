#pragma once

#include <array>
#include <cstddef>
#include <optional>

template <std::size_t Window>
class RollingAverage {
public:
    std::optional<double> add(double value) {
        if (count_ < Window) {
            buffer_[count_] = value;
            sum_ += value;
            ++count_;
            if (count_ == Window) {
                return sum_ / static_cast<double>(Window);
            }
            return std::nullopt;
        }

        sum_ -= buffer_[index_];
        buffer_[index_] = value;
        sum_ += value;
        index_ = (index_ + 1) % Window;

        return sum_ / static_cast<double>(Window);
    }

    void reset() {
        count_ = 0;
        index_ = 0;
        sum_ = 0.0;
    }

    std::size_t size() const {
        return count_;
    }

private:
    std::array<double, Window> buffer_{};
    std::size_t index_ = 0;
    std::size_t count_ = 0;
    double sum_ = 0.0;
};
