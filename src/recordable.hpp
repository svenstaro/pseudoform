 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef RECORDABLE_HPP_INCLUDED
#define RECORDABLE_HPP_INCLUDED

#include <map>

#include <boost/foreach.hpp>

#include "types.hpp"

namespace engine
{
    /**
    * @class recordable
    * @brief Stores some events in history
    */
    template<class T>
    class recordable
    {
        protected:
            typedef T history;
            typedef std::map<real, history> historyMap;
            historyMap _history;

            /**
            * @brief Add history event into list
            * @param t event index
            * @param h history event
            */
            void _addHistory(real t, const history &h)
            {
                _history[t] = h;
            }

            /**
            * @brief Find event in history list
            * @return 0 if event found in history
            */
            real _findHistory(real t, history &before, history &after)
            {
                //typedef std::pair<real, history> val;
                typedef typename historyMap::value_type val;
                val *temp = NULL;

                BOOST_FOREACH(val &i, _history)
                {
                    if (i.first < t)
                    {
                        temp = &i;
                    }
                    else
                    {
                        if (temp)
                            before = temp->second;
                        else
                            before = i.second;
                        after = i.second;

                        if (temp)
                            return (t - temp->first) / (i.first - temp->first);
                        else
                            return 0;
                    }
                }

                if (temp)
                {
                    after = before = temp->second;
                    return 0;
                }

                return -1;
            }

            /// The _time variable is the current point in time
            /// we're at (either virtual or "for real")
            real _time, _freq, _maxHistory;

        public:
            /**
            * Constructor
            */
            recordable():
                _time(0), _freq(1.0 / 60), _maxHistory(60.0) {}
            virtual ~recordable()
            {
            }

            /**
            * @brief Max events in history
            * @param t max events number
            */
            virtual void maxHistory(engine::real t) { _maxHistory = t; }
            /**
            * @brief Get max events count in history
            */
            engine::real maxHistory() const { return _maxHistory; }

            engine::real freq() const { return _freq; }

            /**
            * @brief Interpolation method
            * @param dt delta time sine some hisory event
            */
            virtual void lerp(real dt)
            {
                _time += dt;

                // Can't go to where we didn't record
                if (_time < 0)
                    _time = 0;
            }

            const T &latestHistory() const
            {
                assert(!_history.empty());
                return _history.rbegin()->second;
            }

            /**
            * @brief Latest event in history
            */
            T &latestHistory()
            {
                assert(!_history.empty());
                return _history.rbegin()->second;
            }
    };
}

#endif // RECORDABLE_HPP_INCLUDED
