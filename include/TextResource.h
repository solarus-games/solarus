/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef ZSDX_TEXT_RESOURCE_H
#define ZSDX_TEXT_RESOURCE_H

#include "Common.h"

/**
 * This class provides some strings loaded from an external file containing the texts in the current language.
 * This class is used only to load simple strings. The messages displayed in the dialog box come from more
 * complex data files, see classes DialogBox and Message.
 */
class TextResource {

  private:

    // we don't need to instanciate this class
    TextResource(void);
    ~TextResource(void);

  public:

    static std::string get_string(const std::string &id);  
};

#endif

