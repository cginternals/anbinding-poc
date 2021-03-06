
#include <anbinding-aux/Meta.h>

#include <anbinding/an/bitfield.h>
#include <anbinding/an/boolean.h>
#include <anbinding/an/enum.h>
#include <anbinding/an/extension.h>

#include <anbinding/AbstractFunction.h>
#include <anbinding/Binding.h>
#include <anbinding/Version.h>

#include <anbinding-aux/ValidVersions.h>

#include "anrevision.h"
#include "Meta_Maps.h"

using namespace an;


namespace 
{


static const auto none = std::string{};
static const auto noneVersion = anbinding::Version{};
static const auto noneStringSet = std::set<std::string>{};
static const auto noneExtensions = std::set<an::AnExtension>{};


} // namespace


namespace anbinding { namespace aux
{


int Meta::anRevision()
{
    return AN_REVISION;
}

size_t Meta::alphabeticalGroupIndex(const std::string & identifier, const std::uint8_t prefixLength)
{
    auto index = static_cast<size_t>(identifier[prefixLength]); // ignore prefix ('GL_' or 'gl')

    // bold uppercase conversion -> non letters are discarded in next step
    if (index > 96)
    {
        index -= 32;
    }

    // every non upper case letter is assigned to index 0
    if (index < 65 || index > 90)
    {
        index = 64;
    }
    index -= 64;

    return index;
}

std::vector<AnBitfield> Meta::bitfields()
{
    auto bitfields = std::vector<AnBitfield>{};

    for(const auto & map : Meta_BitfieldsByStringMaps)
    {
        for (const auto & p : map)
        {
            bitfields.push_back(p.second);
        }
    }

    return bitfields;    
}

std::set<AnEnum> Meta::enums()
{
    auto enums = std::vector<AnEnum>{};

    for (const auto & p : Meta_StringsByEnum)
    {
        enums.push_back(p.first);
    }

    return std::set<AnEnum>(enums.begin(), enums.end());
}


AnExtension Meta::getExtension(const std::string & glextension)
{
    const auto index = alphabeticalGroupIndex(glextension, 4);
    const auto & map = Meta_ExtensionsByStringMaps[index];
    const auto i = map.find(glextension);

    if (i != map.cend())
    {
        return i->second;
    }

    return AnExtension::UNKNOWN;
}

std::set<AnExtension> Meta::extensions()
{
    auto extensions = std::set<AnExtension>{};

    for (const auto & m : Meta_ExtensionsByStringMaps)
    {
        for (const auto & p : m)
        {
            extensions.insert(p.second);
        }
    }

    return extensions;
}


const std::string & Meta::getString(const AnBool32 & glboolean)
{
    const auto i = Meta_StringsByBoolean.find(glboolean);

    if (i != Meta_StringsByBoolean.cend())
    {
        return i->second;
    }

    return none;
}

const std::string & Meta::getString(const AnEnum glenum)
{
    const auto i = Meta_StringsByEnum.find(glenum);

    if (i != Meta_StringsByEnum.cend())
    {
        return i->second;
    }

    return none;
}

const std::string & Meta::getString(const AnExtension glextension)
{
    const auto i = Meta_StringsByExtension.find(glextension);

    if (i != Meta_StringsByExtension.cend())
    {
        return i->second;
    }

    return none;
}

AnBitfield Meta::getBitfield(const std::string & glbitfield)
{
    const auto index = alphabeticalGroupIndex(glbitfield, 4);
    const auto & map = Meta_BitfieldsByStringMaps[index];
    const auto i = map.find(glbitfield);

    if (i != map.cend())
    {
        return i->second;
    }

    return static_cast<AnBitfield>(-1);
}

AnEnum Meta::getEnum(const std::string & glenum)
{
    const auto index = alphabeticalGroupIndex(glenum, 4);
    const auto & map = Meta_EnumsByStringMaps[index];
    const auto i = map.find(glenum);

    if (i != map.cend())
    {
        return i->second;
    }

    return static_cast<AnEnum>(static_cast<unsigned int>(-1));
}

const std::set<AnExtension> Meta::extensions(const Version & version)
{
    auto required = std::set<AnExtension>{};

    if (version.isNull())
    {
        required = Meta::extensions();
        for (const auto & p : Meta_ReqVersionsByExtension)
        {
            required.erase(p.first);
        }
    }
    else
    {
        for (const auto & p : Meta_ReqVersionsByExtension)
        {
            if (p.second == version)
            {
                required.insert(p.first);
            }
        }
    }
    return required;
}

const std::set<AnExtension> Meta::extensions(const std::string & glfunction)
{
    const auto index = alphabeticalGroupIndex(glfunction, 3);
    const auto & map = Meta_ExtensionsByFunctionStringMaps[index];
    const auto i = map.find(glfunction);

    if (i != map.cend())
    {
        return i->second;
    }

    return noneExtensions;
}

const std::set<AbstractFunction *> Meta::functions(const Version & version)
{
    const auto i = Meta_FunctionStringsByVersion.find(version);

    if (i == Meta_FunctionStringsByVersion.cend())
    {
        return std::set<AbstractFunction *>{};
    }

    const auto & functionNames = i->second;
    const auto & allFunctions = Binding::functions();

    auto requiredFunctions = std::set<AbstractFunction *>{};

    for (const auto function : allFunctions)
    {
        if (functionNames.find(function->name()) != functionNames.cend())
            requiredFunctions.insert(function);
    }

    const auto exts = extensions(version);
    for (const auto & ext : exts)
    {
        const auto f = functions(ext);
        requiredFunctions.insert(f.cbegin(), f.cend());
    }

    return requiredFunctions;
}

const std::set<AbstractFunction *> Meta::functions(const AnExtension extension)
{
    const auto i = Meta_FunctionStringsByExtension.find(extension);

    if (i == Meta_FunctionStringsByExtension.cend())
    {
        return std::set<AbstractFunction *>{};
    }

    const auto & functionNames = i->second;
    const auto & allFunctions = Binding::functions();

    auto requiredFunctions = std::set<AbstractFunction *>{};
    for (const auto function : allFunctions)
    {
        if (functionNames.find(function->name()) != functionNames.cend())
            requiredFunctions.insert(function);      
    }

    return requiredFunctions;
}

const Version & Meta::version(const AnExtension extension)
{
    const auto i = Meta_ReqVersionsByExtension.find(extension);

    if (i != Meta_ReqVersionsByExtension.cend())
    {
        return i->second;
    }

    return noneVersion;
}

const std::set<Version> & Meta::versions()
{
    return ValidVersions::versions();
}


} } // namespace anbinding::aux
