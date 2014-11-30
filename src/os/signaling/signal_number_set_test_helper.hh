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

class signal_number_set_fake : public signal_number_set {

private:

    std::set<signal_number> m_set;
    bool m_is_positive = true;

    signal_number_set &reinitialize(bool is_positive) {
        m_is_positive = is_positive;
        m_set.clear();
        return *this;
    }

public:

    bool test(signal_number n) const override {
        return common::contains(m_set, n) == m_is_positive;
    }

    signal_number_set &set(signal_number n, bool v = true) override {
        if (v == m_is_positive)
            m_set.insert(n);
        else
            m_set.erase(n);
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

    void insert_all(const signal_number_set_fake &that) {
        signal_number_set_fake result;

        result.m_is_positive = this->m_is_positive & that.m_is_positive;

        if (this->m_is_positive)
            if (that.m_is_positive)
                std::set_union(
                        this->m_set.begin(),
                        this->m_set.end(),
                        that.m_set.begin(),
                        that.m_set.end(),
                        std::inserter(result.m_set, result.m_set.end()));
            else
                std::set_difference(
                        this->m_set.begin(),
                        this->m_set.end(),
                        that.m_set.begin(),
                        that.m_set.end(),
                        std::inserter(result.m_set, result.m_set.end()));
        else
            if (that.m_is_positive)
                std::set_difference(
                        that.m_set.begin(),
                        that.m_set.end(),
                        this->m_set.begin(),
                        this->m_set.end(),
                        std::inserter(result.m_set, result.m_set.end()));
            else
                std::set_intersection(
                        this->m_set.begin(),
                        this->m_set.end(),
                        that.m_set.begin(),
                        that.m_set.end(),
                        std::inserter(result.m_set, result.m_set.end()));

        *this = std::move(result);
    }

    void erase_all(const signal_number_set_fake &that) {
        m_is_positive = !m_is_positive;
        insert_all(that);
        m_is_positive = !m_is_positive;
    }

}; // class signal_number_set_fake

} // namespace signaling
} // namespace os
} // namespace sesh

#endif // #ifndef INCLUDED_os_signaling_signal_number_set_test_helper_hh

/* vim: set et sw=4 sts=4 tw=79 cino=\:0,g0,N-s,i2s,+2s ft=cpp: */
