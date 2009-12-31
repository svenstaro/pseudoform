 /*
 * Copyright (c) 2008-2009 Agop 'nullsquared' Shirinian and Sven-Hendrik 'Svenstaro' Haase
 * This file is part of Pseudoform (Pseudoform project at http://www.pseudoform.org).
 * For conditions of distribution and use, see copyright notice in COPYING
 */

#include "txml/txml.hpp"

#include "gfx/utils.hpp"

#include "skin.hpp"

#include "log.hpp"

#include "data_dir.hpp"

namespace engine
{
    namespace gui
    {

        const align_t A_TL("top_left"), A_T("top"), A_TR("top_right"),
            A_R("right"), A_BR("bottom_right"), A_B("bottom"),
            A_BL("bottom_left"), A_L("left"), A_MI("middle");

        skin::piece skin::_defaultPiece = { "", vec2(0, 0) };

//        const string ALIGNMENT_STRINGS[9] =
//        {
//            "tl", "t", "tr", "r", "br", "b", "bl", "l", "mi"
//        };

        skin::skin(const string &n):
            _name(n),
            _prefix("gui/skins/" + _name + "/" + _name)
        {
            txml::document doc;
            doc.LoadFile((engine::DATA_DIR + _prefix + ".xml").c_str());
            if (doc.Error())
            {
                engine::log("failed to load GUI skin " + _name);
                return;
            }

            txml::element *rootElem = doc.RootElement();

            for (txml::node *i = rootElem->FirstChild(); i; i = i->NextSibling())
            {
                pieceList &pl = _elements[i->Value()];

                for (txml::node *j = i->FirstChild(); j; j = j->NextSibling())
                {
                    txml::element *element = j->ToElement();
                    if (element)
                    {
                        piece &p = pl[element->Value()];
                        string tex = element->attrib<string>("image", string());

                        if (!tex.empty())
                        {
                            p.tex = "gui/skins/" + _name + "/" + tex;

                            Ogre::TexturePtr tex = gfx::getTexture(p.tex);
                            if (tex.get())
                                p.size = vec2(tex->getWidth(), tex->getHeight());
                            else
                                p.size = vec2(0, 0);
                        }
                        else
                        {
                            p.tex.clear();
                            p.size = vec2(0, 0);
                        }
                    }
                }
            }
        }

        skin::~skin()
        {
        }

        const skin::piece &skin::getPiece(const string &type, const string &align) const
        {
//            skin::piece ret;
//            if (a != A_NONE)
//                ret.tex = _prefix + type + "_" + ALIGNMENT_STRINGS[a] + ".png";
//            else
//                ret.tex = _prefix + type + ".png";
//
//            Ogre::TexturePtr tex = gfx::getTexture(ret.tex);
//            if (tex.get())
//            {
//                ret.size = vec2(tex->getWidth(), tex->getHeight());
//            }
//            else
//                ret.size = vec2(1, 1);

            elementMap::const_iterator i = _elements.find(type);

            if (i != _elements.end())
            {
                pieceList::const_iterator j = i->second.find(align);
                if (j != i->second.end())
                    return j->second;
            }

//            log("cannot find skin " + type + " :: " + align);

            return _defaultPiece;
        }

        string skin::getTexture(const string &tex) const
        {
            string ret = _prefix + "_" + tex;
//            std::cout << "trying " << ret << "\n";
            std::map<string, bool>::iterator i = _textures.find(tex);
            if (i != _textures.end())
            {
                if (i->second)
                    return ret;
                else
                    return tex;
            }

            bool has = gfx::hasTextureFile(ret);
            _textures[tex] = has;

            if (has)
                return ret;
            return tex;
        }

    }
}
