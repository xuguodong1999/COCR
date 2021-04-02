/**********************************************************************
plugin.h - facilitates construction of plugin classes

Copyright (C) 2007 by Chris Morley

This file is part of the Open Babel project.
For more information, see <http://openbabel.org/>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation version 2 of the License.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.
***********************************************************************/

#ifndef OB_PLUGIN_H
#define OB_PLUGIN_H

#include <openbabel/babelconfig.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <cstring>

#include <string.h>

#ifndef OBERROR
#define OBERROR
#endif

namespace OpenBabel {
    template<class Arg1, class Arg2, class Result>
    struct binary_function {
    };
///@addtogroup plugins Plugins
///@{

/// @brief Case insensitive string comparison for PluginMapType key.
    struct OBERROR CharPtrLess : public binary_function<const char *, const char *, bool> {
        bool operator()(const char *p1, const char *p2) const { return strcasecmp(p1, p2) < 0; }
    };

/** \class OBPlugin plugin.h <openbabel/plugin.h>
    \brief Base class for all types of dynamic classes discovered at runtime
    \since version 2.2
    \sa plugin.h
 */
    class OBERROR OBPlugin {
    public:

        //Maps of thistype are used to store
        // (a)a list of the plugin types in OBPlugin, and
        // (b)a list of the sub-types in each type class derived from OBPlugin.
        typedef std::map<const char *, OBPlugin *, CharPtrLess> PluginMapType;
        typedef PluginMapType::const_iterator PluginIterator;

        ///Virtual destructor necessary for classes with virtual functions
        virtual ~OBPlugin() {};

        ///Required description of a sub-type
        virtual const char *Description() { return nullptr; };

        ///Redefined by each plugin type: "formats", "fingerprints", etc.
        virtual const char *TypeID() { return "plugins"; }

        ///Write information on a plugin class to the string txt.
        ///Return false if not written.
        ///The default implementation outputs:
        /// the ID, a tab character, and the first line of the Description.
        ///The param string can be used in derived types to provide different outputs.
        virtual bool Display(std::string &txt, const char *param, const char *ID = nullptr);

        ///Make a new instance of the class.
        ///See OpTransform, OBGroupContrib, SmartsDescriptor classes for derived versions.
        ///Usually, the first parameter is the classname, the next three are
        ///parameters(ID, filename, description) for a constructor, and the rest data.
        virtual OBPlugin *MakeInstance(const std::vector<std::string> &) { return nullptr; }

        ///Initialize the plugin.
        ///The default version does nothing.
        virtual void Init() {};

        ///Get a pointer to a plugin from its type and ID. Return NULL if not found.
        ///If Type is NULL, search all types. Not cast to Type*
        static OBPlugin *GetPlugin(const char *Type, const char *ID);

        ///Return the ID of the sub-type instance.
        const char *GetID() const { return _id; };

        ///Output a list of sub-type classes of the the type PluginID,
        ///or, if PluginID is "plugins" or empty, a list of the base types.
        ///If PluginID is not recognized or is NULL, the base types are output and the return is false.
        static bool ListAsVector(const char *PluginID, const char *param, std::vector<std::string> &vlist);

        ///As ListAsVector but sent to an ostream with a default of cout if not specified
        static void List(const char *PluginID, const char *param = nullptr, std::ostream *os = &std::cout);

        ///As ListAsVector but returns a string containing the list
        static std::string ListAsString(const char *PluginID, const char *param = nullptr);

        ///Utility function to return only the first line of a string
        static std::string FirstLine(const char *txt);

        ///Return an iterator at the start of the map of the plugin types PluginID or,
        /// if there is no such map, the end of the top level plugin map.
        static PluginIterator Begin(const char *PluginID) {
            if (!strcmp(PluginID, "plugins") || GetTypeMap(PluginID) != PluginMap())
                return GetTypeMap(PluginID).begin();
            else
                return PluginMap().end();
        }

        static PluginIterator End(const char *PluginID) {
            return GetTypeMap(PluginID).end();
        }

        ///Returns the map of the subtypes
        virtual PluginMapType &GetMap() const = 0;

        ///Load all plugins (formats, fingerprints, forcefields etc.)
        static void LoadAllPlugins();

    protected:
        ///\brief Returns a reference to the map of the plugin types.
        /// Is a function rather than a static member variable to avoid initialization problems.
        static PluginMapType &PluginMap() {
            static PluginMapType m;
            return m;
        }

        ///Keep a record if all plugins have been loaded
        static int AllPluginsLoaded;

        ///Returns the map of a particular plugin type, e.g. GetMapType("fingerprints")
        static PluginMapType &GetTypeMap(const char *PluginID);

        ///\brief Returns the type with the specified ID, or NULL if not found.
        ///Needs to be cast to the appropriate class in the calling routine.
        static OBPlugin *BaseFindType(PluginMapType &Map, const char *ID);

    protected:
        const char *_id;
    };

//Macro to be added to definition of the base class
#define MAKE_PLUGIN(BaseClass)\
protected:\
  static PluginMapType& Map() {\
    static PluginMapType m;\
    return m;\
  }\
  virtual PluginMapType& GetMap() const {\
    return Map();\
  }\
public:\
  static BaseClass*& Default() {\
    static BaseClass* d;\
    return d;\
  }\
  BaseClass(const char* ID, bool IsDefault=false) {\
    _id=ID;\
    if (ID&&*ID) {\
      if (IsDefault || Map().empty()) {\
        Default() = this;\
      }\
      if (Map().count(ID) == 0) {\
        Map()[ID] = this;\
        PluginMap()[TypeID()] = this;\
      }\
    }\
  }\
  static BaseClass* FindType(const char* ID) {\
    if (!ID || *ID==0 || *ID==' ') {\
      return Default();\
    }\
    return static_cast<BaseClass*>(BaseFindType(Map(),ID));\
  }
} // end namespce

#endif
