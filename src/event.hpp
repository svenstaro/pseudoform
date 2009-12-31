 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef EVENT_HPP_INCLUDED
#define EVENT_HPP_INCLUDED

#include <list>

#include <boost/shared_ptr.hpp>

namespace engine
{

    class event
    {
        private:

        public:

            event()
            {
            }

            virtual ~event()
            {
            }

            virtual bool undo() { return false; }
            virtual bool redo() { return false; }

    };

    typedef boost::shared_ptr<event> eventPtr;
    typedef std::list<eventPtr> eventList;

    class eventManager
    {

        private:

            eventList _done, _undone;

        public:

            eventManager operator+=(const eventPtr &p)
            {
                _done.push_back(p);
                return *this;
            }

            void clear()
            {
                _done.clear();
                _undone.clear();
            }

            bool undo()
            {
                if (_done.empty())
                    return false;

                eventPtr p = _done.back();
                _done.pop_back();

                if (!p->undo())
                    return false;

                _undone.push_back(p);

                return true;
            }

            bool redo()
            {
                if (_undone.empty())
                    return false;

                eventPtr p = _undone.back();
                _undone.pop_back();

                if (!p->redo())
                    return false;

                _done.push_back(p);

                return true;
            }
    };

}

#endif // EVENT_HPP_INCLUDED
