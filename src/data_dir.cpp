 /*
 * Copyright (c) 2008-2010 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include <fstream>
#include "data_dir.hpp"

namespace engine
{
    // Load the ROOT_DIR and DATA_DIR if applicable
    void configureDirs()
    {
        std::ifstream dirs("dirs.txt");
        if (dirs.good())
        {
            std::getline(dirs, ROOT_DIR);
            std::getline(dirs, DATA_DIR);
        }
    }

    // Set up directories by hands
    // /data/ is at the same level as /bin/
    // the exe is in /bin/<os>/<build type> (3 levels deep)
    string ROOT_DIR("../../../");
    string DATA_DIR(ROOT_DIR + "data/");
}
