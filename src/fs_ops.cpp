 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <boost/filesystem.hpp>

#include "fs_ops.hpp"
#include "log.hpp"

namespace engine
{

    void makeDir(const string &dir)
    {
        if (!boost::filesystem::exists(dir))
        {
            //log("making dir " + dir);
            boost::filesystem::create_directory(dir);
        }
    }

    void removeFile(const string &file)
    {
        if (boost::filesystem::exists(file))
        {
            //log("removing file " + file);
            boost::filesystem::remove(file);
        }
    }

    void copyFile(const string &source, const string &dest)
    {
        if (!boost::filesystem::exists(dest))
        {
            //log("copying file " + source + " to " + dest);
            if (boost::filesystem::exists(source))
                boost::filesystem::copy_file(source, dest);
        }
    }
}
