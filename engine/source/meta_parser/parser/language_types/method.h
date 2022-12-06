#pragma once

#include "type_info.h"

class Class;

class Method 
    : public TypeInfo
{
public:
    Method(
        const Cursor &cursor, 
        const Namespace &currentNamespace, 
        Class *parent = nullptr
    );

    virtual ~Method(void) { }

    bool ShouldCompile(void) const;
    
public:
    Class *m_parent;
    std::string m_name;

private:
    bool isAccessible(void) const;
};