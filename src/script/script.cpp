 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <stdexcept>

#include <chaiscript/chaiscript.hpp>

#include "script.hpp"

namespace engine
{

    namespace script
    {

        script::script():
            _chai(new chaiscript::ChaiScript())
        {
        }

        script::~script()
        {
            delete _chai;
        }

        bool script::file(const engine::string &f)
        {
            try
            {
            	_chai->eval_file(f);
            }
            catch (const std::exception &e)
            {
            	_error = e.what();
            	return false;
            }
            return true;
        }

        bool script::str(const engine::string &str)
        {
            try
            {
            	_chai->eval(str);
            }
            catch (const std::exception &e)
            {
            	_error = e.what();
            	return false;
            }
            return true;
		}

        void script::clear()
        {
            delete _chai;
            _chai = new chaiscript::ChaiScript();
        }

        engine::string script::error()
        {
            engine::string ret = _error;
            if (ret.empty())
                return "no error";

            _error.clear();

            return ret;
        }

    }


}
