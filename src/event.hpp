 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#ifndef EVENT_HPP_INCLUDED
#define EVENT_HPP_INCLUDED

#include <list>

#include <boost/shared_ptr.hpp>

/**
 * @namespace engine
 * @brief Consist of functions related to the game-engine
 */
namespace engine
{
	/**
	 * @class event
	 * @brief Event in game system
	 *
	 * @remarks these events are used in editor state
	 */
    class event
    {
        private:
			// Empty

        public:
			/**
			 * Constructor
			 */
            event()
            {
            }

            /**
             * Destructor
             */
            virtual ~event()
            {
            }

            /**
             * @brief Undo implementation
             */
            virtual bool undo() { return false; }

            /**
             * @brief Redo implementation
             */
            virtual bool redo() { return false; }

    };

    /// Shared pointer for the event
    typedef boost::shared_ptr<event> eventPtr;

    /// List of events
    typedef std::list<eventPtr> eventList;

    /**
     * @class eventManager
     * @brief Event manager
     *
     * Works with game events
     */
    class eventManager
    {
        private:
			/// List of events
            eventList _done, _undone;

        public:
            /**
             * @brief Add new event
             * @param p pointer for the new event
             */
            eventManager operator+=(const eventPtr &p)
            {
                _done.push_back(p);
                return *this;
            }

            /**
             * @brief Clear all events
             */
            void clear()
            {
                _done.clear();
                _undone.clear();
            }

            /**
             * @brief Undo one event
             */
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

            /**
             * @brief Redo one event
             */
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
