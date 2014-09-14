/* Copyright (C) 2014 WATANABE Yuki
 *
 * This file is part of Sesh.
 *
 * Sesh is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * Sesh is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * Sesh.  If not, see <http://www.gnu.org/licenses/>.  */

#ifndef INCLUDED_os_signaling_signal_number_set_test_helper_hh
#define INCLUDED_os_signaling_signal_number_set_test_helper_hh

#include "buildconfig.h"

#include <algorithm>
#include <iterator>
#include <set>
#include <utility>
#include "common/container_helper.hh"
#include "os/signaling/signal_number.hh"
#include "os/signaling/signal_number_set.hh"

namespace sesh {
namespace os {
namespace signaling {

class SignalNumberSetFake : public signal_number_set {

private:

    std::set<signal_number> mSet;
    bool mIsPositive = true;

    signal_number_set &reinitialize(bool isPositive) {
        mIsPositive = isPositive;
        mSet.clear();
        return *this;
    }

public:

    bool test(signal_number n) const override {
        return common::contains(mSet, n) == mIsPositive;
    }

    signal_number_set &set(signal_number n, bool v = true) override {
        if (v == mIsPositive)
            mSet.insert(n);
        else
            mSet.erase(n);
        return *this;
    }

    signal_number_set &set() override {
        return reinitialize(false);
    }

    signal_number_set &reset() override {
        return reinitialize(true);
    }

    std::unique_ptr<signal_number_set> clone() const override {
        return std::unique_ptr<signal_number_set>(new auto(*this));
    }

    void insertAll(const SignalNumberSetFake &that) {
        SignalNumberSetFake result;

        result.mIsPositive = this->mIsPositive & that.mIsPositive;

        if (this->mIsPositive)
            if (that.mIsPositive)
                std::set_union(
                        this->mSet.begin(),
                        this->mSet.end(),
                        that.mSet.begin(),
                        that.mSet.end(),
                        std::inserter(result.mSet, result.mSet.end()));
            else
                std::set_difference(
                        this->mSet.begin(),
                        this->mSet.end(),
                        that.mSet.begin(),
                        that.mSet.end(),
                        std::inserter(result.mSet, result.mSet.end()));
        else
            if (that.mIsPositive)
                std::set_difference(
                        that.mSet.begin(),
                        that.mSet.end(),
                        this->mSet.begin(),
                        this->mSet.end(),
                        std::inserter(result.mSet, result.mSet.end()));
            else
                std::set_intersection(
                        this->mSet.begin(),
                        this->mSet.end(),
                        that.mSet.begin(),
                        that.mSet.end(),
                        std::inserter(result.mSet, result.mSet.end()));

        *this = std::move(result);
    }

    void eraseAll(const SignalNumberSetFake &that) {
        mIsPositive = !mIsPositive;
        insertAll(that);
        mIsPositive = !mIsPositive;
    }

}; // class SignalNumberSetFake

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_signal_number_set_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s: */
