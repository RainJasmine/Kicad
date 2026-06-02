/*
 * This program source code file is part of KiCad, a free EDA CAD application.
 *
 * Copyright The KiCad Developers, see AUTHORS.txt for contributors.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, you may find one here:
 * http://www.gnu.org/licenses/old-licenses/gpl-2.0.html
 * or you may search the http://www.gnu.org website for the version 2 license,
 * or you may write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA
 */

#ifndef PCB_ITEM_CONTAINERS_H_
#define PCB_ITEM_CONTAINERS_H_

// Board-level items
class FOOTPRINT;
class PCB_TRACK;
class PCB_GROUP;
class PCB_GENERATOR;
class PCB_POINT;
class PCB_MARKER;
class ZONE;

typedef std::vector<PCB_MARKER*> MARKERS;
typedef std::vector<ZONE*> ZONES;
typedef std::deque<PCB_TRACK*> TRACKS;
typedef std::deque<FOOTPRINT*> FOOTPRINTS;
typedef std::deque<PCB_GROUP*> GROUPS;
typedef std::deque<PCB_GENERATOR*> GENERATORS;
typedef std::deque<PCB_POINT*> PCB_POINTS;


// Shared with board and footprint
class BOARD_ITEM;

typedef std::deque<BOARD_ITEM*> DRAWINGS;

// Footprint-level items
class PAD;
class PCB_FIELD;

typedef std::deque<PAD*> PADS;
typedef std::deque<PCB_FIELD*> PCB_FIELDS;
//typedef std::vector<PCB_FIELD*> PCB_FIELD_VEC;

#endif // PCB_ITEM_CONTAINERS_H_
